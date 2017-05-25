#include "ksync/pstreams_command_system.h"

namespace KSync {
	namespace Commanding {
		int PSExecutionContext::LaunchCommand(const std::string& command) {
			process_stream.reset(new redi::ipstream(command.c_str(), redi::pstreams::pstdout|redi::pstreams::pstderr));
			return 0;
		}

		ExecutionContext::Status_t PSExecutionContext::GetOutputUpdate(std::string& std_out, std::string& std_err) {
			std_out.clear();
			std_err.clear();
			bool out_status = bool(std::getline(process_stream->out(), std_out));
			bool err_status = bool(std::getline(process_stream->err(), std_err));
			if(out_status||err_status) {
				return Success;
			} else {
				return NoMore;
			}
		}

		bool PSExecutionContext::IsFinished() {
			if(process_stream) {
				return process_stream->rdbuf()->exited();
			} else {
				return true;
			}
		}

		ExecutionContext::Return_t PSExecutionContext::GetReturnCode() {
			if(this->IsFinished()) {
				return process_stream->rdbuf()->status();
			} else {
				return -1;
			}
		}

		std::string PSExecutionContext::GetCommandLaunched() {
			if(process_stream) {
				return process_stream->command();
			} else {
				return "";
			}
		}
	}
}
