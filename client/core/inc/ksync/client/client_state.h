#ifndef KSYNC_CLIENT_STATE_HDR
#define KSYNC_CLIENT_STATE_HDR

#include <atomic>

namespace KSync {
	namespace Client {
		class ClientState {
			public:
				ClientState();
				bool GetFinished() const;
				void SetFinished(bool in);
			private:
				std::atomic<bool> finished;
		};
	}
}

#endif
