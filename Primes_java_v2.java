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

    public static long[] test_numbers_for_primes(long numbers_to_test[], long primesArr[], int numbers_to_test_nn, int primesArr_nn)
    {   /*
        Tests, if the entries in numbers_to_test are prime numbers
        Returns an array in which all non-primes are set to 0
        */
        int new_prime_nn = 0;
        for(int ii = 0; ii <= numbers_to_test_nn; ii++){
        
            boolean isprime = true;
            for(int jj = 0; jj <= primesArr_nn; jj++){
                if (numbers_to_test[ii]%primesArr[jj] == 0){
                    isprime = false;
                    break;
                }   // if
                /* Not neccessary as done in the creation of the numbers_to_test array
                if (numbers_to_test[ii]/primesArr[jj] < primesArr[jj]){    // testing higher primes is not necessary
                    break;
                }   // if */
            }   // for
            if (!isprime){
                numbers_to_test[ii] = 0 ;
            }   //if
            else {
                System.out.println(numbers_to_test[ii]);
            }   // else
        } // for
        
        return numbers_to_test;
    }
  
    public static void main(String[] args)
    {
        /*List<ArrayList<String>> listOfLists = new ArrayList<ArrayList<String>>();
        ArrayList<String> list1 = new ArrayList<String>();
        list1.add("Delhi");
        list1.add("Mumbai");
        listOfLists.add(list1);
 
        ArrayList<String> anotherList = new ArrayList<String>();
 
        anotherList.add("Beijing");
        anotherList.add("Shanghai");
        listOfLists.add(anotherList);
 
        listOfLists.forEach((list)  -> 
        {
            list.forEach((city)->System.out.println(city));
        }
                );*/
        
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
        int add_entries_arr = 10000;
        int primesArr_nn_max = add_entries_arr;
        long[] primesArr = new long[primesArr_nn_max];
        primesArr[0] = 3;
        primesArr[1] = 7;
        int primesArr_nn = 1;
        long stepstart = 11;    // Start with prime 11
        
        byte pos_add = 0;
        
        while(stepstart <= maxprime){
            List<List<Long>> numbers_to_test_list = new ArrayList<>();
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
            for(int i=0; i< add_prime.length ; i++) {
                System.out.print(add_prime[i] +" ");
            }
            System.out.println("from " + stepstart);
            
            long max_available_prime_2 = primesArr[primesArr_nn]*primesArr[primesArr_nn];
            // create at most a 1000 different lists to with numbers to check for primes, each containing a range of steprange
            while(numbers_to_test_list.size() < 3 && stepstart < max_available_prime_2 && stepstart <= maxprime){
                List<Long> numbers_to_test_iter = new ArrayList<>();
                long number_to_test = stepstart;
                long stepend = stepstart + steprange;
                if(stepend > maxprime){stepend = maxprime;}
                while(number_to_test <= stepend){
                    numbers_to_test_iter.add(number_to_test);
                    // Calculate the next prime
                    number_to_test += add_prime[pos_add];
                    pos_add ++;
                    if (pos_add > 3){pos_add = 0;}
                }   // while
                numbers_to_test_iter.forEach((entry) -> {System.out.println(entry);});
                numbers_to_test_list.add(numbers_to_test_iter);
                stepstart = number_to_test;
            }   // while
            System.out.println("new set");
            // Give the each entry of numbers_to_test_list to test_numbers_for_primes and then deal with the results


        }   // while
            
            
    }       // main()
}           // class primes_java_v1


/*
time java primes_java_v1.java 1000000

*/

