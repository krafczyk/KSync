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
#include "ksync/utilities.h"
#include "ksync/comm_system_interface.h"
#include "ksync/comm_system_factory.h"

#include "ArgParse/ArgParse.h"

int main(int argc, char** argv) {
	std::string gateway_socket_url;
	bool gateway_socket_url_defined;
	bool nanomsg;

	ArgParse::ArgParser arg_parser("KSync Server - Client side of a Client-Server synchonization system using rsync.");
	KSync::Utilities::set_up_common_arguments_and_defaults(arg_parser, gateway_socket_url, gateway_socket_url_defined, nanomsg);

	if(arg_parser.ParseArgs(argc, argv) < 0) {
		Error("Problem parsing arguments\n");
		arg_parser.PrintHelp();
		return -1;
	}

	if(arg_parser.HelpPrinted()) {
		return 0;
	}

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

	if(gateway_socket->SetRecvTimeout(20000) < 0) {
		Error("There was a problem setting the receive timeout!\n");
		return -4;
	}

	if (gateway_socket->Connect(gateway_socket_url) < 0) {
		Error("There was a problem connecting to the gateway socket!\n");
		return -4;
	}

	int status = 0;

	while (true) {
		KPrint("Print message to send to the server:\n");
		KSync::Comm::CommString message_to_send;
		std::getline(std::cin, message_to_send);
		KSync::Comm::CommObject* send_obj = 0;
		if(message_to_send == "request") {
			KPrint("Sending request\n");
			KSync::Comm::GatewaySocketInitializationRequest request(KSync::Utilities::GenerateNewClientId());
			send_obj = request.GetCommObject();
		} else {
			KPrint("Sending message: (%s)\n", message_to_send.c_str());
			send_obj = message_to_send.GetCommObject();
		}

		status = gateway_socket->Send(send_obj);
		if(status == KSync::Comm::CommSystemSocket::Other) {
			Error("There was a problem sending the message!!\n");
		} else if (status == KSync::Comm::CommSystemSocket::Timeout) {
			Warning("Sending the message timed out!\n");
		} else {
			KSync::Comm::CommObject* recv_obj = 0;
			status = gateway_socket->Recv(recv_obj);
			if(status == KSync::Comm::CommSystemSocket::Other) {
				Error("Problem receiving response\n");
			} else if (status == KSync::Comm::CommSystemSocket::Timeout) {
				Warning("Receiving response timed out!\n");
			} else {
				if(recv_obj->GetType() == KSync::Comm::GatewaySocketInitializationChangeId::Type) {
					Warning("Received a ChangeId Request!!\n");
				} else if(recv_obj->GetType() == KSync::Comm::CommString::Type) {
					KSync::Comm::CommString message(recv_obj);
					KPrint("Received (%s)\n", message.c_str());
					if(message_to_send != message) {
						Error("Message received wasn't the same as that sent!\n");
					}
				} else {
					Error("Unrecognized Message!\n");
				}
				delete recv_obj;
			}
		}
		delete send_obj;
	}

	delete gateway_socket;
	delete comm_system;

	return 0;
}
