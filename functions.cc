#include "functions.h"

// TODO refactor to v8::Value member
v8::Local<v8::Value> Clone(v8::Local<v8::Value> src) {
    if (src->IsObject()) {
        v8::Local<v8::Object> shallowSrc = Nan::To<v8::Object>(src).ToLocalChecked()->Clone();
        v8::Local<v8::Array> keys = Nan::GetOwnPropertyNames(shallowSrc).ToLocalChecked();
        uint32_t length = keys->Length();
        for(uint32_t i = 0; i < length; i++) {
            v8::Local<v8::Value> key = Nan::Get(keys, i).ToLocalChecked();
            v8::Local<v8::Value> value = Nan::Get(shallowSrc, key).ToLocalChecked();
            if (value->IsObject()) {
                Nan::Set(shallowSrc, key, Clone(value));
            }
        }
        return shallowSrc;
    }
    return src;
}

// TODO refactor to v8::Array member
bool IndexOf(v8::Local<v8::Array> arr, v8::Local<v8::Value> value) {
    uint32_t arrLength = arr->Length();
    for(uint32_t i = 0; i < arrLength; i++) {
        v8::Local<v8::Value> curValue = Nan::Get(arr, i).ToLocalChecked();
        if (curValue->StrictEquals(value)) return true;
    }
    return false;
}

v8::Local<v8::Value> DeepMergeRecursive(v8::Local<v8::Value> target, v8::Local<v8::Value> src) {
    const bool isSrcArray = src->IsArray();
    if (isSrcArray) {
        v8::Local<v8::Value> dst;
        v8::Local<v8::Array> srcArray = v8::Local<v8::Array>::Cast(src);
        if (target->IsUndefined() || target->IsNull()) {
            dst = Nan::New<v8::Array>();
        } else {
            dst = Clone(target);
        }
        v8::Local<v8::Array> dstArray = v8::Local<v8::Array>::Cast(dst);
        uint32_t srcArrayLength = srcArray->Length();
        for(uint32_t i = 0; i < srcArrayLength; i++) {
            v8::Local<v8::Value> value = Nan::Get(srcArray, i).ToLocalChecked();
            if (Nan::Get(dstArray, i).ToLocalChecked()->IsUndefined()) {
                Nan::Set(dstArray, i, value);
            } else if (value->IsObject()) {
                Nan::Set(dstArray, i, DeepMergeRecursive(Nan::Get(dstArray, i).ToLocalChecked(), value));
            } else {
                if (!IndexOf(dstArray, value)) {
                    Nan::Set(dstArray, dstArray->Length(), value); 
                }
            }
        }
        return dst;
    } else {
        v8::Local<v8::Object> dst;
        if (!target->IsNull() && target->IsObject()) {
            dst = v8::Local<v8::Object>::Cast(Clone(target));
        } else {
            dst = Nan::New<v8::Object>();
        }
        v8::Local<v8::Object> srcObj = v8::Local<v8::Object>::Cast(src);
        v8::Local<v8::Array> keys = Nan::GetOwnPropertyNames(srcObj).ToLocalChecked();
        uint32_t keysLength = keys->Length();
        for (uint32_t i = 0; i < keysLength; i++) {
            v8::Local<v8::Value> key = Nan::Get(keys, i).ToLocalChecked();
            v8::Local<v8::Value> value = Nan::Get(srcObj, key).ToLocalChecked();
            if (!value->IsObject() || value->IsNull()) {
                Nan::Set(dst, key, value);
            } else {
                v8::Local<v8::Object> targetObj = v8::Local<v8::Object>::Cast(target);
                v8::Local<v8::Value> targetValue = Nan::Get(targetObj, key).ToLocalChecked();
                if (targetValue->IsUndefined()) {
                    Nan::Set(dst, key, value);
                } else {
                    Nan::Set(dst, key, DeepMergeRecursive(targetValue, value));
                }
            }
        }
        return v8::Local<v8::Value>::Cast(dst);
    }
}

NAN_METHOD(DeepMerge) {
    v8::Local<v8::Value> target = info[0];
    v8::Local<v8::Value> src = info[1];
    if (target->Equals(src)) {
        info.GetReturnValue().Set(target);
    } else {
        info.GetReturnValue().Set(DeepMergeRecursive(target, src));
    }
}
