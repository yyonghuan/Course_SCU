# -*- coding: utf-8 -*-
"""
Created on Tue Mar  7 11:31:51 2017

@author: Vergil
"""

import paramiko
import sys

'''
if sys.argv[1] == '--help':
    print('first argument: IP address')
    print('seocnd argument: username')
    print('third argument: password')
    print('fourth argument: localpath')
    print('fifth argument: remotepath')
    sys.exit()

ip = sys.argv[1]
username = sys.argv[2]
password=sys.argv[3]
localpath = sys.argv[4]
remotepath = sys.argv[5]
'''
def putfile(ip, username, password, localpath, remotepath):

    #put file
    t = paramiko.Transport((ip, 22))
    try:
        t.connect(username = username, password = password)
        sftp = paramiko.SFTPClient.from_transport(t)
        sftp.put(localpath,remotepath)
        print('Put file succeedded')
    except paramiko.AuthenticationException:
        print('Authentication error')
        quit()
    except FileNotFoundError:
        print('File not Found!')
    except:
        print('Connection failed')
        quit()
    t.close()

if __name__ == "__main__":
    files = ['client_1.py', 'client_2.py', 'client_3.py', 'client_4.py',
             'client_5.py', 'client_6.py', 'client_7.py', 'client_8.py',
             'client_9.py', 'client_10.py']
    ip = "127.0.0.1"
    username = input("Please input the username of this PC")
    password = input("Please input the password of this PC")
    path = input("Please input the path of the source folder")
    localpath = path + "/client1/"
    remotepath = path + "/client/"

    for f in files:
        path_l = localpath + f
        path_r = remotepath + f
        putfile(ip, username, password, path_l, path_r)
