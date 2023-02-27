/*
Copyright 2022 Jordon Gulley

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef GODOT_EDITOR_FMOD_H
#define GODOT_EDITOR_FMOD_H

#include "editor/editor_node.h"
#include "editor/editor_inspector.h"
#include "core/object/class_db.h"
#include "core/object/object.h"
#include "../api/core/inc/fmod.hpp"
#include "../api/studio/inc/fmod_studio.hpp"
#include "editor_fmod_event_inspector.h"

class EditorFmodEventInspector;
class EditorFmodManager : public Object {
	GDCLASS(EditorFmodManager, Object);
    static EditorFmodManager *singleton;

protected:
	EditorFmodEventInspector *inspector_plugin;
	static void _bind_methods();
	Vector<String> get_event_paths(FMOD::Studio::Bank *bank);

public:
    static EditorFmodManager *get_singleton();
    // Vector<String> banks_to_index = Vector<String>();
	struct BankInfo {
		int bank_modified_time = 0;
		FMOD::Studio::Bank *bank = nullptr;
		bool is_dirty(int file_modified_time) {
			print_line(vformat("DIRT1 %s  2 %s", bank_modified_time, file_modified_time));
			return bank_modified_time != file_modified_time;
		}

		BankInfo() {
			bank_modified_time = 0;
			bank = nullptr;
		}

		BankInfo(int file_modified_time, FMOD::Studio::Bank *loaded_bank) {
			bank_modified_time = file_modified_time;
			bank = loaded_bank;
		}
	};
    struct Cache {
		bool dirty = true;
        Vector<String> event_paths = Vector<String>();
    };
	void generate_cache();
    Cache cache = Cache();
    HashMap<String, BankInfo> loaded_banks = HashMap<String, BankInfo>();
    FMOD::Studio::System *fmod_system;

	static void init_callback();
	void _scan_files(EditorFileSystemDirectory *dir);
	void _scan_dir(EditorFileSystemDirectory *dir);
	void scan_for_banks();

    Error load_bank(const String &bank_path);

    EditorFmodManager();
    ~EditorFmodManager();
};

#endif //GODOT_EDITOR_FMOD_H
