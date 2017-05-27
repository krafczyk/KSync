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

#include <future>

#include "ksync/logging.h"

namespace KSync {
	class KSyncSink : public g3::FileSink {
		public:
			enum FG_Color {
				RED = 31,
				GRN = 32,
				YEL = 33,
				BLU = 34,
				MAG = 35,
				CYN = 36,
				WHT = 37,
				BLK = 0,
			};

			FG_Color GetColor(const LEVELS level) const {
				if (level.value == MESSAGE.value) {
					return GRN;
				} else if (level.value == INFO.value) {
					return BLK;
				} else if (level.value == WARNING.value) {
					return YEL;
				} else if (level.value == DEBUG.value) {
					return BLU;
				} else if (level.value == SEVERE.value) {
					return RED;
				} else if (level.value == FATAL.value) {
					return MAG;
				} else {
					return WHT;
				}
			}

			KSyncSink(const bool echo_to_std, const std::string& log_prefix, const std::string& log_directory, const std::string& logger_id="ksync") : FileSink(log_prefix, log_directory, logger_id) { this->echo_to_std = echo_to_std;};
			virtual ~KSyncSink() {};
			void ReceiveMessage(g3::LogMessageMover message);
		private:
			bool echo_to_std;
	};

	void KSyncSink::ReceiveMessage(g3::LogMessageMover message) {
		if(echo_to_std) {
			fprintf(stdout, "\x1B[%im%s\x1B[0m", GetColor(message.get()._level), message.get().toString().c_str());
		}
		this->fileWrite(message);
	}

	void InitializeLogger(std::unique_ptr<g3::LogWorker>& logworker, std::string& logfile_name,  const bool echo_to_std, const std::string& log_prefix, const std::string& log_file_dir) {
		logworker = g3::LogWorker::createLogWorker();
		//auto handle = logworker->addDefaultLogger(log_prefix, log_file_dir);
		auto handle = logworker->addSink(std2::make_unique<KSyncSink>(echo_to_std, log_prefix, log_file_dir), &KSyncSink::ReceiveMessage);
		g3::initializeLogging(logworker.get());
		std::future<std::string> log_file_name = handle->call(&KSyncSink::fileName);
		logfile_name = log_file_name.get();
		fprintf(stdout, "Now printing to log file (%s)\n", log_file_name.get().c_str());
	}
}
