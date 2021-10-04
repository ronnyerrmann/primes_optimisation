#!/usr/bin/env python
"""
Prime number generator in python, removing some inefficiencies of version 1
- we only test numbers that can be primes (not even, not ending in 5)
- we don't test with prime numbers 2 and 5, because of the previous statement
and version 2
- using numpy properly, multiple calculation, only necessary steps are calculated
- about 1 second is now used for the printing of the numbers
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

printprimes = True
if 'noprint' in sys.argv:
    printprimes = False

# Start with the lowest primes
primes = [2,3,5,7]
if printprimes:
    for entry in primes:
        print(entry)

if maxprime < max(primes):      # Finished already
    exit(0)

primes = np.array([3,7])        # Division by 2 or 5 is not necessary to test
primes_2 = primes**2

stepstart = 11                  # Make sure it starts with an uneven number
while stepstart <  maxprime:
    stepend = min(stepstart+100000, primes_2[-1], maxprime)
    numbers_to_test = np.arange(stepstart,stepend,2)            # Initialise the numbers to be tested in this step
    for ii in range(5):
        if str(numbers_to_test[ii])[-1] == '5':
            break
    remove_5 = np.arange(ii,numbers_to_test.shape[0],5)         # Positions of numbers ending with 5 in the above list,
    numbers_to_test = np.delete(numbers_to_test, remove_5)
    
    isprime = (numbers_to_test > 0)                             # Start with setting that all numbers could be primes
    maxpos = np.where(primes_2 > numbers_to_test[-1])[0][0]     # Not all prime numbers need to be tested
    for prime in primes[:maxpos]:
        remain = numbers_to_test[isprime]%prime                 # Only calculate for numbers that can be primes
        isprime[isprime] = (remain > 0)                         # Only if a remainder exist the number still could be a prime
    
    primes = np.append(primes, numbers_to_test[isprime])
    primes_2 = np.append(primes_2, numbers_to_test[isprime]**2)
    if printprimes:
        for ii in numbers_to_test[isprime]:
            print(ii)
    stepstart = numbers_to_test[-1]+2


"""
time python3 primes_python_v3.py 1000000
0m2.723s 0m2.780s 0m2.910s
time python3 primes_python_v3.py 10000000
without the print, steps of 1000000, only 1 core free
0m42.741s 0m48.301s 0m50.935s
without the print, steps of 100000, only 1 core free
0m46.227s 0m43.924s 0m44.255s
without the print, steps of 100000, cores are free
0m13.577s 0m13.496s 0m13.607s
"""
