//
// Created by jordi on 7/14/2021.
//

#ifndef GODOT_FMOD_VCA_H
#define GODOT_FMOD_VCA_H

#include "scene/2d/node_2d.h"
#include "api/core/inc/fmod.hpp"
#include "api/studio/inc/fmod_studio.hpp"

class FmodVCA : public RefCounted {
	GDCLASS(FmodVCA, RefCounted);

protected:
    static void _bind_methods();

public:
    FMOD::Studio::VCA *_vca = nullptr;
    void set_volume(float volume);
    float get_volume();
    String path;
    String get_path();
    FmodVCA();

};

#endif //GODOT_FMOD_VCA_H
