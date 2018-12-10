#ifndef KSYNC_COMMON_OPS_HDR
#define KSYNC_COMMON_OPS_HDR

#include <string>
#include <memory>

#include "ksync/comm/interface.h"

namespace KSync {
	namespace Utilities {
		int GetGatewaySocketURL(std::string& gateway_socket_url, const bool gateway_socket_url_defined);
		int GetCommSystem(std::shared_ptr<KSync::Comm::CommSystemInterface> comm_system, const bool nanomsg);
	}
}

#endif
