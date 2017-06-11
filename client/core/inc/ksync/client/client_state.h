#ifndef KSYNC_CLIENT_STATE_HDR
#define KSYNC_CLIENT_STATE_HDR

#include <atomic>

#include "ksync/utilities.h"

namespace KSync {
	namespace Client {
		class ClientState {
			public:
				ClientState();
				bool GetFinished() const;
				void SetFinished(const bool in);
				bool GetCommInitialized() const;
				void SetCommInitialized(const bool in);
				bool GetCommNanomsg() const;
				void SetCommNanomsg(const bool in);
				bool GetConnectedToServer() const;
				void SetConnectedToServer(const bool in);
				KSync::Utilities::client_id_t GetClientId() const;
				void SetClientId(const KSync::Utilities::client_id_t in);
			private:
				std::atomic<bool> finished;
				std::atomic<bool> comm_initialized;
				std::atomic<bool> comm_nanomsg;
				std::atomic<bool> connected_to_server;
				std::atomic<KSync::Utilities::client_id_t> client_id;
		};
	}
}

#endif
