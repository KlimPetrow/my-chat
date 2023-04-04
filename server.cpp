#include "server.h" 

#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <cstring>
#include <string>
#include <pthread.h>
#include <vector>
#include <mutex>
#include <thread>

#define BUFF_SIZE 1024
#define LOCALHOST "127.0.0.1"
#define MAX_CLIENTS 5
using namespace std;
mutex locker;

struct server_codition{
  int client_socket;
  vector<int> client_list;
};

void handle(int client_socket){
    char buffer[BUFF_SIZE];
    memset(buffer, 0, BUFF_SIZE);

    while (true) {
        int bytes_read = recv(client_socket, buffer, BUFF_SIZE, 0);
        if (bytes_read <= 0) {
            break;
        }
        {
            lock_guard<mutex> lock(locker);
            cout << "Received message from client: " << buffer << endl;
        }
        send(client_socket, buffer, bytes_read, 0);
        memset(buffer, 0, BUFF_SIZE);
    }

    close(client_socket);
}

void init_server(const char *port){
  int server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if(server_socket == -1){
    cerr<<"create_socket_error"<<endl;
    exit(EXIT_FAILURE);
  }
  cout<<"server socket is create"<<endl;
  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = stoi(port);
  server_address.sin_addr.s_addr = inet_addr(LOCALHOST);

  if(bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address)) == -1){
    cerr<<"bind error"<<endl;
    close(server_socket);
    exit(EXIT_FAILURE);
  }
  
  if(listen(server_socket, MAX_CLIENTS) == -1){
    cerr<<"listen_error"<<endl;
    close(server_socket);
    exit(EXIT_FAILURE);
  }

  cout<<"server is started on port: "<<server_address.sin_port<<"... waiting for incoming connections"<<endl;
  
  vector<int> client_list;
  vector<thread> threads;
  struct sockaddr_in client_address;
  while(true){
    socklen_t client_address_size = sizeof(client_address);
    int client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_address_size);
    if(client_socket == -1){
      cerr<<"accept_error"<<endl;
    }

    {
      lock_guard<mutex> lock(locker);
      cout<<"New client connected"<<endl;
    }
    //client_list.push_back(client_socket);
    //cout<<"client "<<client_list.size()<<" "<<" is connected"<<endl;
    //struct server_codition condition;
    //condition.client_socket = client_socket;
    //condition.client_list = client_list;
    threads.emplace_back(handle, client_socket);
  }
    for (auto &t : threads) {
        t.join();
    }

    close(server_socket);
}


