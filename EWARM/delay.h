#ifndef _DELAY_H_
#define _DELAY_H_

#include<stdint.h>

extern volatile  uint32_t TickCounter;
extern volatile  uint32_t ticktimer;

void Delay_Us (uint32_t delay);
void Delay_Ms(uint32_t delay );
void HAL_Delay_nMs( uint32_t Delay );

#define HAL_GetTick()  TickCounter

#endif

