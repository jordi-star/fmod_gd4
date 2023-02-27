#include "fmod_event_resource.h"

void FmodEventResource::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_event_path", "value"), &FmodEventResource::set_event_path);
	ClassDB::bind_method(D_METHOD("get_event_path"), &FmodEventResource::get_event_path);
	ClassDB::bind_method(D_METHOD("play"), &FmodEventResource::play);
	ClassDB::bind_method(D_METHOD("instantiate"), &FmodEventResource::instantiate);
}

FMOD::Studio::EventDescription *FmodEventResource::get_event_description() const {
	return event_description;
}

void FmodEventResource::set_event_description() {
	if (event_path.is_empty()) {
		return;
	}
	FMOD::Studio::System *system = nullptr;
	if (Engine::get_singleton()->is_editor_hint()) {
		system = EditorFmodManager::get_singleton()->fmod_system;
	} else {
		ERR_FAIL_COND_MSG(!FmodManager::get_singleton()->initialized, vformat("Unable to get event description from FmodEventResource %s. FMOD not initialized!", event_path));
		system = FmodManager::get_singleton()->fmod_system;
	}
	FMOD_RESULT result = system->getEvent(event_path.utf8().ptr(), &event_description);
	parameters.clear();
	notify_property_list_changed();
	if (result != FMOD_OK) {
		return;
	}
    int parameter_count = 0;
	event_description->getParameterDescriptionCount(&parameter_count);
	for (int i = 0; i < parameter_count; i++) {
		FMOD_STUDIO_PARAMETER_DESCRIPTION parameter;
		event_description->getParameterDescriptionByIndex(i, &parameter);
		String prefix;
		if (parameter.flags & FMOD_STUDIO_PARAMETER_GLOBAL) {
			prefix = "global_parameters/";
		} else {
			prefix = "initial_parameter_values/";
		}
		parameters.insert(prefix + String(parameter.name), ParameterInitializer(parameter));
	}
}

void FmodEventResource::set_event_path(String value) {
	event_path = value;
	set_event_description();
	notify_property_list_changed();
}

String FmodEventResource::get_event_path() const {
	return event_path;
}

Ref<FmodEventInstance> FmodEventResource::instantiate() {
	Ref<FmodEventInstance> instance = memnew(FmodEventInstance);
	instance->initialize_from(event_description);
	set_params_on_instance(instance);
	return instance;
}

Ref<FmodEventInstance> FmodEventResource::play() {
	Ref<FmodEventInstance> instance = memnew(FmodEventInstance);
	instance->initialize_from(event_description);
	set_params_on_instance(instance);
	instance->play();
	return instance;
}

bool FmodEventResource::_set(const StringName &p_name, const Variant &p_value) {
	if (p_name == "event_path") {
		if (p_value.get_type() != Variant::Type::STRING) {
			return false;
		}
		set_event_path(p_value);
		return true;
	}
	if (parameters.has(p_name)) {
		if (!Variant::can_convert_strict(p_value.get_type(), Variant::Type::FLOAT)) {
			return false;
		}
		parameters[p_name].initial_value = p_value.operator float();
		return true;
	}
	return false;
}

bool FmodEventResource::_get(const StringName &p_name, Variant &r_ret) const {
	if (p_name == "event_path") {
		r_ret = get_event_path();
		return true;
	}
	if (parameters.has(p_name)) {
		r_ret = parameters[p_name].initial_value;
		return true;
	}
	return false;
}

void FmodEventResource::set_params_on_instance(Ref<FmodEventInstance> instance) {
	for (const KeyValue<String, ParameterInitializer> &PARAM : parameters) {
		if (PARAM.value.is_global) {
			FmodManager::get_singleton()->set_global_parameter(PARAM.value.parameter_description.name, PARAM.value.initial_value);
			continue;
		}
		instance->set_parameter(PARAM.value.parameter_description.name, PARAM.value.initial_value);
	}
}

void FmodEventResource::_get_property_list(List<PropertyInfo> *p_list) const {
	p_list->push_back(PropertyInfo(Variant::STRING, "event_path", PROPERTY_HINT_NONE, "fmod_event"));

    FMOD::Studio::EventDescription *event_desc = get_event_description();
    if (event_desc == nullptr) {
        return;
    }
	for (const KeyValue<String, ParameterInitializer> &PARAM : parameters) {
		int min = PARAM.value.parameter_description.minimum;
		int max = PARAM.value.parameter_description.maximum;
		float step = 0.01;
		PropertyHint property_hint = PROPERTY_HINT_RANGE;
		String hint_string = vformat("%s,%s,%s", min, max, step);
		uint32_t usage = PROPERTY_USAGE_DEFAULT;
		if (PARAM.value.parameter_description.type != FMOD_STUDIO_PARAMETER_GAME_CONTROLLED) {
			usage |= PROPERTY_USAGE_READ_ONLY;
		}
		Variant::Type value_type = Variant::Type::FLOAT;
		if (PARAM.value.is_labled()) {
			value_type = Variant::Type::INT;
			property_hint = PROPERTY_HINT_ENUM;
			hint_string = "";
			for (int i = 0; i <= max; i++) {
				int size = 0;
				event_desc->getParameterLabelByID(PARAM.value.parameter_description.id, i, nullptr, 0, &size);
				char *label = (char *)memalloc(size);
				event_desc->getParameterLabelByID(PARAM.value.parameter_description.id, i, label, size, nullptr);
				hint_string += String(label) + ",";
				memfree(label);
			}
			// Remove trailing comma
			hint_string.resize(hint_string.size() - 1);
		} else if (PARAM.value.is_discrete()) {
			value_type = Variant::Type::INT;
			step = 1.0;
		}
		p_list->push_back(PropertyInfo(value_type, PARAM.key, property_hint, hint_string, usage));
	}
}

bool FmodEventResource::_property_can_revert(const StringName &p_name) const {
	if (parameters.has(p_name)) {
		ParameterInitializer val = parameters.get(p_name);
		return val.initial_value != val.parameter_description.defaultvalue;
	}
	if (p_name == "event_path") {
		return get(p_name) != "";
	}
	return false;
}

bool FmodEventResource::_property_get_revert(const StringName &p_name, Variant &r_property) const {
	if (parameters.has(p_name)) {
		r_property = parameters.get(p_name).parameter_description.defaultvalue;
		return true;
	}
	if (p_name == "event_path") {
		r_property = String();
		return true;
	}
	return false;
}

FmodEventResource::FmodEventResource() {
	event_path = "";
	event_description = nullptr;
}

FmodEventResource::~FmodEventResource() {
}
