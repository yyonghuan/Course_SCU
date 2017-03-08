Name: Yonghuan Yang
Assignment Name: Programming Assignment 1
Date:Feb,8,2017

Project Description: The goal of this programming assignment is to build a functional web server. 
A web server listens for the connections on a socket. Clients connect to this socket and use a simple
text-based protocol to retrive files from the server. The web server is implemented in C language.

List of submitted files:
	1. sorce code: server.c
	2. Readme.txt
	3. Script file for more explanation of the project: scritp.docx
	
	
Program running instructions:
	source file: server.c
	compile command: gcc -o server server.c
	running command: ./server -document_root "server files directory path" -port 8888
	
	Client Side: two options
		1. telnet 127.0.0.1 8888
		   GET / HTTP/1.0
		2. Use a browser as the client, input 127.0.0.1:8888 in the address field. The home page of
		   Santa Clara University will pop out. And you can retrive other files of the server.

Other info:
	Make sure that your computer has saved the home page of SantaClaraUniversity website and you do have
	an defalt index.html file for GET / HTTP/1.0 request.
		