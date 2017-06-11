#ifndef KSYNC_UI_NCURSES_MENU_HDR
#define KSYNC_UI_NCURSES_MENU_HDR

#include <vector>
#include <string>

#include "ksync/ui/ncurses/window.h"
#include "ksync/ui/ncurses/interface.h"

namespace KSync {
	namespace Ui {
		class NCursesMenu : public NCursesWindow {
			public:
				NCursesMenu(unsigned int h, unsigned int w, unsigned int y, unsigned int x, Object* parent = 0) : NCursesWindow(h, w, y, x, parent) { selected_item = 0; }
				virtual ~NCursesMenu() {};
				void AppendToMenu(const std::string& entry_string);
				void SetMenuItem(const size_t m_i, const std::string& menu_item);
				int GetIdxOfMenuItem(const std::string& menu_item);
				void ReplaceMenuItem(const std::string& menu_item, const std::string& new_name);
				const std::string& GetNameOfSelected();

				virtual void Draw();
				virtual void HandleEvent(const chtype event);
			private:
				std::vector<std::string> menu_items;
				size_t selected_item;
		};
	}
}

#endif
