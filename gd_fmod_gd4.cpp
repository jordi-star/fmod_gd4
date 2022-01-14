/*
Copyright 2022 Jordon Gulley

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "gd_fmod_gd4.h"
FmodManager * FmodManager::singleton;

FmodManager* FmodManager::get_singleton() {
    if(!Engine::get_singleton()->is_editor_hint()) {
        if(ProjectSettings::get_singleton()->get_setting("fmod/config/auto_initialize")) {
            print_line("Auto-initializing FMOD. Visit \"Project Settings/Fmod\" to configure.");
            singleton->initialize(ProjectSettings::get_singleton()->get_setting("fmod/config/max_channels"), static_cast<InitFlags>(ProjectSettings::get_singleton()->get_setting("fmod/config/initialization_mode").operator unsigned int()));
            PackedStringArray banks = ProjectSettings::get_singleton()->get_setting("fmod/config/banks_to_load").operator PackedStringArray();
            
            Array failed_banks = Array();

            for(int i = 0; i < banks.size();i++) {
                print_line("Loading bank -> " + banks[i] + ".bank");
                Error e = singleton->load_bank(banks[i] + ".bank", NORMAL_LOAD);
                if(e) {
                    failed_banks.append(banks[i]);
                }
            }
            if(failed_banks.size() > 0) {
                print_line(vformat("Some banks failed to load: ", failed_banks));
            }
        }
    }
    return singleton;
}

Error FmodManager::initialize(int max_channels, InitFlags studio_flags) {
    if(!initialized) {
        FMOD_RESULT result = FMOD::Studio::System::create(&f_system);
        if(result != FMOD_RESULT::FMOD_OK) {
            print_line("An error occurred while trying to create the Fmod Studio System." + vformat("Error code: %s", itos(static_cast<int>(result))));
            return ERR_CANT_CREATE;
        }

        FMOD_STUDIO_INITFLAGS init_flag;

        switch (studio_flags) {
            case NORMAL: {
                init_flag = FMOD_STUDIO_INIT_NORMAL;
            } break;
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
        }

        result = f_system->initialize(max_channels, init_flag, FMOD_INIT_NORMAL, 0);
        if(result != FMOD_RESULT::FMOD_OK) {
            print_line("An error occurred while trying to initialize the Fmod Studio System." + vformat("Error code: %s", itos(static_cast<int>(result))));
            return ERR_CANT_CREATE;
        }
        else {
            initialized = true;
			randomize_seed();
			set_process_internal(true);
			set_process(true);
            print_line("Initialized FMOD successfully.");
            return OK;
        }
    }
    else {
        return ERR_ALREADY_IN_USE;
    }
}

FmodEventInstance* FmodManager::create_event_instance(String event_path, bool autoplay, bool one_shot) {
    if(initialized) {


        if(get_parent() == nullptr) {
            set_name("FMOD Manager");
            OS::get_singleton()->get_main_loop()->call("get_root").call("add_child", this);
            set_name("FMOD Manager");
        }

        FMOD::Studio::EventDescription *e_desc = nullptr;
        f_system->getEvent(event_path.utf8().get_data(), &e_desc);
        if(e_desc) {
            FMOD::Studio::EventInstance *e_instance = nullptr;
            e_desc->createInstance(&e_instance);
            if(e_instance) {
                randomize_seed();
                FmodEventInstance* event = memnew(FmodEventInstance);
                //event->manager = *this;
                event->_event_instance = e_instance;
                event->set_process_mode(PROCESS_MODE_ALWAYS);
				event->set_callback();
				event->set_process(true);
                event->set_name(event_path);
				event->event_path = event_path;
				event->one_shot = one_shot;
				add_child(event);
                if(autoplay) {
					event->play();
					print_line(vformat("Playing instance. ", event_path));
                }
                events.append(event);
                return event;
            }
            else {
                print_line(vformat("Could not create Event Instance for: %s", event_path));
                return nullptr;
            }
        }
        else {
            print_line(vformat("Could not get EventDescription for: %s", event_path));
            return nullptr;
        }
    }
    else {
        return nullptr;
    }
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
    if(initialized) {
        if (!path_relative_to_project_root.begins_with("./")) {
            path_relative_to_project_root = "./" + path_relative_to_project_root;
        }
        if(loaded_banks.has(path_relative_to_project_root)) {
            return ERR_ALREADY_EXISTS;
        }
        FMOD::Studio::Bank *bank = nullptr;

        FMOD_STUDIO_LOAD_BANK_FLAGS load_flag;

        switch (flags) {
            case NORMAL_LOAD: {
                load_flag = FMOD_STUDIO_LOAD_BANK_NORMAL;
            }
                break;
            case NONBLOCKING: {
                load_flag = FMOD_STUDIO_LOAD_BANK_NONBLOCKING;
            }
                break;
            case DECOMPRESS_SAMPLES: {
                load_flag = FMOD_STUDIO_LOAD_BANK_DECOMPRESS_SAMPLES;
            }
                break;
            case UNENCRYPTED: {
                load_flag = FMOD_STUDIO_LOAD_BANK_UNENCRYPTED;
            }
                break;
        }

        f_system->loadBankFile(path_relative_to_project_root.utf8().get_data(), load_flag, &bank);
        if (bank) {
            loaded_banks.insert(path_relative_to_project_root, bank);
            return OK;
        } else {
            return ERR_CANT_OPEN;
        }
    }
    else {
        return ERR_ALREADY_IN_USE;
    }
}

void FmodManager::randomize_seed() {
    if(initialized) {
        FMOD_ADVANCEDSETTINGS *advancedSettings = new FMOD_ADVANCEDSETTINGS();
        advancedSettings->cbSize = sizeof(FMOD_ADVANCEDSETTINGS);
        time_t seed;
        time(&seed);
        advancedSettings->randomSeed = seed;
        FMOD::System *core = nullptr;
        f_system->getCoreSystem(&core);
        core->setAdvancedSettings(advancedSettings);
    }
}

void FmodManager::_bind_methods() {
    ClassDB::bind_method(D_METHOD("initialize", "max_channels", "studio_flags"), &FmodManager::initialize);
    ClassDB::bind_method(D_METHOD("randomize_seed"), &FmodManager::randomize_seed);
	ClassDB::bind_method(D_METHOD("create_event_instance", "event_path", "autoplay", "one_shot"), &FmodManager::create_event_instance);
    ClassDB::bind_method(D_METHOD("oneshot", "event_path", "autoplay"), &FmodManager::oneshot);
    ClassDB::bind_method(D_METHOD("play", "event_path"), &FmodManager::play);
    ClassDB::bind_method(D_METHOD("get_vca", "vca_path"), &FmodManager::get_vca);
    ClassDB::bind_method(D_METHOD("load_bank", "path_relative_to_project_root", "load_flags"), &FmodManager::load_bank);
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
    //ADD_ARRAY()
}

Array FmodManager::get_events() {
    return events;
}

void FmodManager::set_events(Array a) {
    events = a;
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
		if(singleton != this) {
			if(!Engine::get_singleton()->is_editor_hint()) {
				singleton->queue_delete();
				singleton = this;
			}
		}
    }
}
