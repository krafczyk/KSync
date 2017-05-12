#ifndef KSYNC_NANOMSG_COMM_SYSTEM_HDR
#define KSYNC_NANOMSG_COMM_SYSTEM_HDR

#include "ksync/comm_system_interface.h"
#include "ksync/comm_system_factory.h"

namespace KSync {
	namespace Comm {
		class NanomsgCommSystem;
		class NanomsgCommSystemSocket : public CommSystemSocket {
			friend class NanomsgCommSystem;
			public:
				NanomsgCommSystemSocket();
				~NanomsgCommSystemSocket();

				int Bind(const std::string& address);
				int Connect(const std::string& address);
				int Send(const CommObject* comm_obj);
				int Recv(CommObject*& comm_obj);
				int SetSendTimeout(int timeout);
				int SetRecvTimeout(int timeout);

				int GetSocketId() const {
					return socket;
				}
			private:
				int socket;
		};

		class NanomsgCommSystem : public CommSystemInterface {
			public:
				NanomsgCommSystem();
				~NanomsgCommSystem();

				int Create_Gateway_Req_Socket(CommSystemSocket*& socket);
				int Create_Gateway_Rep_Socket(CommSystemSocket*& socket);
				int Create_Pair_Socket(CommSystemSocket*& socket);
		};
	}
}

#endif
