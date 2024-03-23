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
const llu step(5E5);    // smaller than 4E9                  4,294,967,296
const std::string primes_filename_base="primes.txt";

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

class PrimesBase
{
public:
    PrimesBase() {}

    ~PrimesBase() {}

    void addElement(llu value) {
        if (!m_primes3.empty() || value >= max_lu) m_primes3.push_back(value);
        else m_primes2.push_back(value);
    }

    void addVector(const std::vector<llu>& tmp_vector) {
        for (const auto& element : tmp_vector) {
            addElement(element);
        }
    }

    size_t size() const {
        return m_primes2.size() + m_primes3.size();
    }

    llu getElement(size_t index) const {
        if (index < m_primes2.size()) { return (llu)m_primes2[index]; }
        else {
            size_t index_sub = index - m_primes2.size();
            if (index_sub < m_primes3.size()) { return m_primes3[index_sub]; }
        }
        std::cout << "Problem " << index << " redused " << index - m_primes2.size() << ":" << m_primes2.size() << ":" << m_primes3.size() << std::endl;
        throw std::out_of_range("Index out of range");
    }

    llu getBack() {
        return getElement(size() - 1);
    }

    llu getMaxPrime() {
        return m_max_prime;
    }

    size_t getNumberPrimes() {
        return m_number_primes_total;
    }

    void clear() {
        m_primes2.clear();
        m_primes3.clear();
    }

    const llu max_lu = pow(2, 32);

private:
    // vectors have the disadvantage that things will break as soon as the vector is read in one thread and modified in another
    // std::vector<unsigned> m_primes1;            // only 6542 primes
    std::vector<unsigned long int> m_primes2;   // 203273679 (2E8) primes
    std::vector<llu> m_primes3;

protected:
    llu m_max_prime;
    size_t m_number_primes_total;
    const llu max_lu_half = pow(2, 31);

};

class PrimesStore: public PrimesBase
{
public:
    PrimesStore(llu max_prime, size_t number_primes_total) {
        m_max_prime = max_prime;
        m_number_primes_total = number_primes_total;
    }

    ~PrimesStore() {}

    void addVector(const std::vector<llu>& tmp_vector) {
        PrimesBase::addVector(tmp_vector);
        save_primes();
    }

    void save_primes() {
        std::ofstream file(primes_filename_base, std::ios::app);
        if (!file.is_open()) {
            std::cerr << "Error opening file!" << std::endl;
            return;
        }

        for (size_t i = 0; i < size(); i++) {
            // to_string + "\n" is much faster then getElement(i) << endl or std::to_string(getElement(i)) << endl
            file << std::to_string(getElement(i)) + "\n";
        }
        m_max_prime = getBack();
        m_number_primes_total += size();

        file.close();
        std::cout << "Saved " << size() << " Primes between " << getElement(0) << " and " << m_max_prime << std::endl;
        //std::cout << " (at index " << m_next_save_index << " and " << size() - 1 << " (starting from 0))" << std::endl;
        clear();
    }

private:

};

class PrimesCalc: public PrimesBase
{
public:
    PrimesCalc() {
        m_max_prime = 0;
    }

    ~PrimesCalc() {}

    void read_primes(llu& store_until) {
        std::cout << "Reading primes ... " << std::endl;
        std::ifstream file(primes_filename_base);

        if (!file.is_open()) {
            std::cout << "No stored primes yet." << std::endl;
            addElement(2);
            addElement(3);
            addElement(5);
            return;
        }

        std::string line;
        size_t index = 0;
        size_t start_adding_from_index = size();
        while (std::getline(file, line)) {
            try {
                llu num = std::stoull(line);
                if (index >= start_adding_from_index && num < store_until) {
                    addElement(num);
                }
                index++;
                m_max_prime = num;
            } catch (const std::invalid_argument& e) {
                std::cerr << "Invalid integer: " << line << std::endl;
            } catch (const std::out_of_range& e) {
                std::cerr << "Integer out of range: " << line << std::endl;
            }
        }
        m_number_primes_total = index;

        file.close();
        if (size() - start_adding_from_index > 0) {
            std::cout << "Read " << size() - start_adding_from_index << " Primes until " << getBack();
            std::cout << " ,largest prime in file: " << getMaxPrime() << " at " << getNumberPrimes() << std::endl;
        } else {
            std::cout << " nothing required" << std::endl;
        }
    }

    PrimesCalc transfer_until(llu number) {
        PrimesCalc primes_sub;
        if (number > getBack()) {
            llu temp = number >= max_lu_half ? number : number * 2;     // read a bit ahead, except close to hearching the limit of llu
            read_primes(temp);
        }
        for (size_t i = 0; i < size(); i++) {
            llu prime = getElement(i);
            // std::cout << "222 " << i << "of" << number << ":" << prime << std::endl;
            if (prime > number) { break; }
            primes_sub.addElement(prime);
        }
        //std::cout << "000 " << primes_sub.size() << "-" << size() << std::endl;
        return primes_sub;
    }

    PrimesCalc transfer_calc_until(llu number) {
        PrimesCalc primes_sub=transfer_until((llu)sqrt(number) + 1);
        // if (primes_sub.size() < size()) {
        //     // just to be save
        //     primes_sub.addElement(getElement(primes_sub.size()), false);
        // }
        return primes_sub;
    }

private:

};

void calc_primes(PrimesCalc primes_local, llu start, llu range, std::vector<llu>& primes_tmp, unsigned& done) {
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
    if (argc < 2) {
        std::cout << "Please start with the maximum potential prime to be calculated" << std::endl;
        return 1;
    }
    llu max_prime = (llu)std::stold(argv[1]);
    
    const unsigned int num_threads = std::thread::hardware_concurrency(); // Get the number of CPU threads
    llu start_value = 1E6; //sqrt(10 * num_threads * step);        // some starting value

    PrimesCalc primes;
    primes.read_primes(start_value);
    if (primes.size() > 0 && primes.getMaxPrime() >= max_prime) {
        std::cout << "Nothing to do";
        return 0;
    }
    PrimesStore primesSave(primes.getMaxPrime(), primes.getNumberPrimes());
    if (primes.getBack() == 5) {        // that's bit of a hack
        primesSave.addVector(std::vector<llu> {2,3,5});
    }

    llu start = primesSave.getMaxPrime();
    Worker workers[num_threads];
    std::vector<std::vector<llu>> primes_in_progress(num_threads);      // has the result vector for each thread
    std::vector<unsigned> threads_finished(num_threads);    // can't use bool because of std::ref
    std::vector<int> work_in_progress(num_threads);         // store the counter of work the thread is doing
    size_t next_index_to_start = 0;
    size_t next_index_to_insert = 0;
    std::vector<std::vector<llu>> primes_to_insert;         // store the work temporarily
    std::vector<size_t> primes_to_insert_index;             // store the work temporarily
    bool only_single_thread = false;

    for (auto& worker : workers){
        worker.start();
    }
    for (unsigned int i = 0; i < num_threads; ++i) {
        // don't run the result code before starting the threads
        threads_finished[i] = 2;
    }
    
    while (next_index_to_insert < next_index_to_start || next_index_to_start == 0) {
        //std::this_thread::sleep_for(100);
        sleep(0.1);
        for (unsigned int i = 0; i < num_threads; ++i) {
            bool start_work = false;
            if (threads_finished[i] == 1) {
                only_single_thread = false;
                //std::cout << "thread " << i << " finished: " << std::endl;
                if (work_in_progress[i] == next_index_to_insert) {
                    // This was the next bit of work, add to primes directly
                    //std::cout << "transfer " << primes_in_progress[i].size() << " entries to " << primes.size() << " entries" << std::endl;
                    primesSave.addVector(primes_in_progress[i]);
                    next_index_to_insert++;
                    // if (!primes_to_insert.empty()) {
                    //     std::cout << "next stored:" << primes_to_insert_index[0] << "-" << next_index_to_insert << std::endl;
                    // }
                    while (!primes_to_insert.empty() && primes_to_insert_index[0] == next_index_to_insert) {
                        // Is the next set of primes already available to insert
                        //std::cout << "transfer " << primes_to_insert[0].size() << " entries to " << primes.size() << " entries" << std::endl;
                        primesSave.addVector(primes_to_insert[0]);
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
                primes_in_progress[i].clear();
                start_work = true;
            } else if (threads_finished[i] == 2) {
                start_work = true;
            }
            if (start_work) {
                // Check how far we can calculate
                PrimesCalc primes_part = primes.transfer_calc_until(start + step);
                llu temp_max_prime = primes.getMaxPrime();      // getMaxPrime is not available on primes_part (maybe primes_part should be the base class?) and is only updated after transfer_*until
                llu temp_max_to_calc = temp_max_prime >= primes.max_lu ? (llu)std::numeric_limits<llu>::max() : (llu)pow(temp_max_prime, 2);    // avoid llu overrun
                llu temp_step = temp_max_to_calc > start ? temp_max_to_calc - start : (llu)0;                    // otherwise that could get nasty
                //std::cout << "temp_step " << temp_step << " " << temp_max_to_calc << " " << temp_max_prime << " " << (temp_max_prime >= primes.max_lu) << std::endl;
                if (step < temp_step) {     // enough primes are available
                    temp_step = step;
                } else if (i == 0) {        // some primes need to be gathered before all threads can be used
                    only_single_thread = true;
                    if (temp_step % 2 != 0) {
                        temp_step--;        // make sure it is an even number
                    }
                } else {
                    start_work = false;
                }
                //threads[i] = std::thread(calc_primes, std::ref(primes), start, step, std::ref(primes_in_progress.back()), std::ref(threads_finished[i]));
                if (start < max_prime){
                    if (start_work) {
                        threads_finished[i] = 0;
                        workers[i].push_task(calc_primes, primes_part, start, temp_step, std::ref(primes_in_progress[i]), std::ref(threads_finished[i]));
                        work_in_progress[i] = next_index_to_start++;
                        start += temp_step;
                    }
                } else {
                    threads_finished[i] = 0;
                    std::cout << "thread " << i << " not started" << std::endl;
                }
            }
            if (only_single_thread) {
                break;
            }
        }
    }

    std::cout << "max: " << primesSave.getMaxPrime() << ", entries: " << primesSave.getNumberPrimes() << ", cores:" << num_threads << std::endl;

    return 0; // Return success
}


/*
g++ -o primes_cpp_v5 primes_cpp_v5.cpp && rm primes.txt ; time ./primes_cpp_v5 10E6 5E5
real	0m1.095s 0m1.158s 0m1.165s
user	0m6.612s 0m6.638s 0m6.725s
-> quicker due to multithreading
*/

