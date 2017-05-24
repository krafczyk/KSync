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

				int BindImp(const std::string& address);
				int ConnectImp(const std::string& address);
				int Send(const std::shared_ptr<CommObject> comm_obj);
				int Recv(std::shared_ptr<CommObject>& comm_obj);
				int SetSendTimeout(int timeout);
				int SetRecvTimeout(int timeout);

			private:
				zmq::socket_t* socket;
		};

		class ZeroMQCommSystem : public CommSystemInterface {
			public:
				ZeroMQCommSystem();
				~ZeroMQCommSystem();

				int Create_Gateway_Req_Socket(std::shared_ptr<CommSystemSocket>& socket);
				int Create_Gateway_Rep_Socket(std::shared_ptr<CommSystemSocket>& socket);
				int Create_Pair_Socket(std::shared_ptr<CommSystemSocket>& socket);
				int Create_Pub_Socket(std::shared_ptr<CommSystemSocket>& socket);
				int Create_Sub_Socket(std::shared_ptr<CommSystemSocket>& socket);

			private:
				zmq::context_t* context;
		};
	}
}

#endif
