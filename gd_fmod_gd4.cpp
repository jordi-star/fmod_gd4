/*
Copyright 2022 Jordon Gulley

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "gd_fmod_gd4.h"
FmodManager *FmodManager::singleton = nullptr;

FmodManager *FmodManager::get_singleton() {
	if (!singleton) {
		memnew(FmodManager);
	}
	singleton->add_to_tree();
    return singleton;
}

Error FmodManager::initialize(InitFlags studio_flags, int max_channels) {
	ERR_FAIL_COND_V_MSG(initialized, ERR_ALREADY_IN_USE, "Fmod is already initalized!");
    FMOD_RESULT result = FMOD::Studio::System::create(&fmod_system);
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

    result = fmod_system->initialize(max_channels, init_flag, FMOD_INIT_NORMAL, nullptr);
    ERR_FAIL_COND_V_MSG(result != FMOD_RESULT::FMOD_OK, ERR_CANT_CREATE, vformat("An error occurred while trying to initialize the Fmod Studio System. Error code: %s", itos(static_cast<int>(result))));
    initialized = true;
	randomize_seed();
	set_process_internal(true);
#ifdef DEBUG_ENABLED
    print_line("Initialized FMOD successfully.");
#endif
    return OK;
}

void FmodManager::auto_initialize(InitFlags init_flags, TypedArray<String> banks_to_load, int max_channels) {
#ifdef DEBUG_ENABLED
    print_line("Auto-initializing FMOD. Visit \"Project Settings/Fmod\" to configure.");
#endif
    Error result = initialize(init_flags, max_channels);
	ERR_FAIL_COND_MSG(result != OK, vformat("An error occured while auto-initializing FMOD. Error Code: %s", result));

    Array failed_banks = Array();
    for(int i = 0; i < banks_to_load.size(); i++) {
        print_verbose(vformat("Loading bank -> %s", banks_to_load[i]));
        Error e = load_bank(banks_to_load[i], NORMAL_LOAD);
        if(e != OK) {
            failed_banks.append(banks_to_load[i]);
        }
    }
#ifdef DEBUG_ENABLED
    if(failed_banks.size() > 0) {
        print_line(vformat("Some banks failed to load: ", failed_banks));
    }
#endif
}

void FmodManager::add_to_tree() {
	if (added_to_tree) {
		return;
	}
    if (get_parent() != nullptr) {
        return;
    }
    SceneTree *scene_tree = SceneTree::get_singleton();
	if (!scene_tree) {
		return;
	}
    scene_tree->get_root()->call_deferred(SNAME("add_child"), this);
	added_to_tree = true;
    set_name("FMOD Manager");
}

Ref<FmodEventInstance> FmodManager::create_event_instance(String event_path) {
	ERR_FAIL_COND_V_MSG(!initialized, nullptr, "Unable to create Event Instance. Fmod not initalized!");
    return FmodEventInstance::create(event_path);
}

Ref<FmodVCA> FmodManager::get_vca(String vca_path) {
    Ref<FmodVCA> vca = memnew(FmodVCA);
    fmod_system->getVCA(vca_path.utf8().get_data(), &vca->_vca);
    vca->path = vca_path;
    return vca;
}

Ref<FmodEventInstance> FmodManager::play(String event_path) {
    Ref<FmodEventInstance> event_instance = create_event_instance(event_path);
    event_instance->play();
    return event_instance;
}

Error FmodManager::load_bank(String path_relative_to_project_root, BankLoadFlags flags) {
	ERR_FAIL_COND_V_MSG(!initialized, ERR_UNCONFIGURED, "Unable to load bank. Fmod not initalized!");
    if (!path_relative_to_project_root.begins_with("./")) {
        path_relative_to_project_root = "./" + path_relative_to_project_root;
    }
	ERR_FAIL_COND_V_MSG(loaded_banks.has(path_relative_to_project_root), ERR_ALREADY_EXISTS, vformat("Unable to load bank. Bank at '%s' already loaded!", path_relative_to_project_root));
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

    FMOD_RESULT result = fmod_system->loadBankFile(path_relative_to_project_root.utf8().get_data(), load_flag, &bank);
	ERR_FAIL_COND_V_MSG(!bank, ERR_CANT_OPEN, vformat("Unable to load bank at %s. FMOD_RESULT error code: %s", path_relative_to_project_root, static_cast<int>(result)));
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
    fmod_system->getCoreSystem(&core);
    core->setAdvancedSettings(advancedSettings);
}

void FmodManager::_bind_methods() {
    ClassDB::bind_method(D_METHOD("initialize", "init_flags", "max_channels"), &FmodManager::initialize);
    ClassDB::bind_method(D_METHOD("randomize_seed"), &FmodManager::randomize_seed);
	ClassDB::bind_method(D_METHOD("create_event_instance", "event_path"), &FmodManager::create_event_instance);
    ClassDB::bind_method(D_METHOD("play", "event_path"), &FmodManager::play);
    ClassDB::bind_method(D_METHOD("get_vca", "vca_path"), &FmodManager::get_vca);
    ClassDB::bind_method(D_METHOD("load_bank", "path_relative_to_project_root", "load_flags"), &FmodManager::load_bank);
    ClassDB::bind_method(D_METHOD("set_global_parameter", "param_name", "new_value"), &FmodManager::set_global_parameter);
    ClassDB::bind_method(D_METHOD("get_global_parameter", "param_name"), &FmodManager::get_global_parameter);

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

void FmodManager::set_global_parameter(String p_name, float p_value) {
	ERR_FAIL_COND_MSG(!initialized, vformat("FMOD not initalized. Could not set global paramter %s", p_name));
	fmod_system->setParameterByName(p_name.utf8(), p_value);
}

float FmodManager::get_global_parameter(String p_name) {
	ERR_FAIL_COND_V_MSG(!initialized, -1.0, vformat("FMOD not initalized. Could not set global paramter %s", p_name));
	float r = 0;
	FMOD_RESULT result = fmod_system->getParameterByName(p_name.utf8(), nullptr, &r);
	ERR_FAIL_COND_V_MSG(result != FMOD_OK, -1.0, vformat("An error occured while getting Global Parameter. FMOD_RESULT Error Code: %s", static_cast<int>(result)));
	return r;
}

void FmodManager::_notification(int p_what) {
    switch (p_what) {
        case NOTIFICATION_INTERNAL_PROCESS: {
            if(!initialized) {
                return;
            }
            fmod_system->update();
            for (int i = 0; i < events.size();) {
                Ref<FmodEventInstance> event = events.get(i);
                // Check if event instance has been freed and inner event instance is valid.
                if (!event.is_valid() || !event->is_instance_valid()) {
                    events.remove_at(i);
                    continue;
                }
                event->process_current_callback();
                i++;
            }
        } break;
    }
}

FmodManager::FmodManager() {
	singleton = this;
	bool auto_initialize = GLOBAL_DEF_BASIC("fmod/config/auto_initialize", true);
	// bool use_custom = GLOBAL_DEF("fmod/config/use_custom_fmod_manager", false);
	int max_channels = GLOBAL_DEF("fmod/config/max_channels", 1024);
	InitFlags init_flags = static_cast<InitFlags>(GLOBAL_DEF_BASIC(PropertyInfo(Variant::INT, "fmod/config/initialization_mode", PROPERTY_HINT_ENUM, INIT_FLAGS_PROPERTY_HINT), 0).operator unsigned int());
	TypedArray<String> default_banks = TypedArray<String>();
	default_banks.push_back("./Master.bank");
	default_banks.push_back("./Master.strings.bank");
	TypedArray<String> initial_banks = GLOBAL_DEF_BASIC("fmod/config/banks_to_load", default_banks);
	if (Engine::get_singleton()->is_editor_hint()) {
		return;
	}
	if (auto_initialize) {
		singleton->auto_initialize(init_flags, initial_banks, max_channels);
	}
}

FmodManager::~FmodManager() {
    if(initialized) {
       fmod_system->release();
    }
}