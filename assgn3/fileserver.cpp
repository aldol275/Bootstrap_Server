#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include<bits/stdc++.h>

#define SIZE 4096000
using namespace std;
const int backLog = 6;
const int maxDataSize = 1500;
int max_clients = 4;
string access_token="";
//send file to client
void send_file_to_clients(int socket,char *filename){

    FILE *file_send;
    int size=0, read_size=0, rcvd=0;

    //send_buffer sizeis more as we have more to send then receive
    char send_buffer[40960], read_buffer[256];

    //calculating the total size of file to be sent
    file_send = fopen(filename, "rb");

    if(file_send==NULL){
      cout<<"File not found"<<endl;
      char sends[10] = "not found";
      send(socket, sends, strlen(sends),0);
      return;
    }
    else if(file_send){
      cout<<"Requested File is found"<<endl;
      char sends[10] = "found";
      send(socket, sends, strlen(sends),0);
    }

    fseek(file_send, 0, SEEK_END);
    size = ftell(file_send);
    fseek(file_send, 0, SEEK_SET);

    //sending the file size to client
    printf("Total file size: %i\n",size);
    write(socket, (void *)&size, sizeof(int));

    do { //Read while we get errors that are due to signals.
      rcvd=read(socket, &read_buffer , 255);
      printf("Bytes read: %i\n",rcvd);
   } while (rcvd < 0);

    //looping to read and send parallely
    while(!feof(file_send)) {
      
      //read from file and send to client
      read_size = fread(send_buffer, 1, sizeof(send_buffer)-1, file_send);
      do{
        rcvd = write(socket, send_buffer, read_size);  
      }while (rcvd < 0);

      //rcvd = send(socket, send_buffer, strlen(send_buffer),0);
      //cout<<"sent"<<endl;
      //clear the sending buffer to refresh the buffer
      bzero(send_buffer, sizeof(send_buffer));

     }
     //file_send = NULL;
     //fclose(file_send);
}
//connect to bootp server to register
void as_client(){
   uint16_t serverPort=3002;
   string serverIpAddr = "127.0.0.1";
  cout<<"Enter the ip address and port number of bootpserver"<<endl;
  //cin>>serverIpAddr;
  cin>>serverPort;

   int clientSocketFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
   if(!clientSocketFd)
   {
      cout<<"Error creating socket"<<endl;
      exit(1);
   }

   struct sockaddr_in serverSockAddressInfo;
   serverSockAddressInfo.sin_family = AF_INET;
   serverSockAddressInfo.sin_port = htons(serverPort);
 
   inet_pton(AF_INET, serverIpAddr.c_str(), &(serverSockAddressInfo.sin_addr));

   memset(&(serverSockAddressInfo.sin_zero), '\0', 8);

   socklen_t sinSize = sizeof(struct sockaddr_in);
   int flags = 0;
   int dataRecvd = 0, dataSent = 0;
   struct sockaddr_in clientAddressInfo;
   char rcvDataBuf[maxDataSize], sendDataBuf[maxDataSize];
   string sendDataStr, rcvDataStr;

   cin.ignore();
 //  while(1)
   //{
      cout<<"Enter details: MSGTYPE(REGISTRN,DISCOVERY), IP address, port number, servicename, service access token or simply type 'bye' to end"<<endl;
      memset(&rcvDataBuf, 0, maxDataSize);
      memset(&sendDataBuf, 0, maxDataSize);
      
      cin.clear();
      
      getline(cin, sendDataStr);
      char *ptr,*msgtype;   
      cout<<"sent to bootp "<<sendDataStr<<endl;

      //IP address, port number, servicename, and a service access token 
      //to get the access token
      strcpy(rcvDataBuf, sendDataStr.c_str());
      ptr = strtok(rcvDataBuf, ",");
      int i=0;
      while (ptr != NULL)  
      {  
         if(i==4){
            access_token = ptr; cout<<access_token<<endl;}
         ptr = strtok (NULL, ",");  i++;
      } 
      dataSent = sendto(clientSocketFd, sendDataStr.c_str(), sendDataStr.length(), flags,(struct sockaddr *)&serverSockAddressInfo,sizeof(serverSockAddressInfo));

      dataSent == -1 ? cout<<"Error in sending data : "<<endl: cout<<"message sent successfully "<<endl; 
    
     
      int len;
      memset(&rcvDataBuf, 0, maxDataSize);
     
   cout<<"AT is -"<<access_token<<endl;
   cout<<"All done closing socket now"<<endl;
   close(clientSocketFd);
   //return access_token;
   
}
/// listen to client requests
void as_server(){ cout<<"Server "<<access_token<<endl;
  char *ip = "127.0.0.1";
  int port = 8081;
  int sockt,flags=0,dataRecvd=0;
  char rcvDataBuf[SIZE];
  int sockfd, new_sock[30],sd,max_sd,activity,new_socket,valread;
  struct sockaddr_in server_addr, new_addr;
  socklen_t addr_size;
  char buffer[SIZE];
  string sendDatastr;
  char *filename = "../Supriya.pdf";
  cout<<"Enter the port no"<<endl;
  cin>>port;
  for (int i = 0; i < max_clients; i++)  
    {  
        new_sock[i] = 0;  
    }  
    
  //set of socket descriptors 
  fd_set readfds; 
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0) {
    cout<<"Error in socket"<<endl;
    exit(1);
  }
  printf("Server socket created successfully.\n");
 
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = port;
  server_addr.sin_addr.s_addr = inet_addr(ip);
 
  sockt = bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
  if(sockt < 0) {
    cout<<"Error in bind"<<endl;
    exit(1);
  }
  
  if(listen(sockfd, 10) == 0){
    cout<<"Listening....."<<endl;
    }
  else{
    cout<<"Error in listening"<<endl;
    exit(1);
  }

  if (listen(sockfd, 3) < 0)  
    {  
        cout<<"listen"<<endl;  
        exit(EXIT_FAILURE);  
    }  

  addr_size = sizeof(new_addr);int j=0;
  while(1){ 
    FD_ZERO(&readfds); 
    FD_SET(sockfd, &readfds);  
    max_sd = sockfd;
    
    for (int i = 0 ; i<max_clients; i++)  
        {  
            //socket descriptor 
            sd = new_sock[i];  
                 
            //if valid socket descriptor then add to read list 
            if(sd > 0)  
                FD_SET( sd , &readfds);  
                 
            //highest file descriptor number, need it for the select function 
            if(sd > max_sd)  
                max_sd = sd;  
        }  
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);  
       
        if ((activity < 0) && (errno!=EINTR))  
        {  
            cout<<"error"<<endl;  
        } 

     if (FD_ISSET(sockfd, &readfds))  
        {  
            if ((new_socket = accept(sockfd, 
                    (struct sockaddr *)&server_addr, (socklen_t*)&addr_size))<0)  
            {  
                cout<<"accept"<<endl;  
                exit(0);  
            }  
             
            //new client connected
            cout<<"new client connected "<<ip<<" "<<"port "<<port<<endl;//socket fd is %d , ip is : %s , port : %d\n" , new_socket , inet_ntoa(server_addr.sin_addr) , ntohs
                 // (server_addr.sin_port));  
           
            memset(&rcvDataBuf, 0,SIZE);

            //get access_token from client
            dataRecvd = recv(new_socket, &rcvDataBuf, SIZE, flags);
            cout<<"Client sent this access token-"<<rcvDataBuf<<" Server AT-"<<access_token<<endl;
            cin.ignore();

            //compare the access token with that os server's instance
            if(access_token == rcvDataBuf){

               if(!strcmp(rcvDataBuf, "bye"))
               {
                  cout<<"client left"<<endl;
                  close(new_socket);
                  break;

               }
               sendDatastr = "valid";
               send(new_socket, sendDatastr.c_str(), sendDatastr.length(), flags);
               //get the filename to be sent frosendDataStrm client
               memset(&rcvDataBuf, 0,SIZE);
               dataRecvd = recv(new_socket, &rcvDataBuf, maxDataSize, flags);
               filename = rcvDataBuf;
               cout<<"client is requesting file-"<<filename<<endl;
               //send file to client
               send_file_to_clients(new_socket,filename);
               
            }
            //access token did not match
            else{
               sendDatastr = "Invalid";
               send(new_socket, sendDatastr.c_str(), sendDatastr.length(), flags);
               cout<<"Invalid client"<<endl;
               close(new_socket);
            // break;
            }
            // cout<<"Press 1 to keep listening, else 0"<<endl;
            // cin>>choice;
            //printf("Data written in the file successfully.\n");
    
            //add new socket to array of sockets 
            for (int i = 0; i<max_clients; i++)  
            {  
                if( new_sock[i] == 0 )  
                {  
                    new_sock[i] = new_socket;  
                    printf("add to existing sockets list %d\n" , i);  
                         
                    break;  
                }  
            }  
        }  
             
        //existing client is asking for other file
        for (int i = 0;i<max_clients; i++)  
        {  
            sd = new_sock[i];  
                 
            if (FD_ISSET( sd , &readfds))  
            {  
                //check if client disconnected
                if ((valread = read( sd , buffer, 1024)) == 0)  
                {  
                     
                    getpeername(sd , (struct sockaddr*)&server_addr , \
                        (socklen_t*)&addr_size);  
                    printf("Host disconnected , ip %s , port %d \n" , 
                          inet_ntoa(server_addr.sin_addr) , ntohs(server_addr.sin_port));  
                         
                    close( sd );  
                    new_sock[i] = 0;  
                }  
                 //write in the existing client connection     
                else 
                {  
                   
                    int val = int(*buffer);
                    cout<<"val "<<buffer<<endl;
                     cin.ignore();

                     memset(&rcvDataBuf, 0, maxDataSize);

                     dataRecvd = recv(new_socket, &rcvDataBuf, maxDataSize, flags);
                     cout<<rcvDataBuf<<endl;
                     if(access_token == rcvDataBuf){

                        if(!strcmp(rcvDataBuf, "bye"))
                        {
                           cout<<"client left"<<endl;
                           close(new_socket);
                           break;
                        }

                        memset(&rcvDataBuf, 0, maxDataSize);
                        dataRecvd = recv(new_socket, &rcvDataBuf, maxDataSize, flags);
                        filename = rcvDataBuf;
                        send_file_to_clients(new_socket,filename);
                     }
                     else{
                        cout<<"Invalid client"<<endl;
                        close(new_socket);
                     // break;
                     }
                     // cout<<"Press 1 to keep listening, else 0"<<endl;
                     // cin>>choice;
                                    
               }  
            }  
        }  
         
  }

}
/// @brief

int main()
{
   
   as_client();
   
   thread t2(as_server);
   //t1.join();
   t2.join();
   
   return 0;
}
