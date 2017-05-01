#include <cstring>

#include "ksync/comm_system_object.h"

namespace KSync {
	namespace Comm {
		CommObject::CommObject(char* data, size_t size) {
			this->data = data;
			this->size = size;
		}

		CommObject::CommObject(const std::string& in) {
			this->size = in.size();
			this->data = new char[this->size];
			memcpy(this->data, in.c_str(), this->size);
		}

		CommObject::~CommObject() {
			if (data != 0) {
				delete data;
			}
			size = 0;
		}

		void CommObject::RepackWithCRC() {
			char crc = GenCRC8((char*) this->data, this->size);
			char* new_data = new char[size+1];
			memcpy(new_data, this->data, this->size);
			new_data[size] = crc;
			delete this->data;
			this->data = new_data;
			this->size = size+1;
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

		bool CommObject::CheckCRC() {
			char crc = GenCRC8((char*) this->data, this->size-1);
			if (this->data[this->size] == crc ) {
				return true;
			} else {
				return false;
			}
		}

		int CommObject::GetString(std::string& out) const {
			out.clear();
			out.reserve(this->size);
			for(size_t i=0; i < this->size; ++i) {
				out.push_back(this->data[i]);
			}
			return 0;
		}
	}
}
