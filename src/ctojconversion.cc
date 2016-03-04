
#include <iostream>
#include <sstream>
#include <v8.h>
#include <nan.h>
#include <time.h>
#include "bacaddr.h"
#include "bacapp.h"
#include "bactext.h"
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
    Nan::Set(rpa, Nan::New("type").ToLocalChecked(), Nan::New(bactext_object_type_name(object_type)).ToLocalChecked());
    Nan::Set(rpa, Nan::New("instance").ToLocalChecked(), Nan::New(object_instance));
    return rpa;
}

Local<Value> octetStringToBuffer(Nan::HandleScope *scope, BACNET_OCTET_STRING octet_string) {
    return Nan::Encode(octet_string.value, octet_string.length, Nan::Encoding::BUFFER);
}

// bit string is converted to a biffer with an extra field 'bitLength' specifying the number of bits used
Local<Object> bitStringToBuffer(Nan::HandleScope *scope, BACNET_BIT_STRING bit_string) {
    int byte_length = (bit_string.bits_used + 8 - 1) / 8;
    Local<Object> buffer = Nan::To<Object>(Nan::Encode(bit_string.value, byte_length, Nan::Encoding::BUFFER)).ToLocalChecked();
    Nan::Set(buffer, Nan::New("bitLength").ToLocalChecked(), Nan::New(bit_string.bits_used));
    return buffer;
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
        return Nan::Encode(character_string.value, character_string.length, Nan::Encoding::BINARY).As<String>();
    }
    return Nan::New("Unsupported string encoding - Unknown").ToLocalChecked();
}

Local<String> bacnetPropertyEnumToJ(Nan::HandleScope *scope, BACNET_OBJECT_PROPERTY_VALUE * object_value) {
    char str[20];
    int str_len = 20;
    char *char_str;
    int ret_val = -1;
    BACNET_APPLICATION_DATA_VALUE *value = object_value->value;
    BACNET_PROPERTY_ID property = object_value->object_property;
    BACNET_OBJECT_TYPE object_type = object_value->object_type;
    switch (property) {
    case PROP_PROPERTY_LIST:
        char_str = (char *) bactext_property_name_default(
            value->type.Enumerated, NULL);
        if (char_str) {
            ret_val = snprintf(str, str_len, "%s", char_str);
        } else {
            ret_val =
                snprintf(str, str_len, "%lu",
                (unsigned long) value->type.Enumerated);
        }
        break;
    case PROP_OBJECT_TYPE:
        if (value->type.Enumerated < MAX_ASHRAE_OBJECT_TYPE) {
            ret_val =
                snprintf(str, str_len, "%s",
                bactext_object_type_name(value->type.
                    Enumerated));
        } else if (value->type.Enumerated < 128) {
            ret_val =
                snprintf(str, str_len, "reserved %lu",
                (unsigned long) value->type.Enumerated);
        } else {
            ret_val =
                snprintf(str, str_len, "proprietary %lu",
                (unsigned long) value->type.Enumerated);
        }
        break;
    case PROP_EVENT_STATE:
        ret_val =
            snprintf(str, str_len, "%s",
            bactext_event_state_name(value->type.Enumerated));
        break;
    case PROP_UNITS:
        if (value->type.Enumerated < 256) {
            ret_val =
                snprintf(str, str_len, "%s",
                bactext_engineering_unit_name(value->
                    type.Enumerated));
        } else {
            ret_val =
                snprintf(str, str_len, "proprietary %lu",
                (unsigned long) value->type.Enumerated);
        }
        break;
    case PROP_POLARITY:
        ret_val =
            snprintf(str, str_len, "%s",
            bactext_binary_polarity_name(value->
                type.Enumerated));
        break;
    case PROP_PRESENT_VALUE:
    case PROP_RELINQUISH_DEFAULT:
        if (object_type < OBJECT_PROPRIETARY_MIN) {
            ret_val =
                snprintf(str, str_len, "%s",
                bactext_binary_present_value_name(value->type.
                    Enumerated));
        } else {
            ret_val =
                snprintf(str, str_len, "%lu",
                (unsigned long) value->type.Enumerated);
        }
        break;
    case PROP_RELIABILITY:
        ret_val =
            snprintf(str, str_len, "%s",
            bactext_reliability_name(value->type.Enumerated));
        break;
    case PROP_SYSTEM_STATUS:
        ret_val =
            snprintf(str, str_len, "%s",
            bactext_device_status_name(value->
                type.Enumerated));
        break;
    case PROP_SEGMENTATION_SUPPORTED:
        ret_val =
            snprintf(str, str_len, "%s",
            bactext_segmentation_name(value->type.Enumerated));
        break;
    case PROP_NODE_TYPE:
        ret_val =
            snprintf(str, str_len, "%s",
            bactext_node_type_name(value->type.Enumerated));
        break;
    default:
        ret_val =
            snprintf(str, str_len, "%lu",
            (unsigned long) value->type.Enumerated);
        break;
    }
    return Nan::New(str).ToLocalChecked();
}

Local<Object> bacnetDateToJ(Nan::HandleScope *scope, BACNET_DATE * date) {
    Local<Object> jdate = Nan::New<Object>();
    Nan::Set(jdate, Nan::New("year").ToLocalChecked(), Nan::New(date->year));
    Nan::Set(jdate, Nan::New("month").ToLocalChecked(), Nan::New(date->month));
    Nan::Set(jdate, Nan::New("day").ToLocalChecked(), Nan::New(date->day));
    Nan::Set(jdate, Nan::New("weekday").ToLocalChecked(), Nan::New(bactext_day_of_week_name(date->wday)).ToLocalChecked());
    return jdate;
}

Local<Object> bacnetTimeToJ(Nan::HandleScope *scope, BACNET_TIME * time) {
    Local<Object> jtime = Nan::New<Object>();
    Nan::Set(jtime, Nan::New("hour").ToLocalChecked(), Nan::New(time->hour));
    Nan::Set(jtime, Nan::New("min").ToLocalChecked(), Nan::New(time->min));
    Nan::Set(jtime, Nan::New("sec").ToLocalChecked(), Nan::New(time->sec));
    Nan::Set(jtime, Nan::New("hundredths").ToLocalChecked(), Nan::New(time->hundredths));
    return jtime;
}

// Converts BACNET_OBJECT_PROPERTY_VALUE to a js value
Local<Value> bacnetApplicationValueToJ(Nan::HandleScope *scope, BACNET_APPLICATION_DATA_VALUE * value) {
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
    case BACNET_APPLICATION_TAG_ENUMERATED:
        return Nan::New(value->type.Enumerated); // without the context of the property id the enumeration value is just a number
    case BACNET_APPLICATION_TAG_DATE:
        return bacnetDateToJ(scope, &value->type.Date);
    case BACNET_APPLICATION_TAG_TIME:
        return bacnetTimeToJ(scope, &value->type.Time);
    case BACNET_APPLICATION_TAG_OBJECT_ID:
        return objectHandleToJ(scope, (BACNET_OBJECT_TYPE) value->type.Object_Id.type, value->type.Object_Id.instance);
    }
    std::cout << "ERROR: value tag (" << +value->tag << ") not converted to js '" << bactext_application_tag_name(value->tag) << "'" << std::endl;
    return Nan::Null();
}

// Converts BACNET_OBJECT_PROPERTY_VALUE to a js value
Local<Value> bacnetObjectPropertyValueToJ(Nan::HandleScope *scope, BACNET_OBJECT_PROPERTY_VALUE * propertyValue) {
    BACNET_APPLICATION_DATA_VALUE *value = propertyValue->value;
    switch (value->tag) {
    case BACNET_APPLICATION_TAG_ENUMERATED:
        return bacnetPropertyEnumToJ(scope, propertyValue);
    default:
        return bacnetApplicationValueToJ(scope, value);
    }
}

// Reads a bacnet application data value from the raw data and returns as a js value
Local<Value> bacnetApplicationDataToJ(Nan::HandleScope *scope,
            BACNET_READ_PROPERTY_DATA * data) {
    unsigned application_data_len = data->application_data_len;
    uint8_t * application_data = data->application_data;
    BACNET_APPLICATION_DATA_VALUE value;
    BACNET_OBJECT_PROPERTY_VALUE object_value;
    object_value.object_type = data->object_type;
    object_value.object_instance = data->object_instance;
    object_value.object_property = data->object_property;
    object_value.value = &value;
    if (data->array_index == BACNET_ARRAY_ALL) { // an array
        int value_length = bacapp_decode_application_data(application_data, application_data_len, &value);
        application_data_len = application_data_len - value_length;
        int array_length = 0;
        Local<Array> array = Nan::New<v8::Array>(0);
        object_value.array_index = array_length;
        Nan::Set(array, array_length++, bacnetObjectPropertyValueToJ(scope, &object_value));
        while (application_data_len > 0) {
            application_data = application_data + value_length;
            value_length = bacapp_decode_application_data(application_data, application_data_len, &value);
            application_data_len = application_data_len - value_length;
            object_value.array_index = array_length;
            Nan::Set(array, array_length++, bacnetObjectPropertyValueToJ(scope, &object_value));
        }
        return array;
    } else { // single array index
        int value_length = bacapp_decode_application_data(application_data, application_data_len, &value);
        object_value.array_index = data->array_index;
        return bacnetObjectPropertyValueToJ(scope, &object_value);
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
//    Nan::Set(rpa, Nan::New("error").ToLocalChecked(),
    Nan::Set(rpa, Nan::New("value").ToLocalChecked(), bacnetApplicationDataToJ(scope, data));
    return rpa;
}

Local<Object> errorCodesToJ(Nan::HandleScope *scope, BACNET_ERROR_CLASS error_class, BACNET_ERROR_CODE error_code) {
    Local<Object> error = Nan::New<Object>();
    Nan::Set(error, Nan::New("error-class").ToLocalChecked(), Nan::New(bactext_error_class_name(error_class)).ToLocalChecked());
    Nan::Set(error, Nan::New("error-code").ToLocalChecked(), Nan::New(bactext_error_code_name(error_code)).ToLocalChecked());
    return error;
}

Local<String> abortReasonToJ(Nan::HandleScope *scope, uint8_t abortReason) {
    return Nan::New(bactext_abort_reason_name(abortReason)).ToLocalChecked();
}

Local<String> rejectReasonToJ(Nan::HandleScope *scope, uint8_t rejectReason) {
    return Nan::New(bactext_abort_reason_name(rejectReason)).ToLocalChecked();
}
