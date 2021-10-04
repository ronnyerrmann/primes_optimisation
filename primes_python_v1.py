#!/usr/bin/env python
"""
Prime number generator in python, simplest version, very unefficient
- why are even numbers tested?
- why are numbers ending in 5 tested?
- for loops are inefficient
- second if in the inner loop can be moved into the loop condition
"""
import sys

# Check that the number given by the user makes sense
if len(sys.argv) < 2:
    print('Please start with the maximum potential prime to be calculated')
    exit(1)

try:
    maxprime = int(float(sys.argv[1]))
except:
    print('Please give an integer for the maximum potential prime to be calculated')

# Start with the lowest primes
primes = [2,3]
for entry in primes:
    print(entry)

if maxprime < max(primes):      # Finished already
    exit(0)
    
for ii in range(max(primes)+1,maxprime):
    isprime = True
    for prime in primes:
        print(ii, prime, type(ii), type(prime))
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
time python3 primes_python_v1.py 1000000
0m8.075s 0m7.984s 0m7.322s
"""
