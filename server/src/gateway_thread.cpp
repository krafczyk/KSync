#include <unistd.h>

#include "ksync/logging.h"
#include "ksync/gateway_thread.h"

namespace KSync {
	namespace Server {
		void gateway_thread(std::shared_ptr<KSync::Comm::CommSystemInterface> comm_system, const std::string& gateway_thread_socket_url, const std::string& gateway_socket_url) {
			if(comm_system == nullptr) {
				LOGF(SEVERE, "The Gateway thread was given a null comm_system!!\n");
				return;
			}

			//Setup our end of the gateway thread socket
			std::shared_ptr<KSync::Comm::CommSystemSocket> gateway_thread_socket;
			if(comm_system->Create_Pair_Socket(gateway_thread_socket) < 0) {
				LOGF(SEVERE, "There was a problem creating the gateway thread pair socket!\n");
				return;
			}

			if(gateway_thread_socket->SetRecvTimeout(1000) < 0) {
				LOGF(SEVERE, "There was a problem setting the recv timeout!\n");
				return;
			}

			if(gateway_thread_socket->Connect(gateway_thread_socket_url) < 0) {
				LOGF(SEVERE, "There was a problem connectng to the gateway thread pair socket!\n");
				return;
			}

			//Herald our existence
			KSync::Comm::SocketConnectHerald herald;
			std::shared_ptr<KSync::Comm::CommObject> herald_obj = herald.GetCommObject();
			if(gateway_thread_socket->Send(herald_obj) < 0) {
				LOGF(SEVERE, "There was a problem sending the herald message!\n");
				return;
			}

			//Check for acknowledgement
			std::shared_ptr<KSync::Comm::CommObject> ack_obj;
			if(gateway_thread_socket->Recv(ack_obj) < 0) {
				LOGF(SEVERE, "Didn't receive the Acknowledge message!\n");
				return;
			} else {
				if(ack_obj->GetType() != KSync::Comm::SocketConnectAcknowledge::Type) {
					LOGF(SEVERE, "Message received wasn't an acknowledgement!\n");
					return;
				}
			}

			//Set up gateway socket
			std::shared_ptr<KSync::Comm::CommSystemSocket> gateway_socket;
			if (comm_system->Create_Gateway_Rep_Socket(gateway_socket) < 0) {
				LOGF(SEVERE, "There was a problem creating the gateway socket!\n");
				return;
			}

			if(gateway_socket->SetRecvTimeout(1000) < 0) {
				LOGF(SEVERE, "There was a problem setting the recv timeout!\n");
				return;
			}

			if (gateway_socket->Bind(gateway_socket_url) < 0) {
				LOGF(SEVERE, "There was a problem binding the gateway socket!\n");
				return;
			}

			bool finished = false;

			int status = 0;
			//Start gateway loop!
			while(!finished) {
				//Listen for new connections 
				std::shared_ptr<KSync::Comm::CommObject> recv_obj;
				status = gateway_socket->Recv(recv_obj);
				if(status == KSync::Comm::CommSystemSocket::Other) {
					LOGF(SEVERE, "There was a problem receiving connection requests!\n");
					return;
				} else if (status == KSync::Comm::CommSystemSocket::Timeout) {
				} else if (status == KSync::Comm::CommSystemSocket::EmptyMessage) {
				} else {
					if(recv_obj->GetType() == KSync::Comm::GatewaySocketInitializationRequest::Type) {
						LOGF(INFO, "Received init request, Passing on..\n");
						status = gateway_thread_socket->Send(recv_obj);
						if(status == KSync::Comm::CommSystemSocket::Other) {
							LOGF(SEVERE, "There was a problem passing on the init request!\n");
							return;
						} else if (status == KSync::Comm::CommSystemSocket::Timeout) {
							LOGF(SEVERE, "Sending timed out!!!\n");
							return;
						} else {
							std::shared_ptr<KSync::Comm::CommObject> resp_obj;
							status = gateway_thread_socket->ForceRecv(resp_obj);
							if (status == KSync::Comm::CommSystemSocket::Other) {
								LOGF(SEVERE, "There was a problem getting the response!!\n");
								return;
							} else {
								status = gateway_socket->Send(resp_obj);
								if (status == KSync::Comm::CommSystemSocket::Other) {
									LOGF(SEVERE, "There was a problem passing on the response!\n");
									return;
								} else if (status == KSync::Comm::CommSystemSocket::Timeout) {
									LOGF(SEVERE, "There was a timeout when sending on the response!\n");
									return;
								}
							}
						}
					} else if(recv_obj->GetType() == KSync::Comm::CommString::Type) {
						std::shared_ptr<KSync::Comm::CommString> message;
						KSync::Comm::CommCreator(message, recv_obj);
						LOGF(INFO, "Received (%s)\n", message->c_str());
						std::shared_ptr<KSync::Comm::CommObject> send_obj = message->GetCommObject();
						status = gateway_socket->Send(send_obj); 
						if(status == KSync::Comm::CommSystemSocket::Other) {
							LOGF(SEVERE, "There was a problem sending a message!!\n");
							return;
						} else if (status == KSync::Comm::CommSystemSocket::Timeout) {
							LOGF(SEVERE, "Sending message timedout!\n");
							return;
						}
					} else {
						LOGF(WARNING, "Message unsupported! (%i) (%s)\n", recv_obj->GetType(), KSync::Comm::GetTypeName(recv_obj->GetType()));
					}
				}

				// Check for messages from the master!
				std::shared_ptr<KSync::Comm::CommObject> master_obj;
				status = gateway_thread_socket->Recv(master_obj);
				if(status == KSync::Comm::CommSystemSocket::Other) {
					LOGF(WARNING, "There was a problem checking the master thread!\n");
				} else if ((status == KSync::Comm::CommSystemSocket::Timeout)||(status == KSync::Comm::CommSystemSocket::EmptyMessage)) {
				} else {
					if (master_obj->GetType() == KSync::Comm::ServerShuttingDown::Type) {
						finished = true;
					}
				}
			}
		}
	}
}
