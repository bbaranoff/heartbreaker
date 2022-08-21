/**
 * Written by Austin Walters
 * For an example on austingwalters.com,
 * on sockets
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
char text[51];
char* catch_rand(){

  int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  struct addrinfo directives, *result;
  memset(&directives, 0, sizeof(struct addrinfo));
  directives.ai_family = AF_INET;
  directives.ai_socktype = SOCK_STREAM;
  directives.ai_flags = AI_PASSIVE;

  /* Translates IP, port, protocal into struct */
  if(0 !=  getaddrinfo("0.0.0.0", "888", &directives, &result))
    exit(1);
 
  /* Binds socket to port, so we know where new connections form */
  if(bind(sock_fd, result->ai_addr, result->ai_addrlen) != 0)
      exit(1);
  /* Places socket to "listen" or "wait for stuff" state */
  if(listen(sock_fd, 10) != 0)
      exit(1);
  int i=0;
  printf("Waiting for connection on http://foreignhost:888 ...\n");
  while(i==0){
   
    /* Accepts Connection */
    char buffer[1000];
    int client_fd = accept(sock_fd, NULL, NULL); 
    int len = read(client_fd, buffer, 999);
    buffer[len] = '\0';
    
    char * header = "<b>You Connected to the Server!</b></br></br>";
    i=i+1;
    write(client_fd, header, strlen(header));
    
    printf("=== Client Sent ===\n");
    printf("%s\n", buffer);
    memcpy(text,buffer,51);
    close(client_fd);

  }
  return text;
}
