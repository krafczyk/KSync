#KSync - Client-Server synchronization system using rsync.
#Copyright (C) 2014  Matthew Scott Krafczyk

#This program is free software: you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation, either version 2 of the License, or
#(at your option) any later version.

#This program is distributed in the hope that it will be useful,
#WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.

#You should have received a copy of the GNU General Public License
#along with this program.  If not, see <http://www.gnu.org/licenses/>.

find_package(G3LOG REQUIRED)
include_directories(${libksync_INCLUDE_DIR})
include_directories(${G3LOG_INCLUDE_DIRS})

add_library (ksync SHARED src/logging.cxx src/messages.cxx src/comm_system_interface.cxx src/comm_system_object.cxx src/command_system_interface.cxx src/pstreams_command_system.cxx src/utilities.cxx)

install (TARGETS ksync DESTINATION lib)
install (DIRECTORY inc/ksync DESTINATION include FILES_MATCHING PATTERN "*.h")
