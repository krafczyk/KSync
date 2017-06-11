#include "ksync/logging.h"
#include "ksync/ui/ncurses/menu.h"

namespace KSync {
	namespace Ui {
		void NCursesMenu::AppendToMenu(const std::string& entry_string) {
			this->menu_items.push_back(entry_string);
		}

		void NCursesMenu::Draw() {
			this->draw_border();
			for(size_t m_i = 0; m_i < this->menu_items.size(); ++m_i) {
				if(selected_item == m_i) {
					attron(A_REVERSE);
				}
				this->print_center_justified(m_i+1, 1, this->width()-2, this->menu_items[m_i]);
				if(selected_item == m_i) {
					attroff(A_REVERSE);
				}
			}
		}

		void NCursesMenu::HandleEvent(const chtype event) {
			if(this->menu_items.size() != 0) {
				if (event == KEY_UP) {
					if(this->selected_item != 0) {
						--this->selected_item;
					}
				} else if (event == KEY_DOWN) {
					if(this->selected_item != (this->menu_items.size()-1)) {
						++this->selected_item;
					}
				}
			}
		}

		const std::string& NCursesMenu::GetNameOfSelected() {
			return this->menu_items[this->selected_item];
		}

		void NCursesMenu::SetMenuItem(const size_t m_i, const std::string& menu_item) {
			this->menu_items[m_i] = menu_item;
		}

		int NCursesMenu::GetIdxOfMenuItem(const std::string& menu_item) {
			int m_i = 0;
			for(; (size_t) m_i<this->menu_items.size(); ++m_i) {
				if(this->menu_items[m_i] == menu_item) {
					break;
				}
			}
			if((size_t) m_i != this->menu_items.size()) {
				return m_i;
			} else {
				return -1;
			}
		}

		void NCursesMenu::ReplaceMenuItem(const std::string& menu_item, const std::string& new_name) {
			int idx = GetIdxOfMenuItem(menu_item);
			if(idx != -1) {
				SetMenuItem(idx, new_name);
			}
		}
	}
}
