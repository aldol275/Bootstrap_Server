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
const int backLog = 3;
const int maxDataSize = 1500;
map<string,vector<string>> fileservers;

int main()
{
   //server IP and port 
   uint16_t serverPort=3002;
   string serverIpAddr = "127.0.0.1";
   cout<<"Enter the port number to listen the connections for"<<endl;
   //cin>>serverIpAddr;
   cin>>serverPort;

   //for UDP connection
   int serverSocketFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
   if(!serverSocketFd)
   {
      cout<<"Error creating socket"<<endl;
      exit(1);
   }

   //set the socket values
   struct sockaddr_in serverSockAddressInfo;
   serverSockAddressInfo.sin_family = AF_INET;
   serverSockAddressInfo.sin_port = htons(serverPort);
   
   inet_pton(AF_INET, serverIpAddr.c_str(), &(serverSockAddressInfo.sin_addr));
   memset(&(serverSockAddressInfo.sin_zero), '\0', 8);
   
   cout<<"Server listening......"<<endl;
   //binding socket
   int ret = bind(serverSocketFd, (struct sockaddr *)&serverSockAddressInfo, sizeof(struct sockaddr)); 
   if(ret<0)
   {
      cout<<"Error binding socket"<<endl;
      close(serverSocketFd);
      exit(1);
   }

   socklen_t sinSize = sizeof(struct sockaddr_in);
   int flags = 0;
   int dataRecvd = 0, dataSent = 0;
   struct sockaddr_in clientAddressInfo;
   char rcvDataBuf[maxDataSize], sendDataBuf[maxDataSize];
   string sendDataStr, rcvDataStr;

   memset(&clientAddressInfo, 0, sizeof(struct sockaddr_in));
   memset(&rcvDataBuf, 0, maxDataSize);
   cin.ignore();
   //continuous loop so that client and server can register parallely
   while(1)
   {
      memset(&rcvDataBuf, 0, maxDataSize);
      memset(&sendDataBuf, 0, maxDataSize);
      rcvDataStr = "";
      dataRecvd = recvfrom(serverSocketFd, &rcvDataBuf, maxDataSize, flags,(struct sockaddr *)&clientAddressInfo,&sinSize);
      cout<<"Message from client : "<<rcvDataBuf<<endl;

       rcvDataStr = rcvDataBuf; 
        
      if(!strcmp(rcvDataStr.c_str(), "bye"))
      {
         break;
      }
      
      char *ptr,*ip,*msgtype;   
      
      // servicename, IP address, port number, and a service access token 
      msgtype = strtok(rcvDataBuf, ",");
      string s(msgtype);

      //server payload starts with REGISTRN and client's starts with DISCOVERY
      if(s == "REGISTRN"){
         ip =strtok (NULL, ",");   
         ptr =strtok (NULL, ","); 
         int i=0;

         //extracting server information from comma seperated string
         while (ptr != NULL)  
         { 
            
            if(fileservers.find(ip) == fileservers.end()){
               fileservers.insert({ip,{ptr}});
            }
            else{
               fileservers[ip].push_back(ptr);
            }
              
            ptr = strtok (NULL, ",");  
         }
      }

      else if(s =="DISCOVERY"){ //cout<<"in client section"<<endl;
         string temp="";
         for(auto x: fileservers){
            temp = x.first;
            for(auto i:fileservers[x.first]){
      	      temp += ","+i; 
            }
   
            dataSent = sendto(serverSocketFd, temp.c_str(), temp.length(), flags,(struct sockaddr *)&clientAddressInfo,sizeof(clientAddressInfo));
            //cout<<temp<<endl;
            temp="";
         }
         temp="end";
         dataSent = sendto(serverSocketFd, temp.c_str(), temp.length(), flags,(struct sockaddr *)&clientAddressInfo,sizeof(clientAddressInfo));
         cout<<"ended sending server info"<<endl;
      }
    //  cout<<"out"<<endl; 
      memset(&sendDataBuf, 0, maxDataSize);
      cin.clear();
      int send=0;
   
       
      // dataSent = sendto(serverSocketFd, sendDataBuf, strlen(sendDataBuf), flags,(struct sockaddr *)&clientAddressInfo,sizeof(clientAddressInfo));
      // dataSent == -1 ? cout<<"Error in sending data : "<<endl: 
      // cout<<"message sent successfully - "<<sendDataBuf<<endl; 
   }

   cout<<"All done closing server socket now"<<endl;
   close(serverSocketFd);
}
