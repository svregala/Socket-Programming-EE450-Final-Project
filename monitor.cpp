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
#define MAXBUFLEN 1024     // max number of bytes at once


/**
 * Global variables
 */
int monitor_sockfd_TCP;
struct sockaddr_in main_server_addr;

string monitor_operation;
char send_to_main[MAXBUFLEN];    // message that will be sent back to the main server
char receive_from_main[MAXBUFLEN];     // message that will be received from the main server


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

   connect(monitor_sockfd_TCP, (struct sockaddr *)&main_server_addr, sizeof(struct sockaddr));
   
   cout << "The monitor is up and running." << endl;
}


/**
 * Main function 
 */
int main(int argc, char* argv[]){

   monitor_socket_TCP();

   if(argc!=2){
      cout << "ERROR: Proper command for monitor program is: \"./monitor TXLIST\"" << endl;
      exit(1);
   }

   else{
      monitor_operation = argv[1];
      if(monitor_operation!="TXLIST"){
         cout << "ERROR: Proper command for monitor program is: \"./monitor TXLIST\"" << endl;
         exit(1);
      }
      strncpy(send_to_main, monitor_operation.c_str(), MAXBUFLEN);

      // send monitor operation "TXLIST" to Main Server
      if(send(monitor_sockfd_TCP, send_to_main, sizeof(send_to_main), 0) == -1){
         perror("ERROR: Monitor failed to send information to Main Server");
         exit(1);
      }
      cout << "Monitor sent a sorted list request to the main server." << endl;

      // receive the confirmation from the main server that the list has been generated
      if(recv(monitor_sockfd_TCP, receive_from_main, sizeof(receive_from_main), 0) == -1){
         perror("ERROR: Client failed to receive information from Main Server");
         exit(1);
      }
      
      cout << "Successfully received a sorted list of transactions from the main server." << endl;
   }

   // must close monitor socket
   close(monitor_sockfd_TCP);
   return 0;
}