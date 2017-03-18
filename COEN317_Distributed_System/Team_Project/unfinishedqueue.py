# -*- coding: utf-8 -*-
"""
Created on Thu Mar  9 11:10:47 2017

@author: yangyonghuan
"""

def unfinishedqueue(putorget,job=' '):
    if putorget == 'put':
        with open ('unfinishedqueue.txt','a+') as file:
            file.write(job + '\n')
    elif putorget == 'get':
        with open ('unfinishedqueue.txt','r') as file:
            data = file.read().splitlines(True)
        index = data.index(job + '\n')
        data = data[:index] + data[index+1:]
        with open ('unfinishedqueue.txt','w') as file:
            for element in data:
                file.write(element)
    else:
        print('Input error')

if __name__ == '__main__':
    unfinishedqueue('put', 'client_1')
    unfinishedqueue('put', 'client_2')
    unfinishedqueue('put', 'client_3')
    unfinishedqueue('get', 'client_2')