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
#include "ksync/ui/ncurses/interface.h"
#include "ksync/ui/ncurses/window.h"
#include "ksync/ui/ncurses/menu.h"

class MainMenu : public KSync::Ui::NCursesMenu {
	public:
		MainMenu(unsigned int h, unsigned int w, unsigned int y, unsigned int x, Object* parent = 0);
		virtual ~MainMenu() {};

		void HandleEvent(const chtype event);
		void Draw();
};

class AppStateManager : public KSync::Ui::NCursesWindow {
	public:
		AppStateManager();
		virtual ~AppStateManager();

		void Draw();
		void HandleEvent(const chtype event);

		void Run();

		void Quit();

		typedef int State_t;
		static const State_t Main = 0;

	private:
		State_t state;
		bool finished;
};

MainMenu::MainMenu(unsigned int h, unsigned int w, unsigned int y, unsigned int x, Object* parent) : KSync::Ui::NCursesMenu(h, w, y, x, parent) {
	this->AppendToMenu("Test 1");
	this->AppendToMenu("Test 2");
	this->AppendToMenu("Quit");
}

void MainMenu::HandleEvent(const chtype event) {
	if((event == KEY_ENTER)||(event == 10)) {
		if(this->GetNameOfSelected() == "Quit") {
			((AppStateManager*) this->GetParentObject())->Quit();
		}// else if (this->GetNameOfSelected() == "Connect
	} else {
		KSync::Ui::NCursesMenu::HandleEvent(event);
	}
}

void MainMenu::Draw() {
	KSync::Ui::NCursesMenu::Draw();
	this->draw_title();
}

AppStateManager::AppStateManager() : KSync::Ui::NCursesWindow(LINES,COLS,0,0) {
	state = Main;
	finished = false;
	MainMenu* main_menu = new MainMenu(LINES-2,COLS/2,1,1,this);
	main_menu->title("Main Menu");
	this->AddChildObject("main_menu", main_menu);
}

AppStateManager::~AppStateManager() {
}

void AppStateManager::Draw() {
	this->draw_border();
	this->draw_title();
	Object* main_menu = this->GetChildObject("main_menu");
	if(main_menu != 0) {
		main_menu->Draw();
	}
}

void AppStateManager::HandleEvent(const chtype event) {
	if(this->state == Main) {
		Object* main_menu = this->GetChildObject("main_menu");
		if(main_menu == 0) {
			this->finished = true;
		} else {
			main_menu->HandleEvent(event);
			if(this->GetChildObject("main_menu") == 0) {
				this->finished = true;
			}
		}
	}
}

void AppStateManager::Run() {
	int c = 0;
	do {
		this->clear();
		this->Draw();
		refresh();
		c = getch();
		this->HandleEvent(c);
	} while (!(this->finished));
}

void AppStateManager::Quit() {
	this->finished = true;
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

	AppStateManager app_man;
	app_man.title("KSync Client");

	app_man.Run();

	endwin();
	return 0;
}
