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

namespace KSync {
	namespace Utilities {
		void reset_error();
		int check_error();
		int get_socket_dir(std::string& dir);
		int get_default_ipc_connection_url(std::string& connection_url);
		int get_default_connection_url(std::string& connection_url);
		//char get_random_letter();
		//std::string get_random_string(const int num_char);
	}
}

#endif
