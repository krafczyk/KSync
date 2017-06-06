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
#include <limits>

#include <cstring>

#include "ksync/logging.h"
#include "ksync/messages.h"
#include "ksync/comm/object.h"

namespace KSync {
	namespace Comm {
		const Type_t YieldType = std::numeric_limits<Type_t>::max();
		const Type_t CommunicableObject::Type = 0;
		const Type_t SimpleCommunicableObject::Type = 1;
		const Type_t CommData::Type = 2;
		const Type_t CommString::Type = 3;
		const Type_t GatewaySocketInitializationRequest::Type = 4;
		const Type_t GatewaySocketInitializationChangeId::Type = 5;
		const Type_t ClientSocketCreation::Type = 6;
		const Type_t SocketConnectHerald::Type = 7;
		const Type_t SocketConnectAcknowledge::Type = 8;
		const Type_t ShutdownRequest::Type = 9;
		const Type_t ShutdownAck::Type = 10;
		const Type_t ServerShuttingDown::Type = 11;
		const Type_t ExecuteCommand::Type = 12;
		const Type_t CommandOutput::Type = 13;

		const char* GetTypeName(const Type_t type) {
			if (type == CommunicableObject::Type) {
				return "Basic CommunicableObject";
			} else if (type == SimpleCommunicableObject::Type) {
				return "Simple CommunicableObject";
			} else if (type == CommData::Type) {
				return "Data";
			} else if (type == CommString::Type) {
				return "String";
			} else if (type == GatewaySocketInitializationRequest::Type) {
				return "GatewaySocketInitializationRequest";
			} else if (type == GatewaySocketInitializationChangeId::Type) {
				return "GatewaySocketInitializationChangeId";
			} else if (type == SocketConnectHerald::Type) {
				return "SocketConnectHerald";
			} else if (type == SocketConnectAcknowledge::Type) {
				return "SocketConnectAcknowledge";
			} else if (type == ShutdownRequest::Type) {
				return "ShutdownRequest";
			} else if (type == ShutdownAck::Type) {
				return "ShutdownAck";
			} else if (type == ServerShuttingDown::Type) {
				return "ServerShuttingDown";
			} else {
				LOGF(SEVERE, "Here (%i)\n", type);
				throw TypeException(type);
			}
		}

		TypeException::TypeException(Type_t type) {
			std::stringstream ss;
			ss << "Type (" << type << ") is not a valid type!";
			SetMessage(ss.str());
		}

		void CheckTypeCompatibility(const Type_t typea, const Type_t typeb) {
			if(typea != typeb) {
				LOGF(SEVERE, "Typea: %u Typeb: %u\n", typea, typeb);
				throw TypeException(typea);
			}
		}

		template<class T> void CommCreator(std::shared_ptr<T>& message, const std::shared_ptr<CommObject>& comm_obj) {
			CheckTypeCompatibility(comm_obj->GetType(), T::Type);
			message.reset(new T(comm_obj));
		}

		CommunicableObject::CommunicableObject(const std::shared_ptr<CommObject>& comm_obj) {
			if(comm_obj->UnPack() < 0) {
				throw CommObject::UnPackException(comm_obj->GetType());
			}
		}

		std::shared_ptr<CommObject> SimpleCommunicableObject::GetCommObject() {
			return std::shared_ptr<CommObject>(new CommObject(0, 0, false, this->GetType()));
		}

		CommData::CommData(const std::shared_ptr<CommObject>& comm_obj) : CommunicableObject(comm_obj){
			this->data = new char[comm_obj->GetDataSize()];
			memcpy(this->data, comm_obj->GetDataPointer(), comm_obj->GetDataSize());
			this->size = comm_obj->GetDataSize();
		}

		CommData::~CommData() {
			if (this->data != 0) {
				delete this->data;
			}
		}
		std::shared_ptr<CommObject> CommData::GetCommObject() {
			return std::shared_ptr<CommObject>(new CommObject(this->data, this->size, false, this->GetType()));
		}

		CommString::CommString(const std::shared_ptr<CommObject>& comm_obj) : CommunicableObject(comm_obj) {
			this->clear();
			this->reserve(comm_obj->GetDataSize());
			for(size_t i=0; i < comm_obj->GetDataSize(); ++i) {
				this->push_back(comm_obj->GetDataPointer()[i]);
			}
		}

		std::shared_ptr<CommObject> CommString::GetCommObject() {
			size_t size = this->size();
			char* data = new char[size];
			memcpy(data, this->c_str(), size);
			std::shared_ptr<CommObject> new_obj(new CommObject(data, size, false, this->GetType()));
			delete[] data;
			return new_obj;
		}

		CommStringArray::CommStringArray(const std::shared_ptr<CommObject>& comm_obj) : CommunicableObject(comm_obj) {
			const char* data = comm_obj->GetDataPointer();
			size_t d_i = 0;
			const size_t n_s = *((size_t*)(data+d_i));
			d_i += sizeof(size_t);
			this->resize(n_s);
			for(size_t s_i = 0; s_i < n_s; ++s_i) {
				const size_t s_i_n = *((size_t*)(data+d_i));
				d_i += sizeof(size_t);
				(*this)[s_i].assign(data+d_i, s_i_n);
				d_i += s_i_n;
			}
		}

		std::shared_ptr<CommObject> CommStringArray::GetCommObject() {
			size_t total_new_size = 0;
			total_new_size += sizeof(size_t);
			for(size_t s_i = 0; s_i < this->size(); ++s_i) {
				total_new_size += sizeof(size_t);
				total_new_size += (*this)[s_i].size();
			}
			char* new_data = new char[total_new_size];
			size_t d_i = 0;
			//Write number of strings
			*((size_t*)(new_data + d_i)) = this->size();
			d_i += sizeof(size_t);
			for(size_t s_i = 0; s_i < this->size(); ++s_i) {
				//Write strings
				*((size_t*)(new_data + d_i)) = (*this)[s_i].size();
				d_i += sizeof(size_t);
				memcpy(new_data+d_i, (*this)[s_i].data(), (*this)[s_i].size());
				d_i += (*this)[s_i].size();
			}
			std::shared_ptr<CommObject> new_obj(new CommObject(new_data, total_new_size, false, this->GetType()));
			delete[] new_data;
			return new_obj;
		}

		GatewaySocketInitializationRequest::GatewaySocketInitializationRequest(const std::shared_ptr<CommObject>& comm_obj) : CommunicableObject(comm_obj) {
			this->ClientId = ((Utilities::client_id_t*) comm_obj->GetDataPointer())[0];
		}

		std::shared_ptr<CommObject> GatewaySocketInitializationRequest::GetCommObject() {
			char* new_data = new char[sizeof(Utilities::client_id_t)];
			((Utilities::client_id_t*) new_data)[0] = this->ClientId;
			size_t size = sizeof(Utilities::client_id_t);
			std::shared_ptr<CommObject> new_obj(new CommObject(new_data, size, false, this->GetType()));
			delete[] new_data;
			return new_obj;
		}

		ClientSocketCreation::ClientSocketCreation() {
			this->resize(2);
		}

		CommandOutput::CommandOutput(const std::shared_ptr<CommObject>& comm_obj) : CommunicableObject(comm_obj) {
			const char* data = comm_obj->GetDataPointer();
			size_t d_i = 0;
			size_t s_n = *((size_t*)(data+d_i));
			d_i += sizeof(size_t);
			this->std_out.assign(data+d_i, s_n);
			d_i += s_n;
			s_n = *((size_t*)(data+d_i));
			d_i += sizeof(size_t);
			this->std_err.assign(data+d_i, s_n);
			d_i += s_n;
			this->return_code = *((KSync::Commanding::ExecutionContext::Return_t*) (data+d_i));
		}

		std::shared_ptr<CommObject> CommandOutput::GetCommObject() {
			size_t total_new_size = 0;
			total_new_size += sizeof(size_t);
			total_new_size += this->std_out.size();
			total_new_size += sizeof(size_t);
			total_new_size += this->std_err.size();
			total_new_size += sizeof(this->return_code);

			char* new_data = new char[total_new_size];
			size_t d_i = 0;

			//Write strings
			*((size_t*)(new_data + d_i)) = this->std_out.size();
			d_i += sizeof(size_t);
			memcpy(new_data+d_i, this->std_out.data(), this->std_out.size());
			d_i += this->std_out.size();
			*((size_t*)(new_data + d_i)) = this->std_err.size();
			d_i += sizeof(size_t);
			memcpy(new_data+d_i, this->std_err.data(), this->std_err.size());
			d_i += this->std_err.size();
			*((KSync::Commanding::ExecutionContext::Return_t*) (new_data+d_i)) = this->return_code;

			std::shared_ptr<CommObject> new_obj(new CommObject(new_data, total_new_size, false, this->GetType()));
			delete[] new_data;
			return new_obj;
		}

		template void CommCreator(std::shared_ptr<SimpleCommunicableObject>& message, const std::shared_ptr<CommObject>& comm_obj);
		template void CommCreator(std::shared_ptr<CommData>& message, const std::shared_ptr<CommObject>& comm_obj);
		template void CommCreator(std::shared_ptr<CommString>& message, const std::shared_ptr<CommObject>& comm_obj);
		template void CommCreator(std::shared_ptr<GatewaySocketInitializationRequest>& message, const std::shared_ptr<CommObject>& comm_obj);
		template void CommCreator(std::shared_ptr<GatewaySocketInitializationChangeId>& message, const std::shared_ptr<CommObject>& comm_obj);
		template void CommCreator(std::shared_ptr<ClientSocketCreation>& message, const std::shared_ptr<CommObject>& comm_obj);
		template void CommCreator(std::shared_ptr<SocketConnectHerald>& message, const std::shared_ptr<CommObject>& comm_obj);
		template void CommCreator(std::shared_ptr<SocketConnectAcknowledge>& message, const std::shared_ptr<CommObject>& comm_obj);
		template void CommCreator(std::shared_ptr<ShutdownRequest>& message, const std::shared_ptr<CommObject>& comm_obj);
		template void CommCreator(std::shared_ptr<ShutdownAck>& message, const std::shared_ptr<CommObject>& comm_obj);
		template void CommCreator(std::shared_ptr<ServerShuttingDown>& message, const std::shared_ptr<CommObject>& comm_obj);
		template void CommCreator(std::shared_ptr<ExecuteCommand>& message, const std::shared_ptr<CommObject>& comm_obj);
		template void CommCreator(std::shared_ptr<CommandOutput>& message, const std::shared_ptr<CommObject>& comm_obj);
	}
}
