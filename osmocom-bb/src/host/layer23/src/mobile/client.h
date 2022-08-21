/**
 * Example taken from CS 241 @ UIUC
 * Edited by Austin Walters
 * Used as example for austingwalters.com,
 * in socket IPC explanation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

void client(char buffer[]){

  int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

  struct addrinfo info, *result;
  memset(&info, 0, sizeof(struct addrinfo));
  info.ai_family = AF_INET;
  info.ai_socktype = SOCK_STREAM;

  if(0 != getaddrinfo("172.17.0.2", "777", &info, &result))
    exit(1);
  
  /* Connects to bound socket on the server */  
  connect(sock_fd, result->ai_addr, result->ai_addrlen);

  printf("SENDING: %s", buffer);
  write(sock_fd, buffer, strlen(buffer));

  char resp[999];
  int len = strlen(buffer);
  resp[len] = '\0';
  printf("%s\n", resp);
}
