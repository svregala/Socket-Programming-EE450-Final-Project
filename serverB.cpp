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
#include <fstream>
#include <vector>
#include <charconv>
#include <map>
#include <unordered_map>
#include <climits>

using namespace std;

/**
 * Hardcoded constants
 */
#define LOCALHOST "127.0.0.1"
#define B_SERVER_PORT_UDP 22280  // main server will connect to this, server B port number
#define MAXBUFLEN 1000     // max number of bytes at once


/**
 * Global variables
 */
int serverB_sockfd_UDP;
struct sockaddr_in serverB_addr, main_server_addr; // server B's address info and connector's (main server) address info
socklen_t sin_size_main_server;

unordered_map<string, int> transfer_amount;  // will hold the total transferred amount of each person in the network
unordered_map<string, int> receive_amount;   // will hold the total received amount of each person in the network
map<int, vector<string> > ordered_transactions; // will hold the ordered transactions from each text file, key=serial number, value = vector of strings (transferer, receiver, amount)

char input_from_main_server[MAXBUFLEN];
char send_back_main_server[MAXBUFLEN];


/**
 * Create server B UDP socket, connect to main server, and bind
 */
void serverB_socket_UDP(){

   if((serverB_sockfd_UDP = socket(PF_INET, SOCK_DGRAM, 0)) == -1){
      perror("ERROR: Server B failed to create socket for main server");
      exit(1);
   }

   serverB_addr.sin_family = AF_INET;
   serverB_addr.sin_port = htons(B_SERVER_PORT_UDP);
   serverB_addr.sin_addr.s_addr = inet_addr(LOCALHOST);
   memset(&(serverB_addr.sin_zero), '\0', 8);   // set everything else to 0

   // bind to associate socket with port number and IP address of server B side
   // need :: (global scope resolution operator) to use bind for sockets from socket.h
   if(::bind(serverB_sockfd_UDP, (struct sockaddr *)&serverB_addr, sizeof(struct sockaddr)) == -1){
      perror("ERROR: Server B could not bind UDP socket");
      exit(1);
   }

   cout << "The ServerB is up and running using UDP on port " << B_SERVER_PORT_UDP << "." << endl; 
}


/**
 * Helper function to decipher coded name
 */
string decoder(string s){

   string result = "";
   int temp_num =0;
   int sub_num = 0;
   for(int i=0; i<(int)s.length(); i++){
      // deal with letters
      if( ((int)s[i]>64 && (int)s[i]<91) || ((int)s[i]>96 && (int)s[i]<123) ){
         // upper case
         if(isupper(s[i])){

            temp_num = s[i];
            if(temp_num == 67 || temp_num == 66 || temp_num == 65){
               if(temp_num==67){ // if character is 'C'
                  result+=char(90);
               }else if(temp_num==66){ // if character is 'B'
                  result+=char(89);
               }else{   // if character is 'A'
                  result+=char(88);
               }
            }else{
               temp_num = s[i]-3;
               result+=char(temp_num);
            }
         // lower case
         }else{

            temp_num = s[i];
            if(temp_num == 99 || temp_num == 98 || temp_num == 97){
               if(temp_num == 99){
                  result+=char(122);
               }else if(temp_num == 98){
                  result+=char(121);
               }else{
                  result+=char(120);
               }
            }else{
               temp_num = s[i]-3;
               result+=char(temp_num);
            }

         }
      }

      // deal with numbers
      else if((int)s[i]>47 && (int)s[i]<58){
         temp_num = s[i];
         if(temp_num == 50 || temp_num == 49 || temp_num == 48){
            if(temp_num == 50){  // if character is '2'
               result+=char(57);
            }else if(temp_num == 49){  // if character is '1'
               result+=char(56);
            }else{   // if character is '0'
               result+=char(55);
            }
         }else{
            sub_num = stoi(to_string(s[i]));
            sub_num -= 3;
            result += char(sub_num);
         }
      }

      // deal with anything else, e.g. '!', '.', '?', etc.
      else {
         result += s[i];
      }
   }

   return result;
}


/**
 * Read in block2.txt and create reference for when balance is needed
 */
void create_transfer_receive_map(string file_name){

   string serial_num, transferer, receiver, amount;
   ifstream infile(file_name);
   string line;

   while(getline(infile, line)){

      int add_num=0;
      int sub_num=0;
      vector<string> value_vect;

      if(line.length()!=0){
         istringstream iss(line);
         iss >> serial_num >> transferer >> receiver >> amount;

         int final_serial = stoi(serial_num);
         string final_transferer = decoder(transferer);
         string final_receiver = decoder(receiver);
         int final_amount = stoi(decoder(amount));

         // fill in transfer_amount unordered map
         if(transfer_amount.count(final_transferer)<=0){    // if element is NOT already in the map
            sub_num -= final_amount;
            transfer_amount.insert(make_pair(final_transferer, sub_num));
         }else{   // if element is IN map
            sub_num = transfer_amount.at(final_transferer);
            sub_num -= final_amount;
            transfer_amount[final_transferer] = sub_num;
         }

         // fill in receive_amount unordered map
         if(receive_amount.count(final_receiver)<=0){    // if element is NOT already in the map
            add_num += final_amount;
            receive_amount.insert(make_pair(final_receiver, add_num));
         }else{   // if element is IN map
            add_num = receive_amount.at(final_receiver);
            add_num += final_amount;
            receive_amount[final_receiver] = add_num;
         }


         // fill in ordered_transactions ordered map
         value_vect.push_back(final_transferer);
         value_vect.push_back(final_receiver);
         value_vect.push_back(decoder(amount));
         ordered_transactions.insert(make_pair(final_serial, value_vect));
         value_vect.clear();
      }
   }

}


/**
 * Calculates the balance from the transaction file that Server B is assigned to
 */
int calculate_balance(string input){

   int amount_rec = 0;
   int amount_trans = 0;
   if(receive_amount.count(input)>0){
      amount_rec = receive_amount.at(input);
   }
   if(transfer_amount.count(input)>0){
      amount_trans = transfer_amount.at(input);
   }

   if((transfer_amount.count(input)<=0) && (receive_amount.count(input)<=0)){    // if given input is NOT in either map
      return INT_MIN;
   }

   return amount_rec + amount_trans;   // summation because transfer amount is negative
}


/**
 * Read in string sent from main server and place in vector
 */
vector<string> read_input_from_main(string s){
   string word;
   istringstream iss(s);
   vector<string> result;

   while(iss >> word){
      result.push_back(word);
   }
   return result;
}


int main(int argc, char* argv[]){

   serverB_socket_UDP();
   create_transfer_receive_map("block2.txt");

   while(1){

      // receive data from main server
      sin_size_main_server = sizeof(main_server_addr);
      if(recvfrom(serverB_sockfd_UDP, input_from_main_server, sizeof(input_from_main_server), 0, (struct sockaddr *)&main_server_addr, &sin_size_main_server) == -1){
         perror("ERROR: Server B failed to receive data from the main server");
         exit(1);
      }
      cout <<  "The ServerB received a request from the Main Server." << endl;

      string check_monitor_case = string(input_from_main_server);
      vector<string> temp_vect = read_input_from_main(input_from_main_server);
      // return either negative or positive amount (if statement) OR update the text files for transactions (else if) OR return string of transactions for monitor (else)
      if((temp_vect.size()==1) && (check_monitor_case != "TXLIST")){   // BALANCE CASE

         int temp_balance = calculate_balance(input_from_main_server);
         string temp_string = to_string(temp_balance);
         strcpy(send_back_main_server, temp_string.c_str());

         if(sendto(serverB_sockfd_UDP, send_back_main_server, sizeof(send_back_main_server), 0, (struct sockaddr *)&main_server_addr, sizeof(main_server_addr)) == -1){
            perror("ERROR: Server B failed to send data to the main server");
            exit(1);
         }

         cout << "The ServerB finished sending the response to the Main Server." << endl;
      }
      
      else if(temp_vect.size() == 3){  // TRANSACTION CASE

         // check if one or both users are in the network, send back to main server the balance and highest serial number
         string transfer_person = temp_vect.at(0);
         string receive_person = temp_vect.at(1);
         // (if transferer is in transfer map OR in receiver map) AND (if receiver is in transfer map OR in receiver map) ==> BOTH users are in the network B
         if((transfer_amount.count(transfer_person)>0 || receive_amount.count(transfer_person)>0) && (transfer_amount.count(receive_person)>0 || receive_amount.count(receive_person)>0)){
            int transferrer_balance = calculate_balance(transfer_person);
            int highest_serial_num = ordered_transactions.rbegin()->first;
            string temp_send = to_string(transferrer_balance) + " " + to_string(highest_serial_num) + " " + "BOTH";
            strcpy(send_back_main_server, temp_send.c_str());
         }

         // (if ONLY transferrer is in transfer map OR receiver map) OR (if ONLY receiver is in the transfer map OR receiver map) ==> TRANSFERRER or RECEIVER is in network B
         else if((transfer_amount.count(transfer_person)>0 || receive_amount.count(transfer_person)>0) || (transfer_amount.count(receive_person)>0 || receive_amount.count(receive_person)>0)){
            // check to see which one is in the network (transferrer or receiver)
            // only transferrer in the network
            if(transfer_amount.count(transfer_person)>0 || receive_amount.count(transfer_person)>0){
               int transferrer_balance = calculate_balance(transfer_person);
               int highest_serial_num = ordered_transactions.rbegin()->first;
               string temp_send = to_string(transferrer_balance) + " " + to_string(highest_serial_num) + " " + "TRANSFERRER";
               strcpy(send_back_main_server, temp_send.c_str());
            }
            // only receiver in the network
            else if(transfer_amount.count(receive_person)>0 || receive_amount.count(receive_person)){
               int transferrer_balance = 0;
               int highest_serial_num = ordered_transactions.rbegin()->first;
               string temp_send = to_string(transferrer_balance) + " " + to_string(highest_serial_num) + " " + "RECEIVER";
               strcpy(send_back_main_server, temp_send.c_str());
            }
         }

         // neither of them are in network B
         else{
            int transferrer_balance = 0;
            int highest_serial_num = ordered_transactions.rbegin()->first;
            string temp_send = to_string(transferrer_balance) + " " + to_string(highest_serial_num) + " " "NONE";
            strcpy(send_back_main_server, temp_send.c_str());
         }

         // to the main server: return balance of transferrer, highest serial number, AND which one or both exists in network B (transferrer, receiver, OR Both)
         if(sendto(serverB_sockfd_UDP, send_back_main_server, sizeof(send_back_main_server), 0, (struct sockaddr *)&main_server_addr, sizeof(main_server_addr)) == -1){
            perror("ERROR: Server A failed to send data to the main server");
            exit(1);
         }

         cout << "The ServerB finished sending the response to the Main Server." << endl;
      }

      else{    // Monitor case

      }




      /*
      For transactions
         - have a global variable that we update each time we process a transaction, to correctly establish the serial number
      */


      /*
      - ordered map -> int, the rest of the string (vector of strings)
         - takes care of monitor request (TXLIST)
      - undordered map receive -> string name, int value (everything received)
      - unordered map sent -> string name, int value (everything sent)
         - takes care of check balance, transferring money
      */
   }

   close(serverB_sockfd_UDP);
   return 0;
}