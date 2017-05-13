#include "ksync/logging.h"
#include "ksync_zeromq/zeromq_comm_system.h"

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

		int ZeroMQCommSystemSocket::Bind(const std::string& address) {
			if (socket == 0) {
				return -1;
			}
			socket->bind(address.c_str());
			return 0;
		}

		int ZeroMQCommSystemSocket::Connect(const std::string& address) {
			if (socket == 0) {
				return -1;
			}
			socket->connect(address.c_str());
			return 0;
		}

		int ZeroMQCommSystemSocket::Send(const CommObject* comm_obj) {
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
					Warning("Send timed out!!\n");
					return Timeout;
				} else {
					Error("Problem sending data!! %i (%s)\n", err, zmq_strerror(err));
					return Other;
				}
			}
			return Success;
		}

		int ZeroMQCommSystemSocket::Recv(CommObject*& comm_obj) {
			if (comm_obj != 0) {
				printf("Please pass an empty pointer\n");
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
					Warning("Recv timed out!!\n");
					return Timeout;
				} else {
					Error("Problem sending data!! %i (%s)\n", err, zmq_strerror(err));
					return Other;
				}
			}
			if(recv.size() == 0) {
				Warning("Empty Message!\n");
				return EmptyMessage;
			}
			comm_obj = new CommObject((char*) recv.data(), recv.size(), true);
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

		int ZeroMQCommSystem::Create_Gateway_Req_Socket(CommSystemSocket*& socket __attribute__((unused))) {
			if (socket == 0) {
				ZeroMQCommSystemSocket* zmq_socket = new ZeroMQCommSystemSocket();
				zmq_socket->socket = new zmq::socket_t(*this->context, ZMQ_REQ);
				socket = (CommSystemSocket*) zmq_socket;
			}
			return 0;
		}

		int ZeroMQCommSystem::Create_Gateway_Rep_Socket(CommSystemSocket*& socket __attribute__((unused))) {
			if (socket == 0) {
				ZeroMQCommSystemSocket* zmq_socket = new ZeroMQCommSystemSocket();
				zmq_socket->socket = new zmq::socket_t(*this->context, ZMQ_REP);
				socket = (CommSystemSocket*) zmq_socket;
			}
			return 0;
		}

		int ZeroMQCommSystem::Create_Pair_Socket(CommSystemSocket*& socket __attribute__((unused))) {
			if (socket == 0) {
				ZeroMQCommSystemSocket* zmq_socket = new ZeroMQCommSystemSocket();
				zmq_socket->socket = new zmq::socket_t(*this->context, ZMQ_PAIR);
				socket = (CommSystemSocket*) zmq_socket;
			}
			return 0;
		}

		int GetZeromqCommSystem(CommSystemInterface*& comm_interface __attribute__((unused))) {
			Message("Starting ZeroMQ Communication Backend\n");
			comm_interface = new ZeroMQCommSystem();
			return 0;
		}
	}
}
