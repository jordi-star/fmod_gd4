/*
Copyright 2022 Jordon Gulley

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

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

FmodVCA::~FmodVCA() {
}

