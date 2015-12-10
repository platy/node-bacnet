
#ifndef LISTENABLE_H
#define LISTENABLE_H

#include <v8.h>

void emitterSetListener(v8::Isolate* isolate, v8::Local<v8::Object> localListener);

#endif
