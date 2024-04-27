#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <iomanip>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <functional>
#include <list>
#include <math.h>
#include <mutex>
#include <regex>
#include <sstream>
#include <stdint.h>
#include <string>
#include <thread>
#include <unistd.h>
#include <utility>
#include <vector>

// g++ -Wall -g -o prime prime_main.cpp

typedef unsigned long long int llu;
// const llu max_prime(1E11);            // smaller than 18E18   18,446,744,073,709,551,616
// const llu step(1E8);    // smaller than 4E9                  4,294,967,296
// const std::string primes_filename_base="/mnt/d/Backup_ronny/primes.txt";

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
    PrimesBase(std::string primes_filename_base): m_primes_filename_base(primes_filename_base) {
        //m_primes_filename_base = primes_filename_base;
    }

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
        if (size() == 0) {
            return 0;
        }
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
    // const llu max_lu = (llu)std::numeric_limits<unsigned long int>::max();
    std::string m_primes_filename_base;

private:
    // vectors have the disadvantage that things will break as soon as the vector is read in one thread and modified in another
    // std::vector<unsigned> m_primes1;            // only 6542 primes

protected:
    bool useBinaryFile()
    {
        return (m_primes_filename_base.substr(m_primes_filename_base.size() - 4) == ".bin");
    }

    std::vector<unsigned long int> m_primes2;   // 203273679 (2E8) primes, // only using llu is not faster (as it probably stores 64 bit integers anyway)
    std::vector<llu> m_primes3;
    llu m_max_prime;
    size_t m_number_primes_total;
    const llu max_lu_half = pow(2, 31);

};

class PrimesSave: public PrimesBase
{
// This shouldn't use most of PrimeBase funktionality except useBinaryFile and m_primes_filename_base
public:
    PrimesSave(llu max_prime, size_t number_primes_total, std::string primes_filename_base): PrimesBase(primes_filename_base) {
        m_max_prime = max_prime;
        m_number_primes_total = number_primes_total;
    }

    ~PrimesSave() {}

    void addVector(std::vector<llu>& tmp_vector) {
        if (tmp_vector.size() == 0) {
            return;
        }
        save_prepare_primes(tmp_vector);
    }

private:
    std::string add_number_to_filename(int digits) {
        const size_t position = m_primes_filename_base.length() > 3 ? m_primes_filename_base.length() - 4 : m_primes_filename_base.length();
        std::ostringstream file_extension;
        file_extension << "_to1E" << std::setw(3) << std::setfill('0') << digits + 1;
        std::string file_name = m_primes_filename_base;
        file_name.insert(position, file_extension.str());
        return file_name;
    }

    void save_prepare_primes(std::vector<llu>& primes) {
        unsigned digits = (unsigned)int(log10(primes[0]));
        digits = digits > 8 ? digits : 8;       // start with numbers below 1E9
        llu next_number = (llu)pow(10, digits + 1);
        if (primes.back() < next_number) {
            save_primes(primes, add_number_to_filename(digits), digits);
            return;
        }
        std::vector<llu> primes_tmp;
        for (size_t i = 0; i < primes.size(); i++) {
            if (primes[i] > next_number) {
                save_primes(primes_tmp, add_number_to_filename(digits), digits);
                primes_tmp.clear();
                digits++;
                next_number = (llu)pow(10, digits + 1);
            }
            primes_tmp.push_back(primes[i]);
        }
        save_primes(primes_tmp, add_number_to_filename(digits), digits);
    }

    void save_primes(std::vector<llu>& primes, std::string file_name, unsigned digits) {
        std::fstream file;
        if (useBinaryFile()) {
            file.open(file_name, std::fstream::binary | std::ios::in | std::ios::out | std::ios::app);
        } else {
            file.open(file_name, std::ios::out | std::ios::app);
        }
        if (!file.is_open()) {
            std::cerr << "Error opening file: " << file_name << std::endl;
            return;
        }
        uint_least16_t max_diff = 0;
        if (useBinaryFile()) {
            // go to the end of the file
            file.seekg(0, std::ios::end);
            if (digits <= 8) {      // save numbers - important for first file to avoid issue with (3-2)/2
                // until 4E9 the primes can be safed as 32 bit (long unsigned). However, as there are only 5E7 numbers to 1E9, it only saves about 750MB
                // old: file.write(reinterpret_cast<const char *>(&primes[0]), primes.size()*sizeof(llu));   // save the whole array in 64bit
                for (size_t i = 0; i < primes.size(); i++) {
                    uint_least32_t num = (uint_least32_t)primes[i];
                    file.write(reinterpret_cast<char*>(&num), sizeof(num));
                }
            } else {    
                // until 1E10 reach max_diff of 177, 1E11 of 232, 1E12 of 270 - 8bit would be alright for E10 and E11 but would only save 3.8GB of storage, afterwards needs uint_least16_t
                // the following idea doesn't work as each 8 bit would only alllow for another 255 - better: be flexible use smallest possible type and if it's used completely, then add another 8bit - disadvantage: need to read all lines to get number of entries (or store  number entries, max prime, start prime at beginning)
                if (file.tellg() == 0) {
                    // if the file is empty
                    uint_least64_t num = primes[0];
                    // std::cout << "220 " << num << std::endl;
                    file.write(reinterpret_cast<char*>(&num), sizeof(num));
                } else {
                    uint_least16_t diff = (uint_least16_t)((uint_least32_t)(primes[0] - getMaxPrime())/2);
                    // std::cout << "221 " << diff << " " << getMaxPrime() << " " << primes[0] << std::endl;
                    file.write(reinterpret_cast<char*>(&diff), sizeof(diff));
                }
                for (size_t i = 1; i < primes.size(); i++) {
                    uint_least16_t diff = (uint_least16_t)((uint_least32_t)(primes[i] - primes[i-1])/2);
                    // std::cout << "222 " << diff << " " << primes[i] << " " << primes[i-1] << std::endl;
                    file.write(reinterpret_cast<char*>(&diff), sizeof(diff));
                    if (diff > max_diff) {
                        max_diff = diff;
                    }
                }
            }
        } else {
            for (size_t i = 0; i < primes.size(); i++) {
                // to_string + "\n" is much faster then getElement(i) << endl or std::to_string(getElement(i)) << endl
                file << std::to_string(primes[i]) + "\n";
            }
        }
        m_max_prime = primes.back();
        m_number_primes_total +=  primes.size();

        file.close();
        std::cout << "Saved " << primes.size() << " Primes between " << primes[0] << " and " << m_max_prime << " into " << file_name;
        if (max_diff) {
            std::cout << " Max diff/2 between consecutive numbers: " << max_diff;
        }
        if (max_diff >= std::numeric_limits<uint_least16_t>::max() - 1000) {
            std::cout << " save_primes/read_primes need to be updated soon, coming close to the limit of the data type " << max_diff;
        }
        std::cout << std::endl;
    }
};


class PrimesCalc: public PrimesBase
{
public:
    PrimesCalc(): PrimesBase("") {
        m_max_prime = 0;
    }

    ~PrimesCalc() {}

    void calc_primes(llu start, llu range, std::vector<llu>& primes_tmp, unsigned& done) {
        auto start_time = std::chrono::high_resolution_clock::now();
        llu max_prime = start + range;
        std::cout << start << " - " << max_prime << " : using " << size() << " old primes until " << getBack() << std::endl;
        //llu max_test_prime = sqrt(max_prime) + 1;   // calculating it here (and do a few too many tests for the first primes) then for each prime is about 10% faster for 3E9 5E7
        //bool is_prime;    // definition inside or outside of the loop doesn't matter
        for (llu test_number = start+2; test_number <= max_prime; test_number += 2) {
            bool is_prime = true;
            // bool cont_primes3 = true;
            // llu max_test_prime = sqrt(test_number) + 1;     // 10% slower
            // for (size_t j = 0; j < primes_size; j++) {   // 25% slower than using m_primes2/3
            //     llu prime = getElement(j);
            // for (llu prime : m_primes2) {    // twice as slow as getElement
            for (size_t j = 1; j < m_primes2.size(); j++) { // j==0: 2
                // llu prime = m_primes2[j];    // doesn't make a difference
                if (test_number % m_primes2[j] == 0) {
                    is_prime = false;
                    break;
                }
                // if (prime > max_test_prime) { // should never be hit as the primes list is prepared beforehand, saves about 8%
                //     cont_primes3 = false;
                //     break;
                // }
            }
            if (!is_prime) {    // not needed anymore && cont_primes3) {
                for (size_t j = 0; j < m_primes3.size(); j++) {
                    // llu prime = m_primes3[j];
                    if (test_number % m_primes3[j] == 0) {
                        is_prime = false;
                        break;
                    }
                    // if (prime > max_test_prime) { // should never be hit as the primes list is prepared beforehand
                    //     break;
                    // }
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
            if (is_prime) {
                primes_tmp.push_back(test_number);
            }
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start_time);
        std::cout << duration.count() << "s " << " - " << max_prime << " : " << primes_tmp.size() << " primes found between " << primes_tmp[0] << " and " << primes_tmp.back() << std::endl;
        done = 1;
    }
};


class PrimesStore: public PrimesBase
{
public:
    typedef bool (PrimesStore::*ReadPrimesFunc)(const llu&, const size_t&);

    PrimesStore(std::string primes_filename_base): PrimesBase(primes_filename_base) {
        m_max_prime = 0;
        m_number_primes_total = 0;
    }

    ~PrimesStore() {}

    void read_primes(llu store_until) {
        m_read_store_until = store_until;
        read_prepare_primes(&PrimesStore::read_primes_action);
    }

    void ouput_primes(llu read_min, llu read_max, size_t read_min_index, size_t read_max_index) {
        m_output_read_max = (read_max == 0) ? (llu)std::numeric_limits<llu>::max() : read_max;
        m_read_store_until = m_output_read_max;
        m_output_read_max_index = (read_max_index == 0) ? (size_t)std::numeric_limits<size_t>::max() : read_max_index;
        m_output_read_min = read_min;
        m_output_read_min_index = read_min_index;
        read_prepare_primes(&PrimesStore::output_primes_action);
    }

    PrimesCalc transfer_until(llu number) {
        PrimesCalc primes_sub;
        if (number > getBack()) {
            llu temp = number >= max_lu_half ? number : number * 2;     // read a bit ahead, except close to reaching the limit of llu
            read_primes(temp);
        }
        for (size_t i = 0; i < size(); i++) {
            llu prime = getElement(i);
            if (prime > number) { break; }
            primes_sub.addElement(prime);
        }
        return primes_sub;
    }

    PrimesCalc transfer_calc_until(llu number) {
        PrimesCalc primes_sub=transfer_until((llu)sqrt(number) + 1);
        return primes_sub;
    }

private:
    void read_prepare_primes(ReadPrimesFunc read_primes_activity) {
        size_t lastSlashPos = m_primes_filename_base.find_last_of('/');
        std::string path = ".";
        std::string filename_search = m_primes_filename_base;
        if (lastSlashPos != std::string::npos) {
            path = m_primes_filename_base.substr(0, lastSlashPos);
            filename_search = m_primes_filename_base.substr(lastSlashPos + 1);
        }
        const size_t position = filename_search.length() > 3 ? filename_search.length() - 4 : filename_search.length();
        filename_search.insert(position, "_to(\\d+E\\d+)");
        const std::regex pattern(filename_search);

        std::vector<std::pair<double, std::filesystem::path>> files;

        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            std::smatch matches;
            std::string filename = entry.path().filename().string();

            if (std::regex_match(filename, matches, pattern)) {
                double number = std::stod(matches[1].str());
                files.emplace_back(number, entry.path());
            }
        }

        if (files.size() == 0) {
            std::cout << "No stored primes yet." << std::endl;
            addElement(2);
            addElement(3);
            addElement(5);
            return;
        }

        // Sort the files based on the scientific number in the filename
        auto comparator = [](const auto& a, const auto& b) {
            return a.first < b.first;
        };
        std::sort(files.begin(), files.end(), comparator);

        size_t index_total = 0;
        bool read_next_file = true;
        for (size_t i = 0; i < files.size(); i++) {
            bool read = false;
            if (files[i].first <= m_read_store_until || i == files.size() -1) {
                // reading the last file, just to get the last number takes over 10 minutes, a solution could be to save last number and inxed in a txt file (delete before saving primes)
                read = true;
            } else if (read_next_file) {
                // read one more file as otherwise a store_until == 9E5 won't read the 1E6 file, that's the issue of using _to instead of _from
                read = true;
                read_next_file = false;
            }
            read_primes_file(files[i].second, index_total, read, read_primes_activity, files[i].first);
        }
    }

    bool read_primes_action(const llu& num, const size_t& index) {
        if (index >= m_read_start_adding_from_index && num < m_read_store_until) {
            addElement(num);
        }
        return false;
    }

    bool output_primes_action(const llu& num, const size_t& index) {
        if (index > m_output_read_max_index || num > m_output_read_max) {
            return true;    // break the reading
        }
        if (index >= m_output_read_min_index && num >= m_output_read_min) {
            std::cout << num << std::endl;
        }
        return false;
    }

    void read_primes_file(std::filesystem::path filename, size_t& index, bool read, ReadPrimesFunc read_primes_activity, double numbers_until) {
        auto start_time = std::chrono::high_resolution_clock::now();
        std::cout << "Reading primes from " << filename << " ... " << std::endl;
        std::ifstream file;
        if (useBinaryFile()) {
            file.open(filename, std::ofstream::binary);
        } else {
            file.open(filename);
        }

        m_read_start_adding_from_index = size();
        if (useBinaryFile()) {
            if (read) {
                if (numbers_until <= 1E9) {
                    uint_least32_t num;
                    while (file.read(reinterpret_cast<char*> (&num), sizeof(uint_least32_t))) {
                        //std::cout << "111 " << num << std::endl;
                        if ((this->*read_primes_activity)(num, index)) {
                            break;
                        }
                        index++;
                        m_max_prime = num;
                    }
                } else {
                    llu prime;
                    file.read(reinterpret_cast<char*> (&prime), sizeof(uint_least64_t));
                    (this->*read_primes_activity)(prime, index);
                    // std::cout << "110 " << prime << " " << index << std::endl;
                    m_max_prime = prime;
                    index++;
                    uint_least16_t num;
                    while (file.read(reinterpret_cast<char*> (&num), sizeof(uint_least16_t))) {
                        prime += 2 * (llu)num;
                        // if (index < 50847600) std::cout << "111 " << num << " " << prime << " " << index << std::endl;
                        if ((this->*read_primes_activity)(prime, index)) {
                            break;
                        }
                        index++;
                        m_max_prime = prime;
                    }

                }
                // while (file.read(reinterpret_cast<char*> (&num), sizeof(llu))) {
                //     //std::cout << "111 " << num << std::endl;
                //     if ((this->*read_primes_activity)(num, index)) {
                //         break;
                //     }
                //     index++;
                //     m_max_prime = num;
                // }
            } else {
                file.seekg(0, std::ios::end);
                std::streampos fileSize = file.tellg();
                file.seekg(0, std::ios::beg);
                if (numbers_until <= 1E9) {
                    index += fileSize / sizeof(uint_least32_t);
                } else {
                    index ++;   // first full entry (llu)
                    fileSize -= sizeof(llu);
                    index += fileSize / sizeof(uint_least16_t);
                }
            }
        } else {
            std::string line;
            while (std::getline(file, line)) {
                if (read) {
                    try {
                        llu num = std::stoull(line);
                        if ((this->*read_primes_activity)(num, index)) {
                            break;
                        }
                        m_max_prime = num;
                    } catch (const std::invalid_argument& e) {
                        std::cerr << "Invalid integer: " << line << std::endl;
                    } catch (const std::out_of_range& e) {
                        std::cerr << "Integer out of range: " << line << std::endl;
                    }
                }
                index++;
            }
        }
        m_number_primes_total = index;

        file.close();
        if (m_output_read_max_index == 0) {     // not when outputting primes
            if (size() - m_read_start_adding_from_index > 0) {
                std::cout << "Read " << size() - m_read_start_adding_from_index << " Primes until " << getBack();
            } else {
                std::cout << " nothing required";
            }
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start_time);
            std::cout << ", largest prime in file: " << getMaxPrime() << " at total number " << getNumberPrimes() << " Took " << duration.count() << "s " << std::endl;
        }
    }

    size_t m_read_start_adding_from_index;
    llu m_read_store_until;
    llu m_output_read_min;
    llu m_output_read_max;
    size_t m_output_read_min_index;
    size_t m_output_read_max_index = 0;


};


void calc_primes(PrimesCalc primes, llu start, llu range, std::vector<llu>& primes_tmp, unsigned& done) {
    primes.calc_primes(start, range, primes_tmp, done);
}


int main(int argc, char *argv[]) {
    std::string primes_filename_base = "my_primes.txt";
    if (argc < 3) {
        std::cout << "Please start with the maximum potential prime to be calculated and the number of numbers to test per thread call, e.g. 1E10 1E8" << std::endl;
        std::cout << "Optional add the base filename to which the numbers should be saved and the number of threads to use." << std::endl;
        std::cout << "Alternatively, use parameters output <num1> <num2> to get all primes between two numbers (including). Optionally give base filename." << std::endl;
        std::cout << "Alternatively, use parameters output_i <index1> <index2> to get all primes between two indexes (including, starting at zero). Optionally give base filename." << std::endl;
        return 1;
    }
    llu read_min = 0;
    llu read_max = 0;
    if (std::string(argv[1]) == "output") {
        if (argc < 4) {
            std::cout << "Please start with the minimum and maximum number, e.g. 2E8 2.1E8" << std::endl;
            return 1;
        }
        read_min = (llu)std::stold(argv[2]);
        read_max = (llu)std::stold(argv[3]);
    }
    size_t read_min_index = 0;
    size_t read_max_index = 0;
    if (std::string(argv[1]) == "output_i") {
        if (argc < 4) {
            std::cout << "Please start with the minimum and maximum index (including, starting at zero), e.g. 2E6 2.1E6" << std::endl;
            return 1;
        }
        read_min_index = (size_t)std::stold(argv[2]);
        read_max_index = (size_t)std::stold(argv[3]);
    }
    if (read_max > 0 || read_max_index > 0) {
        if (argc >= 5) {
            primes_filename_base = std::string(argv[4]);
        }
        PrimesStore primes(primes_filename_base);
        primes.ouput_primes(read_min, read_max, read_min_index, read_max_index);
        return 0;
    }

    llu max_prime = (llu)std::stold(argv[1]);
    llu step = (llu)std::stold(argv[2]);        // a little bit smaller will be quicker as it saves some useless calculations
    if (argc >= 4) {
        primes_filename_base = std::string(argv[3]);
    }
    unsigned int num_threads = std::thread::hardware_concurrency(); // Get the number of CPU threads
    if (argc >= 5) {
        num_threads = (unsigned)std::stold(argv[4]);
    }

    llu start_value = 1E9; // some starting value, until which value to store numbers in the first reading. If high, then only needs to read files ones, but needs more memory.

    PrimesStore primes(primes_filename_base);
    primes.read_primes(start_value);
    if (primes.size() > 0 && primes.getMaxPrime() >= max_prime) {
        std::cout << "Nothing to do" << std::endl;
        return 0;
    }
    PrimesSave primesSave(primes.getMaxPrime(), primes.getNumberPrimes(), primes_filename_base);
    if (primes.getBack() == 5) {        // that's bit of a hack
        std::vector<llu> primes_temp = {2,3,5};
        primesSave.addVector(primes_temp);
    }

    llu start = primesSave.getMaxPrime();
    Worker workers[num_threads];
    std::vector<std::vector<llu>> primes_in_progress(num_threads);      // has the result vector for each thread
    std::vector<unsigned> threads_finished(num_threads);    // can't use bool because of std::ref
    std::vector<size_t> work_in_progress(num_threads);         // store the counter of work the thread is doing
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
                    primesSave.addVector(primes_in_progress[i]);
                    next_index_to_insert++;
                    while (!primes_to_insert.empty() && primes_to_insert_index[0] == next_index_to_insert) {
                        // Is the next set of primes already available to insert to the file
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
                }
                primes_in_progress[i].clear();
                start_work = true;
            } else if (threads_finished[i] == 2) {
                start_work = true;
            }
            if (start_work) {
                // Check how far we can calculate
                PrimesCalc primes_calc = primes.transfer_calc_until(start + step);
                llu temp_max_prime = primes.getMaxPrime();      // getMaxPrime is not available on primes_calc and is only updated after transfer_*until
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
                if (start < max_prime){
                    if (start_work) {
                        threads_finished[i] = 0;
                        workers[i].push_task(calc_primes, primes_calc, start, temp_step, std::ref(primes_in_progress[i]), std::ref(threads_finished[i]));
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
g++ -Wall -g -o primes_cpp_v7 primes_cpp_v7.cpp && rm my_primes*.txt ; time ./primes_cpp_v7 10E6 5E5
real	0m0.895s 0m0.902s 0m0.972s
user	0m5.280s 0m5.276s 0m5.234s
-> slightly quicker, but improvement is in less storage use
g++ -Wall -g -o primes_cpp_v7 primes_cpp_v7.cpp && rm my_primes* ; time ./primes_cpp_v7 100E6 1E6 my_primes.bin 4       # limit to number of physical cores
real	0m23.578s 0m22.809s 0m24.557s
user	1m33.827s 1m30.396s 1m37.596s
*/
