/*---------------------------------------------------
	PWM.h (v1.00)
	
	header file for PWM.c
----------------------------------------------------*/

#ifndef _PWM_H
#define _PWM_H

//-----public constants------------------------------

#define HIBYTE(v1) ((tByte)((v1)>>8))                     
#define LOBYTE(v1) ((tByte)((v1)&0xFF))

// define the frequency
#define PWMP            0x1d		
// define duty ratio
#define PWM0_Duty       0x0d		
#define PWM1_Duty       0x100
#define PWM2_Duty       0x080
#define PWM3_Duty       0x040

typedef enum 
	{
	E_CHANNEL0,
	E_CHANNEL1,      
	E_CHANNEL2,      
	E_CHANNEL3,
	}	E_PWMCNL_SEL;

//-----public function prototype---------------------

void Init_PWM(void);
void PWM_Channel(E_PWMCNL_SEL Channel);	
void myPwm(); 

#endif
											
/*---------------------------------------------------
	end of file
----------------------------------------------------*/