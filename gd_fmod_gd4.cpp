/*
Copyright 2022 Jordon Gulley

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "gd_fmod_gd4.h"
FmodManager * FmodManager::singleton;

FmodManager* FmodManager::get_singleton() {
    if(Engine::get_singleton()->is_editor_hint()) {
    	return singleton;
    }
    if(!GLOBAL_GET("fmod/config/auto_initialize")) {
    	return singleton;
    }
    print_line("Auto-initializing FMOD. Visit \"Project Settings/Fmod\" to configure.");
    Error result = singleton->initialize(GLOBAL_GET("fmod/config/max_channels"), static_cast<InitFlags>(GLOBAL_GET("fmod/config/initialization_mode").operator unsigned int()));
	ERR_FAIL_COND_V_MSG(result != OK, singleton, vformat("An error occured while auto-initializing FMOD. Error Code: %s", result));
    PackedStringArray banks = GLOBAL_GET("fmod/config/banks_to_load").operator PackedStringArray();

    Array failed_banks = Array();
    for(int i = 0; i < banks.size();i++) {
        print_line("Loading bank -> " + banks[i] + ".bank");
        Error e = singleton->load_bank(banks[i] + ".bank", NORMAL_LOAD);
        if(e != OK) {
            failed_banks.append(banks[i]);
        }
    }
    if(failed_banks.size() > 0) {
        print_line(vformat("Some banks failed to load: ", failed_banks));
    }
    return singleton;
}

Error FmodManager::initialize(int max_channels, InitFlags studio_flags) {
	ERR_FAIL_COND_V_MSG(initialized, ERR_ALREADY_IN_USE, "Fmod is already initalized!");
    FMOD_RESULT result = FMOD::Studio::System::create(&f_system);
    ERR_FAIL_COND_V_MSG(result != FMOD_RESULT::FMOD_OK, ERR_CANT_CREATE, vformat("An error occurred while trying to create the Fmod Studio System. Error code: %s", itos(static_cast<int>(result))));

    FMOD_STUDIO_INITFLAGS init_flag;
    switch (studio_flags) {
        case LIVE_UPDATE: {
            init_flag = FMOD_STUDIO_INIT_LIVEUPDATE;
        } break;
        case ALLOW_MISSING_PLUGINS: {
            init_flag = FMOD_STUDIO_INIT_ALLOW_MISSING_PLUGINS;
        } break;
        case SYNCHRONOUS_UPDATE: {
           init_flag = FMOD_STUDIO_INIT_SYNCHRONOUS_UPDATE;
        } break;
        case DEFERRED_CALLBACKS: {
            init_flag = FMOD_STUDIO_INIT_DEFERRED_CALLBACKS;
        } break;
        case LOAD_FROM_UPDATE: {
            init_flag = FMOD_STUDIO_INIT_LOAD_FROM_UPDATE;
        } break;
        case MEMORY_TRACKING: {
            init_flag = FMOD_STUDIO_INIT_MEMORY_TRACKING;
        } break;
        default: {
            init_flag = FMOD_STUDIO_INIT_NORMAL;
        } break;
    }

    result = f_system->initialize(max_channels, init_flag, FMOD_INIT_NORMAL, 0);
    ERR_FAIL_COND_V_MSG(result != FMOD_RESULT::FMOD_OK, ERR_CANT_CREATE, vformat("An error occurred while trying to initialize the Fmod Studio System. Error code: %s", itos(static_cast<int>(result))));
    initialized = true;
	randomize_seed();
	set_process_internal(true);
	set_process(true);
    print_line("Initialized FMOD successfully.");
    return OK;
}

FmodEventInstance* FmodManager::create_event_instance(String event_path, bool autoplay, bool one_shot) {
	ERR_FAIL_COND_V_MSG(!initialized, nullptr, "Unable to create Event Instance. Fmod not initalized!");
    if (get_parent() == nullptr) {
        set_name("FMOD Manager");
        OS::get_singleton()->get_main_loop()->call("get_root").call("add_child", this);
        set_name("FMOD Manager");
    }

    FMOD::Studio::EventDescription *e_desc = nullptr;
    FMOD_RESULT result = f_system->getEvent(event_path.utf8().get_data(), &e_desc);
	ERR_FAIL_COND_V_MSG(!e_desc, nullptr, vformat("Could not get EventDescription for: %s. FMOD_RESULT Error Code: %s", event_path, result));

    FMOD::Studio::EventInstance *e_instance = nullptr;
    e_desc->createInstance(&e_instance);
	ERR_FAIL_COND_V_MSG(!e_instance, nullptr, vformat("Could not create Event Instance for: %s. FMOD_RESULT Error Code: %s", event_path, static_cast<int>(result)));

    randomize_seed();
    FmodEventInstance* event = memnew(FmodEventInstance);
    event->_event_instance = e_instance;
    event->set_process_mode(PROCESS_MODE_ALWAYS);
	event->set_callback();
	event->set_process(true);
    event->set_name(event_path);
	event->event_path = event_path;
	event->one_shot = one_shot;
	add_child(event);
    if (autoplay) {
		event->play();
    }
    events.append(event);
    return event;
}

FmodVCA* FmodManager::get_vca(String vca_path) {
    FmodVCA* vca = memnew(FmodVCA);
    f_system->getVCA(vca_path.utf8().get_data(), &vca->_vca);
    vca->path = vca_path;
    return vca;
}

FmodEventInstance* FmodManager::oneshot(String event_path, bool autoplay) {
    return create_event_instance(event_path, autoplay, true);
}

FmodEventInstance* FmodManager::play(String event_path) {
    return create_event_instance(event_path, true, true);
}

Error FmodManager::load_bank(String path_relative_to_project_root, BankLoadFlags flags) {
	ERR_FAIL_COND_V_MSG(!initialized, ERR_UNCONFIGURED, "Unable to load bank. Fmod not initalized!");
    if (!path_relative_to_project_root.begins_with("./")) {
        path_relative_to_project_root = "./" + path_relative_to_project_root;
    }
	ERR_FAIL_COND_V_MSG(loaded_banks.has(path_relative_to_project_root), ERR_ALREADY_EXISTS, vformat("Unable to create load bank. Bank at '%s' already loaded!", path_relative_to_project_root));
    FMOD::Studio::Bank *bank = nullptr;

    FMOD_STUDIO_LOAD_BANK_FLAGS load_flag;
    switch (flags) {
		case NONBLOCKING: {
			load_flag = FMOD_STUDIO_LOAD_BANK_NONBLOCKING;
		} break;
		case DECOMPRESS_SAMPLES: {
			load_flag = FMOD_STUDIO_LOAD_BANK_DECOMPRESS_SAMPLES;
		} break;
		case UNENCRYPTED: {
			load_flag = FMOD_STUDIO_LOAD_BANK_UNENCRYPTED;
		} break;
		default: {
				load_flag = FMOD_STUDIO_LOAD_BANK_NORMAL;
		} break;
    }

    FMOD_RESULT result = f_system->loadBankFile(path_relative_to_project_root.utf8().get_data(), load_flag, &bank);
	ERR_FAIL_COND_V_MSG(!bank, ERR_CANT_OPEN, vformat("Unable to create load bank. FMOD_RESULT error code: %s", static_cast<int>(result)));
    loaded_banks.insert(path_relative_to_project_root, bank);
    return OK;
}

void FmodManager::randomize_seed() {
	ERR_FAIL_COND_MSG(!initialized, "Unable to randomize FMOD seed. FMOD not initalized!");
    FMOD_ADVANCEDSETTINGS *advancedSettings = new FMOD_ADVANCEDSETTINGS();
    advancedSettings->cbSize = sizeof(FMOD_ADVANCEDSETTINGS);
    time_t seed;
    time(&seed);
    advancedSettings->randomSeed = seed;
    FMOD::System *core = nullptr;
    f_system->getCoreSystem(&core);
    core->setAdvancedSettings(advancedSettings);
}

void FmodManager::_bind_methods() {
    ClassDB::bind_method(D_METHOD("initialize", "max_channels", "studio_flags"), &FmodManager::initialize);
    ClassDB::bind_method(D_METHOD("randomize_seed"), &FmodManager::randomize_seed);
	ClassDB::bind_method(D_METHOD("create_event_instance", "event_path", "autoplay", "one_shot"), &FmodManager::create_event_instance);
    ClassDB::bind_method(D_METHOD("oneshot", "event_path", "autoplay"), &FmodManager::oneshot);
    ClassDB::bind_method(D_METHOD("play", "event_path"), &FmodManager::play);
    ClassDB::bind_method(D_METHOD("get_vca", "vca_path"), &FmodManager::get_vca);
    ClassDB::bind_method(D_METHOD("load_bank", "path_relative_to_project_root", "load_flags"), &FmodManager::load_bank);
    ClassDB::bind_method(D_METHOD("set_global_parameter", "param_name", "new_value"), &FmodManager::set_global_parameter);
    ClassDB::bind_method(D_METHOD("get_global_parameter", "param_name"), &FmodManager::get_global_parameter);
    ClassDB::bind_method(D_METHOD("set_events", "a"), &FmodManager::set_events);
    ClassDB::bind_method(D_METHOD("get_events"), &FmodManager::get_events);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "events", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_INTERNAL), "set_events", "get_events");

    BIND_ENUM_CONSTANT(NORMAL);
    BIND_ENUM_CONSTANT(LIVE_UPDATE);
    BIND_ENUM_CONSTANT(ALLOW_MISSING_PLUGINS);
    BIND_ENUM_CONSTANT(SYNCHRONOUS_UPDATE);
    BIND_ENUM_CONSTANT(DEFERRED_CALLBACKS);
    BIND_ENUM_CONSTANT(LOAD_FROM_UPDATE);
    BIND_ENUM_CONSTANT(MEMORY_TRACKING);
    BIND_ENUM_CONSTANT(NORMAL_LOAD);
    BIND_ENUM_CONSTANT(NONBLOCKING);
    BIND_ENUM_CONSTANT(DECOMPRESS_SAMPLES);
    BIND_ENUM_CONSTANT(UNENCRYPTED);
}

Array FmodManager::get_events() {
    return events;
}

void FmodManager::set_events(Array a) {
    events = a;
}

void FmodManager::set_global_parameter(String p_name, float p_value) {
	ERR_FAIL_COND_MSG(!initialized, vformat("FMOD not initalized. Could not set global paramter %s", p_name));
	f_system->setParameterByName(p_name.utf8(), p_value);
}

float FmodManager::get_global_parameter(String p_name) {
	ERR_FAIL_COND_V_MSG(!initialized, -1.0, vformat("FMOD not initalized. Could not set global paramter %s", p_name));
	float r = 0;
	FMOD_RESULT result = f_system->getParameterByName(p_name.utf8(), 0, &r);
	ERR_FAIL_COND_V_MSG(result != FMOD_OK, -1.0, vformat("An error occured while getting Global Parameter. FMOD_RESULT Error Code: %s", static_cast<int>(result)));
	return r;
}

void FmodManager::run_callbacks() {
    if(initialized) {
		f_system->update();
	}
}

void FmodManager::_notification(int p_what) {
    switch (p_what) {
        case NOTIFICATION_INTERNAL_PROCESS: {
            run_callbacks();
        } break;
        case NOTIFICATION_PREDELETE: {
            if(initialized) {
                f_system->release();
                memdelete(&f_system);
            }
        } break;
    }
}

FmodManager::FmodManager() {
    if(singleton == nullptr) {
        singleton = this;
    }
    else {
		if(singleton == this) {
			return;
		}
		if(!Engine::get_singleton()->is_editor_hint()) {
			singleton->queue_free();
			singleton = this;
		}
    }
}
