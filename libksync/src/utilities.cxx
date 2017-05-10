#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sstream>
#include <random>
#include <limits>

#include "ksync/utilities.h"
#include "ksync/logging.h"

namespace KSync {
	namespace Utilities {
		void set_up_common_arguments_and_defaults(ArgParse::ArgParser& Parser, std::string& connect_socket_url, bool& connect_socket_url_defined, bool& nanomsg) {
			connect_socket_url = "";
			connect_socket_url_defined = false;
			nanomsg = false;

			Parser.AddArgument("--nanomsg", "Use nanomsg comm backend. Deafult is zeromq", &nanomsg);
			Parser.AddArgument("connect-socket", "Socket to use to negotiate new client connections. Default is : ipc:///tmp/ksync/<user>/ksync-connect.ipc", &connect_socket_url, ArgParse::Argument::Optional, &connect_socket_url_defined);
		}
		int get_socket_dir(std::string& dir) {
			char* login_name = getlogin();
			if(login_name == 0) {
				Error("Couldn't get the username!\n");
				return -1;
			}

			std::stringstream ss;
			ss << "/tmp/" << login_name;

			if(access(ss.str().c_str(), F_OK) != 0) {
				if(errno != 2) {
					Error("An error was encountered while checking for the existence of the user temporary directory!\n");
					return -2;
				}
				//Directory doesn't exist, we better create it
				
				if(mkdir(ss.str().c_str(), 0700) != 0) {
					Error("An error was encountered while creating the user temporary directory!\n");
					return -3;
				}
			}

			ss.clear();
			ss << "/tmp/" << login_name << "/ksync";
			
			if(access(ss.str().c_str(), F_OK) != 0) {
				if(errno != 2) {
					Error("An error was encountered while checking for the existence of the ksync directory!\n");
					return -2;
				}
				//Directory doesn't exist, we better create it
				
				if(mkdir(ss.str().c_str(), 0700) != 0) {
					Error("An error was encountered while creating the ksync directory!\n");
					return -3;
				}
			}

			dir = ss.str();

			return 0;
		}
		int get_default_ipc_connection_url(std::string& connection_url) {
			std::string socket_dir;
			if(KSync::Utilities::get_socket_dir(socket_dir) < 0) {
				Error("There was a problem getting the default socket directory!\n");
				return -2;
			}
			std::stringstream ss;
			ss << "ipc://" << socket_dir << "/ksync-connect.ipc";
			connection_url = ss.str();
			return 0;
		}
		int get_default_tcp_connection_url(std::string& connection_url) {
			connection_url = "tcp://*:6060";
			return 0;
		}
		int get_default_connection_url(std::string& connection_url) {
			return get_default_ipc_connection_url(connection_url);
		}
		client_id_t GenerateNewClientId() {
			std::random_device rd;
			std::mt19937_64 gen(rd());
			std::uniform_int_distribution<client_id_t> dis(std::numeric_limits<client_id_t>::min(), std::numeric_limits<client_id_t>::max());
			return dis(gen);
		}
	}
}
