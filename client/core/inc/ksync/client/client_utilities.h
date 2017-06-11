#ifndef KSYNC_CLIENT_UTILITIES_HDR
#define KSYNC_CLIENT_UTILITIES_HDR

#include "ksync/comm/interface.h"

namespace KSync {
	namespace Client {
		int ConnectToServer(std::shared_ptr<KSync::Comm::CommSystemInterface>& comm_system, const std::string gateway_socket_url, std::shared_ptr<KSync::Comm::CommSystemSocket>& client_push_socket, std::shared_ptr<KSync::Comm::CommSystemSocket>& client_pull_socket);
	}
}

#endif
