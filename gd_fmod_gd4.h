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
