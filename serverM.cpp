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
#include <sstream>
#include <vector>
#include <climits>

using namespace std;

/**
 * Hardcoded constants
 */
#define LOCALHOST "127.0.0.1"
#define CLIENT_PORT_TCP 25280    // client connects to this
#define MONITOR_PORT_TCP 26280   // monitor connects to this
#define SERVER_PORT_UDP 24280    // servers A,B,C connect to this
#define A_SERVER_PORT_UDP 21280
#define B_SERVER_PORT_UDP 22280
#define C_SERVER_PORT_UDP 23280
#define BACKLOG 10   // amount of pending connections queue will hold
#define MAXBUFLEN 1000     // max number of bytes at once


/**
 * Global variables
 */
int client_sockfd_TCP, monitor_sockfd_TCP, server_sockfd_UDP; // parent sockets for requests
int client_sockfd_child, monitor_sockfd_child; // child sockets for data exchange
struct sockaddr_in client_addr_main, monitor_addr_main, server_addr_UDP; // main server address info
struct sockaddr_in client_addr, monitor_addr, serverA_addr, serverB_addr, serverC_addr; // Connector's address info
socklen_t sin_size_client, sin_size_monitor, sin_size_server_A, sin_size_server_B, sin_size_server_C;

char input_from_client[MAXBUFLEN];
char send_to_servers_ABC[MAXBUFLEN];
char A_receive_from_servers[MAXBUFLEN];
char B_receive_from_servers[MAXBUFLEN];
char C_receive_from_servers[MAXBUFLEN];
char send_back_to_client[MAXBUFLEN];



/**
 * Create a client TCP socket and bind
 */
void client_socket_TCP(){
   
   // create TCP socket for client
   if((client_sockfd_TCP = socket(PF_INET, SOCK_STREAM, 0)) == -1){
      perror("ERROR: Main server failed to create socket for client");
      exit(1);
   }

   client_addr_main.sin_family = AF_INET; 
   client_addr_main.sin_port = htons(CLIENT_PORT_TCP);
   client_addr_main.sin_addr.s_addr = inet_addr(LOCALHOST);
   memset(&(client_addr_main.sin_zero), '\0', 8); // set everything else to 0

   // bind to associate socket with port number and IP address of server side
   // need :: (global scope resolution operator) to use bind for sockets from socket.h
   if(::bind(client_sockfd_TCP, (struct sockaddr *)&client_addr_main, sizeof(struct sockaddr)) == -1){
      perror("ERROR: Main server could not bind client socket");
      exit(1);
   }

}


/**
 * Create a monitor TCP socket and bind
 */
void monitor_socket_TCP(){

   // create TCP socket for monitor
   if((monitor_sockfd_TCP = socket(PF_INET, SOCK_STREAM, 0)) == -1){
      perror("ERROR: Main server failed to create socket for monitor");
      exit(1);
   }

   monitor_addr_main.sin_family = AF_INET;
   monitor_addr_main.sin_port = htons(MONITOR_PORT_TCP);
   monitor_addr_main.sin_addr.s_addr = inet_addr(LOCALHOST);
   memset(&(monitor_addr_main.sin_zero), '\0', 8); // set everything else to 0

   // bind to associate socket with port number and IP address of server side
   // need :: (global scope resolution operator) to use bind for sockets from socket.h
   if(::bind(monitor_sockfd_TCP, (struct sockaddr *)&monitor_addr_main, sizeof(struct sockaddr)) == -1){
      perror("ERROR: Main server could not bind monitor socket");
      exit(1);
   }

}


/**
 * Create a UDP socket and bind
 */
void server_socket_UDP(){

   // create a UDP socket
   if((server_sockfd_UDP = socket(PF_INET, SOCK_DGRAM, 0)) == -1){
      perror("ERROR: Main server could now create UDP socket");
      exit(1);
   }

   server_addr_UDP.sin_family = AF_INET;
   server_addr_UDP.sin_port = htons(SERVER_PORT_UDP);
   server_addr_UDP.sin_addr.s_addr = inet_addr(LOCALHOST);
   memset(&(server_addr_UDP.sin_zero), '\0', 8); // set everything else to 0

   // bind to associate socket with port number and IP address of server side
   // need :: (global scope resolution operator) to use bind for sockets from socket.h
   if(::bind(server_sockfd_UDP, (struct sockaddr *)&server_addr_UDP, sizeof(struct sockaddr)) == -1){
      perror("ERROR: Main server could not bind UDP socket");
      exit(1);
   }

}


/**
 * Prepare client and monitor socket for incoming connections; only for stream-oriented (connection-oriented) data modes
 */
void client_monitor_listen(){
   if(listen(client_sockfd_TCP, BACKLOG) == -1){
      perror("ERROR: Main server could not listen for client socket");
      exit(1);
   }

   if(listen(monitor_sockfd_TCP, BACKLOG) == -1){
      perror("ERROR: Main server could not listen for monitor socket");
      exit(1);
   }
}


/**
 * Connect main server to servers A,B,C
 */
void connect_servers(){

   // connection with server A
   serverA_addr.sin_family = AF_INET;
   serverA_addr.sin_port = htons(A_SERVER_PORT_UDP);
   serverA_addr.sin_addr.s_addr = inet_addr(LOCALHOST);
   memset(&(serverA_addr.sin_zero), '\0', 8);   // set everything else to 0

   // connection with server B
   serverB_addr.sin_family = AF_INET;
   serverB_addr.sin_port = htons(B_SERVER_PORT_UDP);
   serverB_addr.sin_addr.s_addr = inet_addr(LOCALHOST);
   memset(&(serverB_addr.sin_zero), '\0', 8);   // set everything else to 0

   // connection with server C
   serverC_addr.sin_family = AF_INET;
   serverC_addr.sin_port = htons(C_SERVER_PORT_UDP);
   serverC_addr.sin_addr.s_addr = inet_addr(LOCALHOST);
   memset(&(serverC_addr.sin_zero), '\0', 8);   // set everything else to 0

}


/**
 * Read in string sent from client and place in vector
 */
vector<string> read_input_from_client(string s){
   string word;
   istringstream iss(s);
   vector<string> result;

   while(iss >> word){
      result.push_back(word);
   }
   return result;
}


/**
 * Perform client operations
 */
void client_operations(){

   sin_size_client = sizeof(struct sockaddr_in);
   if((client_sockfd_child = accept(client_sockfd_TCP, (struct sockaddr *)&client_addr, &sin_size_client)) == -1){
      perror("ERROR: Main server failed to accept connection with client");
      exit(1);
   }

   // received information from client through child socket
   if(recv(client_sockfd_child, input_from_client, MAXBUFLEN, 0) == -1){
      perror("ERROR: Main server failed to receive information from client");
      exit(1);
   }

   // initialize connection with UDP servers A,B,C
   connect_servers();

   vector<string> temp_vect = read_input_from_client(string(input_from_client));
   // return balance of the name (if statement) OR return transaction (else statement)
   // ------------------------------------------------------- BALANCE CASE -------------------------------------------------------
   if(temp_vect.size()==1){

      cout << "The main server received input=" << input_from_client << " from the client using TCP over port " << CLIENT_PORT_TCP << "." << endl;

      // copy input_from_client into send_to_servers_ABC, then send it out to the servers
      strcpy(send_to_servers_ABC, input_from_client);

      // SERVER A --> SEND AND RECEIVE -------------------------------------------------------
      if(sendto(server_sockfd_UDP, send_to_servers_ABC, sizeof(send_to_servers_ABC), 0, (struct sockaddr *)&serverA_addr, sizeof(serverA_addr)) == -1){
         perror("ERROR: Main server failed to send data to server A");
         exit(1);
      }
      cout << "The main server sent a request to server A." << endl;

      sin_size_server_A = sizeof(serverA_addr);
      if(recvfrom(server_sockfd_UDP, A_receive_from_servers, sizeof(A_receive_from_servers), 0, (struct sockaddr *)&serverA_addr, &sin_size_server_A) == -1){
         perror("ERROR: Main server failed to receive data from server A");
         exit(1);
      }
      cout << "The main server received transactions from Server A using UDP over port " << SERVER_PORT_UDP << endl;
      

      // SERVER B --> SEND AND RECEIVE -------------------------------------------------------
      if(sendto(server_sockfd_UDP, send_to_servers_ABC, sizeof(send_to_servers_ABC), 0, (struct sockaddr *)&serverB_addr, sizeof(serverB_addr)) == -1){
         perror("ERROR: Main server failed to send data to server B");
         exit(1);
      }
      cout << "The main server sent a request to server B." << endl;

      sin_size_server_B = sizeof(serverB_addr);
      if(recvfrom(server_sockfd_UDP, B_receive_from_servers, sizeof(B_receive_from_servers), 0, (struct sockaddr *)&serverB_addr, &sin_size_server_B) == -1){
         perror("ERROR: Main server failed to receive data from server B");
         exit(1);
      }
      cout << "The main server received transactions from Server B using UDP over port " << SERVER_PORT_UDP << endl;


      // SERVER C --> SEND AND RECEIVE -------------------------------------------------------
      if(sendto(server_sockfd_UDP, send_to_servers_ABC, sizeof(send_to_servers_ABC), 0, (struct sockaddr *)&serverC_addr, sizeof(serverC_addr)) == -1){
         perror("ERROR: Main server failed to send data to server C");
         exit(1);
      }
      cout << "The main server sent a request to server C." << endl;

      sin_size_server_C = sizeof(serverC_addr);
      if(recvfrom(server_sockfd_UDP, C_receive_from_servers, sizeof(C_receive_from_servers), 0, (struct sockaddr *)&serverC_addr, &sin_size_server_C) == -1){
         perror("ERROR: Main server failed to receive data from server C");
         exit(1);
      }
      cout << "The main server received transactions from Server C using UDP over port " << SERVER_PORT_UDP << endl;


      // combine results from A, B, C
      int A_temp_result = atoi(A_receive_from_servers);
      int B_temp_result = atoi(B_receive_from_servers);
      int C_temp_result = atoi(C_receive_from_servers);

      // case 1: user that requested for balance is NOT in the network
      if((A_temp_result==INT_MIN) && (B_temp_result==INT_MIN) && (C_temp_result==INT_MIN)){
         strcpy(send_back_to_client, "NEGATIVE");
      }
      // case2: user that requested for balance IS in the network
      else{
         int final_result = 1000;
         if(A_temp_result != INT_MIN){
            final_result += A_temp_result;
         }
         if(B_temp_result != INT_MIN){
            final_result += B_temp_result;
         }
         if(C_temp_result != INT_MIN){
            final_result += C_temp_result;
         }
         string positive_result = to_string(final_result);
         strcpy(send_back_to_client, positive_result.c_str());
      }

      // send response back to client
      if(send(client_sockfd_child, send_back_to_client, sizeof(send_back_to_client), 0) == -1){
         perror("ERROR: Main server failed to send data back to client server.");
         exit(1);
      }
      cout << "The main server sent the current balance to the client." << endl;

   }
   
   // ------------------------------------------------------- TRANSACTION CASE -------------------------------------------------------
   else{

      string client_sender = temp_vect.at(0);
      string client_receiver = temp_vect.at(1);
      int client_amount = stoi(temp_vect.at(2));

      cout << "The main server received from " << client_sender << " to transfer " << client_amount << " coins to " << client_receiver << " using TCP over port " << CLIENT_PORT_TCP << "." << endl;

      // *********** SEND BOTH USERS TO ALL THREE SERVERS TO CHECK IF BOTH USERS ARE PART OF THE NETWORK ***********
      // copy input_from_client into send_to_servers_ABC, then send it out to the servers
      strcpy(send_to_servers_ABC, input_from_client);

      // SERVER A --> SEND AND RECEIVE (CHECKING IF BOTH USERS EXIST IN THE NETWORK) -------------------------------------------------------
      if(sendto(server_sockfd_UDP, send_to_servers_ABC, sizeof(send_to_servers_ABC), 0, (struct sockaddr *)&serverA_addr, sizeof(serverA_addr)) == -1){
         perror("ERROR: Main server failed to send data to server A");
         exit(1);
      }
      cout << "The main server sent a request to server A." << endl;

      sin_size_server_A = sizeof(serverA_addr);
      if(recvfrom(server_sockfd_UDP, A_receive_from_servers, sizeof(A_receive_from_servers), 0, (struct sockaddr *)&serverA_addr, &sin_size_server_A) == -1){
         perror("ERROR: Main server failed to receive data from server A");
         exit(1);
      }
      cout << "The main server received the feedback from server A using UDP over port " << SERVER_PORT_UDP << endl;

      // SERVER B --> SEND AND RECEIVE (CHECKING IF BOTH USERS EXIST IN THE NETWORK) -------------------------------------------------------
      if(sendto(server_sockfd_UDP, send_to_servers_ABC, sizeof(send_to_servers_ABC), 0, (struct sockaddr *)&serverB_addr, sizeof(serverB_addr)) == -1){
         perror("ERROR: Main server failed to send data to server B");
         exit(1);
      }
      cout << "The main server sent a request to server B." << endl;

      sin_size_server_B = sizeof(serverB_addr);
      if(recvfrom(server_sockfd_UDP, B_receive_from_servers, sizeof(B_receive_from_servers), 0, (struct sockaddr *)&serverB_addr, &sin_size_server_B) == -1){
         perror("ERROR: Main server failed to receive data from server B");
         exit(1);
      }
      cout << "The main server received the feedback from server B using UDP over port " << SERVER_PORT_UDP << endl;

      // SERVER C --> SEND AND RECEIVE (CHECKING IF BOTH USERS EXIST IN THE NETWORK) -------------------------------------------------------
      if(sendto(server_sockfd_UDP, send_to_servers_ABC, sizeof(send_to_servers_ABC), 0, (struct sockaddr *)&serverC_addr, sizeof(serverC_addr)) == -1){
         perror("ERROR: Main server failed to send data to server C");
         exit(1);
      }
      cout << "The main server sent a request to server C." << endl;

      sin_size_server_C = sizeof(serverC_addr);
      if(recvfrom(server_sockfd_UDP, C_receive_from_servers, sizeof(C_receive_from_servers), 0, (struct sockaddr *)&serverC_addr, &sin_size_server_C) == -1){
         perror("ERROR: Main server failed to receive data from server C");
         exit(1);
      }
      cout << "The main server received the feedback from server C using UDP over port " << SERVER_PORT_UDP << endl;

      // combine data from 3 servers confirming both users are in the network
      vector<string> result_from_A = read_input_from_client(A_receive_from_servers);
      int A_compute_balance = stoi(result_from_A.at(0));
      int A_highest_serial = stoi(result_from_A.at(1));
      string who_in_A = result_from_A.at(2); // either "BOTH", "TRANSFERRER", "RECEIVER", or "NONE"

      vector<string> result_from_B = read_input_from_client(B_receive_from_servers);
      int B_compute_balance = stoi(result_from_B.at(0));
      int B_highest_serial = stoi(result_from_B.at(1));
      string who_in_B = result_from_B.at(2); // either "BOTH", "TRANSFERRER", "RECEIVER", or "NONE"

      vector<string> result_from_C = read_input_from_client(C_receive_from_servers);
      int C_compute_balance = stoi(result_from_C.at(0));
      int C_highest_serial = stoi(result_from_C.at(1));
      string who_in_C = result_from_C.at(2); // either "BOTH", "TRANSFERRER", "RECEIVER", or "NONE"

      // check to see if both sender and receiver exist in the overall network
      // CASE 1: NEITHER the sender nor the receiver are in the network
      if((who_in_A == "NONE") && (who_in_B == "NONE") && (who_in_C == "NONE")){
         string both_not_in_network = client_sender + " " + client_receiver;
         strcpy(send_back_to_client, both_not_in_network.c_str());

         // send response back to client
         if(send(client_sockfd_child, send_back_to_client, sizeof(send_back_to_client), 0) == -1){
            perror("ERROR: Main server failed to send data back to client server.");
            exit(1);
         }
      }

      // CASE 2: One of the clients (sender OR receiver) are NOT in the network
      // Sub-case 1: Sender/Transferrer is NOT in the network
      else if((who_in_A == "RECEIVER" || who_in_A == "NONE") && (who_in_B == "RECEIVER" || who_in_B == "NONE") && (who_in_C == "RECEIVER" || who_in_C == "NONE")){
         string transferrer_not_in_network = client_sender;
         strcpy(send_back_to_client, transferrer_not_in_network.c_str());

         // send response back to client
         if(send(client_sockfd_child, send_back_to_client, sizeof(send_back_to_client), 0) == -1){
            perror("ERROR: Main server failed to send data back to client server.");
            exit(1);
         }
      }
      // Sub-case 2: Receiver is NOT in the network
      else if((who_in_A == "TRANSFERRER" || who_in_A == "NONE") && (who_in_B == "TRANSFERRER" || who_in_B == "NONE") && (who_in_C == "TRANSFERRER" || who_in_C == "NONE")){
         string receiver_not_in_network = client_receiver;
         strcpy(send_back_to_client, receiver_not_in_network.c_str());

         // send response back to client
         if(send(client_sockfd_child, send_back_to_client, sizeof(send_back_to_client), 0) == -1){
            perror("ERROR: Main server failed to send data back to client server.");
            exit(1);
         }
      }

      // CASE 3: Both sender & receiver exist in the network
      else{
         int sender_balance = 1000;
         sender_balance = sender_balance + A_compute_balance + B_compute_balance + C_compute_balance;

         // Sub-case 1: Sender does NOT have enough coins to transfer
         if(sender_balance < client_amount){
            string not_enough_coins = client_sender + " " + client_receiver + " " + to_string(sender_balance);
            strcpy(send_back_to_client, not_enough_coins.c_str());

            // send response back to client
            if(send(client_sockfd_child, send_back_to_client, sizeof(send_back_to_client), 0) == -1){
               perror("ERROR: Main server failed to send data back to client server.");
               exit(1);
            }
         }

         // Sub-case 2: Successful transaction because Sender has enough coins to transfer
         else{
            cout << endl;
         }

      }

      

      cout << "The main server sent the result of the transaction to the client." << endl;


      /*
      ./client Steve Fernando 100

      [CASE 1]: It is a successful transfer
         - Main server receives the request from client --> 
            - First, send information to all three backend servers to check if both users are part of the network
               - if they both are, have the backend servers return the balance of the transferer AND all their highest SERIAL numbers
                  - [CASE 2]: if one or both of the users are NOT a part of the network, backend servers will tell the main server, then main server will tell client
               - using the balance, check to see if the transferer has enough coins to transfer (>= transfer amount)
                  - [CASE 3]: if the transferer does NOT have enough coins to transfer
               - if user has enough to transfer, main server will send the transaction to a random backend server ALONG with the maximum 
                  - the random backend server will store it in their designated block.txt file
               - the random backend server will let the main server know it was successful
               - the main server will send the balance minus transfer amount to the client
               - the client will output the success message and the new balance of the transferer
                  - (possibly use the balance as an indicator that it was successful)
      
      [CASE 2]: if one or both of the users are NOT a part of the network, the backend servers will tell the main server
         - then the main server will relay the message to the client
         - the client will output message: unable to transfer because one or both users are not part of the network

      [CASE 3]: if the transfere does NOT have enough coins to transfer
         - then the main server will relay the message to the client
         - the client will output message: unable to transfer because transferer has insufficient balance
      */
   }

}



/**
 * Perform monitor operations
 */
void monitor_operations(){

   sin_size_monitor = sizeof(struct sockaddr_in);
      if((monitor_sockfd_child = accept(monitor_sockfd_TCP, (struct sockaddr *)&monitor_addr, &sin_size_monitor)) == -1){
         perror("ERROR: Main server failed to accept connection with monitor");
         exit(1);
      }

}


int main(int argc, char* argv[]){

   client_socket_TCP();
   monitor_socket_TCP();
   client_monitor_listen();
   server_socket_UDP();

   cout << "The main server is up and running." << endl;

   while(1){

      // FIRST accept, receive, compute, send sequence for CLIENT -------------------------------------------------------
      client_operations();

      // SECOND accept, receive, compute, send sequence for CLIENT -------------------------------------------------------
      client_operations();

      // THIRD accept, receive, compute, send sequence for MONITOR -------------------------------------------------------
      monitor_operations();
      
   }


   // must close socket parents
   close(client_sockfd_TCP);
   close(monitor_sockfd_TCP);
   close(server_sockfd_UDP);
   return 0;
}