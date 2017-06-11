#include "ksync/logging.h"
#include "ksync/client/client_utilities.h"

namespace KSync {
	namespace Client {
		int ConnectToServer(std::shared_ptr<KSync::Comm::CommSystemInterface>& comm_system, const std::string gateway_socket_url, std::shared_ptr<KSync::Comm::CommSystemSocket>& client_push_socket, std::shared_ptr<KSync::Comm::CommSystemSocket>& client_pull_socket) {
	//Connect to gateway socket
	std::shared_ptr<KSync::Comm::CommSystemSocket> gateway_socket;
	if (comm_system->Create_Gateway_Req_Socket(gateway_socket) < 0) {
		LOGF(SEVERE, "There was a problem creating the gateway socket!");
		return -1;
	}

	if(gateway_socket->SetRecvTimeout(20000) < 0) {
		LOGF(SEVERE, "There was a problem setting the receive timeout!");
		return -2;
	}

	if (gateway_socket->Connect(gateway_socket_url) < 0) {
		LOGF(SEVERE, "There was a problem connecting to the gateway socket!");
		return -3;
	}

	client_push_socket.reset();
	client_pull_socket.reset();
	int status = 0;
	//Request client socket connection
	while ((!client_push_socket)||(!client_pull_socket)) {
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

		}
	}
}
