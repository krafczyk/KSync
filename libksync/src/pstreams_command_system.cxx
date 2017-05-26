#include "ksync/logging.h"
#include "ksync/pstreams_command_system.h"

namespace KSync {
	namespace Commanding {
		int PSExecutionContext::LaunchCommand(const std::string& command) {
			process_stream.reset(new redi::pstream(command.c_str(), redi::pstreams::pstdout|redi::pstreams::pstderr));
			return 0;
		}

		ExecutionContext::Status_t PSExecutionContext::GetOutputUpdate(std::string& std_out, std::string& std_err) {
			std_out.clear();
			std_err.clear();
			if(process_stream->eof()) {
				process_stream->clear();
			}
			bool std_out_status = bool(std::getline(process_stream->out(), std_out));
			if(process_stream->eof()) {
				process_stream->clear();
			}
			bool std_err_status = bool(std::getline(process_stream->err(), std_err));
			if(std_out_status||std_err_status) {
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
