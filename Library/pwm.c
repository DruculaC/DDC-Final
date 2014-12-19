/*---------------------------------------------------
	pwm.c (v1.00)
	
	方波输出功能
---------------------------------------------------*/

#include "main.h"
#include "port.h"

#include "pwm.h"

/*------ private variable --------------------------*/
tWord PWM_shadow;

/*---------------------------------------------------
	Init_PWM()
	初始化PWM端口
----------------------------------------------------*/
void Init_PWM(void)
	{    
	// Enable PWM and select auto reload mode 
	PWMCON0 = 0xC0;                        

	PWM_shadow = PWMP;
	// PWM Period
	PWMPH = HIBYTE(PWM_shadow);         
	PWMPL = LOBYTE(PWM_shadow);                               
	}

/*---------------------------------------------------
	PWM_Channel()
	choose the PWM channel
----------------------------------------------------*/
void PWM_Channel(E_PWMCNL_SEL Channel)	
	{
    switch(Channel)
		{
		// PWM0 Duty (P0.1)
		case E_CHANNEL0:
			{
			PWM_shadow = PWM0_Duty;
			PWM0H = HIBYTE(PWM_shadow);
			PWM0L = LOBYTE(PWM_shadow);
			}
		break;
		// PWM1 Duty (P1.6)
		case E_CHANNEL1:
			{
			PWM_shadow = PWM1_Duty;
			PWM1H = HIBYTE(PWM_shadow);
			PWM1L = LOBYTE(PWM_shadow);
			}
		break;
		// PWM2 Duty (P1.7)
		case E_CHANNEL2:
			{
			PWM_shadow = PWM2_Duty;
			PWM2H = HIBYTE(PWM_shadow);
			PWM2L = LOBYTE(PWM_shadow);			
			}
		break;
		// PWM3 Duty (P0.0)
		case E_CHANNEL3:
			{
			PWM_shadow = PWM3_Duty;
			PWM3H = HIBYTE(PWM_shadow);
			PWM3L = LOBYTE(PWM_shadow);
			}
		break; 
		}	
	}

/*-------------------------------------------------------
	myPwm()
	方波输出打开
--------------------------------------------------------*/
void myPwm()
	{
	// choose channel for pwm
	PWM_Channel(E_CHANNEL0);         
	// pwm out enable
	Init_PWM();                        
	}

/*---------------------------------------------------
	end of file
----------------------------------------------------*/ 