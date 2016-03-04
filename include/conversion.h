#include <v8.h>
#include "rp.h"
#include "bacaddr.h"
#include "bacapp.h"

using v8::Local;
using v8::Value;
using v8::Object;
using v8::Int32;
using v8::Uint32;
using v8::String;

uint32_t getUint32Default(Local<Object> target, std::string key, uint32_t defalt);
std::string getStringOrEmpty(Local<Object> target, std::string key);
std::string extractString(Local<String> jsString);
BACNET_ADDRESS bacnetAddressToC(Local<Value> addressValue);
int bacnetAppValueToC(BACNET_APPLICATION_DATA_VALUE * cvalue, Local<Value> jvalue, BACNET_APPLICATION_TAG tag);
const char * objectTypeToC(Local<Value> type, unsigned * index);
const char * applicationTagToC(Local<Value> type, unsigned * index);
uint8_t inferBacnetType(Local<Value> jvalue);
bool addressOrBoundDeviceIdToC(Local<Value> value, unsigned * max_apdu, BACNET_ADDRESS * dest);


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
Local<Value> bacnetApplicationValueToJ(Nan::HandleScope *scope, BACNET_APPLICATION_DATA_VALUE * value);
Local<Object> bacnetAddressToJ(Nan::HandleScope *scope, BACNET_ADDRESS *src);
Local<String> abortReasonToJ(Nan::HandleScope *scope, uint8_t abortReason);
Local<String> rejectReasonToJ(Nan::HandleScope *scope, uint8_t rejectReason);
Local<String> errorCodesToJ(Nan::HandleScope *scope, BACNET_ERROR_CLASS error_class, BACNET_ERROR_CODE error_code);
