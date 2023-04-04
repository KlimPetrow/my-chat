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

#define BUFF_SIZE 1024
#define LOCALHOST "127.0.0.1"
using namespace std;

struct server_codition{
  int client_socket;
  vector<int> client_list;
};

void* handle(void *arg){
  struct server_codition condition = *((server_codition*)arg);
  string first_message = "Welcome to the club, body!";
  send(condition.client_socket, first_message.c_str(), first_message.length(), 0);
  return nullptr;
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
  
  if(listen(server_socket, 1) == -1){
    cerr<<"listen_error"<<endl;
    close(server_socket);
    exit(EXIT_FAILURE);
  }

  cout<<"server is started on port: "<<server_address.sin_port<<"... waiting for incoming connections"<<endl;
  
  vector<int> client_list = {0};
  struct sockaddr_in client_address;

  socklen_t client_address_size = sizeof(client_address);
  while(true){
    int client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_address_size);
    if(client_socket == -1){
      cerr<<"accept_error"<<endl;
    }
    client_list.push_back(client_socket);
    cout<<"client "<<client_list.size()<<" "<<" is connected"<<endl;
    struct server_codition condition;
    condition.client_socket = client_socket;
    condition.client_list = client_list;
    
    pthread_t thread;
    int rc = pthread_create(&thread, nullptr, handle, &condition);
    if (rc){
      cout<<"pthread_create_error"<<endl;
      exit(EXIT_FAILURE);
    }

  }

  close(server_socket);
}



