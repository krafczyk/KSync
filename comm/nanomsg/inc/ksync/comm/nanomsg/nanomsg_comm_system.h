#ifndef KSYNC_NANOMSG_COMM_SYSTEM_HDR
#define KSYNC_NANOMSG_COMM_SYSTEM_HDR

#include "ksync/comm/interface.h"
#include "ksync/comm/factory.h"

namespace KSync {
	namespace Comm {
		class NanomsgCommSystem;
		class NanomsgCommSystemSocket : public CommSystemSocket {
			friend class NanomsgCommSystem;
			public:
				NanomsgCommSystemSocket();
				~NanomsgCommSystemSocket();

				int BindImp(const std::string& address);
				int ConnectImp(const std::string& address);
				int Send(const std::shared_ptr<CommObject> comm_obj);
				int Recv(std::shared_ptr<CommObject>& comm_obj);
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

				int Create_Gateway_Req_Socket(std::shared_ptr<CommSystemSocket>& socket);
				int Create_Gateway_Rep_Socket(std::shared_ptr<CommSystemSocket>& socket);
				int Create_Pair_Socket(std::shared_ptr<CommSystemSocket>& socket);
				int Create_Pub_Socket(std::shared_ptr<CommSystemSocket>& socket);
				int Create_Sub_Socket(std::shared_ptr<CommSystemSocket>& socket);
		};
	}
}

#endif
