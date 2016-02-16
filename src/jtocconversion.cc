#include <iostream>
#include <v8.h>
#include <node.h>
#include <nan.h>
#include "address.h"
#include "datalink.h"
#include "bacapp.h"
#include "bactext.h"

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

// Currently converts an address string ie '123.123.123.123:456' to a bacnet address
// Non-strings create the broadcast address
// TODO : accept objects which specify the additional address properties - for which partial logic exists below
BACNET_ADDRESS bacnetAddressToC(Local<Value> addressValue) {
    BACNET_ADDRESS dest = { 0 };

    if (addressValue->IsString()) { // address object parameter
        BACNET_MAC_ADDRESS mac = { 0 };
        BACNET_MAC_ADDRESS adr = { 0 };
        long dnet = -1;
        char* dest_mac = 0;

        Nan::Utf8String address(addressValue.As<v8::String>());
        address_mac_from_ascii(&mac, *address);
//          if (strcmp(argv[argi], "--dnet") == 0) {
//              if (++argi < argc) {
//                  dnet = strtol(argv[argi], NULL, 0);
//                  if ((dnet >= 0) && (dnet <= BACNET_BROADCAST_NETWORK)) {
//                      global_broadcast = false;
//                  }
//              }
//          }
//          if (strcmp(argv[argi], "--dadr") == 0) {
//              if (++argi < argc) {
//                  if (address_mac_from_ascii(&adr, argv[argi])) {
//                      global_broadcast = false;
//                  }
//              }
//          }

        if (adr.len && mac.len) {
            memcpy(&dest.mac[0], &mac.adr[0], mac.len);
            dest.mac_len = mac.len;
            memcpy(&dest.adr[0], &adr.adr[0], adr.len);
            dest.len = adr.len;
            if ((dnet >= 0) && (dnet <= BACNET_BROADCAST_NETWORK)) {
                dest.net = dnet;
            } else {
                dest.net = BACNET_BROADCAST_NETWORK;
            }
        } else if (mac.len) {
            memcpy(&dest.mac[0], &mac.adr[0], mac.len);
            dest.mac_len = mac.len;
            dest.len = 0;
            if ((dnet >= 0) && (dnet <= BACNET_BROADCAST_NETWORK)) {
                dest.net = dnet;
            } else {
                 dest.net = 0;
            }
        } else {
            if ((dnet >= 0) && (dnet <= BACNET_BROADCAST_NETWORK)) {
                dest.net = dnet;
            } else {
                dest.net = BACNET_BROADCAST_NETWORK;
            }
            dest.mac_len = 0;
            dest.len = 0;
        }
    } else {
        datalink_get_broadcast_address(&dest);
    }
    return dest;
}

// For enums we'll need to know the context - ie properties and stuff
// This shouldn't be called for arrays as we dont know whether the return value is an array
uint8_t inferBacnetType(Local<Value> jvalue) {
    if (jvalue->IsNull()) {
        return BACNET_APPLICATION_TAG_NULL;
    } else if (jvalue->IsBoolean()) {
        return BACNET_APPLICATION_TAG_BOOLEAN;
    } else if (jvalue->IsString()) {
        return BACNET_APPLICATION_TAG_CHARACTER_STRING;
    } else if (jvalue->IsInt32()) {
        return BACNET_APPLICATION_TAG_SIGNED_INT;
    } else if (jvalue->IsUint32()) {
        return BACNET_APPLICATION_TAG_UNSIGNED_INT;
    } else if (jvalue->IsNumber()) {
        return BACNET_APPLICATION_TAG_DOUBLE; // I don't think there's a reason to use float instead
    } else {
        return 1; // Error : unsupported type (array, object, symbol)
    }
    return 0; // Success
}

// For enums we'll need to know the context - ie properties and stuff
// This shouldn't be called for arrays as we dont know whether the return value is an array
// TODO check types etc
int bacnetAppValueToC(BACNET_APPLICATION_DATA_VALUE * cvalue, Local<Value> jvalue, BACNET_APPLICATION_TAG tag) {
    cvalue->tag = tag;
    switch (tag) {
    case BACNET_APPLICATION_TAG_NULL:
        return 0;
    case BACNET_APPLICATION_TAG_BOOLEAN:
        cvalue->type.Boolean = Nan::To<bool>(jvalue).FromJust();
        return 0;
    case BACNET_APPLICATION_TAG_UNSIGNED_INT:
        cvalue->type.Unsigned_Int = Nan::To<uint32_t>(jvalue).FromJust();
        return 0;
    case BACNET_APPLICATION_TAG_SIGNED_INT:
        cvalue->type.Signed_Int = Nan::To<int32_t>(jvalue).FromJust();
        return 0;
    case BACNET_APPLICATION_TAG_REAL:
        cvalue->type.Real = Nan::To<double>(jvalue).FromJust();
        return 0;
    case BACNET_APPLICATION_TAG_DOUBLE:
        cvalue->type.Double = Nan::To<double>(jvalue).FromJust();
        return 0;
    case BACNET_APPLICATION_TAG_OCTET_STRING:
        {
        Local<Object> bjvalue = Nan::To<Object>(jvalue).ToLocalChecked();
        size_t bufferLength = node::Buffer::Length(bjvalue);
        if (bufferLength < MAX_OCTET_STRING_BYTES) {
            cvalue->type.Octet_String.length = bufferLength;
            memcpy(node::Buffer::Data(bjvalue), cvalue->type.Octet_String.value, bufferLength);
            return 0;
        } else {
            return 2; // Error: Too large
        }
        }
    case BACNET_APPLICATION_TAG_CHARACTER_STRING:
        Local<String> sjvalue = Nan::To<String>(jvalue).ToLocalChecked();
        if (sjvalue->Utf8Length() < MAX_CHARACTER_STRING_BYTES) {
            cvalue->type.Character_String.length = sjvalue->Utf8Length();
            cvalue->type.Character_String.encoding = CHARACTER_UTF8;
            sjvalue->WriteUtf8(cvalue->type.Character_String.value);
            return 0;
        } else {
            return 2; // Error: Too large
        }
//    case BACNET_APPLICATION_TAG_BIT_STRING:
//        return bitStringToBuffer(scope, value->type.Bit_String);
//    case BACNET_APPLICATION_TAG_ENUMERATED:
//        return Nan::New(value->type.Enumerated); // without the context of the property id the enumeration value is just a number
//    case BACNET_APPLICATION_TAG_DATE:
//        return bacnetDateToJ(scope, &value->type.Date);
//    case BACNET_APPLICATION_TAG_TIME:
//        return bacnetTimeToJ(scope, &value->type.Time);
//    case BACNET_APPLICATION_TAG_OBJECT_ID:
//        return objectHandleToJ(scope, (BACNET_OBJECT_TYPE) value->type.Object_Id.type, value->type.Object_Id.instance);
    }
    std::cout << "ERROR: value tag (" << +cvalue->tag << ") not converted to js '" << bactext_application_tag_name(cvalue->tag) << "'" << std::endl;
    return 1;
}
