#include <stdio.h>
#include <stdlib.h>

#include "format.h"
#include "client/client.h"
#include "server/server.h"

int main() {
  // Ideally, I'll standardize everything that happens in here, get
  // extra methods that handle all the fancy formatting and stuff,
  // but for now it's just manual printing/checking of stuff.
  
  printf(A_RED "\
     __  __ _      __  __ _   _ ___  \n\
    |  \\/  (_)_ _ |  \\/  | | | |   \\ \n\
    | |\\/| | | ' \\| |\\/| | |_| | |) |\n\
    |_|  |_|_|_||_|_|  |_|\\___/|___/ \n\n\
 (c) E Stringham | Stuyvesant High School\n\n"
    A_RES "Available options:\n\
  - s: run server\n\
  - c: run client\n\
> ");

  char in[2];
  fgets(in, sizeof(in), stdin);
  
  if (in[0] == 's') {
    printf("Running server...\n");
    run_server();
  } else if (in[0] == 'c') {
    printf("Running client...\n");
    run_client();
  } else {
    printf("Invalid input!\n");
  }

  return 0;
}
