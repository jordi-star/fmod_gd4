/*
Copyright 2022 Jordon Gulley

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "editor_fmod_manager.h"
#include "editor/editor_file_system.h"
#include "modules/fmod_gd4/api/studio/inc/fmod_studio.hpp"

EditorFmodManager *EditorFmodManager::singleton = nullptr;

void EditorFmodManager::_bind_methods() {
}

EditorFmodManager *EditorFmodManager::get_singleton() {
	if (!singleton) {
		memnew(EditorFmodManager);
	}
	return singleton;
}

void EditorFmodManager::generate_cache() {
	if (!cache.dirty) {
		return;
	}
	cache.dirty = false;
	cache.event_paths.clear();
	for (const KeyValue<String, BankInfo> &E : loaded_banks) {
		cache.event_paths.append_array(get_event_paths(E.value.bank));
	}
}

void EditorFmodManager::init_callback() {
	EditorFileSystem::get_singleton()->connect(SNAME("filesystem_changed"), callable_mp(singleton, &EditorFmodManager::scan_for_banks));
	singleton->scan_for_banks();

	singleton->inspector_plugin = memnew(EditorFmodEventInspector);
	EditorInspector::add_inspector_plugin(singleton->inspector_plugin);
}

void EditorFmodManager::_scan_files(EditorFileSystemDirectory *dir) {
	int count = dir->get_file_count();
	for (int i = 0; i < count; i++) {
		String file_name = dir->get_file(i);
		if (file_name.get_extension() == "bank") {
			load_bank(dir->get_file_path(i));
		}
	}
}

void EditorFmodManager::_scan_dir(EditorFileSystemDirectory *dir) {
	if (dir == nullptr) {
		return;
	}
	// Scan current directory
	_scan_files(dir);
	int count = dir->get_subdir_count();
	// Scan subdirs
	for (int d = 0; d < count;) {
		_scan_dir(dir->get_subdir(d++));
	}
}

void EditorFmodManager::scan_for_banks() {
	_scan_dir(EditorFileSystem::get_singleton()->get_filesystem());
	generate_cache();
}

Error EditorFmodManager::load_bank(const String &bank_path) {
	const int bank_modified_time = FileAccess::get_modified_time(bank_path);
	if (loaded_banks.has(bank_path)) {
		if (!loaded_banks[bank_path].is_dirty(bank_modified_time)) {
			return Error::OK;
		}
	}
	Error err;
	Ref<FileAccess> bank_file = FileAccess::open(bank_path, FileAccess::READ, &err);
	if (err != Error::OK) {
		return err;
	}

	// Load bank into memory
	int length = bank_file->get_length();
	char *buffer = (char *)memalloc(length);
	bank_file->get_buffer((uint8_t *)buffer, length);

	// Load bank into FMOD *from* memory.
	FMOD::Studio::Bank *bank = nullptr;
	FMOD_RESULT result = fmod_system->loadBankMemory(buffer, length, FMOD_STUDIO_LOAD_MEMORY, FMOD_STUDIO_LOAD_BANK_NORMAL, &bank);
	ERR_FAIL_COND_V_MSG(result != FMOD_OK, ERR_CANT_OPEN, vformat("Unable to load bank at %s. FMOD_RESULT error code: %s", bank_path, static_cast<int>(result)));
	loaded_banks.insert(bank_path, BankInfo(bank_modified_time, bank));
	cache.dirty = true;
	return Error::OK;
}

Vector<String> EditorFmodManager::get_event_paths(FMOD::Studio::Bank *bank) {
	Vector<String> result = Vector<String>();
	int event_count;
	bank->getEventCount(&event_count);
	FMOD::Studio::EventDescription **events = (FMOD::Studio::EventDescription**) memalloc(event_count * sizeof(FMOD::Studio::EventDescription*));
	bank->getEventList(events, event_count, nullptr);
	for (int e = 0; e < event_count; e++) {
		int size;
		events[e]->getPath(nullptr, 0, &size);
		char *path = (char *)memalloc(size);
		events[e]->getPath(path, size, nullptr);
		String p = String(path);
		result.push_back(p);
		memfree(path);
	}
	memfree(events);
	return result;
}

EditorFmodManager::EditorFmodManager() {
	if (singleton != nullptr) {
		return;
	}
	singleton = this;
	FMOD::Studio::System::create(&fmod_system);
	fmod_system->initialize(1, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_NORMAL, nullptr);
}

EditorFmodManager::~EditorFmodManager() {
	EditorInspector::remove_inspector_plugin(inspector_plugin);
}