/*---------------------------------------------------
	ADC.c (v1.00)
	
	ADC function, for voltage conversion
---------------------------------------------------*/	

#include "main.h"
#include "port.h"

#include "AD.h"
#include "Delay.h"

/*----------------------------------------------------
	GetADCResult(tByte ch)
	assign the channel, to get the voltage
----------------------------------------------------*/

tWord GetADCResult(tByte ch)
	{
	tByte DataL = 0;
	tWord DataH = 0;
	// channel 5 is MIC voltage detection
	if(ch == 5)
		{	
		// Set ADC0 (P0.5 default) is input only mode
		P0DIDS |= 0x20; 
      // choose channel 5
		AADR0 = 0;	
		AADR1 = 0;
		AADR2 = 1;
	                                                                  
	   ADCCON1 |= 0x80;                                          
		
		ADCI = 0;                                           
    	ADCS = 1;  
		
		DataL = ADCCON0;
		DataL = DataL>>6; 

		DataH = ADCH;
		DataH = (DataH<<2) + DataL;

		return DataH;
		}
	// channel 6 is battery voltage detection
	else if(ch == 6) 
		{
		// Set ADC0 (P0.6 default) is input only mode
		P0DIDS |= 0x40; 

		// choose channel 6.
		AADR0 = 1;
		AADR1 = 0;
		AADR2 = 1;
	                                                                  
	   ADCCON1 |= 0x80;                                      
		
		ADCI = 0;                                       
    	ADCS = 1;  
		
		DataL = ADCCON0;
		DataL = DataL>>6; 

		DataH = ADCH;
		DataH = (DataH<<2) + DataL;

		return DataH;
		}
	}

/*---------------------------------------------------
	end of file
----------------------------------------------------*/ 