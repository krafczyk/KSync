#ifndef KSYNC_COMM_SYSTEM_OBJ_HDR
#define KSYNC_COMM_SYSTEM_OBJ_HDR

#include "ksync/types.h"

#define CRC8 0x9B

namespace KSync {
	namespace Comm {
		class CommObject {
			public:
				CommObject(char* data, size_t size);
				CommObject(const std::string& in);
				~CommObject();

				void RepackWithCRC();
				char GenCRC8(const char* data, const size_t size);
				bool CheckCRC();

				int GetString(std::string& out) const __attribute__((warn_unused_result));

				char* data;
				size_t size;
		};
	}
}

#endif
