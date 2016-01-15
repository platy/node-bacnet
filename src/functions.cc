// for addon
#include <node.h>
#include <v8.h>
#include <nan.h>
#include <string>
#include <iostream>
#include "functions.h"
#include "basicwhois.h"
#include "listen.h"
#include "init.h"
#include "listenable.h"
#include "client.h"
#include "bactext.h"

NAN_METHOD(objectTypeToString) {
    if (info.Length() >= 1 && info[0]->IsString()) {

    } else if (info.Length() >= 1 && info[0]->IsUint32()) {
        const char * name = bactext_object_type_name(info[0]->ToUint32()->Value());
        info.GetReturnValue().Set(Nan::New(name).ToLocalChecked());
    } else {
        // TODO : error
    }
}
NAN_METHOD(objectTypeToNumber);
NAN_METHOD(propertyKeyToString);
NAN_METHOD(propertyKeyToNumber);

// whois([destination], [min_id , [max_id]])
NAN_METHOD(whois) {
  char* mac = 0;
  long dest_net = -1;
  char* dest_mac = 0;
  int32_t min_id = -1;
  int32_t max_id = -1;
  int idOffset = 0; // moves the arg position of the min & max depending on whether the address field is given

  if (info.Length() >= 1 && info[0]->IsString()) { // address object parameter
    idOffset = 1;
    Nan::Utf8String address(info[0].As<v8::String>());
//          if (address->Has(0, "mac")) {
              mac = *address;
//          }
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
  }
  if (info.Length() - idOffset > 0) {
    min_id = max_id = info[idOffset]->ToInt32()->Value();
  }
  if (info.Length() - idOffset == 2) {
    max_id = info[idOffset + 1]->ToInt32()->Value();
  }
  int ret = whoisBroadcast(mac, dest_net, dest_mac, min_id, max_id);
  v8::Local<v8::Number> retval = Nan::New(ret);
  info.GetReturnValue().Set(retval);
}

// readProperty(deviceId, objectType, objectId)
NAN_METHOD(readProperty) {
  int32_t device_id = info[0]->ToInt32()->Value();
  int32_t object_type = info[1]->ToInt32()->Value();
  int32_t object_id = info[2]->ToInt32()->Value();
  int32_t propertyId = info[3]->ToInt32()->Value();
  std::cout << "reading property " << device_id << ", " << object_type << ", " << object_id << ", " << propertyId << std::endl;

  int ret = Send_Read_Property_Request(device_id, (BACNET_OBJECT_TYPE)object_type, object_id, (BACNET_PROPERTY_ID)propertyId, BACNET_ARRAY_ALL);
  v8::Local<v8::Number> retval = Nan::New(ret);
  info.GetReturnValue().Set(retval);
}

NAN_METHOD(listen) {
  listenLoop();
}

NAN_METHOD(initClient) {
  v8::Local<v8::Object> localEventEmitter = info[0]->ToObject();
  eventEmitterSet(localEventEmitter);

  init_service_handlers();
}

NAN_METHOD(initDevice) {
  init_device_service_handlers();
}
