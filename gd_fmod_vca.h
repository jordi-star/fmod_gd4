/*
Copyright 2022 Jordon Gulley

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

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
    ~FmodVCA();

};

#endif //GODOT_FMOD_VCA_H
