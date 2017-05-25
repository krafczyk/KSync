#ifndef KSYNC_COMMAND_SYSTEM_INTERFACE_HDR
#define KSYNC_COMMAND_SYSTEM_INTERFACE_HDR

#include <memory>
#include <string>

namespace KSync {
	namespace Commanding {
		class ExecutionContext {
			public:
				typedef int Status_t;
				static const Status_t Success = 0;
				static const Status_t Failure = -1;
				static const Status_t NoMore = 1;
				typedef int Return_t;
				virtual int LaunchCommand(const std::string& command) = 0; // Launch command
				virtual Status_t GetOutputUpdate(std::string& std_out, std::string& std_err) = 0; // Nonblocking Output fetching
				Status_t GetOutput(std::string& std_out, std::string& std_err); // Block until program finishes
				virtual bool IsFinished() = 0; // Check whether command has completed
				virtual Return_t GetReturnCode() = 0;
				virtual std::string GetCommandLaunched() = 0;
		};
		class SystemInterface {
			public:
				virtual std::shared_ptr<ExecutionContext> GetExecutionContext() = 0;
		};
	}
}

#endif
