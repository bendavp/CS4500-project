// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <iostream>
using namespace std;

#define PORT 5555

int main(int argc, char const *argv[])
{
  int sock = 0, valread;
  struct sockaddr_in serv_addr;
  char hello[256] = {0};
  char buffer[1024] = {0};

  // setup socket 
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    printf("\n Socket creation error \n");
    return -1;
  }

  // connect to localhost with server port #
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);

  // Convert IPv4 and IPv6 addresses from text to binary form
  if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
  {
    printf("\nInvalid address/ Address not supported \n");
    return -1;
  }

  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
  {
    printf("\nConnection Failed \n");
    return -1;
  }

  // send sever registration message
  // seed rondom number with timestamp as client id
  srand(time(0));
  sprintf(hello, "client ID[%d]", rand());

  send(sock, hello, strlen(hello), 0);
  printf("registration message sent\n");
  valread = read(sock, buffer, 1024);
  printf("clints in the registry\n");
  printf("%s\n", buffer);
  return 0;
}
