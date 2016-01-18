#include <v8.h>

using v8::Local;
using v8::Value;
using v8::Object;
using v8::Int32;
using v8::Uint32;
using v8::String;

uint32_t getUint32Default(Local<Object> target, std::string key, uint32_t defalt);
std::string getStringOrEmpty(Local<Object> target, std::string key);
std::string extractString(Local<String> jsString);
