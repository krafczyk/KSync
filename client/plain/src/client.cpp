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
#include <chrono>
#include <thread>
#include <atomic>
#include <future>

#include "ksync/logging.h"
#include "ksync/client.h"
#include "ksync/messages.h"
#include "ksync/utilities.h"
#include "ksync/comm/interface.h"
#include "ksync/comm/factory.h"

#include "ksync/ArgParseStandalone.h"

int main(int argc, char** argv) {
	std::string log_dir;
	std::string gateway_socket_url;
	bool gateway_socket_url_defined;
	bool nanomsg;

	ArgParse::ArgParser arg_parser("KSync Server - Client side of a Client-Server synchonization system using rsync.");
	KSync::Utilities::set_up_common_arguments_and_defaults(arg_parser, log_dir, gateway_socket_url, gateway_socket_url_defined, nanomsg);

	if(arg_parser.ParseArgs(argc, argv) < 0) {
		LOGF(SEVERE, "Problem parsing arguments");
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
	KSync::InitializeLogger(logworker, true, "KSync Client", log_dir);

	//Get Default gateway socket url
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

	//Initialize Comm System
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

	//Connect to gateway socket
	std::shared_ptr<KSync::Comm::CommSystemSocket> gateway_socket;
	if (comm_system->Create_Gateway_Req_Socket(gateway_socket) < 0) {
		LOGF(SEVERE, "There was a problem creating the gateway socket!");
		return -3;
	}

	if(gateway_socket->SetRecvTimeout(20000) < 0) {
		LOGF(SEVERE, "There was a problem setting the receive timeout!");
		return -4;
	}

	if (gateway_socket->Connect(gateway_socket_url) < 0) {
		LOGF(SEVERE, "There was a problem connecting to the gateway socket!");
		return -4;
	}

	std::shared_ptr<KSync::Comm::CommSystemSocket> client_socket;
	std::shared_ptr<KSync::Comm::CommSystemSocket> broadcast_socket;
	int status = 0;
	//Request client socket connection
	while (!client_socket) {
		KSync::Comm::GatewaySocketInitializationRequest request(KSync::Utilities::GenerateNewClientId());
		std::shared_ptr<KSync::Comm::CommObject> request_obj = request.GetCommObject();
		status = gateway_socket->Send(request_obj);
		if(status == KSync::Comm::CommSystemSocket::Other) {
			LOGF(WARNING, "There was a problem sending the message!!");
		} else if (status == KSync::Comm::CommSystemSocket::Timeout) {
			LOGF(WARNING, "Sending the message timed out!");
		} else {
			std::shared_ptr<KSync::Comm::CommObject> recv_obj;
			status = gateway_socket->Recv(recv_obj);
			if(status == KSync::Comm::CommSystemSocket::Other) {
				LOGF(WARNING, "Problem receiving response");
			} else if (status == KSync::Comm::CommSystemSocket::Timeout) {
				LOGF(WARNING, "Receiving response timed out!");
			} else {
				if(recv_obj->GetType() == KSync::Comm::ClientSocketCreation::Type) {
					std::shared_ptr<KSync::Comm::ClientSocketCreation> creation_response;
					KSync::Comm::CommCreator(creation_response, recv_obj);
					//Start and connect to client socket
					if(comm_system->Create_Pair_Socket(client_socket) < 0) {
						LOGF(SEVERE, "There was a problem creating the pair socket!");
						return -1;
					}
					if(client_socket->SetRecvTimeout(1000) < 0) {
						LOGF(SEVERE, "There was a problem setting the client socket timeout!");
						return -2;
					}
					if(client_socket->Connect(creation_response->GetClientUrl()) < 0) {
						LOGF(SEVERE, "Couldn't connect to the new client socket address!!");
						return -3;
					}

					//Start and connect to broadcast socket
					if(comm_system->Create_Sub_Socket(broadcast_socket) < 0) {
						LOGF(SEVERE, "There was a problem creating the broadcast socket!");
						return -4;
					}
					if(broadcast_socket->SetRecvTimeout(1000) < 0) {
						LOGF(SEVERE, "Couldn't set the timeout of the broadcast socket!");
						return -5;
					}
					if(broadcast_socket->Connect(creation_response->GetBroadcastUrl()) < 0) {
						LOGF(SEVERE, "There was a problem connecting to the broadcast socket!");
						return -5;
					}
				} else if(recv_obj->GetType() == KSync::Comm::GatewaySocketInitializationChangeId::Type) {
					LOGF(WARNING, "Received a ChangeId Request!!");
				} else {
					LOGF(WARNING, "Unrecognized Message!");
				}
			}
		}
	}

	//Close gateway socket connection
	gateway_socket.reset();

	std::shared_ptr<std::thread> io_thread;
	std::shared_ptr<std::promise<std::string>> io_result;
	std::future<std::string> io_future;
	bool finished = false;
	while (!finished) {
		//restart thread if it's stopped
		if(!io_thread) {
			//Flush and clear cin buffer and item string
			if(std::cin.rdbuf()->in_avail() > 0) {
				std::cin.ignore(std::cin.rdbuf()->in_avail());
			}
			std::cin.clear();
			//Restart IO thread
			//Reset finished bool
			io_result.reset(new std::promise<std::string>);
			io_future = io_result->get_future();
			io_thread.reset(new std::thread([&io_result] {
				std::string item;
				printf("Print message to send to the server:\n");
				std::getline(std::cin, item);
				if(item == "quit") {
					std::string item2;
					int action_selection_state = 2;
					while (action_selection_state == 2) {
						printf("Are you sure you want to quit? (Y/n)\n");
						item2.clear();
						std::getline(std::cin, item2);
						if ((item2 == "y")||(item2 == "Y")) {
							action_selection_state = 0;
						} else if ((item2 == "n") || (item2 == "N")) {
							action_selection_state = 1;
						} else {
							printf("Please choose either Y or n.\n");
						}
					}
				}
				io_result->set_value(item);
			}));
		}

		//Test if result is ready
		if(io_result) {
			std::future_status fut_stat;
			try {
				fut_stat = io_future.wait_for(std::chrono::milliseconds(0));
			} catch (std::future_error e) {
				LOGF(SEVERE, "There was a futures error!!! (%s)", e.what());
				return -100;
			}
			if((fut_stat== std::future_status::ready)&&(!finished)) {
				//Result is ready
				KSync::Comm::CommString message_to_send = io_future.get();
				if (message_to_send == "quit") {
					KSync::Comm::ShutdownRequest shutdown_req;
					std::shared_ptr<KSync::Comm::CommObject> shutdown_obj = shutdown_req.GetCommObject();
					status = client_socket->Send(shutdown_obj);
					if(status == KSync::Comm::CommSystemSocket::Other) {
						LOGF(WARNING, "There was an error sending the shutdown request!");
					} else if (status == KSync::Comm::CommSystemSocket::Timeout) {
						LOGF(WARNING, "There was a timeout sending the shutdown request!");
					} else {
						std::shared_ptr<KSync::Comm::CommObject> rep_obj;
						status = client_socket->ForceRecv(rep_obj);
						if(status == KSync::Comm::CommSystemSocket::Other) {
							LOGF(WARNING, "There was a problem getting a response to the shutdown request!");
						} else {
							if(rep_obj->GetType() != KSync::Comm::ShutdownAck::Type) {
								LOGF(WARNING, "Shutdown Acknowledgement not received!");
								break;
							}
						}
					}
				} else if (message_to_send.substr(0,8) == "command:") {
					std::string extracted_command = message_to_send.substr(8);
					extracted_command = KSync::Utilities::trim(extracted_command);
					KSync::Comm::ExecuteCommand command = extracted_command;
					std::shared_ptr<KSync::Comm::CommObject> send_obj = command.GetCommObject();
					status = client_socket->Send(send_obj);
					if(status == KSync::Comm::CommSystemSocket::Other) {
						LOGF(WARNING, "There was a problem sending the command!");
					} else if (status == KSync::Comm::CommSystemSocket::Timeout) {
					} else {
						std::shared_ptr<KSync::Comm::CommObject> ret_obj;
						status = client_socket->ForceRecv(ret_obj);
						if(status == KSync::Comm::CommSystemSocket::Other) {
							LOGF(WARNING, "There was a problem receiving the reply!");
						} else if (status == KSync::Comm::CommSystemSocket::Success) {
							if(ret_obj->GetType() == KSync::Comm::CommandOutput::Type) {
								std::shared_ptr<KSync::Comm::CommandOutput> com_output;
								KSync::Comm::CommCreator(com_output, ret_obj);
								printf("Output:\n");
								printf("%s\n", com_output->GetStdout().c_str());
								printf("Error:\n");
								printf("%s\n", com_output->GetStderr().c_str());
								printf("Return Code: (%i)\n", com_output->GetReturnCode());
							} else {
								LOGF(WARNING, "Other object types are not supported here");
							}
						}
					}
				} else {
					std::shared_ptr<KSync::Comm::CommObject> send_obj;
					LOGF(INFO, "Sending message: (%s)", message_to_send.c_str());
					send_obj = message_to_send.GetCommObject();
					status = client_socket->Send(send_obj);
					if(status == KSync::Comm::CommSystemSocket::Other) {
						LOGF(SEVERE, "There was a problem sending the string to the server!");
						return -1;
					} else if (status == KSync::Comm::CommSystemSocket::Timeout) {
						LOGF(SEVERE, "Sending the message timed out!!");
						return -2;
					} else {
						std::shared_ptr<KSync::Comm::CommObject> recv_obj;
						status = client_socket->ForceRecv(recv_obj);
						if(status == KSync::Comm::CommSystemSocket::Other) {
							LOGF(SEVERE, "There was a problem receiving a response!");
							return -3;
						} else if ((status == KSync::Comm::CommSystemSocket::Timeout)||(status == KSync::Comm::CommSystemSocket::EmptyMessage)) {
						} else {
							if(recv_obj->GetType() == KSync::Comm::CommString::Type) {
								std::shared_ptr<KSync::Comm::CommString> received_string;
								KSync::Comm::CommCreator(received_string, recv_obj);
								LOGF(INFO, "Received(%s)", received_string->c_str());
								if (*received_string != message_to_send) {
									LOGF(SEVERE, "Received message was different!");
								}
							}
						}
					}
				}
				//Cleanup IO thread stuff
				if(io_thread) {
					if(io_thread->joinable()) {
						io_thread->join();
					}
					io_thread.reset();
				}
			}
		}

		//Test broadcast socket
		std::shared_ptr<KSync::Comm::CommObject> broad_obj;
		status = broadcast_socket->Recv(broad_obj);
		if(status == KSync::Comm::CommSystemSocket::Other) {
			LOGF(WARNING, "There was a problem reading from the broadcast socket!");
		} else if ((status == KSync::Comm::CommSystemSocket::Timeout)||(status == KSync::Comm::CommSystemSocket::EmptyMessage)) {
		} else {
			if(broad_obj->GetType() == KSync::Comm::ServerShuttingDown::Type) {
				LOGF(WARNING, "Server shutdown detected, Shutting down.");
				finished = true;
			} else {
				LOGF(WARNING, "Other message types unsupported here!");
			}
		}
	}

	//Join io_thread;
	if(io_thread) {
		if(io_thread->joinable()) {
			io_thread->join();
		}
	}
	return 0;
}
