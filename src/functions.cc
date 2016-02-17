// for addon
#include <node.h>
#include <v8.h>
#include <nan.h>
#include <string>
#include <iostream>
#include <sstream>
#include "functions.h"
#include "listen.h"
#include "init.h"
#include "listenable.h"
#include "client.h"
#include "newclient.h"
#include "bactext.h"
#include "address.h"
#include "conversion.h"
#include "BacnetValue.h"


NAN_METHOD(objectTypeToString) {
    if (info.Length() >= 1 && info[0]->IsString()) {
        unsigned index;
        if (bactext_object_type_index(extractString(info[0].As<v8::String>()).c_str(), &index)) {
            const char * name = bactext_object_type_name(index);
            info.GetReturnValue().Set(Nan::New(name).ToLocalChecked());
        } else {
            Nan::ThrowError("Object type string not valid");
        }
    } else if (info.Length() >= 1 && info[0]->IsUint32()) {
        const char * name = bactext_object_type_name(info[0]->ToUint32()->Value());
        info.GetReturnValue().Set(Nan::New(name).ToLocalChecked());
    } else {
        Nan::ThrowError("Object type must be either a string or unsigned int");
    }
}
NAN_METHOD(objectTypeToNumber) {
    if (info.Length() >= 1) {
        unsigned value;
        const char * error = objectTypeToC(info[0], &value);
        if (error) {
            Nan::ThrowError(error);
        } else {
            info.GetReturnValue().Set(Nan::New(value));
        }
    } else {
        Nan::ThrowError("Object type must be either a string or unsigned int");
    }
}
NAN_METHOD(propertyKeyToString) {
    if (info.Length() >= 1 && info[0]->IsString()) {
        unsigned index;
        if (bactext_property_index(extractString(info[0].As<v8::String>()).c_str(), &index)) {
            const char * name = bactext_property_name(index);
            info.GetReturnValue().Set(Nan::New(name).ToLocalChecked());
        } else {
             Nan::ThrowError("Property key string not valid");
         }
    } else if (info.Length() >= 1 && info[0]->IsUint32()) {
        uint32_t propertyKey = info[0]->ToUint32()->Value();
        if (propertyKey <= MAX_BACNET_PROPERTY_ID) {
            const char * name = bactext_property_name(propertyKey);
            info.GetReturnValue().Set(Nan::New(name).ToLocalChecked());
        } else {
            Nan::ThrowRangeError("Property key too large, maximum is 4194303");
        }
    } else {
        Nan::ThrowError("Property key must be either a string or unsigned int");
    }
}
NAN_METHOD(propertyKeyToNumber) {
    std::ostringstream errorStringStream;
    if (info.Length() >= 1 && info[0]->IsString()) {
        unsigned index;
        const char * inputString = extractString(info[0].As<v8::String>()).c_str();
        if (bactext_property_index(inputString, &index)) {
            info.GetReturnValue().Set(Nan::New(index));
        } else {
            errorStringStream << "Property key string not valid : " << inputString;
            Nan::ThrowError(errorStringStream.str().c_str());
        }
    } else if (info.Length() >= 1 && info[0]->IsUint32()) {
        uint32_t propertyKey = info[0]->ToUint32()->Value();
        if (propertyKey <= MAX_BACNET_PROPERTY_ID) {
            info.GetReturnValue().Set(Nan::New(propertyKey));
        } else {
            Nan::ThrowRangeError("Property key too large, maximum is 4194303");
        }
    } else {
        Nan::ThrowError("Property key must be either a string or unsigned int");
    }
}

// whois([destination, [min_id , [max_id]]])
NAN_METHOD(whois) {
    int32_t min_id = -1;
    int32_t max_id = -1;
    BACNET_ADDRESS dest = bacnetAddressToC(info[0]);

    if (info.Length() > 1) {
        min_id = max_id = info[1]->ToInt32()->Value();
    }
    if (info.Length() > 2) {
        max_id = info[2]->ToInt32()->Value();
    }

    if (min_id > BACNET_MAX_INSTANCE) {
        fprintf(stderr, "device-instance-min=%u - it must be less than %u\n",
            min_id, BACNET_MAX_INSTANCE + 1);
        return;
    }
    if (max_id > BACNET_MAX_INSTANCE) {
        fprintf(stderr, "device-instance-max=%u - it must be less than %u\n",
            max_id, BACNET_MAX_INSTANCE + 1);
        return;
    }
    /* send the request */
    Send_WhoIs_To_Network(&dest, min_id,
        max_id);

    info.GetReturnValue().Set(Nan::New(true));
}

bool addressOrBoundDeviceIdToC(Local<Value> value, unsigned * max_apdu, BACNET_ADDRESS * dest) {
    if (value->IsNumber()) {  // device id
        int32_t device_id = value->ToInt32()->Value();

        /* is the device bound? */
        bool isBound = address_get_by_device(device_id, max_apdu, dest);
        if (!isBound) {
            Nan::ThrowError("device is not bound\n");
            return false;
        }
    } else {   // device address
        *dest = bacnetAddressToC(value);
        *max_apdu = MAX_APDU; // without doing the whois we dont know the Max apdu for the device - so we will just hope it is the same as ours
    }
    return true;
}

// readProperty(deviceId, objectType, objectId, property [, arrayIndex])
NAN_METHOD(readProperty) {
    int32_t object_type = info[1]->ToInt32()->Value();
    int32_t object_instance = info[2]->ToInt32()->Value();
    int32_t object_property = info[3]->ToInt32()->Value();
    uint32_t array_index = BACNET_ARRAY_ALL;
    int invoke_id = 0;

    if (info[4]->IsUint32()) {
        array_index = info[4]->ToUint32()->Value();
    }

    BACNET_ADDRESS dest = { 0 };
    unsigned max_apdu = 0;

    bool addressed = addressOrBoundDeviceIdToC(info[0], &max_apdu, &dest);
    if (addressed) {
        invoke_id = Send_Read_Property_Request_Address(
            &dest,
            max_apdu,
            (BACNET_OBJECT_TYPE)object_type,
            object_instance,
            (BACNET_PROPERTY_ID)object_property,
            array_index);

        info.GetReturnValue().Set(Nan::New(invoke_id));
    }
}



// writeProperty(deviceId, objectType, objectId, property, arrayIndex, value [, priority])
NAN_METHOD(writeProperty) {
    int32_t object_type = info[1]->ToInt32()->Value();
    int32_t object_instance = info[2]->ToInt32()->Value();
    int32_t object_property = info[3]->ToInt32()->Value();
    uint32_t array_index = BACNET_ARRAY_ALL;
    int invoke_id = 0;

    if (info[4]->IsUint32()) {
        array_index = info[4]->ToUint32()->Value();
    }


  // TODO : shuffle all the shit in s_wp around so that we can write by address
  // TODO : create a h_wp_a.c to emit the responses

    BACNET_ADDRESS dest = { 0 };
    unsigned max_apdu = 0;

    bool addressed = addressOrBoundDeviceIdToC(info[0], &max_apdu, &dest);
    if (addressed) {
        Local<Object> valueObject = Nan::To<Object>(info[5]).ToLocalChecked();
        BacnetValue * bacnetValue = BacnetValue::Unwrap<BacnetValue>(valueObject);
        BACNET_APPLICATION_DATA_VALUE object_value = {0};
        if (bacnetValue->bacnetValue(&object_value)) {
            invoke_id = Send_Write_Property_Request_Address(
                &dest,
                max_apdu,
                (BACNET_OBJECT_TYPE)object_type,
                object_instance,
                (BACNET_PROPERTY_ID)object_property,
                &object_value,
                BACNET_NO_PRIORITY,
                array_index);
            info.GetReturnValue().Set(Nan::New(invoke_id));
        } else {
            Nan::ThrowError("Some kind of error converting bacnet value for write property");
        }
    }
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
