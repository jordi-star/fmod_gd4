#ifndef GODOT_FMOD_BANK_RESOURCE_H
#define GODOT_FMOD_BANK_RESOURCE_H
#include "core/io/resource.h"
#include "core/config/project_settings.h"
#include "core/variant/typed_array.h"
#include "api/core/inc/fmod_common.h"
#include "../gd_fmod_gd4.h"

class FmodBankResource : public Resource {
	GDCLASS(FmodBankResource, Resource);

protected:
	static void _bind_methods();
	bool autoload = false;
	bool is_master = false;
	bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;

public:
	Error load_bank();
	void unload_bank();

	// Resource
	Error load(const String &path);
	static void set_autoload(const String &path, bool value);
	bool get_autoload() const;
	void _get_property_list(List<PropertyInfo> *p_list) const;
	FmodBankResource();
	~FmodBankResource();
};
#endif // GODOT_FMOD_BANK_RESOURCE_H
