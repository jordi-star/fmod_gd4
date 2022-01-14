/*
Copyright 2022 Jordon Gulley

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef GODOT_FMOD_EVENT_INSTANCE_H
#define GODOT_FMOD_EVENT_INSTANCE_H

#include "scene/main/node.h"
#include "api/core/inc/fmod.hpp"
#include "api/studio/inc/fmod_studio.hpp"

class FmodEventInstance : public Node {
	GDCLASS(FmodEventInstance, Node);

protected:
    static void _bind_methods();
    void _notification(int p_what);

public:
    int current_callback = -1;
    //auto manager;
    FMOD::Studio::EventInstance *_event_instance = nullptr;
    enum PlaybackState {
		PLAYING,
		PAUSED,
        STOPPING,
        STOPPED
    };

    String event_path;

    String get_event_path();
    void set_event_path(String path);

    PlaybackState get_playback_state();
	void stop(bool stop_immediately=false);
	void set_parameter(String p_name, float p_value);
	float get_parameter(String p_name);
	void set_callback();
    void release_event();
	void play();
	void pause();

	bool one_shot = false;

    int current_beat = 0;

	static FMOD_RESULT F_CALLBACK fmod_callback(FMOD_STUDIO_EVENT_CALLBACK_TYPE type, FMOD_STUDIO_EVENTINSTANCE *event, void *parameters);

    FmodEventInstance();

    void perform_release();

    void read_callback();

    int get_signal();

    void set_signal(int t);
};

VARIANT_ENUM_CAST(FmodEventInstance::PlaybackState);

#endif //GODOT_FMOD_EVENT_INSTANCE_H
