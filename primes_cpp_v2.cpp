#include <iostream>
#include <vector>
#include <string>

using namespace std;


void myrange(int start, int end, int step=1) {
    int number = (int)((end-start)/step+.5);
    int *array = new int[number];
    int jj = start;
	for(int ii=0; ii<number; ii++)
	{
		array[ii] = jj;
		jj += step;
	}

	for(int ii=0; ii<=number+1; ii++){
	    cout << array[ii] << endl;
	}
}

vector<long> addVectorLong_mult(int nn_max, vector<long> arr, int oldnn)
{
    /* Instead of adding one element at the time, and each time copying each element from the old to the new vector,
    it adds many entries at the end, which then can be filled slowly. 
    Compared to python the programmer has to know up to which element the vector is filled.
    :param nn: new length of vector
    :param arr: old vector
    :param oldnn: old length of vector arr
    return newarr: vector of length nn, filled until oldnn with arr
    */
    int ii;
  
    // create a new array of size nn_max
    vector<long> newVec(nn_max);
  
    // insert the elements from
    // the old vector into the new vector
    for (ii = 0; ii <= oldnn; ii++)
    {
        newVec[ii] = arr[ii];
        //cout << 'b' << newVec[ii] << endl;
    }
    return newVec;
}


	

	

int main(int argc, char *argv[])
{
    if (argc < 2) {
        cout << "Please start with the maximum potential prime to be calculated" << endl;
        return 1;
    }
    
    int maxprime = (long)(stold( argv[1] ) + 0.5) ;
    bool isprime = true;
    int add_entries_vec = 10000;
    int primesVec_nn_max = add_entries_vec;
    vector<long> primesVec(primesVec_nn_max);
    primesVec[0] = 3;
    primesVec[1] = 7;
    int primesVec_nn = 1;
    int number_to_test = 11;    // Start with prime 11
    int add[4] = {2,4,2,2};     // Ignore the numbers endding with 5
    int pos_add = 0;
    
    while(number_to_test<=maxprime){
        isprime = true;
        for(int jj=0; jj<=primesVec_nn; jj++){
            //cout << number_to_test << " " << jj << " " << primes[jj] << " " << maxprime << endl;
            if (number_to_test%primesVec[jj] == 0){
                isprime = false;
                break;
            }   //if
            if (number_to_test/primesVec[jj] < primesVec[jj]){    // testing higher primes is not necessary
                break;
            }   //if
        }   //for
        if (isprime){
            primesVec_nn += 1;
            if (primesVec_nn >= primesVec_nn_max)
            {
                primesVec = addVectorLong_mult(primesVec_nn_max+add_entries_vec, primesVec, primesVec_nn-1);
                //for(int jj = 0; jj <= primesVec_nn-1; jj++){cout << 'a' << primesVec[jj] << endl;}
                primesVec_nn_max += add_entries_vec;
            }   // if
            primesVec[primesVec_nn] = number_to_test;
            //cout << number_to_test << endl;
        }   //if
        number_to_test += add[pos_add];
        pos_add ++;
        if (pos_add > 3){pos_add = 0;}
    }   //while
    
    return 0;
}

/*
g++ -o primes_cpp_v2 primes_cpp_v2.cpp
time ./primes_cpp_v2 10E6
0m6.554s 0m6.653s 0m6.638s
-> no significant improvements compared to <vector>.push_back
*/
