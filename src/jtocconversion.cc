#include <v8.h>
#include <nan.h>
#include "address.h"
#include "datalink.h"

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
