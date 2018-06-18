#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

/* chatclient - Simple chat client by Philip Jarrett for CS372. This code is a modified version of what was used in
CS 344 Operating Systems and Beej's Guide to Network Programming. */

#define MAX_CHAR 500

void error(const char *msg) { perror(msg); exit(0); } // Error function used for reporting issues


/***************************** Helper functions *****************************/


// Function to get user handle
void getUserName(char* buffer, int bufferSize) {
	printf("Please enter username (max 10 characters): ");
    memset(buffer, '\0', bufferSize); // Clear out the buffer array
    fgets(buffer, bufferSize - 1, stdin); // Get input from the user, trunc to buffer - 1 chars, leaving \0

  // discard any extra characters:
  if (!strchr(buffer, '\n'))
    while (fgetc(stdin) != '\n');

  buffer[strcspn(buffer, "\n")] = '\0'; // Remove the trailing \n that fgets adds
}

// Function to check for exit command in user input
int checkExit(char* command, char* string) {
  if (strstr(string, command) != NULL)
    return 1; // command was found
  else
    return 0; // command was NOT found
}

// Send message to server
void sendMessage(int socket, char* buffer){
	int charsWritten = send(socket, buffer, strlen(buffer), 0); // Write to the server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < strlen(buffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");
}


/***************************** Main function *****************************/
/********* Controls socket connections and main program loop. ************/


int main(int argc, char *argv[])
{
	int socketFD, portNumber, charsRead;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char buffer[MAX_CHAR];

	char userName[11];  // 10 + 1 for the newline
	char exitCommand[] = "\\quit";
	int exitStatus = 0;

    
	if (argc < 3) { fprintf(stderr,"USAGE: %s hostname port\n", argv[0]); exit(0); } // Check usage & args

	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[2]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverHostInfo = gethostbyname(argv[1]); // Convert the machine name into a special form of address
	if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) error("CLIENT: ERROR opening socket");
	
	// Connect to server
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
		error("CLIENT: ERROR connecting");

	// Ask for user handle
	getUserName(userName, sizeof(userName));
	printf("Hello %s!\n", userName);

	// Get input message from user
	while(1){
		printf("%s, Enter text to send to the server, and then hit enter: ", userName);

		// Get user message and append username
		memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer array
		memcpy(buffer, userName, sizeof(userName)); // Add username to buffer

		// Find end of user name
		char* end = strchr(buffer, '\0');
		int i = (int)(end - buffer); // Index for appending > symbol

		// Add > symbol and a space after username.
		buffer[i] = '>';
		buffer[++i] = ' '; 

		// User input buffer
		char inputBuffer[512];    // Accounts for username and > symbol
		memset(inputBuffer, '\0', 512);   // Clear out the buffer array
		fgets(inputBuffer, MAX_CHAR + 1, stdin);  

		 // Remove any extra chracters over max allowed from stdin
  		if (!strchr(inputBuffer, '\n'))     // new line does not exist
			while (fgetc(stdin) != '\n');   // discard until newline

		inputBuffer[strcspn(inputBuffer, "\n")] = '\0';   // Remove the trailing \n that fgets adds

		// Copy user input to buffer
		memcpy(&buffer[++i], inputBuffer, sizeof(inputBuffer));

		exitStatus = checkExit(exitCommand, buffer);

		// If user entered exit command
		if(exitStatus == 1){
			printf("Closing connection to chatserve and exiting...\n");
			sendMessage(socketFD, buffer); // Tell the server you quit
			close(socketFD); // Close socket
			exit(0); // Shutdown program
		}

		// No exit command, send message and continue chatting
		else{
			sendMessage(socketFD, buffer);

			// Get return message from server
			printf("Waiting for server response...\n");
			memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
			charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
			if (charsRead < 0) error("CLIENT: ERROR reading from socket");
			printf("%s\n", buffer);
			
			// Check for server exit command.
      		if (checkExit(exitCommand, buffer)) {
        		printf("Chatserve closed connection, exiting...\n\n");
        		close(socketFD); // Close the socket
        		exit(0);

				// Display server's message
				printf("%s\n", buffer);
			}
		} // end else
	} // end while

	return 0;
}
