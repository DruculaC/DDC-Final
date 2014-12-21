/*-----------------------------------------------------
	Delay.h (v1.00)
	
	header file for Delay.c
-----------------------------------------------------*/

#ifndef _DELAY_H
#define _DELAY_H


// *** Timings vary with compiler optimisation settings ***
#define LOOP_TIMEOUT_INIT_001ms 65427
#define LOOP_TIMEOUT_INIT_005ms 64995
#define LOOP_TIMEOUT_INIT_010ms 64455
#define LOOP_TIMEOUT_INIT_050ms 60135
#define LOOP_TIMEOUT_INIT_500ms 11535

//-----public constants-------------------------------

//-----public function prototype----------------------
void Delay(tByte n);
void Custom_Delay(tByte t, m);
void delay_ms(unsigned int count);
void delay_us(unsigned int count);
void Magnet_Delay(tWord x, y);
void Delay_1ms(void);
void Delay_5ms(void);
void Delay_10ms(void);
void Delay_50ms(void);
void Delay_500ms(void);

#endif
											
/*---------------------------------------------------
	end of file
----------------------------------------------------*/