
/*
* Assignment 2: Data Transformation Services
* 
* 
* This is the main client which uses TCP as the transport layer protocol for data transfer with the master server
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <stdbool.h>

/* Some generic error handling stuff */
extern int errno;
void perror(const char *s);

/* Manifest constants used by client program */
#define MAX_HOSTNAME_LENGTH 64
#define MAX_WORD_LENGTH 100
#define BYNAME 1
#define MYPORTNUM 5550   /* must match the server's port! */


/* Prompt the user to enter a word */
void printmenu()
  {
    printf("\n");
    printf("1 - Identity \n");
    printf("2 - Reverse \n");
    printf("3 - Upper \n");
    printf("4 - Lower \n");
    printf("5 - Caesar \n");
    printf("6 - Alternating Cases \n");
    printf("Please enter a command (ex. 2153) or type quit to exit:\n");
  }

/* Checks if command is valid */
bool validCommand(char *command){
  for(int i = 0; i != strlen(command); i++){
    if(command[i] != '1' && command[i] != '2' && command[i] != '3' && command[i] != '4' && command[i] != '5' && command[i] != '6'){
      return false;
    }
  }
  return true;
}

/* Main program of client */
int main()
  {
    int sockfd, sockfd2;
    char c;
    struct sockaddr_in server;
    struct hostent *hp;
    char hostname[MAX_HOSTNAME_LENGTH];
    char message[MAX_WORD_LENGTH];
    char command[1000];
    char messageback[MAX_WORD_LENGTH];
    int choice, len, bytes;
    int messagelen;

    /* Initialization of server sockaddr data structure */
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(MYPORTNUM);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

#ifdef BYNAME
    strcpy(hostname, "127.0.0.1");
    hp = gethostbyname(hostname);
    if (hp == NULL)
      {
	fprintf(stderr, "%s: unknown host\n", hostname);
	exit(1);
      }
    /* copy the IP address into the sockaddr structure */
    bcopy(hp->h_addr, &server.sin_addr, hp->h_length);
#else
    /* hard code the IP address so you don't need hostname resolver */
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
#endif

    /* create the client socket for its transport-level end point */
    if( (sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1 )
      {
	fprintf(stderr, "socket() call failed!\n");
	exit(1);
      }

    /* connect the socket to the server's address using TCP */
    if( connect(sockfd, (struct sockaddr *)&server, sizeof(struct sockaddr_in)) == -1 )
      {
	fprintf(stderr, "connect() call failed!\n");
	perror(NULL);
	exit(1);
      }

    /* Print welcome message*/
    printf("Enter a sentence: \n");
    messagelen = 0;
	  while( (c = getchar()) != '\n' )
	      {
		message[messagelen] = c;
		messagelen++;
	      }
	    /* make sure the message is null-terminated in C */
	  message[messagelen] = '\0';

    printf("Message: %s\n",message);
    /* main loop: read a word, send to server, and print answer received */

    printmenu(); // print menu
    len = 0;
	  while( (c = getchar()) != '\n' )
	      {
        command[len] = c;
        len++;
	      }
	  command[len] = '\0';
    while(strcmp (command, "quit") != 0)
      {
        if(validCommand(command)){
          char messagein[MAX_WORD_LENGTH];
          strcpy(messagein,message);
          strcat(messagein," ");
          strcat(messagein,command); // concactenate command to message
          printf("Sending message\n");
          send(sockfd, messagein, messagelen+len+1, 0);


            /* see what the server sends back */
          if( (bytes = recv(sockfd, messageback, messagelen+len+1, 0)) > 0 )
            {
            /* make sure the message is null-terminated in C */
            messageback[bytes] = '\0';
            printf("Transformed Message: ");
            printf("%s\n", messageback);
            }
          else
            {
            /* an error condition if the server dies unexpectedly */
            printf("Sorry, dude. Server failed!\n");
            close(sockfd);
            exit(1);
            }
        }
        else{
          printf("Error in Command. Try Again\n");
        }
 
  
	      printmenu();
	      len = 0;
	      while( (c = getchar()) != '\n' )
	      {
          command[len] = c;
          len++;
	      }
	      command[len] = '\0';
        
        



      }

    /* Program all done, so clean up and exit the client */
    close(sockfd);
    exit(0);
  }
