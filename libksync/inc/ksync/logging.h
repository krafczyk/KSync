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

#define TNRM  "\x1B[0m"
#define TRED  "\x1B[31m"
#define TGRN  "\x1B[32m"
#define TYEL  "\x1B[33m"
#define TBLU  "\x1B[34m"
#define TMAG  "\x1B[35m"
#define TCYN  "\x1B[36m"
#define TWHT  "\x1B[37m"

#define TMSG TGRN
#define TERR TRED
#define TWRN TYEL
#define TDBG TBLU

namespace KSync {
	void Print(const char* format, ...);
}

#define KPrint(format, ...) KSync::Print(format, ##__VA_ARGS__)
#define Message(format, ...) KSync::Print("%s%s-M (%s:%i)%s: " format , TMSG, __PRETTY_FUNCTION__, basename(__FILE__), __LINE__, TNRM, ##__VA_ARGS__)
#define Warning(format, ...) KSync::Print("%s%s-W (%s:%i)%s: " format , TWRN, __PRETTY_FUNCTION__, basename(__FILE__), __LINE__, TNRM, ##__VA_ARGS__)
#define Error(format, ...) KSync::Print("%s%s-E (%s:%i)%s: " format , TERR, __PRETTY_FUNCTION__, basename(__FILE__), __LINE__, TNRM, ##__VA_ARGS__)

#endif
