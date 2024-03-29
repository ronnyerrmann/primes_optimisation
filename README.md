# Prime number calculation and optimisation
Primenumber generator, exploring different implementations and languages


## primes_python_v1.py
* Starting very simple, doing calculations as one would do by hand

Needed about 8 seconds for primes up to 1E6


## primes_python_v2.py
* Thinking about what numbers don't have to be tested to decrease the number of computations

Needed about 10 seconds for primes up to 1E6, the longer time is because numpy.int64 is already used.


## primes_python_v3.py
* Using numpy array operations: Numbers spanning over a range of 100k are tested for the same prime at the same time.

Needed less than 3 seconds for primes up to 1E6. Only one core was free. About 1 second comes from the printing of the numbers.

Needed about 14 seconds for primes up to 10E6 when the other 7 cores are free and the numbers are not printed out.


## primes_python_v4.py
* Using numpy array operations and multiprocessing: Numbers spanning over a range of 100k are tested for the same prime at the same time and that is done on 8 cores in parallel.

Needed about 4 seconds for primes up to 10E6 when running on 8 cores and the numbers are not printed out.

Needed about 95 seconds for primes up to 100E6 (over 10 minutes total CPU time).


## primes_cpp_v1.cpp
* Only doing the necessary calculations, each number and each prime are calculated consecutively

Needed less than 1 seconds for primes up to 1E6.

Needed about 4 seconds for primes up to 10E6 when the other 7 cores are free and the numbers are not printed out. About 2 seconds comes from the printing of the numbers.

Needed about 7 seconds for primes up to 10E6 when using long instead of int.


## primes_cpp_v2.cpp
* Instead of adding one prime at a time to the vector, enlarge the vector in bigger steps and then fill it once the primes are calculated

Needed about 7 seconds for primes up to 10E6, improvement to primes\_cpp\_v1.cpp is only few percent.


## primes_cpp_v3.cpp
* Based on v1, but uses long long unsigned int

This didn't increase computation time (as seen in python).


## primes_cpp_v4.cpp
* Use multithreading
* Use three different vectors to store the primes, depending on the size of the number, to save memory

This sped up the computation, but the overall CPU time remained more or less the same.


## primes_cpp_v5.cpp
* Only keep necessary primes to calculate the next primes in memory, as otherwise running out memory
* Use only two different vectors to store the primes, depending on the size of the number, as the number of items in first adds more overhead than savings (32 bit numbers might be stored as 64 bit anyways)

Slightly faster on real time, but slightly more CPU time. Saving might have some impact as well.

## primes_cpp_v6.cpp
* When calculating primes, don't use the interface of PrimesBase, but the vector directly.
* Allows the storing the primes in a binary file, which reads and writes quicker. The file is still quite big (400MB until 1E9) and another improvement could be to split into several files or to spend some CPU time to covert it into 62-adecimal number and store as text
* This also shows well how quickly the complexity growths, this version requires 10 times the number of code lines compared to the first versions.

Slightly faster on real time (1 second) and less CPU time (5.6 s) for primes up to 10E6.

## primes_java_v1.cpp
* Only doing the necessary calculations, each number and each prime are calculated consecutively

Needed less than 2 seconds for primes up to 1E6.

Needed about 5 seconds for primes up to 10E6 when the other 7 cores are free and the numbers are not printed out. About 1 second comes from the printing of the numbers. Without compiling through javac another 0.5 seconds are needed.

Improvement with replacing a division with a multiplication: nearly twice as fast for primes up to 10E6.

## primes_java_v2.cpp
* Only doing the necessary calculations, creating some lists in preparation to do parallel calculations. This has improved the speed, as the check which primes are not needed to be tested as often, but then handling the lists takes a bit longer.

Needed less than 2 seconds for primes up to 1E6.

Needed about 3 to 4 seconds for primes up to 10E6 when the other 7 cores are free and the numbers are not printed out. Testing primes in ranges of 100 is more efficient than in the ranges of 10k (trade off between uneccessary primes tested and calculating prime*prime to check what primes are not useful).

