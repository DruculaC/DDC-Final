/*---------------------------------------------------
	ElecMotor.c (v1.00)
	
	All programs related to Electric Motor in Main board
---------------------------------------------------*/	

#include "main.h"
#include "port.h"

#include "ElecMotor.h"
#include "Delay.h"
#include "communication.h"

/*------- Public variable declarations -----------------*/
extern tByte myTxRxData[7];

/*-------------------------------------------------------
	ElecMotor_CW()
	Electric Motor rotates clockwise.
--------------------------------------------------------*/
void ElecMotor_CW(void)
	{
	ElecMotor_code();

	MagentControl_1 = 1;
	MagentControl_2 = 0;
	ElecMotor_Delay();
	MagentControl_1 = 1;
	MagentControl_2 = 1;
	motor_lock = 0;
	}

/*-------------------------------------------------------
	ElecMotor_ACW()
	Electric Motor rotates anticlockwise.
--------------------------------------------------------*/
void ElecMotor_ACW(void)
	{
	motor_lock = 1;
	MagentControl_1 = 0;
	MagentControl_2 = 1;
	ElecMotor_Delay();
	MagentControl_1 = 1;
	MagentControl_2 = 1;
	}

/*--------------------------------------------------
	ElecMotor_code()
	send the code to Electric Motor.
---------------------------------------------------*/
void ElecMotor_code(void)	
	{	
	tByte i,n;
	myTxRxData[0]=CmdHead;
	myTxRxData[1]=MyAddress;
	myTxRxData[2]=ComMode_1;
	
	for(i=0;i<3;i++)
		{
		for(n=0;n<8;n++)
			{
			if((myTxRxData[i]&0x80) == 0x80)
				{
				MagentControl_2 = 0;
				Delay_10ms();
				}
			else
				{
				MagentControl_2 = 0;
				Delay_5ms();
				}
			MagentControl_2 = 1;	
			myTxRxData[i] <<= 1;
			Delay_5ms();
			}
		}
	}

/*----------------------------------------------------
	Magnet_Delay()
	Delay program for Electric Motor.
-----------------------------------------------------*/
void ElecMotor_Delay(void)
	{
	Delay_500ms();
	Delay_500ms();
	if(ElecMotor_overcurrent == 0)
		{
		Delay_500ms();
		Delay_500ms();
		Delay_500ms();
		}
	}

/*---------------------------------------------------
	end of file
----------------------------------------------------*/
