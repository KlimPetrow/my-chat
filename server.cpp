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
#define MAX_CLIENTS 5

using namespace std;
mutex locker;

void send_message(int client_socket, string message) {
  send(client_socket, message.c_str(), message.size(), 0);
}

void server_shutdown(int *sockets_array, int server_socket) {
  string stop_message = "server is stoped! disconnecting...";
  {
    lock_guard<mutex> lock(locker);
    for (int i = 0; i < MAX_CLIENTS; i++) {
      if (sockets_array[i] == 0)
        continue;
      send_message(sockets_array[i], stop_message);
      close(sockets_array[i]);
    }
  }
  close(server_socket);
  exit(EXIT_SUCCESS);
}

void server_handler(int *sockets_array, int server_socket) {
  while (true) {
    string command;
    getline(cin, command);
    if (command == "stop_server()") {
      server_shutdown(sockets_array, server_socket);
    }
  }
}

void handle(int client_socket, int *socket_array) {
  char buffer[BUFF_SIZE];

  string first_message = "Hello! Enter you name, please: ";
  send_message(client_socket, first_message);

  memset(buffer, 0, BUFF_SIZE);
  if (recv(client_socket, buffer, BUFF_SIZE, 0) < 0) {
    cerr << "recv_error" << endl;
    exit(EXIT_FAILURE);
  }
  string client_name = buffer;
  cout << client_name << " is connected!" << endl;

  while (true) {
    memset(buffer, 0, BUFF_SIZE);
    if (recv(client_socket, buffer, BUFF_SIZE, 0) <= 0)
      continue;
    if (string(buffer) == "exit()") {
      string disconnect_message = client_name + " disconnected!";
      cout << disconnect_message << endl << flush;
      send_message(client_socket, "Goodbye!");
      {
        lock_guard<mutex> lock(locker);
        for (int i = 0; i < MAX_CLIENTS; i++) {
          if (socket_array[i] == client_socket) {
            socket_array[i] = 0;
            continue;
          }
          if (socket_array[i] == 0)
            continue;
          send_message(socket_array[i], disconnect_message);
        }
        break;
      }
    }
    string message = client_name + ": " + string(buffer);
    cout << message << endl;
    {
      lock_guard<mutex> lock(locker);
      for (int i = 0; i < MAX_CLIENTS; i++) {
        if (socket_array[i] == client_socket || socket_array[i] == 0)
          continue;
        send_message(socket_array[i], message);
      }
    }
  }
  close(client_socket);
}

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
  int sockets[MAX_CLIENTS] = {0};
  thread std_input(server_handler, sockets, server_socket);

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
      for (int i = 0; i < MAX_CLIENTS; i++) {
        if (sockets[i] == 0) {
          sockets[i] = client_socket;
          break;
        }
      }
    }

    threads.emplace_back(handle, client_socket, sockets);
  }

  std_input.join();

  for (auto &t : threads) {
    t.join();
  }

  close(server_socket);
}
