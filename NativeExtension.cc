#include "functions.h"

using v8::FunctionTemplate;
using v8::Local;
using v8::Object;

void Init(Local<Object> exports, Local<Object> module) {
    Nan::Set(module, Nan::New("exports").ToLocalChecked(),
        Nan::GetFunction(Nan::New<FunctionTemplate>(DeepMerge)).ToLocalChecked());
}

NODE_MODULE(NativeExtension, Init)
