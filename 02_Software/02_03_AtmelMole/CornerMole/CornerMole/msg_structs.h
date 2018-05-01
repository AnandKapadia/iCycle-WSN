

#define MASTER_ADDR     (0x0000)
#define BROADCAST_ADDR  (0xFFFF)

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
