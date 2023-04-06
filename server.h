#pragma once
#include <string>
#include <vector>
using namespace std;

struct client_list {
  vector<string> names;
  vector<int> sockets;
};

void init_server(const char *port);

void handle(int);
