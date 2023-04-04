#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 5001
#define MAXLINE 1024
using namespace std;

int main(){
    int listenfd, connfd[10], udpfd, nready, maxfdp1, sd, valread;
    char buffer[MAXLINE];
    pid_t childpid;
    fd_set rset;
    ssize_t n;
    socklen_t len;
    const int on = 1;
    sockaddr_in cliaddr, servaddr;
    const char* message = "Hello Client", *end = "BYE";
    void sig_child(int);
    
    for(int i=0;i<10;i++)
      connfd[i] = 0;

    // Creating TCP socket 
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    // Binding server addr struct to listenfd
    bind(listenfd, (sockaddr*)&servaddr, sizeof(servaddr));
    listen(listenfd, 3);

    // Creating UDP socket
    udpfd = socket(AF_INET, SOCK_DGRAM, 0);

    bind(udpfd, (sockaddr*)&servaddr, sizeof(servaddr));


    char str[MAXLINE];
    
    while(1){
        // Reset descriptor set to zero
        FD_ZERO(&rset);
        
        // maxfd for select
        //maxfdp1 = max(listenfd, udpfd) + 1;

        // set fds in readset;
        FD_SET(listenfd, &rset);
        FD_SET(udpfd, &rset);
        maxfdp1 = listenfd;  
        
        for(int i=0;i<10;i++){
          sd = connfd[i];
          if(sd>0)
            FD_SET(sd, &rset);
          
          if(maxfdp1 < sd)
            maxfdp1 = sd;
        }
        

        // Select the ready descriptor
        nready = select(maxfdp1 + 1, &rset, NULL, NULL, NULL);
        // select(maxfd, readser, writeset, exceptionset, timeout);

        // If TCP socket is readable, accept connection
        if(FD_ISSET(listenfd, &rset)){
            len = sizeof(cliaddr);
            int conn = accept(listenfd, (sockaddr*)&cliaddr, &len);
            //close(listenfd);
            
            for(int i=0;i<10;i++){
              if(connfd[i] == 0){
                connfd[i] = conn;
                break;
              }
            }


            /*while(1){
              
              bzero(buffer, sizeof(buffer));
              cout << "Message from TCP Client: ";
              read(connfd, buffer, sizeof(buffer));
              cout << buffer << endl;
              if(buffer==end){
                  close(connfd);
                  break;
              }
              cout<< "Enter message: ";
              cin >> str;
              write(connfd, (const char*)str, sizeof(buffer));
            }*/
        }
        
        for(int i=0;i<10;i++){
          sd = connfd[i];
          
          if(FD_ISSET(sd, &rset)){
              bzero(buffer, sizeof(buffer));
              cout << "Message from TCP Client: ";
              read(sd, buffer, sizeof(buffer));
              cout << buffer << endl;
              if(buffer==end){
                  close(sd);
                  break;
              }
              cout<< "Enter message: ";
              cin >> str;
              write(sd, (const char*)str, sizeof(buffer));
          }
        }

        // If UDP socket is readable, accept the connection
        if(FD_ISSET(udpfd, &rset)){
            len = sizeof(cliaddr);
            bzero(buffer, sizeof(buffer));
            cout << "Message from UDP Client: ";
            n = recvfrom(udpfd, buffer, sizeof(buffer), 0, (sockaddr*)&cliaddr, &len);
            cout << buffer << endl;
            sendto(udpfd, (const char *)message, sizeof(buffer), 0, (sockaddr*)&cliaddr, sizeof(cliaddr));

        }

    }

}