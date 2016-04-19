#include <iostream>
#include <stdint.h>
#include <v8.h>
#include <nan.h>
#include <uv.h>
#include <cstring>
#include "conversion.h"
#include "bacaddr.h"
#include "rp.h"
#include "emitter.h"
#include "listenable.h"
#include "functions.h"
#include "NonBlockingQueue.h"

using namespace v8;

static Nan::Persistent<Object> eventEmitter;


class NodeCallback {
public:
    virtual ~NodeCallback() {};
    virtual void call()     = 0;
};

static NonBlockingQueue<NodeCallback*> callbackQueue;
static uv_async_t  callback_async;

static void RunCallbacks(uv_async_t *req) {
    NodeCallback *callback;

    while (callbackQueue.pop(&callback)) {
        callback->call();
        delete callback;
    }
}

void eventEmitterSet(Local<Object> localEventEmitter) {
    Nan::HandleScope scope;
    eventEmitter.Reset(localEventEmitter);
    uv_async_init(uv_default_loop(), &callback_async, RunCallbacks);
}

void eventEmitterClose() {
    uv_close((uv_handle_t*) &callback_async, NULL);
}

void schedule(NodeCallback * event) {
    callbackQueue.push(event);
    uv_async_send(&callback_async);
}


class IamEvent: public NodeCallback {
private:
    Local<Object> iamToJ(Nan::HandleScope *scope) {
        Local<Object> iamEvent = Nan::New<Object>();
        Nan::Set(iamEvent, Nan::New("deviceId").ToLocalChecked(), Nan::New(device_id));
        Nan::Set(iamEvent, Nan::New("vendorId").ToLocalChecked(), Nan::New(vendor_id));
        Nan::Set(iamEvent, Nan::New("segmentation").ToLocalChecked(), Nan::New(segmentation));
        Nan::Set(iamEvent, Nan::New("src").ToLocalChecked(), bacnetAddressToJ(scope, src));
        return iamEvent;
    }
public:
  uint32_t device_id;
  unsigned max_apdu;
  int segmentation;
  uint16_t vendor_id;
  BACNET_ADDRESS * src;

  void call() {
    Nan::HandleScope scope;

    Local<Object> iamEvent = iamToJ(&scope);
    Local<Value> argv[] = {
        Nan::New("iam").ToLocalChecked(),
        iamEvent
    };

    Local<Object> localEventEmitter = Nan::New(eventEmitter);
    Nan::MakeCallback(localEventEmitter, "emit", 2, argv);
  }
};

void emit_iam(uint32_t device_id, unsigned max_apdu, int segmentation, uint16_t vendor_id, BACNET_ADDRESS * src) {
    IamEvent * event = new IamEvent();
    event->device_id = device_id;
    event->max_apdu = max_apdu;
    event->segmentation = segmentation;
    event->vendor_id = vendor_id;
    event->src = src;

    schedule(event);
}

class RPAEvent: public NodeCallback {
public:
    uint8_t invoke_id;
    BACNET_READ_PROPERTY_DATA data;

    ~RPAEvent() {
        delete data.application_data;
    }

    void call() {
        Nan::HandleScope scope;
        Local<Object> localEventEmitter = Nan::New(eventEmitter);

        Local<Object> property = readPropertyAckToJ(&scope, &data);

        // emit the read property ack in case you want all of those
        Local<Value> emit_rp_a_args[] = {
                Nan::New("read-property-ack").ToLocalChecked(),
                property,
                Nan::New(invoke_id),
            };
        Nan::MakeCallback(localEventEmitter, "emit", 3, emit_rp_a_args);

        // emit the general ack - it is used for firing callbacks of by invoke_id
        Local<Value> emit_a_args[] = {
                Nan::New("ack").ToLocalChecked(),
                Nan::New(invoke_id),
                property,
            };
        Nan::MakeCallback(localEventEmitter, "emit", 3, emit_a_args);
    }
};

void emit_read_property_ack(uint8_t invoke_id, BACNET_READ_PROPERTY_DATA * data) {
    uint8_t * application_data = new uint8_t[data->application_data_len];
    memcpy(application_data, data->application_data, data->application_data_len);
    RPAEvent * event = new RPAEvent();
    event->invoke_id = invoke_id;
    event->data = *data;
    event->data.application_data = application_data;

    schedule(event);
}

class AckEvent: public NodeCallback {
public:
    uint8_t invoke_id;
    const char * type;

    void call() {
        Nan::HandleScope scope;
        Local<Object> localEventEmitter = Nan::New(eventEmitter);

        // emit the read property ack in case you want all of those
        Local<Value> emit_wp_a_args[] = {
                Nan::New(type).ToLocalChecked(),
                Nan::Undefined(),
                Nan::New(invoke_id),
            };
        Nan::MakeCallback(localEventEmitter, "emit", 3, emit_wp_a_args);

        // emit the general ack - it is used for firing callbacks of by invoke_id
        Local<Value> emit_a_args[] = {
                Nan::New("ack").ToLocalChecked(),
                Nan::New(invoke_id),
            };
        Nan::MakeCallback(localEventEmitter, "emit", 3, emit_a_args);
    }
};

void emit_generic_ack(const char * type, uint8_t invoke_id) {
    AckEvent * event = new AckEvent();
    event->invoke_id = invoke_id;
    event->type = type;

    schedule(event);
}

class AbortEvent: public NodeCallback {
public:
    BACNET_ADDRESS src;
    uint8_t invoke_id;
    uint8_t abort_reason;

    void call() {
        Nan::HandleScope scope;
        Local<Object> localEventEmitter = Nan::New(eventEmitter);

        // emit the abort - it is used for firing callbacks by invoke_id
        Local<Value> emit_a_args[] = {
            Nan::New("abort").ToLocalChecked(),
            Nan::New(invoke_id),
            abortReasonToJ(&scope, abort_reason)
        };
        Nan::MakeCallback(localEventEmitter, "emit", 3, emit_a_args);
    }
};

void emit_abort(
       BACNET_ADDRESS * src,
       uint8_t invoke_id,
       uint8_t abort_reason,
       bool server) {
    AbortEvent * event = new AbortEvent();
    event->invoke_id = invoke_id;
    event->src = *src;
    event->abort_reason = abort_reason;

    schedule(event);
}

class RejectEvent: public NodeCallback {
public:
    BACNET_ADDRESS src;
    uint8_t invoke_id;
    uint8_t reject_reason;

    void call() {
        Nan::HandleScope scope;
        Local<Object> localEventEmitter = Nan::New(eventEmitter);

        // emit the abort - it is used for firing callbacks by invoke_id
        Local<Value> emit_a_args[] = {
            Nan::New("reject").ToLocalChecked(),
            Nan::New(invoke_id),
            rejectReasonToJ(&scope, reject_reason)
        };
        Nan::MakeCallback(localEventEmitter, "emit", 3, emit_a_args);
    }
};

void emit_reject(
       BACNET_ADDRESS * src,
       uint8_t invoke_id,
       uint8_t reject_reason) {
    RejectEvent * event = new RejectEvent();
    event->invoke_id = invoke_id;
    event->src = *src;
    event->reject_reason = reject_reason;

    schedule(event);
}


class ErrorEvent: public NodeCallback {
public:
    BACNET_ADDRESS src;
    uint8_t invoke_id;
    BACNET_ERROR_CLASS error_class;
    BACNET_ERROR_CODE error_code;

    void call() {
        Nan::HandleScope scope;
        Local<Object> localEventEmitter = Nan::New(eventEmitter);

        // emit the abort - it is used for firing callbacks by invoke_id
        Local<Value> emit_a_args[] = {
            Nan::New("error-ack").ToLocalChecked(),
            Nan::New(invoke_id),
            errorCodesToJ(&scope, error_class, error_code)
        };
        Nan::MakeCallback(localEventEmitter, "emit", 3, emit_a_args);

    }
};

void emit_error(
       BACNET_ADDRESS * src,
       uint8_t invoke_id,
       BACNET_ERROR_CLASS error_class,
       BACNET_ERROR_CODE error_code) {
    ErrorEvent * event = new ErrorEvent();
    event->invoke_id = invoke_id;
    event->src = *src;
    event->error_class = error_class;
    event->error_code = error_code;

    schedule(event);
}
