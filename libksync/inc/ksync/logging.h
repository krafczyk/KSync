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

#ifndef KSYNC_LOGGING_HDR
#define KSYNC_LOGGING_HDR

#include <string>
#include <memory>

#include "g3log/g3log.hpp"
#include "g3log/logworker.hpp"

namespace KSync {
	void InitializeLogger(std::unique_ptr<g3::LogWorker>& logworker, const bool echo_to_std, const std::string& log_prefix, const std::string& log_file_dir);
}

const LEVELS MESSAGE { (INFO.value+DEBUG.value)/2, {""}};
const LEVELS SEVERE { (FATAL.value-1), {"SEVERE"}};

#endif
