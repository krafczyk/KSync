#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sstream>

#include <nanomsg/nn.h>

#include "ksync/utilities.h"
#include "ksync/logging.h"

namespace KSync {
	namespace Utilities {
		void reset_error() {
			errno = 0;
		}
		int check_error() {
			if(nn_errno() != 0) {
				Error("An error occured! %i (%s)\n", nn_errno(), nn_strerror(nn_errno()));
				return nn_errno();
			}
			return 0;
		}
		int get_socket_dir(std::string& dir) {
			char* login_name = getlogin();
			if(login_name == 0) {
				Error("Couldn't get the username!\n");
				return -1;
			}

			std::stringstream ss;
			ss << "/tmp/ksync-" << login_name;

			if(access(ss.str().c_str(), F_OK) != 0) {
				if(errno != 2) {
					Error("An error was encountered while checking for the existence of the socket directory!\n");
					return -2;
				}
				//Directory doesn't exist, we better create is
				
				if(mkdir(ss.str().c_str(), 0700) != 0) {
					Error("An error was encountered while creating the socket directory!\n");
					return -3;
				}
			}
			
			dir = ss.str();

			return 0;
		}
	}
}