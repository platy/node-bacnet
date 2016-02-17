#include <nan.h>
#include <node.h>
#include <node_object_wrap.h>
#include <bacapp.h>

using v8::Value;
using v8::Local;
using v8::Object;
using v8::Function;
using v8::Uint32;

class BacnetValue: public Nan::ObjectWrap {
 public:
  static void Init(Local<Object> exports);
  bool bacnetValue(BACNET_APPLICATION_DATA_VALUE * value);

 private:
  explicit BacnetValue();
  ~BacnetValue();

  // js fields
  Local<Value> value();
  BACNET_APPLICATION_TAG tag();

  // js factories
  static NAN_METHOD(FromJs);
  static NAN_METHOD(FromBytes);

  // js instance functions
  static NAN_METHOD(Bytes);
  static NAN_METHOD(ToString);
  static NAN_METHOD(ValueOf);

  // js accessors
  // TODO use accessors instead of properties

  static inline Nan::Persistent<v8::Function> & constructor() {
    static Nan::Persistent<v8::Function> my_constructor;
    return my_constructor;
  }
};
