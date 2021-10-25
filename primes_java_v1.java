/*
Run directly with:
    java primes_java_v1.java
Compile and run with:
    javac primes_java_v1.java
    java primes_java_v1 
*/

public class primes_java_v1       // Better: start with upper case letter, but then also the filename should start with an upper case letter
{
  
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

  
    public static void main(String[] args)
    {
        if (args.length < 1) {
            System.out.println("Please start with the maximum potential prime to be calculated");
            System.exit(1);
        }       // if
        
        int maxprime = (int)(Float.parseFloat( args[0] ) + 0.5) ;
        boolean isprime = true;
        int add_entries_arr = 10000;
        int primesArr_nn_max = add_entries_arr;
        long[] primesArr = new long[primesArr_nn_max];
        primesArr[0] = 3;
        primesArr[1] = 7;
        int primesArr_nn = 1;
        long number_to_test = 11;    // Start with prime 11
        byte[] add = {2,4,2,2};     // Ignore the numbers endding with 5
        byte pos_add = 0;
        
        while(number_to_test <= maxprime){
            isprime = true;
            for(int jj = 0; jj <= primesArr_nn; jj++){
                if (number_to_test%primesArr[jj] == 0){
                    isprime = false;
                    break;
                }   // if
                if (number_to_test/primesArr[jj] < primesArr[jj]){    // testing higher primes is not necessary
                    break;
                }   // if
            }   // for
            if (isprime){
                primesArr_nn += 1;
                if (primesArr_nn >= primesArr_nn_max)
                {
                    primesArr = addArrayLong_mult(primesArr_nn_max+add_entries_arr, primesArr, primesArr_nn-1);
                    //for(int jj = 0; jj <= primesArr_nn-1; jj++){System.out.println(primesArr[jj]);}
                    primesArr_nn_max += add_entries_arr;
                }   // if
                primesArr[primesArr_nn] = number_to_test;
                System.out.println(number_to_test);
            }   //if
            number_to_test += add[pos_add];
            pos_add ++;
            if (pos_add > 3){pos_add = 0;}
        }   // while
    }       // main()
}           // class primes_java_v1


/*
time java primes_java_v1.java 1000000
with println and cores used
0m1.751s 0m1.828s 0m1.903s
time java primes_java_v1.java 10E6
no println and cores free
0m5.443s 0m5.561s 0m5.490s
javac primes_java_v1.java
time java primes_java_v1 10E6
no println and cores free
0m5.061s 0m5.076s 0m4.982s
*/

