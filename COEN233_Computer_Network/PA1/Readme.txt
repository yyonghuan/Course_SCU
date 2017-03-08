1 Files included:
	Source code: 
		client.c -- The client sends packet to the server;
		server.c -- The server recieves packet from the client and sends ack packet to the client;
	Testing file:
		test1.txt -- sending file for testing the program
	Readme.txt

2 Compile command
	gcc -o client client.c
	gcc -o server server.c

3 How to run the program
	Run the server: ./server
	Run the client: ./client + error code(0/1/2/3/4)
4 Testing steps
	Normal sending: Client will send 5 packets to the server. The payload is reading from the test1.txt. The maximum bytes of the payload is 255bytes each time.
	
	./server
	./client 0
	
	Result: Snapshot 0
	
	ACK timeout test: Do not run the server, client begins to send packets to the server. The client will try to resend 3 times if the server does not respond ack
					  message for 3 seconds. After 3 times trying, the client prints out: "Server does not respond".
	Result: Snapshot 5
	
	Error code testing:
	0xFFF4: reject out of sequence
		./server
		./client 1 //Client sends packet number 1 first, and then send packet number 4.
		Result: Snapshot 1
	
	0xFFF5: reject length mismatch
		./server
		./client 2 //Client sends packet with wrong packet length.
		Result: Snapshot 2
		
	0xFFF6: End of packet missing
		./server
		./client 3 //Client sends packet without end of packet 0xFFFF.
		Result: Snapshot 3
		
	0xFFF7: duplicate packet(same sequence number with the last packet)
		./server
		./client 4 //Client sends packet number 1 first, and then send packet number 1 again.
		Result: Snapshot 4
		