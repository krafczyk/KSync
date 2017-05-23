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

#ifndef KSYNC_COMM_SYSTEM_INT_HDR
#define KSYNC_COMM_SYSTEM_INT_HDR

#include <string>

#include "ksync/comm_system_object.h"

namespace KSync {
	namespace Comm {
		class CommSystemSocket {
			public:
				CommSystemSocket();
				virtual ~CommSystemSocket();

				static const int Success = 0;
				static const int Timeout = 1;
				static const int EmptyMessage = 2;
				static const int Other = -1;

				virtual int Bind(const std::string& address) = 0;
				virtual int Connect(const std::string& address) = 0;
				virtual int Send(const std::shared_ptr<CommObject> comm_obj) = 0;
				virtual int Recv(std::shared_ptr<CommObject>& comm_obj) = 0;
				virtual int ForceRecv(std::shared_ptr<CommObject>& comm_obj);
				virtual int SetSendTimeout(int timeout) = 0;
				virtual int SetRecvTimeout(int timeout) = 0;
		};

		class CommSystemInterface {
			public:
				CommSystemInterface();
				virtual ~CommSystemInterface();

				virtual int Create_Gateway_Req_Socket(std::shared_ptr<CommSystemSocket>& socket) = 0;
				virtual int Create_Gateway_Rep_Socket(std::shared_ptr<CommSystemSocket>& socket) = 0;
				virtual int Create_Pair_Socket(std::shared_ptr<CommSystemSocket>& socket) = 0;
				virtual int Create_Pub_Socket(std::shared_ptr<CommSystemSocket>& socket) = 0;
				virtual int Create_Sub_Socket(std::shared_ptr<CommSystemSocket>& socket) = 0;
		};
	}
}

#endif
