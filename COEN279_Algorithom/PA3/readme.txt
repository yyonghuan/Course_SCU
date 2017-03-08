COEN279 Programming3 :Implement the algorithm to find the strongly connected components of a given directed graph G

1. clasee Vertex //The vertex of graph, 
	Attributes:
        self.id = key
        self.connectedTo = []
        self.color = 'white'
        self.f_time = 0
        self.d_time = 0
        self.pare = None
	Methods:
	    def addNeighbor(self,nbr):
	    def __str__(self):
	    def getConnections(self):
	    def getId(self):
	    def set_color(self,color):
	    def get_color(self): 
	    def set_f_time(self, time):
	    def set_d_time(self, time):
	    def get_f_time(self):
	    def get_d_time(self):
	    def set_pare(self,pare):
	    def get_pare(self):

2. Class Graph:
	Attributes:
	        self.vertList = {}
	        self.numVertices = 0
	        self.time = 0
	Methods:
	    def addVertex(self,key):
	    def getVertex(self,n):
	    def addEdge(self,f,t):
	    def getVertices(self):
	    def dfs(self):        
	    def dfs2(self, order):        
	    def dfsvisit(self,startVertex):
	    def print_tree(self):
		
3. def main()// If the input is invalid, print "Invalid Input!";
				If the input is valid, print the result of scc numbers and vertices in each tree

4. Time Complexity Analysis:
	The time complexity of dfs() and dfs2() is the same, which is O(V+E).
	The time complexity of the whole algorithm is O(V+E)

5. Performance:

time /opt/python-3.4/linux/bin/python3 P3.py < /home/twang1/test/coen279/t30.dat >> out

real	0m0.075s
user	0m0.042s
sys	0m0.010s
time /opt/python-3.4/linux/bin/python3 P3.py < /home/twang1/test/coen279/t31.dat >> out

real	0m0.058s
user	0m0.039s
sys	0m0.009s
time /opt/python-3.4/linux/bin/python3 P3.py < /home/twang1/test/coen279/t37.dat >> out

real	0m0.045s
user	0m0.026s
sys	0m0.007s
time /opt/python-3.4/linux/bin/python3 P3.py < /home/twang1/test/coen279/t38.dat >> out

real	0m0.048s
user	0m0.031s
sys	0m0.007s

6. Output:
[yyang1@linux60813 ~]$ cat AutoTestDir/yyang1/out
4
1 5 2
3 4
6 7
8

--------------------

0


--------------------

Invalid input!

--------------------

Invalid input!

--------------------

