/*
KSync - Client-Server synchronization system using rsync.
Copyright (C) 2014  Matthew Scott Krafczyk

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

#ifndef KSYNC_SERVER_SERVER_HDR
#define KSYNC_SERVER_SERVER_HDR

#include <vector>
#include <utility>

namespace KSync {
	namespace Server {
		int Process_New_Connections(std::vector<std::pair<int,int>> active_sockets, const int connection_socket);
	}
}

#endif
