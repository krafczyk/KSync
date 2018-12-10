#ifndef KSYNC_ZEROMQ_COMM_SYSTEM_HDR
#define KSYNC_ZEROMQ_COMM_SYSTEM_HDR

#include "ksync/comm/interface.h"
#include "ksync/comm/factory.h"

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
				int SetSendTimeout(int timeout = -1);
				int SetRecvTimeout(int timeout = -1);

			private:
				zmq::socket_t* socket;
		};

		class ZeroMQCommSystem : public CommSystemInterface {
			public:
				ZeroMQCommSystem();
				~ZeroMQCommSystem();

				int Create_Gateway_Req_Socket(std::shared_ptr<CommSystemSocket>& socket, int recv_timeout = -1, int send_timeout = -1);
				int Create_Gateway_Rep_Socket(std::shared_ptr<CommSystemSocket>& socket, int recv_timeout = -1, int send_timeout = -1);
				int Create_Pair_Socket(std::shared_ptr<CommSystemSocket>& socket, int recv_timeout = -1, int send_timeout = -1);
				int Create_Pub_Socket(std::shared_ptr<CommSystemSocket>& socket, int recv_timeout = -1, int send_timeout = -1);
				int Create_Sub_Socket(std::shared_ptr<CommSystemSocket>& socket, int recv_timeout = -1, int send_timeout = -1);
				int Create_Pull_Socket(std::shared_ptr<CommSystemSocket>& socket, int recv_timeout = -1, int send_timeout = -1);
				int Create_Push_Socket(std::shared_ptr<CommSystemSocket>& socket, int recv_timeout = -1, int send_timeout = -1);
				
			private:
				zmq::context_t* context;
		};
	}
}

#endif
