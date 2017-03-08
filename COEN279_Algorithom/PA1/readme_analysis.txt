Author: Yonghuan Yang[W1270098]

1. Coding language: Python3

2. Complexity analysis

(1)def problem1 (x): Function to solve problem1, to find out the largest positive integer N  
        such that x^N does not cause overflow for x in the range[1.00001,1.1] using 64-bit
        representation.
Algorithm: 
	Two steps to find the largest N(Nmax). 
	Step 1: Do the multiplications like x*x, x^2*x^2, x^4*x^4, x^8*x^8,...,until the final value is overflow. Now save the numeber of Ntemp = lnum.
	Step 2: Find the largest N between lnum and Nmax by multiply x once a time.
	
	Time complexity analysis: T(N) = (1+2+...+logN) <= logN * logN
	Space complexity is constant in this algorithm.

def problem1 (x):
    expo = x
    num = 1
    while (1):
        lexpo = x
        lnum = 1

        while (1):
            ltmp = lexpo * lexpo
            tmp = expo * ltmp
            if not tmp == float("inf"):
                lexpo = ltmp
                lnum *= 2
            else:
                break
        if lnum == 1:
            tmp = expo * x
            if not tmp == float("inf"):
                expo = tmp
                num += 1
            #print (num)
            #print ('%.13e'%expo)
            break
        else:
            expo *= lexpo
            num += lnum
    return num

(2) def problem2(x,numN): Function to solve problem2, to compute x^N for any positive integer N = 2^H * 3^I * 5^J *7^K with non-negative H, I, J, K.
Algorithm:
	Firstly, compute the value of H, I, J, K. Actually, we can compute K first and then J and then I and H. The Time complexity <logN
	Secondly, compute the value of x^N based on H, I, J, K. 
	
Time complexity:
	Time to compute value H, I, J, K + time to compute x^(2^H * 3^I * 5^J *7^K) < 
	(logN) + (2*H+3*I+5*J+7*K) < c *logN (c is an constant greater than 0)
	Big O = lgN
Space complexity: Constant

(3)In addition, there are still two functions for brute force way and fast exponention way.

def expo_bruteforce( num ) //The brute force way to solve problem 1, by multiply x once a time until overflow.
	Time complexity: Nmax times multiplication 
	Space complexity is constant.


def expo_Goldenresult( x, n)://The fast exponention way to solve x^N
    result=1
    while n:
        if (n&1):
            result*=x
        x*=x
        n>>=1
    return result
	
	Time complexity: logN
	Space complexity: constant

3. Result

[yyang1@linux60809 ~]$ cat AutoTestDir/yyang1/out
70978626
1.0003600630071e+00
1.0063198550414e+00
1.1583531116470e+00
1.5542572752295e+00
3.2575458174918e+00
5.1834095250492e+21
8.8766113821076e+68
1.1397043617210e+212

--------------------

x is out of range!

--------------------

7447
Overflow!

--------------------

4. Time

time /opt/python-3.4/linux/bin/python3 P1.py < /home/twang1/test/coen279/t10.dat >> out

real	0m0.066s
user	0m0.031s
sys	0m0.011s
time /opt/python-3.4/linux/bin/python3 P1.py < /home/twang1/test/coen279/t17.dat >> out

real	0m0.062s
user	0m0.041s
sys	0m0.011s
time /opt/python-3.4/linux/bin/python3 P1.py < /home/twang1/test/coen279/t18.dat >> out

real	0m0.055s
user	0m0.036s
sys	0m0.009s

