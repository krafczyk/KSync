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
#include <map>

#include "ksync/master_thread.h"
#include "ksync/logging.h"
#include "ksync/messages.h"
#include "ksync/utilities.h"
#include "ksync/comm/interface.h"
#include "ksync/comm/factory.h"
#include "ksync/comm/object.h"
#include "ksync/command_system_interface.h"
#include "ksync/pstreams_command_system.h"
#include "ksync/gateway_thread.h"
#include "ksync/pstream.h"

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
	std::string log_dir;
	std::string gateway_socket_url;
	bool gateway_socket_url_defined;
	bool nanomsg;

	ArgParse::ArgParser arg_parser("KSync Server - Server side of a Client-Server synchonization system using rsync.");
	KSync::Utilities::set_up_common_arguments_and_defaults(arg_parser, log_dir, gateway_socket_url, gateway_socket_url_defined, nanomsg);

	int status;
	if((status = arg_parser.ParseArgs(argc, argv)) < 0) {
		printf("Problem parsing arguments\n");
		arg_parser.PrintHelp();
		return -1;
	}

	if(arg_parser.HelpPrinted()) {
		return 0;
	}

	if (log_dir == "") {
		if(KSync::Utilities::get_user_ksync_dir(log_dir) < 0) {
			printf("There was a problem getting the ksync user directory!\n");
			return -2;
		}
	}

	//Initialize logging:
	std::unique_ptr<g3::LogWorker> logworker;
	KSync::InitializeLogger(logworker, true, "KSync Server", log_dir);

	//Get gateway URL
	if (!gateway_socket_url_defined) {
		if(KSync::Utilities::get_default_ipc_connection_url(gateway_socket_url) < 0) {
			LOGF(SEVERE, "There was a problem getitng the default IPC connection URL.");
			return -2;
		}
	} else {
		if(gateway_socket_url.substr(0, 3) != "icp") {
			LOGF(SEVERE, "Non icp sockets are not properly implemented at this time.");
			return -2;
		}
	}

	LOGF(INFO, "Using the following socket url: %s", gateway_socket_url.c_str());

	//Initialize communication system
	std::shared_ptr<KSync::Comm::CommSystemInterface> comm_system;
	if (!nanomsg) {
		if (KSync::Comm::GetZeromqCommSystem(comm_system) < 0) {
			LOGF(SEVERE, "There was a problem initializing the ZeroMQ communication system!");
			return -2;
		}
	} else {
		if (KSync::Comm::GetNanomsgCommSystem(comm_system) < 0) {
			LOGF(SEVERE, "There was a problem initializing the Nanomsg communication system!");
			return -2;
		}
	}

	//Initialize command system
	std::shared_ptr<KSync::Commanding::SystemInterface> command_system(new KSync::Commanding::PSCommandSystem());

	//Initialize Gateway Thread socket
	std::shared_ptr<KSync::Comm::CommSystemSocket> gateway_thread_socket;
	if (comm_system->Create_Pair_Socket(gateway_thread_socket) < 0) {
		LOGF(SEVERE, "There was a problem creating the gateway thread socket!");
		return -3;
	}

	if (gateway_thread_socket->SetRecvTimeout(1000) < 0) {
		LOGF(SEVERE, "There was a problem setting the recv timeout!");
		return -3;
	}

	std::string gateway_thread_socket_url;
	if(KSync::Utilities::get_default_gateway_thread_url(gateway_thread_socket_url) < 0) {
		LOGF(SEVERE, "There was a problem getting the default gateway thread socket url!");
		return -4;
	}

	if(gateway_thread_socket->Bind(gateway_thread_socket_url) < 0) {
		LOGF(SEVERE, "There was a problem binding the gateway thread socket!");
		return -5;
	}

	//Initialize Broadcast socket
	std::shared_ptr<KSync::Comm::CommSystemSocket> broadcast_socket;
	if (comm_system->Create_Pub_Socket(broadcast_socket) < 0) {
		LOGF(SEVERE, "There was a problem creating the broadcast socket!");
		return -3;
	}

	std::string broadcast_url;
	if(KSync::Utilities::get_default_broadcast_url(broadcast_url) < 0) {
		LOGF(SEVERE, "There was a problem getting the default broadcast socket url!");
		return -4;
	}

	if(broadcast_socket->Bind(broadcast_url) < 0) {
		LOGF(SEVERE, "There was a problem binding the broadcast socket!");
		return -5;
	}

	//Launch Gateway Thread
	std::thread gateway(KSync::Server::gateway_thread, comm_system, gateway_thread_socket_url, gateway_socket_url);

	//Acknowledge connection
	std::shared_ptr<KSync::Comm::CommObject> herald_obj;
	status = gateway_thread_socket->Recv(herald_obj);
	if(status == KSync::Comm::CommSystemSocket::Other) {
		LOGF(SEVERE, "Didn't receive connect herald!");
		return -6;
	} else if (status == KSync::Comm::CommSystemSocket::Timeout) {
		LOGF(SEVERE, "Herald retreive timed out!!!");
		return -7;
	} else {
		if(herald_obj->GetType() == KSync::Comm::SocketConnectHerald::Type) {
			KSync::Comm::SocketConnectAcknowledge ack;
			std::shared_ptr<KSync::Comm::CommObject> ack_obj = ack.GetCommObject();
			status = gateway_thread_socket->Send(ack_obj);
			if(status == KSync::Comm::CommSystemSocket::Other) {
				LOGF(SEVERE, "Couldn't send Acknowledgement!");
				return -7;
			} else if (status == KSync::Comm::CommSystemSocket::Timeout) {
				LOGF(SEVERE, "Retreival of Ack timed out!!");
				return -8;
			}
		} else {
			LOGF(SEVERE, "Didn't get a herald type (%i)!!\n", herald_obj->GetType());
			return -8;
		}
	}

	std::map<KSync::Utilities::client_id_t, std::shared_ptr<KSync::Comm::CommSystemSocket>> client_sockets;

	while(!finished) {
		//Check gateway thread
		std::shared_ptr<KSync::Comm::CommObject> recv_obj;
		status = gateway_thread_socket->Recv(recv_obj);
		if(status == KSync::Comm::CommSystemSocket::Other) {
			LOGF(SEVERE,"There was a problem checking the gateway thread socket!");
			return -9;
		} else if (status == KSync::Comm::CommSystemSocket::Timeout) {
		} else if (status == KSync::Comm::CommSystemSocket::EmptyMessage) {
		} else {
			// Handle connection request!!
			if(recv_obj->GetType() == KSync::Comm::GatewaySocketInitializationRequest::Type) {
				LOGF(INFO, "Received a connection request!");
				std::shared_ptr<KSync::Comm::GatewaySocketInitializationRequest> request;
				KSync::Comm::CommCreator(request, recv_obj);
				LOGF(INFO, "Received client id: (%lu)\n",request->GetClientId());
				if(client_sockets.find(request->GetClientId()) == client_sockets.end()) {
					LOGF(INFO, "Generating new client socket!");
					//Don't have a client with that ID yet! Handle creation of new socket
					std::string new_socket_url;
					if(KSync::Utilities::get_client_socket_url(new_socket_url, request->GetClientId()) < 0) {
						LOGF(SEVERE, "Error! Couldn't get the client socket url!");
						return -10;
					}

					std::shared_ptr<KSync::Comm::CommSystemSocket> client_socket;
					if (comm_system->Create_Pair_Socket(client_socket) < 0) {
						LOGF(SEVERE, "There was a problem creating the gateway thread socket!");
						return -10;
					}

					if (client_socket->SetRecvTimeout(1000) < 0) {
						LOGF(SEVERE, "There was a problem setting the recv timeout!");
						return -10;
					}

					if(client_socket->Bind(new_socket_url) < 0) {
						LOGF(SEVERE, "There was a problem binding the gateway thread socket!");
						return -10;
					}

					client_sockets[request->GetClientId()] = client_socket;

					KSync::Comm::ClientSocketCreation socket_message;
					socket_message.SetClientUrl(new_socket_url);
					socket_message.SetBroadcastUrl(broadcast_url);
					std::shared_ptr<KSync::Comm::CommObject> socket_message_obj = socket_message.GetCommObject();
					LOGF(INFO, "Sending new client socket address!");
					status = gateway_thread_socket->Send(socket_message_obj);
					if(status == KSync::Comm::CommSystemSocket::Other) {
						LOGF(SEVERE, "Couldn't send response!");
						return -11;
					} else if (status == KSync::Comm::CommSystemSocket::Timeout) {
						LOGF(SEVERE, "Sending response timed out!!");
						return -12;
					}
				} else {
					//We have a client with that ID!
					LOGF(WARNING, "We already have a client with ID (%lu)!\n", request->GetClientId());
					KSync::Comm::GatewaySocketInitializationChangeId response;
					std::shared_ptr<KSync::Comm::CommObject> resp_obj = response.GetCommObject();
					status = gateway_thread_socket->Send(resp_obj);
					if(status == KSync::Comm::CommSystemSocket::Other) {
						LOGF(SEVERE, "There was a problem sending a response to the gateway thread socket!");
						return -11;
					} else if (status == KSync::Comm::CommSystemSocket::Timeout) {
						LOGF(SEVERE, "Sending response timedout!!");
						return -12;
					}
				}
			} else {
				LOGF(SEVERE, "Unsupported message from gateway thread! (%i) (%s)\n", recv_obj->GetType(), KSync::Comm::GetTypeName(recv_obj->GetType()));
				return -11;
			}
		}

		//Check client sockets
		for(auto client_socket_it = client_sockets.begin(); client_socket_it != client_sockets.end(); ++client_socket_it) {
			//Check for incoming messages
			recv_obj = 0;
			std::shared_ptr<KSync::Comm::CommSystemSocket> client_socket = client_socket_it->second;
			status = client_socket->Recv(recv_obj);
			if(status == KSync::Comm::CommSystemSocket::Other) {
				LOGF(WARNING, "There was a problem receiving a message from a client socket!");
			} else if ((status == KSync::Comm::CommSystemSocket::Timeout)||(status == KSync::Comm::CommSystemSocket::EmptyMessage)) {
			} else {
				if(recv_obj->GetType() == KSync::Comm::CommString::Type) {
					std::shared_ptr<KSync::Comm::CommString> message;
					KSync::Comm::CommCreator(message, recv_obj);
					LOGF(INFO, "Got message (%s)\n", message->c_str());
					std::shared_ptr<KSync::Comm::CommObject> send_obj = message->GetCommObject();
					status = client_socket->Send(send_obj);
					if(status == KSync::Comm::CommSystemSocket::Other) {
						LOGF(WARNING, "There was a problem sending a message on a client socket!");
					} else if (status == KSync::Comm::CommSystemSocket::Timeout) {
						LOGF(WARNING, "Sending a message on a client socket timed out!");
					}
				} else if (recv_obj->GetType() == KSync::Comm::ShutdownRequest::Type) {
					finished = true;
					KSync::Comm::ShutdownAck shutdown_ack;
					std::shared_ptr<KSync::Comm::CommObject> shutdown_obj = shutdown_ack.GetCommObject();
					status = client_socket->Send(shutdown_obj);
					if(status == KSync::Comm::CommSystemSocket::Other) {
						LOGF(WARNING, "There was a problem sending the shutdown ack");
					} else if (status == KSync::Comm::CommSystemSocket::Timeout) {
						LOGF(WARNING, "There was a timeout sending the shutdown ack");
					}
				} else if (recv_obj->GetType() == KSync::Comm::ExecuteCommand::Type) {
					std::shared_ptr<KSync::Comm::ExecuteCommand> exec_com;
					KSync::Comm::CommCreator(exec_com, recv_obj);
					LOGF(INFO, "Received command (%s)\n", exec_com->c_str());

					std::shared_ptr<KSync::Commanding::ExecutionContext> command_context = command_system->GetExecutionContext();
					command_context->LaunchCommand(exec_com->c_str());
					std::string std_out;
					std::string std_err;
					command_context->GetOutput(std_out, std_err);

					KSync::Comm::CommandOutput com_out;
					com_out.SetStdout(std_out);
					com_out.SetStderr(std_err);
					com_out.SetReturnCode(command_context->GetReturnCode());

					std::shared_ptr<KSync::Comm::CommObject> test_resp = com_out.GetCommObject();
					status = client_socket->Send(test_resp);
					if(status != KSync::Comm::CommSystemSocket::Success) {
						LOGF(WARNING, "There was a problem sending the test reponse!");
					}
				}
			}
		}
	}

	// Shutting down
	// Broadcast shutdown message
	KSync::Comm::ServerShuttingDown shutdown_message;
	std::shared_ptr<KSync::Comm::CommObject> shutdown_obj = shutdown_message.GetCommObject();
	status = broadcast_socket->Send(shutdown_obj);
	if(status == KSync::Comm::CommSystemSocket::Other) {
		LOGF(WARNING, "There was a problem sending the shutdown message!");
	} else if (status == KSync::Comm::CommSystemSocket::Timeout) {
		LOGF(WARNING, "There was a timeout sending the shutdown message!");
	} else {
		LOGF(WARNING, "Shutdown sent!");
	}
	// Shutdown gateway thread
	status = gateway_thread_socket->Send(shutdown_obj);
	if(status == KSync::Comm::CommSystemSocket::Other) {
		LOGF(WARNING, "There was a problem closing down the gateway thread!");
	} else if (status == KSync::Comm::CommSystemSocket::Timeout) {
		LOGF(WARNING, "There was a timeout closing down the gateway thread!");
	}
	//Join gateway thread
	gateway.join();
	return 0;
}
