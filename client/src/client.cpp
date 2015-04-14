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

#include "ArgParse/ArgParse.h"

int main(int argc, char** argv) {
	char* login_name = getlogin();
	if(login_name == 0) {
		Error("Couldn't get the username!\n");
		return -1;
	}

	std::stringstream ss;
	ss << "ipc:///temp/" << login_name << "/ksync-connect.ipc";

	std::string connect_socket_url = ss.str();

	ArgParse::ArgParser arg_parser("KSync Server - Client side of a Client-Server synchonization system using rsync.");
	arg_parser.AddOption("connect-socket", "Socket to use to negotiate new client connections. Default is : ipc:///ksync/<user>/ksync-connect.ipc", &connect_socket_url);

	if(arg_parser.ParseArgs(argc, argv) < 0) {
		Error("Problem parsing arguments\n");
		arg_parser.PrintHelp();
		return -1;
	}

	if(arg_parser.HelpPrinted()) {
		return 0;
	}

	int connection_socket = 0;
	int connection_endpoint = 0;

	printf("create and connect\n");

	if(KSync::SocketOps::Create_And_Connect_Connection_Socket(connection_socket, connection_endpoint, connect_socket_url) < 0) {
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
