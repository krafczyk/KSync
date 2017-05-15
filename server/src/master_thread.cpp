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
#include <thread>

#include "ksync/master_thread.h"
#include "ksync/logging.h"
#include "ksync/messages.h"
#include "ksync/utilities.h"
#include "ksync/comm_system_interface.h"
#include "ksync/comm_system_factory.h"
#include "ksync/comm_system_object.h"
#include "ksync/gateway_thread.h"

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

	//Define and process command line arguments
	std::string gateway_socket_url;
	bool gateway_socket_url_defined;
	bool nanomsg;

	ArgParse::ArgParser arg_parser("KSync Server - Server side of a Client-Server synchonization system using rsync.");
	KSync::Utilities::set_up_common_arguments_and_defaults(arg_parser, gateway_socket_url, gateway_socket_url_defined, nanomsg);

	int status;
	if((status = arg_parser.ParseArgs(argc, argv)) < 0) {
		Error("Problem parsing arguments\n");
		arg_parser.PrintHelp();
		return -1;
	}

	if(arg_parser.HelpPrinted()) {
		return 0;
	}

	//Get gateway URL
	if (!gateway_socket_url_defined) {
		if(KSync::Utilities::get_default_ipc_connection_url(gateway_socket_url) < 0) {
			Error("There was a problem getitng the default IPC connection URL.\n");
			return -2;
		}
	} else {
		if(gateway_socket_url.substr(0, 3) != "icp") {
			Error("Non icp sockets are not properly implemented at this time.\n");
			return -2;
		}
	}

	KPrint("Using the following socket url: %s\n", gateway_socket_url.c_str());

	//Initialize communication system
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

	//Initialize Gateway Thread socket
	KSync::Comm::CommSystemSocket* gateway_thread_socket = 0;
	if (comm_system->Create_Pair_Socket(gateway_thread_socket) < 0) {
		KPrint("There was a problem creating the gateway thread socket!\n");
		return -3;
	}

	if (gateway_thread_socket->SetRecvTimeout(1000) < 0) {
		KPrint("There was a problem setting the recv timeout!\n");
		return -3;
	}

	std::string gateway_thread_socket_url;
	if(KSync::Utilities::get_default_gateway_thread_url(gateway_thread_socket_url) < 0) {
		KPrint("There was a problem getting the default gateway thread socket url!\n");
		return -4;
	}

	if(gateway_thread_socket->Bind(gateway_thread_socket_url) < 0) {
		KPrint("There was a problem binding the gateway thread socket!\n");
		return -5;
	}

	//Launch Gateway Thread
	std::thread gateway(KSync::Server::gateway_thread, comm_system, gateway_thread_socket_url, gateway_socket_url);

	//Acknowledge connection
	Debug("1\n");
	KSync::Comm::CommObject* herald_obj = 0;
	status = gateway_thread_socket->Recv(herald_obj);
	if(status == KSync::Comm::CommSystemSocket::Other) {
		Error("Didn't receive connect herald!\n");
		return -6;
	} else if (status == KSync::Comm::CommSystemSocket::Timeout) {
		Error("Herald retreive timed out!!!\n");
		return -7;
	} else {
		Debug("2\n");
		if(herald_obj->GetType() == KSync::Comm::SocketConnectHerald::Type) {
			KSync::Comm::SocketConnectAcknowledge ack;
			KSync::Comm::CommObject* ack_obj = ack.GetCommObject();
			status = gateway_thread_socket->Send(ack_obj);
			if(status == KSync::Comm::CommSystemSocket::Other) {
				Error("Couldn't send Acknowledgement!\n");
				return -7;
			} else if (status == KSync::Comm::CommSystemSocket::Timeout) {
				Error("Retreival of Ack timed out!!\n");
				return -8;
			}
			delete ack_obj;
		} else {
			Error("Didn't get a herald type (%i)!!\n", herald_obj->GetType());
			return -8;
		}
	}
	delete herald_obj;
	Debug("3\n");

	while(!finished) {
		Debug("4\n");
		//Check gateway thread
		KSync::Comm::CommObject* recv_obj = 0;
		status = gateway_thread_socket->Recv(recv_obj);
		Debug("5\n");
		if(status == KSync::Comm::CommSystemSocket::Other) {
			Error("There was a problem checking the gateway thread socket!\n");
			return -9;
		} else if (status == KSync::Comm::CommSystemSocket::Timeout) {
			Warning("Receiving info from gateway thread timed out\n");
		} else if (status == KSync::Comm::CommSystemSocket::EmptyMessage) {
			Warning("Received EmptyMessage!\n");
		} else {
			// Handle connection request!!
			if(recv_obj->GetType() == KSync::Comm::GatewaySocketInitializationRequest::Type) {
				KSync::Comm::GatewaySocketInitializationChangeId response;
				Debug("6\n");
				KSync::Comm::CommObject* resp_obj = response.GetCommObject();
				Debug("7\n");
				status = gateway_thread_socket->Send(resp_obj);
				if(status == KSync::Comm::CommSystemSocket::Other) {
					Error("There was a problem sending a response to the gateway thread socket!\n");
					return -11;
				} else if (status == KSync::Comm::CommSystemSocket::Timeout) {
					Error("Sending response timedout!!\n");
					return -12;
				}
			} else {
				Debug("6\n");
				Error("Unsupported message from gateway thread! (%i) (%s)\n", recv_obj->GetType(), KSync::Comm::GetTypeName(recv_obj->GetType()));
				Debug("7\n");
				return -11;
			}
		}
	}

	delete comm_system;
	return 0;
}
