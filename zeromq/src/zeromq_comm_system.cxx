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

		int ZeroMQCommSystemSocket::Send(const void* data, const size_t size) {
			if (socket == 0) {
				return -1;
			}
			zmq::message_t send(size);
			memcpy(send.data(), data, size);
			socket->send(send);
			return 0;
		}

		int ZeroMQCommSystemSocket::Recv(void*& data, size_t& size) {
			if (socket == 0) {
				return -1;
			}
			zmq::message_t recv;
			socket->recv(&recv);

			size = recv.size();
			data = new void[size];
			memcpy(data, recv.data(), size);
			return 0;
		}

		ZeroMQCommSystemEndpoint::ZeroMQCommSystemEndpoint() {
		}

		ZeroMQCommSystemEndpoint::~ZeroMQCommSystemEndpoint() {
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

		int GetZeromqCommSystem(CommSystemInterface*& comm_interface __attribute__((unused))) {
			comm_interface = new ZeroMQCommSystem();
			return 0;
		}
	}
}
