#ifndef KSYNC_ZEROMQ_COMM_SYSTEM_HDR
#define KSYNC_ZEROMQ_COMM_SYSTEM_HDR

#include "ksync/comm_system_interface.h"
#include "ksync/comm_system_factory.h"

#include "zmq.hpp"

namespace KSync {
	namespace Comm {
		class ZeroMQCommSystem;
		class ZeroMQCommSystemSocket : public CommSystemSocket {
			friend class ZeroMQCommSystem;
			public:
				ZeroMQCommSystemSocket();
				~ZeroMQCommSystemSocket();

				int Bind(const std::string& address);
				int Send(const std::string& message);
				int Recv(std::string& message);

			private:
				zmq::socket_t* socket;
		};

		class ZeroMQCommSystemEndpoint : public CommSystemEndpoint {
			public:
				ZeroMQCommSystemEndpoint();
				~ZeroMQCommSystemEndpoint();
		};

		class ZeroMQCommSystem : public CommSystemInterface {
			public:
				ZeroMQCommSystem();
				~ZeroMQCommSystem();

				int Create_Gateway_Req_Socket(CommSystemSocket*& socket);
				int Create_Gateway_Rep_Socket(CommSystemSocket*& socket);

			private:
				zmq::context_t* context;
		};
	}
}

#endif
