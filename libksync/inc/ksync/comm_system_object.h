#ifndef KSYNC_COMM_SYSTEM_OBJ_HDR
#define KSYNC_COMM_SYSTEM_OBJ_HDR

#include "ksync/types.h"
#include "ksync/ksync_exception.h"
#include "ksync/messages.h"

#define CRC8 0x9B

namespace KSync {
	namespace Comm {
		class CommObject {
			public:
				class PackException : public KSync::Exception::BasicException {
					public:
						PackException(Comm::Type_t type);
				};
				class UnPackException : public KSync::Exception::BasicException {
					public:
						UnPackException(Comm::Type_t type);
				};
				class CRCException : public KSync::Exception::BasicException {
					public:
						CRCException();
				};

				CommObject(const char* data, const size_t size, const bool pre_packed, const Comm::Type_t type = Comm::CommunicableObject::Type);
				~CommObject();

				int Pack() __attribute__((warn_unused_result));
				int UnPack() __attribute__((warn_unused_result));
				static char GenCRC8(const char* data, const size_t size);

				const char* GetDataPointer() const {
					return this->data;
				}
				size_t GetDataSize() const {
					return this->size;
				}

				Comm::Type_t GetType() const {
					return this->type;
				}

			private:
				bool packed;
				Comm::Type_t type;
				char crc;
				char* data;
				size_t size;
		};
	}
}

#endif
