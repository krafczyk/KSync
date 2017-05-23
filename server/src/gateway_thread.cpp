#include <unistd.h>

#include "ksync/logging.h"
#include "ksync/gateway_thread.h"

namespace KSync {
	namespace Server {
		void gateway_thread(std::shared_ptr<KSync::Comm::CommSystemInterface> comm_system, const std::string& gateway_thread_socket_url, const std::string& gateway_socket_url) {
			if(comm_system == nullptr) {
				Error("The Gateway thread was given a null comm_system!!\n");
				return;
			}

			//Setup our end of the gateway thread socket
			std::shared_ptr<KSync::Comm::CommSystemSocket> gateway_thread_socket;
			if(comm_system->Create_Pair_Socket(gateway_thread_socket) < 0) {
				KPrint("There was a problem creating the gateway thread pair socket!\n");
				return;
			}

			if(gateway_thread_socket->SetRecvTimeout(1000) < 0) {
				KPrint("There was a problem setting the recv timeout!\n");
				return;
			}

			if(gateway_thread_socket->Connect(gateway_thread_socket_url) < 0) {
				KPrint("There was a problem connectng to the gateway thread pair socket!\n");
				return;
			}

			//Herald our existence
			KSync::Comm::SocketConnectHerald herald;
			std::shared_ptr<KSync::Comm::CommObject> herald_obj = herald.GetCommObject();
			if(gateway_thread_socket->Send(herald_obj) < 0) {
				KPrint("There was a problem sending the herald message!\n");
				return;
			}

			//Check for acknowledgement
			std::shared_ptr<KSync::Comm::CommObject> ack_obj;
			if(gateway_thread_socket->Recv(ack_obj) < 0) {
				KPrint("Didn't receive the Acknowledge message!\n");
				return;
			} else {
				if(ack_obj->GetType() != KSync::Comm::SocketConnectAcknowledge::Type) {
					KPrint("Message received wasn't an acknowledgement!\n");
					return;
				}
			}

			//Set up gateway socket
			std::shared_ptr<KSync::Comm::CommSystemSocket> gateway_socket;
			if (comm_system->Create_Gateway_Rep_Socket(gateway_socket) < 0) {
				KPrint("There was a problem creating the gateway socket!\n");
				return;
			}

			if(gateway_socket->SetRecvTimeout(1000) < 0) {
				KPrint("There was a problem setting the recv timeout!\n");
				return;
			}

			if (gateway_socket->Bind(gateway_socket_url) < 0) {
				KPrint("There was a problem binding the gateway socket!\n");
				return;
			}

			bool finished = false;

			int status = 0;
			//Start gateway loop!
			while(!finished) {
				//Listen for 
				std::shared_ptr<KSync::Comm::CommObject> recv_obj;
				status = gateway_socket->Recv(recv_obj);
				if(status == KSync::Comm::CommSystemSocket::Other) {
					Error("There was a problem receiving connection requests!\n");
					return;
				} else if (status == KSync::Comm::CommSystemSocket::Timeout) {
				} else if (status == KSync::Comm::CommSystemSocket::EmptyMessage) {
				} else {
					if(recv_obj->GetType() == KSync::Comm::GatewaySocketInitializationRequest::Type) {
						KPrint("Received init request, Passing on..\n");
						status = gateway_thread_socket->Send(recv_obj);
						if(status == KSync::Comm::CommSystemSocket::Other) {
							Error("There was a problem passing on the init request!\n");
							return;
						} else if (status == KSync::Comm::CommSystemSocket::Timeout) {
							Error("Sending timed out!!!\n");
							return;
						} else {
							std::shared_ptr<KSync::Comm::CommObject> resp_obj;
							status = gateway_thread_socket->ForceRecv(resp_obj);
							if (status == KSync::Comm::CommSystemSocket::Other) {
								Error("There was a problem getting the response!!\n");
								return;
							} else {
								status = gateway_socket->Send(resp_obj);
								if (status == KSync::Comm::CommSystemSocket::Other) {
									Error("There was a problem passing on the response!\n");
									return;
								} else if (status == KSync::Comm::CommSystemSocket::Timeout) {
									Error("There was a timeout when sending on the response!\n");
									return;
								}
							}
						}
					} else if(recv_obj->GetType() == KSync::Comm::CommString::Type) {
						std::shared_ptr<KSync::Comm::CommString> message;
						KSync::Comm::CommCreator(message, recv_obj);
						KPrint("Received (%s)\n", message->c_str());
						std::shared_ptr<KSync::Comm::CommObject> send_obj = message->GetCommObject();
						status = gateway_socket->Send(send_obj); 
						if(status == KSync::Comm::CommSystemSocket::Other) {
							Error("There was a problem sending a message!!\n");
							return;
						} else if (status == KSync::Comm::CommSystemSocket::Timeout) {
							Warning("Sending message timedout!\n");
							return;
						}
					} else {
						Warning("Message unsupported! (%i) (%s)\n", recv_obj->GetType(), KSync::Comm::GetTypeName(recv_obj->GetType()));
					}
				}
			}
		}
	}
}
