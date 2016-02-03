#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "format.h"

int run_client() {
  int s_fd;
  struct sockaddr_in s_addr;

  s_addr.sin_family = AF_INET;
  s_addr.sin_port = htons(26101);
  inet_aton("127.0.0.1", &(s_addr.sin_addr));
  
  s_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (connect(s_fd, (struct sockaddr *)&s_addr, sizeof(s_addr)) < 0)
    perror("error connecting to server");

}