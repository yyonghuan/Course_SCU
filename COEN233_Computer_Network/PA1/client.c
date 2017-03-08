/*
** client.c -- a datagram "client" demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <sys/time.h>


#define SERVERPORT "8888"    // the port users will be connecting to
#define BUF_SIZE 300
#define PACKET_NUM 5
#define HOST "127.0.0.1"

//The first 7 bytes of the packet
struct packet{
	
    unsigned short int start;
	unsigned short int client_id:8;
	unsigned short int type;
	unsigned short int segment_no:8;
	unsigned short int length:8;		
} buf = {0xFFFF, 0x01, 0xFFF1, 0x0, 0xFF};

struct error_packet{
    unsigned short int start;
	unsigned short int client_id:8;
	unsigned short int type;
	unsigned short int code;
	unsigned short int segment_no:8;
	unsigned short int end;
} error_packet = {0xFFFF, 0x0, 0xFFF3, 0x0, 0x0, 0xFFFF};

//The ending 2 bytes of the packet
struct endsign{
	unsigned short int end;
} end = {0xFFFF};

//Sending buffer
char buffer[BUF_SIZE];

//Recieving buffer
char recieving[BUF_SIZE];

//Payload buffer
char payload[256];
struct addrinfo hints, *servinfo, *p;
unsigned short int error_code;
unsigned short int reject_code;



void sending(int, int, int);



int main(int argc, char *argv[])
{
    int sockfd;
    int rv;
    int numbytes;
	int packet_num = 0;
	FILE * pFile;
	struct timeval timeout;
	int count;
	int code;
	
    timeout.tv_sec  = 3;
    timeout.tv_usec = 0;
	
	if (argc != 2) {
	        fprintf(stderr,"Please add Error sub code\n");
	        exit(1);
	    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
	
    if ((rv = getaddrinfo(HOST, SERVERPORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and make a socket
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("talker: socket");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "talker: failed to create socket\n");
		
        return 2;
    }
	
	//Set Recv timer
	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
	
	// Open the sending file
	pFile = fopen ("test1.txt" , "r");
	
	// Open file error processing
	if (pFile == NULL){
		perror ("Error opening file");
		return -1;
	}
	
	// Sending 5 pakets to the server
	for (packet_num = 0; packet_num < PACKET_NUM; packet_num++){
		bzero(payload, 255);
		fgets (payload , 256, pFile);
		code = atoi(argv[1]);
		sending(code, sockfd, packet_num + 1);
		
		count = 0;
		rv = -1;
		memset(recieving,0,BUF_SIZE);
		
		//Client is waiting for the ACK message, the ack_timer is set to be 3 secs, the client will resent 3 times if the timer is timeout
		while (count < 4 && rv < 0){
			
			rv = recvfrom(sockfd, recieving, BUF_SIZE-1, MSG_WAITALL, p->ai_addr, &p->ai_addrlen);
	    	if(rv == -1 && errno == EAGAIN){
				
				if(count == 3){
					printf("\nServer does not Respond.\n\n");
					return -1;
				}
				count ++;
				printf("\nRecv timeout. Resend for the %d time.\n\n", count);
				sending(code, sockfd, packet_num + 1);
	      	}
		}
		
		memcpy(&buf, recieving, 5);
		printf("packet type \"%#X\"\n", buf.type);
		if ( buf.type == 0xFFF3) {
			memcpy(&error_packet, recieving, sizeof(error_packet));
			
			if (error_packet.code == 0xFFF4)
				printf("Reject Error: Packet out of sequence - %#X \n", error_packet.code);
			
			if (error_packet.code == 0xFFF5)
				printf("Reject Error: Packet length mismatch - %#X \n", error_packet.code);
			
			else if (error_packet.code == 0xFFF6)
				printf("Reject Error: End of packet missing - %#X \n", error_packet.code);
			
			else if (error_packet.code == 0xFFF7)
				printf("Reject Error: Duplicate packet - %#X \n", error_packet.code);
			return -1;
		}
		
	}
    
	fclose (pFile);
    freeaddrinfo(servinfo);
    close(sockfd);

    return 0;
}


void sending(int code, int sockfd, int number) {
	int numbytes;
	
	//Reading max 255 bytes one time
	buf.type = 0XFFF1;
	buf.segment_no = number;
	
	//code == 0, normal case
	if (code == 0) {
		buf.length = strlen(payload);
	}
	
	//code == 1, out of sequence packet
	if (code == 1) {
		buf.segment_no = number*number;
	}
	
	//code == 2, length mismatch
	if (code == 2) {
		buf.length = 100;
	}
	
	//if code == 4, send duplicate packet - same sequence number
	if (code == 4) {
		buf.segment_no = 1;
	}
	
	memset(buffer,0,BUF_SIZE);
	memcpy(buffer,&buf,sizeof(buf));
	memcpy(buffer + sizeof(buf), payload, strlen(payload));
	
	//if code == 3, do not add 0xFFFF at the end of packet
	if (code != 3) {
		memcpy(buffer + sizeof(buf) + strlen(payload), &end, 2);
	}
	
	printf("This is packet number %d.\n", buf.segment_no);
	/*
	printf("start \"%#X\"\n", buf.start);
	printf("client Id \"%#X\"\n", buf.client_id);
	printf("packet type \"%#X\"\n", buf.type);	
	printf("length of packet: %hu\n",buf.length);
	printf("segment number of packet: \"%#X\"\n", buf.segment_no);
	printf("end \"%#X\"\n", end.end);*/

	numbytes = sendto(sockfd, buffer, sizeof(buf) + strlen(payload) + 2, 0, p->ai_addr, p->ai_addrlen);
	
	if (numbytes == -1) {
		perror("send error!");
		exit(1);
	}
	
	printf("talker: sent %d bytes\n\n", numbytes);
	
	
}