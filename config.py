#config.py

AVAILABLE_PLATFORMS = ["windows", "macos", "linuxbsd"]

def can_build(env, platform):
    return platform in AVAILABLE_PLATFORMS

def configure(env):
    if env["platform"] == "windows":
        if env["arch"]=="x86":
            env.Append(LINKFLAGS=["fmodL.dll", "fmodstudioL.dll"])
            env.Append(LIBPATH=["#modules/fmod_gd4/api/core/lib/x86/", "#modules/fmod_gd4/api/studio/lib/x86/"])
        else: # 64 bit
            env.Append(LINKFLAGS=["fmodL_vc.lib", "fmodstudioL_vc.lib"])
            env.Append(LIBPATH=["#modules/fmod_gd4/api/core/lib/x64/", "#modules/fmod_gd4/api/studio/lib/x64/"])
    elif env["platform"] == "macos":
        env.Append(
            LIBPATH=["#modules/fmod_gd4/api/core/lib/", "#modules/fmod_gd4/api/studio/lib/"])
        env.Append(LIBS=["fmodL", "fmodstudioL"])
    elif env["platform"] == "linuxbsd":
        env.Append(
            LIBPATH=["#modules/fmod_gd4/api/core/lib/x86_64/", "#modules/fmod_gd4/api/studio/lib/x86_64/"])
        env.Append(LIBS=["fmodL", "fmodstudioL"])

def get_doc_path():
    return "doc_classes"

def get_doc_classes():
    return [
        "FmodManager",
        "FmodEventInstance",
        "FmodVCA"
    ]
