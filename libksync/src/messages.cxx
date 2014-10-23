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

#include <sstream>
#include <map>

#include <cstring>

#include "ksync/messages.h"
#include "ksync/logging.h"

namespace KSync {
	namespace Messages {
		const Message_t Command = 1;
		const Message_t Reply = 2;
		const Message_t End = 3;
		const Message_t Quit = 4;

		const char* Command_Message = "Command";
		const char* Reply_Message = "Reply";
		const std::string End_Message = "End";
		const std::string Quit_Message = "Quit";

		void CreateEnd(std::string& end) {
			std::stringstream ss;
			ss << End_Message << " " << std::endl;
			end = ss.str();
		}

		void CreateQuit(std::string& quit) {
			std::stringstream ss;
			ss << Quit_Message << " " << std::endl;
			quit = ss.str();
		}

		int WrapAsCommand(std::string& command, const std::string& in) {
			std::stringstream ss;
			ss << Command_Message << " " << in << std::endl;
			command = ss.str();
			return 0;
		}
		int WrapAsReply(std::string& reply, const std::string& in) {
			std::stringstream ss;
			ss << Reply_Message << " " << in << std::endl;
			reply = ss.str();
			return 0;
		}
		int UnWrapMessage(std::string& unwrapped_message, Message_t& Message_Type, const std::string& message) {
			std::stringstream ss;
			size_t control_size;

			ss.str("");
			ss << End_Message << " ";
			control_size = ss.str().size();
			if(strncmp(message.c_str(), ss.str().c_str(), control_size) == 0) {
				KPrint("Detect End\n");
				Message_Type = End;
				return 0;
			}

			ss.str("");
			ss << Quit_Message << " ";
			control_size = ss.str().size();
			if(strncmp(message.c_str(), ss.str().c_str(), control_size) == 0) {
				KPrint("Detect Quit\n");
				Message_Type = Quit;
				return 0;
			}

			ss.str("");
			ss << Command_Message << " ";
			control_size = ss.str().size();
			if(strncmp(message.c_str(), ss.str().c_str(), control_size) == 0) {
				KPrint("Detect Command\n");
				size_t buffer_size = message.size()-control_size+2;
				//Detected a command message
				char temp_buffer[buffer_size];
				bzero(temp_buffer, buffer_size);
				strcpy(temp_buffer, &(message.c_str()[control_size]));
				unwrapped_message = std::string(temp_buffer);
				Message_Type = Command;
				return 0;
			}
			ss.str("");
			ss << Reply_Message << " ";
			control_size = ss.str().size();
			if(strncmp(message.c_str(), ss.str().c_str(), control_size) == 0) {
				KPrint("Detect Reply\n");
				size_t buffer_size = message.size()-control_size+2;
				//Detected a command message
				char temp_buffer[buffer_size];
				bzero(temp_buffer, buffer_size);
				strcpy(temp_buffer, &(message.c_str()[control_size]));
				unwrapped_message = std::string(temp_buffer);
				Message_Type = Reply;
				return 0;
			}
			return -1;
		}
	}
}
