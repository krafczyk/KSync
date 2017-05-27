#include <cstring>
#include <sstream>

#include "ksync/logging.h"
#include "ksync/comm_system_object.h"

namespace KSync {
	namespace Comm {
		CommObject::PackException::PackException(Type_t type) {
			std::stringstream ss;
			ss << "There was a problem packing an object of type (" << GetTypeName(type) << ")";
			SetMessage(ss.str());
		}
		CommObject::UnPackException::UnPackException(Type_t type) {
			std::stringstream ss;
			ss << "There was a problem un packing an object of type (" << GetTypeName(type) << ")";
			SetMessage(ss.str());
		}
		CommObject::CRCException::CRCException() {
			SetMessage("There was a CRC miss-match");
		}
		CommObject::CommObjectConstructorException::CommObjectConstructorException() {
			SetMessage("There was a problem constructing the comm object!");
		}

		CommObject::CommObject(const char* data, const size_t size, const bool pre_packed, const Type_t type) {
			if(data == 0) {
				if (size != 0) {
					KError("Can't use size != 0 with a null data pointer!!\n");
					throw CommObjectConstructorException();
				}
				if (pre_packed) {
					KError("Can't say an object is pre-packed if using a null data input\n");
					throw CommObjectConstructorException();
				}
				this->type = type;
				this->data = 0;
				this->size = 0;
				if(Pack() < 0) {
					throw PackException(this->type);
				}
			} else {
				this->data = new char[size];
				memcpy(this->data, data, size);
				this->size = size;
				if (pre_packed) {
					this->type = ((Type_t*)this->data)[0];
					this->crc = ((char*)this->data)[sizeof(this->type)];
					this->packed = true;
				} else {
					this->type = type;
					if(Pack() < 0) {
						throw PackException(this->type);
					}
				}
			}

		}

		CommObject::~CommObject() {
			if (data != 0) {
				delete[] data;
			}
			size = 0;
		}

		int CommObject::Pack() {
			size_t num_extra_bytes = sizeof(this->type)+sizeof(this->crc);
			char* new_data = new char[num_extra_bytes+this->size];
			if (this->data == 0) {
				this->crc = 0;
				new_data[0] = this->type;
				new_data[sizeof(this->type)] = this->crc;
				this->data = new_data;
				this->size += num_extra_bytes;
				this->packed = true;
			} else {
				this->crc = GenCRC8(this->data, this->size);
				new_data[0] = this->type;
				new_data[sizeof(this->type)] = this->crc;
				memcpy(new_data+num_extra_bytes, this->data, this->size);
				delete[] this->data;
				this->data = new_data;
				this->size += num_extra_bytes;
				this->packed = true;
			}
			return 0;
		}

		int CommObject::UnPack() {
			if (!this->packed) {
				return 0;
			}
			size_t num_extra_bytes = sizeof(this->type)+sizeof(this->crc);
			if (this->size == num_extra_bytes) {
				if (this->crc != 0) {
					return -1;
				}
				delete[] this->data;
				this->data = 0;
			} else {
				char new_crc = GenCRC8(this->data+num_extra_bytes, this->size-num_extra_bytes);
				if (new_crc != this->crc) {
					return -1;
				}
				size_t new_size = this->size-num_extra_bytes;
				char* new_data = new char[this->size-num_extra_bytes];
				memcpy(new_data, this->data+num_extra_bytes, new_size);
				delete[] this->data;
				this->data = new_data;
			}
			this->size -= num_extra_bytes;
			this->packed = false;
			return 0;
		}

		char CommObject::GenCRC8(const char* data, const size_t size) {
			char out = 0;
			int bits_read = 0;
			int bit_flag;

			if (data == 0) {
				return 0;
			}

			size_t temp_size = size;

			while(temp_size > 0) {
				bit_flag = out >> 15;

				/* Get next bit: */
				out <<= 1;
				out |= (*data >> bits_read) & 1; // item a) work from the least significant bits

				/* Increment bit counter: */
				bits_read++;
				if(bits_read > 7) {
					bits_read = 0;
					data++;
					temp_size--;
				}

				/* Cycle check: */
				if (bit_flag)
					out ^= CRC8;
			}

			// item b) "push out" the last 8 bits
			for (int i = 0; i < 8; ++i) {
				bit_flag = out >> 7;
				out <<= 1;
				if (bit_flag) {
					out ^= CRC8;
				}
			}

			// item c) reverse the bits
			char crc = 0;
			int i = 0x80;
			int j = 0x01;
			for (; i != 0; i >>= 1, j <<= 1) {
				if (i & out) {
					crc |= j;
				}
			}

			return crc;
		}
	}
}
