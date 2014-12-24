/*---------------------------------------------------
	voice.c (v1.00)
	通用语音文件
---------------------------------------------------*/

#include "main.h"
#include "port.h"

#include "voice.h"
#include "Delay.h"
#include "communication.h"

/*------ private variable --------------------------*/
tByte key_rotate_on_speech_number = 1;		// 循环报两段开机语音

extern bit flashing_flag;
   
/*--------------------------------------------------
	SC_Speech()
	按段数发送脉冲，即可报第几段语音。
---------------------------------------------------*/
void SC_Speech(tByte cnt)
	{
	tByte ii;
	SC_RST = 0;
//	delay_ms(100);
	Delay_50ms();
	SC_RST = 1;
//	delay_ms(150);
	Delay_50ms();
	for(ii=0; ii < cnt; ii++)
		{
		SC_DATA = 1; 
		delay_us(20);
		SC_DATA = 0; 
		delay_us(20);
		}
	}

/*-------------------------------------------------
	noVoice()
	在上电的时候调用，屏蔽语音的误报
--------------------------------------------------*/
void noVoice()
	{
	P14=0;
	SC_DATA = 0;
	SC_RST = 0;
	delay_us(350); 
	SC_RST = 1;
	delay_us(350);
	P14 = 1;
	}

/*----------------------------------------------------
	key_rotate_on_speech()
	钥匙旋转至ON后，语音提示
-----------------------------------------------------*/
void key_rotate_on_speech(void)
	{
	switch(key_rotate_on_speech_number)
		{
		case 1:
			{
			voice_EN = 1;
			SC_Speech(14);  
			Delay(45);
			SC_Speech(18);  
			Delay(50);
			SC_Speech(11);  
			Delay(70);
			voice_EN = 0;
			key_rotate_on_speech_number = 2;
			}
		break;
		
		case 2:
			{
			voice_EN = 1;
			SC_Speech(14);  
			Delay(40);
			SC_Speech(12);  
			Delay(60);
			voice_EN = 0;
			key_rotate_on_speech_number = 1;			
			}
		break;
		}
	}
	
/*-----------------------------------------------------
	key_rotate_off_speech()
	钥匙旋转到OFF后，语音提示
void key_rotate_off_speech(void)
	{
	voice_EN = 1;
	SC_Speech(13);
	Delay(40);
	voice_EN = 0;
	}
------------------------------------------------------*/

/*----------------------------------------------------
	motorBAT_low_speech()
	主机电瓶电量低于最小限定值，语音提示电量不足。
-----------------------------------------------------*/
void motorBAT_low_speech(void)
	{
	voice_EN = 1;
	SC_Speech(6);
	Delay(80);
	voice_EN = 0;
	}

/*----------------------------------------------------
	ID_speech()
	第一次碰主机，会有一句提示语音
--------------------------------------------------------*/
void ID_speech(void)
	{
	voice_EN = 1;
	SC_Speech(6);  
	Delay_10ms();
	Delay_10ms();
	Delay_10ms();
	voice_EN = 0;
	}

/*----------------------------------------------------
	Self_learn_speech()
	第一次碰主机，会有一句提示语音
--------------------------------------------------------*/
void Self_learn_speech(void)
	{
	ID_speech();
	Delay(10);
	ID_speech();
	flashing_flag = 0;
	}

/*----------------------------------------------------
	host_touch_speech()
	第一次碰主机，会有一句提示语音
--------------------------------------------------------*/
void host_touch_speech(void)
	{
	voice_EN = 1;
	SC_Speech(2);  
	Delay(90);
	voice_EN = 0;
	Delay(30);
	}

/*-----------------------------------------------------
	host_2ndtouch_speech()
	第二次触碰主机，再报一个提示语音
-------------------------------------------------------*/
void host_2ndtouch_speech(void)
	{
	voice_EN = 1;
	SC_Speech(27);  
	Delay(140);
	voice_EN = 0;
//	Delay(10);
	}
	
/*----------------------------------------------------
	stolen_alarm_speech()
	判断为被盗的时候，发出报警语音第一段
----------------------------------------------------*/
void stolen_alarm_speech1(void)
	{
	voice_EN = 1;
	SC_Speech(3); 
	Delay(180);
	voice_EN = 0; 
	}

/*----------------------------------------------------
	stolen_alarm_speech()
	判断为被盗的时候，发出报警语音第二段
----------------------------------------------------*/
 void stolen_alarm_speech2(void)
	{
	voice_EN = 1;
	SC_Speech(22); 
	Delay(180);
	voice_EN = 0;
	Delay(20);
	}

/*----------------------------------------------------
	open_lock_speech()
	附机在主机附近，通过认证后，语音提示
-----------------------------------------------------*/
void open_lock_speech(void)
	{
	voice_EN=1;
	SC_Speech(23); 
	Delay(60);
	voice_EN=0;
	}

/*----------------------------------------------------
	close_lock_speech()
	判断附机离开，语音提示
-----------------------------------------------------*/
void close_lock_speech(void)
	{
	voice_EN = 1;
	SC_Speech(24);  
	Delay(60);
	SC_Speech(13);
	Delay(70);
	voice_EN = 0;
	}

/*----------------------------------------------------
	battery_stolen_speech()
	主机检测到电平不在时，语音提示电平被盗
-----------------------------------------------------*/
void battery_stolen_speech(void)
	{
	voice_EN = 1;
	SC_Speech(19);  
	Delay(80);
	voice_EN = 0;
	}

/*----------------------------------------------------
	Cutting_Wire_Voice()
	有人剪断锁线的时候，播报的语音
-----------------------------------------------------*/
void Cutting_Wire_Voice(void)
	{
	voice_EN = 1;
	SC_Speech(20);  
	Delay(120);
	voice_EN = 0;
	}

/*----------------------------------------------------
	Product_Test_Voice()
	测试程序中的语音提示
-----------------------------------------------------*/
void Product_Test_Voice(void)
	{
	voice_EN = 1;
	SC_Speech(19);  
	Delay(150);
	voice_EN = 0;
	}

/*---------------------------------------------------------------
	Battery_low_alarm_speech()
	用于附机，当电池电量低于一定值，语音提示
----------------------------------------------------------------*/
void Battery_low_alarm_speech(void)
	{
	voice_EN = 1;
	SC_Speech(9);
	Delay(120);
	voice_EN = 0;
	}

/*---------------------------------------------------------------
	Battery_high_alarm_speech()
	用于附机，电量高于一定值的时候，语音提示
----------------------------------------------------------------*/
void Battery_high_alarm_speech(void)
	{
	voice_EN = 1;
	SC_Speech(14);
	Delay(120);
	voice_EN = 0;
	}

/*-------------------------------------------------------------
	Alarm_stolen_speech()
	用于附机，主机被盗报警
---------------------------------------------------------------*/
void Alarm_stolen_speech(void)
	{
	voice_EN = 1;
	SC_Speech(4);
	Delay(150);
	voice_EN = 0;
	}
	
/*--------------------------------------------------------------
	Alarm_raised_speech()
	用于附机，主机被抬起报警
---------------------------------------------------------------*/
void Alarm_raised_speech(void)
	{
	voice_EN = 1;
	SC_Speech(3);
	Delay(140);
	voice_EN = 0;
	}
	
/*--------------------------------------------------------------
	Alarm_fell_speech()
	用于附机，主机倒地报警
----------------------------------------------------------------*/
void Alarm_fell_speech(void)
	{
	voice_EN = 1;
	SC_Speech(2);
	Delay(120);
	voice_EN = 0;
	}
	
/*---------------------------------------------------------------
	Host_battery_high_alarm_speech()
	用于附机，主机电量充满提示
---------------------------------------------------------------*/
void Host_battery_high_alarm_speech(void)
	{
	voice_EN = 1;
	SC_Speech(7);  
	Delay(80);
	voice_EN = 0;
	}

/*---------------------------------------------------------------
	battery_stolen_speech_F3()
	用于附机，主机电池被盗报警
---------------------------------------------------------------*/
void battery_stolen_speech_F3(void)
	{
	voice_EN = 1;
	SC_Speech(11);  
	Delay(50);
	voice_EN = 0;
	}
	
/*---------------------------------------------------------------
	wire_broken_speech(void)
	用于附机，主机锁线被剪断报警
----------------------------------------------------------------*/
void wire_broken_speech(void)
	{
	voice_EN = 1;
	SC_Speech(15);  
	Delay(50);
	voice_EN = 0;	
	}

/*---------------------------------------------------------------
	lock_rotated_on_speech(void)
	用于附机，主机锁线被剪断报警
----------------------------------------------------------------*/
void lock_rotated_on_speech(void)
	{
	voice_EN = 1;
	SC_Speech(12);  
	Delay(80);
	voice_EN = 0;	
	}
/*---------------------------------------------------------------
	lock_rotated_off_speech(void)
	用于附机，主机锁线被剪断报警
----------------------------------------------------------------*/
void lock_rotated_off_speech(void)
	{
	voice_EN = 1;
	SC_Speech(13);  
	Delay(80);
	voice_EN = 0;	
	}

/*---------------------------------------------------
	end of file
----------------------------------------------------*/