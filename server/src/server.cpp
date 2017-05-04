/*
KSync - Client-Server synchronization system using rsync.
Copyright (C) 2014  Matthew Scott Krafczyk

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <unistd.h>
#include <signal.h>
#include <sstream>
#include <utility>

#include "ksync/server.h"
#include "ksync/logging.h"
#include "ksync/messages.h"
#include "ksync/utilities.h"
//#include "ksync/socket_ops.h"
#include "ksync/comm_system_interface.h"
#include "ksync/comm_system_factory.h"
#include "ksync/comm_system_object.h"

#include "ArgParse/ArgParse.h"

bool finished = false;

//Function to stop the loop if signaled
static void Cleanup(int signal) {
	if(signal == SIGTERM) {
		printf("SIGTERM Sent\n");
	} else if(signal == SIGINT) {
		printf("SIGINT Sent\n");
	}
	finished = true;
}

int main(int argc, char** argv) {
	//Setting the signals to trigger the cleanup function
	signal(SIGTERM, Cleanup);
	signal(SIGINT, Cleanup);

	std::vector<std::pair<int,int>> active_sockets;

	std::string connect_socket_url = "";
	bool connect_socket_url_defined = false;

	ArgParse::ArgParser arg_parser("KSync Server - Server side of a Client-Server synchonization system using rsync.");
	arg_parser.AddArgument("connect-socket", "Socket to use to negotiate new client connections. Default is : ipc:///tmp/ksync-<user>/ksync-connect.ipc", &connect_socket_url, ArgParse::Argument::Optional, &connect_socket_url_defined);

	int status;
	if((status = arg_parser.ParseArgs(argc, argv)) < 0) {
		Error("Problem parsing arguments\n");
		arg_parser.PrintHelp();
		return -1;
	}

	if(arg_parser.HelpPrinted()) {
		return 0;
	}

	connect_socket_url = "tcp://*:5555";

	//if(!connect_socket_url_defined){
	//	if(KSync::Utilities::get_default_connection_url(connect_socket_url) < 0) {
	//		Error("There was a problem getting the default connection url!\n");
	//		return -2;
	//	}
	//}

	KPrint("Using the following socket url: %s\n", connect_socket_url.c_str());

	//Start the connection socket
	
	//int connection_socket = 0;
	//int endpoint = 0;
	//
	//if(KSync::SocketOps::Create_And_Bind_Rep_Socket(connection_socket, endpoint, connect_socket_url) < 0) {
	//	Error("There was a problem creating and binding the connection socket!\n");
	//	return -2;
	//}
	//
	//active_sockets.push_back(std::pair<int,int>(connection_socket,endpoint));

	KSync::Comm::CommSystemInterface* comm_system = 0;
	if (KSync::Comm::GetZeromqCommSystem(comm_system) < 0) {
		KPrint("There was a problem initializing the ZeroMQ communication system!\n");
		return -2;
	}

	KSync::Comm::CommSystemSocket* gateway_socket = 0;
	if (comm_system->Create_Gateway_Rep_Socket(gateway_socket) < 0) {
		KPrint("There was a problem creating the gateway socket!\n");
		return -3;
	}

	if (gateway_socket->Bind(connect_socket_url) < 0) {
		KPrint("There was a problem binding the gateway socket!\n");
		return -4;
	}

	while(!finished) {
		KSync::Comm::CommObject* recv_obj = 0;
		if(gateway_socket->Recv(recv_obj) == 0) {
			std::string message;
			if(recv_obj->GetString(message) < 0) {
				Warning("There was a problem decoding message!\n");
			} else {
				KPrint("Received (%s)\n", message.c_str());
			}
			delete recv_obj;
			//usleep(1*1000000);
			usleep(100000);
			KSync::Comm::CommObject* send_obj = new KSync::Comm::CommObject(message);
			if(gateway_socket->Send(send_obj) != 0) {
				Warning("There was a problem sending a message!!");
			}
			delete send_obj;
		}
		//if(KSync::Server::Process_New_Connections(active_sockets, connection_socket) < 0) {
		//	Warning("There was an error processing new connections\n");
		//}
	}

	//for(size_t i=0; i< active_sockets.size(); ++i) {
	//	if(KSync::SocketOps::Shutdown_Socket(active_sockets[i].first, active_sockets[i].second) < 0) {
	//		Warning("There was a problem while trying to shutdown the server side of a socket!\n");
	//	}
	//}

	delete gateway_socket;
	delete comm_system;
	return 0;
}

//int KSync::Server::Process_New_Connections(std::vector<std::pair<int,int>> active_sockets, const int connection_socket) {
//	std::string message;
//	int status = KSync::SocketOps::Receive_Message(message, connection_socket);
//	if(status < 0) {
//		//Warning("There was a problem receiving a message from the connection socket!\n");
//		return -1;
//	}
//
//	if(status == 0) {
//		printf("Received the message: %s\n", message.c_str());
//	}
//	active_sockets.size();
//	return 0;
//}
