#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 4444
int main(){
	
	int clientSocket;
	struct sockaddr_in serverAddr;
	char buffer[1024];
      // socket create and verification
	clientSocket = socket(PF_INET, SOCK_STREAM, 0);
	printf("[+]Client Socket Created Sucessfully.\n");
 
	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    // Server is ready to listen and verification 
	connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	printf("[+]Connected to Server.\n");
    // Receiver from the sever receiver messeger from sever
	recv(clientSocket, buffer, 1024, 0);
	printf("[+]Data Recv from sever: %s\n", buffer);
    strcpy(buffer, "hillo");
    send(clientSocket, buffer, strlen(buffer), 0);
	printf("[+]Closing the connection.\n");
	return 0;
}
