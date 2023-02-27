#include "fmod_bank_resource.h"

void FmodBankResource::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_autoload"), &FmodBankResource::get_autoload);
	ClassDB::bind_method(D_METHOD("load"), &FmodBankResource::load_bank);
	ClassDB::bind_method(D_METHOD("unload"), &FmodBankResource::unload_bank);
}

bool FmodBankResource::_set(const StringName &p_name, const Variant &p_value) {
	if (p_name == "autoload") {
		set_autoload(get_path(), p_value);
		notify_property_list_changed();
		return true;
	}
	return false;
}

bool FmodBankResource::_get(const StringName &p_name, Variant &r_ret) const {
	if (p_name == "autoload") {
		r_ret = get_autoload();
		return true;
	}
	return false;
}

void FmodBankResource::set_autoload(const String &path, bool value) {
	TypedArray<String> autoloaded_banks = GLOBAL_DEF_BASIC("fmod/config/banks_to_autoload", TypedArray<String>());
	int position_in_autoloads = autoloaded_banks.find(path);
	if (value) {
		if (position_in_autoloads != -1) { // If autoload exists already
			return;
		}
		autoloaded_banks.push_back(path);
	} else {
		if (position_in_autoloads == -1) { // If autoload doesn't exist
			return;
		}
		autoloaded_banks.remove_at(position_in_autoloads);
	}
	ProjectSettings::get_singleton()->set_setting("fmod/config/banks_to_autoload", autoloaded_banks);
	ProjectSettings::get_singleton()->save();
}

bool FmodBankResource::get_autoload() const {
	TypedArray<String> autoloaded_banks = GLOBAL_DEF_BASIC("fmod/config/banks_to_autoload", TypedArray<String>());
	return autoloaded_banks.has(get_path());
}

Error FmodBankResource::load_bank() {
	ERR_FAIL_COND_V_MSG(!FmodManager::get_singleton()->initialized, Error::ERR_UNCONFIGURED, "Unable to load bank. Fmod not initalized!");
	ERR_FAIL_COND_V_MSG(FmodManager::get_singleton()->loaded_banks.has(get_path()), ERR_ALREADY_EXISTS, vformat("Unable to load bank. Bank at '%s' already loaded!", get_path()));

	// Open bank file
	Error file_access_err;
	Ref<FileAccess> file = FileAccess::open(get_path(), FileAccess::READ, &file_access_err);
	if (file_access_err != Error::OK) {
		return file_access_err;
	}

	// Load bank into memory
	int length = file->get_length();
	char *buffer = (char *)memalloc(length);
	file->get_buffer((uint8_t *)buffer, length);

	// Load bank into FMOD *from* memory.
	FMOD::Studio::Bank *bank = nullptr;
	FMOD_RESULT result = FmodManager::get_singleton()->fmod_system->loadBankMemory(buffer, length, FMOD_STUDIO_LOAD_MEMORY, FMOD_STUDIO_LOAD_BANK_NORMAL, &bank);
	ERR_FAIL_COND_V_MSG(result != FMOD_OK, ERR_CANT_OPEN, vformat("Unable to load bank at %s. FMOD_RESULT error code: %s", get_path(), static_cast<int>(result)));

	// Add bank to loaded_banks map.
    FmodManager::get_singleton()->loaded_banks.insert(get_path(), bank);
	return Error::OK;
}

void FmodBankResource::unload_bank() {
	ERR_FAIL_COND_MSG(!FmodManager::get_singleton()->loaded_banks.has(get_path()), vformat("Unable to unload bank. Bank at '%s' is not loaded!", get_path()));
    FMOD::Studio::Bank *bank = FmodManager::get_singleton()->loaded_banks[get_path()];
	bank->unload();
	memdelete(bank);
	FmodManager::get_singleton()->loaded_banks.erase(get_path());
}

Error FmodBankResource::load(const String &path) {
	set_path(path);
	if (!Engine::get_singleton()->is_editor_hint()) {
		load_bank();
		return Error::OK;
	}
	if (path.contains("Master")) {
		set_autoload(get_path(), true);
		notify_property_list_changed();
		is_master = true;
	}
	return Error::OK;
}

void FmodBankResource::_get_property_list(List<PropertyInfo> *p_list) const {
	uint32_t usage = PROPERTY_USAGE_DEFAULT;
	if (is_master) {
		usage |= PROPERTY_USAGE_READ_ONLY;
	}
	p_list->push_back(PropertyInfo(Variant::BOOL, "autoload", PROPERTY_HINT_NONE, "", usage));
}

FmodBankResource::FmodBankResource() {
	is_master = false;
}

FmodBankResource::~FmodBankResource() {
}
