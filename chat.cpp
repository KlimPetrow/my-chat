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

  if (argc < 2) {
    usage();
    exit(EXIT_FAILURE);
  }

  if (argc == 3 && !strcmp(argv[1], "-s")) {
    // server mode
    init_server(argv[2]);

  } else if (argc == 4 && !strcmp(argv[1], "-c")) {
    // client mode
    init_client(argv[2], argv[3]);
  }
}
