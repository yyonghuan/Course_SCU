COEN279 Programming2 :Dynamic Programming Python implementation of Matrix Chain Multiplication.

1. def InputProcess() //Input the numbers of the matries and each matrix A[i];
		The function must identify the invalid input;

2. def MatrixChainOrder(p, n)// Give the matrix chain order m and s, with the input p and n.
		m[i,j] = Minimum number of scalar multiplications needed
		to compute the matrix A[i]A[i+1]...A[j] = A[i..j] where
		dimension of A[i] is p[i-1] x p[i];
		s[i][j] records the value of k such that an optimal parenthesization of 		A[i]A[i+1]...A[j]
3. def PrintOptimalParents(s, i, j) //Print an optimal parenthesization of <A[1]A[2]...A[n]>
4. def multi(A, B, i, j, k)//Because the Autotest do not support numpy, this function is used 
		to mutiply two matrix A(i,k) and B(k,j);
5. def Multiplication(A, p, s, i, j)//Do the matix multiplication in the order given by 		MatrixChainOrder(p, n) function;
6. def main()// If the input is invalid, print "Invalid Input!";
				If the input is valid, print the result of multiplication and the dimension of 
				the result matrix.

Time Complexity Analysis:
	The time complexity of MatrixChainOrder(p, n) is O(n^3).
Space Complexity Analysis:
	The space complexity of MatrixChainOrder(p,n) is O(n^2).
Performance:

[yyang1@linux60812 ~]$ cat AutoTestDir/yyang1/out
Invalid input!

--------------------

Invalid input!

--------------------

2 3
6.7215017341077e+00	3.1550634130836e+00	6.2647937871516e+00
4.1942975185812e+00	3.9486638940871e+00	7.7822001874447e+00

--------------------

2 2
5.1107250485727e+01	5.5224411496872e+01
4.5917403719794e+01	6.7372373694719e+01

--------------------

2 2
2.5744765119736e+09	1.6790447577507e+09
2.6514067091340e+09	1.7292013942933e+09

--------------------

