#include "client.h"
#include "server.h"
#include <cstring>
#include <iostream>
#include <string>
#include <unistd.h>

#define BUFF_SIZE 1024
using namespace std;

static void usage() {
  cout << "chat -c <ip> <port>" << endl;
  cout << "or" << endl;
  cout << "chat -s <port>" << endl;
}

int main(int argc, char *argv[]) {

  // TODO: BUG HERE
  if (argc < 2) {
    usage();
    exit(EXIT_FAILURE);
  }
  
  // TODO: Use `find()`
  if (argc == 3 && !strcmp(argv[1], "-s")) {
    // server mode
    init_server(argv[2]);

  // TODO: Use `find()`
  } else if (argc == 4 && !strcmp(argv[1], "-c")) {
    // client mode
    init_client(argv[2], argv[3]);
  }
}
