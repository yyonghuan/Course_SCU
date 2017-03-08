# -*- coding: utf-8 -*-
"""
Created on Fri Oct 28 14:35:18 2016

# Dynamic Programming Python implementation of Matrix Chain Multiplication.
@author: yangyonghuan
"""

import sys

def InputProcess():
       
    s = []
    for line in sys.stdin:
        s.append(line)
        
    N = int(s[0])
    p = [0 for i in range(N+1)]
    A = [ 0 for i in range(N+1)]
    
    temp = s[1].split(' ')
    p[0] = int(temp[0])
    p[1] = int(temp[1])
    flag = True
    
    i = 1 + p[0] + 1
    j = 1
    
    while i<len(s):
        temp1 = s[i].split(' ')
        temp2 = int(temp1[0])
        if temp2 != p[j]:
            flag = False
            break
        flag = True
        
        i = i + p[j] + 1
        j = j + 1
        p[j] = int(temp1[1])
    
    if flag == False:
        return flag, p, A, N
        
#    print(N)
#    print(p)

#Initialize each A[i]
    i = 1
    while i < N+1:
        A[i] = [ [ 0 for j in range(p[i]) ] for k in range(p[i-1]) ]
        i += 1
#Input the value of each A[i]
    i = 1
    k = 2
    while i < N+1:
        for j in range(p[i-1]):
            temp = s[k].split('\t')
            for h in range(p[i]):
                if len(temp[h]) < 10:
                    flag = False
                    return flag, p, A, N
                    
                A[i][j][h] = float(temp[h])
            k += 1
        k += 1
        i += 1
    return flag, p, A, N


# Matrix Ai has dimension p[i-1] x p[i] for i = 1..n
def MatrixChainOrder(p, n):
    # For simplicity of the program, one extra row and one
    # extra column are allocated in m[][].  0th row and 0th
    # column of m[][] are not used
    m = [[0 for x in range(n)] for x in range(n)]
    s = [[0 for x in range(n)] for x in range(n)]
 
    # m[i,j] = Minimum number of scalar multiplications needed
    # to compute the matrix A[i]A[i+1]...A[j] = A[i..j] where
    # dimension of A[i] is p[i-1] x p[i]
 
    # cost is zero when multiplying one matrix.
    for i in range(1, n):
        m[i][i] = 0
 
    # L is chain length.
    for L in range(2, n):
        for i in range(1, n-L+1):
            j = i+L-1
            m[i][j] = float("inf")
            for k in range(i, j):
 
                # q = cost/scalar multiplications
                q = m[i][k] + m[k+1][j] + p[i-1]*p[k]*p[j]
                if q < m[i][j]:
                    m[i][j] = q
                    s[i][j] = k
#                    print(i, j, s[i][j])

    return m,s

def PrintOptimalParents(s, i, j):
    res = ""
    if i == j:
        return("A"+ str(i))
    else:
        res += "("
        res += str(PrintOptimalParents(s, i, s[i][j]))
        res += str(PrintOptimalParents(s, s[i][j]+1, j))
        res += ")"
        return res

def multi(A, B, i, j, k):
    C = [ [ 0 for h1 in range(j) ] for k in range(i) ]
    
    for l1 in range(i):
        for l2 in range(j):
            C[l1][l2] = 0
            for l3 in range(k):
                C[l1][l2] = C[l1][l2] + A[l1][l3] * B[l3][l2]
    return C

def Multiplication(A, p, s, i, j):
    if i == j:
        return A[i]
    else:
        k = s[i][j]
        X = Multiplication(A, p, s, i, k)
        Y = Multiplication(A, p, s, k + 1, j)
        return multi(X, Y, p[i-1], p[j], p[k])

def main():
    flag, p, A, N = InputProcess()
    if flag == True:
        m,s = MatrixChainOrder(p, N+1)
        C = Multiplication(A, p, s, 1, N)
        print(p[0],p[N])
        str_list = [['{0:.13e}'.format(flt) for flt in sublist] for sublist in C]
        str_display = '\n'.join('\t'.join(lst) for lst in str_list)
        print(str_display)
    else:
        print("Invalid input!")

if __name__ == '__main__':
    main()
