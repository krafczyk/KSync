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

#ifndef KSYNC_SOCKET_OPS_HDR
#define KSYNC_SOCKET_OPS_HDR

#include <string>

namespace KSync {
	namespace SocketOps {
		int Create_Rep_Socket(int& socket);
		int Bind_Rep_Socket(int& endpoint, const int socket, const std::string& socket_url);
		int Create_Req_Socket(int& socket);
		int Connect_Req_Socket(int& endpoint, const int socket, const std::string& socket_url);
		int Create_Pair_Socket(int& socket);
		int Bind_Pair_Socket(int& endpoint, const int socket, const std::string& socket_url);
		int Connect_Pair_Socket(int& endpoint, const int socket, const std::string& socket_url);
		int Set_Socket_Timeout(const int socket);
		int Create_And_Bind_Connection_Socket(int& socket, int& endpoint, const std::string& connect_socket_url);
		int Create_And_Connect_Connection_Socket(int& socket, int& endpoint, const std::string& connect_socket_url);
		int Shutdown_Socket(const int socket, const int endpoint);
		int Receive_Message(std::string& message, const int socket);
		int Send_Message(const std::string& message, const int socket);
	}
}

#endif
