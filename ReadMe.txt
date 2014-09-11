README

TO COMPILE:

g++ threadTTL.cpp -w -pthread

TO RUN:

./a.out

Program will prompt you for host (i.e bob.cs-georgetown.net). The program MUST be in the same director of the file taleoftwocities.txt (named exactly like that). The program will send a packet for each character from the message in the TTL and a line from Dickens in the payload.

Program only tested on Alice  and Bob, with no known compile errors.

Sources Used:

Significant portions of NETWORKING code taken from 

http://www.cs.ucsb.edu/~almeroth/classes/W01.176B/hw2/examples/udp-client.c
http://www.cs.ucsb.edu/~almeroth/classes/W01.176B/hw2/examples/udp-server.c

Threads adapted from:

http://www.thegeekstuff.com/2012/04/create-threads-in-linux/