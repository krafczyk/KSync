#ifndef KSYNC_CLIENT_COMMUNICATOR_HDR
#define KSYNC_CLIENT_COMMUNICATOR_HDR

#include <future>
#include <thread>
#include <map>

#include "ksync/comm/object.h"
#include "ksync/comm/interface.h"
#include "ksync/utilities.h"
#include "ksync/thread_utilities.h"
#include "ksync/ksync_exception.h"

namespace KSync {
	namespace Comm {
		class ClientCommunicator {
			public:
				class SocketException : public KSync::Exception::BasicException {
					public:
						SocketException();
				};

				ClientCommunicator(std::shared_ptr<KSync::Comm::CommSystemInterface>& comm_system, const KSync::Utilities::client_id_t client_id, const bool bind);
				~ClientCommunicator();

				KSync::Utilities::FutureWrapper<std::shared_ptr<CommObject>> send_get_response(std::shared_ptr<CommObject>& obj);
				void send(std::shared_ptr<CommObject>& obj);
				std::shared_ptr<CommObject> get();

				void finish();

				void watch_function();

				const std::string& GetSocketUrl() const {
					return this->socket_url;
				}
				KSync::Utilities::client_id_t GetClientId() const {
					return this->id;
				}
			private:
				std::shared_ptr<Utilities::threadsafe_lock_free_queue<CommObject>> push_queue;
				std::shared_ptr<Utilities::threadsafe_lock_free_queue<CommObject>> pull_queue;

				std::shared_ptr<KSync::Comm::CommSystemSocket> socket;
				std::string socket_url;

				std::map<CommObject::message_id_t,KSync::Utilities::PromiseWrapper<std::shared_ptr<CommObject>>> promise_map;

				std::shared_ptr<std::thread> watch_thread;
				std::atomic<bool> finished;
				KSync::Utilities::client_id_t id;
		};

		class ClientCommunicatorList {
			public:
				ClientCommunicatorList() {}
				~ClientCommunicatorList() {}


				void push_front(ClientCommunicator const& value);
				std::shared_ptr<ClientCommunicator> find_first_if(KSync::Utilities::client_id_t id);
				void remove_if(KSync::Utilities::client_id_t id);
				template<typename Function> void for_each(Function f) {
					list.for_each(f);
				}
			private:
				KSync::Utilities::threadsafe_list<ClientCommunicator> list;
		};
	}
}

#endif
