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

#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#include <nanomsg/nn.h>
#include <nanomsg/pair.h>

#include "ksync/logging.h"
#include "ksync/client.h"
#include "ksync/messages.h"

#include "ArgParse/ArgParse.h"

#define PORT "3490" // the port client will be connecting to 

#define MAXDATASIZE 100 // max number of bytes we can get at once

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char** argv) {
	std::string hostname;

	ArgParse::DebugLevel = 10;

	ArgParse::ArgParser arg_parser("KSync Server - Client side of a Client-Server synchonization system using rsync.");
	arg_parser.AddOption("host", "The host name to use.", &hostname, ArgParse::Option::Required);

	if(arg_parser.ParseArgs(argc, argv) < 0) {
		Error("Problem parsing arguments\n");
		arg_parser.PrintHelp();
		return -1;
	}

	if(arg_parser.HelpPrinted()) {
		return 0;
	}

	int sockfd, numbytes;  
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(hostname.c_str(), PORT, &hints, &servinfo)) != 0) {
		Error("getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			Error("client: socket\n");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			Error("client: connect\n");
			continue;
		}

		break;
	}

	if (p == NULL) {
		Error("client: failed to connect\n");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
	KPrint("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure

	KPrint("KSync: Client Server command test\n");
	KPrint("'quit' to quit.\n");
	bool quit = false;
	while(!quit) {

		KPrint("ksync_client: ");
		std::string command;
		std::getline(std::cin, command);
		std::string wrapped_message;

		if(command == "quit") {
			KSync::Messages::CreateQuit(wrapped_message);
		} else {
			if(KSync::Messages::WrapAsCommand(wrapped_message, command)<0) {
				Warning("Failed to wrap command (%s)\n", command.c_str());
				continue;
			}
		}

		if (send(sockfd, wrapped_message.c_str(), wrapped_message.size(), 0) == -1) {
			Warning("Failed to send command.\n");
			quit = true;
			continue;
		}

		while(true) {
			KPrint("Receiving a reply.\n");
			if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
				Error("recv\n");
				quit = true;
				break;
			}
			buf[numbytes] = '\0';

			//Unwrap reply.
			std::string message(buf);
			KSync::Messages::Message_t message_type;
			std::string unwrapped_message;
			if(KSync::Messages::UnWrapMessage(unwrapped_message, message_type, message)<0) {
				Error("Couldn't unwrap the message (%s)\n", message.c_str());
				quit = true;
				break;
			}
	
			if(message_type == KSync::Messages::Quit) {
				quit = true;
				break;
			}

			if(message_type == KSync::Messages::End) {
				KPrint("Received End\n");
				break;
			}

			if(message_type == KSync::Messages::Reply) {
				KPrint("Received Reply: (%s)\n", unwrapped_message.c_str());
			}
		}
	}

	KPrint("Quitting.\n");
	
	//Close the connection;

	close(sockfd);

	return 0;
}
