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
		void set_up_common_arguments_and_defaults(ArgParse::ArgParser& Parser, std::string& log_dir, std::string& gateway_socket_url, bool& gateway_socket_url_defined, bool& nanomsg) {
			log_dir = "";
			gateway_socket_url = "";
			gateway_socket_url_defined = false;
			nanomsg = false;

			Parser.AddArgument("--log-dir", "Use this directory for logging.", &log_dir);
			Parser.AddArgument("--nanomsg", "Use nanomsg comm backend. Deafult is zeromq", &nanomsg);
			Parser.AddArgument("gateway-socket", "Socket to use to negotiate new client connections. Default is : ipc:///tmp/ksync/<user>/ksync-connect.ipc", &gateway_socket_url, ArgParse::Argument::Optional, &gateway_socket_url_defined);
		}
		int get_user_ksync_dir(std::string& dir) {
			char* login_name = getlogin();
			if(login_name == 0) {
				LOGF(SEVERE, "Couldn't get the username!");
				return -1;
			}
			std::stringstream ss;
			ss << "/home/" << login_name << "/.ksync";
			dir = ss.str();
			if(access(dir.c_str(), F_OK) != 0) {
				if(mkdir(dir.c_str(), 0700) != 0) {
					LOGF(SEVERE, "There was a problem creating the ksync user directory!");
					return -2;
				}
			}
			return 0;
		}
		int get_socket_dir(std::string& dir) {
			char* login_name = getlogin();
			if(login_name == 0) {
				LOGF(SEVERE, "Couldn't get the username!");
				return -1;
			}

			std::stringstream ss;
			ss << "/tmp/" << login_name;

			if(access(ss.str().c_str(), F_OK) != 0) {
				if(errno != 2) {
					LOGF(SEVERE, "An error was encountered while checking for the existence of the user temporary directory!");
					return -2;
				}
				//Directory doesn't exist, we better create it
				
				if(mkdir(ss.str().c_str(), 0700) != 0) {
					LOGF(SEVERE, "An error was encountered while creating the user temporary directory!");
					return -3;
				}
			}

			ss.str(std::string());
			ss << "/tmp/" << login_name << "/ksync";
			
			if(access(ss.str().c_str(), F_OK) != 0) {
				if(errno != 2) {
					LOGF(SEVERE, "An error was encountered while checking for the existence of the ksync directory!");
					return -2;
				}
				//Directory doesn't exist, we better create it
				
				if(mkdir(ss.str().c_str(), 0700) != 0) {
					LOGF(SEVERE, "An error was encountered while creating the ksync directory (%s)!", ss.str().c_str());
					return -3;
				}
			}

			dir = ss.str();

			return 0;
		}
		int get_default_ipc_connection_url(std::string& connection_url) {
			std::string socket_dir;
			if(KSync::Utilities::get_socket_dir(socket_dir) < 0) {
				LOGF(SEVERE, "There was a problem getting the default socket directory!");
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
		int get_default_gateway_thread_url(std::string& connection_url) {
			connection_url = "inproc://gateway_thread";
			return 0;
		}
		int get_default_broadcast_url(std::string& url) {
			std::string socket_dir;
			if(KSync::Utilities::get_socket_dir(socket_dir) < 0) {
				LOGF(SEVERE, "There was a problem getting the default socket directory!");
				return -2;
			}
			std::stringstream ss;
			ss << "ipc://" << socket_dir << "/ksync-broadcast.ipc";
			url = ss.str();
			return 0;
		}
		int get_default_connection_url(std::string& connection_url) {
			return get_default_ipc_connection_url(connection_url);
		}

		int get_client_socket_url(std::string& socket_url, const client_id_t client_id) {
			std::string socket_dir;
			if(KSync::Utilities::get_socket_dir(socket_dir) < 0 ) {
				LOGF(SEVERE, "There was a problem getting the default socket directory!");
				return -2;
			}
			std::stringstream ss;
			ss << "ipc://" << socket_dir << "/ksync-" << client_id << ".ipc";
			socket_url = ss.str();
			return 0;
		}
	}
}
