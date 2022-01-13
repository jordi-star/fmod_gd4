//
// Created by jordi on 7/14/2021.
//

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
	if(_event_instance != nullptr) {
		PlaybackState state = get_playback_state();
		if(state != PLAYING && state != PAUSED) {
			_event_instance->start();
		}
		else {
			_event_instance->setPaused(false);
		}
	}
	else {
		queue_delete();
	}
}
void FmodEventInstance::pause() {
	if(_event_instance != nullptr) {
		PlaybackState state = get_playback_state();
		if(state != PAUSED) {
			_event_instance->setPaused(true);
		}
	}
}

FmodEventInstance::PlaybackState FmodEventInstance::get_playback_state() {
    if(_event_instance != nullptr) {
        FMOD_STUDIO_PLAYBACK_STATE p_state = FMOD_STUDIO_PLAYBACK_STOPPED;
        _event_instance->getPlaybackState(&p_state);

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
            case FMOD_STUDIO_PLAYBACK_STOPPED: {
                state_to_return = STOPPED;
            } break;
		}
		return state_to_return;
	} else {
		return PlaybackState::STOPPED;
	}
}

void FmodEventInstance::stop(bool stop_immediately) {
    if(_event_instance != nullptr) {
        FMOD_STUDIO_STOP_MODE stop_mode;
        if(stop_immediately) {
            stop_mode = FMOD_STUDIO_STOP_IMMEDIATE;
        }
        else {
            stop_mode = FMOD_STUDIO_STOP_ALLOWFADEOUT;
        }
        _event_instance->stop(stop_mode);
    }
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
	queue_delete();
}

void FmodEventInstance::set_parameter(String p_name, float p_value) {
	if(_event_instance != nullptr) {
		_event_instance->setParameterByName(p_name.utf8(), p_value);
	}
}

float FmodEventInstance::get_parameter(String p_name) {
	float r = 0;
	if(_event_instance != nullptr) {
		_event_instance->getParameterByName(p_name.utf8(), 0, &r);
	}
	return r;
}

void FmodEventInstance::_notification(int p_notification) {
	if(p_notification == NOTIFICATION_PREDELETE) {
		if(_event_instance != nullptr) {
			_event_instance->release();
		}
	}
	else if(p_notification == NOTIFICATION_INTERNAL_PROCESS) {
		if(current_callback != -1) {
			//print_line("CALLBACK");
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
