#include <iostream>
#include <vector>
#include <string>

using namespace std;
	

int main(int argc, char *argv[])
{
    if (argc < 2) {
        cout << "Please start with the maximum potential prime to be calculated" << endl;
        return 1;
    }
    
    int maxprime = (int)(stold( argv[1] ) + 0.5) ;
    bool isprime = true;
    vector<int> primes {3,7}; 
    int number_to_test = 11;    // Start with prime 11
    int add[4] = {2,4,2,2};     // Ignore the numbers endding with 5
    int pos_add = 0;
    
    while(number_to_test<=maxprime){
        isprime = true;
        for(int jj=0; jj<=primes.size(); jj++){
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
g++ -o primes_cpp_v1 primes_cpp_v1.cpp
time ./primes_cpp_v1 1000000
0m0.512s 0m0.538s 0m0.547s
time ./primes_cpp_v1 10E6
with cout, 4 cores free
0m9.077s 0m8.763s 0m8.889s
without cout, 4 cores free
0m6.808s 0m6.975s 0m5.994s
without cout, cores are free
0m3.873s 0m3.816s 0m3.953s
time ./primes_cpp_v1 100E6
1m28.725s 1m31.448s 1m28.707s
*/




