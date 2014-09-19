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

#ifndef KSYNC_OPTPARSE_HDR
#define KSYNC_OPTPARSE_HDR

#include <vector>
#include <string>

namespace KSync {
	class Option {
		public:
			//Type management
			typedef int Type_t;
			const Type_t Bool = 0;
			const Type_t Str = 1;
			const Type_t Int = 2;
			const Type_t Float = 3;

			//Mode management
			typedef int Mode_t;
			const Mode_t Single = 0;
			const Mode_t Multiple = 1;

		public:
			Option(const std::string& call_name, const std::string& help_text, bool* option);
			Option(const std::string& call_name, const std::string& help_text, std::vector<bool>* options);
			Option(const std::string& call_name, const std::string& help_text, std::string* option);
			Option(const std::string& call_name, const std::string& help_text, std::vector<std::string>* options);
			Option(const std::string& call_name, const std::string& help_text, int* options);
			Option(const std::string& call_name, const std::string& help_text, std::vector<int>* options);
			Option(const std::string& call_name, const std::string& help_text, double* options);
			Option(const std::string& call_name, const std::string& help_text, std::vector<double>* options);
			Option(const std::string& call_name, const Type_t& Type, const Mode_t& Mode, const std::string& help_text, void* options);

			//Getters/Setters
			std::string GetHelpText();

			bool IsOption(const char* opt) __attribute__((warn_unused_result));
			bool NeedsArgument();
			std::string GetName(size_t i=0);
			size_t GetNum() {
				return call_names.size();
			}

			static bool IsArgumentInt(const char* optarg) __attribute__((warn_unused_result));
			static bool IsArgumentFloat(const char* optarg) __attribute__((warn_unused_result));
			int SetValue(const char* optarg) __attribute__((warn_unused_result));

			static std::vector<std::string> GetCallNames(const std::string& combined_names);
			
		private:
			std::vector<std::string> call_names;
			Type_t type;
			Mode_t mode;
			std::string help_text;
			void* value;
	};

	class ArgParser {
		public:
			ArgParser(const std::string& help_intro);

			void AddOption(Option* option);

			void PrintHelp();

			int ParseArgs(int argc, char** argv) __attribute__((warn_unused_result));

		private:
			std::string help_intro;
			std::vector<Option*> options;
	};
}

#endif
