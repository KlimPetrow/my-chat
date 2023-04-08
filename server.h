#pragma once
#include <string>
using namespace std;

void init_server(const char *);

void handle(int, int *);

void send_message(int, string);

void server_handler(int *, int);

void server_shutdown(int *, int);
