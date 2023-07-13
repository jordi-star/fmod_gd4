#include "fmod_event_path_property_editor.h"
#include "editor/editor_scale.h"
#include "editor_fmod_manager.h"

void FmodEventPathPicker::_bind_methods() {
}

void FmodEventPathPicker::populate_events() {
	if (picker == nullptr) {
		picker = memnew(FmodEventPickerDialog);
		add_child(picker);
		picker->connect("picked", callable_mp(this, &FmodEventPathPicker::assign_event));
	}
	picker->populate_events("");
	picker->popup_centered_clamped(Size2(350, 600) * EDSCALE);
}

void FmodEventPathPicker::assign_event(String event) {
	emit_changed(get_edited_property(), event);
}

void FmodEventPathPicker::update_property() {
	String event_path = get_edited_object()->get(get_edited_property());
	if (event_path.is_empty()) {
		assign->set_icon(EditorNode::get_singleton()->get_class_icon("AudioStreamMP3", "AudioStream"));
		assign->set_text("Assign Event...");
		assign->set_flat(false);
		return;
	}
	assign->set_flat(true);
	assign->set_text(event_path);
	assign->set_icon(EditorNode::get_singleton()->get_class_icon("AudioStreamPlayer"));
}

FmodEventPathPicker::FmodEventPathPicker() {
	HBoxContainer *container = memnew(HBoxContainer);
	container->add_theme_constant_override("separation", 0);
	add_child(container);
	assign = memnew(Button);
	assign->set_flat(true);
	assign->set_h_size_flags(SIZE_EXPAND_FILL);
	assign->set_clip_text(true);
	assign->connect("pressed", callable_mp(this, &FmodEventPathPicker::populate_events));
	container->add_child(assign);
	// Only create when first clicked.
	picker = nullptr;
}

void FmodEventPickerDialog::_bind_methods() {
	ADD_SIGNAL(MethodInfo("picked", PropertyInfo(Variant::STRING, "name")));
}

void FmodEventPickerDialog::item_activated(int index) {
	_ok_pressed();
}

void FmodEventPickerDialog::dialog_confirmed() {
	emit_signal("picked", events->get_item_text(events->get_selected_items()[0]));
	events->clear();
}

void FmodEventPickerDialog::populate_events(String search_text) {
	if (EditorFmodManager::get_singleton()->cache.dirty) {
		EditorFmodManager::get_singleton()->scan_for_banks();
	}
	events->clear();
	search_text = search_text.to_lower().replace(" ", "");
	PackedStringArray *event_paths = &EditorFmodManager::get_singleton()->cache.event_paths;
	for (int i = 0; i < event_paths->size(); i++) {
		const String event_path = event_paths->get(i);
		if (!search_text.is_empty() && event_path.to_lower().replace(" ", "").findn(search_text) == -1) {
			continue;
		}
		events->add_item(event_path);
	}
}

void FmodEventPickerDialog::select(int _index_selected) {
	get_ok_button()->set_disabled(false);
}

void FmodEventPickerDialog::_notification(int p_what) {
	if (p_what == NOTIFICATION_READY) {
		search_box->connect("text_changed", callable_mp(this, &FmodEventPickerDialog::populate_events));
		events->connect("item_selected", callable_mp(this, &FmodEventPickerDialog::select));
		events->connect("item_activated", callable_mp(this, &FmodEventPickerDialog::item_activated));
		connect("confirmed", callable_mp(this, &FmodEventPickerDialog::dialog_confirmed));
	}
}

FmodEventPickerDialog::FmodEventPickerDialog() {
	set_title("Pick Event...");
	VBoxContainer *container = memnew(VBoxContainer);
	add_child(container);
	search_box = memnew(LineEdit);
	container->add_margin_child("Search:", search_box);
	register_text_enter(search_box);
	events = memnew(ItemList);
	events->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	container->add_margin_child("Events:", events, true);
	set_ok_button_text("Assign");
	set_hide_on_ok(true);
	get_ok_button()->set_disabled(true);
}

FmodEventPickerDialog::~FmodEventPickerDialog() {
}
