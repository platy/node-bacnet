#include <v8.h>
#include <nan.h>

using v8::Local;
using v8::Value;
using v8::Object;
using v8::Int32;
using v8::Uint32;
using v8::String;
using Nan::MaybeLocal;
using Nan::New;


uint32_t getUint32Default(Local<Object> target, std::string key, uint32_t defalt) {
    Local<String> lkey = New(key).ToLocalChecked();
    Local<Value> ldefault = New(defalt);
    Local<Value> lvalue = Nan::Get(target, lkey).FromMaybe(ldefault);
    return lvalue->ToUint32()->Value();
}

std::string extractString(Local<String> jsString) {
    Nan::Utf8String lstring(jsString);
    return *lstring;
}

std::string getStringOrEmpty(Local<Object> target, std::string key) {
    Local<String> lkey = New(key).ToLocalChecked();
    MaybeLocal<Value> mvalue = Nan::Get(target, lkey);
    if (mvalue.IsEmpty() || mvalue.ToLocalChecked()->IsUndefined()) {
     return "";
    } else {
        return extractString(mvalue.ToLocalChecked()->ToString());
    }
}
