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
#include <sstream>
#include <utility>

#include "ksync/server.h"
#include "ksync/logging.h"
#include "ksync/messages.h"
#include "ksync/socket_ops.h"

#include "ArgParse/ArgParse.h"

int main(int argc, char** argv) {
	Debug("Error: %i (%s)\n", nn_errno(), nn_strerror(nn_errno()));
	std::vector<std::pair<int,int>> active_sockets;

	char* login_name = getlogin();
	//char* login_name = 0;
	Debug("Error: %i (%s)\n", nn_errno(), nn_strerror(nn_errno()));
	if(login_name == 0) {
		Error("Couldn't get the username!\n");
		return -1;
	}
	errno = 0;
	Debug("Error: %i (%s)\n", nn_errno(), nn_strerror(nn_errno()));

	std::stringstream ss;
	ss << "ipc:///temp/" << login_name << "/ksync-connect.ipc";

	Debug("Error: %i (%s)\n", nn_errno(), nn_strerror(nn_errno()));
	std::string connect_socket_url = ss.str();

	Debug("Error: %i (%s)\n", nn_errno(), nn_strerror(nn_errno()));
	ArgParse::ArgParser arg_parser("KSync Server - Server side of a Client-Server synchonization system using rsync.");
	arg_parser.AddArgument("connect-socket", "Socket to use to negotiate new client connections. Default is : ipc:///ksync/<user>/ksync-connect.ipc", &connect_socket_url);

	Debug("Error: %i (%s)\n", nn_errno(), nn_strerror(nn_errno()));
	int status;
	if((status = arg_parser.ParseArgs(argc, argv)) < 0) {
		Error("Problem parsing arguments\n");
		arg_parser.PrintHelp();
		return -1;
	}

	Debug("Error: %i (%s)\n", nn_errno(), nn_strerror(nn_errno()));
	if(arg_parser.HelpPrinted()) {
		return 0;
	}

	Debug("Error: %i (%s)\n", nn_errno(), nn_strerror(nn_errno()));
	printf("Using the following socket url: %s\n", connect_socket_url.c_str());

	//Start the connection socket
	
	int connection_socket = 0;
	int endpoint = 0;

	Debug("Error: %i (%s)\n", nn_errno(), nn_strerror(nn_errno()));
	if(KSync::SocketOps::Create_And_Bind_Connection_Socket(connection_socket, endpoint, connect_socket_url) < 0) {
		Error("There was a problem creating and binding the connection socket!\n");
		return -2;
	}

	active_sockets.push_back(std::pair<int,int>(connection_socket,endpoint));

	bool finished = false;
	while(!finished) {
		if(KSync::Server::Process_New_Connections(active_sockets, connection_socket) < 0) {
			Warning("There was an error processing new connections\n");
		}
		usleep(100000);
	}

	for(size_t i=0; i< active_sockets.size(); ++i) {
		if(KSync::SocketOps::Shutdown_Socket(active_sockets[i].first, active_sockets[i].second) < 0) {
			Warning("There was a problem while trying to shutdown the server side of a socket!\n");
		}
	}

	return 0;
}

int KSync::Server::Process_New_Connections(std::vector<std::pair<int,int>> active_sockets, const int connection_socket) {
	std::string message;
	int status = KSync::SocketOps::Receive_Message(message, connection_socket);
	if(status < 0) {
		Warning("There was a problem receiving a message from the connection socket!\n");
		return -1;
	}

	if(status == 0) {
		printf("Received the message: %s\n", message.c_str());
	}
	active_sockets.size();
	return 0;
}
