# FMOD GD4
Godot 4 integration of FMOD. Not feature-complete yet.

# Installation
1. Clone `godot`: 
> `git clone https://github.com/godotengine/godot`
2. Clone this repository into Godot's `modules/` folder. Rename the repository to `fmod`.
> `cd godot/modules && git clone https://github.com/summertimejordi/fmod_gd4`
3. Download [FMOD Engine](https://www.fmod.com/download#FMOD%20Engine-select)
3. Copy FMOD API folders to the `api` folder in the `fmod_gd4` module.
5. [Compile Godot](https://docs.godotengine.org/en/latest/development/compiling/introduction_to_the_buildsystem.html?highlight=compile)

# Usage
* Configure FMOD by visting `Project Settings/Fmod`
> Bank paths are relative to "res://": For example: we'd change `res://banks/Master.bank` to just `banks/Master.bank`. Autoinitialization automatically adds `.bank`, omit it when changing "Banks to load" (`Fmod/config/banks_to_load` in `ProjectSettings`)
* Create event instances using 
```py
Fmod.create_event_instance(String event_path, bool autoplay, bool oneshot); # Returns FmodEventInstance
```
* Or use the alias functions for readability:
```py
# Oneshots free themselves as soon as they completely stop.
Fmod.oneshot(String event_path, bool autoplay); # Returns FmodEventInstance
```
```py
# Alias for oneshot(event_path, true)
Fmod.play(String event_path); # Returns FmodEventInstance
```

# Advanced Usage
Sometimes we may want to initialize Fmod ourselves.
To do this we simply create a new script that extends FmodManager, then add that script as an Autoload. Disable Autoinitialization in `ProjectSettings/Fmod`.

Here's an example of a custom FmodManager in GD Script:
```swift
# MyCustomFmodManager.gd
extends FmodManager

func _ready():
	initialize(1024, FmodManager.NORMAL);
	load_bank("Master.bank", FmodManager.NORMAL_LOAD);
	load_bank("Master.strings.bank", FmodManager.NORMAL_LOAD);
	load_bank("Music.bank", FmodManager.NORMAL_LOAD);
```
You should be all set! Just know that you will need to use your autoload instead of the `Fmod` singleton when creating events, etc. (For this example, I'd call `MyCustomFmodManager.play(event_path)` rather than `Fmod.play(event_path)`)
#
Here's another example of a custom FmodManager that also adds a function that can fade in/out VCAs.
```swift
# FadingFmodManager.gd
extends FmodManager

var main_vca:FmodVCA;
var music_vca:FmodVCA;
var sfx_vca:FmodVCA;

func _ready():
	initialize(1024, FmodManager.NORMAL);
	load_bank("banks/Master.bank", FmodManager.NORMAL_LOAD);
	load_bank("banks/Master.strings.bank", FmodManager.NORMAL_LOAD);
	load_bank("banks/Music.bank", FmodManager.NORMAL_LOAD);
	load_bank("banks/SFX.bank", FmodManager.NORMAL_LOAD);
	
	main_vca = get_vca("vca:/Main");
	music_vca = get_vca("vca:/Music");
	sfx_vca = get_vca("vca:/SFX");

var fading_vca:String;
func _tw_vca_vol(val:float):
	match fading_vca:
		"main":
			main_vca.volume = val;
		"music":
			music_vca.volume = val;
		"sfx":
			sfx_vca.volume = val;

func fade_vca(vca:String, final_value:float, duration:float):
	var tw = create_tween();
	tw.stop();
	fading_vca = vca;
	match vca:
		"main", _:
			tw.tween_method(Callable(self, "_tw_vca_vol"), main_vca.volume, final_value, duration);
		"music":
			tw.tween_method(Callable(self, "_tw_vca_vol"), music_vca.volume, final_value, duration);
		"sfx":
			tw.tween_method(Callable(self, "_tw_vca_vol"), sfx_vca.volume, final_value, duration);
	tw.play();
	await tw.finished;
```


# Completed Features:
- [x] Singleton and Autoinitialization
- [x] Playing events and loading banks
- [x] Get VCAs

The rest of the FMOD API will be added in my free time :+1:, this project isn't a priority.
