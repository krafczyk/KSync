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
#include "ksync/client/client_state.h"
#include "ksync/comm/interface.h"
#include "ksync/comm/factory.h"

class StateInfo : public KSync::Ui::NCursesWindow {
	public:
		StateInfo(std::shared_ptr<KSync::Client::ClientState>& state, unsigned int h, unsigned int w, unsigned int y, unsigned int x, KSync::Ui::Object* parent = 0);

		void Draw();
		void HandleEvent(chtype event);

	private:
		std::shared_ptr<KSync::Client::ClientState> client_state;
};

class MainMenu : public KSync::Ui::NCursesMenu {
	public:
		MainMenu(unsigned int h, unsigned int w, unsigned int y, unsigned int x, Object* parent = 0);
		virtual ~MainMenu() {};

		void HandleEvent(const chtype event);
		void Draw();
};

class AppStateManager : public KSync::Ui::NCursesWindow {
	public:
		AppStateManager(const bool nanomsg, const std::string& gateway_socket_url);
		virtual ~AppStateManager();

		void PositionSubordinates();

		void Draw();
		void InitializeCommSystem();
		void HandleEvent(const chtype event);

		void Run();

		void Quit();

		typedef int State_t;
		static const State_t Main = 0;

	private:
		State_t state;
		std::string gateway_socket_url;
		std::shared_ptr<KSync::Client::ClientState> client_state;
		std::shared_ptr<KSync::Comm::CommSystemInterface> comm_interface;
};

StateInfo::StateInfo(std::shared_ptr<KSync::Client::ClientState>& state, unsigned int h, unsigned int w, unsigned int y, unsigned int x, KSync::Ui::Object* parent) : KSync::Ui::NCursesWindow(h, w, y, x, parent) {
	this->title("State Information");
	this->client_state = state;
}

void StateInfo::Draw() {
	this->draw_border();
	this->draw_title();
	if(this->client_state->GetFinished()) {
		this->print_center_justified(1, 1, this->width()-2, "Finished...");
	} else {
		this->print_center_justified(1, 1, this->width()-2, "Running...");
	}
	mvaddch(this->starty()+2, this->startx(), lt);
	mvaddch(this->starty()+2, this->startx()+this->width()-1, rt);
	mvhline(this->starty()+2, this->startx()+1, ts, this->width()-2);
	std::stringstream ss;
	if(this->client_state->GetCommNanomsg()) {
		ss << "Nanomsg ";
	} else {
		ss << "Zeromq ";
	}
	ss << "Comm System ";
	if(this->client_state->GetCommInitialized()) {
		ss << "Initialized";
	} else {
		ss << "Uninitialized";
	}
	this->print_center_justified(3, 1, this->width()-2, ss.str());
	mvaddch(this->starty()+4, this->startx(), lt);
	mvaddch(this->starty()+4, this->startx()+this->width()-1, rt);
	mvhline(this->starty()+4, this->startx()+1, ts, this->width()-2);
}


void StateInfo::HandleEvent(chtype event __attribute__((unused))) {
}

MainMenu::MainMenu(unsigned int h, unsigned int w, unsigned int y, unsigned int x, Object* parent) : KSync::Ui::NCursesMenu(h, w, y, x, parent) {
	this->title("Main Menu");
	this->AppendToMenu("Initialize Comm System");
	this->AppendToMenu("Quit");
}

void MainMenu::HandleEvent(const chtype event) {
	if((event == KEY_ENTER)||(event == 10)) {
		if(this->GetNameOfSelected() == "Quit") {
			((AppStateManager*) this->GetParentObject())->Quit();
		} else if (this->GetNameOfSelected() == "Initialize Comm System") {
			((AppStateManager*) this->GetParentObject())->InitializeCommSystem();
		}
	} else {
		KSync::Ui::NCursesMenu::HandleEvent(event);
	}
}

void MainMenu::Draw() {
	KSync::Ui::NCursesMenu::Draw();
	this->draw_title();
}

AppStateManager::AppStateManager(const bool nanomsg, const std::string& gateway_socket_url) : KSync::Ui::NCursesWindow(LINES,COLS,0,0) {
	state = Main;
	this->client_state.reset(new KSync::Client::ClientState());
	this->client_state->SetCommNanomsg(nanomsg);
	MainMenu* main_menu = new MainMenu(0,0,0,0,this);
	this->AddChildObject("main_menu", main_menu);
	StateInfo* state_info = new StateInfo(this->client_state, 0,0,0,0, this);
	this->AddChildObject("state_info", state_info);
	this->PositionSubordinates();
	this->gateway_socket_url = gateway_socket_url;
}

AppStateManager::~AppStateManager() {
}

void AppStateManager::PositionSubordinates() {
	this->Mvwin(0,0);
	this->resize(LINES, COLS);

	MainMenu* main_menu = (MainMenu*) this->GetChildObject("main_menu");
	if(main_menu != 0) {
		main_menu->Mvwin(1,1);
		main_menu->resize(LINES-2,COLS/2);
	}
	StateInfo* state_info = (StateInfo*) this->GetChildObject("state_info");
	if(state_info != 0) {
		state_info->Mvwin(1,1+(COLS/2));
		state_info->resize(LINES-2, COLS-2-(COLS/2));
	}
}

void AppStateManager::Draw() {
	this->draw_border();
	this->draw_title();
	Object* main_menu = this->GetChildObject("main_menu");
	if(main_menu != 0) {
		main_menu->Draw();
	}
	Object* state_info = this->GetChildObject("state_info");
	if(state_info != 0) {
		state_info->Draw();
	}
}

void AppStateManager::HandleEvent(const chtype event) {
	if(this->state == Main) {
		Object* main_menu = this->GetChildObject("main_menu");
		main_menu->HandleEvent(event);
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
	} while (!(this->client_state->GetFinished()));
}

void AppStateManager::Quit() {
	this->client_state->SetFinished(true);
}

void AppStateManager::InitializeCommSystem() {
	if(this->client_state->GetCommNanomsg()) {
		if(KSync::Comm::GetNanomsgCommSystem(this->comm_interface) < 0) {
			LOGF(SEVERE, "There was a problem initializing the ZeroMQ communication system!");
			this->Quit();
		}
	} else {
		if(KSync::Comm::GetZeromqCommSystem(this->comm_interface) < 0) {
			LOGF(SEVERE, "There was a problem initializing the Nanomsg communication system!");
			this->Quit();
		}
	}
	this->client_state->SetCommInitialized(true);
	MainMenu* main_menu = (MainMenu*) this->GetChildObject("main_menu");
	if(main_menu != 0) {
		main_menu->ReplaceMenuItem("Initialize Comm System", "Connect To Server");
	}
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

	//Get Default gateway socket url
	if (!gateway_socket_url_defined) {
		if(KSync::Utilities::get_default_ipc_connection_url(gateway_socket_url) < 0) {
			LOGF(SEVERE, "There was a problem getitng the default IPC connection URL.");
			return -2;
		}
	} else {
		if(gateway_socket_url.substr(0, 3) != "icp") {
			LOGF(SEVERE, "Non icp sockets are not properly implemented at this time.");
			return -2;
		}
	}

	LOGF(INFO, "Using the following socket url: %s", gateway_socket_url.c_str());

	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	refresh();

	AppStateManager app_man(nanomsg, gateway_socket_url);
	app_man.title("KSync Client");

	app_man.Run();

	endwin();
	return 0;
}
