/*
Run directly with:
    java primes_java_v2.java
Compile and run with:
    javac primes_java_v2.java
    java primes_java_v2 
*/

import java.util.ArrayList;
import java.util.List;
import java.util.*;
import java.lang.Thread;


public class Primes_java_v2
{
    public Primes_java_v2() // Constructor of Class Primes_java_v2
    {
        int testvar = 10;
    }
    
    
    // Function to add x to end of arr -- not used
    public static long[] addArrayLong(int nn, long arr[], long x)
    {
        /* in python: <list>.append(<value>)
        :param nn: length of array arr
        :param arr: old array
        :param x: new value
        return newarr: array of length nn+1, copy of arr, with x at the end
        */
        int ii;
  
        // create a new array of size n+1
        long newarr[] = new long[nn + 1];
  
        // insert the elements from
        // the old array into the new array
        // insert all elements till nn
        // then insert x at n+1
        for (ii = 0; ii < nn; ii++)
            newarr[ii] = arr[ii];
  
        newarr[nn] = x;
  
        return newarr;
    }
  
    public static long[] addArrayLong_mult(int nn_max, long arr[], int oldnn)
    {
        /* Instead of adding one element at the time, and each time copying each element from the old to the new array,
        it adds many entries at the end, which then can be filled slowly. 
        Compared to python the programmer has to know up to which element the array is filled.
        :param nn: new length of array
        :param arr: old array
        :param oldnn: old length of array arr
        return newarr: array of length nn, filled until oldnn with arr
        */
        int ii;
  
        // create a new array of size nn_max
        long newarr[] = new long[nn_max];
  
        // insert the elements from
        // the old array into the new array
        for (ii = 0; ii <= oldnn; ii++)
        {
            newarr[ii] = arr[ii];
        }
  
        return newarr;
    }
    
    //public static long[] test_numbers_for_primes(ArrayList<Long> numbers_to_test, long primesArr[], int numbers_to_test_nn, int primesArr_nn)
    public static long[] test_numbers_for_primes(long numbers_to_test[], long primesArr[], int numbers_to_test_nn, int primesArr_nn)
    {   /*
        Tests, if the entries in numbers_to_test are prime numbers
        Returns an array in which all non-primes are set to 0
        */
        for (int jj = 0; jj <= primesArr_nn; jj++){
            if (numbers_to_test[numbers_to_test_nn] < primesArr[jj]*primesArr[jj]){    // testing higher primes is not necessary
            //if (numbers_to_test[numbers_to_test_nn]/primesArr[jj] < primesArr[jj]){    // testing higher primes is not necessary, this increases the calculation time by 10% for java Primes_java_v2 10E6 100
                // System.out.println("Changed max prime index from: " + primesArr_nn + " to: " + jj);
                primesArr_nn = jj;
                break;
            }   // if
        }   // for
        for(int ii = 0; ii <= numbers_to_test_nn; ii++){
        
            boolean isprime = true;
            for(int jj = 0; jj <= primesArr_nn; jj++){
                if (numbers_to_test[ii]%primesArr[jj] == 0){
                    isprime = false;
                    break;
                }   // if
            }   // for
            if (!isprime){
                numbers_to_test[ii] = 0 ;
            }   //if
            //else {System.out.println("Prime: " + numbers_to_test[ii]);}   // else
        } // for
        
        //return newprimes;
        return numbers_to_test;
    }
  
    public static void main(String[] args)
    {
        if (args.length < 1) {
            System.out.println("Please start with the maximum potential prime to be calculated");
            System.exit(1);
        }       // if
        
        int maxprime = (int)(Float.parseFloat( args[0] ) + 0.5) ;
        int steprange = 100000;
        if (args.length > 1) {
            steprange = (int)(Float.parseFloat( args[1] ) + 0.5);
        }   // if

        int cpu_cores = Runtime.getRuntime().availableProcessors();
        
        boolean isprime = true;
        //int add_entries_arr = 10000;
        //int primesArr_nn_max = add_entries_arr;
        int primesArr_nn_max = 2;
        long[] primesArr = new long[primesArr_nn_max];
        primesArr[0] = 3;
        primesArr[1] = 7;
        int primesArr_nn = 1;   // last entry with a prime
        long stepstart = 11;    // Start with prime 11
        
        while(stepstart <= maxprime){
            List<ArrayList<Long>> numbers_to_test_list = new ArrayList<>();
            //List<long[]> numbers_to_test_list = new ArrayList<>();
            byte[] add_prime = {2,2,2,2};     // Ignore the numbers endding with 5
            String number_to_test_string = String.valueOf(stepstart);
            char number_to_test_last_digit = number_to_test_string.charAt(number_to_test_string.length() - 1);
            //System.out.println(number_to_test_last_digit);
            switch(number_to_test_last_digit){
                case '1': add_prime[1] = 4; break;
                case '3': add_prime[0] = 4; break;
                case '7': add_prime[3] = 4; break;
                case '9': add_prime[2] = 4; break;
            }
            //for(int ii=0; ii< add_prime.length ; ii++) {System.out.print(add_prime[ii] +" ");}
            //System.out.println("from " + stepstart);
            byte pos_add = 0;
            
            long max_available_prime_2 = primesArr[primesArr_nn]*primesArr[primesArr_nn];
            //System.out.println(numbers_to_test_list.size() + " " + stepstart + " " + primesArr_nn + " " + primesArr[primesArr_nn] + " " + max_available_prime_2 + " " + maxprime);
            /*try{Thread.sleep(1000);}
            catch (Exception expn){System.out.println(expn);}*/
            // create at most a 1000 different lists to with numbers to check for primes, each containing a range of steprange
            while(numbers_to_test_list.size() < 1000 && stepstart < max_available_prime_2 && stepstart <= maxprime){
                ArrayList<Long> numbers_to_test_iter = new ArrayList<>();
                //long[] numbers_to_test_iter = new long[(int)(steprange/2)];
                //int numbers_to_test_iter_nn = 0;
                long number_to_test = stepstart;
                long stepend = stepstart + steprange;
                if(stepend > maxprime){stepend = maxprime;}
                //System.out.println(stepstart + " " + stepend);
                while(number_to_test <= stepend){
                    numbers_to_test_iter.add(number_to_test);
                    //numbers_to_test_iter[numbers_to_test_iter_nn] = number_to_test;
                    // Calculate the next prime
                    number_to_test += add_prime[pos_add];
                    pos_add ++;
                    if (pos_add > 3){pos_add = 0;}
                }   // while
                //numbers_to_test_iter.forEach((entry) -> {System.out.println("to test: " + entry);});
                numbers_to_test_list.add(numbers_to_test_iter);
                stepstart = number_to_test;
            }   // while
            // make primesArr and primesArr_nn final, as otherwise: error: local variables referenced from a lambda expression must be final or effectively final
            final long[] primesArrF = new long[primesArr_nn+1];
            for (int ii = 0; ii <= primesArr_nn; ii++) {primesArrF[ii] = primesArr[ii];}
            final int primesArr_nnF = primesArr_nn;
            
            ArrayList<Long> new_primes = new ArrayList<>();
            // Give the each entry of numbers_to_test_list to test_numbers_for_primes and then deal with the results
            numbers_to_test_list.forEach((numbers_to_test_iter) -> {
                /* convert from ArrayList<Long> into a long array as otherwise test_numbers_for_primes(numbers_to_test_iter...) comes up with:
                    Primes_java_v2.java:198: error: incompatible types: long[] cannot be converted to ArrayList<Long>
                    ArrayList<Long> new_primes = test_numbers_for_primes(numbers_to_test_iter, primesArr, numbers_to_test_iter.size()-1, primesArr_nn);
                */
                final long[] numbers_to_test_iter_arr = new long[numbers_to_test_iter.size()];
                int index = 0;
                for (final Long value : numbers_to_test_iter) {numbers_to_test_iter_arr[index++] = value;}
                long new_primes_iter[] = test_numbers_for_primes(numbers_to_test_iter_arr, primesArrF, numbers_to_test_iter.size()-1, primesArr_nnF);
                
                for(int ii = 0; ii <= numbers_to_test_iter.size()-1; ii++){
                    if (new_primes_iter[ii] > 0){
                        new_primes.add(new_primes_iter[ii]);
                    }   // if
                }   //for
                
            });     // forEach
            // Increase Primes array if neccessary
            if (primesArr_nn + new_primes.size() > primesArr_nn_max){
                primesArr = addArrayLong_mult(primesArr_nn + new_primes.size()+1, primesArr, primesArr_nn);
                primesArr_nn_max = primesArr_nn + new_primes.size();
            }   // if
            // add all entries to the array
            for (final Long value : new_primes) {
                primesArr_nn++;
                primesArr[primesArr_nn] = value;
                //System.out.println(value);
            }


        }   // while
            
            
    }       // main()
}           // class primes_java_v1


/*
time java Primes_java_v2.java 1000000 10000
with println and cores used
0m1.776s 0m2.130s 0m2.237s
time java Primes_java_v2.java 10E6 10000
no println and cores free
0m4.173s 0m4.214s 0m4.204s
javac Primes_java_v2.java
time java Primes_java_v2 10E6 100
0m2.818s 0m2.839s 0m2.849s
time java Primes_java_v2 10E6 1000
no println and cores free
0m3.056s 0m3.057s 0m2.988s
time java Primes_java_v2 10E6 10000
no println and cores free
0m3.654s 0m3.658s 0m3.639s
time java Primes_java_v2 10E6 100000
0m3.641s 0m3.706s 0m3.665s
*/

