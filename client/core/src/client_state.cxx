#include "ksync/client/client_state.h"

namespace KSync {
	namespace Client {
		ClientState::ClientState() {
			this->finished.store(false);
		}

		bool ClientState::GetFinished() const {
			return this->finished.load();
		}

		void ClientState::SetFinished(bool in) {
			this->finished.store(in);
		}
	}
}
