#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <fcntl.h>
#include <pthread.h>


#define SERVER_PORT  8888
#define BUF_SIZE 1024
#define REQUEST_NUM 5
#define SERVER_STRING "Server: SantaClaraUniversity/0.1.0\r\n"

#define TRUE             1
#define FALSE            0

char   buffer[BUF_SIZE];
char current_time[50];
char server_path[500];

void getTime();
void respond(int);
int getFileSize(char *);
void headers(int, char *);
void not_found(int);
void bad_request(int);
void forbidden(int);


int main (int argc, char *argv[])
{
   int    i, len, rc, on = 1;
   int port = SERVER_PORT;
   int    listen_sd, max_sd, new_sd;
   int    desc_ready, end_server = FALSE;
   int    close_conn;
   int request_num;
   struct sockaddr_in   addr;
   struct timeval       timeout;
   struct fd_set        master_set, working_set;
   
   if (argc != 5) {
	   fprintf(stderr, "Parameter numbers ERROR\n");
	   exit(1);
   }
   
   for(i=0; i<5;i++){
	   if (strcmp(argv[i], "-document_root") == 0){
	   strcpy(server_path, argv[i+1]);
       }
	   
	   if (strcmp(argv[i], "-port") == 0){
   		port = atoi(argv[i+1]);
       }
   }
   

   listen_sd = socket(AF_INET, SOCK_STREAM, 0);
   if (listen_sd < 0)
   {
      perror("socket() failed");
      exit(-1);
   }

   rc = setsockopt(listen_sd, SOL_SOCKET,  SO_REUSEADDR,
                   (char *)&on, sizeof(on));
   if (rc < 0)
   {
      perror("setsockopt() failed");
      close(listen_sd);
      exit(-1);
   }

   rc = ioctl(listen_sd, FIONBIO, (char *)&on);
   if (rc < 0)
   {
      perror("ioctl() failed");
      close(listen_sd);
      exit(-1);
   }

   memset(&addr, 0, sizeof(addr));
   addr.sin_family      = AF_INET;
   addr.sin_addr.s_addr = htonl(INADDR_ANY);
   addr.sin_port        = htons(port);
   rc = bind(listen_sd,
             (struct sockaddr *)&addr, sizeof(addr));
   if (rc < 0)
   {
      perror("bind() failed");
      close(listen_sd);
      exit(-1);
   }

   rc = listen(listen_sd, 32);
   if (rc < 0)
   {
      perror("listen() failed");
      close(listen_sd);
      exit(-1);
   }

   FD_ZERO(&master_set);
   max_sd = listen_sd;
   FD_SET(listen_sd, &master_set);

   timeout.tv_sec  = 3 * 60;
   timeout.tv_usec = 0;

   do
   {
      memcpy(&working_set, &master_set, sizeof(master_set));

      /**********************************************************/
      /* Call select() and wait 5 minutes for it to complete.   */
      /**********************************************************/
      printf("Waiting on select()...\n");
      rc = select(max_sd + 1, &working_set, NULL, NULL, &timeout);

      if (rc < 0)
      {
         perror("  select() failed");
         break;
      }

      if (rc == 0)
      {
         printf("  select() timed out.  End program.\n");
		 end_server = FALSE;
         break;
      }

      desc_ready = rc;
      for (i=0; i <= max_sd  &&  desc_ready > 0; ++i)
      {
         if (FD_ISSET(i, &working_set))
         {

            desc_ready -= 1;

            if (i == listen_sd)
            {
               printf("  Listening socket is readable\n");

               do
               {

                  new_sd = accept(listen_sd, NULL, NULL);
                  if (new_sd < 0)
                  {
                     if (errno != EWOULDBLOCK)
                     {
                        perror("  accept() failed");
                        end_server = TRUE;
                     }
                     break;
                  }

                  printf("  New incoming connection - %d\n", new_sd);
                  FD_SET(new_sd, &master_set);
                  if (new_sd > max_sd)
                     max_sd = new_sd;

               } while (new_sd != -1);
            }

            else
            {
               printf("  Descriptor %d is readable\n", i);
               close_conn = FALSE;
               do
               {
                  bzero(buffer, BUF_SIZE);
				  rc = read(i, buffer, sizeof(buffer));
                  if (rc < 0)
                  {
                     if (errno != EWOULDBLOCK)
                     {
                        perror("  recv() failed");
                        close_conn = TRUE;
                     }
                     break;
                  }

                  if (rc == 0)
                  {
                     printf("  Connection closed\n");
                     close_conn = TRUE;
                     break;
                  }

                  len = rc;
				  printf("The request of client %d is recieved\n", i);
				  printf("The message is: %s\n", buffer);
                  
				  respond(i);

               } while (TRUE);

               if (close_conn)
               {
                  close(i);
                  FD_CLR(i, &master_set);
                  if (i == max_sd)
                  {
                     while (FD_ISSET(max_sd, &master_set) == FALSE)
                        max_sd -= 1;
                  }
               }
            } /* End of existing connection is readable */
         } /* End of if (FD_ISSET(i, &working_set)) */
      } /* End of loop through selectable descriptors */

   } while (end_server == FALSE);

   for (i=0; i <= max_sd; ++i)
   {
      if (FD_ISSET(i, &master_set))
         close(i);
   }
}

void respond(int fd_num){
	
	//int rc;
	char *reqline[3];
	char path[1024];
	int fd;
	int bytes_read;
	char temp[256];
	int length;
    struct stat buf;
	int r;
	
	strcpy(path, server_path);
	
	reqline[0] = strtok(buffer, " \t\n");
	
	
	
	if (strncmp(reqline[0], "GET\0", 4) != 0)
	{
		write(fd_num, "HTTP/1.0 400 Bad Request!", 25);
	}
	
	else 
	{
		
		reqline[1] = strtok(NULL, " \t");
        reqline[2] = strtok (NULL, " \t\n");
		
        if ( (strncmp( reqline[2], "HTTP/1.0", 8)!=0 && strncmp( reqline[2], "HTTP/1.1", 8)!=0))
        {
			//bad_request(fd_num);
			write(fd_num, "HTTP/1.0 400 Bad Request!", 25);
        }
        else
		{
			
            if ( strncmp(reqline[1], "/\0", 2)==0 )
				//strcpy(path, "/index.html");//Because if no file is specified, index.html will be opened by default (like it happens in APACHE...
				strcat(path, "/index.html");
			else{
				strcat(path, reqline[1]);
	            /*strcpy(temp, reqline[1]);
				length = strlen(temp);
				for (int i=1; i <= length; i++){
					path[i-1] = temp[i];
				}*/
			}
			stat(path, &buf);//Return the information of the file to buf
			r = buf.st_mode & S_IROTH;
			
			if ( (fd = open(path, O_RDONLY))!=-1 )    //FILE FOUND
            {
				
				if(r != S_IROTH)
				{
					forbidden(fd_num);
				}
				else
				{
	                //send(fd_num, "HTTP/1.0 200 OK\n\n", 17, 0);
					headers(fd_num, path);
				
					bzero(buffer, BUF_SIZE);
	                while ( (bytes_read=read(fd, buffer, BUF_SIZE))>0 )
	                    write (fd_num, buffer, bytes_read);
				}

            }
			else    
	            not_found(fd_num);//404 Not Found	
        }
	}
	printf("Respond message sent!\n");
	
	close(fd_num);
}

void headers(int client, char *filename)
{
 char buf[1024];
 int file_length;
 char type[10];
 //(void)filename;
 
 strcpy(buf, "HTTP/1.0 200 OK\r\n");
 send(client, buf, strlen(buf), 0);
 
 getTime();
 strcpy(buf, current_time);
 send(client, buf, strlen(buf), 0);
 
 strcpy(buf, SERVER_STRING);
 send(client, buf, strlen(buf), 0);

 file_length = getFileSize(filename);
 sprintf(buf, "Content-Length: %d\r\n", file_length);
 send(client, buf, strlen(buf), 0);

 char* ret = strchr(filename, '.');

 if (strcmp(ret, ".html\0")==0){
  	 sprintf(buf, "Content-Type:text/html\r\n");
  	 send(client, buf, strlen(buf), 0);
   }
   
 else if (strcmp(ret, ".jpg\0")==0){
	sprintf(buf, "Content-Type:image/jpg\r\n");
	send(client, buf, strlen(buf), 0);
 }
 
 else if (strcmp(ret, ".gif\0")==0){
	 sprintf(buf, "Content-Type:image/gif\r\n");
	 send(client, buf, strlen(buf), 0);
 }
else if (strcmp(ret, ".png\0")==0){
	 sprintf(buf, "Content-Type:image/png\r\n");
	 send(client, buf, strlen(buf), 0);
 }
else if (strcmp(ret, ".jpeg\0")==0){
	 sprintf(buf, "Content-Type:image/jpeg\r\n");
	 send(client, buf, strlen(buf), 0);
 }
else if (strcmp(ret, ".txt\0")==0){
	 sprintf(buf, "Content-Type:text/plain\r\n");
	 send(client, buf, strlen(buf), 0);
 }
else if (strcmp(ret, ".ico\0")==0){
	 sprintf(buf, "Content-Type:image/ico\r\n");
	 send(client, buf, strlen(buf), 0);
 }
 
 strcpy(buf, "\r\n");
 send(client, buf, strlen(buf), 0);
}


void not_found(int client)
{
 char buf[1024];

 sprintf(buf, "HTTP/1.0 404 NOT FOUND\r\n");
 send(client, buf, strlen(buf), 0);
 
 sprintf(buf, SERVER_STRING);
 send(client, buf, strlen(buf), 0);
 
 sprintf(buf, "Content-Type: text/html\r\n");
 send(client, buf, strlen(buf), 0);
 
 sprintf(buf, "\r\n");
 send(client, buf, strlen(buf), 0);
 
 sprintf(buf, "<HTML><TITLE>Not Found</TITLE>\r\n");
 send(client, buf, strlen(buf), 0);
 
 sprintf(buf, "<BODY><P>HTTP/1.0 404 NOT FOUND: The server could not fulfill\r\n");
 send(client, buf, strlen(buf), 0);
 
 sprintf(buf, "your request because the resource specified\r\n");
 send(client, buf, strlen(buf), 0);
 
 sprintf(buf, "is unavailable or nonexistent.\r\n");
 send(client, buf, strlen(buf), 0);
 
 sprintf(buf, "</BODY></HTML>\r\n");
 send(client, buf, strlen(buf), 0);
}

void bad_request(int client)
{
 char buf[1024];
 
 sprintf(buf, "HTTP/1.0 400 BAD REQUEST\r\n");
 send(client, buf, strlen(buf), 0);
 
 sprintf(buf, SERVER_STRING);
 send(client, buf, strlen(buf), 0);
 
 sprintf(buf, "Content-Type: text/html\r\n");
 send(client, buf, strlen(buf), 0);
 
 sprintf(buf, "\r\n");
 send(client, buf, strlen(buf), 0);
 
 sprintf(buf, "<HTML><TITLE>400 BAD REQUEST</TITLE>\r\n");
 send(client, buf, strlen(buf), 0);
 
 sprintf(buf, "<BODY><P>HTTP/1.0 400 BAD REQUEST: You are sending a bad request\r\n");
 send(client, buf, strlen(buf), 0);
 
 sprintf(buf, "</BODY></HTML>\r\n");
 send(client, buf, strlen(buf), 0);

}

void forbidden(int client)
{
 char buf[1024];
 
 sprintf(buf, "HTTP/1.0 403 FORBIDDEN\r\n");
 send(client, buf, strlen(buf), 0);
 
 sprintf(buf, SERVER_STRING);
 send(client, buf, strlen(buf), 0);
 
 sprintf(buf, "Content-Type: text/html\r\n");
 send(client, buf, strlen(buf), 0);
 
 sprintf(buf, "\r\n");
 send(client, buf, strlen(buf), 0);
 
 sprintf(buf, "<HTML><TITLE>403 Forbidden</TITLE>\r\n");
 send(client, buf, strlen(buf), 0);
 
 sprintf(buf, "<BODY><P>HTTP/1.0 403 FORBIDDEN: The request was a valid request, \r\n");
 send(client, buf, strlen(buf), 0);
 
 sprintf(buf, "but you do not have the necessary permissions for the resource.\r\n");
 send(client, buf, strlen(buf), 0);
 
 sprintf(buf, "</BODY></HTML>\r\n");
 send(client, buf, strlen(buf), 0);
 

}

int getFileSize(char *filename)    
{   
    FILE * fp = fopen(filename, "r");   
    fseek(fp, 0L, SEEK_END);   
    int size = ftell(fp);   
    fclose(fp);   
    return size;   
}

void getTime()
{
	time_t rawtime;
	struct tm * timeinfo;
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	sprintf(current_time, "Date: %s\r", asctime (timeinfo));
}

