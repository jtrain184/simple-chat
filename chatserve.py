# chatserve - a simple chat server written in python for CS372
#!/usr/bin/python
#!/usr/bin/env python

import SocketServer
import signal
import sys

def signal_handler(signal, frame):
        print('SIGINT received, shutting down.')
        server.server_close()
	sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)

# Globals
exit_code = '\\quit'
server_handle = 'chatserve'
exit_status = True

# Used Example here to build upon:
# https://docs.python.org/release/2.6.5/library/socketserver.html#socketserver-tcpserver-example
class MyTCPHandler(SocketServer.BaseRequestHandler):
    """
    The RequestHandler class for our server.

    It is instantiated once per connection to the server, and must
    override the handle() method to implement communication to the
    client.
    """

    def handle(self):
        global exit_status
        # self.request is the TCP socket connected to the client
        # exit_status tells us if new client
        if (exit_status):
            print "%s connected on port %s." % (self.client_address[0], PORT)
            exit_status = False
        self.data = self.request.recv(1024).strip()

        while(1):
            if exit_code in self.data:
                print 'Client closed connection.'
                exit_status = True
                self.request.close()
                print "chatserve open on port %s, listening for connections." % PORT
                return
            else:
                # Print the client's message
                print self.data

            # Get a message to send back
            raw = raw_input('Enter message to send: ')
            server_message = server_handle + '> ' + raw[:500]

            # Server gives exit command
            if exit_code in server_message:
                print 'Closing connection to client.'
                exit_status = True
                self.request.send(server_message)
                self.request.close()
                print "chatserve open on port %s, listening for connections." % PORT
                return

            self.request.send(server_message)
            print 'Waiting for client message...'
            self.data = self.request.recv(1024).strip()


if __name__ == "__main__":

    if len(sys.argv) < 2:
        print 'Error: USAGE ./chatserve <port#>'
        sys.exit(1)
    HOST, PORT = "localhost", int(sys.argv[1])

    # Create the server, binding to localhost on port 9999
    server = SocketServer.TCPServer((HOST, PORT), MyTCPHandler)
    print "chatserve open on port %s, listening for connections." % PORT
    # Activate the server; this will keep running until you
    # interrupt the program with Ctrl-C

    server.serve_forever()