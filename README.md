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


## primes_java_v1.cpp
* Only doing the necessary calculations, each number and each prime are calculated consecutively

Needed less than 2 seconds for primes up to 1E6.

Needed about 6 seconds for primes up to 10E6 when the other 7 cores are free and the numbers are not printed out. About 2 seconds comes from the printing of the numbers.

