#include "VTimer.h"

/*====================================*/
/*	Global Variable Declaration       */
/*====================================*/

volatile uint32_t timerperiod[NUMBER_OF_VIR_TIMER];

void VTimer_InitController(void)
{
	uint8_t i = 0;
	for(i = 0; i< NUMBER_OF_VIR_TIMER; i++)
	{
		timerperiod[i] = 0;
	}
}
/********************************************************************//**
 * @brief		Check if an Virtual Timer is fired or not
 * @param[in]	TimerId: Id of VTimer that you want to check
 * @return 		
 *				- 1 : Fired
 *				- 0 : Not Fired
 *********************************************************************/
uint8_t VTimerIsFired(uint8_t TimerId)
{	
	if (timerperiod[TimerId] == 1)
		return 1;
	else return 0;
}

/********************************************************************//**
 * @brief		Virtual Timer Service to update VTimer status
 * @param[in]	None
 * @warning		This Service has to be called from SysTick_Handler()
 * @return 		None
 *********************************************************************/
void VTimerService(void)
{
	uint8_t i = 0;
	for(i = 0; i< NUMBER_OF_VIR_TIMER; i++)
	{
		if (timerperiod[i] >1 )
			timerperiod[i] --;
	}	
}
/********************************************************************//**
 * @brief		Setup a VTimer to fire after a period
 * @param[in]	timerId: ID of VTimer
 * @param[in]	period: VTimer will fire after this period(ms)
 * @return 			- 0 : FAIL
 *					- 1 : SUCCESS
 *********************************************************************/
uint8_t VTimerSet(uint8_t timerId,uint32_t period)
{	
	timerperiod[timerId] = period+1;
	return 1;
}
/********************************************************************//**
 * @brief		Get ID of a Free Virtual Timer 
 * @param[in]	None
 * @warning		You have only a NUMBER_OF_VIR_TIMER
 * @return 		ID of a Free Virtual Timer
 *********************************************************************/
uint8_t VTimerGetID()
{
	uint8_t i = 1;
	for(i = 1; i< NUMBER_OF_VIR_TIMER; i++)
	{
		if (timerperiod[i] == 0){
			timerperiod[i] = 1;
			return i;
		}
	}	
	return 0;
}
/********************************************************************//**
 * @brief		Release a VTimer that you don't use anymore
 * @param[in]	timerId: ID of VTimer that you want to release
 * @warning		You have only a NUMBER_OF_VIR_TIMER, so release VTimer immediatelly if you 
 				don't you it anymore
 * @return 		None
 *********************************************************************/
void VTimerRelease(uint8_t timerId)
{
	timerperiod[timerId] = 0;
}

uint32_t VTimerGetTime(uint8_t timerId){
	return timerperiod[timerId];

}

void DelayMs(uint16_t period)
{
	timerperiod[0] = 1+period;
	while (timerperiod[0]!=1);
}

