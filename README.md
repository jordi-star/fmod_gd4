# FMOD GD4 - Stable Branch
Godot 4.0 integration of FMOD. Not feature-complete yet.

This branch targets the latest stable Godot release (4.0.x) and FMOD 2.02.
This branch does not compile on 4.1. Check the `dev` branch for more expiramental releases.
# Installation
1. Clone `godot` and switch to 4.0.3:
> `git clone https://github.com/godotengine/godot`
> `cd godot && git checkout tags/4.0.3-stable`
2. Clone `fmod_gd4` into Godot's `modules/` folder.
> `cd modules && git clone https://github.com/summertimejordi/fmod_gd4`
3. Download [FMOD Engine 2.02+](https://www.fmod.com/download#FMOD%20Engine-select)
4. Copy FMOD API folders to the `api` folder in the `fmod_gd4` module.
5. [Compile Godot](https://docs.godotengine.org/en/stable/development/compiling/introduction_to_the_buildsystem.html?highlight=compile)

**Not done yet!**
Your newly built version of Godot will need to have the following libraries included in the same folder as it.

Locate the libraries by navigating to the `api` folder you copied into the module path.

**You will need:**
		The Fmod library located in `core/lib`, and the Fmod Studio library located in `studio/lib`.

You'll see folders named after the architecture they're designed for. (x86 for 32-Bit, x64 for 64-bit, and arm for Mac Sillicon)

**On Windows:**
	Copy `fmodL.dll` and `fmodstudioL.dll` to your Godot location.

**On Mac and Linux:**
	Copy `libfmodL.so` and `libfmodstudioL.so` to your Godot location.

**This process can be confusing. Create an issue or DM me on Discord `jordi ★#0317` for further support!**

# Usage
* Configure FMOD by visting `Project Settings/Fmod`
> Bank paths are relative to "res://": For example: we'd change `res://banks/Master.bank` to just `banks/Master.bank`.
* Create event instances using
```py
Fmod.create_event_instance(String event_path, bool autoplay, bool oneshot); # Returns FmodEventInstance
```
* Or:
```py
FmodEventInstance.create(String event_path);
```
```py
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
- [ ] Banks as resources
- [ ] Editor cache of Master.strings to use for FmodEventInstance editor plugin
- [ ] FmodEventInstance editor plugin

