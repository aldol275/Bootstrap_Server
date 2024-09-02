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

using namespace std;
const int backLog = 6;
#define SIZE 4096000
const int maxDataSize = 1460;
map<string,vector<string>> fileservers;

//client requesting server information from bootp
void to_bootpserver(){

   uint16_t serverPort=1212;
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
   //while(1)
   //{
      cout<<"Enter message to send to server in  or simply type 'bye' to end"<<endl;
      memset(&rcvDataBuf, 0, maxDataSize);
      memset(&sendDataBuf, 0, maxDataSize);
      
      cin.clear();
      
      getline(cin, sendDataStr);
     
      dataSent = sendto(clientSocketFd, sendDataStr.c_str(), sendDataStr.length(), flags,(struct sockaddr *)&serverSockAddressInfo,sizeof(serverSockAddressInfo));

      dataSent == -1 ? cout<<"Error in sending data : "<<endl: cout<<"message sent successfully "<<endl; 
    
      if(!strcmp(sendDataStr.c_str(), "bye"))
      {
         cout<<"All done closing socket now"<<endl;
         close(clientSocketFd);
      }
      int len;
      memset(&rcvDataBuf, 0, maxDataSize);
     // cout<<"before while"<<endl;
      while(1){  //cout<<"after while"<<endl;
         memset(&rcvDataBuf, 0, maxDataSize);
         dataRecvd = recvfrom(clientSocketFd, &rcvDataBuf, maxDataSize, flags,(struct sockaddr *)&serverSockAddressInfo,&sinSize); //cout<<"after rcv"<<endl;
         rcvDataStr = rcvDataBuf;
         cout<<"Message from server: "<<rcvDataStr.c_str()<<endl;

         char *ptr,*ip,*msgtype;   
        if(!strcmp(rcvDataStr.c_str(), "end"))
         {
            cout<<"All done closing socket now"<<endl;
            close(clientSocketFd);
            break;
         }
         //IP address, port number, servicename, and a service access token 
         
         ip = strtok(rcvDataBuf, ",");  cout<<"ip - "<<ip<<endl;
         ptr = strtok(NULL, ","); //cout<<"port -"<<ptr<<endl;
         int i=0;
         while (ptr != NULL)
         {    
            
            if(fileservers.find(ip) == fileservers.end()){
               fileservers.insert({ip,{ptr}});
            }
            else{
               fileservers[ip].push_back(ptr);
            }
            cout <<i++<<"   "<< ptr  << endl;
            ptr = strtok (NULL, ","); 
            //cout<<"ptr - "<<ptr<<endl;
         }
         cout<<"received server infos"<<endl;
         // memset(&rcvDataBuf, 0, maxDataSize);
         // dataRecvd = recvfrom(clientSocketFd, &rcvDataBuf, maxDataSize, flags,(struct sockaddr *)&serverSockAddressInfo,&sinSize);
         // rcvDataStr = rcvDataBuf;
         

     }
  // }
      cout<<"All done closing udp bootpsocket now"<<endl;
       //  close(clientSocketFd);

}

//client receiving file from server
void receive_file_from_server(int socket,char *filename)
{ 
    printf("Receiving\n");
    int buffersize = 0, recv_size = 0,size = 0, read_size, write_size,rcvd,buffer_fd;
    fd_set fds;
  
    //array to receive file
    char file_array[40960];
    FILE *file_rcvd;

    //size of file received
     do{
    rcvd = read(socket, &size, sizeof(int));
    }while(rcvd<0);

   //  if(size==0){
   //    cout<<"file not found"<<endl;
   //    return;
   //  }

    cout<<"file size is - "<<size<<endl;
    char buffer[]="Got";
   do{
      rcvd = write(socket, &buffer, sizeof(int));
    }while(rcvd<0);

    file_rcvd = fopen(filename,"wb");

    if( file_rcvd == NULL) {
      cout<<"Error in file creation"<<endl;
      exit(0);
    }

    struct timeval timeout = {10,0};

    while(recv_size < size) {

        FD_ZERO(&fds);
        FD_SET(socket,&fds);

        buffer_fd = select(FD_SETSIZE,&fds,NULL,NULL,&timeout);

        if(buffer_fd>0)
        {
            //read_size = recv(socket, &file_array, 10241, 0);
            do{
                   read_size = read(socket,file_array, 10241);
                }while(read_size <0);
            //cout<<"recvd"<<endl;
            //Writing the data read parallely in our output file_rcvd
            write_size = fwrite(file_array,1,read_size, file_rcvd);
            
            //Increment the total number of bytes read
            recv_size += read_size;
        }
        else{
           cout<<"error"<<endl;
        }
    }
   // file_rcvd = NULL;
    //fclose(file_rcvd);
    printf("File successfully Received!\n");
}

main()
{
   to_bootpserver();
   int flags=0,dataSent=0;
   int sockt;
   char rcvDataBuf[SIZE], sendDataBuf[SIZE];
   int sockfd,serverPort,dataRecvd;
   struct sockaddr_in server_addr;
   string sendDataStr,rcvDataStr,filen,change_at;
   char filename[SIZE],*serverIpAddr;

   for(auto x: fileservers){
      serverIpAddr = "127.0.0.1";
      vector<string> serverD(x.second);
      serverPort = stoi(serverD[0]);

      // cout<<"IP and Port"<<endl;
      // cin>>serverIpAddr;
      // cin>>serverPort;

      cout<<"connecting to server "<<x.first<<" at port "<<serverPort<<endl; //x.first
      string access_token = serverD[2],serverType= serverD[1];
      cout<<"ACCESS TOKEN -"<<access_token<<" server type "<<serverType<<endl;
      //create socket
      sockfd = socket(AF_INET, SOCK_STREAM, 0);
      server_addr.sin_family = AF_INET;
      server_addr.sin_port = serverPort;
      server_addr.sin_addr.s_addr = inet_addr(serverIpAddr);

      if(sockfd < 0) {
         cout<<"Error in socket"<<endl;
         exit(1);
      }

      cout<<"Server socket created successfully."<<endl;
      
      sockt = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));

      if(sockt == -1) {
         cout<<"Error in socket"<<endl;
         exit(1);
      }
      cout<<"Connected to Server."<<endl;    
     // cin.ignore();
      // while(1)
      // {
         
         //cin.clear();
         
         //cin.ignore();
         cout<<"want to change the access token"<<endl;
         cin>>change_at;
         if(change_at=="yes"){
            cout<<"Enter new access token"<<endl;
            cin>>access_token;
         }

         sendDataStr = "";
         sendDataStr = access_token;

         cout<<"sending access token to server - "<<sendDataStr.c_str()<<endl;
      
       
         dataSent = send(sockfd, sendDataStr.c_str(), sendDataStr.length(), flags);
      
        // cout<<"Data sent successfully"<<endl;

         memset(&rcvDataBuf, 0, maxDataSize);
         dataRecvd = recv(sockfd, &rcvDataBuf, maxDataSize, flags);
         rcvDataStr = rcvDataBuf;
         cout<<rcvDataBuf<<endl;
         if(rcvDataStr == "Invalid"){
            cout<<"Invalid client"<<endl;
            close(sockfd);
         }
         else if(rcvDataStr == "valid"){
            cout<<"Server can send file of type "<<serverType<<endl;
            cout<<"Enter the filename"<<endl;
            cin>>filen; cout<<"filen "<<filen<<endl;
            strcpy(filename, filen.c_str());  cout<<"filename "<<filen<<endl;
            cout<<"requesting file "<<filen<<"   "<<filename<<endl;
            //sending the file name that client wants
            dataSent = send(sockfd, filen.c_str(), filen.length(), flags);
            
            read(sockfd,rcvDataBuf,maxDataSize);
            cout<<rcvDataBuf<<endl; 
            if(!strcmp(rcvDataBuf, "not found")){
               cout<<"File does not exists"<<endl;
               //break;
            }
            else{
               filen = "client."+serverType;
               cout<<"creating "<<filen<<endl;;
               strcpy(filename, filen.c_str()); 
               receive_file_from_server(sockfd,filename);
            }
         }
         
         if(!strcmp(sendDataStr.c_str(), "bye"))
         {
            close(sockfd);
            break;
         }
   }

   cout<<"All done closing socket now"<<endl;
   

}


