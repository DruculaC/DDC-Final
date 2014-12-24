/*---------------------------------------------------
	Battery.c (v1.00)
	
	µç³ØµçÁ¿ÅÐ¶Ï
---------------------------------------------------*/	

#include "main.h"
#include "port.h"

#include "Battery.h"
#include "voice.h"
#include "Delay.h"
#include "AD.h"

extern tWord ADC_check_result;		

/*----------------------------------------------------
	CheckADC() - Z2
	Check P20's voltage.
-----------------------------------------------------*/
void CheckADC(void)
	{
	// detect the battery voltage
	ADC_check_result = GetADCResult(6);
	}

                                                         
/*----------------------------------------------------
	verifybattery() - Z2
	voice alarm for battery status without power module
-----------------------------------------------------*/
void verifybattery(tWord Check2)
	{	
	// battery is sufficient, battery > 50.5V, 3.72V/4.93V
	if((Check2 >= 0x304))
		{
		voice_EN = 1;
		SC_Speech(28);  	
		Delay(60);
		voice_EN = 0;
		}
   // 20km, 49.5V < battery < 50.5V, 3.68V/4.93V
	else if((Check2 < 0x304)&&(Check2 >= 0x2fc))		
		{
		voice_EN = 1;
		SC_Speech(10);  
		Delay(100);
		voice_EN = 0;
		}
	// 15km, 48.8V < battery < 49.5V, 3.65V/4.93V 
	else if((Check2 < 0x2fc)&&(Check2 >= 0x2f6))		
		{
		voice_EN = 1;
		SC_Speech(9);  	
		Delay(100);
		voice_EN = 0;
		}
	// 10km, 48V < battery <48.8V, 3.61V/4.9V
	else if((Check2 < 0x2f6)&&(Check2 >= 0x2ed))		
		{
		voice_EN = 1;
		SC_Speech(8);  	
		Delay(100);
		voice_EN = 0;
		}
	// 5km, 46.8V < battery < 48V, 3.55V/4.9V
	else if((Check2 < 0x2ed)&&(Check2 >= 0x2e1))		
		{
		voice_EN =1;
		SC_Speech(7);  	
		Delay(100);
		voice_EN = 0;
		}                     
	// battery is low, battery < 46.8V, 
	else if(Check2 < 0x2e1)
		{
		motorBAT_low_speech();
		}
	}
/*----------------------------------------------------
	verifybattery()  - Z4
	voice alarm for battery status with power module
void verifybattery(tWord Check2)
	{	
	// battery is sufficient, battery > 50.4V, 2.48V/5V
	if((Check2 >= 0x1fb))
		{
		voice_EN = 1;
		SC_Speech(28);  	
		Delay(60);
		voice_EN = 0;
		}
   // 20km, 49V < battery < 50.4V, 2.42V/5V
	else if((Check2 < 0x1fb)&&(Check2 >= 0x1ef))		
		{
		voice_EN = 1;
		SC_Speech(10);  
		Delay(100);
		voice_EN = 0;
		}
	// 15km, 47.8V < battery < 49V, 2.36V/5V 
	else if((Check2 < 0x1ef)&&(Check2 >= 0x1e3))		
		{
		voice_EN = 1;
		SC_Speech(9);  	
		Delay(100);
		voice_EN = 0;
		}
	// 10km, 46.5V < battery <47.8V, 2.3V/5V
	else if((Check2 < 0x1e3)&&(Check2 >= 0x1d7))		
		{
		voice_EN = 1;
		SC_Speech(8);  	
		Delay(100);
		voice_EN = 0;
		}
	// 5km, 45.1V < battery < 46.5V, 2.86V/4.93V
	else if((Check2 < 0x1d7)&&(Check2 >= 0x1c9))		
		{
		voice_EN =1;
		SC_Speech(7);  	
		Delay(100);
		voice_EN = 0;
		}                     
	// battery is low, battery < 45.1V, 
	else if(Check2 < 0x1c9)
		{
		motorBAT_low_speech();
		}
	}
-----------------------------------------------------*/
/*---------------------------------------------------
	end of file
----------------------------------------------------*/