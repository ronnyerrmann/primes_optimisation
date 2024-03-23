#include <condition_variable>
#include <iostream>
#include <fstream>
#include <functional>
#include <list>
#include <math.h>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include <unistd.h>

typedef unsigned long long int llu;
//const llu max_prime(1E11);            // smaller than 18E18   18,446,744,073,709,551,616
const unsigned long int step_to_save(1E8);    // smaller than 4E9                  4,294,967,296

class Worker
{
public:
    Worker(bool start) : m_Running(start) { if (start) private_start(); }
    Worker() : m_Running(false) { }
    ~Worker() { stop(); }

    template<typename... Args>
    void push_task(Args&&... args) {
        {
            std::lock_guard<std::mutex> lk(m_Mutex);
            m_Queue.push_back(std::bind(std::forward<Args>(args)...));
        }

        m_Condition.notify_all();
    }

    void start() {
        {
            std::lock_guard<std::mutex> lk(m_Mutex);
            if (m_Running == true) return;
            m_Running = true;
        }

        private_start();
    }

    void stop() {
        {
            std::lock_guard<std::mutex> lk(m_Mutex);
            if (m_Running == false) return;
            m_Running = false;
        }

        m_Condition.notify_all();
        m_Thread.join();
    }

private:
    void private_start() {
        m_Thread = std::thread([this] {
            for (;;) {
                decltype(m_Queue) local_queue;
                {
                    std::unique_lock<std::mutex> lk(m_Mutex);
                    m_Condition.wait(lk, [&] { return !m_Queue.empty() + !m_Running; });

                    if (!m_Running)
                    {
                        for (auto& func : m_Queue)
                            func();

                        m_Queue.clear();
                        return;
                    }

                    std::swap(m_Queue, local_queue);
                }

                for (auto& func : local_queue)
                    func();
            }
        });
    }

private:
    std::condition_variable m_Condition;
    std::list<std::function<void()>> m_Queue;
    std::mutex m_Mutex;
    std::thread m_Thread;
    bool m_Running = false;
};

class Primes
{
public:
    Primes() {}

    ~Primes() {}

    void addElement(llu value, bool save=true) {
        if (!m_primes3.empty() || value >= max_lu) m_primes3.push_back(value);
        else if (!m_primes2.empty() || value >= max_u) m_primes2.push_back(value);
        else m_primes1.push_back(value);
        if (save && value >= m_next_save_value) {
            save_primes();
        }
    }

    void addVector(const std::vector<llu>& tmp_vector) {
        for (const auto& element : tmp_vector) {
            addElement(element);
        }
    }

    size_t size() const {
        return m_primes1.size() + m_primes2.size() + m_primes3.size();
    }

    llu getElement(size_t index) const {
        if (index < m_primes1.size()) { return m_primes1[index]; }
        else if (index - m_primes1.size() < m_primes2.size()) { return m_primes2[index - m_primes1.size()]; }
        else if (index - m_primes1.size() - m_primes2.size() < m_primes3.size()) { return m_primes3[index - m_primes1.size() - m_primes2.size()]; }
        std::cout << "Problem " << index << " redused " << index - m_primes1.size() - m_primes2.size() << ":" << m_primes1.size() << ":" << m_primes2.size() << ":" << m_primes3.size() << std::endl;
        throw std::out_of_range("Index out of range");
    }

    llu getBack() const {
        return getElement(size() - 1);
    }

    Primes transfer_until(llu number) {
        Primes primes_sub;
        for (size_t i = 0; i < size(); i++) {
            llu prime = getElement(i);
            // std::cout << "222 " << i << "of" << number << ":" << prime << std::endl;
            if (prime > number) { break; }
            primes_sub.addElement(prime, false);
        }
        //std::cout << "000 " << primes_sub.size() << "-" << size() << std::endl;
        return primes_sub;
    }

    Primes transfer_calc_until(llu number) {
        Primes primes_sub=transfer_until(sqrt(number + 1));
        // if (primes_sub.size() < size()) {
        //     // just to be save
        //     primes_sub.addElement(getElement(primes_sub.size()), false);
        // }
        return primes_sub;
    }

    void read_primes() {
        std::ifstream file(m_primes_filename);

        if (!file.is_open()) {
            std::cout << "No stored primes yet." << std::endl;
            addElement(2);
            addElement(3);
            addElement(5);
            return;
        }

        std::cout << "Reading primes ... ";
        std::string line;
        while (std::getline(file, line)) {
            try {
                llu num = std::stoull(line);
                addElement(num, false); 
            } catch (const std::invalid_argument& e) {
                std::cerr << "Invalid integer: " << line << std::endl;
            } catch (const std::out_of_range& e) {
                std::cerr << "Integer out of range: " << line << std::endl;
            }
        }

        file.close();
        std::cout << "Read " << size() << " Primes until " << getBack() << std::endl;
        m_next_save_index = size();
        m_next_save_value = getBack() + step_to_save;
    }

    void save_primes() {
        std::ofstream file(m_primes_filename, std::ios::app);
        if (!file.is_open()) {
            std::cerr << "Error opening file!" << std::endl;
            return;
        }

        for (size_t i = m_next_save_index; i < size(); i++) {
            // to_string + "\n" is much faster then getElement(i) << endl or std::to_string(getElement(i)) << endl
            file << std::to_string(getElement(i)) + "\n";
        }

        file.close();
        std::cout << "Saved " << size() - m_next_save_index << " Primes between " << getElement(m_next_save_index) << " and " << getBack();
        std::cout << " (at index " << m_next_save_index << " and " << size() - 1 << " (starting from 0))" << std::endl;
        m_next_save_index = size();
        m_next_save_value = getBack() + step_to_save;
    }


private:
    // vectors have the disadvantage that things will break as soon as the vector is read in one thread and modified in another
    std::vector<unsigned> m_primes1;            // only 6542 primes
    std::vector<unsigned long int> m_primes2;   // 203273679 (2E8) primes
    std::vector<llu> m_primes3;
    const llu max_lu = pow(2, 32);
    const llu max_u = pow(2, 16);
    const std::string m_primes_filename="primes.txt";
    size_t m_next_save_index = 0;
    llu m_next_save_value = step_to_save;

};

void calc_primes(Primes primes_local, llu start, unsigned long int range, std::vector<llu>& primes_tmp, unsigned& done) {
    llu max_prime = start + range;
    std::cout << start << "-" << max_prime << "-" << primes_local.size() << ":" << primes_local.getBack() << std::endl;
    llu max_test_prime = sqrt(max_prime) + 1;
    size_t primes_size = primes_local.size();
    //bool is_prime;    // definition inside or outside of the loop doesn't matter
    //llu prime;
    for (llu i = start+2; i <= max_prime; i+=2) {
        bool is_prime = true;
        for (size_t j = 0; j < primes_size; j++) {
            llu prime = primes_local.getElement(j);
            if (i%prime == 0) {
                is_prime = false;
                break;
            }
            if (prime > max_test_prime) {
                break;
            }
        }
        /* This is quite a bit slower
        unsigned long int j = 0;
        while (++j < primes_size && primes_local.getElement(j) < max_test_prime) {     // start with the second prime (3)
            if (i%primes_local.getElement(j) == 0) {
                is_prime = false;
                break;
            }
        } */
        //std::cout << start << "-" << i << "-" << is_prime << std::endl;
        if (is_prime) {
            primes_tmp.push_back(i);
        }
    }
    //std::cout << start << "+" << max_prime << ":" << primes_tmp.size() << "(" << primes_tmp[0] << "-" << primes_tmp.back() << ")" << std::endl;
    done = 1;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cout << "Please start with the maximum potential prime to be calculated and the steps to split into" << std::endl;
        return 1;
    }
    llu max_prime = (llu)std::stold(argv[1]);
    llu step = (llu)std::stold(argv[2]);

    Primes primes;
    const unsigned int num_threads = std::thread::hardware_concurrency(); // Get the number of CPU threads
    
    primes.read_primes();
    if (primes.size() > 0 && primes.getBack() >= max_prime) {
        std::cout << "Nothing to do";
        return 0;
    }

    // Prepare enough primes to run in parallel
    while (primes.getBack() < (llu)sqrt(10*num_threads*step)) {
        unsigned done(false);
        std::vector<llu> primes_in_progress;
        llu last = primes.getBack();
        calc_primes(primes, last, (primes.getBack() > (int)sqrt(step)) ? step : last*last, primes_in_progress, done);
        primes.addVector(primes_in_progress);
    }

    Worker workers[num_threads];
    std::vector<std::vector<llu>> primes_in_progress(num_threads);
    std::vector<unsigned> threads_finished(num_threads);    // can't use bool because of std::ref
    std::vector<int> work_in_progress(num_threads);
    llu start = primes.getBack();

    for (auto& worker : workers){
        worker.start();
    }

    for (unsigned int i = 0; i < num_threads; ++i) {
        // don't run the result code before starting the threads
        threads_finished[i] = 2;
    }
    unsigned long int next_index_to_start = 0;
    unsigned long int next_index_to_insert = 0;
    std::vector<std::vector<llu>> primes_to_insert;
    std::vector<unsigned long int> primes_to_insert_index;
    
    while (next_index_to_insert < next_index_to_start || next_index_to_start == 0) {
        //std::this_thread::sleep_for(100);
        sleep(0.1);
        for (unsigned int i = 0; i < num_threads; ++i) {
            //std::cout << i << " ";
            bool start_work = false;
            if (threads_finished[i] == 1) {
                //std::cout << "thread " << i << " finished: " << std::endl;
                if (work_in_progress[i] == next_index_to_insert) {
                    // This was the next bit of work, add to primes directly
                    //std::cout << "transfer " << primes_in_progress[i].size() << " entries to " << primes.size() << " entries" << std::endl;
                    primes.addVector(primes_in_progress[i]);
                    next_index_to_insert++;
                    // if (!primes_to_insert.empty()) {
                    //     std::cout << "next stored:" << primes_to_insert_index[0] << "-" << next_index_to_insert << std::endl;
                    // }
                    while (!primes_to_insert.empty() && primes_to_insert_index[0] == next_index_to_insert) {
                        // Is the next set of primes already available to insert
                        //std::cout << "transfer " << primes_to_insert[0].size() << " entries to " << primes.size() << " entries" << std::endl;
                        primes.addVector(primes_to_insert[0]);
                        primes_to_insert.erase(primes_to_insert.begin());
                        primes_to_insert_index.erase(primes_to_insert_index.begin());
                        next_index_to_insert++;
                    }
                }
                else {  // We can't add these primes yet, as previous threads have not yet finished -> store them for now
                    bool added = false;
                    for (unsigned int j = 0; j < primes_to_insert_index.size(); ++j) {
                        if (work_in_progress[i] < primes_to_insert_index[j]) {
                            primes_to_insert_index.insert(primes_to_insert_index.begin() + j, work_in_progress[i]);
                            primes_to_insert.insert(primes_to_insert.begin() + j, primes_in_progress[i]);
                            added = true;
                            break;
                        }
                    }
                    if (!added) {
                        primes_to_insert_index.insert(primes_to_insert_index.end(), work_in_progress[i]);
                        primes_to_insert.insert(primes_to_insert.end(), primes_in_progress[i]);
                    }
                    // for (auto& index : primes_to_insert_index) {
                    //     std::cout << index << ".";
                    // }
                    // std::cout << std::endl;
                }
                start_work = true;
            } else if (threads_finished[i] == 2) {
                start_work = true;
            }
            if (start_work) {
                threads_finished[i] = 0;
                primes_in_progress[i].clear();
                //threads[i] = std::thread(calc_primes, std::ref(primes), start, step, std::ref(primes_in_progress.back()), std::ref(threads_finished[i]));
                if (start < max_prime){
                    Primes primes_part = primes.transfer_calc_until(start + step);
                    workers[i].push_task(calc_primes, primes_part, start, step, std::ref(primes_in_progress[i]), std::ref(threads_finished[i]));
                    work_in_progress[i] = next_index_to_start++;
                    start += step;
                } else {
                    std::cout << "thread " << i << " not started" << std::endl;
                }
            }
        }
    }

    primes.save_primes();
    std::cout << "max: " << primes.getBack() << ", entries: " << primes.size() << ", cores:" << num_threads << std::endl;

    return 0; // Return success
}

/*
g++ -o primes_cpp_v4 primes_cpp_v4.cpp && rm primes.txt ; time ./primes_cpp_v4 10E6 5E5
real	0m1.230s 0m1.200s 0m1.285s
user	0m6.443s 0m6.456s 0m6.540s
-> quicker due to multithreading? Step doesn't change result much. Using only primes3 doesn't change result
*/

