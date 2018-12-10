#include "ksync/logging.h"
#include "ksync/comm/zeromq/zeromq_comm_system.h"

namespace KSync {
	namespace Comm {
		ZeroMQCommSystemSocket::ZeroMQCommSystemSocket() {
			socket = 0;
		}

		ZeroMQCommSystemSocket::~ZeroMQCommSystemSocket() {
			if (socket != 0) {
				delete socket;
			}
		}

		int ZeroMQCommSystemSocket::BindImp(const std::string& address) {
			if (socket == 0) {
				return -1;
			}
			socket->bind(address.c_str());
			return 0;
		}

		int ZeroMQCommSystemSocket::ConnectImp(const std::string& address) {
			if (socket == 0) {
				return -1;
			}
			socket->connect(address.c_str());
			return 0;
		}

		int ZeroMQCommSystemSocket::Send(const std::shared_ptr<CommObject> comm_obj) {
			if (socket == 0) {
				return Other;
			}
			const char* data = comm_obj->GetDataPointer();
			size_t size = comm_obj->GetDataSize();
			zmq::message_t send(size);
			memcpy(send.data(), data, size);
			int status = socket->send(send);
			if(status == -1) {
				int err = zmq_errno();
				if(err == EAGAIN) {
					LOGF(WARNING, "Send timed out!!");
					return Timeout;
				} else {
					LOGF(WARNING, "Problem sending data!! %i (%s)", err, zmq_strerror(err));
					return Other;
				}
			}
			return Success;
		}

		int ZeroMQCommSystemSocket::Recv(std::shared_ptr<CommObject>& comm_obj) {
			if (comm_obj) {
				printf("Please pass an empty pointer");
				return Other;
			}
			if (socket == 0) {
				return Other;
			}
			zmq::message_t recv;
			int status = socket->recv(&recv);
			if(status == -1) {
				int err = zmq_errno();
				if(err == EAGAIN) {
					LOGF(WARNING, "Recv timed out!!");
					return Timeout;
				} else {
					LOGF(WARNING, "Problem receiving data!! %i (%s)", err, zmq_strerror(err));
					return Other;
				}
			}
			if(recv.size() == 0) {
				return EmptyMessage;
			}
			comm_obj.reset(new CommObject((char*) recv.data(), recv.size(), true));
			return Success;
		}

		int ZeroMQCommSystemSocket::SetSendTimeout(int timeout) {
			socket->setsockopt(ZMQ_SNDTIMEO, &timeout, sizeof(timeout));
			return 0;
		}

		int ZeroMQCommSystemSocket::SetRecvTimeout(int timeout) {
			socket->setsockopt(ZMQ_RCVTIMEO, &timeout, sizeof(timeout));
			return 0;
		}

		ZeroMQCommSystem::ZeroMQCommSystem() {
			context = new zmq::context_t(1);
		}

		ZeroMQCommSystem::~ZeroMQCommSystem() {
			delete context;
		}

		int ZeroMQCommSystem::Create_Gateway_Req_Socket(std::shared_ptr<CommSystemSocket>& socket, int recv_timeout, int send_timeout) {
			if (!socket) {
				ZeroMQCommSystemSocket* zmq_socket = new ZeroMQCommSystemSocket();
				zmq_socket->socket = new zmq::socket_t(*this->context, ZMQ_REQ);
				socket.reset((CommSystemSocket*) zmq_socket);
				socket->SetSendTimeout(send_timeout);
				socket->SetRecvTimeout(recv_timeout);
				return 0;
			} else {
				return -1;
			}
		}

		int ZeroMQCommSystem::Create_Gateway_Rep_Socket(std::shared_ptr<CommSystemSocket>& socket, int recv_timeout, int send_timeout) {
			if (!socket) {
				ZeroMQCommSystemSocket* zmq_socket = new ZeroMQCommSystemSocket();
				zmq_socket->socket = new zmq::socket_t(*this->context, ZMQ_REP);
				socket.reset((CommSystemSocket*) zmq_socket);
				socket->SetSendTimeout(send_timeout);
				socket->SetRecvTimeout(recv_timeout);
				return 0;
			} else {
				return -1;
			}
		}

		int ZeroMQCommSystem::Create_Pair_Socket(std::shared_ptr<CommSystemSocket>& socket, int recv_timeout, int send_timeout) {
			if (!socket) {
				ZeroMQCommSystemSocket* zmq_socket = new ZeroMQCommSystemSocket();
				zmq_socket->socket = new zmq::socket_t(*this->context, ZMQ_PAIR);
				socket.reset((CommSystemSocket*) zmq_socket);
				socket->SetSendTimeout(send_timeout);
				socket->SetRecvTimeout(recv_timeout);
				return 0;
			} else {
				return -1;
			}
		}

		int ZeroMQCommSystem::Create_Pub_Socket(std::shared_ptr<CommSystemSocket>& socket, int recv_timeout, int send_timeout) {
			if (!socket) {
				ZeroMQCommSystemSocket* zmq_socket = new ZeroMQCommSystemSocket();
				zmq_socket->socket = new zmq::socket_t(*this->context, ZMQ_PUB);
				socket.reset((CommSystemSocket*) zmq_socket);
				socket->SetSendTimeout(send_timeout);
				socket->SetRecvTimeout(recv_timeout);
				return 0;
			} else {
				return -1;
			}
		}

		int ZeroMQCommSystem::Create_Sub_Socket(std::shared_ptr<CommSystemSocket>& socket, int recv_timeout, int send_timeout) {
			if (!socket) {
				ZeroMQCommSystemSocket* zmq_socket = new ZeroMQCommSystemSocket();
				zmq_socket->socket = new zmq::socket_t(*this->context, ZMQ_SUB);
				zmq_socket->socket->setsockopt(ZMQ_SUBSCRIBE, 0, 0);
				socket.reset((CommSystemSocket*) zmq_socket);
				socket->SetSendTimeout(send_timeout);
				socket->SetRecvTimeout(recv_timeout);
				return 0;
			} else {
				return -1;
			}
		}

		int ZeroMQCommSystem::Create_Pull_Socket(std::shared_ptr<CommSystemSocket>& socket, int recv_timeout, int send_timeout) {
			if (!socket) {
				ZeroMQCommSystemSocket* zmq_socket = new ZeroMQCommSystemSocket();
				zmq_socket->socket = new zmq::socket_t(*this->context, ZMQ_PULL);
				socket.reset((CommSystemSocket*) zmq_socket);
				socket->SetSendTimeout(send_timeout);
				socket->SetRecvTimeout(recv_timeout);
				return 0;
			} else {
				return -1;
			}
		}

		int ZeroMQCommSystem::Create_Push_Socket(std::shared_ptr<CommSystemSocket>& socket, int recv_timeout, int send_timeout) {
			if (!socket) {
				ZeroMQCommSystemSocket* zmq_socket = new ZeroMQCommSystemSocket();
				zmq_socket->socket = new zmq::socket_t(*this->context, ZMQ_PUSH);
				socket.reset((CommSystemSocket*) zmq_socket);
				socket->SetSendTimeout(send_timeout);
				socket->SetRecvTimeout(recv_timeout);
				return 0;
			} else {
				return -1;
			}
		}

		int GetZeromqCommSystem(std::shared_ptr<CommSystemInterface>& comm_interface __attribute__((unused))) {
			LOGF(MESSAGE, "Starting ZeroMQ Communication Backend");
			comm_interface.reset(new ZeroMQCommSystem());
			return 0;
		}
	}
}
