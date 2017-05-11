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
#include "ksync/comm_system_object.h"
#include "ksync/logging.h"

namespace KSync {
	namespace Comm {
		const Type_t CommunicableObject::Type = 0;
		const Type_t CommData::Type = 1;
		const Type_t CommString::Type = 2;
		const Type_t GatewaySocketInitializationRequest::Type = 3;
		const Type_t GatewaySocketInitializationChangeId::Type = 4;

		const char* GetTypeName(const Type_t type) {
			if (type == CommunicableObject::Type) {
				return "Basic CommunicableObject";
			} else  if (type == CommData::Type) {
				return "Data";
			} else if (type == CommString::Type) {
				return "String";
			} else if (type == GatewaySocketInitializationRequest::Type) {
				return "GatewaySocketInitializationRequest";
			} else if (type == GatewaySocketInitializationChangeId::Type) {
				return "GatewaySocketInitializationChangeId";
			} else {
				throw TypeException(type);
			}
		}

		TypeException::TypeException(Type_t type) {
			std::stringstream ss;
			ss << "Type (" << type << ") is not a valid type!";
			SetMessage(ss.str());
		}

		CommData::CommData(CommObject* comm_obj) {
			if (comm_obj->GetType() != this->Type) {
				throw TypeException(comm_obj->GetType());
			}
			if (comm_obj->UnPack() < 0) {
				throw CommObject::UnPackException(comm_obj->GetType());
			}
			this->data = new char[comm_obj->GetDataSize()];
			memcpy(this->data, comm_obj->GetDataPointer(), comm_obj->GetDataSize());
			this->size = comm_obj->GetDataSize();
		}

		CommData::~CommData() {
			if (this->data != 0) {
				delete this->data;
			}
		}
		CommObject* CommData::GetCommObject() {
			CommObject* new_obj = new CommObject(this->data, this->size, false, this->Type);
			return new_obj;
		}

		CommString::CommString(CommObject* comm_obj) {
			if (comm_obj->GetType() != this->Type) {
				throw TypeException(comm_obj->GetType());
			}
			if(comm_obj->UnPack() < 0) {
				throw CommObject::UnPackException(comm_obj->GetType());
			}
			this->clear();
			this->reserve(comm_obj->GetDataSize());
			for(size_t i=0; i < comm_obj->GetDataSize(); ++i) {
				this->push_back(comm_obj->GetDataPointer()[i]);
			}
		}

		CommObject* CommString::GetCommObject() {
			size_t size = this->size();
			char* data = new char[size];
			memcpy(data, this->c_str(), size);
			CommObject* new_obj = new CommObject(data, size, false, this->Type);
			delete[] data;
			return new_obj;
		}

		GatewaySocketInitializationRequest::GatewaySocketInitializationRequest(CommObject* comm_obj) {
			if (comm_obj->GetType() != this->Type) {
				throw TypeException(comm_obj->GetType());
			}
			if(comm_obj->UnPack() < 0) {
				throw CommObject::UnPackException(comm_obj->GetType());
			}
			this->ClientId = ((Utilities::client_id_t*) comm_obj->GetDataPointer())[0];
		}

		CommObject* GatewaySocketInitializationRequest::GetCommObject() {
			char* new_data = new char[sizeof(Utilities::client_id_t)];
			((Utilities::client_id_t*) new_data)[0] = this->ClientId;
			size_t size = sizeof(Utilities::client_id_t);
			CommObject* new_obj = new CommObject(new_data, size, false, this->Type);
			delete[] new_data;
			return new_obj;
		}

		GatewaySocketInitializationChangeId::GatewaySocketInitializationChangeId(CommObject* comm_obj) {
			if (comm_obj->GetType() != this->Type) {
				throw TypeException(comm_obj->GetType());
			}
			if(comm_obj->UnPack() < 0) {
				throw CommObject::UnPackException(comm_obj->GetType());
			}
		}

		CommObject* GatewaySocketInitializationChangeId::GetCommObject() {
			CommObject* new_obj = new CommObject(0, 0, false, this->Type);
			return new_obj;
		}
	}
}
