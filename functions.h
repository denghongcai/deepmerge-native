#ifndef NATIVE_EXTENSION_GRAB_H
#define NATIVE_EXTENSION_GRAB_H

#include <nan.h>
#include <stack>

struct MergeStackElement {
    v8::Local<v8::Value> dst;
    v8::Local<v8::Value> src;
    v8::Local<v8::Value> target;
};

NAN_METHOD(DeepMerge);

#endif
