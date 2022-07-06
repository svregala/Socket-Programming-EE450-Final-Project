Final Project
Name: Steve Regala
Student ID: 7293040280
Email: sregala@usc.edu


** PROJECT DESCRIPTION **: ------------------------------------------------------------------------------------------------------------------------------
- The whole project description can be read/found under EE450 Socket Programming Project.pdf.
- In this project, I've implemented files client.cpp, monitor.cpp, serverM.cpp, serverA.cpp, serverB.cpp, and serverC.cpp.
   - The backend servers A,B,C have designated text files that they access block1.txt, block2.txt, and block3.txt, respectively.
      - These files will hold encrypted transactions (serial number, sender, receiver, amount).
   - The client.cpp file (client server) will make requests and send those requests to the main server.
      - The requests consist of asking for a balance of a particular client, or inserting a transaction.
      - The main server will communicate with the backend servers and perform necessary calculations, or generate error message to send back to the client server.
   - The monitor.cpp file (monitor server) will make a sorted transactions list request and send it to the main server.
      - The main server will communicate with the backend servers and have the servers send all the existing transactions.
      - The main server will then sort this transactions list by serial num and generate the text file txchain.txt.
- The format of the messages is according to the project description pdf. 
   - However, when the main server handles the TXLIST operation, the following message will be produced by the main server:
      The main server received a sorted list request from the monitor using TCP over port 26280.
      The main server sent a request to server A.
      The main server sent a request to server B.
      The main server sent a request to server C.
      The main server received TXLIST data from Server A using UDP over port 24280
      The main server received TXLIST data from Server B using UDP over port 24280
      The main server received TXLIST data from Server C using UDP over port 24280
      The main server confirms the list of sorted transactions has been generated.


** INSTRUCTIONS **: -------------------------------------------------------------------------------------------------------------------------------------
- Use "make" or "make all" to compile. If necessary, use "make clean" to remove any possible object files.
- The program will be compiled using C++ version C++11.


** PROGRAM INFORMATION **: ------------------------------------------------------------------------------------------------------------------------------
- More details could be found in the project description.

The following algorithm is for handling the BALANCE case:
   - The main server will send the name to the backend servers, and they send a message back to the main server indicating that the client exists or not.
   - If the client exists, the backend servers will have sent a calculated balance; then send back the result to the client and output the success message.
   - Otherwise, the main server will send "NEGATIVE" to the client indicating that the person does not exist.

The following algorithm is for handling the TRANSACTION case:
/*
   ./client Steve Ben 100

   [CASE 1]: It is a successful transfer
      - Main server receives the request from client --> 
         - First, send information to all three backend servers to check if both users are part of the network
            - if they both are, have the backend servers return the balance of the transferer AND all their highest SERIAL numbers
               - [CASE 2]: if one or both of the users are NOT a part of the network, backend servers will tell the main server, then main server will tell client
            - using the balance, check to see if the transferer has enough coins to transfer (>= transfer amount)
               - [CASE 3]: if the transferer does NOT have enough coins to transfer
            - if user has enough to transfer, main server will send the transaction to all the backend servers with a key specifying which server should store the transaction
               - the random backend server will store it in their designated block.txt file, then all the backend servers will send the balance of the sender back to the main server
            - the backend servers will let the main server know it was successful
            - the main server will send the balance minus transfer amount to the client
            - the client will output the success message and the new balance of the transferer
               - (possibly use the balance as an indicator that it was successful)
         For a SUCCESSFUL transaction case:
         - have a receiver code here, main server will send a string particular to a backend server  
         - all the backend servers will receive that string, process it --> if it's meant for that particular server, that server will proceed to store transaction
         - it will be sending over <sender> <receiver> <amount> <highest serial num+1> <server indicator>, each server will act according to the server indicator
         - servers will then send back the balance of the transferer back to the main server
   
   [CASE 2]: if one or both of the users are NOT a part of the network, the backend servers will tell the main server
      - then the main server will relay the message to the client
      - the client will output message: unable to transfer because one or both users are not part of the network

   [CASE 3]: if the transfere does NOT have enough coins to transfer
      - then the main server will relay the message to the client
      - the client will output message: unable to transfer because transferer has insufficient balance
*/

The following algorithm is for handling the TXLIST case:
/*
   - When the main server sends the request to the servers, the main server will print out that it has sent a request to the servers
   - Similarly, when the main server receives information for TXLIST from the servers, it will print out that it has received TXLIST information from them
   - Then, it will continuously receive data from all the backend servers
   /*
      - store incoming transaction lines into a list that is later going to get sorted, using quick sort
      - have a variable corresponding to all the servers A,B,C representing the size of their designated text files --> say num_transactions_A, num_transactions_B, num_transactions_C
      - before sending TXLIST to the backend servers, first acquire the size of each ordered map from each of the servers
         - use this value as a length for the for loop - the amount of times that we're going to receive from each server
         - each server is going to send each transaction line one at a time and that will be put into the global list (transaction_list)
      - the main server will have generated a txchain.txt that stores all the transactions
   */

   /*
      The data structures used in the backend servers A,B,C are the following:
      - ordered map -> int, the rest of the string (vector of strings), takes care of monitor request (TXLIST)
      - undordered map receive -> string name of a client, int value (everything received)
      - unordered map sent -> string name of a client, int value (everything sent)
         - the unordered maps are used to take care of check balance and transferring money operations
   */

   The following content is the messages printed out on the main server side when handling TXLIST case:
   Main server message output for when monitor sends a sorted list request to the main server:
      The main server received a sorted list request from the monitor using TCP over port 26280.
      The main server sent a request to server A.
      The main server sent a request to server B.
      The main server sent a request to server C.
      The main server received TXLIST data from Server A using UDP over port 24280
      The main server received TXLIST data from Server B using UDP over port 24280
      The main server received TXLIST data from Server C using UDP over port 24280
      The main server confirms the list of sorted transactions has been generated.
*/


** CITATIONS/DESCRIPTIONS **: ------------------------------------------------------------------------------------------------------------------------------

- I referred to Beej's socket programming in all the files (client.cpp, monitor.cpp, serverA.cpp, serverB.cpp, serverC.cpp).
- The following functions in the following files used code that was from Beej's socket programming:
   client.cpp: client_socket_TCP and the main function, i.e. the code with connecting to the socket and sending/receiving from a socket.
      - this file will communicate with the main server about checking a balance or making a transaction
   monitor.cpp: monitor_socket_TCP and the main function, i.e. the code with connecting to the socket and sending/receiving from a socket.
      - this file will communicate with the main server about generating a txchain.txt file (a list of all transactions)
   serverM.cpp: client_socket_TCP, monitor_socket_TCP, server_socket_UDP, client_monitor_listen, connect_servers, client_operations, monitor_operations, and the main function, 
      i.e. the code with connecting to the socket and sending/receiving from a socket.
      - Additionally, in serverM.cpp with the functions quick_sort, sort_partition, and swap, I used the QuickSort sorting algorithm and the pseudo-code in the example presented in https://www.softwaretestinghelp.com/quick-sort/
      - this file will be the main file for this whole network, it will handle all the requests and communication with the backend servers
   serverA: serverA_socket_UDP and the main function, i.e. the code with connecting to the socket and sending/receiving from a socket.
      - this fille will communicate with the main server providing responses regarding checking balance, transactions, or TXLIST
   serverB: serverB_socket_UDP and the main function, i.e. the code with connecting to the socket and sending/receiving from a socket.
      - this fille will communicate with the main server providing responses regarding checking balance, transactions, or TXLIST
   serverC: serverC_socket_UDP and the main function, i.e. the code with connecting to the socket and sending/receiving from a socket.
      - this fille will communicate with the main server providing responses regarding checking balance, transactions, or TXLIST
