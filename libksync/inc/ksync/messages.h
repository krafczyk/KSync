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

#ifndef KSYNC_MESSAGES_HDR
#define KSYNC_MESSAGES_HDR

#include <string>

namespace KSync {
	namespace Messages {
		typedef unsigned int Message_t;
		extern const Message_t Command;
		extern const Message_t Reply;
		extern const Message_t End;
		extern const Message_t Quit;

		void CreateEnd(std::string& end);
		void CreateQuit(std::string& quit);
		int WrapAsCommand(std::string& command, const std::string& in) __attribute__((warn_unused_result));
		int WrapAsReply(std::string& reply, const std::string& in) __attribute__((warn_unused_result));
		int UnWrapMessage(std::string& unwrapped_message, Message_t& Message_Type, const std::string& message) __attribute__((warn_unused_result));
	}
}

#endif
