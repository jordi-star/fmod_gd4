// WIP. All functionality is in place to implement this, the work just needs to be done.
// This editor intends to function similarly to the NodePath picker, using the cached event paths
// in EditorFmodManager
#ifndef GODOT_FMOD_EVENT_PATH_EDITOR_H
#define GODOT_FMOD_EVENT_PATH_EDITOR_H
#include "editor/editor_node.h"
#include "scene/gui/dialogs.h"
#include "scene/gui/item_list.h"
#include "scene/gui/line_edit.h"
#include "scene/gui/option_button.h"
#include "editor/editor_inspector.h"

class FmodEventPickerDialog : public ConfirmationDialog {
	GDCLASS(FmodEventPickerDialog, ConfirmationDialog);
	LineEdit *search_box = nullptr;
	ItemList *events = nullptr;

protected:
	static void _bind_methods();
	void item_activated(int _index);
	void dialog_confirmed();

public:
	void select(int _index_selected);
	void populate_events(String search_text);
	void _notification(int p_what);
	FmodEventPickerDialog();
	~FmodEventPickerDialog();
};

class FmodEventPathPicker : public EditorProperty {
	GDCLASS(FmodEventPathPicker, EditorProperty)
	Button *playback = nullptr;
	Button *assign = nullptr;
	FmodEventPickerDialog *picker = nullptr;

protected:
	static void _bind_methods();
public:
	void populate_events();
	void assign_event(String event);
	virtual void update_property() override;

	FmodEventPathPicker();
};
#endif // GODOT_FMOD_EVENT_PATH_EDITOR_H