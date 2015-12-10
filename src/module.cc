#include "functions.h"
#include "init.h"

using v8::FunctionTemplate;

// NativeExtension.cc represents the top level of the module.
// C++ constructs that are exposed to javascript are exported here

NAN_MODULE_INIT(InitAll) {
  init_bacnet();

  Nan::Set(target, Nan::New("whois").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(whois)).ToLocalChecked());
  Nan::Set(target, Nan::New("listen").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(listen)).ToLocalChecked());
  Nan::Set(target, Nan::New("initClient").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(initClient)).ToLocalChecked());
  Nan::Set(target, Nan::New("initDevice").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(initDevice)).ToLocalChecked());
}

NODE_MODULE(binding, InitAll)
