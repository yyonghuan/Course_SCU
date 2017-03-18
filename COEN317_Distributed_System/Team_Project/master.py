# -*- coding: utf-8 -*-
"""
Created on Thu Mar  11 11:11:37 2017

@author: yangyonghuan
"""

import time
import sys
import queue
from multiprocessing.managers import BaseManager
from job import Job
from watchdog.observers import Observer
from watchdog.events import PatternMatchingEventHandler
from putfile import *
from getfile import *
from unfinishedqueue import *

username = input("Please input the username of this PC")
password = input("Please input the password of this PC")
path = input("Please input the path of the source folder")
localpath = path + "/result/"
remotepath = path + "/client1/"

class MyHandler(PatternMatchingEventHandler):
#    patterns = ["*.xml", "*.lxml"]

    def process(self, event):
        """
        event.event_type
            'modified' | 'created' | 'moved' | 'deleted'
        event.is_directory
            True | False
        event.src_path
            path/to/observed/file
        """
        global job_count
        # the file will be processed there
        print(event.src_path, event.event_type)  # print now only for degug
        temp = event.src_path.split('/')

        #if event.event_type == 'created':
        if len(temp) == 3:
            #temp = event.src_path.split('/')
            if temp[-2] == 'client':

                job_count += 1
                job = Job()
                job.job_id = job_count
                job.filename = temp[-1]
                job.client_id = int(job.filename.split('.')[0].split('_')[-1])

                send.put(job)
                unfinishedqueue('put', job.filename)
                print("put job %s in the queue" % job.filename)

            if temp[-2] == 'result':
                job = Job()
                job = receive.get(True)

                ip = "127.0.0.1"
                localpath += job.result
                remotepath += job.result
                putfile(ip, username, password, localpath, remotepath)
                unfinishedqueue('get', job.filename)
                print("Finish job: %s" % job.filename)
                print("Result file name: %s" % job.result)



    def on_modified(self, event):
        self.process(event)

    def on_created(self, event):
        self.process(event)

class Master(BaseManager):
    pass

class Slave(BaseManager):
    pass

try:
    Slave.register("send_msg")
    Slave.register("receive_msg")

    server = '127.0.0.1'
    print('Connect to server %s...' % server)
    slave = Slave(address=(server, 9000), authkey = b"test")
    print("I am the standby master!")

    #slave.connect()
    #send = slave.send_msg()
    #recieve = slave.receive_msg()
    while True:
        slave.connect()
        time.sleep(1)

except KeyboardInterrupt:
    print("Key boardInterrupt")

except Exception:
    print("Multiprocess Connect Failed! Master1 is down!")


dispatched_job_queue = queue.Queue()
finished_job_queue = queue.Queue()
job_count = 0

#Register to the network
Master.register("send_msg", callable=lambda :dispatched_job_queue)
Master.register("receive_msg", callable=lambda :finished_job_queue)

#Listen to port 9000
master = Master(address=("",9000), authkey=b"test")

#start
master.start()
send = master.send_msg()
receive = master.receive_msg()
print("I am the master now.")

# Monitor Client uploading file folder and Testing result folder
path1 = "./client"
path2 = "./result"

observer1 = Observer()
observer1.schedule(MyHandler(), path1)
observer1.start()

#observe the changes of the test machine folder
observer2 = Observer()
observer2.schedule(MyHandler(), path2)
observer2.start()

#Reading in the log file
with open ('unfinishedqueue.txt','r') as file:
    data = file.read().splitlines(True)
print("open unfinishedqueue.txt")
if len(data) == 0:
    print("No unfinished jobs.")
else:
    print(data)
for elment in data:
    job_count += 1
    job = Job()
    job.job_id = job_count
    job.filename = elment[:-1]
    job.client_id = int(job.filename.split('.')[0].split('_')[-1])
    send.put(job)
    print("put unfinished job  %s into the queue" % job.filename)

try:
    while True:
        time.sleep(1)
except KeyboardInterrupt:
    observer1.stop()
    observer2.stop()

observer1.join()
observer2.join()
