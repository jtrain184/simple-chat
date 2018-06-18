# simple-chat
A simple server/client chat program.  The client in implemented in C and the server is in Python.  Illustrates socket programming using TCP.

Files:
    chatserve.py - python script
    chatserve - python 'executable'
    chatclient.c - C code for client 
    Makefile - make file to compile client


Instructions:
1. Unzip 
2. Type make to compile client
3. Start server by running ./chatserve <port#>
4. In another terminal window, start client by running ./chatclient <server-hostname> <port#>
5. Follow prompts on screen, type \quit to close connection from either
terminal. 
6. To stop chatserve, press CTRL+C
7. To clean up client files, type make clean
