Final Project
Name: Steve Regala
Student ID: 7293040280
Email: sregala@usc.edu

** Instructions **:
- Use "make" to compile. If necessary, use "make clean" to remove any possible object files.

** Program Information **:

Algorithm for handling transaction case
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


      For a SUCCESSFUL transaction case:
         // have a receiver code here, main server will send a string particular to a backend server  
         // all the backend servers will receive that string, process it --> if it's meant for that particular server, that server will proceed to store transaction
         // it will be sending over <sender> <receiver> <amount> <highest serial num+1> <server indicator>, each server will act according to the server indicator
         // servers will then send back the balance of the transferer back to the main server


Algorithm for TXLIST case
   - When the main server sends the request to the servers, the main server will print out that it has sent a request to the servers
   - Similarly, when the main server receives information for TXLIST from the servers, it will print out that it has received TXLIST information from them
   // continuously receive data from 
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


** Citations/Descriptions **:
Beej's socket programming in all the files, in functions ________, _____, _____

Sorting algorithm used the pseudo-code and example in https://www.softwaretestinghelp.com/quick-sort/
