//
// Created by jordi on 7/14/2021.
//

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
