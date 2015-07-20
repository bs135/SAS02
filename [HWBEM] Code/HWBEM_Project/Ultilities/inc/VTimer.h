#ifndef	__VTIMER_H__
#define __VTIMER_H__

#include "chip.h"

/*============================*/
/* Constant setting           */
/*============================*/

#define NUMBER_OF_VIR_TIMER 		4

void VTimer_InitController();
unsigned char VTimerIsFired(unsigned char timerId);
void VTimerService();
unsigned char VTimerSet(uint8_t timerId,uint32_t period);
unsigned char VTimerGetID();
void VTimerRelease(unsigned char timerId);
uint32_t VTimerGetTime(uint8_t timerId);
void DelayMs(uint16_t period);

#endif

/****************END FILE**************/


