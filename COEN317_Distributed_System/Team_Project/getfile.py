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
    print('fourth argument: remotepath')
    print('fifth argument: localpath')
    sys.exit()

ip = sys.argv[1]
username = sys.argv[2]
password=sys.argv[3]
remotepath = sys.argv[4]
localpath = sys.argv[5]
'''

def getfile(ip, username, password, localpath, remotepath):

    #put file
    t = paramiko.Transport((ip, 22))
    try:
        t.connect(username = username, password = password)
        sftp = paramiko.SFTPClient.from_transport(t)
        sftp.get(remotepath,localpath)
        print('Get file succeedded')
    except paramiko.AuthenticationException:
        print('Authentication error')
        quit()
    except FileNotFoundError:
        print('File not Found!')
    except:
        print('Paramiko Connection Failed')
        quit()
    t.close()


if __name__ == '__main__':
    ip = "127.0.0.1"
    username = input("Please input the username of this PC")
    password = input("Please input the password of this PC")
    path = input("Please input the path of the source folder")
    localpath = path + "/test1/test1.txt"
    remotepath = path + "/client/test1.txt"
    getfile(ip, username, password, localpath, remotepath)
