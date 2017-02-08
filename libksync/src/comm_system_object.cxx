#include "ksync/comm_system_object.h"

namespace KSync {
	namespace Comm {
		CommObject::CommObject(void* data, size_t size) {
			this->data = data;
			this->size = size;
		}

		CommObject::~CommObject() {
			if (data != 0) {
				delete data;
			}
			size = 0;
		}

		void CommObject::RepackWithCRC() {
			uint8_t crc = GenCRC8((uint8_t*) this->data, this->size);
			uint8_t* new_data = new uint8_t[size+1];
			memcpy(new_data, this->data, this->size);
			new_data[size] = crc;
			delete this->data;
			this->data = new_data;
			this->size = size+1;
		}

		uint8_t CommObject::GenCRC8(const uint8_t* data, const size_t size) {
			uint8_t out = 0;
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
			uint8_t crc = 0;
			i = 0x80;
			int j = 0x01;
			for (; i != 0; i >>= 1; j <<= 1) {
				if (i & out) {
					crc |= j;
				}
			}

			return crc;
		}

		bool CommObject::CheckCRC() {
			uint8_t crc = GenCRC8((uint8_t*) this->data, this->size-1);
			if (this->data[this->size] == crc ) {
				return true;
			} else {
				return false;
			}
		}

		template<> int CommObjectTranslator(const std::string& in, CommObject*& out, const bool crc) {
			void* data = new void[in.size()];
			memcpy(data, in.c_str(), in.size());
			out = new CommObject(data, in.size());
			if (crc) {
				out->RepackWithCRC();
			}
			return 0;
		}

		template<> int CommObjectTranslator(CommObject*& in, const std::string& out, const bool crc) {
			size_t new_size = in->size;
			if (crc) {
				new_size--;
			}
			out.reserve(new_size);
			for(size_t i=0; i < new_size; ++i) {
				out.append((char) in->data[i]);
			}
			return 0;
		}
	}
}
