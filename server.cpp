#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>

using namespace std;
int main(){
	//Create socket
	int mySocket= socket(AF_INET,SOCK_STREAM,0);
	if(mySocket==-1){
		cerr<<"Socket couldn't be created";
		return -1;
	}
	//Bind socket to IP/port
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(1084);
	inet_pton(AF_INET,"0.0.0.0",&hint.sin_addr);
	
	if(bind(mySocket,(sockaddr*)&hint,sizeof(hint))==-1){
		cerr<<"Cannot bind to IP/port";
		return -2;
	}
	//Mark socket for listening in
	if (listen(mySocket,SOMAXCONN)==-1){
		cerr<<"Cannot listen";
		return -3;
	}
	//Accept a call
	sockaddr_in client;
	socklen_t clientSize=sizeof(client);
	char host[NI_MAXHOST];
	char svc[NI_MAXSERV];
	
	int clientSocket=accept(mySocket,(sockaddr*)&client,&clientSize);
	if (clientSocket == -1){
		cerr<<"Problem with client connecting";
		return -4;
	}
	//Close Listening socket
	close(mySocket);
	memset(host,0,NI_MAXHOST);
	memset(svc,0,NI_MAXSERV);
	
	int result= getnameinfo((sockaddr*)&client,sizeof(client),host,NI_MAXHOST,svc,NI_MAXSERV,0);
	if(result)
	{
		cout<<host<<" connected on "<<svc<<endl;
	}
	else
	{
		inet_ntop(AF_INET,&client.sin_addr,host,NI_MAXHOST);
		cout<<host<<" connected on "<<ntohs(client.sin_port)<<endl;
	}
	//While receiving display message,echo
	char buf[4096];
	while(true)
	{
		//clear the buffer
		memset(buf,0,4096);
		//wait for message
		int bytesRecv = recv(clientSocket,buf,4096,0);
		if(bytesRecv== -1){
			cerr<<"There was a connection issue"<<endl;
			break;
		}
		if(bytesRecv == 0){
			cout<<"The client disconnected"<<endl;
			break;
		}
		//display message
		cout<<"Received : "<<string(buf,0,bytesRecv)<<endl;
		//Resend message
		send(clientSocket,buf,bytesRecv +1,0);
		
	
		
	}
	//Close socket
	close(clientSocket);
	return 0;
}
