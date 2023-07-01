/*
Copyright 2022 Jordon Gulley

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef GODOT_FMOD_EVENT_RES_H
#define GODOT_FMOD_EVENT_RES_H
#include "core/io/resource.h"
#ifdef TOOLS_ENABLED
#include "../editor/editor_fmod_manager.h"
#endif
#include "../gd_fmod_event_instance.h"
#include "../gd_fmod_gd4.h"

class FmodEventResource : public Resource {
	GDCLASS(FmodEventResource, Resource);
protected:
	static void _bind_methods();
	FMOD::Studio::EventDescription *event_description = nullptr;
	String event_path;
	struct ParameterInitializer {
		FMOD_STUDIO_PARAMETER_DESCRIPTION parameter_description;
		float initial_value;
		enum Kind {
			CONTINUOUS,
			DISCRETE,
			LABELED,
		};
		Kind kind = Kind::CONTINUOUS;

		bool is_global = false;
		bool is_labled() const { return kind == Kind::LABELED; }
		bool is_discrete() const { return kind == Kind::DISCRETE; }
		bool is_continuous() const { return kind == Kind::CONTINUOUS; }

		ParameterInitializer(FMOD_STUDIO_PARAMETER_DESCRIPTION description) {
			parameter_description = description;
			initial_value = 0.0;
			if (description.flags & FMOD_STUDIO_PARAMETER_GLOBAL) {
				is_global = true;
			}
			if (description.flags & FMOD_STUDIO_PARAMETER_LABELED) {
				kind = Kind::LABELED;
				return;
			}
			if (description.flags & FMOD_STUDIO_PARAMETER_DISCRETE) {
				kind = Kind::DISCRETE;
				return;
			}
		}
		ParameterInitializer() {
			initial_value = 0.0;
		}
	};
	HashMap<String, ParameterInitializer> parameters = HashMap<String, ParameterInitializer>();
	void set_event_description();
	bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;
	void set_params_on_instance(Ref<FmodEventInstance> instance);

	// TODO: Add more properties
	// For example, fetch event parameters and list them to be altered by this resource on instantiate.

public:
	FMOD::Studio::EventDescription *get_event_description() const;
	void set_event_path(String value);
	String get_event_path() const;

	// Create a new FMOD Event Instance
	Ref<FmodEventInstance> instantiate();
	// Play oneshot
	Ref<FmodEventInstance> play();
	void _get_property_list(List<PropertyInfo> *p_list) const;
	bool _property_can_revert(const StringName &p_name) const;
	bool _property_get_revert(const StringName &p_name, Variant &r_property) const;
	FmodEventResource();
	~FmodEventResource();
};

#endif // GODOT_FMOD_EVENT_RES_H