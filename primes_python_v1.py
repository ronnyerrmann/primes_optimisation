#!/usr/bin/env python
"""
Prime number generator in python, simplest version
"""
import sys

if len(sys) < 2:
    print('Please start with the maximum prime to be calculated')
    exit(1)

maxprime = sys.argv[1]
