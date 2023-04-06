#include "server.h"

#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <mutex>
#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
#include <vector>

#define BUFF_SIZE 1024
#define LOCALHOST "127.0.0.1"
#define MAX_IN_QUEUE 10
#define MAX_CLIENTS 20

using namespace std;
mutex locker;
// TODO: Use array for that
// TODO: Don't use global variable
vector<int> sockets;

void send_message(int client_socket, string message) {
  send(client_socket, message.c_str(), message.size(), 0);
}

void handle(int client_socket) {
  char buffer[BUFF_SIZE];

  // TODO: Send in cycle
  string first_message = "Hello! Enter you name, please: ";
  send(client_socket, first_message.c_str(), first_message.size(), 0);

  // TODO: Check return status and rename or don't name
  memset(buffer, 0, BUFF_SIZE);
  int name_size = recv(client_socket, buffer, BUFF_SIZE, 0);
  string client_name = buffer;
  cout << client_name << " is connected!" << endl;

  while (true) {
    memset(buffer, 0, BUFF_SIZE);
    if (recv(client_socket, buffer, BUFF_SIZE, 0) <= 0)
      continue;
    if (string(buffer) == "exit()") {
      cout << client_name << " disconnected!" << endl << flush;
      send_message(client_socket, "Goodbye!");
      {
        lock_guard<mutex> lock(locker);
        // TODO: Use size_t to supress warnings
        // TODO: Try to use range for
        // TODO: Use array for that
        for (int i = 0; i < sockets.size(); i++) {
          if (sockets[i] == client_socket) {
            sockets[i] = 0;
          }
        }
        break;
      }
    }

    cout << client_name << ": " << buffer << endl;
    {
      lock_guard<mutex> lock(locker);
      // TODO: Use size_t to supress warnings
      // TODO: Try to use range for
      for (int i = 0; i < sockets.size(); i++) {
        if (sockets[i] == client_socket || sockets[i] == 0)
          continue;
        string message = client_name + ": " + string(buffer);
        send_message(sockets[i], message);
      }
    }
  }
  close(client_socket);
}

// TODO: When shutdown server you should also properly shutdown all clients
void init_server(const char *port) {
  int server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket == -1) {
    cerr << "create_socket_error" << endl;
    exit(EXIT_FAILURE);
  }
  cout << "server socket is create" << endl;
  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = stoi(port);
  server_address.sin_addr.s_addr = inet_addr(LOCALHOST);

  if (bind(server_socket, (struct sockaddr *)&server_address,
           sizeof(server_address)) == -1) {
    cerr << "bind error" << endl;
    close(server_socket);
    exit(EXIT_FAILURE);
  }

  if (listen(server_socket, MAX_IN_QUEUE) == -1) {
    cerr << "listen_error" << endl;
    close(server_socket);
    exit(EXIT_FAILURE);
  }

  cout << "server is started on port: " << server_address.sin_port
       << "... waiting for incoming connections" << endl;

  vector<thread> threads;
  struct sockaddr_in client_address;
  while (true) {
    socklen_t client_address_size = sizeof(client_address);
    int client_socket =
        accept(server_socket, (struct sockaddr *)&client_address,
               &client_address_size);
    if (client_socket < 0) {
      cerr << "accept_error" << endl;
      continue;
    }

    {
      lock_guard<mutex> lock(locker);
      if (sockets.size() <= MAX_CLIENTS) {
        sockets.push_back(client_socket);
      } else {
        close(client_socket);
        continue;
      }
    }

    threads.emplace_back(handle, client_socket);
  }

  for (auto &t : threads) {
    t.join();
  }

  close(server_socket);
}
