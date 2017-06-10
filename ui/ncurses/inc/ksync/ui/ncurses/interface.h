#ifndef KSYNC_UI_NCURSES_INTERFACES_HDR
#define KSYNC_UI_NCURSES_INTERFACES_HDR

#include <map>

#include "curses.h"

namespace KSync {
	namespace Ui {
		class Object {
			public:
				Object(Object* parent);
				virtual ~Object();
				void DestroyChild(Object* obj);
				void DestroyMe();
				void AddChildObject(const std::string& name, Object* obj);
				Object* GetChildObject(const std::string& name);
				Object* GetParentObject();

				virtual void Draw() = 0;
				virtual void HandleEvent(const chtype event) = 0;
			private:
				Object* parent;
				std::map<std::string, Object*> children;
		};
	}
}

#endif
