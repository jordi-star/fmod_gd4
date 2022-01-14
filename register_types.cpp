/*
Copyright 2022 Jordon Gulley

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "register_types.h"

#include "gd_fmod_gd4.h"

static FmodManager* FmodManagerPtr = NULL;

void register_fmod_gd4_types() {
    // Register classes
    ClassDB::register_class<FmodManager>();
    ClassDB::register_class<FmodEventInstance>();
    ClassDB::register_class<FmodVCA>();
    // Singleton
    FmodManagerPtr = memnew(FmodManager);
    Engine::get_singleton()->add_singleton(Engine::Singleton("Fmod", FmodManager::get_singleton()));
    
    // Add FMOD settings
    if(Engine::get_singleton()->is_editor_hint()) {
        if(!ProjectSettings::get_singleton()->has_setting("fmod/config/auto_initialize")) {
            ProjectSettings::get_singleton()->set_setting("fmod/config/auto_initialize", true);
            ProjectSettings::get_singleton()->set_setting("fmod/config/max_channels", 1024);
            ProjectSettings::get_singleton()->set_setting("fmod/config/initialization_mode", 0);
            ProjectSettings::get_singleton()->set_custom_property_info("fmod/config/initialization_mode", PropertyInfo(Variant::INT, "initialization_mode", PROPERTY_HINT_ENUM, "NORMAL,LIVE_UPDATE,ALLOW_MISSING_PLUGINS,SYNCHRONOUS_UPDATE,DEFERRED_CALLBACKS,LOAD_FROM_UPDATE,MEMORY_TRACKING"));
            PackedStringArray init_banks = PackedStringArray();
            init_banks.append("Master");
            init_banks.append("Master.strings");
            ProjectSettings::get_singleton()->set_setting("fmod/config/banks_to_load", init_banks);
            //ProjectSettings::get_singleton()->set_initial_value()
            print_line(ProjectSettings::get_singleton()->save());
            ProjectSettings::get_singleton()->set_initial_value("fmod/config/auto_initialize", true);
            ProjectSettings::get_singleton()->set_initial_value("fmod/config/max_channels", 1024);
            ProjectSettings::get_singleton()->set_initial_value("fmod/config/initialization_mode", 0);
            ProjectSettings::get_singleton()->set_initial_value("fmod/config/banks_to_load", init_banks);
        }
    }
}

void unregister_fmod_gd4_types() {
  memdelete(FmodManagerPtr);
}
