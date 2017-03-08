/*
** client.c -- a datagram "client" 
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
#define HOST "127.0.0.1"

//The packet format
struct packet{

	unsigned short int start;
	unsigned short int client_id:8;
	unsigned short int type;
	unsigned short int segment_no:8;
	unsigned short int length:8;
	unsigned short int technology:8;
	unsigned int subscriber_no;
	unsigned short int end;
} buf = {0xFFFF, 0x01, 0x0, 0x1, 0xE, 0x0, 0x0, 0xFFFF};

//Sending buffer
char buffer[BUF_SIZE];

//Recieving buffer
char recieving[BUF_SIZE];

struct addrinfo hints, *servinfo, *p;

void sending(int, int, int, unsigned int);


int main(int argc, char *argv[])
{
	int sockfd;
  int rv;
  int numbytes;
	int packet_num = 1;
	struct timeval timeout;
	int count;
	char sub_no[20];
	char tech[20];
	unsigned int subscriber_no;
	int technology;

  timeout.tv_sec  = 3;
  timeout.tv_usec = 0;

	/*if (argc != 3) {
	        fprintf(stderr,"Please add subscriber number and technology type. \n");
	        exit(1);
	    }*/

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;

  if ((rv = getaddrinfo(HOST, SERVERPORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return -1;
	}

    // loop through all the results and make a socket
    for(p = servinfo; p != NULL; p = p->ai_next) {

        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {

            perror("talker: socket");
            continue;
        }

        break;
    }

    if (p == NULL) {

        fprintf(stderr, "talker: failed to create socket\n");
        return -1;
    }

	//Set Recv timer
	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));

	// Sending pakets to the server
	while (1) {
		count = 0;
		rv = -1;
		//subscriber_no = atoi(argv[1]);
		//technology = atoi(argv[2]);
		printf("Please input the subscriber number: \n");
		scanf("%s",sub_no);

		printf("Please input the technology type: \n");
		scanf("%s",tech);
		subscriber_no = atoi(sub_no);
		technology = atoi(tech);

		sending(sockfd, packet_num, technology, subscriber_no);
		memset(recieving,0,BUF_SIZE);

		//Client is waiting for the ACK message, the ack_timer is set to be 3 secs,
		//the client will resent 3 times if the timer is timeout
		while (count < 4 && rv < 0){

			rv = recvfrom(sockfd, recieving, BUF_SIZE-1, MSG_WAITALL, p->ai_addr, &p->ai_addrlen);
	    	if(rv == -1 && errno == EAGAIN){

				if(count == 3){
					printf("\nServer does not Respond.\n\n");
					return -1;
				}
				count ++;
				printf("\nRecv timeout. Resend for the %d time.\n\n", count);
				sending(sockfd, packet_num, technology, subscriber_no);
	      	}
		}

		packet_num ++;
		memcpy(&buf, recieving, sizeof(buf));
		printf("packet ack type \"%#X\"\n", buf.type);

		if (buf.type == 0xFFFA)
			printf("Reject: Subscriber does not exist on database - %#X \n\n", buf.type);

		else if (buf.type == 0xFFF9)
			printf("Reject: Subscriber has not paid - %#X \n\n", buf.type);

		else if (buf.type == 0xFFFB){
			printf("Subscriber permitted to access the network! \n");
			printf("The technology type is %d \n\n", buf.technology);
		}
	}
    freeaddrinfo(servinfo);
    close(sockfd);

    return 0;
}


void sending(int sockfd, int number, int technology, unsigned int subscriber_no)
{

	int numbytes;
	printf("This is packet number %d.\n", number);

	//Setting the sending buffer
	buf.type = 0XFFF8;
	buf.segment_no = number;
	buf.technology = technology;
	buf.subscriber_no = subscriber_no;

	memset(buffer, 0, BUF_SIZE);
	memcpy(buffer, &buf, sizeof(buf));

	printf("The subscriber number is %u \n", buf.subscriber_no);
  printf("The technology is %d \n", buf.technology);

	numbytes = sendto(sockfd, buffer, sizeof(buf), 0, p->ai_addr, p->ai_addrlen);

	if (numbytes == -1) {
		perror("send error!");
		exit(1);
	}

	printf("talker: sent %d bytes\n\n", numbytes);
}
