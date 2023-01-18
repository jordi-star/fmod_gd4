/*
Copyright 2022 Jordon Gulley

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "gd_fmod_event_instance.h"

String FmodEventInstance::get_event_path() {
    return event_path;
}

void FmodEventInstance::set_event_path(String path) {

}

void FmodEventInstance::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_playback_state"), &FmodEventInstance::get_playback_state);
    ClassDB::bind_method(D_METHOD("stop", "stop_immediately"), &FmodEventInstance::stop);
	ClassDB::bind_method(D_METHOD("play"), &FmodEventInstance::play);
	ClassDB::bind_method(D_METHOD("pause"), &FmodEventInstance::pause);
    ClassDB::bind_method(D_METHOD("release_event"), &FmodEventInstance::release_event);
    ClassDB::bind_method(D_METHOD("perform_release"), &FmodEventInstance::perform_release);
	ClassDB::bind_method(D_METHOD("get_event_path"), &FmodEventInstance::get_event_path);
	ClassDB::bind_method(D_METHOD("set_event_path", "path"), &FmodEventInstance::set_event_path);
	ClassDB::bind_method(D_METHOD("get_parameter"), &FmodEventInstance::get_parameter);
    ClassDB::bind_method(D_METHOD("set_parameter", "name", "value"), &FmodEventInstance::set_parameter);

    ADD_PROPERTY(PropertyInfo(Variant::STRING, "event_path"), "set_event_path", "get_event_path");

	BIND_ENUM_CONSTANT(PLAYING);
	BIND_ENUM_CONSTANT(PAUSED);
    BIND_ENUM_CONSTANT(STOPPING);
    BIND_ENUM_CONSTANT(STOPPED);


	ADD_SIGNAL(MethodInfo("event_started"));
	ADD_SIGNAL(MethodInfo("event_stopped"));
	ADD_SIGNAL(MethodInfo("event_beat"));

}

void FmodEventInstance::set_callback() {
	_event_instance->setUserData(&current_callback);
	_event_instance->setCallback(FmodEventInstance::fmod_callback, FMOD_STUDIO_EVENT_CALLBACK_ALL);
}

FMOD_RESULT F_CALLBACK FmodEventInstance::fmod_callback(FMOD_STUDIO_EVENT_CALLBACK_TYPE type, FMOD_STUDIO_EVENTINSTANCE *event, void *parameters) {
	FMOD::Studio::EventInstance *instance = (FMOD::Studio::EventInstance *)event;
	int *e_inst = nullptr;

	instance->getUserData((void **)&e_inst);
    *e_inst = type;
	return FMOD_OK;
}

void FmodEventInstance::play() {
	if(_event_instance == nullptr) {
		queue_free();
		return;
	}
	PlaybackState state = get_playback_state();
	if(state == PAUSED) {
		_event_instance->setPaused(false);
		return;
	}
	_event_instance->start();
}

void FmodEventInstance::pause() {
	if(_event_instance == nullptr) {
		return;
	}
	PlaybackState state = get_playback_state();
	if(state != PAUSED) {
		_event_instance->setPaused(true);
	}
}

FmodEventInstance::PlaybackState FmodEventInstance::get_playback_state() {
    if(_event_instance == nullptr) {
		queue_free();
		return PlaybackState::STOPPED;
	}
    FMOD_STUDIO_PLAYBACK_STATE p_state = FMOD_STUDIO_PLAYBACK_STOPPED;
    _event_instance->getPlaybackState(&p_state);

    PlaybackState state_to_return = STOPPED;
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
        case FMOD_STUDIO_PLAYBACK_STOPPED: {
            state_to_return = STOPPED;
        } break;
	}
	return state_to_return;
}

void FmodEventInstance::stop(bool stop_immediately) {
    if(_event_instance == nullptr) {
		return;
    }
    _event_instance->stop(stop_immediately ? FMOD_STUDIO_STOP_IMMEDIATE : FMOD_STUDIO_STOP_ALLOWFADEOUT);
}

void FmodEventInstance::release_event() {
    perform_release();
}

void FmodEventInstance::perform_release() {
    if(_event_instance != nullptr) {
		if(get_playback_state() != STOPPED) {
			stop(true);
		}
        _event_instance->release();
    }
    queue_free();
}

void FmodEventInstance::set_parameter(String p_name, float p_value) {
	ERR_FAIL_COND_MSG(_event_instance == nullptr, vformat("Event instance invalid. Could not set paramter %s", p_name));
	FMOD_RESULT result = _event_instance->setParameterByName(p_name.utf8(), p_value);
	ERR_FAIL_COND_MSG(result != OK, vformat("An error occured while setting parameter %s on %s. FMOD_RESULT Error Code: %s", p_name, event_path, static_cast<int>(result)));
}

float FmodEventInstance::get_parameter(String p_name) {
	ERR_FAIL_COND_V_MSG(_event_instance == nullptr, -1.0, vformat("Event instance invalid. Could not get paramter %s", p_name));
	float r = 0;
	FMOD_RESULT result = _event_instance->getParameterByName(p_name.utf8(), 0, &r);
	ERR_FAIL_COND_V_MSG(result != OK, -1.0, vformat("An error occured while getting parameter %s on %s. FMOD_RESULT Error Code: %s", p_name, event_path, static_cast<int>(result)));
	return r;
}

void FmodEventInstance::_notification(int p_notification) {
	if(p_notification == NOTIFICATION_PREDELETE) {
		if(_event_instance != nullptr) {
			_event_instance->release();
		}
	}
	else if(p_notification != NOTIFICATION_INTERNAL_PROCESS) {
		return;
	}

	if(current_callback != -1) {
		switch(current_callback) {
			case FMOD_STUDIO_EVENT_CALLBACK_STARTED: {
				emit_signal("event_started");
			} break;
			case FMOD_STUDIO_EVENT_CALLBACK_SOUND_STOPPED: {
				emit_signal("event_stopped");
				if (one_shot) {
					perform_release();
				}
			} break;
			case FMOD_STUDIO_EVENT_CALLBACK_TIMELINE_BEAT: {
				current_beat += 1;
				emit_signal("event_beat", current_beat);
			} break;
		}
	}
	current_callback = -1;
	if(one_shot) {
		//print_line(vformat("Oneshot", 0));
		FMOD_STUDIO_PLAYBACK_STATE state = FMOD_STUDIO_PLAYBACK_STOPPED;
		FMOD_RESULT f = _event_instance->getPlaybackState(&state);
		if (f == FMOD_OK) {
			if (state == FMOD_STUDIO_PLAYBACK_STOPPED) {
				perform_release();
			}
		}
		else {
			perform_release();
		}
	}
}
//
//bool FmodEventInstance::unreference() {
//    if(_event_instance != nullptr) {
//        if (reference_get_count() - 1 <= 0) {
//            FMOD_STUDIO_PLAYBACK_STATE *state = nullptr;
//            _event_instance->getPlaybackState(state);
//            if(*state == FMOD_STUDIO_PLAYBACK_STOPPED) {
//				print_line("Unreferencing to zero");
//                return RefCounted::unreference();
//            }
//            else {
//                return false;
//            }
//        }
//        else {
//            return RefCounted::unreference();
//        }
//    }
//    else {
//        return RefCounted::unreference();
//    }
//}

FmodEventInstance::FmodEventInstance() {
	set_process_internal(true);
}
