#include "ksync/logging.h"
#include "ksync/command_system_interface.h"

namespace KSync {
	namespace Commanding {
		ExecutionContext::Status_t ExecutionContext::GetOutput(std::string& std_out, std::string& std_err) {
			Status_t status;
			std_out.clear();
			std_err.clear();
			std::string temp_std_out;
			std::string temp_std_err;
			do {
				if(temp_std_out != "") {
					std_out += temp_std_out + "\n";
				}
				if(temp_std_err != "") {
					std_err += temp_std_err + "\n";
				}
				status = this->GetOutputUpdate(temp_std_out, temp_std_err);
			} while (status == Success);
			return status;
		}
	}
}
