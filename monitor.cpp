/*
Lab 1
Name: Steve Regala
Student ID: 7293040280
Email: sregala@usc.edu
*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#include <iostream>
#include <string>

using namespace std;

/**
 * Hardcoded constants
 */
#define LOCALHOST "127.0.0.1"
#define MONITOR_PORT_TCP 26280     // port that monitor connects to


/**
 * Global variables
 */
int monitor_sockfd_TCP;
struct sockaddr_in main_server_addr;


/**
 * Create a monitor TCP socket and connect to main server
 */
void monitor_socket_TCP(){

   if((monitor_sockfd_TCP = socket(PF_INET, SOCK_STREAM, 0)) == -1){
      perror("ERROR: Monitor failed to create socket for main server");
      exit(1);
   }

   main_server_addr.sin_family = AF_INET;
   main_server_addr.sin_port = htons(MONITOR_PORT_TCP);
   main_server_addr.sin_addr.s_addr = inet_addr(LOCALHOST);
   memset(&(main_server_addr.sin_zero), '\0', 8);  // set everything else to 0

   /*if(connect(monitor_sockfd_TCP, (struct sockaddr *)&main_server_addr, sizeof(struct sockaddr)) == -1){
      perror("ERROR: Monitor failed to connect to main server");
      exit(1);
   }*/
   connect(monitor_sockfd_TCP, (struct sockaddr *)&main_server_addr, sizeof(struct sockaddr));
   
   cout << "The monitor is up and running." << endl;

}



int main(int argc, char* argv[]){

   monitor_socket_TCP();

   // must close monitor socket
   close(monitor_sockfd_TCP);
   return 0;
}