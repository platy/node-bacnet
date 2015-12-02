
#ifndef BASICWHOIS_H
#define BASICWHOIS_H

#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

int whoisBroadcast(
        char* mac_ascii,
        long destination_network,
        char* destination_mac_ascii,
        int32_t Target_Object_Instance_Min,
        int32_t Target_Object_Instance_Max);

#ifdef __cplusplus
}
#endif

#endif