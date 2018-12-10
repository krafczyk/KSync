#ifndef KSYNC_EXCEPTION_HDR
#define KSYNC_EXCEPTION_HDR

#include <exception>
#include <string>

namespace KSync {
	namespace Exception {
		class BasicException : public std::exception {
			public:
				BasicException(const std::string& message = "") {
					this->message = message;
				};
				const std::string& GetMessage() {
					return this->message;
				}
				void SetMessage(const std::string& message) {
					this->message = message;
				}
			private:
				std::string message;
		};
	}
}

#endif
