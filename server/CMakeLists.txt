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

add_executable(ksync_server src/server.cpp)
include_directories(inc ${libksync_INCLUDE_DIR})
target_link_libraries(ksync_server ksync)
target_link_libraries(ksync_server ksync_zeromq_module)
target_link_libraries(ksync_server ${ArgParse_LDFLAGS})
target_link_libraries(ksync_server ${libzmq_LDFLAGS})
#target_link_libraries(ksync_server ${nanomsg_LDFLAGS})

install (TARGETS ksync_server DESTINATION bin)
