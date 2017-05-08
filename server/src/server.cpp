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

	std::string connect_socket_url;
	bool connect_socket_url_defined;
	bool nanomsg;

	ArgParse::ArgParser arg_parser("KSync Server - Server side of a Client-Server synchonization system using rsync.");
	KSync::Utilities::set_up_common_arguments_and_defaults(arg_parser, connect_socket_url, connect_socket_url_defined, nanomsg);

	int status;
	if((status = arg_parser.ParseArgs(argc, argv)) < 0) {
		Error("Problem parsing arguments\n");
		arg_parser.PrintHelp();
		return -1;
	}

	if(arg_parser.HelpPrinted()) {
		return 0;
	}

	if(!connect_socket_url_defined) {
		connect_socket_url = "tcp://*:5555";
	}

	KPrint("Using the following socket url: %s\n", connect_socket_url.c_str());

	KSync::Comm::CommSystemInterface* comm_system = 0;
	if (!nanomsg) {
		if (KSync::Comm::GetZeromqCommSystem(comm_system) < 0) {
			KPrint("There was a problem initializing the ZeroMQ communication system!\n");
			return -2;
		}
	} else {
		if (KSync::Comm::GetNanomsgCommSystem(comm_system) < 0) {
			KPrint("There was a problem initializing the Nanomsg communication system!\n");
			return -2;
		}
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
			if (message == "quit") {
				KPrint("Detected 'quit'. Quitting.\n");
				finished = true;
			} else {
				//usleep(1*1000000);
				usleep(100000);
				KSync::Comm::CommObject* send_obj = new KSync::Comm::CommObject(message);
				if(gateway_socket->Send(send_obj) != 0) {
					Warning("There was a problem sending a message!!");
				} 
				delete send_obj;
			}
		}
	}

	delete gateway_socket;
	delete comm_system;
	return 0;
}
