Final Project
Name: Steve Regala
Student ID: 7293040280
Email: sregala@usc.edu

** Instructions **: ------------------------------------------------------------------------------------------------------------------------------
- Use "make" to compile. If necessary, use "make clean" to remove any possible object files.

** Program Information **: ------------------------------------------------------------------------------------------------------------------------------

The following algorithm is for handling the transaction case:
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


      For a SUCCESSFUL transaction case:
         // have a receiver code here, main server will send a string particular to a backend server  
         // all the backend servers will receive that string, process it --> if it's meant for that particular server, that server will proceed to store transaction
         // it will be sending over <sender> <receiver> <amount> <highest serial num+1> <server indicator>, each server will act according to the server indicator
         // servers will then send back the balance of the transferer back to the main server
*/


The following algorithm is for handling the TXLIST case:
/*
   - When the main server sends the request to the servers, the main server will print out that it has sent a request to the servers
   - Similarly, when the main server receives information for TXLIST from the servers, it will print out that it has received TXLIST information from them
   // continuously receive data from all the backend servers
   /*
      - store incoming transaction lines into a list that is later going to get sorted
      - have a global variable corresponding to all the servers A,B,C --> say num_transactions_A, num_transactions_B, num_transactions_C
      - Before sending TXLIST to the backend servers, first acquire the size of each ordered map from each of the servers
         - use this value as a length for the for loop - the amount of time that we're going to receive from each server
         - each server is going to send each transaction line one at a time and that will be put into the global list (transaction_list)

      - the same case should apply to the relationship between the monitor and the main server, 
            - the monitor will first get the length from the main server, this value will be the length of the forloop in the client, when it is reading in
               the sorted lines from the main server
   
   */

   /*
      - ordered map -> int, the rest of the string (vector of strings)
         - takes care of monitor request (TXLIST)
      - undordered map receive -> string name, int value (everything received)
      - unordered map sent -> string name, int value (everything sent)
         - takes care of check balance, transferring money
      */


The following content is the messages printed out on the main server side when handling TXLIST case:
   Main server message output for when monitor sends a sorted list request:
   The main server received a sorted list request from the monitor using TCP over port 26280.
   The main server sent a request to server A.
   The main server sent a request to server B.
   The main server sent a request to server C.
   The main server received TXLIST data from Server A using UDP over port 24280
   The main server received TXLIST data from Server B using UDP over port 24280
   The main server received TXLIST data from Server C using UDP over port 24280
   The main server confirms the list of sorted transactions has been generated.


** Citations/Descriptions **: ------------------------------------------------------------------------------------------------------------------------------

- I used Beej's socket programming in all the files (client.cpp, monitor.cpp, serverA.cpp, serverB.cpp, serverC.cpp).
- The following functions in the following files used code that was from Beej's socket programming:
   client.cpp: client_socket_TCP and the main function, i.e. the code with sending and receiving from a socket.
   monitor.cpp: monitor_socket_TCP and the main function, i.e. the code with sending and receiving from a socket.
   serverM.cpp: client_socket_TCP, monitor_socket_TCP, server_socket_UDP, client_monitor_listen, connect_servers, client_operations, monitor_operations, and the main function, i.e. the code with sending and receiving from a socket.
   serverA: serverA_socket_UDP and the main function, i.e. the code with sending and receiving from a socket.
   serverB: serverB_socket_UDP and the main function, i.e. the code with sending and receiving from a socket.
   serverC: serverC_socket_UDP and the main function, i.e. the code with sending and receiving from a socket.


- I used the QuickSort sorting algorithm and the pseudo-code in the example presented in https://www.softwaretestinghelp.com/quick-sort/.
   - The code can be seen in serverM.cpp in the functions: quick_sort, sort_partition, and swap.
