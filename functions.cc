#include "functions.h"

// TODO refactor to v8::Value member
v8::Local<v8::Value> Clone(v8::Local<v8::Value> src) {
    if (!src->IsObject()) {
        return Nan::New<v8::Value>(src);
    }
    std::stack<v8::Local<v8::Object>> stack;
    v8::Local<v8::Object> shallowSrc = Nan::To<v8::Object>(src).ToLocalChecked()->Clone();
    stack.push(shallowSrc);
    do {
        v8::Local<v8::Object> src = stack.top();
        stack.pop();
        if (src->IsObject()) {
            v8::Local<v8::Object> shallowSrc = src->Clone();
            v8::Local<v8::Array> keys = Nan::GetOwnPropertyNames(shallowSrc).ToLocalChecked();
            uint32_t length = keys->Length();
            for(uint32_t i = 0; i < length; i++) {
                v8::Local<v8::Value> key = Nan::Get(keys, i).ToLocalChecked();
                v8::Local<v8::Value> value = Nan::Get(shallowSrc, key).ToLocalChecked();
                if (value->IsObject()) {
                    stack.push(Nan::To<v8::Object>(value).ToLocalChecked());
                }
            }
            return shallowSrc;
        }
    } while (stack.size() > 0);
    return shallowSrc;
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
    std::stack<MergeStackElement> stack;
    MergeStackElement el;
    el.target = target;
    el.src = src;
    v8::Local<v8::Value> dst;
    if (src->IsArray()) {
        if (target->IsUndefined() || target->IsNull()) {
            dst = Nan::New<v8::Array>();
        } else {
            dst = Clone(target);
        }
        el.dst = dst;
    } else {
        if (!target->IsNull() && target->IsObject()) {
            dst = v8::Local<v8::Object>::Cast(Clone(target));
        } else {
            dst = Nan::New<v8::Object>();
        }
        el.dst = dst;
    }
    stack.push(el);
    do {
        MergeStackElement el = stack.top();
        stack.pop();
        v8::Local<v8::Value> src = el.src;
        v8::Local<v8::Value> target = el.target;
        v8::Local<v8::Value> dst = el.dst;
        const bool isSrcArray = src->IsArray();
        if (isSrcArray) {
            v8::Local<v8::Array> srcArray = v8::Local<v8::Array>::Cast(src);
            v8::Local<v8::Array> dstArray = v8::Local<v8::Array>::Cast(dst);
            uint32_t srcArrayLength = srcArray->Length();
            for(uint32_t i = 0; i < srcArrayLength; i++) {
                v8::Local<v8::Value> value = Nan::Get(srcArray, i).ToLocalChecked();
                if (Nan::Get(dstArray, i).ToLocalChecked()->IsUndefined()) {
                    Nan::Set(dstArray, i, value);
                } else if (value->IsObject()) {
                    target = Nan::Get(dstArray, i).ToLocalChecked();
                    MergeStackElement el;
                    el.src = value;
                    el.target = target;
                    if (value->IsArray()) {
                        if (target->IsUndefined() || target->IsNull()) {
                            el.dst = Nan::New<v8::Array>();
                        } else {
                            el.dst = Clone(target);
                        }
                    } else {
                        if (!target->IsNull() && target->IsObject()) {
                            el.dst = v8::Local<v8::Object>::Cast(Clone(target));
                        } else {
                            el.dst = Nan::New<v8::Object>();
                        }
                    }
                    Nan::Set(dstArray, i, el.dst);
                    stack.push(el);
                } else {
                    if (!IndexOf(dstArray, value)) {
                        Nan::Set(dstArray, dstArray->Length(), value); 
                    }
                }
            }
        } else {
            v8::Local<v8::Object> srcObj = v8::Local<v8::Object>::Cast(src);
            v8::Local<v8::Array> keys = Nan::GetOwnPropertyNames(srcObj).ToLocalChecked();
            v8::Local<v8::Object> dstObj = Nan::To<v8::Object>(dst).ToLocalChecked();
            uint32_t keysLength = keys->Length();
            for (uint32_t i = 0; i < keysLength; i++) {
                v8::Local<v8::Value> key = Nan::Get(keys, i).ToLocalChecked();
                v8::Local<v8::Value> value = Nan::Get(srcObj, key).ToLocalChecked();
                if (!value->IsObject() || value->IsNull()) {
                    Nan::Set(dstObj, key, value);
                } else {
                    v8::Local<v8::Object> targetObj = v8::Local<v8::Object>::Cast(target);
                    v8::Local<v8::Value> targetValue = Nan::Get(targetObj, key).ToLocalChecked();
                    if (targetValue->IsUndefined()) {
                        Nan::Set(dstObj, key, value);
                    } else {
                        MergeStackElement el;
                        el.src = value;
                        el.target = targetValue;
                        if (value->IsArray()) {
                            if (targetValue->IsUndefined() || targetValue->IsNull()) {
                                el.dst = Nan::New<v8::Array>();
                            } else {
                                el.dst = Clone(targetValue);
                            }
                        } else {
                            if (!targetValue->IsNull() && targetValue->IsObject()) {
                                el.dst = v8::Local<v8::Object>::Cast(Clone(targetValue));
                            } else {
                                el.dst = Nan::New<v8::Object>();
                            }
                        }
                        Nan::Set(dstObj, key, el.dst);
                        stack.push(el);
                    }
                }
            }
        }
    } while (stack.size() > 0);
    return dst;
}

NAN_METHOD(DeepMerge) {
    v8::Local<v8::Value> target = info[0];
    v8::Local<v8::Value> src = info[1];
    if (target->Equals(src)) {
        info.GetReturnValue().Set(Clone(target));
    } else {
        info.GetReturnValue().Set(DeepMergeRecursive(target, src));
    }
}
