/**
 * Micah's client program.  Copies everything from standard input
 * to a specified destination over TCP (port 22382).
 */


#include <sys/types.h> 
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>



#define BUFLEN 1024



///////// THIS PROGRAM SENDS THE SECRET MESSAGES TO THE OTHER PROGRAM 0--- SIT ON BOB

int main( int argc, char *argv[] )
{
  int sock;			/* socket */ //similar to a file handle
  char *dest;
  struct addrinfo hints;
  struct addrinfo *servinfo;	/* will point to the results */
  char buf[BUFLEN];
  int br;			/* bytes read */


  /* check command-line arguments */
  if (argc != 2) {
    fprintf( stderr, "Usage: %s destination\n\n", argv[0] );
    return 1;
  } else {
    dest = argv[1]; //host name "i.e. bob@cs-class.uis etc"
  }

  /* see 
     http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html */
  memset(&hints, 0, sizeof(hints)); // make sure the struct is empty
  hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
  hints.ai_socktype = SOCK_DGRAM; // TCP stream sockets -- set to DGRAM

  /* 
     get ready to connect (22382 is our port number).
     specifically, this performs a DNS lookup (if necessary) and
     copies the result to the servinfo structure (see "connect" below).
  */
  if (getaddrinfo(dest, "20226", &hints, &servinfo) != 0) // check return values
  {
    perror( "getaddrinfo" ); //prints out an OS error returned after the string "getaddrinfo"
    //return 1;
  }

  /* create a socket for sending data */
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) //"socket = open"
  {
    perror("socket");
    return 1;
  }
  
  /* connect to destination */
  if (connect( sock, servinfo->ai_addr, servinfo->ai_addrlen ) == -1) {
    perror( "connect" );
    close(sock);
    return 1;
  }

  /* read from standard input, and send whatever we read to 
     the destination */

	//(br = read(STDIN_FILENO,buf,BUFLEN)) > 0
  while( true ) // read returns the number of bytes that were read from the standard input
  {
	
	char * secret_value;
	*secret_value = 'Z';
	
	// call setsockopt(), set the char from standard input = TTL value for this packet -- send it.
		setsockopt(sock, IPPROTO_IP, IP_TTL, secret_value, sizeof(secret_value));
	    write( sock, buf, br );	/* write br bytes to the destination */ // This wont change, just change options above
	 // man 7 ip or man setsockopt()
  }
  
  printf( "Goodbye!\n" );

  close( sock );

  return 0;			/* all's well that ends well */
}

//TO RUN ON COMMAND LINE:


// ./myclient bob@cs-georgetown.edu
