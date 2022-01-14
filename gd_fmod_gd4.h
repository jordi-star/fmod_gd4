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

#include "scene/main/node.h"
#include "scene/main/window.h"
#include "core/variant/array.h"
#include "api/core/inc/fmod.hpp"
#include "api/studio/inc/fmod_studio.hpp"

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
    enum BankLoadFlags {
        NORMAL_LOAD,
        NONBLOCKING,
        DECOMPRESS_SAMPLES,
        UNENCRYPTED
    };

    static FmodManager* get_singleton();

    FMOD::Studio::System *f_system;

    Map<String, FMOD::Studio::Bank*> loaded_banks;
    Array events = Array();

    Array get_events();
    void set_events(Array a);

    bool initialized = false;

    void run_callbacks();
    void _notification(int p_what);
    Error initialize(int max_channels, InitFlags studio_flags);
    Error load_bank(String path_relative_to_project_root, BankLoadFlags flags);

	FmodEventInstance* create_event_instance(String event_path, bool autoplay = false, bool one_shot = false);
    FmodVCA* get_vca(String vca_path);
	FmodEventInstance* oneshot(String event_path, bool autoplay = false);
    FmodEventInstance* play(String event_path); // Convinence function for ease of use, alias for oneshot(event_path, true)

    void randomize_seed();

    FmodManager();
};

VARIANT_ENUM_CAST(FmodManager::InitFlags);
VARIANT_ENUM_CAST(FmodManager::BankLoadFlags);

#endif
