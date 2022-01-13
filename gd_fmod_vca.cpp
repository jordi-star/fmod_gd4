//
// Created by jordi on 7/14/2021.
//

#include "gd_fmod_vca.h"

float FmodVCA::get_volume() {
    if(_vca != nullptr)
    {
        float r = 0;
        _vca->getVolume(&r, 0);
        return r;
    }
    else {
        return -1;
    }
}

void FmodVCA::set_volume(float volume) {
    if(_vca != nullptr)
    {
        _vca->setVolume(volume);
    }
}

String FmodVCA::get_path() {
    return path;
}

void FmodVCA::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_volume"), &FmodVCA::get_volume);
    ClassDB::bind_method(D_METHOD("set_volume", "set_volume"), &FmodVCA::set_volume);
    ClassDB::bind_method(D_METHOD("get_path"), &FmodVCA::get_path);

    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "volume"), "set_volume", "get_volume");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "path"), "", "get_path");
}

FmodVCA::FmodVCA() {}
