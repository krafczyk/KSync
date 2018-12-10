#include "ksync/logging.h"
#include "ksync/client_communicator.h"

namespace KSync {
	namespace Comm {
		ClientCommunicator::SocketException::SocketException() {
			this->SetMessage("Socket problem during construction of client communicator");
		};

		ClientCommunicator::ClientCommunicator(std::shared_ptr<KSync::Comm::CommSystemInterface>& comm_system, const KSync::Utilities::client_id_t client_id, const bool bind) {
			this->id = client_id;
			this->finished.store(false);
			//Get client socket URL
			if(KSync::Utilities::get_client_socket_url(this->socket_url, this->id) < 0) {
				throw SocketException();
			}
			//Create socket
			if(comm_system->Create_Pair_Socket(this->socket, 10) < 0) {
				throw SocketException();
			}

			//Bind or connect to said socket
			if(bind) {
				if(this->socket->Bind(this->socket_url) < 0) {
					throw SocketException();
				}
			} else {
				if(this->socket->Connect(this->socket_url) < 0) {
					throw SocketException();
				}
			}

			//Launch watcher thread
			watch_thread.reset(new std::thread(&ClientCommunicator::watch_function, this));
		}

		ClientCommunicator::~ClientCommunicator() {
			this->finish();
			if (watch_thread) {
				if(watch_thread->joinable()) {
					watch_thread->join();
				}
			}
		}

		Utilities::FutureWrapper<std::shared_ptr<CommObject>> ClientCommunicator::send_get_response(std::shared_ptr<CommObject>& obj) {
			CommObject::message_id_t message_id = obj->GetMessageId();
			Utilities::PromiseWrapper<std::shared_ptr<CommObject>> promise;
			Utilities::FutureWrapper<std::shared_ptr<CommObject>> future_comm_obj = promise.get_future();
			promise_map[message_id] = std::move(promise);
			return future_comm_obj;
		}

		void ClientCommunicator::send(std::shared_ptr<CommObject>& obj) {
			this->push_queue->push(obj);
		}

		std::shared_ptr<CommObject> ClientCommunicator::get() {
			return this->pull_queue->pop();
		}

		void ClientCommunicator::finish() {
			this->finished.store(true);
		}

		void ClientCommunicator::watch_function() {
			std::shared_ptr<KSync::Comm::CommObject> recv_obj;
			int status;
			while (!this->finished.load()) {
				//Check socket for new messages
				recv_obj.reset();
				status = this->socket->Recv(recv_obj);
				if(status == KSync::Comm::CommSystemSocket::Other) {
					LOGF(SEVERE, "There was a problem checking for new messages!");
				} else if (status == KSync::Comm::CommSystemSocket::Timeout) {
				} else if (status == KSync::Comm::CommSystemSocket::EmptyMessage) {
				} else if (status == KSync::Comm::CommSystemSocket::Success) {
					//there's a new message
					//Check whether there's a promise waiting
					bool stored = false;
					if(recv_obj->GetReplyId() > 0) {
						try {
							auto& obj_i = promise_map.at(recv_obj->GetReplyId());
							obj_i.set_value(recv_obj);
							stored = true;
						} catch (std::out_of_range) {
							LOGF(SEVERE, "Couldn't find promise for reply_id!");
						}
					}
					if (!stored) {
						//Store unmatched promises in the pull queue.
						this->pull_queue->push(recv_obj);
					}
				}

				//Check promise map for finished promises
				for(auto promise_map_i = promise_map.begin(); promise_map_i != promise_map.end();) {
					if(promise_map_i->second.grabbed()) {
						promise_map_i = promise_map.erase(promise_map_i);
					} else {
						++promise_map_i;
					}
				}

				//Get message from the push queue
				std::shared_ptr<CommObject> send_obj = this->push_queue->pop();
				if(send_obj) {
					//We have a non-null object
					status = this->socket->Send(send_obj);
					if(status == KSync::Comm::CommSystemSocket::Other) {
						LOGF(SEVERE, "Couldn't send message! message lost!");
					} else if (status == KSync::Comm::CommSystemSocket::Timeout) {
						LOGF(SEVERE, "Timeout sending message! message lost!");
					}
				}
			}
		}

		void ClientCommunicatorList::push_front(ClientCommunicator const& value) {
			list.push_front(value);
		}

		std::shared_ptr<ClientCommunicator> ClientCommunicatorList::find_first_if(KSync::Utilities::client_id_t id) {
			return list.find_first_if(
				[id](ClientCommunicator& item) {
					if (item.GetClientId() == id) {
						return true;
					} else {
						return false;
					}
				});
		}

		void ClientCommunicatorList::remove_if(KSync::Utilities::client_id_t id) {
			list.remove_if(
				[id](ClientCommunicator& item) {
					if (item.GetClientId() == id) {
						return true;
					} else {
						return false;
					}
				});
		}
	}
}
