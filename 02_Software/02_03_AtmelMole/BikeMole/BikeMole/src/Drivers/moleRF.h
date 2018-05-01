/*
 * moleRF.h
 *
 * Created: 4/29/2018 10:21:15 PM
 *  Author: user
 */ 


#ifndef MOLERF_H_
#define MOLERF_H_

#include <stdbool.h>
#include <string.h>
#include "gpio.h"
#include "delay.h"
#include "config.h"
#include "nwk.h"
#include "phy.h"
#include "sys.h"
#include "sysTimer.h"

#include "msg_structs.h"

void appInit(void);
void sendPacket(uint16_t destAddr, uint8_t *payload, uint8_t size);
bool receivePacket(NWK_DataInd_t *ind);




#endif /* MOLERF_H_ */