
#include <sstream>
#include <stdint.h>
#include <v8.h>
#include <uv.h>
#include "bacaddr.h"
#include "emitter.h"
#include "listenable.h"
#include "functions.h"

using namespace v8;

static Persistent<Object> listener;

struct IamEvent {
  uv_work_t  request;
  uint32_t device_id;
  unsigned max_apdu;
  int segmentation;
  uint16_t vendor_id;
  BACNET_ADDRESS * src;
};

// called by libuv worker in separate thread
static void EmitAsync(uv_work_t *req) {
}

Local<Object> bacnetIPToJ(Isolate * isolate, uint8_t *mac, uint8_t mac_len) {
    Local<Object> address = Object::New(isolate);
    std::ostringstream stringStream;
    uint16_t port = (mac[4] << 8) + mac[5];
    stringStream << (int)mac[0] << '.' << (int)mac[1] << '.' << (int)mac[2] << '.' << (int)mac[3];
    std::string copyOfStr = stringStream.str();
    address->Set(String::NewFromUtf8(isolate, "ip"), String::NewFromUtf8(isolate, copyOfStr.c_str()));
    address->Set(String::NewFromUtf8(isolate, "port"), Integer::NewFromUnsigned(isolate, port));
    return address;
}

Local<Object> bacnetAddressToJ(Isolate * isolate, BACNET_ADDRESS *src) {
    Local<Object> address = Object::New(isolate);
    address->Set(String::NewFromUtf8(isolate, "mac"), bacnetIPToJ(isolate, src->mac, src->mac_len));
    assert(!src->len); // TODO support hw addresses other than broadcast
//    address->Set(String::NewFromUtf8(isolate, "hwaddr"), Undefined(isolate));
    address->Set(String::NewFromUtf8(isolate, "network"), Integer::NewFromUnsigned(isolate, src->net));
    return address;
}

Local<Object> iamToJ(Isolate * isolate, IamEvent *work) {
    Local<Object> iamEvent = Object::New(isolate);
    iamEvent->Set(String::NewFromUtf8(isolate, "objectId"), Integer::NewFromUnsigned(isolate, work->device_id));
    iamEvent->Set(String::NewFromUtf8(isolate, "vendorId"), Integer::NewFromUnsigned(isolate, work->vendor_id));
    iamEvent->Set(String::NewFromUtf8(isolate, "segmentation"), Integer::NewFromUnsigned(isolate, work->segmentation));
    iamEvent->Set(String::NewFromUtf8(isolate, "src"), bacnetAddressToJ(isolate, work->src));
    return iamEvent;
}

// called by libuv in event loop when async function completes
static void EmitAsyncComplete(uv_work_t *req,int status) {
    Isolate * isolate = Isolate::GetCurrent();
    HandleScope handleScope(isolate);

    IamEvent *work = static_cast<IamEvent *>(req->data);

    Local<Object> iamEvent = iamToJ(isolate, work);
    Handle<Value> argv[] = { iamEvent };

    // execute the callback
    // https://stackoverflow.com/questions/13826803/calling-javascript-function-from-a-c-callback-in-v8/28554065#28554065
    Local<Object> localCallback = Local<Object>::New(isolate, listener);
    localCallback->CallAsFunction(isolate->GetCurrentContext()->Global(), 1, argv);

    delete work;
}

void emit_iam(uint32_t device_id, unsigned max_apdu, int segmentation, uint16_t vendor_id, BACNET_ADDRESS * src) {
  IamEvent * event = new IamEvent();
  event->request.data = event;
  event->device_id = device_id;
  event->max_apdu = max_apdu;
  event->segmentation = segmentation;
  event->vendor_id = vendor_id;
  event->src = src;

  // kick of the worker thread
  uv_queue_work(uv_default_loop(),&event->request,EmitAsync,EmitAsyncComplete);
}

void emitterSetListener(Isolate* isolate, Local<Object> localListener) {
    listener.Reset(isolate, localListener);
}
