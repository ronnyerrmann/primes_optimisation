#!/usr/bin/env python
"""
Prime number generator in python, removing some inefficiencies of version 1
- we only test numbers that can be primes (not even, not ending in 5)
- we don't test with prime numbers 2 and 5, because of the previous statement
! needs longer than version 1 because it uses class numpy.int64 instead of int
"""
import sys
import numpy as np

# Check that the number given by the user makes sense
if len(sys.argv) < 2:
    print('Please start with the maximum potential prime to be calculated')
    exit(1)

try:
    maxprime = int(float(sys.argv[1]))
except:
    print('Please give an integer for the maximum potential prime to be calculated')

# Start with the lowest primes
primes = [2,3,5,7]
for entry in primes:
    print(entry)

if maxprime < max(primes):      # Finished already
    exit(0)

numbers_to_test = np.arange(11,maxprime,2)              # Make sure it starts with an uneven number
remove_5 = np.arange(2,numbers_to_test.shape[0],5)      # Positions of numbers ending with 5 in the above list,
numbers_to_test = np.delete(numbers_to_test, remove_5)

primes = [3,7]      # Division by 2 or 5 is not necessary to test

for ii in numbers_to_test:
    isprime = True
    for prime in primes:
        #print(ii, prime, type(ii), type(prime))
        if ii%prime == 0:
            isprime = False
            break
        # Don't check the primes that can't be in ii anyway
        if prime**2 > ii:
            break
    if isprime:
        primes.append(ii)
        print(ii)
    
"""
time python3 primes_python_v2.py 1000000
0m11.340s 0m9.972s 0m9.874s
"""
