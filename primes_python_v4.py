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
import multiprocessing

def test_numbers_for_primes(data):
    numbers_to_test, primes = data
    isprime = (numbers_to_test > 0)                             # Start with setting that all numbers could be primes
    maxpos = np.where(primes_2 > numbers_to_test[-1])[0][0]     # Not all prime numbers need to be tested
    for prime in primes[:maxpos]:
        remain = numbers_to_test[isprime]%prime                 # Only calculate for numbers that can be primes
        isprime[isprime] = (remain > 0)                         # Only if a remainder exist the number still could be a prime
    
    return numbers_to_test[isprime]                             # Prime numbers

if __name__ == "__main__":
    # Check that the number given by the user makes sense
    if len(sys.argv) < 2:
        print('Please start with the maximum potential prime to be calculated')
        exit(1)

    try:
        maxprime = int(float(sys.argv[1]))
    except:
        print('Please give an integer for the maximum potential prime to be calculated')
        exit(1)

    if len(sys.argv) >= 3:
        try:
            steprange = int(float(sys.argv[2]))
        except:
            print('Please give an integer for number calculated in each step')
    else:
        steprange = 100000

    printprimes = True
    if 'noprint' in sys.argv:
        printprimes = False
        
    cpu_cores = multiprocessing.cpu_count()
    print('Running the script on {0} CPU cores in parallel.'.format(cpu_cores))

    primes = np.array([3,7])        # Division by 2 or 5 is not necessary to test
    primes_2 = primes**2

    stepstart = 11                  # Make sure it starts with an uneven number
    while stepstart <  maxprime:
        numbers_to_test_list = []
        while len(numbers_to_test_list) < 1000 and stepstart < primes_2[-1] and stepstart < maxprime:
            stepend = min(stepstart+steprange, primes_2[-1], maxprime)
            numbers_to_test_iter = np.arange(stepstart,stepend,2)            # Initialise the numbers to be tested in this step
            for ii in range(5):
                if str(numbers_to_test_iter[ii])[-1] == '5':
                    break
            remove_5 = np.arange(ii,numbers_to_test_iter.shape[0],5)         # Positions of numbers ending with 5 in the above list,
            numbers_to_test_iter = np.delete(numbers_to_test_iter, remove_5)
            numbers_to_test_list.append([numbers_to_test_iter, primes])
            
            stepstart = numbers_to_test_iter[-1]+2                           # Next test
        
        if cpu_cores > 1:
            with multiprocessing.Pool(cpu_cores) as p:       # only possible in python3
                primes_new_list = p.map(test_numbers_for_primes, numbers_to_test_list)
        else:
            primes_new_list = []
            for numbers_to_test in numbers_to_test_list:
                primes_new_list.append( test_numbers_for_primes([numbers_to_test, primes]) )
        
        for primes_new in primes_new_list:
            primes = np.append(primes, primes_new)
            primes_2 = np.append(primes_2, primes_new**2)
            if printprimes:
                for ii in primes_new:
                    print(ii)
            #print(primes_new)
        


"""
time python3 primes_python_v4.py 1E7 1E5 noprint
0m3.634s 0m3.657s 0m3.658s
time python3 primes_python_v4.py 1E8 1E5 noprint
1m35.063s 
"""
