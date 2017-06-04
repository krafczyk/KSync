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

//ACS Characters
//Upper Left corner:        ACS_ULCORNER
//Upper Right corner:       ACS_URCORNER
//Lower Left corner:        ACS_LLCORNER
//Lower Right corner:       ACS_LRCORNER
//Tee pointing right:       ACS_LTEE
//Tee pointing left:        ACS_RTEE
//Tee pointing up:          ACS_BTEE
//Tee pointing down:        ACS_TTEE
//Horizontal line:          ACS_HLINE
//Vertical line:            ACS_VLINE

class NCursesWindow {
	public:
		const chtype ls = ACS_VLINE;
		const chtype rs = ACS_VLINE;
		const chtype ts = ACS_HLINE;
		const chtype bs = ACS_HLINE;
		const chtype tl = ACS_ULCORNER;
		const chtype tr = ACS_URCORNER;
		const chtype bl = ACS_LLCORNER;
		const chtype br = ACS_LRCORNER;
		const chtype lt = ACS_LTEE;
		const chtype rt = ACS_RTEE;
		const chtype bt = ACS_BTEE;
		const chtype tt = ACS_TTEE;

		NCursesWindow(const unsigned int _height, const unsigned int _width, const unsigned int _starty, const unsigned int _startx);
		~NCursesWindow();

		unsigned int height() const {
			return this->_height;
		}
		unsigned int width() const {
			return this->_width;
		}
		unsigned int starty() const {
			return this->_starty;
		}
		unsigned int startx() const {
			return this->_startx;
		}
		const std::string& title() const {
			return this->_title;
		}
		void title(const std::string& in) {
			this->_title = in;
		}
		static std::string shorten_string(const std::string& str, unsigned int w);
		void print_left_justified(unsigned int loc_y, unsigned int loc_x, unsigned int w, const std::string& in);
		void print_right_justified(unsigned int loc_y, unsigned int loc_x, unsigned int w, const std::string& in);
		void print_center_justified(unsigned int loc_y, unsigned int loc_x, unsigned int w, const std::string& in);
		void print(unsigned int loc_y, unsigned int loc_x, const std::string& in);

		void clear();
		void draw_border();
		void Mvwin(unsigned int y, unsigned int x);
		void MVwin(int dy, int dx);
		void resize(unsigned int h, unsigned int w);

	private:
		unsigned int _height;
		unsigned int _width;
		unsigned int _starty;
		unsigned int _startx;
		std::string _title;
};

NCursesWindow::NCursesWindow(const unsigned int _height, const unsigned int _width, const unsigned int _starty, const unsigned int _startx) {
	this->_height = _height;
	this->_width = _width;
	this->_starty = _starty;
	this->_startx = _startx;
}

NCursesWindow::~NCursesWindow() {
}

void NCursesWindow::clear() {
	for(unsigned int i = 0; i < this->_height; ++i) {
		mvhline(starty()+i, this->startx(), ' ', this->_width);
	}
}

void NCursesWindow::draw_border() {
	mvhline(starty(), startx()+1, ts, width()-2);
	mvhline(starty()+height()-1, startx()+1, bs, width()-2);
	mvvline(starty()+1, startx(), ls, height()-2);
	mvvline(starty()+1, startx()+width()-1, rs, height()-2);
	mvaddch(starty(), startx(), tl);
	mvaddch(starty(), startx()+width()-1, tr);
	mvaddch(starty()+height()-1, startx(), bl);
	mvaddch(starty()+height()-1, startx()+width()-1, br);
	//wborder(this->win, ls, rs, ts, bs, tl, tr, bl, br);
}

void NCursesWindow::Mvwin(unsigned int y, unsigned int x) {
	this->clear();
	this->_starty = y;
	this->_startx = x;
}

void NCursesWindow::MVwin(int dy, int dx) {
	this->clear();
	bool set = false;
	if(dy < 0) {
		if(this->_starty < (unsigned int) abs(dy)) {
			this->_starty = 0;
			set = true;
		}
	} else {
		if((unsigned int)(this->_starty+dy) >= (unsigned int)LINES) {
			this->_starty = LINES-1;
			set = true;
		}
	}
	if(!set) {
		this->_starty = ((int)this->_starty)+dy;
	}
	set = false;
	if(dx < 0) {
		if(this->_startx < (unsigned int) abs(dx)) {
			this->_startx = 0;
			set = true;
		}
	} else {
		if((unsigned int)(this->_startx+dx) >= (unsigned int) COLS) {
			this->_startx = COLS-1;
			set = true;
		}
	}
	if(!set) {
		this->_startx = ((int)this->_startx)+dx;
	}
}

void NCursesWindow::resize(unsigned int h, unsigned int w) {
	this->clear();
	this->_height = h;
	this->_width = w;
}

std::string NCursesWindow::shorten_string(const std::string& str, unsigned int w) {
	std::string to_print = str;
	if(str.size() > w) {
		to_print = str.substr(0, w);
		if(w <= 4) {
			for(unsigned int i=1; i <= w; ++i) {
				to_print[i] = '.';
			}
		} else {
			for(unsigned int i=0; i < 3; ++i) {
				to_print[to_print.size()-1-i] = '.';
			}
		}
	}
	return to_print;
}

void NCursesWindow::print(unsigned int loc_y, unsigned int loc_x, const std::string& in) {
	mvprintw(starty()+loc_y, startx()+loc_x, in.c_str());
}

void NCursesWindow::print_left_justified(unsigned int loc_y, unsigned int loc_x, unsigned int w, const std::string& in) {
	if(loc_x > width()) {
		// do nothing..
		return;
	}

	//Adjust if too far
	if(loc_x + w > width()) {
		w = width()-loc_x;
	}
	std::string to_print = shorten_string(in, w);

	print(loc_y, loc_x, to_print);
}

void NCursesWindow::print_right_justified(unsigned int loc_y, unsigned int loc_x, unsigned int w, const std::string& in) {
	unsigned int left = loc_x;
	if(in.size() < w) {
		left = loc_x+(w-in.size());
		w = in.size();
	}

	print_left_justified(loc_y, left, w, in);
}

void NCursesWindow::print_center_justified(unsigned int loc_y, unsigned int loc_x, unsigned int w, const std::string& in) {
	unsigned int center = loc_x+(w/2);
	unsigned int left = loc_x;
	if(in.size() < w) {
		left = center-(in.size()/2);
		w = in.size();
	}

	print_left_justified(loc_y, left, w, in);
}

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

	NCursesWindow test_window(10, 20, 10, 10);
	NCursesWindow test_window_2(10, 20, 10, 10);

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
