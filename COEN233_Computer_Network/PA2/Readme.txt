1 Files included:
	Source code:
		client2.c -- The client sends packet to the server;
		server2.c -- The server receives packet from the client and sends ack packet to the client;
	Database file:
		database.txt
	Readme.txt

2 Compile command
	gcc -o client2 client2.c
	gcc -o server2 server2.c

3 How to run the program
	Run the server: ./server2
	Run the client:
    ./client2
    Please input the subscriber number:
    XXXXXXXX
    Please input the technology type:
    X

4 Testing cases
 a. Ack timeout
    Run the client2 program only.

 b. Subscriber permitted to access the network.
    Run the server2 program first;
    Run the client2 program;
    Input the subscriber number on the database which has paid.

 c. Subscriber does not exist on the database.
   Run the server2 program first;
   Run the client2 program;
   Input the subscriber number does not exist on the database.

 d. Subscriber has not paid.
   Run the server2 program first;
   Run the client2 program;
   Input the subscriber number on the database which has not paid.
