#ifndef KSYNC_THREAD_UTILITIES_HDR
#define KSYNC_THREAD_UTILITIES_HDR

#include <memory>
#include <atomic>
#include <future>
#include <mutex>
#include <utility>

namespace KSync {
	namespace Utilities {
		// Implementation from C++ Concurrency in action
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
					new_node->next = std::atomic_load(head);
					while(!std::atomic_compare_exchange_weak(head, new_node->next, new_node));
				}
				std::shared_ptr<T> pop() {
					std::shared_ptr<node> old_head = std::atomic_load(head);
					while(old_head && !std::atomic_compare_exchange_weak(head,
						old_head, old_head->next));
					return old_head ? old_head->data : std::shared_ptr<T>();
				}
		};

		//Thread-safe lock-free single producer single consumer queue
		//Implementation inspired from C++ Concurrency in action
		template<typename T>
		class spsc_threadsafe_lock_free_queue {
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

				/*std::shared_ptr<node> pop_head() {
					const std::shared_ptr<node> old_head = std::atomic_load(head);
					if(old_head == std::atomic_load(tail)) {
						return std::shared_ptr<node>();
					}
					std::atomic_store(head, old_head->next);
					return old_head;
				}*/
			public:
				spsc_threadsafe_lock_free_queue() : head(std::move(std::make_shared(new node))) , tail(std::atomic_load(head)) {
				}
				spsc_threadsafe_lock_free_queue(const spsc_threadsafe_lock_free_queue& rhs) = delete;
				spsc_threadsafe_lock_free_queue& operator=(const spsc_threadsafe_lock_free_queue& rhs) = delete;
				~spsc_threadsafe_lock_free_queue() {
					while(const std::shared_ptr<node> old_head = std::atomic_load(head)) {
						std::atomic_store(head, old_head->next);
						//std::atomic_store(old_head, nullptr);
					}
				}

				std::shared_ptr<T> pop() {
					//Load head
					std::shared_ptr<node> old_head = std::atomic_load(head);
					//Check that head isn't same as tail
					if(old_head == std::atomic_load(tail)) {
						return std::shared_ptr<T>();
					}

					// Since this is SPSC queue, 
					// can simply move head
					std::atomic_store(head, old_head->next);
					// and remove data from old_head
					std::shared_ptr<T> res(std::move(old_head->data));
					//old_head will delete itself automatically since we took it's data.
					//std::atomic_store(old_head, nullptr);
					return res;
				}

				void push(std::shared_ptr<T>& new_value) {
					//Create new data
					std::shared_ptr<T> new_data(std::move(new_value));
					//Create new empty node for end of queue.
					std::shared_ptr<node> p(new node);
					//Get current tail.
					const std::shared_ptr<node> old_tail = std::atomic_load(tail);
					while (true) {
						//Create emtpy data pointer
						std::shared_ptr<T> old_data;
						//Compare that empty data pointer to that stored in tail.
						//It should be empty since tail should point to an empty node.
						//Exchange it's content for the new data we created.
						//We do this first so that node's data is prepared.
						if(std::atomic_compare_exchange_strong(old_tail->data, old_data, new_data)) {
							//Set tail's next pointer to new empty node.
							old_tail->next = p;
							//atomically store empty node as new tail..
							std::atomic_store(tail, p);
							//old version:
							//old_tail = std::atomic_exchange(tail, p);
							break;
						}
					}
				}
		};

		//Thread-safe queue with a busy-wait queue (not truely lock-free)
		//Implementation from C++ Concurrency in action
		template<typename T>
		class threadsafe_queue {
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

				/*std::shared_ptr<node> pop_head() {
					const std::shared_ptr<node> old_head = std::atomic_load(head);
					if(old_head == std::atomic_load(tail)) {
						return std::shared_ptr<node>();
					}
					std::atomic_store(head, old_head->next);
					return old_head;
				}*/
			public:
				threadsafe_queue() : head(std::move(std::make_shared(new node))) , tail(std::atomic_load(head)) {
				}
				threadsafe_queue(const threadsafe_queue& rhs) = delete;
				threadsafe_queue& operator=(const threadsafe_queue& rhs) = delete;
				~threadsafe_queue() {
					while(const std::shared_ptr<node> old_head = std::atomic_load(head)) {
						std::atomic_store(head, old_head->next);
						//std::atomic_store(old_head, nullptr);
					}
				}

				std::shared_ptr<T> pop() {
					//Same solution as in lock-free stack for pop.
					std::shared_ptr<node> old_head = std::atomic_load(head);
					while(old_head && !std::atomic_compare_exchange_weak(head,
						old_head, old_head->next));
					return old_head ? old_head->data : std::shared_ptr<T>();
				}

				void push(std::shared_ptr<T>& new_value) {
					//Create new data
					std::shared_ptr<T> new_data(std::move(new_value));
					//Create new empty node for end of queue.
					std::shared_ptr<node> p(new node);
					//Create emtpy data pointer
					std::shared_ptr<T> expected_old_data;
					while (true) {
						//Get current tail.
						const std::shared_ptr<node> old_tail = std::atomic_load(tail);
						//Compare that empty data pointer to that stored in tail.
						//It should be empty since tail should point to an empty node.
						//Exchange it's content for the new data we created.
						//We do this first so that node's data is prepared.
						if(std::atomic_compare_exchange_strong(old_tail->data, expected_old_data, new_data)) {
							//Set tail's next pointer to new empty node.
							old_tail->next = p;
							//atomically store empty node as new tail..
							std::atomic_store(tail, p);
							//old version:
							//old_tail = std::atomic_exchange(tail, p);
							break;
						}
					}
				}
		};

		//Thread-safe queue with a busy-wait queue (not truely lock-free)
		//Implementation inspired from C++ Concurrency in action
		template<typename T>
		class threadsafe_lock_free_queue {
			private:
				struct node {
					std::shared_ptr<T> data;
					std::shared_ptr<node> next;
					node(const T& data_):
						data(std::make_shared<T>(data_)) {
					}
					node() {}
				};

				std::shared_ptr<node> head;
				std::shared_ptr<node> tail;

				void set_new_tail(std::shared_ptr<node>& old_tail, const std::shared_ptr<node>& new_tail) {
					const std::shared_ptr<node> current_tail = old_tail;
					while(!std::atomic_compare_exchange_weak(tail, old_tail, new_tail) && old_tail == current_tail);
				}
				/*std::shared_ptr<node> pop_head() {
					const std::shared_ptr<node> old_head = std::atomic_load(head);
					if(old_head == std::atomic_load(tail)) {
						return std::shared_ptr<node>();
					}
					std::atomic_store(head, old_head->next);
					return old_head;
				}*/
			public:
				threadsafe_lock_free_queue() : head(std::move(std::make_shared(new node))) , tail(std::atomic_load(head)) {
				}
				threadsafe_lock_free_queue(const threadsafe_lock_free_queue& rhs) = delete;
				threadsafe_lock_free_queue& operator=(const threadsafe_lock_free_queue& rhs) = delete;
				~threadsafe_lock_free_queue() {
					while(const std::shared_ptr<node> old_head = std::atomic_load(head)) {
						std::atomic_store(head, old_head->next);
						//std::atomic_store(old_head, nullptr);
					}
				}

				// pop with helping for push
				std::shared_ptr<T> pop() {
					//Same solution as in lock-free stack for pop.
					while (true) {
						std::shared_ptr<node> old_head = std::atomic_load(head);
						if(old_head == std::atomic_load(tail)) {
							return std::shared_ptr<T>();
						}
						//Load head's next node pointer
						std::shared_ptr<node> next = std::atomic_load(old_head->next);
						//Move head's pointer to next node
						if(std::atomic_compare_exchange_strong(head, old_head, next)) {
							//Success! return data
							return std::move(next->data);
						}
						//Failure, try again!
					}
				}

				// push to allow helping
				void push(std::shared_ptr<T>& new_value) {
					//Create new data
					std::shared_ptr<T> new_data(std::move(new_value));
					//Create new empty node for end of queue.
					std::shared_ptr<node> p(new node);
					bool finished = false;
					while (!finished) {
						//Create emtpy data pointer
						std::shared_ptr<T> expected_old_data;
						//Get current tail.
						const std::shared_ptr<node> old_tail = std::atomic_load(tail);
						//Compare that empty data pointer to that stored in tail.
						//It should be empty since tail should point to an empty node.
						//Exchange it's content for the new data we created.
						//We do this first so that node's data is prepared.
						if(std::atomic_compare_exchange_strong(old_tail->data, expected_old_data, new_data)) {
							//Success! we can exit this pass through the loop!
							finished = true;
							break;
						}
						//Create empty node pointer
						std::shared_ptr<node> null_next;
						//Point tail's next to new empty node if it has a null pointer
						std::atomic_compare_exchange_strong(old_tail->next, null_next, p);
						//Set tail to new empty node.
						set_new_tail(old_tail, p);

					}
				}
		};

		template<class T>
		class FutureWrapper {
			public:
				FutureWrapper(std::shared_ptr<std::atomic<bool>> grabbed, std::future<T>&& in) : value_grabbed(grabbed), the_future(in) {}
				FutureWrapper(FutureWrapper<T>&& rhs) : value_grabbed(std::move(rhs.value_grabbed)), the_future(std::move(rhs.the_future)) {}
				T get() {
					T answer = the_future.get();
					value_grabbed->store(true);
					return answer;
				}
				void wait() {
					the_future.wait();
				}
				template<class Rep, class Period>
				std::future_status wait_for(const std::chrono::duration<Rep,Period>& timeout_duration) const {
					return the_future.wait_for(timeout_duration);
				}
				template<class Clock, class Duration>
				std::future_status wait_until(const std::chrono::time_point<Clock,Duration>& timeout_time) const {
					return the_future.wait_until(timeout_time);
				}
			private:
				std::shared_ptr<std::atomic<bool>> value_grabbed;
				std::future<T> the_future;
		};

		template<class T>
		class PromiseWrapper {
			public:
				PromiseWrapper() {
					value_grabbed->store(false);
				}
				bool grabbed() {
					return value_grabbed->load();
				}
				FutureWrapper<T>&& get_future() {
					return std::move(FutureWrapper<T>(value_grabbed, std::move(the_promise.get_future())));
				}
				void set_value(const T& value) {
					the_promise.set_value(value);
				}
				void set_value(T&& value) {
					the_promise.set_value(value);
				}
				void set_value_at_thread_exit(const T& value) {
					the_promise.set_value_at_thread_exit(value);
				}
				void set_value_at_thread_exit(T&& value) {
					the_promise.set_value_at_thread_exit(value);
				}
				void set_exception(std::exception_ptr p) {
					the_promise.set_exception(p);
				}
				void set_exception_at_thread_exit(std::exception_ptr p) {
					the_promise.set_exception_at_thread_exit(p);
				}

			private:
				std::shared_ptr<std::atomic<bool>> value_grabbed;
				std::promise<T> the_promise;
		};

		// Implementation taken from C++ Concurrency in Action page 176
		template<class T>
		class threadsafe_list {
			private:
				struct node {
					std::mutex m;
					std::shared_ptr<T> data;
					std::shared_ptr<node> next;
					node():
						next() {
					}
					node(const T& data_):
						data(std::make_shared<T>(data_)) {
					}
				};
				node head;
			public:
				threadsafe_list() {
				}

				~threadsafe_list() {
					remove_if([](node const&){return true;});
				}

				threadsafe_list(threadsafe_list const& other) = delete;
				threadsafe_list& operator=(threadsafe_list const& other) = delete;

				void push_front(T const& value) {
					std::unique_ptr<node> new_node(new node(value));
					std::lock_guard<std::mutex> lk(head.m);
					new_node->next = std::move(head.next);
					head.next = std::move(new_node);
				}

				template<typename Function>
				void for_each(Function f) {
					node* current = &head;
					std::unique_lock<std::mutex> lk(head.m);
					while(const node* next = current->next.get()) {
						std::unique_lock<std::mutex> next_lk(next->m);
						lk.unlock();
						f(*next->data);
						current = next;
						lk=std::move(next_lk);
					}
				}

				template<typename Predicate>
				std::shared_ptr<T> find_first_if(Predicate p) {
					node* current = &head;
					std::unique_lock<std::mutex> lk(head.m);
					while(const node* next = current->next.get()) {
						std::unique_lock<std::mutex> next_lk(next->m);
						lk.unlock();
						if(p(*next->data)) {
							return next->data;
						}
						current = next;
						lk = std::move(next_lk);
					}
					return std::shared_ptr<T>();
				}

				template<typename Predicate>
				void remove_if(Predicate p) {
					node* current = &head;
					std::unique_lock<std::mutex> lk(head.m);
					while(const node* next = current->next.get()) {
						std::unique_lock<std::mutex> next_lk(next->m);
						if(p(*next->data)) {
							std::unique_ptr<node> old_next = std::move(current->next);
							current->next = std::move(next->next);
							next_lk.unlock();
						} else {
							lk.unlock();
							current = next;
							lk = std::move(next_lk);
						}
					}
				}
		};
	}
}

#endif
