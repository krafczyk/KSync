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

set(libksync_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/inc" CACHE INTERNAL "ksync include dir")

include_directories(${libksync_INCLUDE_DIR} ${libnanomsg_INCLUDEDIR})

add_library (ksync SHARED src/messages.cxx src/logging.cxx src/socket_ops.cxx)

install (TARGETS ksync DESTINATION lib)
install (DIRECTORY inc/ksync DESTINATION include FILES_MATCHING PATTERN "*.h")
