#include <string>
#include <memory>
#include <unistd.h>
#include <chrono>

#include "curses.h"
#include "panel.h"
#include "menu.h"

#include "ArgParse/ArgParse.h"
#include "ksync/utilities.h"
#include "ksync/logging.h"
#include "ksync/ui/ncurses/window.h"

//gui thread
int main(int argc, char** argv) {
	std::string log_dir;
	std::string gateway_socket_url;
	bool gateway_socket_url_defined;
	bool nanomsg;

	ArgParse::ArgParser arg_parser("KSync Server - Client side of a Client-Server synchonization system using rsync.");
	KSync::Utilities::set_up_common_arguments_and_defaults(arg_parser, log_dir, gateway_socket_url, gateway_socket_url_defined, nanomsg);

	if(arg_parser.ParseArgs(argc, argv) < 0) {
		printf("Problem parsing arguments\n");
		arg_parser.PrintHelp();
		return -1;
	}

	if(arg_parser.HelpPrinted()) {
		return 0;
	}

	if (log_dir == "") {
		if(KSync::Utilities::get_user_ksync_dir(log_dir) < 0) {
			printf("There was a problem getting the ksync user directory!\n");
			return -2;
		}
	}

	//Initialize logging:
	std::unique_ptr<g3::LogWorker> logworker;
	KSync::InitializeLogger(logworker, false, "KSync NCurses Client", log_dir);

	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	refresh();

	KSync::Ui::NCursesWindow test_window(10, 20, 10, 10);
	KSync::Ui::NCursesWindow test_window_2(10, 20, 10, 10);

	int c = 0;
	do {
		// Key cycle
		switch(c) {
			case KEY_LEFT: {
				test_window.MVwin(0, -1);
				break;
			}
			case KEY_RIGHT: {
				test_window.MVwin(0, 1);
				break;
			}
			case KEY_UP: {
				test_window.MVwin(-1, 0);
				break;
			}
			case KEY_DOWN: {
				test_window.MVwin(1, 0);
				break;
			}
		}
	//	LOGF(INFO, "Start char(%i)", c);
		// Draw cycle
		test_window_2.clear();
		test_window_2.draw_border();
		test_window.clear();
		test_window.draw_border();
		//test_window.print(1,1,"test");
		//test_window.print_left_justified(1, 1, 4, "test");
		test_window.print_left_justified(1, 1, 18, "test test");
		test_window.print_right_justified(2, 1, 18, "test test");
		test_window.print_center_justified(3, 1, 18, "test test");
		refresh();
	} while ((c = getch()) != 'q');

	endwin();
	return 0;
}
