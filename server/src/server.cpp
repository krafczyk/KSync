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

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <errno.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#include "ksync/server.h"
#include "ksync/optparse.h"
#include "ksync/logging.h"
#include "ksync/messages.h"

#define PORT "3490"

#define MAXDATASIZE 100 // max number of bytes we can get at once

#define BACKLOG 10

void sigchld_handler(int s __attribute__((unused))) {
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char** argv) {
	KSync::ArgParser arg_parser("KSync Server - Server side of a Client-Server synchonization system using rsync.");

	int status;
	if((status = arg_parser.ParseArgs(argc, argv)) < 0) {
		Error("Problem parsing arguments\n");
		arg_parser.PrintHelp();
		return -1;
	}

	int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	int yes=1;
	char s[INET6_ADDRSTRLEN];
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		Error("getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			Error("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			Error("setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			Error("server: bind");
			continue;
		}

		break;
	}

	if (p == NULL)  {
		Error("server: failed to bind\n");
		return 2;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (listen(sockfd, BACKLOG) == -1) {
		Error("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		Error("sigaction");
		exit(1);
	}

	KPrint("server: waiting for connections...\n");

	while(1) {  // main accept() loop
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) {
			Error("accept");
			continue;
		}

		inet_ntop(their_addr.ss_family,
			get_in_addr((struct sockaddr *)&their_addr),
			s, sizeof s);
		KPrint("server: got connection from %s\n", s);

		if (!fork()) { // this is the child process
			close(sockfd); // child doesn't need the listener

			bool quit = false;
			while(!quit) {
				//Receive message
				int numbytes;
				char buf[MAXDATASIZE];
				if ((numbytes = recv(new_fd, buf, MAXDATASIZE-1, 0)) == -1) {
					Error("recv");
					quit = true;
					continue;
				}
				buf[numbytes] = '\0';

				KPrint("Received: %s\n", buf);

				std::string message(buf);

				//Formulate Reply
				std::string reply;
				std::string unwrapped_message;
				bool ready_to_send = false;
				KSync::Messages::Message_t message_type;
				if(KSync::Messages::UnWrapMessage(unwrapped_message, message_type, message)<0) {
					Warning("Failed to unwrap message(%s)\n", message.c_str());
					if(KSync::Messages::WrapAsReply(reply, "Failed to unwrap message.")<0) {
						Error("Failed to wrap a reply.\n");
						quit = true;
						continue;
					}
					ready_to_send = true;
				}

				if(message_type == KSync::Messages::Quit) {
					quit = true;
					KSync::Messages::CreateQuit(reply);
					ready_to_send = true;
				}
	
				if(!ready_to_send) {
					//Here I would execute the command
					if(KSync::Messages::WrapAsReply(reply, unwrapped_message)<0) {
						Warning("Failed to wrap reply (%s)\n", buf);
						continue;
					}
				} 
				//Send the quit and simple replies.
				//Send Reply
				KPrint("Sending reply\n");
				if (send(new_fd, reply.c_str(), reply.size(), 0) == -1) {
					Error("send");
					quit = true;
					continue;
				}
				if(quit) {
					continue;
				} else {
					KPrint("Sending end\n");
					KSync::Messages::CreateEnd(reply);
					if (send(new_fd, reply.c_str(), reply.size(), 0) == -1) {
						Error("send");
						quit = true;
						continue;
					}
				}
			}
			KPrint("I'm quitting.\n");
			close(new_fd);
			exit(0);
		}
		close(new_fd);  // parent doesn't need this
	}

	return 0;
}
