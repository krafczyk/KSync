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

			//Setup our end of the gateway thread socket
			KSync::Comm::CommSystemSocket* gateway_thread_socket = 0;
			if(comm_system->Create_Pair_Socket(gateway_thread_socket) < 0) {
				KPrint("There was a problem creating the gateway thread pair socket!\n");
				return;
			}

			if(gateway_thread_socket->SetRecvTimeout(10000) < 0) {
				KPrint("There was a problem setting the recv timeout!\n");
				return;
			}

			if(gateway_thread_socket->Connect(gateway_thread_socket_url) < 0) {
				KPrint("There was a problem connectng to the gateway thread pair socket!\n");
				return;
			}

			Debug("1\n");
			//Herald our existence
			KSync::Comm::SocketConnectHerald herald;
			KSync::Comm::CommObject* herald_obj = herald.GetCommObject();
			if(gateway_thread_socket->Send(herald_obj) < 0) {
				KPrint("There was a problem sending the herald message!\n");
				return;
			}

			Debug("2\n");
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
			Debug("3\n");

			//Set up gateway socket
			KSync::Comm::CommSystemSocket* gateway_socket = 0;
			if (comm_system->Create_Gateway_Rep_Socket(gateway_socket) < 0) {
				KPrint("There was a problem creating the gateway socket!\n");
				return;
			}

			if(gateway_socket->SetRecvTimeout(10000) < 0) {
				KPrint("There was a problem setting the recv timeout!\n");
				return;
			}

			if (gateway_socket->Bind(gateway_socket_url) < 0) {
				KPrint("There was a problem binding the gateway socket!\n");
				return;
			}

			bool finished = false;

			Debug("4\n");
			int status = 0;
			//Start gateway loop!
			while(!finished) {
				Debug("5\n");
				//Listen for 
				KSync::Comm::CommObject* recv_obj = 0;
				status = gateway_socket->Recv(recv_obj);
				Debug("6\n");
				if(status == KSync::Comm::CommSystemSocket::Other) {
					Error("There was a problem receiving connection requests!\n");
					return;
				} else if (status == KSync::Comm::CommSystemSocket::Timeout) {
					Warning("Checking for connection request timed out!\n");
				} else if (status == KSync::Comm::CommSystemSocket::EmptyMessage) {
					Warning("Got an empty message?\n");
				} else {
					if(recv_obj->GetType() == KSync::Comm::GatewaySocketInitializationRequest::Type) {
						KSync::Comm::GatewaySocketInitializationChangeId message;
						KSync::Comm::CommObject* send_obj = message.GetCommObject();
						status = gateway_socket->Send(send_obj);
						if(status == KSync::Comm::CommSystemSocket::Other) {
							Error("There was a problem sending the response!!\n");
							return;
						} else if (status == KSync::Comm::CommSystemSocket::Timeout) {
							Error("Sending response timedout!\n");
							return;
						}
						delete send_obj;
					} else if(recv_obj->GetType() == KSync::Comm::CommString::Type) {
						KSync::Comm::CommString message(recv_obj);
						KPrint("Received (%s)\n", message.c_str());
						KSync::Comm::CommObject* send_obj = message.GetCommObject();
						status = gateway_socket->Send(send_obj); 
						if(status == KSync::Comm::CommSystemSocket::Other) {
							Error("There was a problem sending a message!!\n");
							return;
						} else if (status == KSync::Comm::CommSystemSocket::Timeout) {
							Warning("Sending message timedout!\n");
							return;
						}
						delete send_obj;
					} else {
						Debug("7\n");
						Warning("Message unsupported! (%i) (%s)\n", recv_obj->GetType(), KSync::Comm::GetTypeName(recv_obj->GetType()));
						Debug("8\n");
					}
					delete recv_obj;
				}
			}

			delete gateway_socket;
		}
	}
}
