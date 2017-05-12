#include <unistd.h>

#include "ksync/logging.h"
#include "ksync/gateway_thread.h"

namespace KSync {
	namespace Server {
		void gateway_thread(KSync::Comm::CommSystemInterface* comm_system, const std::string& gateway_thread_socket_url, const std::string& gateway_socket_url) {
			if(comm_system == nullptr) {
				Error("The Gateway thread was given a null comm_system!!\n");
				return;
			}

			KSync::Comm::CommSystemSocket* gateway_thread_socket = 0;
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
			KSync::Comm::CommObject* herald_obj = herald.GetCommObject();
			if(gateway_thread_socket->Send(herald_obj) < 0) {
				KPrint("There was a problem sending the herald message!\n");
				return;
			}

			//Check for acknowledgement
			KSync::Comm::CommObject* ack_obj = 0;
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
			KSync::Comm::CommSystemSocket* gateway_socket = 0;
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

			//Start gateway loop!
			while(!finished) {
				//Listen for 
				KSync::Comm::CommObject* recv_obj = 0;
				if(gateway_socket->Recv(recv_obj) == 0) {
					if(recv_obj->GetType() == KSync::Comm::GatewaySocketInitializationRequest::Type) {
						KSync::Comm::GatewaySocketInitializationChangeId message;
						KSync::Comm::CommObject* send_obj = message.GetCommObject();
						if(gateway_socket->Send(send_obj) != 0) {
							Warning("There was a problem sending a message!!");
						}
						delete send_obj;
					} else if(recv_obj->GetType() == KSync::Comm::CommString::Type) {
						KSync::Comm::CommString message(recv_obj);
						KPrint("Received (%s)\n", message.c_str());
						if (message == "quit") {
							KPrint("Detected 'quit'. Quitting.\n");
							finished = true;
						} else {
							//usleep(1*1000000);
							usleep(100000);
							KSync::Comm::CommObject* send_obj = message.GetCommObject();
							if(gateway_socket->Send(send_obj) != 0) {
								Warning("There was a problem sending a message!!");
							} 
							delete send_obj;
						}
					} else {
						Warning("Message unsupported!\n");
					}
					delete recv_obj;
				}
			}

			delete gateway_socket;
		}
	}
}
