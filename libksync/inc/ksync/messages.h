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
#include <vector>

#include "ksync/utilities.h"
#include "ksync/ksync_exception.h"

namespace KSync {
	namespace Comm {
		class CommObject;
		typedef uint8_t Type_t;
		const char* GetTypeName(const Type_t type);
		class TypeException : public KSync::Exception::BasicException {
			public:
				TypeException(Type_t type);
		};

		class CommunicableObject {
			public:
				static const Type_t Type;

				CommunicableObject() {};
				CommunicableObject(CommObject* comm_obj __attribute__((unused))) {};
				virtual CommObject* GetCommObject() = 0;
		};

		class SimpleCommunicableObject {
			public:
				static const Type_t Type;

				SimpleCommunicableObject() {};
				SimpleCommunicableObject(CommObject* comm_obj);
				CommObject* GetCommObject();
		};

		class CommData : public CommunicableObject {
			public:
				static const Type_t Type;

				CommData(char* data, size_t size) {
					this->data = data;
					this->size = size;
				}
				~CommData();

				CommData(CommObject* comm_obj);
				CommObject* GetCommObject();
			private:
				char* data;
				size_t size;
		};

		class CommString : public CommunicableObject, public std::string {
			public:
				static const Type_t Type;

				CommString() {};
				CommString(std::string in) : std::string(in) {};
				CommString(CommObject* comm_obj);
				CommObject* GetCommObject();
		};

		class GatewaySocketInitializationRequest : public CommunicableObject {
			public:
				static const Type_t Type;
				GatewaySocketInitializationRequest(Utilities::client_id_t id) {
					this->ClientId = id;
				}
				GatewaySocketInitializationRequest(CommObject* comm_obj);
				CommObject* GetCommObject();
			private:
				Utilities::client_id_t ClientId;
		};

		class GatewaySocketInitializationChangeId : public SimpleCommunicableObject {
			public:
				static const Type_t Type;
				GatewaySocketInitializationChangeId() {};
		};

		class SocketConnectHerald : public SimpleCommunicableObject {
			public:
				static const Type_t Type;
				SocketConnectHerald() {};
		};

		class SocketConnectAcknowledge : public SimpleCommunicableObject {
			public:
				static const Type_t Type;
				SocketConnectAcknowledge() {};
		};
	}
}

#endif
