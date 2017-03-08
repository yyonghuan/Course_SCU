import time
import sys

# Solving N in Brute force way
def expo_bruteforce( num ):

    maxnum = float("inf")
    Number = 1
    expo = num
    
    while(1):
        temp =  expo * num
        if not temp == maxnum:
            expo = temp
            Number += 1
        else:
            print( Number )
#            print( '%.13e'%temp )            
            break
        
    return Number;

#The Golden Result of x ** N
def expo_Goldenresult( x, n):
    result=1
    while n:
        if (n&1):
            result*=x
        x*=x
        n>>=1
    return result

#Yonghuan Yang's solution for Problem 1
def problem1 ( x ):
    if x < 1.00001 or x > 1.11:
        print (" x is out of range!")
#x = 1.1
    else:
#        x = 1.00001
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

#Yonghuan Yang's solution for Problem2
def problem2 ( x, numN ):
    numH = 0
    numI = 0
    numJ = 0
    numK = 0
    
    while (1):
        if ( numN % 7 == 0 ):
            numK += 1
            numN = numN / 7
        else:
            break
    while (1):
        if ( numN % 5 == 0 ):
            numJ += 1
            numN = numN / 5
        else:
            break
    while (1):
        if ( numN % 3 == 0 ):
            numI += 1
            numN = numN / 3
        else:
            break
    while (1):
        if ( numN % 2 == 0 ):
            numH += 1
            numN = numN / 2
        else:
            break
    if ( numN != 1 ):
        print (" The given N is invalid! ")
    else:
        x2 = x
        temp2 = x
        while (numH):
            temp2 = temp2 * x2
            x2 = temp2
            numH -= 1
            
        temp3 = temp2
        x3 = temp2
        while (numI):
            for i in range(1, 3):
                temp3 = temp3 * x3
            x3 = temp3
            numI -= 1
            
        temp5 = temp3
        x5 = temp3
        while (numJ):
            for i in range(1, 5):
                temp5 = temp5 * x5
            x5 = temp5
            numJ -= 1
            
        temp7 = temp5
        x7 = temp5
        while (numK):
            for i in range(1, 7):
                temp7 = temp7 * x7
            x7 = temp7
            numK -= 1
                        
        expo = temp7
        return expo

# Input x in range [1.00001,1.1] && N                
s = []
for line in sys.stdin:
    s.append(line)
    
for i in range(len(s)):
    s[i] = s[i][:s[i].find(' ')]

#Value of N   
for i in range( 1,len(s)):
    s[i] = int(s[i])

# Value of input x
x = float(s[0])

# An Optimized way to solve N
if x < 1.00001 or x > 1.1:
    print("x is out of range!")
else:
    numN_o = problem1( x )
    print('%d'%numN_o)

    # Solve expo in Factorization way
    for i in range(1, len(s)):
        if ( s[i] > numN_o ):
            print( "Overflow!" )
        else:
            expo = problem2( x, s[i] )
            print( '%.13e'%expo )
