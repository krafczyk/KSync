#include "ksync/comm_system_interface.h"

namespace KSync {
	namespace Comm {
		CommSystemInterface::CommSystemInterface() {
		}

		CommSystemInterface::~CommSystemInterface() {
		}

		CommSystemSocket::CommSystemSocket() {
		}

		CommSystemSocket::~CommSystemSocket() {
		}

		int CommSystemSocket::ForceRecv(std::shared_ptr<CommObject>& comm_obj) {
			int status;
			while(true) {
				status = this->Recv(comm_obj);
				if ((status == Other)||(status == Success)) {
					return status;
				}
			}
		}
	}
}
