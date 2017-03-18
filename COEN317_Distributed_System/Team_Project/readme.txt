
We use five terminals simulated as five computers: one is considered as client machine, two are masters and two are testing machines. There will be one master machine as the standby master, which will run as the master when detects the master is down.
The client machine run a script program to send ten files which are ready to be tested. After receiving the ten testing files, the master will dispatch the testing jobs to the two slaves. The slaves run the testing job independently and send the results to the master. After receiving the testing results, the master will push the result file to the specific client.

1 setup of the PC, we using MAC(macOS Sierra 10.12.3)

#permit ssh port
sudo systemsetup -setremotelogin on

#install python3.5 with brew
xcode-select --install
brew install python3

sudo mv /usr/bin/python /usr/bin/python2
Password:
which python3
/usr/local/bin/python3
ln -s python /usr/local/bin/python3
ln: /usr/local/bin/python3: File exists
ln -s /usr/local/bin/python3 /usr/bin/python
ln: /usr/bin/python: Permission denied
sudo ln -s /usr/local/bin/python3 /usr/bin/python

#install pip
apt install python3-pip
pip3 install --upgrade pip

#install paramiko
apt-get install libssl-dev
pip3 install paramiko

#install watchdog
pip3 install watchdog

2 Including Files

readme.txt
job.py - Job class
slave.py - For testing machines, testing machine fetch files from server and create a result file
master.py - For server
putfile.py - For client machine to upload ten testing files or using bash
getfile.py
unfinishedqueue.py - creating log file for masters
client1 - folders with testing files
unfinishedqueue.txt - log file for masters

3 How to run

Unzip the source file
Terminal1: run master.py [python3 master.py]
Terminal2: run master.py [python3 master.py]
Terminal3: run slave.py input 1 [python3 slave.py]
Terminal4: run slave.py input 2 [python3 slave.py]
Terminal5: run putfile.py [python3 putfile.py]

After terminal5 is done, use "control + C" shut terminal1 down.
