CS39006: Networks Laboratory
Assignment 7: Lightweight Custom Discovery Protocol (CLDP)
Name: Praveen Kumar
Roll Number: 22CS10054


1. Overview
This project implements the Lightweight Custom Discovery Protocol (CLDP) using raw sockets in POSIX C. It includes a client (cldp_client.c) and a server (cldp_server.c) that communicate over a custom IP protocol (number 253).


2. Build Instructions
- Prerequisites: GCC, Linux OS with raw socket support (e.g., Ubuntu).
- Steps:
  1. Ensure all files (cldp_server.c, cldp_client.c, Makefile) are in the same directory.
  2. Run: `make`
  - This compiles cldp_server.c into ./cldp_server and cldp_client.c into ./cldp_client.


3. Run Instructions
- Run the server: `sudo ./cldp_server`
  - Requires sudo due to raw socket privileges.
  - Output: "CLDP Server started, Listening for packets..." followed by received packet logs.
- Run the client: `sudo ./cldp_client`
  - Output: "CLDP Client starting..." followed by periodic HELLO and QUERY messages.
- Both programs must run on a Linux system with appropriate network access.


4. Assumptions
- IP address 172.30.233.130 is hardcoded as the source IP (replace with your machine’s IP if different).
- Broadcast address 255.255.255.255 is used for HELLO and QUERY messages.
- Tested on a single machine; multi-node testing requires network configuration.


5. Limitations
- No checksum calculation for IP header (not required by assignment but could be added).
- Only hostname and timestamp are supported in RESPONSE (third metadata could be added).
- Client does not display RESPONSE packets (focus is on sending HELLO/QUERY as per demo).



6. Demo Output
Client:
CLDP Client starting...
Sending HELLO: HELLO from perfecta
Sending QUERY: QUERY: hostname, timestamp
Waiting 10 seconds before next cycle...
[...repeats every 10 seconds...]

Server:
CLDP Server started, Listening for packets...
Received HELLO from 172.30.233.130: HELLO from perfecta
Received QUERY from 172.30.233.130: QUERY: hostname, timestamp
Sent RESPONSE to 172.30.233.130
[...repeats for each received packet...]
