/*
** server.c -- a datagram sockets "server"
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
#define FILENAME "database.txt"
#define SUB_NUM 3

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
} buffer = {0xFFFF, 0x01, 0x0, 0x1, 0xE, 0x0, 0x0, 0xFFFF};

struct database{
  unsigned int subscriber_no;
  int technology;
  int paid;
} subscribers[SUB_NUM];

//Recieving buffer
char buf[MAXBUFLEN];

struct addrinfo hints, *servinfo, *p;
struct sockaddr_storage their_addr;
socklen_t addr_len;
char s[INET6_ADDRSTRLEN];

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void recieving(int);
void readingdatabase(char *);
void check_ack(int);

int main(void)
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    struct sockaddr_storage their_addr;
    char *filename;

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

    filename = FILENAME;
    readingdatabase(filename);

    while (1) {

      recieving(sockfd);
      check_ack(sockfd);
    }

    close(sockfd);
    return 0;
}

// Reading database
void readingdatabase(char *filename)
{
  FILE * pFile;
  char buf[MAXBUFLEN];
  char *data[3];
  char *number_str1;
  char number_str2[20];
  int i = 0;

  //Openning database file
  pFile = fopen (filename, "r");
  if (pFile == NULL){
    perror ("Error opening file");
    exit(-1);
  }

  while (fgets(buf,MAXBUFLEN,pFile) != NULL){

    number_str1 = strtok(buf, "\t");
    subscribers[i].technology = atoi(strtok(NULL, "\t"));
    subscribers[i].paid = atoi(strtok (NULL, "\t"));

    data[0] = strtok(number_str1, "-");
    data[1] = strtok(NULL, " -");
    data[2] = strtok (NULL, " -");

    bzero(number_str2, sizeof(number_str2));
    strcat(number_str2, data[0]);
    strcat(number_str2, data[1]);
    strcat(number_str2, data[2]);
    subscribers[i].subscriber_no = atoi(number_str2);
    //printf("numbers is %u \n", subscribers[i].subscriber_no);
    //printf("technology is %d \n", subscribers[i].technology);
    //printf("paid sign is %d \n", subscribers[i].paid);
    bzero(buf, MAXBUFLEN);
    i ++;
  }
  fclose (pFile);
}

//Recieving packets from the client
void recieving(int sockfd)
{
  int numbytes;
	bzero(buf, MAXBUFLEN);
	memset(&buffer, 0, sizeof(buffer));
	numbytes = recvfrom(sockfd, buf, MAXBUFLEN - 1 , 0,(struct sockaddr *)&their_addr, &addr_len);

  if (numbytes == -1) {
		perror("recvfrom error!\n");
		exit(1);
	}

  printf("Recieved %d bytes.\n", numbytes);

	memcpy(&buffer, buf, sizeof(buffer));
  printf("The subscriber number is %u \n", buffer.subscriber_no);
  printf("The technology is %d \n", buffer.technology);
  //printf("End of packet is %#X \n", buffer.end);

}

//Checking the subscriber state in the database and sending ack to the client
void check_ack(int sockfd) {

  int i = 0;
  int numbytes;
  buffer.type = 0xFFFA;

  for (i = 0; i < SUB_NUM; i++) {
    if (buffer.subscriber_no == subscribers[i].subscriber_no) {

      buffer.technology = subscribers[i].technology;
      printf("Subscriber technology type in the database is - %#X \n\n", subscribers[i].technology);

      if (subscribers[i].paid == 1) {
        buffer.type = 0xFFFB;
        printf("Subscriber permitted to access the network! \n\n");
      }
      else {
        buffer.type = 0xFFF9;
        printf("Reject: Subscriber has not paid \n\n");
      }
    }
  }

  if (buffer.type == 0xFFFA) {
    printf("Reject: Subscriber does not exist on database. \n\n");
  }

  bzero(buf, MAXBUFLEN);
  memcpy(buf, &buffer, sizeof(buffer));

  numbytes = sendto(sockfd, buf, MAXBUFLEN - 1 , 0,(struct sockaddr *)&their_addr, addr_len);
  if (numbytes == -1) {
		perror("send error!");
		exit(1);
	}
}
