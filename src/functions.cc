// for addon
#include <node.h>
#include <v8.h>
#include <string>
#include <iostream>
#include "functions.h"
#include "basicwhois.h"


// whois([destination], [min_id , [max_id]])
NAN_METHOD(whois) {
  v8::Isolate* isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);
  char* mac = 0;
  long dest_net = -1;
  char* dest_mac = 0;
  long min_id = 0;
  long max_id = 4194303;

  unsigned int target_info = 0;

  for (int argi = 0; argi < info.Length(); argi++) {
      if (argi == 0 && info[0]->IsString()) { // address object parameter
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
//      } else {
//          if (target_info == 0) {
//              Target_Object_Instance_Min = Target_Object_Instance_Max =
//                  strtol(argv[argi], NULL, 0);
//              target_info++;
//          } else if (target_info == 1) {
//              Target_Object_Instance_Max = strtol(argv[argi], NULL, 0);
//              target_info++;
//          } else {
//              print_usage(filename);
//              return 1;
//          }
      }
  }
  int ret = whoisBroadcast(mac, dest_net, dest_mac, min_id, max_id);
  std::cout << "returning " << ret << "\n";
  info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, "returned"));
}
