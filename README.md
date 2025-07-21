# FMOD GD4 - Stable Branch
Godot 4.4 integration of FMOD. Not feature-complete yet.

This branch targets the latest stable Godot release (4.4.x) and FMOD 2.02.
# Installation
1. Clone `godot` and switch to 4.4:
> `git clone https://github.com/godotengine/godot`
> `cd godot && git checkout tags/4.4-stable`
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
	Copy `fmod.dll` and `fmodstudio.dll` to your Godot location. FMOD

**On Mac and Linux:**
	Copy `libfmod.so` and `libfmodstudio.so` to your Godot location.

If you'd like to enable debugging/logging for FMOD, copy the FMOD libraries that end with a capital 'L'. These are the ***L***ogging releases of FMOD.

**This process can be confusing. Create an issue for further support!**

# Usage
* Configure FMOD by visting `Project Settings/Fmod`
* To add banks to be loaded, navigate to the bank in the FileSystem tab, and enable `Autoload` in the inspector.
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

# Features:
- [x] Singleton and Autoinitialization
- [x] Playing events and loading banks
- [x] Get VCAs
- [x] Banks as resources
- [x] Editor cache of Master.strings to use for FmodEventInstance editor plugin
- [x] FmodEventInstance editor plugin

