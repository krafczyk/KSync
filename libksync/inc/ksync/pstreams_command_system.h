#ifndef KSYNC_PSTREAMS_COMMAND_SYSTEM_HDR
#define KSYNC_PSTREAMS_COMMAND_SYSTEM_HDR

#include "ksync/command_system_interface.h"
#include "ksync/pstream.h"

namespace KSync {
	namespace Commanding {
		class PSExecutionContext : public ExecutionContext {
			public:
				int LaunchCommand(const std::string& command); // Launch command
				Status_t GetOutputUpdate(std::string& std_out, std::string& std_err); // Nonblocking Output fetching
				bool IsFinished(); // Check whether command has completed
				Return_t GetReturnCode();
				std::string GetCommandLaunched();
			protected:
				std::shared_ptr<redi::ipstream> process_stream;
		};

		class PSCommandSystem : public SystemInterface {
			public:
				std::shared_ptr<ExecutionContext> GetExecutionContext() {
					return std::shared_ptr<ExecutionContext>(new PSExecutionContext());
				}
		};
	}
}

#endif
