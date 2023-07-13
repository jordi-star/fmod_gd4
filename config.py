#config.py

AVAILABLE_PLATFORMS = ["windows", "macos", "linuxbsd"]

def can_build(env, platform):
    return platform in AVAILABLE_PLATFORMS

def configure(env):
    CORE_LIBRARY_NAME = "fmod"
    STUDIO_LIBRARY_NAME = "fmodstudio"
    if env["debug_symbols"]: # If debug enabled, link with logging/debug libraries instead of production libraries.
        CORE_LIBRARY_NAME += "L"
        STUDIO_LIBRARY_NAME += "L"
    if env["platform"] == "windows":
        if env["arch"]=="x86":
            env.Append(LINKFLAGS=[CORE_LIBRARY_NAME + ".dll", STUDIO_LIBRARY_NAME + ".dll"])
            env.Append(LIBPATH=["#modules/fmod_gd4/api/core/lib/x86/", "#modules/fmod_gd4/api/studio/lib/x86/"])
        else: # 64 bit
            env.Append(LINKFLAGS=[CORE_LIBRARY_NAME + "_vc.lib", STUDIO_LIBRARY_NAME + "_vc.lib"])
            env.Append(LIBPATH=["#modules/fmod_gd4/api/core/lib/x64/", "#modules/fmod_gd4/api/studio/lib/x64/"])
    elif env["platform"] == "macos":
        env.Append(
            LIBPATH=["#modules/fmod_gd4/api/core/lib/", "#modules/fmod_gd4/api/studio/lib/"])
        env.Append(LIBS=[CORE_LIBRARY_NAME, STUDIO_LIBRARY_NAME])
    elif env["platform"] == "linuxbsd":
        env.Append(
            LIBPATH=["#modules/fmod_gd4/api/core/lib/x86_64/", "#modules/fmod_gd4/api/studio/lib/x86_64/"])
        env.Append(LIBS=["lib" + CORE_LIBRARY_NAME, "lib" + STUDIO_LIBRARY_NAME])
        env.Append(RPATH=["\$$ORIGIN:."])

def get_doc_path():
    return "doc_classes"

def get_doc_classes():
    return [
        "FmodManager",
        "FmodEventInstance",
        "FmodVCA"
    ]
