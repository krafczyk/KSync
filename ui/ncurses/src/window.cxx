#include "ksync/logging.h"
#include "ksync/ui/ncurses/window.h"

namespace KSync {
	namespace Ui {
		NCursesWindow::NCursesWindow(const unsigned int _height, const unsigned int _width, const unsigned int _starty, const unsigned int _startx, Object* parent) : Object(parent) {
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

		void NCursesWindow::draw_title() {
			unsigned int w = this->_title.size();
			unsigned int mid = this->startx()+(this->_width/2);
			unsigned int left = mid-(w/2);
			unsigned int right = left + w;
			mvaddch(this->starty(), left-1, rt);
			this->print_left_justified(0,left-this->startx(), w, this->_title);
			mvaddch(this->starty(), right, lt);
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
	}
}

