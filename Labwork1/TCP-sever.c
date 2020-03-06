#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 4444

int main(){

	int sockfd;
	struct sockaddr_in serverAddr;

	int newSocket;
	struct sockaddr_in newAddr;

	socklen_t addr_size;
	char buffer[1024];
     // socket create and varification 
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	printf("[+]Server Socket Created Sucessfully.\n");
	memset(&serverAddr, '\0', sizeof(serverAddr));
        // assign IP, PORT
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
       // Binding newly created socket to given IP and verification
	bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	printf("[+]Bind to Port number %d.\n", 4444);
      // Server is ready to listen and verificatio
	listen(sockfd, 5);
	printf("[+]Listening...\n");

	newSocket = accept(sockfd, (struct sockaddr*)&newAddr, &addr_size);
       // Send the messeger to client and receiver messeger from client
    
	strcpy(buffer, "hello");
	send(newSocket, buffer, strlen(buffer), 0);
    recv(newSocket, buffer, 1024, 0);
    printf("[+]Data Recv from client: %s\n", buffer);
	printf("[+]Closing the connection.\n");
	return 0;
}
