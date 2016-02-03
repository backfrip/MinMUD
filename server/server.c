#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "format.h"

int run_server() {
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  
  printf ("lines %d\n", w.ws_row);
  printf ("columns %d\n", w.ws_col);
  
  int r_pipe, w_pipe;
  {
    int rp[2], wp[2], f;
    
    STAT("initializing pipes...");
    if (pipe(rp) < 0 || pipe(wp) < 0)
      PEXIT("error initializing pipes");
    
    STAT("forking game process...");
    f = fork();
    if (f < 0)
      PEXIT("error forking game process");
    if (f == 0)
      exit(run_game(wp[0], rp[1]));
    
    r_pipe = rp[0];
    w_pipe = wp[1];
    
    STAT("closing unused pipe ends...");
    if (close(rp[1]) < 0 || close(wp[0]) < 0)
      PEXIT("error closing unused pipe ends");
  }
  
  int s_fd, c_fd, max_rfd;
  struct sockaddr_in s_addr, c_addr;
  socklen_t c_size;
  fd_set read_fds, rfds;

  s_addr.sin_family = AF_INET;
  s_addr.sin_port = htons(26101);
  s_addr.sin_addr.s_addr = INADDR_ANY;
  
  STAT("initializing listener socket...");
  s_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (s_fd < 0)
    PEXIT("error initializing listener socket");
  
  STAT("binding listener socket...");
  if (bind(s_fd, (struct sockaddr *)&s_addr, sizeof(s_addr)) < 0)
    PEXIT("error binding listener socket");
  
  STAT("listening on listener socket...");
  if (listen(s_fd, 0) < 0)
    PEXIT("error listening on listener socket");
  
  FD_ZERO(&read_fds);
  FD_SET(s_fd, &read_fds);
  FD_SET(r_pipe, &read_fds);
  max_rfd = max_of(s_fd, r_pipe);
  
  int s, i, r;
  char buff[512];
  while(1) {
    rfds = read_fds;
    
    s = select(FD_SETSIZE, &rfds, NULL, NULL, NULL);
    if (s < 0)
      PEXIT("error while polling read streams");
    
    // For every file descriptor possible in `read_fds`...
    for (i = 0; s > 0 && i <= max_rfd; ++i)
      // If the file descriptor is in the `rfds` set...
      if (FD_ISSET(i, &rfds)) {
        memset(buff, 0, sizeof(buff));
        // If the file descriptor is the listener socket...
        if (i == s_fd) {
          c_size = sizeof(c_addr);
          c_fd = accept(s_fd, (struct sockaddr *)&c_addr, &c_size);
          if (c_fd < 0)
            PEXIT("error accepting new connection");
          inet_ntop(c_addr.sin_family, &(c_addr.sin_addr), &buff, sizeof(buff));
          printf(P_STAT "new connection from %s" NL, buff);
          FD_SET(c_fd, &read_fds);
          max_rfd = max_of(max_rfd, c_fd);
        // Otherwise... (bytes are ready to be read from somewhere)
        } else {
          r = read(i, buff, sizeof(buff) - 1);
          buff[strcspn(buff, "\n")] = 0;
          // If the file descriptor is the game process read pipe...
          if (i == r_pipe) {
            if (r < 0)
              PEXIT("error reading from game process");
            printf(P_GAME "%s" NL, buff);
          // Otherwise... (bytes are ready to be read from a client)
          } else {
            if (r < 0)
              PEXIT("error reading from socket");
            // If EOF condition is met...
            if (r == 0) {
              close(i);
              FD_CLR(i, &read_fds);
              if (i == max_rfd)
                max_rfd--;
            // Otherwise... (business as usual)
            } else
              write(w_pipe, buff, r);
          } 
        }
        s--;
      }
  }
  return 0;
}

int run_game(int r_pipe, int w_pipe) {
  int flags = fcntl(r_pipe, F_GETFL, 0);
  fcntl(r_pipe, F_SETFL, flags | O_NONBLOCK);
  while (1) {
    char buff[512];
    int r = read(r_pipe, buff, sizeof(buff) - 1);
    if (r < 0 && errno != EAGAIN)
      PEXIT("error reading from server in game process");
    if (r > 0)
      write(w_pipe, buff, r);
  }
  return 0;
}

int max_of(int a, int b) {
  if (a >= b) return a;
  return b;
}