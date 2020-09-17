#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define CLIENT_MAX 30
#define MAX_BYTES 77

void error(char *msg)
{
  perror(msg);
  exit(1);
}

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    fprintf(stderr, "%s\n", "Takes in PORT as argument");
    exit(1);
  }
  
  int server_socket, client_sockets[CLIENT_MAX], new_socket, sfd, active_fd, max_fd;
  struct sockaddr_in client_addr, server_addr;
  int client_len, n, i;
  char buffer[MAX_BYTES], current_post[MAX_BYTES];
  fd_set readfds;

  int port = atoi(argv[1]);

  for(i = 0; i < CLIENT_MAX; i++)
  {
    client_sockets[i] = 0;	  
  }

  server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket < 0)
    error("SOCKET");

  bzero((char *) &server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons((unsigned short) port);

  if (bind(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
    error("BIND");

  if (listen(server_socket, CLIENT_MAX) < 0)
    error("LISTEN");

  memset(current_post, '\0', MAX_BYTES);
  strcpy(current_post, "\n");

  while(1)
  {
    FD_ZERO(&readfds);
    FD_SET(server_socket, &readfds);
    max_fd = server_socket;
    for (i = 0; i < CLIENT_MAX; i++)
    {
      sfd = client_sockets[i];

      if (sfd > 0)
        FD_SET(sfd, &readfds);
      if (sfd > max_fd)
	      max_fd = sfd;
    }
    active_fd = select(max_fd + 1, &readfds, NULL, NULL, NULL);
    if(active_fd < 0)
      error("SELECT");

    if(FD_ISSET(server_socket, &readfds))
    {
      client_len = sizeof(client_addr);    
      new_socket = accept(server_socket, (struct sockaddr *) &client_addr, &client_len);
      if (new_socket < 0)
	      error("ACCEPT");

      for (i = 0; i < CLIENT_MAX; i++)
      {
        if (client_sockets[i] == 0)
        {
	        client_sockets[i] = new_socket;
	        break;
	      }
      }
    }
      
    for (i = 0; i < CLIENT_MAX; i++)
    {
      sfd = client_sockets[i];

      if (FD_ISSET(sfd, &readfds))
      {
        n = read(sfd, buffer, MAX_BYTES);
	      if (n < 0)
          error("READ");
	      if (n == 0)
        {
	        close(sfd);
	        client_sockets[i] = 0; 
	      }
	      int newlines = 0;
        buffer[n] = '\0';
        for (int j = 0; j < strlen(buffer); j++)
        {
          if (buffer[j] == '\n')
	          newlines++;
	      }
        	
	      if (newlines == 1 && buffer[n -1] == '\n')
        {
          if (buffer[0] == '!')
	        {
            memmove(buffer, buffer + 1, strlen(buffer));
	          strcpy(current_post, buffer);
	        }
	        if (strcmp(buffer, "?\n") == 0)
	        {  
            n = send(sfd, current_post, strlen(current_post), 0);
	          if (n < 0)
	            error("WRITE");
	        }
        }
      }
    }
  }
}
