// for addon
#include <node.h>
#include <v8.h>
#include <string>
#include <iostream>
#include "functions.h"
#include "basicwhois.h"
#include "listen.h"
#include "init.h"
#include "listenable.h"


// whois([destination], [min_id , [max_id]])
NAN_METHOD(whois) {
  v8::Isolate* isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);
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
  v8::Local<v8::Number> retval = v8::Number::New(isolate, ret);
  info.GetReturnValue().Set(retval);
}

NAN_METHOD(listen) {
  v8::Isolate* isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

//  emitterIsolate = isolate;
//  emitter = info.This()->Get(v8::String::NewFromUtf8(isolate, "ee")).As<v8::Object>();
//
//  v8::Handle<v8::Value> argv[2] = {
//    v8::String::NewFromUtf8(isolate, "iam"), // event name
//    v8::String::NewFromUtf8(isolate, "listen")  // argument
//  };
//  Nan::MakeCallback(emitter, "emit", 2, argv);

  listenLoop(0);
  info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, "returned"));
}

NAN_METHOD(initClient) {
  v8::Isolate* isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  v8::Local<v8::Object> localIamCallback = info[0]->ToObject();
  emitterSetListener(isolate, localIamCallback);

  init_service_handlers();
  info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, "returned"));
}

NAN_METHOD(initDevice) {
  v8::Isolate* isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);
  init_device_service_handlers();
  info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, "returned"));
}
