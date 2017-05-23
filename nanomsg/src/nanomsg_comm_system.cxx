#include "ksync/logging.h"
#include "ksync_nanomsg/nanomsg_comm_system.h"

#include "nanomsg/nn.h"
#include "nanomsg/reqrep.h"
#include "nanomsg/pair.h"
#include "nanomsg/pubsub.h"

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

		int NanomsgCommSystemSocket::Send(const std::shared_ptr<CommObject> comm_obj) {
			if (this->socket < 0) {
				Error("Can't send to a socket which isn't ready!\n");
				return Other;
			}
			int sent_bytes = nn_send(this->socket, (const void*) comm_obj->GetDataPointer(), (int) comm_obj->GetDataSize(), 0);
			if(sent_bytes != (int) comm_obj->GetDataSize()) {
				if(sent_bytes == -1) {
					int err = nn_errno();
					if(err == ETIMEDOUT) {
						return Timeout;
					} else {
						Error("Problem sending data!! %i (%s)\n", err, nn_strerror(err));
						return Other;
					}
				} else {
					Error("Wrong number of bytes sent!\n");
					return Other;
				}
			}
			return Success;
		}

		int NanomsgCommSystemSocket::Recv(std::shared_ptr<CommObject>& comm_obj) {
			if (comm_obj) {
				Error("Please pass a null pointer\n");
				return Other;
			}
			if (this->socket < 0) {
				Error("Can't receive from a socket which isn't ready!\n");
				return Other;
			}
			char* buf = 0;
			int bytes = nn_recv(this->socket, &buf, NN_MSG, 0);
			if(bytes < 0) {
				if(buf != 0) {
					if(nn_freemsg(buf) != 0) {
						Error("Problem freeing message!\n");
						return Other;
					}
				}
				int err = nn_errno();
				if(err == ETIMEDOUT) {
					return Timeout;
				} else {
					Error("Problem receiving data!! %i (%s)\n", err, nn_strerror(err));
					return Other;
				}
			}
			if(bytes == 0) {
				return EmptyMessage;
			}
			comm_obj.reset(new CommObject(buf, bytes, true));
			if(nn_freemsg(buf) != 0) {
				Error("Problem freeing message!\n");
				return Other;
			}
			return Success;
		}

		int NanomsgCommSystemSocket::SetSendTimeout(int timeout) {
			if(nn_setsockopt(this->socket, NN_SOL_SOCKET, NN_SNDTIMEO, &timeout, sizeof(timeout)) != 0) {
				Error("Failed to set the send timeout socket option!\n");
				return -1;
			}
			return 0;
		}

		int NanomsgCommSystemSocket::SetRecvTimeout(int timeout) {
			if(nn_setsockopt(this->socket, NN_SOL_SOCKET, NN_RCVTIMEO, &timeout, sizeof(timeout)) != 0) {
				Error("Failed to set the receive timeout socket option!\n");
				return -1;
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
				nanomsg_socket->socket = nn_socket(AF_SP, NN_REQ);
				if (nanomsg_socket->socket < 0) {
					delete nanomsg_socket;
					Error("There was a problem creating the NN_REQ socket!\n");
					return -1;
				}
				socket = (CommSystemSocket*) nanomsg_socket;
				return 0;
			} else {
				return -1;
			}
		}

		int NanomsgCommSystem::Create_Gateway_Rep_Socket(CommSystemSocket*& socket __attribute__((unused))) {
			if (socket == 0) {
				NanomsgCommSystemSocket* nanomsg_socket = new NanomsgCommSystemSocket();
				nanomsg_socket->socket = nn_socket(AF_SP, NN_REP);
				if (nanomsg_socket->socket < 0) {
					Error("There was a problem creating the NN_REP socket!\n");
					return -1;
				}
				socket = (CommSystemSocket*) nanomsg_socket;
				return 0;
			} else {
				return -1;
			}
		}

		int NanomsgCommSystem::Create_Pair_Socket(CommSystemSocket*& socket __attribute__((unused))) {
			if (socket == 0) {
				NanomsgCommSystemSocket* nanomsg_socket = new NanomsgCommSystemSocket();
				nanomsg_socket->socket = nn_socket(AF_SP, NN_PAIR);
				if (nanomsg_socket->socket < 0) {
					Error("There was a problem creating the NN_PAIR socket!\n");
					return -1;
				}
				socket = (CommSystemSocket*) nanomsg_socket;
				return 0;
			} else {
				return -1;
			}
		}

		int NanomsgCommSystem::Create_Pub_Socket(CommSystemSocket*& socket) {
			if (socket == 0) {
				NanomsgCommSystemSocket* nanomsg_socket = new NanomsgCommSystemSocket();
				nanomsg_socket->socket = nn_socket(AF_SP, NN_PUB);
				if (nanomsg_socket->socket < 0) {
					Error("There was a problem creating the NN_PUB socket!\n");
					return -1;
				}
				socket = (CommSystemSocket*) nanomsg_socket;
				return 0;
			} else {
				return -1;
			}
		}

		int NanomsgCommSystem::Create_Sub_Socket(CommSystemSocket*& socket) {
			if (socket == 0) {
				NanomsgCommSystemSocket* nanomsg_socket = new NanomsgCommSystemSocket();
				nanomsg_socket->socket = nn_socket(AF_SP, NN_SUB);
				if(nanomsg_socket->socket < 0) {
					Error("There was a problem creating the NN_SUB socket!\n");
					return -1;
				}
				socket = (CommSystemSocket*) nanomsg_socket;
				return 0;
			} else {
				return -1;
			}
		}

		int GetNanomsgCommSystem(CommSystemInterface*& comm_interface __attribute__((unused))) {
			Message("Starting Nanomsg Communication Backend\n");
			comm_interface = new NanomsgCommSystem();
			return 0;
		}
	}
}
