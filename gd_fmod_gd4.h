/*
Copyright 2022 Jordon Gulley

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef GODOT_FMOD_GD4_H
#define GODOT_FMOD_GD4_H

#include "gd_fmod_event_instance.h"
#include "gd_fmod_vca.h"
#include "resources/fmod_bank_resource.h"

#include "api/core/inc/fmod.hpp"
#include "api/studio/inc/fmod_studio.hpp"
#include "core/config/project_settings.h"
#include "core/variant/array.h"
#include "scene/main/node.h"
#include "scene/main/window.h"

class FmodManager : public Node {
    GDCLASS(FmodManager, Node);

protected:
    static FmodManager* singleton;
    static void _bind_methods();

public:
    enum InitFlags {
        NORMAL,
        LIVE_UPDATE,
        ALLOW_MISSING_PLUGINS,
        SYNCHRONOUS_UPDATE,
        DEFERRED_CALLBACKS,
        LOAD_FROM_UPDATE,
        MEMORY_TRACKING
    };
	const String INIT_FLAGS_PROPERTY_HINT = "NORMAL,LIVE_UPDATE,ALLOW_MISSING_PLUGINS,SYNCHRONOUS_UPDATE,DEFERRED_CALLBACKS,LOAD_FROM_UPDATE,MEMORY_TRACKING";

    enum BankLoadFlags {
        NORMAL_LOAD,
        NONBLOCKING,
        DECOMPRESS_SAMPLES,
        UNENCRYPTED
    };

    static FmodManager* get_singleton();

    FMOD::Studio::System *fmod_system;

    HashMap<String, FMOD::Studio::Bank *> loaded_banks;
    Vector<FmodEventInstance *> events = Vector<FmodEventInstance *>();

	void set_global_parameter(String p_name, float p_value);
	float get_global_parameter(String p_name);

    bool initialized = false;
	bool added_to_tree = false;

    void _notification(int p_what);
    Error initialize(InitFlags studio_flags, int max_channels);
	bool is_initialized();
	void auto_initialize(InitFlags init_flags, TypedArray<String> banks_to_load, int max_channels);
    void add_to_tree();

	Ref<FmodEventInstance> create_event_instance(String event_path);
    Ref<FmodVCA> get_vca(String vca_path);
    Ref<FmodEventInstance> play(String event_path);

    void randomize_seed();

    FmodManager();
    ~FmodManager();
};

VARIANT_ENUM_CAST(FmodManager::InitFlags);
VARIANT_ENUM_CAST(FmodManager::BankLoadFlags);

#endif
