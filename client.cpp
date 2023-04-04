#include "client.h"
#include<iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <cstring>
#include <string>

#define BUFF_SIZE 1024
using namespace std;

int read_message(int socket){
  char buffer[BUFF_SIZE];
  memset(buffer, 0, BUFF_SIZE);
  int readed_size = read(socket, buffer, BUFF_SIZE-1);
  if(readed_size<=0){
    cerr<<"connection closed by server"<<endl;
    return 0;
  }
  cout<<buffer<<endl;
  return 1;
}

void init_client(const char *ip, const char *port){
  
  int client_socket = socket(AF_INET, SOCK_STREAM, 0);
  if(client_socket == -1){
    cerr<<"error_create_socket"<<endl;
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = inet_addr(ip);
  server_address.sin_port = stoi(port);

  if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
    cerr << "connect_error" <<endl;
    close(client_socket);
    exit(EXIT_FAILURE);
  }
  while(true){
    if(!read_message(client_socket)){
      break;  
    }
  }
  cout<<"disconneting!"<<endl; 
  close(client_socket);

}
