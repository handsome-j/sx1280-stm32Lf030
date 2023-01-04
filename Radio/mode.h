#ifndef __MODE_H
#define __MODE_H
#include "main.h"
#include "sx1280.h"
#include "sx1280_radio.h"

extern PacketParams_t     	packetParams;
extern ModulationParams_t 	modulationParams;

void LORA_SetParams( void );
void GFSK_SetParams( void );
void FLRC_SetParams( void );
void SX1280_Init( void );
#endif