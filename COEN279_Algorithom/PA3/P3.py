
# -*- coding: utf-8 -*-
"""
Created on Mon Nov  7 20:18:09 2016

@author: yangyonghuan
"""

import sys

class Vertex:
    def __init__(self,key):
        self.id = key
        self.connectedTo = []
        self.color = 'white'
        self.f_time = 0
        self.d_time = 0
        self.pare = None

    def addNeighbor(self,nbr):
        self.connectedTo.insert(0, nbr)

    def __str__(self):
        return str(self.id) + ' connectedTo: ' + str([x.id for x in self.connectedTo])

    def getConnections(self):
        return self.connectedTo

    def getId(self):
        return self.id
    
    def set_color(self,color):
        self.color = color
        
    def get_color(self):
        return self.color
        
    def set_f_time(self, time):
        self.f_time = time
        
    def set_d_time(self, time):
        self.d_time = time
        
    def get_f_time(self):
        return self.f_time
    
    def get_d_time(self):
        return self.d_time
    
    def set_pare(self,pare):
        self.pare = pare
    
    def get_pare(self):
        return self.pare

class Graph:
    def __init__(self):
        self.vertList = {}
        self.numVertices = 0
        self.time = 0

    def addVertex(self,key):
        self.numVertices += 1
        newVertex = Vertex(key)
        self.vertList[key] = newVertex
        return newVertex

    def getVertex(self,n):
        if n in self.vertList:
            return self.vertList[n]
        else:
            return None

    def __contains__(self,n):
        return n in self.vertList

    def addEdge(self,f,t):
        if f not in self.vertList:
            nv = self.addVertex(f)
        if t not in self.vertList:
            nv = self.addVertex(t)
        self.vertList[f].addNeighbor(self.vertList[t])

    def getVertices(self):
        return self.vertList.keys()

    def __iter__(self):
        return iter(self.vertList.values())

    def dfs(self):        
        for aVertex in self:
            aVertex.set_color('white')
            aVertex.set_pare(None)
            
        for aVertex in self:
            if aVertex.get_color() == 'white':
                self.dfsvisit(aVertex)

    def dfs2(self, order):        
        for aVertex in self:
            aVertex.set_color('white')
            aVertex.set_pare(None)
            
        i = 0
        while i < self.numVertices:
            aVertex = self.vertList[order[i]]
            if aVertex.get_color() == 'white':
                self.dfsvisit(aVertex)
            i += 1
    
    def dfsvisit(self,startVertex):
        startVertex.set_color('gray')
        self.time += 1
        startVertex.set_d_time(self.time)
        for nextVertex in startVertex.getConnections():
            if nextVertex.get_color() == 'white':
                nextVertex.set_pare(startVertex)
                self.dfsvisit(nextVertex)
        startVertex.set_color('black')
        self.time += 1
        startVertex.set_f_time(self.time)

    def print_tree(self):
        
        vertices = []
        for v in self:
            if len(v.connectedTo) != 0:
                if len(vertices) == 0:
                    vertices.append(v)
                else:
                    j = 0
                    while j < len(vertices):
                        if v.get_d_time() < vertices[j].get_d_time():
                            vertices.insert(j, v)
                            break
                        if v.get_d_time() > vertices[len(vertices)-1].get_d_time():
                            vertices.append(v)
                            break
                        j += 1
        tree = [[0 for x in range(0)] for x in range(self.numVertices)]
        count = 0
        for i in range(len(vertices)):
            if i == len(vertices)-1:
                tree[count].append(vertices[i].getId())
                count += 1           
            
            elif i < len(vertices)-1:
                d1 = vertices[i].get_d_time()
                d2 = vertices[i+1].get_d_time()
                tree[count].append(vertices[i].getId())
            
                if d2 - d1 != 1:
                    count += 1

        return vertices,count,tree

def main():
    
    s = []
    for line in sys.stdin:
        s.append(line)
        
    temp = s[0].split(' ')
    vertices_number = int(temp[0])
    edges_number = int(temp[1])
    #Check if the input is valid or not   
    if len(s)-1 != edges_number:
        print("Invalid input!")
    else:
        
        g = Graph()
        g_inv = Graph()
        #Create vertices for g and g_inv
        for i in range(vertices_number):
            g.addVertex(i+1)
            g_inv.addVertex(i+1)
        #Add edges for g and g_inv
        for i in range(edges_number):
            temp = s[i+1].split(' ')
            temp[0] = int(temp[0])
            temp[1] = int(temp[1])
            g.addEdge(temp[0],temp[1])
            g_inv.addEdge(temp[1],temp[0])
        #Find isolate vertices
        iso_vertex = []
        for v in g:
            if len(v.connectedTo) == 0:
                temp = v.getId()
                iso_vertex.append(temp)
        #Compute dfs for graph g
        g.dfs()
        #Compute the dfs order for g_inv
        vertices = []
        count = 0
        order = []
        
        for v in g:
            if len(vertices) == 0:
                vertices.append(v)
            else:
                j = 0
                while j< len(vertices):
                    if v.get_f_time() > vertices[j].get_f_time():
                        vertices.insert(j, v)
                        break
                    if v.get_f_time() < vertices[len(vertices)-1].get_f_time():
                        vertices.append(v)
                        break
                    j += 1
        
        for i in range(vertices_number):
            num = vertices[i].getId()
            order.append(num)
        #Compute the dfs for g_inv in the given order
        g_inv.dfs2(order)    
        tree = [[0 for x in range(0)] for x in range(vertices_number)]
        #Print the trees
        vertices,count,tree = g_inv.print_tree()
        print(count)
        
        for i in range(vertices_number - count):
            tree.pop()
            
        str_list = [['{0}'.format(flt) for flt in sublist] for sublist in tree]
        str_display = '\n'.join(' '.join(lst) for lst in str_list)
        print(str_display)

if __name__ == '__main__':
    main()
