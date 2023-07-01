/*
Copyright 2022 Jordon Gulley

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "register_types.h"

#include "gd_fmod_gd4.h"
#ifdef TOOLS_ENABLED
#include "editor/editor_fmod_manager.h"
#endif
#include "resources/fmod_bank_resource_loader.h"
#include "resources/fmod_event_resource.h"

#include "core/config/project_settings.h"

static Ref<FmodBankResourceLoader> bank_resource_loader;

void initialize_fmod_gd4_module(ModuleInitializationLevel p_level) {
	switch (p_level) {
		case MODULE_INITIALIZATION_LEVEL_SERVERS: {
#ifdef TOOLS_ENABLED
			if (!Engine::get_singleton()->is_editor_hint()) {
				return;
			}
			memnew(EditorFmodManager);
			EditorNode::add_init_callback(&EditorFmodManager::init_callback);
#endif
		} break;
		case MODULE_INITIALIZATION_LEVEL_CORE: {
			bank_resource_loader.instantiate();
			ResourceLoader::add_resource_format_loader(bank_resource_loader);
		} break;
		case MODULE_INITIALIZATION_LEVEL_SCENE: {
			// Register classes
			GDREGISTER_CLASS(FmodManager);
			GDREGISTER_CLASS(FmodEventInstance);
			GDREGISTER_CLASS(FmodVCA);
			GDREGISTER_CLASS(FmodBankResource);
			GDREGISTER_CLASS(FmodEventResource);
			// Singleton
			Engine::get_singleton()->add_singleton(Engine::Singleton("Fmod", FmodManager::get_singleton()));
		} break;
		default: break;
	}
}

void uninitialize_fmod_gd4_module(ModuleInitializationLevel p_level) {
	switch (p_level) {
		case MODULE_INITIALIZATION_LEVEL_EDITOR: {
#ifdef TOOLS_ENABLED
			if (!Engine::get_singleton()->is_editor_hint()) {
				return;
			}
			memdelete(EditorFmodManager::get_singleton());
#endif
		} break;
		case MODULE_INITIALIZATION_LEVEL_CORE: {
			ResourceLoader::remove_resource_format_loader(bank_resource_loader);
			bank_resource_loader.unref();
		} break;
		case MODULE_INITIALIZATION_LEVEL_SCENE: {
			memdelete(FmodManager::get_singleton());
		} break;
		default: break;
	}
}
