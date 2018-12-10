#include "ksync/logging.h"
#include "ksync/utilities.h"
#include "ksync/common_ops.h"
#include "ksync/comm/interface.h"
#include "ksync/comm/factory.h"

namespace KSync {
	namespace Utilities {
		int GetGatewaySocketURL(std::string& gateway_socket_url, const bool gateway_socket_url_defined) {
			if (!gateway_socket_url_defined) {
				if(KSync::Utilities::get_default_ipc_connection_url(gateway_socket_url) < 0) {
					LOGF(SEVERE, "There was a problem getitng the default IPC connection URL.");
					return -2;
				}
			} else {
				if(gateway_socket_url.substr(0, 3) != "icp") {
					LOGF(SEVERE, "Non icp sockets are not properly implemented at this time.");
					return -2;
				}
			}
			return 0;
		}
		int GetCommSystem(std::shared_ptr<KSync::Comm::CommSystemInterface> comm_system, const bool nanomsg) {
			if (!nanomsg) {
				if (KSync::Comm::GetZeromqCommSystem(comm_system) < 0) {
					LOGF(SEVERE, "There was a problem initializing the ZeroMQ communication system!");
					return -1;
				}
			} else {
				if (KSync::Comm::GetNanomsgCommSystem(comm_system) < 0) {
					LOGF(SEVERE, "There was a problem initializing the Nanomsg communication system!");
					return -1;
				}
			}
			return 0;
		}
	}
}
