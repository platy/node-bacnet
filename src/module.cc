#include <iostream>
#include <v8.h>
#include <nan.h>
#include "BacnetValue.h"
#include "functions.h"
#include "init.h"
#include "conversion.h"

using v8::Local;
using v8::Value;
using v8::Object;
using v8::Int32;
using v8::Uint32;
using v8::String;
using v8::FunctionTemplate;
using Nan::MaybeLocal;
using Nan::New;


// Configures and returns a bacnet instance - currently due to a lot of static c code, only one instance can exist at a time
NAN_METHOD(InitInstance) {
    Local<Object> configJs;
    if (info.Length() > 0 && !info[0]->IsUndefined()) {
        configJs = info[0]->ToObject();
    } else {
        configJs = New<Object>();
    }

    uint32_t device_instance_id = getUint32Default(configJs, "device_instance_id", 0);
    uint16_t ip_port = getUint32Default(configJs, "ip_port", 0xBAC0);
    uint16_t apdu_timeout = getUint32Default(configJs, "apdu_timeout", 3000);
    uint8_t apdu_retries = getUint32Default(configJs, "apdu_retries", 3);
    std::string iface = getStringOrEmpty(configJs, "iface");
    uint8_t invoke_id = getUint32Default(configJs, "invoke_id", 0);
    uint32_t bbmd_port = getUint32Default(configJs, "bbmd_port", 0xBAC0);
    uint32_t bbmd_ttl = getUint32Default(configJs, "bbmd_ttl", 0);
    std::string bbmd_address = getStringOrEmpty(configJs, "bbmd_address");

    struct BACNET_CONFIGURATION config = {device_instance_id, ip_port, apdu_timeout, apdu_retries, iface.c_str(), invoke_id, bbmd_port, bbmd_ttl, bbmd_address.c_str()};
    const char * errorMessage = init_bacnet(&config);
    if (errorMessage) {
        Nan::ThrowError(errorMessage);
        return;
    }

    Local<Object> target = New<Object>();
    Nan::Set(target, New("whois").ToLocalChecked(),
      Nan::GetFunction(New<FunctionTemplate>(whois)).ToLocalChecked());
    Nan::Set(target, New("isBound").ToLocalChecked(),
      Nan::GetFunction(New<FunctionTemplate>(isBound)).ToLocalChecked());
    Nan::Set(target, New("readProperty").ToLocalChecked(),
      Nan::GetFunction(New<FunctionTemplate>(readProperty)).ToLocalChecked());
    Nan::Set(target, New("writeProperty").ToLocalChecked(),
      Nan::GetFunction(New<FunctionTemplate>(writeProperty)).ToLocalChecked());
    Nan::Set(target, New("listen").ToLocalChecked(),
      Nan::GetFunction(New<FunctionTemplate>(listen)).ToLocalChecked());
    Nan::Set(target, New("initClient").ToLocalChecked(),
      Nan::GetFunction(New<FunctionTemplate>(initClient)).ToLocalChecked());
    Nan::Set(target, New("initDevice").ToLocalChecked(),
      Nan::GetFunction(New<FunctionTemplate>(initDevice)).ToLocalChecked());
    Nan::Set(target, New("closeQueue").ToLocalChecked(),
      Nan::GetFunction(New<FunctionTemplate>(closeQueue)).ToLocalChecked());

    info.GetReturnValue().Set(target);
}

NAN_MODULE_INIT(InitModule) {
    Nan::Set(target, New("init").ToLocalChecked(),
      Nan::GetFunction(New<FunctionTemplate>(InitInstance)).ToLocalChecked());
    Nan::Set(target, New("objectTypeToString").ToLocalChecked(),
      Nan::GetFunction(New<FunctionTemplate>(objectTypeToString)).ToLocalChecked());
    Nan::Set(target, New("objectTypeToNumber").ToLocalChecked(),
      Nan::GetFunction(New<FunctionTemplate>(objectTypeToNumber)).ToLocalChecked());
    Nan::Set(target, New("propertyKeyToString").ToLocalChecked(),
      Nan::GetFunction(New<FunctionTemplate>(propertyKeyToString)).ToLocalChecked());
    Nan::Set(target, New("propertyKeyToNumber").ToLocalChecked(),
      Nan::GetFunction(New<FunctionTemplate>(propertyKeyToNumber)).ToLocalChecked());

    BacnetValue::Init(target);
}

NODE_MODULE(binding, InitModule)
