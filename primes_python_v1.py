#!/usr/bin/env python
"""
Prime number generator in python, simplest version
"""
import sys

if len(sys) < 2:
    print('Please start with the maximum prime to be calculated')
    exit(1)

maxprime = sys.argv[1]

# Start with the lowest primes
primes = [2,3]
for entry in primes:
    print(entry)

if maxprime < max(primes):      # Finished already
    exit(0)
    
for ii in range(max(primes),maxprime):
    for prime in primes:
        
