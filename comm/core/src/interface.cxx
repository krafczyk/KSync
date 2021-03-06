#include "ksync/logging.h"
#include "ksync/comm/interface.h"

namespace KSync {
	namespace Comm {
		CommSystemInterface::CommSystemInterface() {
		}

		CommSystemInterface::~CommSystemInterface() {
		}

		CommSystemSocket::CommSystemSocket() {
		}

		CommSystemSocket::~CommSystemSocket() {
			if(this->bind) {
				if(this->url.substr(0,6) == "ipc://") {
					if(remove(this->url.substr(6).c_str()) < 0) {
						LOGF(WARNING, "There was a problem deleting the IPC file");
					}
				}
			}
		}

		int CommSystemSocket::Bind(const std::string& address) {
			this->bind = true;
			this->url = address;
			return this->BindImp(address);
		}

		int CommSystemSocket::Connect(const std::string& address) {
			this->bind = false;
			this->url = address;
			return this->ConnectImp(address);
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
