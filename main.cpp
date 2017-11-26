/* A simple server in the internet domain using TCP.
myServer.c
D. Thiebaut
Adapted from http://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/socket.html
The port number used in 51717.
This code is compiled and run on the Raspberry as follows:
   
    g++ -o myServer myServer.c 
    ./myServer

The server waits for a connection request from a client.
The server assumes the client will send positive integers, which it sends back multiplied by 2.
If the server receives -1 it closes the socket with the client.
If the server receives -2, it exits.
*/

#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include <iostream>
using namespace std;

const int tcpport = 51716;

void error( char *msg ) {
  perror(  msg );
  exit(1);
}

int func( int a ) {
   return 2 * a;
}

void sendData( int sockfd, int x ) {
  int n;

  char buffer[32];
  sprintf( buffer, "%d\n", x );
  if ( (n = write( sockfd, buffer, strlen(buffer) ) ) < 0 )
    error( const_cast<char *>( "ERROR writing to socket") );
  buffer[n] = '\0';
}

int getData( int sockfd ) {
  char buffer[32];
  int n;

  if ( (n = read(sockfd,buffer,31) ) < 0 )
    error( const_cast<char *>( "ERROR reading from socket") );
  buffer[n] = '\0';
  return atoi( buffer );
}

int main(int argc, char *argv[]) {

    cout << "## c++ TCP Server" << endl;

     int portno = tcpport;
     int sockfd, newsockfd, clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     int data;

    cout << "TCP using Port: " << portno << endl;
    //printf( "using port #%d\n", portno );
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // AF_INET is an address family that is used to designate the type of addresses that your socket can communicate with (in this case, Internet Protocol v4 addresses). 
    // SOCK_STREAM = Socket Filestream

    if (sockfd < 0) 
         error( const_cast<char *>("ERROR opening socket") );
    else 
      cout << "TCP Socket open" << endl;


     bzero((char *) &serv_addr, sizeof(serv_addr));
     // mit zero vorbelegen

     serv_addr.sin_family = AF_INET;    // AF_INET is an address family 
     serv_addr.sin_addr.s_addr = INADDR_ANY;  // It binds the socket to all available interfaces.
     serv_addr.sin_port = htons( portno );  // The htons function converts a u_short from host to TCP/IP network byte order (which is big-endian).


     if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
       error( const_cast<char *>( "ERROR on binding" ) );
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
  
     //--- infinite wait on a connection ---
     while ( 1 ) {
        printf( "TCP waiting for new client...\n" );

        // sockfd and newsockfd are file descriptors, i.e. array subscripts into the file descriptor table . These two variables store the values returned by the socket system call and the accept system call.
        //The accept() system call causes the process to block until a client connects to the server.
        // It returns a new file descriptor, 
        // newsockfd ist der neue file descriptor
        if ( ( newsockfd = accept( sockfd, (struct sockaddr *) &cli_addr, (socklen_t*) &clilen) ) < 0 )
            error( const_cast<char *>("ERROR on accept") );
        printf( "opened new communication with client\n" );

        while ( 1 ) {
             //---- wait for a number from client ---
             data = getData( newsockfd );
             printf( "got %d\n", data );
             cout << "TCP incoming message: " << data << endl;
             if ( data < 0 ) 
                break;
                
             // Orginal Code   
             //data = func( data );
             //data = 999;
             // char msg[] = "Send back from TCP Server \n ";
             //sendData( newsockfd,datamsg );
             // send(newsockfd, msg, strlen(msg), 0); 


             //--- send new data back --- 
             printf( "sending back %d\n", data );
             
            std::string dataToSend = "TCP Answer: " + std::to_string(data);
            uint32_t dataLength = htonl(dataToSend.size()); // Ensure network byte order // when sending the data length
            send(newsockfd, dataToSend.c_str(), dataToSend.size(), 0); // Send the string  data 
             
          }
        close( newsockfd );

        //--- if -2 sent by client, we can quit ---
        if ( data == -2 )
          break;
     }
     return 0; 
}
