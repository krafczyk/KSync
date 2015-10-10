#include <nanomsg/nn.h>

#include "ksync/utilities.h"
#include "ksync/logging.h"

namespace KSync {
	namespace Utilities {
		void reset_error() {
			errno = 0;
		}
		int check_error() {
			if(nn_errno() != 0) {
				Error("An error occured! %i (%s)\n", nn_errno(), nn_strerror(nn_errno()));
				return nn_errno();
			}
			return 0;
		}
	}
}
