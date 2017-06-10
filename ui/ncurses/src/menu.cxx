#include "ksync/logging.h"
#include "ksync/ui/ncurses/menu.h"

namespace KSync {
	namespace Ui {
		void NCursesMenu::AppendToMenu(const std::string& entry_string) {
			this->menu_items.push_back(entry_string);
		}

		void NCursesMenu::Draw() {
			LOGF(INFO, "1 (%lu)", this->menu_items.size());
			this->draw_border();
			for(size_t m_i = 0; m_i < this->menu_items.size(); ++m_i) {
				LOGF(INFO, "2");
				if(selected_item == m_i) {
					LOGF(INFO, "3");
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
	}
}
