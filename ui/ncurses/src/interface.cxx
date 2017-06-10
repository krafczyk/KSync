#include "ksync/ui/ncurses/interface.h"

namespace KSync {
	namespace Ui {
		Object::Object(Object* parent) {
			this->parent = parent;
		}

		Object::~Object() {
			for(auto obj_i = this->children.begin(); obj_i != this->children.end(); ++obj_i) {
				delete obj_i->second;
			}
		}

		void Object::DestroyChild(Object* obj) {
			for(auto obj_i = this->children.begin(); obj_i != this->children.end(); ++obj_i) {
				if(obj_i->second == obj) {
					this->children.erase(obj_i);
					delete obj;
				}
			}
		}

		void Object::DestroyMe() {
			this->parent->DestroyChild(this);
		}

		void Object::AddChildObject(const std::string& name, Object* obj) {
			this->children[name] = obj;
		}

		Object* Object::GetChildObject(const std::string& name) {
			auto obj_i = this->children.find(name);
			if(obj_i == this->children.end()) {
				return 0;
			} else {
				return obj_i->second;
			}
		}

		Object* Object::GetParentObject() {
			return this->parent;
		}
	}
}
