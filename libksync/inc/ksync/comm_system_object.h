#ifndef KSYNC_COMM_SYSTEM_OBJ_HDR
#define KSYNC_COMM_SYSTEM_OBJ_HDR

#include "ksync/types.h"

#define CRC8 0x9B

namespace KSync {
	namespace Comm {
		class CommObject {
			public:
				CommObject(void* data, size_t size);
				~CommObject();

				void RepackWithCRC();
				uint8_t GenCRC8(const uint8_t* data, const size_t size);
				bool CheckCRC();

				void* data;
				size_t size;
		};

		template <class T> int CommObjectTranslator(const T& in, CommObject*& out, const bool crc);
		template <class T> int CommObjectTranslator(CommObject*& in, const T& out, const bool crc);

		template<> int CommObjectTranslator(const std::string& in, CommObject*& out, const bool crc);
		template<> int CommObjectTranslator(CommObject*& in, const std::string& out, const bool crc);
	}
}

#endif
