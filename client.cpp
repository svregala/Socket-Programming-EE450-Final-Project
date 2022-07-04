/*
Final Project
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
#include <sstream>
#include <vector>
#include <climits>

using namespace std;

/**
 * Hardcoded constants
 */
#define LOCALHOST "127.0.0.1"
#define CLIENT_PORT_TCP 25280    // port that client connects to
#define MAXBUFLEN 1024     // max number of bytes at once


/**
 * Global variables
 */
int client_sockfd_TCP;
struct sockaddr_in main_server_addr;   // main server's address information

string name_balance;    // for the checking balance operation
string transfer_name, receive_name;    // names for transferring money operation
int amount_transfer;    // amount to transfer

char send_to_main[MAXBUFLEN];
char receive_from_main[MAXBUFLEN];


/**
 * Create a client TCP socket and connect to main server
 */
void client_socket_TCP(){

   if((client_sockfd_TCP = socket(PF_INET, SOCK_STREAM, 0)) == -1){
      perror("ERROR: Client failed to create socket for main server");
      exit(1);
   }

   main_server_addr.sin_family = AF_INET;
   main_server_addr.sin_port = htons(CLIENT_PORT_TCP);
   main_server_addr.sin_addr.s_addr = inet_addr(LOCALHOST);
   memset(&(main_server_addr.sin_zero), '\0', 8);  // set everything else to 0

   connect(client_sockfd_TCP, (struct sockaddr *)&main_server_addr, sizeof(struct sockaddr));

   cout << "The client is up and running." << endl;
}


/**
 * Read in string sent from main server and place in vector
 */
vector<string> read_input_from_main_server(string s){
   string word;
   istringstream iss(s);
   vector<string> result;

   while(iss >> word){
      result.push_back(word);
   }
   return result;
}


/**
 * Main function
 */
int main(int argc, char* argv[]){

   client_socket_TCP();

   if(argc!=2 && argc!=4){
      cout << "ERROR: Proper command for client program is: \"./client <username>\" OR \"./client <username1> <username2> <transfer amount>\"" << endl;
      exit(1);
   }

   // if operation is to check balance, i.e. argc==2
   if(argc == 2){

      name_balance = argv[1];
      strncpy(send_to_main, name_balance.c_str(), MAXBUFLEN);

      // send name to Main Server for balance information
      if(send(client_sockfd_TCP, send_to_main, sizeof(send_to_main), 0) == -1){
         perror("ERROR: Client failed to send information to Main Server");
         exit(1);
      }
      cout << name_balance << " sent a balance enquiry request to the main server." << endl;

      // receive balance information from Main Server
      if(recv(client_sockfd_TCP, receive_from_main, sizeof(receive_from_main), 0) == -1){
         perror("ERROR: Client failed to receive information from Main Server");
         exit(1);
      }

      // check if what the Main Server sent is "NEGATIVE", otherwise output balance
      if(strcmp(receive_from_main, "NEGATIVE") == 0){
         cout << "Unable to proceed with the transaction as " << name_balance << " is not part of the network." << endl;
      }else{
         int balance = atoi(receive_from_main);
         cout << "The current balance of \"" << name_balance << "\" is : " << balance << " txcoins." << endl;
      }

   }

   // if operation is to transfer money, i.e. argc==4
   else if(argc==4){

      transfer_name = argv[1];
      receive_name = argv[2];
      amount_transfer = atoi(argv[3]);

      string temp_string = transfer_name + " " + receive_name + " " + to_string(amount_transfer) + " ";
      strncpy(send_to_main, temp_string.c_str(), MAXBUFLEN);

      // send transfer information to Main Server
      if(send(client_sockfd_TCP, send_to_main, sizeof(send_to_main), 0) == -1){
         perror("ERROR: Client failed to send information to Main Server");
         exit(1);
      }
      cout << transfer_name << " has requested to transfer " << amount_transfer << " txcoins to " << receive_name << "." << endl;

      // receive transfer information from Main Server, either a success or fail(due to insufficient balance OR one/both users are not a part of network)
      if(recv(client_sockfd_TCP, receive_from_main, sizeof(receive_from_main), 0) == -1){
         perror("ERROR: Client failed to receive information from Main Server");
         exit(1);
      }

      // break down response from the main server
      vector<string> response_from_main = read_input_from_main_server(receive_from_main);

      // CASE 1: NEITHER the sender nor the receiver are in the network
      if(response_from_main.size() == 2){    // size==2 because main server is only sending the names of those who are NOT in the network, what the main server sends is UNIQUE in size
         string no_sender_in_network = response_from_main.at(0);
         string no_receiver_in_network = response_from_main.at(1);
         cout << "Unable to proceed with the transaction as " << no_sender_in_network << " and " << no_receiver_in_network << " are not part of the network." << endl;
      }

      // CASE 2: One of the clients (sender OR receiver) are NOT in the network
      else if(response_from_main.size() == 1){  // size==1
         string no_client_in_network = response_from_main.at(0);

         // Sub-Case: SENDER is NOT in the network
         if(no_client_in_network == transfer_name){
            cout << "Unable to proceed with the transaction as " << no_client_in_network << " is not part of the network." << endl;
         }
         // Sub-Case: RECEIVER is NOT in the network
         else if(no_client_in_network == receive_name){
            cout << "Unable to proceed with the transaction as " << no_client_in_network << " is not part of the network." << endl;
         }
      }

      // CASE 3: Sender does NOT have sufficient balance
      else if(response_from_main.size() == 3){  // size==3
         string sender = response_from_main.at(0);
         string receiver = response_from_main.at(1);
         string sender_balance = response_from_main.at(2);
         cout << sender << " was unable to transfer " << amount_transfer << " txcoins to " << receiver << " because of insufficient balance." << endl;
         cout << "The current balance of " << sender << " is: " << sender_balance << " txcoins." << endl;
      }

      // CASE 4: Successful transaction
      else{    // size==4 (consist of new balance, sender, receiver, "SUCCESS")
         string balance_outcome = response_from_main.at(0);
         cout << transfer_name << " successfully transferred " << amount_transfer << " txcoins to " << receive_name << endl;
         cout << "The current balance of " << transfer_name << " is " << balance_outcome << " txcoins." << endl;
      }

   }

   // must close client socket
   close(client_sockfd_TCP);
   return 0;
}