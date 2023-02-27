#ifndef GODOT_FMOD_EVENT_INSP_H
#define GODOT_FMOD_EVENT_INSP_H
#include "editor/editor_inspector.h"
#include "fmod_event_path_property_editor.h"
#include "../resources/fmod_event_resource.h"

class EditorFmodEventInspector : public EditorInspectorPlugin {
public:
	virtual bool can_handle(Object *p_object);
	virtual bool parse_property(Object *p_object, const Variant::Type p_type, const String &p_path, const PropertyHint p_hint, const String &p_hint_text, const BitField<PropertyUsageFlags> p_usage, const bool p_wide = false) override;
};

#endif // GODOT_FMOD_EVENT_INSP_H