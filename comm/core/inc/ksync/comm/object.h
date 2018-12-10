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
				class CommObjectConstructorException : public KSync::Exception::BasicException {
					public:
						CommObjectConstructorException();
				};

				typedef short unsigned int message_id_t;

				static message_id_t GenMessageId();

				CommObject(const char* data, const size_t size, const bool pre_packed, const Comm::Type_t type = Comm::CommunicableObject::Type, const message_id_t reply_id = 0);
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
				message_id_t GetMessageId() const  {
					return this->message_id;
				}
				message_id_t GetReplyId() const {
					return this->reply_id;
				}


			private:
				bool packed;
				Comm::Type_t type;
				message_id_t message_id;
				message_id_t reply_id;
				char crc;
				char* data;
				size_t size;
		};
	}
}

#endif
