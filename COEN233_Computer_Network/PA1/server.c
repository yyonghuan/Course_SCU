/*
** server.c -- a datagram sockets "server" demo
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

#define MYPORT "8888"    // the port users will be connecting to
#define MAXBUFLEN 300
#define PACKET_NUM 5

//Error ACK packet
struct error_packet{
    unsigned short int start;
	unsigned short int client_id:8;
	unsigned short int type;
	unsigned short int code;
	unsigned short int segment_no:8;
	unsigned short int end;
} error_packet = {0xFFFF, 0x0, 0xFFF3, 0x0, 0x0, 0xFFFF};


//The first 7 bytes of the packet
struct packet{

    unsigned short int start;
	unsigned short int client_id:8;
	unsigned short int type;
	unsigned short int segment_no:8;
	unsigned short int length:8;
} buffer = {0x0, 0x0, 0x0, 0x0, 0x00};

//The ending 2 bytes of the packet
struct endsign{
	unsigned short int end;
} end = {0x0};

//Payload buffers
char payload[256];

//Recieving buffer
char buf[MAXBUFLEN];

struct addrinfo hints, *servinfo, *p;
struct sockaddr_storage their_addr;
socklen_t addr_len;
char s[INET6_ADDRSTRLEN];
int segment_no_before = 0;

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void recieving(int);

int main(void)
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    struct sockaddr_storage their_addr;
	int packet_num;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, MYPORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return -1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("listener: socket");
            continue;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("listener: bind");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "listener: failed to bind socket\n");
        return 2;
    }

    freeaddrinfo(servinfo);
    printf("listener: waiting to recvfrom...\n");
    addr_len = sizeof their_addr;

	for (packet_num = 0; packet_num < PACKET_NUM; packet_num++) {
		recieving(sockfd);
	}

    close(sockfd);
    return 0;
}

void recieving(int sockfd){

	int numbytes;
	bzero(buf, MAXBUFLEN);
	bzero(payload, 256);
	memset(&buffer, 0, sizeof(buffer));
	numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,(struct sockaddr *)&their_addr, &addr_len);
	if (numbytes == -1) {
		perror("recvfrom error!\n");
		exit(1);
	}

	memcpy(&buffer, buf, sizeof(buffer));
	memcpy(payload, buf + sizeof(buffer), numbytes - sizeof(buffer) - 2);
	payload[strlen(payload)] = '\0';
	memcpy(&end, buf + numbytes - 2, 2);

  printf("listener: got packet from %s\n",inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),s, sizeof s));
  printf("listener: packet is %d bytes long\n", numbytes);
	printf("This is packet number %d \n", buffer.segment_no);


	printf("start \"%#X\"\n", buffer.start);
	printf("client Id \"%#X\"\n", buffer.client_id);
	printf("packet type \"%#X\"\n", buffer.type);
	printf("segment number \"%d\"\n", buffer.segment_no);
	printf("length of payload \"%hu\"\n", buffer.length);
	printf("Real length of payload \"%lu\"\n", strlen(payload));
	printf("Payload: \"%s\"\n", payload);
	printf("Packet Ending  \"%#X\"\n", end.end);

	//ACK sending
	bzero(buf, MAXBUFLEN);

	if (buffer.length != strlen(payload)) {

		printf("Packet with different length.\n");
		error_packet.code = 0xFFF5;
		error_packet.client_id = buffer.client_id;
		error_packet.segment_no = buffer.segment_no;
		memcpy(buf, &error_packet, sizeof(error_packet));
	}
	else if (end.end != 0xFFFF) {

		printf("Packet without ending.\n\n");
		error_packet.code = 0xFFF6;
		error_packet.client_id = buffer.client_id;
		error_packet.segment_no = buffer.segment_no;
		memcpy(buf, &error_packet, sizeof(error_packet));
	}
	else if (segment_no_before == buffer.segment_no) {

		printf("Duplicate packets!\n\n");
		error_packet.code = 0xFFF7;
		error_packet.client_id = buffer.client_id;
		error_packet.segment_no = buffer.segment_no;
		memcpy(buf, &error_packet, sizeof(error_packet));
	}
	else if (segment_no_before + 1 != buffer.segment_no) {

		printf("Packet out of sequence!\n\n");
		error_packet.code = 0xFFF4;
		error_packet.client_id = buffer.client_id;
		error_packet.segment_no = buffer.segment_no;
		memcpy(buf, &error_packet, sizeof(error_packet));
	}
	else {

		buffer.type = 0xFFF2;
		memcpy(buf, &buffer, sizeof(buffer) - 1);
		end.end = 0xFFFF;
		memcpy(buf + sizeof(buffer), &end, 2);
	}

	segment_no_before = buffer.segment_no;

	numbytes = sendto(sockfd, buf, sizeof(buffer)+1, 0, (struct sockaddr *)&their_addr, addr_len);
	if (numbytes == -1) {
		perror("ack send error!\n");
		exit(1);
	}

}
