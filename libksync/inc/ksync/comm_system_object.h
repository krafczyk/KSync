#ifndef KSYNC_COMM_SYSTEM_OBJ_HDR
#define KSYNC_COMM_SYSTEM_OBJ_HDR

#include "ksync/types.h"
#include "ksync/ksync_exception.h"

#define CRC8 0x9B

namespace KSync {
	namespace Comm {

		class CommObject {
			public:
				typedef uint8_t Type_t;
				static const Type_t TypeData = 0;
				static const Type_t TypeString = 1;

				static const char* GetTypeName(const Type_t type);

				class TypeException : public KSync::Exception::BasicException {
					public:
						TypeException(Type_t type);
				};
				class PackException : public KSync::Exception::BasicException {
					public:
						PackException(Type_t type);
				};
				class CRCException : public KSync::Exception::BasicException {
					public:
						CRCException();
				};

				CommObject(const char* data, const size_t size, const bool pre_packed = false, const bool pack = true);
				CommObject(const std::string& in, const bool pack = true);
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

				Type_t GetType() const {
					return this->type;
				}
				int GetData(char*& data, size_t& size) __attribute__((warn_unused_result));
				int GetString(std::string& out) __attribute__((warn_unused_result));

			private:
				bool packed;
				Type_t type;
				char crc;
				char* data;
				size_t size;
		};
	}
}

#endif
