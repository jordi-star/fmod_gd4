#include "fmod_bank_resource_loader.h"
// #include "../editor/editor_fmod_manager.h"

void FmodBankResourceLoader::_bind_methods() {
}

Ref<Resource> FmodBankResourceLoader::load(const String &p_path, const String &p_original_path, Error *r_error, bool p_use_sub_threads, float *r_progress, CacheMode p_cache_mode) {
	Ref<FmodBankResource> bank = memnew(FmodBankResource);
	Error err = bank->load(p_path);
	if (r_error != nullptr) {
		*r_error = err;
	}
	return bank;
}

void FmodBankResourceLoader::get_recognized_extensions(List<String> *r_extensions) const {
	r_extensions->push_back("bank");
}

bool FmodBankResourceLoader::handles_type(const String &p_type) const {
	return (p_type == "FmodBank");
}
String FmodBankResourceLoader::get_resource_type(const String &p_path) const {
	String el = p_path.get_extension().to_lower();
	if (el == "bank") {
		return "FmodBank";
	}
	return "";
}

FmodBankResourceLoader::FmodBankResourceLoader() {
}
