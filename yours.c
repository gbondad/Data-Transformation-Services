/* UDP Alternating cases
 * Turns every even index char tp upper case and every odd index char to lower case
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
#define PORT 5556

/* Verbose debugging */
#define DEBUG 1




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

      for(int i = 0; messagein[i] != '\0'; i++)
      {
          char c = messagein[i];
          if(i % 2 == 0){
              if(c >= 'a' && c <= 'z')
            {
              messagein[i] = c - 32;
            }
          }
          else{
            if(c >= 'A' && c <= 'Z')
            {
              messagein[i] = c + 32;
            }




          }
      }
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
