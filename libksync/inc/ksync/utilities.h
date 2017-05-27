/*
KSync - Client-Server synchronization system using rsync.
Copyright (C) 2015  Matthew Scott Krafczyk

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef KSYNC_UTILITIES_HDR
#define KSYNC_UTILITIES_HDR

#include <string>
#include <algorithm>
#include "ArgParse/ArgParse.h"

namespace KSync {
	namespace Utilities {
		void set_up_common_arguments_and_defaults(ArgParse::ArgParser& Parser, std::string& log_dir, std::string& gateway_socket_url, bool& gateway_socket_url_defined, bool& nanomsg);
		int get_user_ksync_dir(std::string& dir);
		int get_socket_dir(std::string& dir);
		int get_default_ipc_connection_url(std::string& connection_url);
		int get_default_tcp_connection_url(std::string& connection_url);
		int get_default_gateway_thread_url(std::string& connection_url);
		int get_default_broadcast_url(std::string& url);
		int get_default_connection_url(std::string& connection_url);
		typedef unsigned long client_id_t;
		client_id_t GenerateNewClientId();
		int get_client_socket_url(std::string& dir, const client_id_t client_id);
		static inline std::string &ltrim(std::string &s) {
			s.erase(s.begin(), std::find_if(s.begin(), s.end(),
				std::not1(std::ptr_fun<int, int>(std::isspace))));
			return s;
		}
		static inline std::string &rtrim(std::string &s) {
			s.erase(std::find_if(s.rbegin(), s.rend(),
				std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
			return s;
		}
		static inline std::string &trim(std::string &s) {
			return ltrim(rtrim(s));
		}
	}
}

#endif
