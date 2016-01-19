#include <v8.h>
#include "rp.h"
#include "bacaddr.h"

using v8::Local;
using v8::Value;
using v8::Object;
using v8::Int32;
using v8::Uint32;
using v8::String;

uint32_t getUint32Default(Local<Object> target, std::string key, uint32_t defalt);
std::string getStringOrEmpty(Local<Object> target, std::string key);
std::string extractString(Local<String> jsString);

// Converts the BACNET_READ_PROPERTY_DATA to a js object
//{
//    object: {
//        type: 'device',
//        instance: 124123
//    },
//    property: 'object-list',
//    index: undefined,
//    error: undefined,
//    value: undefined
//}
Local<Object> readPropertyAckToJ(Nan::HandleScope *scope, BACNET_READ_PROPERTY_DATA * data);
Local<Object> bacnetAddressToJ(Nan::HandleScope *scope, BACNET_ADDRESS *src);
Local<String> abortReasonToJ(Nan::HandleScope *scope, uint8_t abortReason);
