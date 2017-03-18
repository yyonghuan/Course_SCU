# -*- coding: utf-8 -*-
"""
Created on Thu Mar  9 11:11:37 2017

@author: yangyonghuan
"""

import time
import queue
from multiprocessing.managers import BaseManager
from subprocess import PIPE, run
from job import Job
from getfile import *
from putfile import *

class Slave(BaseManager):
    pass


tm_number = int(input('Please input the testing machine number:'))
reconnect_count = 0

Slave.register("send_msg")
Slave.register("receive_msg")

server = '127.0.0.1'
print('Connect to server %s...' % server)
slave = Slave(address=(server, 9000), authkey = b"test")

username = input("Please input the username of this PC")
password = input("Please input the password of this PC")
path = input("Please input the path of the source folder")

while True:

    try:
        slave.connect()
        send = slave.send_msg()
        recieve = slave.receive_msg()
        print("Connected to the master now!")

        while True:
            reconnect_count = 0
            job = send.get(True)
            job.test_machine = tm_number
            print('Run job: %s ' % job.job_id)

            ip = "127.0.0.1"
            localpath = path + "/test"
            localpath += str(job.test_machine) + '/'
            job.result = job.filename.split('.')[0] + '_' + str(job.test_machine) + '.txt'

            result_file = open(localpath + job.result, 'w')

            localpath += job.filename
            remotepath = path + "/client/"
            remotepath += job.filename
            getfile(ip, username, password, localpath, remotepath)

            #Run the test case
            command = ['python %s -v' %localpath]
            output = result = run(command, stdout=PIPE, stderr=PIPE,shell=True)
            result_file.write(output.stderr.decode())

            time.sleep(2)
            result_file.close()
            recieve.put(job)
            localpath += '/test'
            localpath += str(job.test_machine) + '/'
            remotepath = path + '/result/'
            localpath += job.result
            remotepath += job.result
            putfile(ip, username, password, localpath, remotepath)

    except KeyboardInterrupt:
        print("Interrupt by the KeyboardInterrupt!")
        break
    except Exception:
        print("Multiprocess Connect Failed! Reconnect after 2 second...")
        time.sleep(2)
        reconnect_count += 1
        print("Reconnect for the %s time" %reconnect_count)
        if reconnect_count == 10:
            break
