/*-----------------------------------------------------
	Timer.h (v1.00)
	
	header file for Timer.c
-----------------------------------------------------*/

#ifndef _TIMER_H
#define _TIMER_H

//-----public constants-------------------------------

//-----public function prototype----------------------

void InitTimer(const tByte Tick_ms_T0, Tick_us_T1);
void sEOS_Go_To_Sleep();
void InitT0(const tByte Tick_ms_T0);

#endif
											
/*---------------------------------------------------
	end of file
----------------------------------------------------*/