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
    BACNET_ADDRESS dest = {};

    if (addressValue->IsString()) { // address object parameter
        BACNET_MAC_ADDRESS mac = {};
        BACNET_MAC_ADDRESS adr = {};
        long dnet = -1;

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
    } else if (node::Buffer::HasInstance(jvalue)) {
        return BACNET_APPLICATION_TAG_OCTET_STRING;
    } else if (jvalue->IsUint32()) {        // the 4 number domains have overlap, for this inference we're using unsigned int, signed int, double as the precedence
        return BACNET_APPLICATION_TAG_UNSIGNED_INT;
    } else if (jvalue->IsInt32()) {
        return BACNET_APPLICATION_TAG_SIGNED_INT;
    } else if (jvalue->IsNumber()) {
        return BACNET_APPLICATION_TAG_REAL; // With my test device a double was an invalid type - it seems that real is more common
    } else if (jvalue->IsObject()) {
        Local<Object> jobject = jvalue->ToObject();
        if (Nan::Has(jobject, Nan::New("year").ToLocalChecked()).FromMaybe(false)) {
            return BACNET_APPLICATION_TAG_DATE;
        } else if (Nan::Has(jobject, Nan::New("hour").ToLocalChecked()).FromMaybe(false)) {
            return BACNET_APPLICATION_TAG_TIME;
        } else if (Nan::Has(jobject, Nan::New("instance").ToLocalChecked()).FromMaybe(false)) {
            return BACNET_APPLICATION_TAG_OBJECT_ID;
        } else {
            return 255; // Error : unsupported object type (array, object, ...)
        }
    } else {
        return 255; // Error : unsupported primitive (symbol, ...)
    }
}

int bacnetOctetStringToC(BACNET_OCTET_STRING * cvalue, Local<Value> jvalue) {
    Local<Object> bjvalue = Nan::To<Object>(jvalue).ToLocalChecked();
    size_t bufferLength = node::Buffer::Length(bjvalue);
    if (bufferLength < MAX_OCTET_STRING_BYTES) {
        cvalue->length = bufferLength;
        memcpy(node::Buffer::Data(bjvalue), cvalue->value, bufferLength);
        return 0;
    } else {
        return 2; // Error: Too large
    }
}

int bacnetCharacterStringToC(BACNET_CHARACTER_STRING * cvalue, Local<Value> jvalue) {
    Local<String> sjvalue = Nan::To<String>(jvalue).ToLocalChecked();
    if (sjvalue->Utf8Length() < MAX_CHARACTER_STRING_BYTES) {
        cvalue->length = sjvalue->Utf8Length();
        cvalue->encoding = CHARACTER_UTF8;
        sjvalue->WriteUtf8(cvalue->value);
        return 0;
    } else {
        return 2; // Error: Too large
    }
}

int bacnetBitStringToC(BACNET_BIT_STRING * cvalue, Local<Value> jvalue) {
    Local<Object> bjvalue = Nan::To<Object>(jvalue).ToLocalChecked();
    size_t bufferLength = node::Buffer::Length(bjvalue);
    if (bufferLength < MAX_BITSTRING_BYTES) {
        cvalue->bits_used = bufferLength * 8;
        memcpy(node::Buffer::Data(bjvalue), cvalue->value, bufferLength);
            return 0;
    } else {
        return 2; // Error: Too large
    }
}

int bacnetDateToC(BACNET_DATE * cvalue, Local<Value> jvalue) {
    Local<Object> jobject = Nan::To<Object>(jvalue).ToLocalChecked();
    cvalue->year = Nan::To<uint32_t>(Nan::Get(jobject, Nan::New("year").ToLocalChecked()).ToLocalChecked()).FromJust();
    cvalue->month = Nan::To<uint32_t>(Nan::Get(jobject, Nan::New("month").ToLocalChecked()).ToLocalChecked()).FromJust();
    cvalue->day = Nan::To<uint32_t>(Nan::Get(jobject, Nan::New("day").ToLocalChecked()).ToLocalChecked()).FromJust();
    std::string weekdaystring = getStringOrEmpty(jobject, "weekday");
    unsigned int wday;
    if (bactext_days_of_week_index(weekdaystring.c_str(), &wday)) {
        cvalue->wday = wday + 1; // BACnet has 2 different enumeration schemes for weekdays, oe starting Monday=1 - that's the one we use
        return 0;
    } else {
        return 3;
    }
}

int bacnetTimeToC(BACNET_TIME * cvalue, Local<Value> jvalue) {
    Local<Object> jobject = Nan::To<Object>(jvalue).ToLocalChecked();
    cvalue->hour = Nan::To<uint32_t>(Nan::Get(jobject, Nan::New("hour").ToLocalChecked()).ToLocalChecked()).FromJust();
    cvalue->min = Nan::To<uint32_t>(Nan::Get(jobject, Nan::New("min").ToLocalChecked()).ToLocalChecked()).FromJust();
    cvalue->sec = Nan::To<uint32_t>(Nan::Get(jobject, Nan::New("sec").ToLocalChecked()).ToLocalChecked()).FromJust();
    cvalue->hundredths = Nan::To<uint32_t>(Nan::Get(jobject, Nan::New("hundredths").ToLocalChecked()).ToLocalChecked()).FromJust();
    return 0;
}

int bacnetObjectHandleToC(BACNET_OBJECT_ID * cvalue, Local<Value> jvalue) {
    Local<Object> jobject = Nan::To<Object>(jvalue).ToLocalChecked();
    std::string objectTypeName = getStringOrEmpty(jobject, "type");
    unsigned objectTypeIndex;
    if (bactext_object_type_index(objectTypeName.c_str(), &objectTypeIndex)) {
        cvalue->type = objectTypeIndex;
        cvalue->instance = Nan::To<uint32_t>(Nan::Get(jobject, Nan::New("instance").ToLocalChecked()).ToLocalChecked()).FromJust();
        return 0;
    } else {
        return 3;
    }
}

// For enums we'll need to know the context - ie properties and stuff
// This shouldn't be called for arrays as we dont know whether the return value is an array
// TODO check types etc
int bacnetAppValueToC(BACNET_APPLICATION_DATA_VALUE * cvalue, Local<Value> jvalue, BACNET_APPLICATION_TAG tag) {
    cvalue->context_specific = false;
    cvalue->context_tag = 0;
    cvalue->next = 0;
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
        return bacnetOctetStringToC(&cvalue->type.Octet_String, jvalue);
    case BACNET_APPLICATION_TAG_CHARACTER_STRING:
        return bacnetCharacterStringToC(&cvalue->type.Character_String, jvalue);
    case BACNET_APPLICATION_TAG_BIT_STRING:
        return bacnetBitStringToC(&cvalue->type.Bit_String, jvalue);
    case BACNET_APPLICATION_TAG_ENUMERATED:
        cvalue->type.Enumerated = Nan::To<uint32_t>(jvalue).FromJust(); // without the context of the property id the enumeration value is just a number
        return 0;
    case BACNET_APPLICATION_TAG_DATE:
        return bacnetDateToC(&cvalue->type.Date, jvalue);
    case BACNET_APPLICATION_TAG_TIME:
        return bacnetTimeToC(&cvalue->type.Time, jvalue);
    case BACNET_APPLICATION_TAG_OBJECT_ID:
        return bacnetObjectHandleToC(&cvalue->type.Object_Id, jvalue);
    default:
        std::cout << "ERROR: value tag (" << +cvalue->tag << ") not converted to js '" << bactext_application_tag_name(cvalue->tag) << "'" << std::endl;
        return 1;
    }
}

// converts a value representing a bacnet object type to its enum value
const char * objectTypeToC(Local<Value> type, unsigned * index) {
    if (type->IsString()) {
        if (bactext_object_type_index(extractString(type.As<v8::String>()).c_str(), index)) {
            return 0;
        } else {
            return "Object type string not valid";
        }
    } else if (type->IsUint32()) {
        const char * name = bactext_object_type_name(type->ToUint32()->Value());
        bactext_object_type_index(name, index);
        return 0;
    } else {
        return "Object type must be either a string or unsigned int";
    }
}

// TODO : provide enums
// converts a value representing a bacnet value application tag to its enum value
// returns zero on success or an error string
const char * applicationTagToC(Local<Value> type, unsigned * index) {
    if (type->IsString()) {
        if (bactext_application_tag_index(extractString(type.As<v8::String>()).c_str(), index)) {
            return 0;
        } else {
            return "Application tag string not valid";
        }
    } else if (type->IsUint32()) {
        const char * name = bactext_application_tag_name(type->ToUint32()->Value());
        bactext_application_tag_index(name, index);
        return 0;
    } else {
        return "Application tag must be either a string or unsigned int";
    }
}
