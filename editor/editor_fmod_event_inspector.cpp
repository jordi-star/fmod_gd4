#include "editor_fmod_event_inspector.h"

bool EditorFmodEventInspector::can_handle(Object *p_object) {
	return Object::cast_to<FmodEventResource>(p_object) != nullptr;
}

bool EditorFmodEventInspector::parse_property(Object *p_object, const Variant::Type p_type, const String &p_path, const PropertyHint p_hint, const String &p_hint_text, const BitField<PropertyUsageFlags> p_usage, const bool p_wide) {
	if (p_hint_text == "fmod_event") {
		add_property_editor(p_path, memnew(FmodEventPathPicker));
		return true;
	}
	return false;
}
