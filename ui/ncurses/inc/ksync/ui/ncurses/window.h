#ifndef KSYNC_UI_NCURSES_WINDOW_HDR
#define KSYNC_UI_NCURSES_WINDOW_HDR

#include <string>

#include "ksync/ui/ncurses/interface.h"

#include "curses.h"

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

namespace KSync {
	namespace Ui {
		class NCursesWindow : public Object {
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

				NCursesWindow(const unsigned int _height, const unsigned int _width, const unsigned int _starty, const unsigned int _startx, Object* parent = 0);
				virtual ~NCursesWindow();

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
				void draw_title();
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
	}
}

#endif
