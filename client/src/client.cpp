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
#include <iostream>
#include <sstream>
#include <iostream>

#include "ksync/logging.h"
#include "ksync/client.h"
#include "ksync/messages.h"
//#include "ksync/socket_ops.h"
#include "ksync/utilities.h"
#include "ksync/comm_system_interface.h"
#include "ksync/comm_system_factory.h"

#include "ArgParse/ArgParse.h"

int main(int argc, char** argv) {
	std::string connect_socket_url = "";
	bool connect_socket_url_defined = false;
	bool nanomsg = false;

	ArgParse::ArgParser arg_parser("KSync Server - Client side of a Client-Server synchonization system using rsync.");
	arg_parser.AddArgument("--nanomsg", "Use nanomsg comm backend. Deafult is zeromq", &nanomsg);
	arg_parser.AddArgument("connect-socket", "Socket to use to negotiate new client connections. Default is : ipc:///ksync/<user>/ksync-connect.ipc", &connect_socket_url, ArgParse::Argument::Optional, &connect_socket_url_defined);

	if(arg_parser.ParseArgs(argc, argv) < 0) {
		Error("Problem parsing arguments\n");
		arg_parser.PrintHelp();
		return -1;
	}

	if(arg_parser.HelpPrinted()) {
		return 0;
	}

	connect_socket_url = "tcp://localhost:5555";

	//if(!connect_socket_url_defined){
	//	if(KSync::Utilities::get_default_connection_url(connect_socket_url) < 0) {
	//		Error("There was a problem getting the default connection url!\n");
	//		return -2;
	//	}
	//}

	printf("Using the following socket url: %s\n", connect_socket_url.c_str());

	KSync::Comm::CommSystemInterface* comm_system = 0;
	if (!nanomsg) {
		if (KSync::Comm::GetZeromqCommSystem(comm_system) < 0) {
			Error("There was a problem initializing the ZeroMQ communication system!\n");
			return -2;
		}
	} else {
		if (KSync::Comm::GetNanomsgCommSystem(comm_system) < 0) {
			Error("There was a problem initializing the Nanomsg communication system!\n");
			return -2;
		}
	}

	KSync::Comm::CommSystemSocket* gateway_socket = 0;
	if (comm_system->Create_Gateway_Req_Socket(gateway_socket) < 0) {
		Error("There was a problem creating the gateway socket!\n");
		return -3;
	}

	if (gateway_socket->Connect(connect_socket_url) < 0) {
		Error("There was a problem connecting to the gateway socket!\n");
		return -4;
	}

	while (true) {
		printf("Print message to send to the server:\n");
		std::string message_to_send;
		std::getline(std::cin, message_to_send);
		printf("Sending message: (%s)\n", message_to_send.c_str());
		KSync::Comm::CommObject* send_obj = new KSync::Comm::CommObject(message_to_send);
		if(gateway_socket->Send(send_obj) == 0) {
			if (message_to_send == "quit") {
				printf("Detected quit message. Quitting.");
				break;
			}
			KSync::Comm::CommObject* recv_obj = 0;
			if(gateway_socket->Recv(recv_obj) != 0) {
				Warning("Problem receiving response\n");
			} else {
				std::string message;
				if(recv_obj->GetString(message) < 0) {
					printf("There was a problem decoding string message\n");
				}
				KPrint("Received (%s)\n", message.c_str());
				if(message_to_send != message) {
					printf("Message received wasn't the same as that sent!\n");
				}
				delete recv_obj;
			}
		} else {
			Error("There was a problem sending the message!!\n");
		}
		delete send_obj;
	}

	delete gateway_socket;
	delete comm_system;

	//int connection_socket = 0;
	//int connection_endpoint = 0;
	//
	//printf("create and connect\n");
	//
	//errno=0;
	//if(KSync::SocketOps::Create_And_Connect_Req_Socket(connection_socket, connection_endpoint, connect_socket_url) < 0) {
	//	Error("Encountered an error trying to create and connect to the connection socket\n");
	//	return -1;
	//}
	//if(KSync::SocketOps::Set_Socket_Linger(connection_socket, -1) < 0) {
	//	Error("Couldn't set socket linger\n");
	//	return -1;
	//}
	//	
	//printf("send\n");

	//std::string message = "test_message";
	//if(KSync::SocketOps::Send_Message(message, connection_socket) < 0) {
	//	Warning("There was a problem sending a message to the server.\n");
	//}

	//usleep(1000000);

	//printf("shutdown\n");

	//if(KSync::SocketOps::Shutdown_Socket(connection_socket, connection_endpoint) < 0) {
	//	Warning("There was a problem shutting down the client side of the connection socket.\n");
	//}

	return 0;
}
