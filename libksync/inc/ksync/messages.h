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
#include <memory>

#include "ksync/utilities.h"
#include "ksync/ksync_exception.h"

namespace KSync {
	namespace Comm {
		class CommObject;
		typedef uint8_t Type_t;
		extern const Type_t YieldType;
		const char* GetTypeName(const Type_t type);
		class TypeException : public KSync::Exception::BasicException {
			public:
				TypeException(Type_t type);
		};

		void CheckTypeCompatibility(const Type_t typea, const Type_t typeb);

		template<class T> void CommCreator(std::shared_ptr<T>& message, const std::shared_ptr<CommObject>& comm_obj);

		class CommunicableObject {
			public:
				static const Type_t Type;

				CommunicableObject() {};
				CommunicableObject(const std::shared_ptr<CommObject>& comm_obj);
				virtual std::shared_ptr<CommObject> GetCommObject() = 0;
				virtual Type_t GetType() const {
					return this->Type;
				}
		};

		class SimpleCommunicableObject : public CommunicableObject {
			public:
				static const Type_t Type;

				SimpleCommunicableObject() {};
				SimpleCommunicableObject(const std::shared_ptr<CommObject>& comm_obj) : CommunicableObject(comm_obj) {};
				virtual std::shared_ptr<CommObject> GetCommObject();
				virtual Type_t GetType() const {
					return this->Type;
				}
		};

		class CommData : public CommunicableObject {
			public:
				static const Type_t Type;

				CommData(char* data, size_t size) {
					this->data = data;
					this->size = size;
				}
				~CommData();

				CommData(const std::shared_ptr<CommObject>& comm_obj);
				std::shared_ptr<CommObject> GetCommObject();
				virtual Type_t GetType() const {
					return this->Type;
				}
			private:
				char* data;
				size_t size;
		};

		class CommString : public CommunicableObject, public std::string {
			public:
				static const Type_t Type;

				CommString() {};
				CommString(const std::string& in) : std::string(in) {};
				CommString(const std::shared_ptr<CommObject>& comm_obj);
				std::shared_ptr<CommObject> GetCommObject();
				virtual Type_t GetType() const {
					return this->Type;
				}
		};

		class GatewaySocketInitializationRequest : public CommunicableObject {
			public:
				static const Type_t Type;
				GatewaySocketInitializationRequest(Utilities::client_id_t id) {
					this->ClientId = id;
				}
				GatewaySocketInitializationRequest(const std::shared_ptr<CommObject>& comm_obj);
				std::shared_ptr<CommObject> GetCommObject();
				virtual Type_t GetType() const {
					return this->Type;
				}
				Utilities::client_id_t GetClientId() const {
					return this->ClientId;
				}
			private:
				Utilities::client_id_t ClientId;
		};

		class GatewaySocketInitializationChangeId : public SimpleCommunicableObject {
			public:
				static const Type_t Type;
				GatewaySocketInitializationChangeId() {};
				GatewaySocketInitializationChangeId(const std::shared_ptr<CommObject>& comm_obj) : SimpleCommunicableObject(comm_obj) {};
				virtual Type_t GetType() const {
					return this->Type;
				}
		};

		class ClientSocketCreation : public CommunicableObject {
			public:
				static const Type_t Type;
				ClientSocketCreation() {};
				ClientSocketCreation(const std::shared_ptr<CommObject>& comm_obj);
				std::shared_ptr<CommObject> GetCommObject();
				virtual Type_t GetType() const {
					return this->Type;
				}
				const std::string& GetBroadcastUrl() {
					return this->broadcast_url;
				}
				void SetBroadcastUrl(const std::string& in) {
					this->broadcast_url = in;
				}
				const std::string& GetClientUrl() {
					return this->client_url;
				}
				void SetClientUrl(const std::string& in) {
					this->client_url = in;
				}
			private:
				std::string broadcast_url;
				std::string client_url;
		};

		class SocketConnectHerald : public SimpleCommunicableObject {
			public:
				static const Type_t Type;
				SocketConnectHerald() {};
				SocketConnectHerald(const std::shared_ptr<CommObject>& comm_obj) : SimpleCommunicableObject(comm_obj) {};
				virtual Type_t GetType() const {
					return this->Type;
				}
		};

		class SocketConnectAcknowledge : public SimpleCommunicableObject {
			public:
				static const Type_t Type;
				SocketConnectAcknowledge() {};
				SocketConnectAcknowledge(const std::shared_ptr<CommObject>& comm_obj) : SimpleCommunicableObject(comm_obj) {};
				virtual Type_t GetType() const {
					return this->Type;
				}
		};

		class ServerShuttingDown : public SimpleCommunicableObject {
			public:
				static const Type_t Type;
				ServerShuttingDown() {};
				ServerShuttingDown(const std::shared_ptr<CommObject>& comm_obj) : SimpleCommunicableObject(comm_obj) {};
				virtual Type_t GetType() const {
					return this->Type;
				}
		};
	}
}

#endif
