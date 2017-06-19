#ifndef KSYNC_CLIENT_COMMUNICATOR_HDR
#define KSYNC_CLIENT_COMMUNICATOR_HDR

#include "ksync/comm/object.h"
#include "ksync/thread_utilities.h"

namespace KSync {
	namespace Comm {
		class ClientCommunicator {
			public:
				ClientCommunicator();
			private:
				std::shared_ptr<Utilities::spsc_threadsafe_lock_free_queue<CommObject>> push_queue;
				std::shared_ptr<Utilities::spsc_threadsafe_lock_free_queue<CommObject>> pull_queue;
		};
	}
}

#endif
