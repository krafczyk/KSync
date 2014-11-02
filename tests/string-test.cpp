#include <time.h>
#include <string>
#include <vector>

#include "ksync/messages.h"

#include "ArgParse/ArgParse.h"

int main(int argc, char** argv) {
	int num = 20;
	int length = 30;
	int seed = 0;
	bool time_rand = false;
	int debug = 0;

	ArgParse::Option NumOption("N/num", "Number of strings to test", &num);
	ArgParse::Option LengthOption("L/len", "Length of strings to produce", &length);
	ArgParse::Option SeedOption("S/seed", "Seed to start random number generator with", &seed);
	ArgParse::Option RandOption("R/rand", "Seed is the unix time, overrides -S/--seed", &time_rand);
	ArgParse::Option DebugOption("d", "Debug level", &debug);

	ArgParse::ArgParser Parser("Unit test for string escaping and unescaping");

	Parser.AddOption(&NumOption);
	Parser.AddOption(&LengthOption);
	Parser.AddOption(&SeedOption);
	Parser.AddOption(&RandOption);
	Parser.AddOption(&DebugOption);

	if(Parser.ParseArgs(argc, argv) < 0) {
		return -1;
	}
	if(Parser.HelpPrinted()) {
		return 0;
	}

	if(time_rand) {
		srand(time(0));
	} else {
		srand(seed);
	}

	int n_success = 0;

	for(int i=0;i< num;++i) {
		std::string gen_string = KSync::Messages::GenRandomString(length);
		if(debug > 0) {
			printf("Generated the string (%s)\n", gen_string.c_str());
		}
		std::string escaped_string;
		if(KSync::Messages::EscapeString(escaped_string, gen_string) < 0) {
			if(debug > 0) {
				printf("Problem escaping the string escaped: (%s)\n", escaped_string.c_str());
			}
			continue;
		}
		std::string unescaped_string;
		if(KSync::Messages::UnEscapeString(unescaped_string, escaped_string) < 0) {
			if(debug > 0) {
				printf("Problem unescaping the string unescaped: (%s)\n", unescaped_string.c_str());
			}
			continue;
		}

		if(gen_string != unescaped_string) {
			if(debug > 0) {
				printf("The transfromed string does not equal the original string.\n");
			}
		} else {
			++n_success;
			if(debug > 0) {
				printf("String was transformed successfully.\n");
			}
		}
	}
	printf("%i/%i strings transformed successfully.\n", n_success, num);

	return 0;
}
