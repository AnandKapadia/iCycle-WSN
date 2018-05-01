#ifndef _MSG_STRUCTS_H_
#define _MSG_STRUCTS_H_

#include <stdint.h>

#define MASTER_ADDR     (0x0000)
#define BROADCAST_ADDR  (0xFFFF)
/*
typedef enum location_field_t {
    NORTH,
    NORTHEAST,
    EAST,
    SOUTHEAST,
    SOUTH,
    SOUTHWEST,
    WEST,
    NORTHWEST
} location_field_t;
*/
typedef struct bikeMessage_t {
    uint8_t other;
    uint8_t imuX;
    uint8_t imuY;
    uint8_t imuZ;
} bikeMessage_t;

typedef struct cornerMessage_t {
    uint16_t bikeAddress;
    int8_t bikeRssi;
    bikeMessage_t bikePayload;
} cornerMessage_t;

typedef struct masterMessage_t {
    uint8_t masterID;
    uint8_t location;
} masterMessage_t;

#endif

