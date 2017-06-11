#include "ksync/client/client_state.h"

namespace KSync {
	namespace Client {
		ClientState::ClientState() {
			this->finished.store(false);
			this->comm_initialized.store(false);
			this->comm_nanomsg.store(false);
		}

		bool ClientState::GetFinished() const {
			return this->finished.load();
		}

		void ClientState::SetFinished(const bool in) {
			this->finished.store(in);
		}

		bool ClientState::GetCommInitialized() const {
			return this->comm_initialized.load();
		}

		void ClientState::SetCommInitialized(const bool in) {
			this->comm_initialized.store(in);
		}

		bool ClientState::GetCommNanomsg() const {
			return this->comm_nanomsg.load();
		}

		void ClientState::SetCommNanomsg(const bool in) {
			this->comm_nanomsg.store(in);
		}

		bool ClientState::GetConnectedToServer() const {
			return this->connected_to_server.load();
		}

		void ClientState::SetConnectedToServer(const bool in) {
			this->connected_to_server.store(in);
		}

		KSync::Utilities::client_id_t ClientState::GetClientId() const {
			return this->client_id.load();
		}

		void ClientState::SetClientId(const KSync::Utilities::client_id_t in) {
			this->client_id.store(in);
		}
	}
}
