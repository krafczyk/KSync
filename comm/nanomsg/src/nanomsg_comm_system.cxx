#include "ksync/logging.h"
#include "ksync/comm/nanomsg/nanomsg_comm_system.h"

#include "nanomsg/nn.h"
#include "nanomsg/reqrep.h"
#include "nanomsg/pair.h"
#include "nanomsg/pubsub.h"
#include "nanomsg/pipeline.h"

namespace KSync {
	namespace Comm {
		NanomsgCommSystemSocket::NanomsgCommSystemSocket() {
			this->socket = 0;
		}

		NanomsgCommSystemSocket::~NanomsgCommSystemSocket() {
			nn_shutdown(this->socket, 0);
		}

		int NanomsgCommSystemSocket::BindImp(const std::string& address) {
			if(nn_bind(this->socket, address.c_str()) < 0) {
				LOGF(SEVERE, "There was a problem binding to the address (%s)", address.c_str());
				return -1;
			}
			return 0;
		}

		int NanomsgCommSystemSocket::ConnectImp(const std::string& address) {
			if(nn_connect(this->socket, address.c_str()) < 0) {
				LOGF(SEVERE, "There was a problem connecting to the address (%s)", address.c_str());
				return -1;
			}
			return 0;
		}

		int NanomsgCommSystemSocket::Send(const std::shared_ptr<CommObject> comm_obj) {
			if (this->socket < 0) {
				LOGF(WARNING, "Can't send to a socket which isn't ready!");
				return Other;
			}
			int sent_bytes = nn_send(this->socket, (const void*) comm_obj->GetDataPointer(), (int) comm_obj->GetDataSize(), 0);
			if(sent_bytes != (int) comm_obj->GetDataSize()) {
				if(sent_bytes == -1) {
					int err = nn_errno();
					if(err == ETIMEDOUT) {
						return Timeout;
					} else {
						LOGF(WARNING, "Problem sending data!! %i (%s)", err, nn_strerror(err));
						return Other;
					}
				} else {
					LOGF(WARNING, "Wrong number of bytes sent!");
					return Other;
				}
			}
			return Success;
		}

		int NanomsgCommSystemSocket::Recv(std::shared_ptr<CommObject>& comm_obj) {
			if (comm_obj) {
				LOGF(WARNING, "Please pass a null pointer");
				return Other;
			}
			if (this->socket < 0) {
				LOGF(WARNING, "Can't receive from a socket which isn't ready!");
				return Other;
			}
			char* buf = 0;
			int bytes = nn_recv(this->socket, &buf, NN_MSG, 0);
			if(bytes < 0) {
				if(buf != 0) {
					if(nn_freemsg(buf) != 0) {
						LOGF(WARNING, "Problem freeing message!");
						return Other;
					}
				}
				int err = nn_errno();
				if(err == ETIMEDOUT) {
					return Timeout;
				} else {
					LOGF(WARNING, "Problem receiving data!! %i (%s)", err, nn_strerror(err));
					return Other;
				}
			}
			if(bytes == 0) {
				return EmptyMessage;
			}
			comm_obj.reset(new CommObject(buf, bytes, true));
			if(nn_freemsg(buf) != 0) {
				LOGF(WARNING, "Problem freeing message!");
				return Other;
			}
			return Success;
		}

		int NanomsgCommSystemSocket::SetSendTimeout(int timeout) {
			if(nn_setsockopt(this->socket, NN_SOL_SOCKET, NN_SNDTIMEO, &timeout, sizeof(timeout)) != 0) {
				LOGF(WARNING, "Failed to set the send timeout socket option!");
				return -1;
			}
			return 0;
		}

		int NanomsgCommSystemSocket::SetRecvTimeout(int timeout) {
			if(nn_setsockopt(this->socket, NN_SOL_SOCKET, NN_RCVTIMEO, &timeout, sizeof(timeout)) != 0) {
				LOGF(WARNING, "Failed to set the receive timeout socket option!");
				return -1;
			}
			return 0;
		}

		NanomsgCommSystem::NanomsgCommSystem() {
		}

		NanomsgCommSystem::~NanomsgCommSystem() {
		}

		int NanomsgCommSystem::Create_Gateway_Req_Socket(std::shared_ptr<CommSystemSocket>& socket, int recv_timeout, int send_timeout) {
			if (!socket) {
				NanomsgCommSystemSocket* nanomsg_socket = new NanomsgCommSystemSocket();
				nanomsg_socket->socket = nn_socket(AF_SP, NN_REQ);
				if (nanomsg_socket->socket < 0) {
					delete nanomsg_socket;
					LOGF(SEVERE, "There was a problem creating the NN_REQ socket!");
					return -1;
				}
				socket.reset((CommSystemSocket*) nanomsg_socket);
				socket->SetSendTimeout(send_timeout);
				socket->SetRecvTimeout(recv_timeout);
				return 0;
			} else {
				return -1;
			}
		}

		int NanomsgCommSystem::Create_Gateway_Rep_Socket(std::shared_ptr<CommSystemSocket>& socket, int recv_timeout, int send_timeout) {
			if (!socket) {
				NanomsgCommSystemSocket* nanomsg_socket = new NanomsgCommSystemSocket();
				nanomsg_socket->socket = nn_socket(AF_SP, NN_REP);
				if (nanomsg_socket->socket < 0) {
					LOGF(SEVERE, "There was a problem creating the NN_REP socket!");
					return -1;
				}
				socket.reset((CommSystemSocket*) nanomsg_socket);
				socket->SetSendTimeout(send_timeout);
				socket->SetRecvTimeout(recv_timeout);
				return 0;
			} else {
				return -1;
			}
		}

		int NanomsgCommSystem::Create_Pair_Socket(std::shared_ptr<CommSystemSocket>& socket, int recv_timeout, int send_timeout) {
			if (!socket) {
				NanomsgCommSystemSocket* nanomsg_socket = new NanomsgCommSystemSocket();
				nanomsg_socket->socket = nn_socket(AF_SP, NN_PAIR);
				if (nanomsg_socket->socket < 0) {
					LOGF(SEVERE, "There was a problem creating the NN_PAIR socket!");
					return -1;
				}
				socket.reset((CommSystemSocket*) nanomsg_socket);
				socket->SetSendTimeout(send_timeout);
				socket->SetRecvTimeout(recv_timeout);
				return 0;
			} else {
				return -1;
			}
		}

		int NanomsgCommSystem::Create_Pub_Socket(std::shared_ptr<CommSystemSocket>& socket, int recv_timeout, int send_timeout) {
			if (!socket) {
				NanomsgCommSystemSocket* nanomsg_socket = new NanomsgCommSystemSocket();
				nanomsg_socket->socket = nn_socket(AF_SP, NN_PUB);
				if (nanomsg_socket->socket < 0) {
					LOGF(SEVERE, "There was a problem creating the NN_PUB socket!");
					return -1;
				}
				socket.reset((CommSystemSocket*) nanomsg_socket);
				socket->SetSendTimeout(send_timeout);
				socket->SetRecvTimeout(recv_timeout);
				return 0;
			} else {
				return -1;
			}
		}

		int NanomsgCommSystem::Create_Sub_Socket(std::shared_ptr<CommSystemSocket>& socket, int recv_timeout, int send_timeout) {
			if (!socket) {
				NanomsgCommSystemSocket* nanomsg_socket = new NanomsgCommSystemSocket();
				nanomsg_socket->socket = nn_socket(AF_SP, NN_SUB);
				if(nanomsg_socket->socket < 0) {
					LOGF(SEVERE, "There was a problem creating the NN_SUB socket!");
					return -1;
				}
				if(nn_setsockopt(nanomsg_socket->socket, NN_SUB, NN_SUB_SUBSCRIBE, 0, 0) < 0) {
					LOGF(SEVERE, "There was a problem subscribing the subscriber socket!!");
					return -2;
				}
				socket.reset((CommSystemSocket*) nanomsg_socket);
				socket->SetSendTimeout(send_timeout);
				socket->SetRecvTimeout(recv_timeout);
				return 0;
			} else {
				return -3;
			}
		}

		int NanomsgCommSystem::Create_Pull_Socket(std::shared_ptr<CommSystemSocket>& socket, int recv_timeout, int send_timeout) {
			if (!socket) {
				NanomsgCommSystemSocket* nanomsg_socket = new NanomsgCommSystemSocket();
				nanomsg_socket->socket = nn_socket(AF_SP, NN_PULL);
				if(nanomsg_socket->socket < 0) {
					LOGF(SEVERE, "There was a problem creating the NN_PULL socket!");
					return -1;
				}
				socket.reset((CommSystemSocket*) nanomsg_socket);
				socket->SetSendTimeout(send_timeout);
				socket->SetRecvTimeout(recv_timeout);
				return 0;
			} else {
				return -3;
			}
		}

		int NanomsgCommSystem::Create_Push_Socket(std::shared_ptr<CommSystemSocket>& socket, int recv_timeout, int send_timeout) {
			if (!socket) {
				NanomsgCommSystemSocket* nanomsg_socket = new NanomsgCommSystemSocket();
				nanomsg_socket->socket = nn_socket(AF_SP, NN_PUSH);
				if(nanomsg_socket->socket < 0) {
					LOGF(SEVERE, "There was a problem creating the NN_PUSH socket!");
					return -1;
				}
				socket.reset((CommSystemSocket*) nanomsg_socket);
				socket->SetSendTimeout(send_timeout);
				socket->SetRecvTimeout(recv_timeout);
				return 0;
			} else {
				return -3;
			}
		}

		int GetNanomsgCommSystem(std::shared_ptr<CommSystemInterface>& comm_interface __attribute__((unused))) {
			LOGF(MESSAGE, "Starting Nanomsg Communication Backend");
			comm_interface.reset(new NanomsgCommSystem());
			return 0;
		}
	}
}
