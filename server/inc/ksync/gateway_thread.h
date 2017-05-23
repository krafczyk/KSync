#ifndef KSYNC_SERVER_GATEWAY_THREAD_HDR
#define KSYNC_SERVER_GATEWAY_THREAD_HDR

#include "ksync/comm_system_interface.h"

namespace KSync {
	namespace Server {
		void gateway_thread(std::shared_ptr<KSync::Comm::CommSystemInterface> comm_system, const std::string& gateway_thread_socket_url, const std::string& gateway_socket_url);
	}
}

#endif
