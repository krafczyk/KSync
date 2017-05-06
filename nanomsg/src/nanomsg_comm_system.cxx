#include "ksync/logging.h"
#include "ksync_nanomsg/nanomsg_comm_system.h"

#include "nanomsg/nn.h"
#include "nanomsg/reqrep.h"

namespace KSync {
	namespace Comm {
		NanomsgCommSystemSocket::NanomsgCommSystemSocket() {
			this->socket = 0;
		}

		NanomsgCommSystemSocket::~NanomsgCommSystemSocket() {
			nn_shutdown(this->socket, 0);
		}

		int NanomsgCommSystemSocket::Bind(const std::string& address) {
			if(nn_bind(this->socket, address.c_str()) < 0) {
				Error("There was a problem binding to the address (%s)\n", address.c_str());
				return -1;
			}
			return 0;
		}

		int NanomsgCommSystemSocket::Connect(const std::string& address) {
			if(nn_connect(this->socket, address.c_str()) < 0) {
				Error("There was a problem connecting to the address (%s)\n", address.c_str());
				return -1;
			}
			return 0;
		}

		int NanomsgCommSystemSocket::Send(const CommObject* comm_obj) {
			if (this->socket < 0) {
				Error("Can't send to a socket which isn't ready!\n");
				return -1;
			}
			if(nn_send(this->socket, (const void*) comm_obj->GetDataPointer(), (int) comm_obj->GetDataSize(), 0) != (int) comm_obj->GetDataSize()) {
				Error("There was a problem sending the message. Sent bytes did not match message size.\n");
				return -2;
			}
			return 0;
		}

		int NanomsgCommSystemSocket::Recv(CommObject*& comm_obj) {
			if (comm_obj != 0) {
				Error("Please pass a null pointer\n");
				return -2;
			}
			if (this->socket < 0) {
				Error("Can't receive from a socket which isn't ready!\n");
				return -1;
			}
			char* buf = 0;
			int bytes = nn_recv(this->socket, &buf, NN_MSG, 0);
			if(bytes < 0) {
				if(buf != 0) {
					if(nn_freemsg(buf) != 0) {
						Error("There was a problem freeing the message!\n");
						return -1;
					}
				}
				Error("There was a problem receiving the message!\n");
				return -2;
			}
			comm_obj = new CommObject(buf, bytes, true);
			if(nn_freemsg(buf) != 0) {
				Error("There was a problem freeing the message!\n");
				return -3;
			}
			return 0;
		}

		NanomsgCommSystem::NanomsgCommSystem() {
		}

		NanomsgCommSystem::~NanomsgCommSystem() {
		}

		int NanomsgCommSystem::Create_Gateway_Req_Socket(CommSystemSocket*& socket __attribute__((unused))) {
			if (socket == 0) {
				NanomsgCommSystemSocket* nanomsg_socket = new NanomsgCommSystemSocket();
				nanomsg_socket->socket = nn_socket (AF_SP, NN_REQ);
				if (nanomsg_socket->socket < 0) {
					delete nanomsg_socket;
					Error("There was a problem creating the NN_REQ socket!\n");
					return -1;
				}
				socket = (CommSystemSocket*) nanomsg_socket;
			}
			return 0;
		}

		int NanomsgCommSystem::Create_Gateway_Rep_Socket(CommSystemSocket*& socket __attribute__((unused))) {
			if (socket == 0) {
				NanomsgCommSystemSocket* nanomsg_socket = new NanomsgCommSystemSocket();
				nanomsg_socket->socket = nn_socket (AF_SP, NN_REP);
				if (nanomsg_socket->socket < 0) {
					Error("There was a problem creating the NN_REP socket!\n");
					return -1;
				}
				socket = (CommSystemSocket*) nanomsg_socket;
			}
			return 0;
		}

		int GetNanomsgCommSystem(CommSystemInterface*& comm_interface __attribute__((unused))) {
			comm_interface = new NanomsgCommSystem();
			return 0;
		}
	}
}