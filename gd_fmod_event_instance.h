/*
Copyright 2022 Jordon Gulley

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef GODOT_FMOD_EVENT_INSTANCE_H
#define GODOT_FMOD_EVENT_INSTANCE_H

#include "core/object/ref_counted.h"
#include "api/core/inc/fmod.hpp"
#include "api/studio/inc/fmod_studio.hpp"

class FmodManager;
class FmodEventInstance : public RefCounted {
	GDCLASS(FmodEventInstance, RefCounted);

protected:
    static void _bind_methods();
    FMOD::Studio::EventInstance *inner_event_instance = nullptr;

public:
    bool is_instance_valid();

    String event_path;

    String get_event_path();
    void set_event_path(const String path);

    // Playback State
    enum PlaybackState {
		PLAYING,
		PAUSED,
        STOPPING,
        STOPPED
    };
	void play();
	void pause();
	bool is_paused();
	void set_is_paused(bool value);
	void stop(const bool stop_immediately = false);
	PlaybackState get_playback_state();

    // Parameters
	void set_parameter(const String p_name, const float p_value);
	float get_parameter(const String p_name);

    // Callbacks
    FMOD_STUDIO_EVENT_CALLBACK_TYPE current_callback = 0;
    int current_beat = 0;
	void activate_fmod_callback_reciever();
	static FMOD_RESULT F_CALL fmod_callback(FMOD_STUDIO_EVENT_CALLBACK_TYPE type, FMOD_STUDIO_EVENTINSTANCE *event, void *parameters);
    void process_current_callback();

    Error initialize(const String event_path);
	Error initialize_from(const FMOD::Studio::EventDescription *e_desc);
	static Ref<FmodEventInstance> create(const String event_path);
	FmodEventInstance();
    ~FmodEventInstance();
};

VARIANT_ENUM_CAST(FmodEventInstance::PlaybackState);

#endif //GODOT_FMOD_EVENT_INSTANCE_H
