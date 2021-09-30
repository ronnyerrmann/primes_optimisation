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
