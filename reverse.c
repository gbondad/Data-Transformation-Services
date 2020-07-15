/* UDP Reverse Server
 * Reverses order of bytes in message
 */
 
/* Include files */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* Manifest constants */
#define MAX_BUFFER_SIZE 100
#define PORT 5552

/* Verbose debugging */
#define DEBUG 1


void reverse(char *str)
{
    int length = strlen(str) - 1;
    char temp;
    for (int i = 0; i <= length/2; i++)
    {
      temp = str[i];
      str[i] = str[length - i];
      str[length - i] = temp;
      
    }
     
}



/* Main program */
int main()
  {
    struct sockaddr_in si_server, si_client;
    struct sockaddr *server, *client;
    int s, i, len=sizeof(si_server);
    char messagein[MAX_BUFFER_SIZE];
    char messageout[MAX_BUFFER_SIZE];
    int readBytes;

    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
      {
        printf("Could not setup a socket!\n");
        return 1;
      }
    
    memset((char *) &si_server, 0, sizeof(si_server));
    si_server.sin_family = AF_INET;
    si_server.sin_port = htons(PORT);
    si_server.sin_addr.s_addr = htonl(INADDR_ANY);
    server = (struct sockaddr *) &si_server;
    client = (struct sockaddr *) &si_client;

    if (bind(s, server, sizeof(si_server))==-1)
      {
        printf("Could not bind to port %d!\n", PORT);
        return 1;
      }
    printf("server now listening on UDP port %d...\n", PORT);
	
    /* big loop, looking for incoming messages from clients */
    for( ; ; )
      {
    /* clear out message buffers to be safe */
    bzero(messagein, MAX_BUFFER_SIZE);
    bzero(messageout, MAX_BUFFER_SIZE);

	  /* see what comes in from a client, if anything */
	  if ((readBytes=recvfrom(s, messagein, MAX_BUFFER_SIZE, 0, client, &len)) < 0)
	  {
      printf("Read error!\n");
      return -1;
	  }
#ifdef DEBUG
	    else printf("Server received %d bytes\n", readBytes);
#endif

      printf("  server received \"%s\" from IP %s port %d\n",
              messagein, inet_ntoa(si_client.sin_addr), ntohs(si_client.sin_port));

#ifdef DEBUG
#endif
      reverse(messagein);
	    sprintf(messageout, "%s", messagein);
#ifdef DEBUG
	    printf("Server sending back the message: \"%s\"\n", messageout);

#endif

      /* send the result message back to the client */
      sendto(s, messageout, strlen(messageout), 0, client, len);		
          }
      close(s);
      return 0;
  }
