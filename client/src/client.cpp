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

#include "ksync/logging.h"
#include "ksync/client.h"
#include "ksync/messages.h"
#include "ksync/socket_ops.h"
#include "ksync/utilities.h"

#include "ArgParse/ArgParse.h"

int main(int argc, char** argv) {
	std::string connect_socket_url = "";
	bool connect_socket_url_defined = false;

	ArgParse::ArgParser arg_parser("KSync Server - Client side of a Client-Server synchonization system using rsync.");
	arg_parser.AddArgument("connect-socket", "Socket to use to negotiate new client connections. Default is : ipc:///ksync/<user>/ksync-connect.ipc", &connect_socket_url, ArgParse::Argument::Optional, &connect_socket_url_defined);

	if(arg_parser.ParseArgs(argc, argv) < 0) {
		Error("Problem parsing arguments\n");
		arg_parser.PrintHelp();
		return -1;
	}

	if(arg_parser.HelpPrinted()) {
		return 0;
	}

	if(!connect_socket_url_defined){
		std::string socket_dir;
		if(KSync::Utilities::get_socket_dir(socket_dir) < 0) {
			Error("There was a problem getting the default socket directory!\n");
			return -2;
		}
		std::stringstream ss;
		ss << "ipc://" << socket_dir << "/ksync-connect.ipc";
		connect_socket_url = ss.str();
	}

	int connection_socket = 0;
	int connection_endpoint = 0;

	printf("create and connect\n");

	errno=0;
	if(KSync::SocketOps::Create_And_Connect_Req_Socket(connection_socket, connection_endpoint, connect_socket_url) < 0) {
		Error("Encountered an error trying to create and connect to the connection socket\n");
		return -1;
	}

	printf("send\n");

	std::string message = "test_message";
	if(KSync::SocketOps::Send_Message(message, connection_socket) < 0) {
		Warning("There was a problem sending a message to the server.\n");
	}

	printf("shutdown\n");

	if(KSync::SocketOps::Shutdown_Socket(connection_socket, connection_endpoint) < 0) {
		Warning("There was a problem shutting down the client side of the connection socket.\n");
	}

	return 0;
}
