#include <iostream>
#include <vector>
#include <string>

using namespace std;
	
typedef unsigned long long int llu;

int main(int argc, char *argv[])
{
    if (argc < 2) {
        cout << "Please start with the maximum potential prime to be calculated" << endl;
        return 1;
    }
    
    llu maxprime = (llu)(stold( argv[1] ) + 0.5) ;
    bool isprime = true;
    vector<llu> primes {3,7}; 
    llu number_to_test = 11;    // Start with prime 11
    llu add[4] = {2,4,2,2};     // Ignore the numbers endding with 5
    unsigned pos_add = 0;
    
    while(number_to_test<=maxprime){
        isprime = true;
        for(size_t jj=0; jj<=primes.size(); jj++){
            //cout << number_to_test << " " << jj << " " << primes[jj] << " " << maxprime << endl;
            if (number_to_test%primes[jj] == 0){
                isprime = false;
                break;
            }   //if
            if (number_to_test/primes[jj] < primes[jj]){    // testing higher primes is not necessary
                break;
            }   //if
        }   //for
        if (isprime){
            primes.push_back(number_to_test);
            //cout << number_to_test << endl;
        }   //if
        number_to_test += add[pos_add];
        pos_add ++;
        if (pos_add > 3){pos_add = 0;}
    }   //while
    
    return 0;
}

/*
g++ -o primes_cpp_v3 primes_cpp_v3.cpp
time ./primes_cpp_v3 10E6
0m6.329s 0m6.497s 0m6.426s
-> change of datatype doesn't make it slower compared to v1
*/




