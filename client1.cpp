#include <iostream>
#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <netdb.h>

#define PORT 5001
#define MAXLINE 1024

using namespace std;

int main(int argc, char *argv[]){
    int sockfd;
    char buffer[MAXLINE];
    const char* message = "Hello Server";
    sockaddr_in servaddr;
    

	
    int n, len;
    hostent *server;

    // Create socket file descriptor
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        cout << "Socket creation failed";
        exit(0);
    }

	
    server = gethostbyname(argv[1]);
    if(server == NULL){
        fprintf(stderr, "Error, no such host");
    }

    memset(&servaddr, 0, sizeof(servaddr));
    

	
    // Filling server info
    bzero((char*) &servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    bcopy((char*)server->h_addr, (char*) &servaddr.sin_addr.s_addr, server->h_length);
    servaddr.sin_port = htons(PORT);

    if(connect(sockfd, (sockaddr*) &servaddr, sizeof(servaddr)) < 0)
        cout << "Connection Failed";

    memset(buffer, 0, sizeof(buffer));
    strcpy(buffer, "Hello Server");
    write(sockfd, buffer, sizeof(buffer));
    cout<< "Message from Server: ";
    read(sockfd, buffer, sizeof(buffer));
    cout << buffer << endl;
    close(sockfd);

		
    return 0;
}