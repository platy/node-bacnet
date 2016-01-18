
#include <sstream>
#include <v8.h>
#include <nan.h>
#include "bacaddr.h"
#include "bacapp.h"
#include "rp.h"

using namespace v8;


Local<Object> bacnetIPToJ(Nan::HandleScope *scope, uint8_t *mac, uint8_t mac_len) {
    Local<Object> address = Nan::New<Object>();
    std::ostringstream stringStream;
    uint16_t port = (mac[4] << 8) + mac[5];
    stringStream << (int)mac[0] << '.' << (int)mac[1] << '.' << (int)mac[2] << '.' << (int)mac[3];
    std::string copyOfStr = stringStream.str();
    Nan::Set(address, Nan::New("ip").ToLocalChecked(), Nan::New(copyOfStr.c_str()).ToLocalChecked());
    Nan::Set(address, Nan::New("port").ToLocalChecked(), Nan::New(port));
    return address;
}

Local<Object> bacnetAddressToJ(Nan::HandleScope *scope, BACNET_ADDRESS *src) {
    Local<Object> address = Nan::New<Object>();
    Nan::Set(address, Nan::New("mac").ToLocalChecked(), bacnetIPToJ(scope, src->mac, src->mac_len));
    assert(!src->len); // TODO support hw addresses other than broadcast
//    Nan::Set(address, Nan::New("hwaddr").ToLocalChecked(), Nan::Undefined());
    Nan::Set(address, Nan::New("network").ToLocalChecked(), Nan::New(src->net));
    return address;
}

// Creates a javascript BACnet object handle
//{
//    type: 'device',
//    instance: 124123
//}
Local<Object> objectHandleToJ(Nan::HandleScope *scope, BACNET_OBJECT_TYPE object_type, uint32_t object_instance) {
    Local<Object> rpa = Nan::New<Object>();
    Nan::Set(rpa, Nan::New("type").ToLocalChecked(), Nan::New(object_type));
    Nan::Set(rpa, Nan::New("instance").ToLocalChecked(), Nan::New(object_instance));
    return rpa;
}

Local<Value> octetStringToBuffer(Nan::HandleScope *scope, BACNET_OCTET_STRING octet_string) {
    return Nan::Encode(octet_string.value, octet_string.length, Nan::Encoding::BUFFER);
}

Local<Value> bitStringToBuffer(Nan::HandleScope *scope, BACNET_BIT_STRING bit_string) {
    int byte_length = (bit_string.bits_used + 8 - 1) / 8;
    return Nan::Encode(bit_string.value, byte_length, Nan::Encoding::BUFFER);
}

// TODO: what does it involve to support this?
Local<String> characterStringToBuffer(Nan::HandleScope *scope, BACNET_CHARACTER_STRING character_string) {
    switch (character_string.encoding) {
    case CHARACTER_MS_DBCS:
        return Nan::New("Unsupported string encoding - MS-DBCS").ToLocalChecked();
    case CHARACTER_JISC_6226:
        return Nan::New("Unsupported string encoding - JISC-6226").ToLocalChecked();
    case CHARACTER_UCS4:
        return Nan::New("Unsupported string encoding - UCS4").ToLocalChecked();
    case CHARACTER_UTF8:
        return Nan::Encode(character_string.value, character_string.length, Nan::Encoding::UTF8).As<String>();
    case CHARACTER_UCS2:
        return Nan::Encode(character_string.value, character_string.length, Nan::Encoding::UCS2).As<String>();
    case CHARACTER_ISO8859:
        return Nan::Encode(character_string.value, character_string.length, Nan::Encoding::ASCII).As<String>();
    }
    return Nan::New("Unsupported string encoding - Unknown").ToLocalChecked();
}

// Converts BACNET_APPLICATION_DATA_VALUE to a js value
Local<Value> bacnetApplicationDataValueToJ(Nan::HandleScope *scope, BACNET_APPLICATION_DATA_VALUE * value) {
    switch (value->tag) {
    case BACNET_APPLICATION_TAG_NULL:
        return Nan::Null();
    case BACNET_APPLICATION_TAG_BOOLEAN:
        return Nan::New(value->type.Boolean);
    case BACNET_APPLICATION_TAG_UNSIGNED_INT:
        return Nan::New(value->type.Unsigned_Int);
    case BACNET_APPLICATION_TAG_SIGNED_INT:
        return Nan::New(value->type.Signed_Int);
    case BACNET_APPLICATION_TAG_REAL:
        return Nan::New(value->type.Real);
    case BACNET_APPLICATION_TAG_DOUBLE:
        return Nan::New(value->type.Double);
    case BACNET_APPLICATION_TAG_OCTET_STRING:
        return octetStringToBuffer(scope, value->type.Octet_String);
    case BACNET_APPLICATION_TAG_CHARACTER_STRING:
        return characterStringToBuffer(scope, value->type.Character_String);
    case BACNET_APPLICATION_TAG_BIT_STRING:
        return bitStringToBuffer(scope, value->type.Bit_String);
    case BACNET_APPLICATION_TAG_ENUMERATED: // TODO : we can get more info here : see bacapp.c
        return Nan::New(value->type.Unsigned_Int);
    case BACNET_APPLICATION_TAG_DATE:
    case BACNET_APPLICATION_TAG_TIME:
        return Nan::Null(); // TODO date and time conversions
    case BACNET_APPLICATION_TAG_OBJECT_ID:
        return objectHandleToJ(scope, (BACNET_OBJECT_TYPE) value->type.Object_Id.type, value->type.Object_Id.instance);
    }
    return Nan::Null();
}

// Reads a bacnet application data value from the raw data and returns as a js value
Local<Value> bacnetApplicationDataToJ(Nan::HandleScope *scope, uint8_t * data, unsigned application_data_len) {
    BACNET_APPLICATION_DATA_VALUE value;
    int value_length = bacapp_decode_application_data(data, application_data_len, &value);
    application_data_len = application_data_len - value_length;
    if (application_data_len == 0)
        return bacnetApplicationDataValueToJ(scope, &value);
    else {
        int array_length = 0;
        Local<Array> array = Nan::New<v8::Array>(0);
        Nan::Set(array, array_length++, bacnetApplicationDataValueToJ(scope, &value));
        while (application_data_len > 0) {
            data = data + value_length;
            value_length = bacapp_decode_application_data(data, application_data_len, &value);
            application_data_len = application_data_len - value_length;
            Nan::Set(array, array_length++, bacnetApplicationDataValueToJ(scope, &value));
        }
        return array;
    }
}

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
Local<Object> readPropertyAckToJ(Nan::HandleScope *scope, BACNET_READ_PROPERTY_DATA * data) {
    Local<Object> rpa = Nan::New<Object>();
    Nan::Set(rpa, Nan::New("object").ToLocalChecked(), objectHandleToJ(scope, (BACNET_OBJECT_TYPE)data->object_type, data->object_instance));
    Nan::Set(rpa, Nan::New("property").ToLocalChecked(), Nan::New(data->object_property));
    if (data->array_index != BACNET_ARRAY_ALL)
        Nan::Set(rpa, Nan::New("index").ToLocalChecked(), Nan::New(data->array_index));
//    Nan::Set(rpa, Nan::New("error").ToLocalChecked(), bacnetAddressToJ(scope, data->src));
    Nan::Set(rpa, Nan::New("value").ToLocalChecked(), bacnetApplicationDataToJ(scope, data->application_data, data->application_data_len));
    return rpa;
}