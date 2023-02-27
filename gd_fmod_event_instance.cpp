/*
Copyright 2022 Jordon Gulley

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "gd_fmod_gd4.h"
#include "gd_fmod_event_instance.h"

void FmodEventInstance::_bind_methods() {
	ClassDB::bind_method(D_METHOD("play"), &FmodEventInstance::play);
	ClassDB::bind_method(D_METHOD("pause"), &FmodEventInstance::pause);
    ClassDB::bind_method(D_METHOD("stop", "stop_immediately"), &FmodEventInstance::stop);
    ClassDB::bind_method(D_METHOD("get_playback_state"), &FmodEventInstance::get_playback_state);
	ClassDB::bind_method(D_METHOD("get_event_path"), &FmodEventInstance::get_event_path);
	ClassDB::bind_method(D_METHOD("set_event_path", "path"), &FmodEventInstance::set_event_path);
	ClassDB::bind_method(D_METHOD("get_parameter"), &FmodEventInstance::get_parameter);
    ClassDB::bind_method(D_METHOD("set_parameter", "name", "value"), &FmodEventInstance::set_parameter);
    ClassDB::bind_method(D_METHOD("initialize", "event_path"), &FmodEventInstance::initialize);
    ClassDB::bind_static_method("FmodEventInstance", D_METHOD("create", "event_path"), &FmodEventInstance::create);

    ADD_PROPERTY(PropertyInfo(Variant::STRING, "event_path"), "set_event_path", "get_event_path");

	BIND_ENUM_CONSTANT(PLAYING);
	BIND_ENUM_CONSTANT(PAUSED);
    BIND_ENUM_CONSTANT(STOPPING);
    BIND_ENUM_CONSTANT(STOPPED);

	ADD_SIGNAL(MethodInfo("event_started"));
	ADD_SIGNAL(MethodInfo("event_stopped"));
	ADD_SIGNAL(MethodInfo("event_beat"));

}

String FmodEventInstance::get_event_path() {
    return event_path;
}

void FmodEventInstance::set_event_path(const String path) {
}

const bool FmodEventInstance::is_instance_valid() {
	return inner_event_instance != nullptr && inner_event_instance->isValid();
}

// Playback state
void FmodEventInstance::play() {
	if(inner_event_instance == nullptr) {
		return;
	}
	PlaybackState state = get_playback_state();
	if(state == PAUSED) {
		inner_event_instance->setPaused(false);
		return;
	}
	inner_event_instance->start();
}

void FmodEventInstance::pause() {
	if(!is_instance_valid()) {
		return;
	}
	PlaybackState state = get_playback_state();
	if(state != PAUSED) {
		inner_event_instance->setPaused(true);
	}
}

void FmodEventInstance::stop(const bool stop_immediately) {
    if(!is_instance_valid()) {
		return;
    }
    inner_event_instance->stop(stop_immediately ? FMOD_STUDIO_STOP_IMMEDIATE : FMOD_STUDIO_STOP_ALLOWFADEOUT);
}

FmodEventInstance::PlaybackState FmodEventInstance::get_playback_state() {
	ERR_FAIL_COND_V_MSG(!is_instance_valid(), PlaybackState::STOPPED, vformat("Could not get playback state. Event instance is invalid. (%s)", event_path));
    FMOD_STUDIO_PLAYBACK_STATE p_state = FMOD_STUDIO_PLAYBACK_STOPPED;
    inner_event_instance->getPlaybackState(&p_state);

    PlaybackState state_to_return;
    switch(p_state) {
		case FMOD_STUDIO_PLAYBACK_PLAYING: {
			state_to_return = PLAYING;
		} break;
		case FMOD_STUDIO_PLAYBACK_SUSTAINING: {
			state_to_return = PAUSED;
		} break;
        case FMOD_STUDIO_PLAYBACK_STOPPING: {
            state_to_return = STOPPING;
        } break;
        default: {
            state_to_return = STOPPED;
        } break;
	}
	return state_to_return;
}

// Parameters
void FmodEventInstance::set_parameter(const String p_name, const float p_value) {
	ERR_FAIL_COND_MSG(!is_instance_valid(), vformat("Event instance invalid. Could not set paramter %s", p_name));
	print_line(vformat("%s, %s", p_name, p_value));
	FMOD_RESULT result = inner_event_instance->setParameterByName(p_name.utf8(), p_value);
	ERR_FAIL_COND_MSG(result != FMOD_RESULT::FMOD_OK, vformat("An error occured while setting parameter %s on %s. FMOD_RESULT Error Code: %s", p_name, event_path, static_cast<int>(result)));
}

float FmodEventInstance::get_parameter(const String p_name) {
	ERR_FAIL_COND_V_MSG(!is_instance_valid(), -1.0, vformat("Event instance invalid. Could not get paramter %s", p_name));
	float r = 0;
	FMOD_RESULT result = inner_event_instance->getParameterByName(p_name.utf8(), 0, &r);
	ERR_FAIL_COND_V_MSG(result != FMOD_RESULT::FMOD_OK, -1.0, vformat("An error occured while getting parameter %s on %s. FMOD_RESULT Error Code: %s", p_name, event_path, static_cast<int>(result)));
	return r;
}

// Callbacks
void FmodEventInstance::activate_fmod_callback_reciever() {
	inner_event_instance->setUserData(&current_callback);
	inner_event_instance->setCallback(FmodEventInstance::fmod_callback, FMOD_STUDIO_EVENT_CALLBACK_ALL);
}

FMOD_RESULT F_CALLBACK FmodEventInstance::fmod_callback(FMOD_STUDIO_EVENT_CALLBACK_TYPE type, FMOD_STUDIO_EVENTINSTANCE *event, void *parameters) {
	FMOD::Studio::EventInstance *event_instance = reinterpret_cast<FMOD::Studio::EventInstance *>(event);
	if (event_instance == nullptr) {
		return FMOD_ERR_INVALID_HANDLE;
	}
	void *event_user_data = nullptr;
	event_instance->getUserData(&event_user_data);
	int *current_callback_ptr = static_cast<int *>(event_user_data);
	if (current_callback_ptr == nullptr) {
		return FMOD_ERR_INVALID_HANDLE;
	}
	*current_callback_ptr = type;
	return FMOD_OK;
}

const void FmodEventInstance::process_current_callback() {
	if (current_callback == -1) {
		return;
	}
	switch(current_callback) {
		case FMOD_STUDIO_EVENT_CALLBACK_STARTED: {
			emit_signal(SNAME("event_started"));
		} break;
		case FMOD_STUDIO_EVENT_CALLBACK_SOUND_STOPPED: {
			emit_signal(SNAME("event_stopped"));
		} break;
		case FMOD_STUDIO_EVENT_CALLBACK_TIMELINE_BEAT: {
			current_beat += 1;
			emit_signal(SNAME("event_beat"), current_beat);
		} break;
	}
	current_callback = -1;
}

// Lifetime
FmodEventInstance::FmodEventInstance() {
}

FmodEventInstance::~FmodEventInstance() {
	if(inner_event_instance != nullptr) {
		inner_event_instance->release();
	}
}

Ref<FmodEventInstance> FmodEventInstance::create(const String instance_event_path) {
	Ref<FmodEventInstance> event_instance = memnew(FmodEventInstance);
	event_instance->initialize(instance_event_path);
	return event_instance;
}

Error FmodEventInstance::initialize(const String instance_event_path) {
	ERR_FAIL_COND_V_MSG(!FmodManager::get_singleton()->initialized, Error::ERR_UNCONFIGURED, vformat("Could not initialize event instance (%s). Fmod is not initalized!", instance_event_path));
	ERR_FAIL_COND_V_MSG(is_instance_valid(), Error::ERR_ALREADY_IN_USE, vformat("Attempted to re-initialize a valid event instance! (%s)", instance_event_path));
	FMOD::Studio::EventDescription *e_desc = nullptr;
    FMOD_RESULT result = FmodManager::get_singleton()->fmod_system->getEvent(instance_event_path.utf8().get_data(), &e_desc);
	ERR_FAIL_NULL_V_MSG(e_desc, Error::ERR_QUERY_FAILED, vformat("Could not get EventDescription for: %s. FMOD_RESULT Error Code: %s", instance_event_path, result));

    FMOD::Studio::EventInstance *e_instance = nullptr;
    e_desc->createInstance(&e_instance);
	ERR_FAIL_NULL_V_MSG(e_instance, Error::ERR_CANT_CREATE, vformat("Could not create Event Instance for: %s. FMOD_RESULT Error Code: %s", instance_event_path, static_cast<int>(result)));

	this->inner_event_instance = e_instance;
	this->event_path = instance_event_path;
	activate_fmod_callback_reciever();
	FmodManager::get_singleton()->events.append(this);
	return Error::OK;
}

Error FmodEventInstance::initialize_from(const FMOD::Studio::EventDescription *e_desc) {
	ERR_FAIL_COND_V_MSG(!FmodManager::get_singleton()->initialized, Error::ERR_UNCONFIGURED, "Could not initialize event instance. Fmod is not initalized!");
	ERR_FAIL_COND_V_MSG(is_instance_valid(), Error::ERR_ALREADY_IN_USE, "Attempted to re-initialize a valid event instance!");

    FMOD::Studio::EventInstance *e_instance = nullptr;
    FMOD_RESULT result = e_desc->createInstance(&e_instance);
	ERR_FAIL_NULL_V_MSG(e_instance, Error::ERR_CANT_CREATE, vformat("Could not create Event Instance from existing event description. FMOD_RESULT: %s", result));

	this->inner_event_instance = e_instance;
	int size;
	e_desc->getPath(nullptr, 0, &size);
	char *path = (char *)memalloc(size);
	e_desc->getPath(path, size, nullptr);
	this->event_path = String(path);
	activate_fmod_callback_reciever();
	FmodManager::get_singleton()->events.append(this);
	return Error::OK;
}
