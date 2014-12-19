/*---------------------------------------------------
	Other.c (v1.00)
	
	其他程序
---------------------------------------------------*/

#include "main.h"
#include "port.h"

#include "Other.h"
#include "Delay.h"
#include "communication.h"

/*------- Public variable declarations --------------------------*/
extern bit position_sensor_EN;  		
extern bit fell_flag;						
extern bit raised_flag;					
extern tByte sensor_trigger_count;	
extern tByte sensor_1ststage_count;	
extern tWord sensor_3rdstage_time;			
extern tByte sensor_3rdstage_effcount;		
extern tByte sensor_3rdstage_count;			
extern tWord sensor_3rdstage_interval;		
extern tWord sensor_2ndstage_time;		
extern tByte sensor_2ndstage_count;		
extern bit stolen_alarm_flag;					
extern bit host_stolen_alarm1_EN;      
extern bit host_stolen_alarm2_EN;     
extern tByte host_stolen_alarm1_count;		
extern tByte host_stolen_alarm2_count;	
extern bit raised_fell_flag;					
extern bit wire_broken_flag;			
extern tByte wire_broken_count;		
extern bit battery_stolen_EN;			
extern tByte battery_stolen_count;	
extern bit sensor_EN;
extern tByte enable_sensor_delay_count;		
extern bit enable_sensor_delayEN;

/*-------------------------------------------------------
	magnet_CW()
	电磁铁正转，顺时针，将锁打开，
--------------------------------------------------------*/
void magnet_CW(tWord x, y, tByte z)
	{
	// 发送开锁密码
	send_code_to_lock(ComMode_1, z);
	
	MagentControl_1 = 1;
	MagentControl_2 = 0;
	Magnet_Delay(x, y);
	MagentControl_1 = 1;
	MagentControl_2 = 1;
	motor_lock = 0;
	}

/*-------------------------------------------------------
	magnet_CW_Taili()
	电磁铁正转，顺时针，将锁打开，
--------------------------------------------------------*/
void magnet_CW_Taili(tWord x, y, tByte z)
	{
	// 发送开锁密码
	send_code_to_lock_Taili(ComMode_1, z);
	
	MagentControl_1 = 1;
	MagentControl_2 = 0;
	Magnet_Delay(x, y);
	MagentControl_1 = 0;
	MagentControl_2 = 0;
	motor_lock = 0;
	}

/*-------------------------------------------------------
	magnet_ACW()
	电磁铁反转，逆时针，将锁关闭
--------------------------------------------------------*/
void magnet_ACW(tWord x, y)
	{
	motor_lock = 1;
	MagentControl_1 = 0;
	MagentControl_2 = 1;
	Magnet_Delay(x, y);
	MagentControl_1 = 1;
	MagentControl_2 = 1;
	}

/*-------------------------------------------------------
	magnet_ACW_Taili()
	电磁铁反转，逆时针，将锁关闭
--------------------------------------------------------*/
void magnet_ACW_Taili(tWord x, y)
	{
	motor_lock = 1;
	MagentControl_1 = 0;
	MagentControl_2 = 1;
	Magnet_Delay(x, y);
	MagentControl_1 = 0;
	MagentControl_2 = 0;
	}
	
/*----------------------------------------------------
	disable_sensor()
	打开振动传感器和三轴传感器，并将其复位
----------------------------------------------------*/
void disable_sensor(void)
	{
	// disable position sensor(vertical and horizontal) and vibration sensor
	position_sensor_EN = 0;	
	sensor_EN = 0;
	// reset all the flag in sensor
	fell_flag=0;  
	raised_flag=0;
	sensor_trigger_count = 0;
	sensor_1ststage_count = 0;
	sensor_2ndstage_count = 0;
	sensor_2ndstage_time = 0;
	sensor_3rdstage_time = 0;
	sensor_3rdstage_interval = 0;
	sensor_3rdstage_count = 0;
	sensor_3rdstage_effcount = 0;
	stolen_alarm_flag = 0;
	host_stolen_alarm1_EN = 0;
	host_stolen_alarm1_count = 0;
	host_stolen_alarm2_EN = 0;
	host_stolen_alarm2_count = 0;
	
	raised_fell_flag = 0;
	
	// reset the wire broken count
	wire_broken_count = 0;
	wire_broken_flag = 0;
	
	enable_sensor_delayEN = 0;
	enable_sensor_delay_count = 0;
	// reset battery stolen
	battery_stolen_EN = 0;
	battery_stolen_count = 0;	
	}
	
/*--------------------------------------------------------
	enable_sensor()
	打开传感器，使其检测振动
---------------------------------------------------------*/
void enable_sensor(void)
	{
	// enable position sensor(vertical and horizontal) and vibration sensor
	position_sensor_EN = 1;	
	sensor_EN = 1;
	// reset relatively sensor count
	sensor_trigger_count = 0;
	sensor_1ststage_count = 0;	
	enable_sensor_delayEN = 0;
	enable_sensor_delay_count = 0;	
	}

/*----------------------------------------------------------------
	Moto_Vibration()
	用于附机，马达振动
-----------------------------------------------------------------*/
void Moto_Vibration(void)
	{
	Moto_EN=0;				
	Delay(5);
	Moto_EN=1;
	}
	
/*---------------------------------------------------
	end of file
----------------------------------------------------*/