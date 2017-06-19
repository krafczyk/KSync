#ifndef KSYNC_THREAD_UTILITIES_HDR
#define KSYNC_THREAD_UTILITIES_HDR

#include <memory>
#include <atomic>

namespace KSync {
	namespace Utilities {
		template<typename T>
		class threadsafe_lock_free_stack {
			private:
				struct node {
					std::shared_ptr<T> data;
					std::shared_ptr<node> next;
					node(const T& data_):
						data(std::make_shared<T>(data_)) {
					}
				};

				std::shared_ptr<node> head;
			public:
				void push(const T& data) {
					const std::shared_ptr<node> new_node = std::make_shared<node>(data);
					new_node->next = head.load();
					while(!std::atomic_compare_exchange_weak(&head, &new_node->next, new_node));
				}
				std::shared_ptr<T> pop() {
					std::shared_ptr<node> old_head = std::atomic_load(&head);
					while(old_head && !std::atomic_compare_exchange_weak(&head,
						&old_head, old_head->next));
					return old_head ? old_head->data : std::shared_ptr<T>();
				}
		};

		template<typename T>
		class threadsafe_lock_free_queue {
			private:
				struct node {
					std::shared_ptr<T> data;
					std::shared_ptr<node> next;
					node(const T& data_):
						data(std::make_shared<T>(data_)) {
					}
				};

				std::shared_ptr<node> head;
				std::shared_ptr<node> tail;

				std::shared_ptr<node> pop_head() {
					const std::shared_ptr<node> old_head = std::atomic_load(head);
					if(old_head == std::atomic_load(tail)) {
						return std::shared_ptr<node>();
					}
					std::atomic_store(head, old_head->next);
					return old_head;
				}
			public:
				threadsafe_lock_free_queue() : head(std::move(std::make_shared(new node))) , tail(std::atomic_load(head)) {
				}
				threadsafe_lock_free_queue(const threadsafe_lock_free_queue& rhs) = delete;
				threadsafe_lock_free_queue& operator=(const threadsafe_lock_free_queue& rhs) = delete;
				~threadsafe_lock_free_queue() {
					while(const std::shared_ptr<node> old_head = std::atomic_load(head)) {
						std::atomic_store(head, old_head->next);
						std::atomic_store(old_head, nullptr);
					}
				}

				std::shared_ptr<T> pop() {
					std::shared_ptr<node> old_head = pop_head();
					if(!old_head) {
						return std::shared_ptr<T>();
					}

					const std::shared_ptr<T> res(std::move(old_head->data));
					std::atomic_store(old_head, nullptr);
					return res;
				}

				void push(std::shared_ptr<T>& new_value) {
					std::shared_ptr<T> new_data(std::move(new_value));
					std::shared_ptr<node> p(new node);
					const std::shared_ptr<node> old_tail = std::atomic_load(tail);
					old_tail->data.swap(new_data);
					old_tail->next = p;
					std::atomic_store(tail, p);
				}
		};
	}
}

#endif
