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

#include <string>

#include <nanomsg/nn.h>
#include <nanomsg/pair.h>

#include "ksync/logging.h"
#include "ksync/utilities.h"
#include "ksync/socket_ops.h"

namespace KSync {
	namespace SocketOps {
		int Create_Pair_Socket(int& socket) {
			socket = nn_socket(AF_SP, NN_PAIR);
			if(socket < 0) {
				Error("An error was encountered while creating the socket. (%s)\n", nn_strerror(nn_errno()));
				return -1;
			}
			return 0;
		}

		int Bind_Pair_Socket(int& endpoint, const int socket, const std::string socket_url) {
			Utilities::reset_error();
			endpoint = nn_bind(socket, socket_url.c_str());
			if(endpoint < 0) {
				Error("An error was encountered while trying to bind to the socket. (%s)\n", nn_strerror(nn_errno()));
				return -1;
			}
			int status = Utilities::check_error();
			if((status != 0)&&(status != 11)) {
				return -2;
			}
			return 0;
		}

		int Connect_Pair_Socket(int& endpoint, const int socket, const std::string socket_url) {
			Utilities::reset_error();
			endpoint = nn_connect(socket, socket_url.c_str());
			if(endpoint < 0) {
				Error("An error was encountered while trying to bind to the socket. (%s)\n", nn_strerror(nn_errno()));
				return -1;
			}
			int status = Utilities::check_error();
			if((status != 0)&&(status != 11)) {
				return -2;
			}
			return 0;
		}

		int Set_Socket_Timeout(const int socket, const int to) {
			Utilities::reset_error();
			if(nn_setsockopt(socket, NN_SOL_SOCKET, NN_RCVTIMEO, &to, sizeof(to)) < 0) {
				Error("An error was concountered while trying to set the timeout for the connection socket! (%s)\n", nn_strerror(nn_errno()));
				return -1;
			}
			if(Utilities::check_error() != 0) {
				return -2;
			}
			return 0;
		}

		int Create_And_Bind_Connection_Socket(int& socket, int& endpoint, const std::string connect_socket_url) {
			if(Create_Pair_Socket(socket) < 0) {
				Error("An error was encountered trying to create the connection socket!\n");
				return -1;
			}
			if(Bind_Pair_Socket(endpoint, socket, connect_socket_url) < 0) {
				Error("An error was encountered trying to bind to the connection socket!\n");
				return -2;
			}
			if(Set_Socket_Timeout(socket, 100) < 0) {
				Error("An error was encountered trying to set the connection socket's timeout!\n");
				return -3;
			}
			return 0;
		}

		int Create_And_Connect_Connection_Socket(int& socket, int& endpoint, const std::string connect_socket_url) {
			if(Create_Pair_Socket(socket) < 0) {
				Error("An error was encountered trying to create the connection socket!\n");
				return -1;
			}
			if(Connect_Pair_Socket(endpoint, socket, connect_socket_url) < 0) {
				Error("An error was encountered trying to bind to the connection socket!\n");
				return -2;
			}
			if(Set_Socket_Timeout(socket, 100) < 0) {
				Error("An error was encountered trying to set the connection socket's timeout!\n");
				return -3;
			}
			return 0;
		}

		int Shutdown_Socket(const int socket, const int endpoint) {
			Utilities::reset_error();
			if(nn_shutdown(socket, endpoint) < 0) {
				Error("An error was encountered while trying to shutdown an endpoint of a socket! (%s)\n", nn_strerror(nn_errno()));
				return -1;
			}
			if(Utilities::check_error() != 0) {
				return -2;
			}
			return 0;
		}

		int Receive_Message(std::string& message, const int socket) {
			char* buf = NULL;
			Utilities::reset_error();
			if(nn_recv(socket, &buf, NN_MSG, 0) < 0) {
				if(nn_errno() == EAGAIN) {
					return 1;
				}
				Error("An error was encountered while trying to receive a message from a socket! (%s)\n", nn_strerror(nn_errno()));
				return -1;
			}
			if(Utilities::check_error() != 0) {
				return -2;
			}
			message = buf;
			if(nn_freemsg(buf) < 0) {
				Error("An error was encountered while trying to free a message buffer. (%s)\n", nn_strerror(nn_errno()));
				return -3;
			}
			if(Utilities::check_error()) {
				return -4;
			}
			return 0;
		}

		int Send_Message(const std::string& message, const int socket) {
			Utilities::reset_error();
			int send_result = nn_send(socket, (void*) message.c_str(), message.size(), 0);
			if(send_result < 0) {
				Error("An error was encountered while sending a message. (%s)\n", nn_strerror(nn_errno()));
				return -1;
			}
			if(Utilities::check_error() != 0) {
				return -2;
			}

			if(send_result != (int) message.size()) {

				Error("An error was encountered while sending a message. The size of the message sent does not match the size of the message we wanted to send.\n");
				return -3;
			}
			return 0;
		}
	}
}
