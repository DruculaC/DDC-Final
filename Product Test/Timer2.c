/*---------------------------------------------------
	Timer.c (v1.00)
	
	Timer functions
---------------------------------------------------*/	

#include "main.h"
#include "port.h"

#include "Timer.h"
#include "communication.h"
#include "voice.h"
#include "Delay.h"
#include "Battery.h"
#include "AD.h"
#include "Other.h"
#include "operation.h"

// ------ Public variable declarations -----------------------------
bit stolen_alarm_flag = 0;					// when host been touch 3 times, this flag 1 before alarm voice present, not to detect sensor for 1st voice alarm.
bit host_stolen_speech_EN = 0;			// when host been touch 3 times, enable 3rd alarm voice.
bit battery_check=0;							// 2置1时，执行一次电量转换，执行完后，将其置0
bit position_sensor_EN=0;  		//位置传感器，即倒地抬起传感器总开关，1的时候，检测这两个传感器
bit slave_away_speech_EN=0;      //判断附机离开后，语音提示，在main里面操作
bit magnet_CW_EN = 0;					//电磁铁顺时针转动使能，转动一次后复位为0
bit magnet_ACW_EN=0;						//电磁铁逆时针转动使能，转动一次后复位为0
bit comm_whole_control=0;				//通信总开关，0关闭通信，1打开通信
tWord host_stolen_speech_count=0;   //判断主机被盗后，语音提示的次数
bit slave_nearby_speech_EN=0;       //判断附近靠近后，语音提示，在main里面操作
bit host_stolen_alarm1_EN = 0;      //判断为被盗报警后的第一段语音使能
bit host_stolen_alarm2_EN = 0;      //判断为被盗报警后的第二段语音使能
tByte host_stolen_alarm1_count = 0;		//判断为被盗报警后的第一段语音次数
tByte host_stolen_alarm2_count = 0;		//判断为被盗报警后的第二段语音次数
bit fell_flag=0;						//判断主机倒下后，置1
bit slave_away = 1;					// flag for slave, 1 means away, 0 means nearby
tByte leave_count=0;					// increment every 2s, reset for every success communication
bit raised_flag=0;					//判断主机被抬起后，置1
tByte sensor_2ndstage_LV_time=0; 	//传感器进入第二阶段检测时，计算低电平的时间
tByte sensor_2ndstage_count = 0;		//传感器进入第二阶段检测时，计算低电平的时间
tByte host_2ndtouch_speech_count = 0;		//主机被第二次触碰后，语音提示的次数
tWord sensor_3rdstage_time = 0;			//传感器进入第三阶段的时间，
tByte sensor_3rdstage_effcount = 0;		//传感器进入第三阶段后，有效触碰次数的计数
tByte sensor_3rdstage_count = 0;			//传感器进入第三阶段后，低电平的计数
tWord sensor_3rdstage_interval = 0;		//传感器在第三阶段中，每次有效低电平计数之间的时间间隔。在这期间的低电平不认为有效。
bit host_touch_speech_EN = 0;				//第一次触碰后语音使能
bit host_2ndtouch_speech_EN = 0;			//第二次触碰后语音使能
bit raised_fell_flag = 0;					//倒地或者抬起触发后，此标志位置1
tByte sensor_trigger_count=0;		//传感器触发计数
tWord sensor_2ndstage_time=0;		//传感器进入第二阶段后流逝时间的计数
tByte sensor_1ststage_count=0;	//传感器第一阶段判断低电平的计数
tByte nearby_away_interval = 0;		//附机离开和靠近时，语音提示和开关锁的时间间隔
bit magnet_ACW_flag=0;

// ------ Private variable definitions -----------------------------
tByte timer0_8H, timer0_8L, timer1_8H, timer1_8L;		// register value of timer0 and timer1, caculated from InitTimer()

tByte host_touch_speech_count=0;
tByte host_touched_flag=0;			//主机被触碰后置1，进行后面的传感器判断
tByte raised_alarm_count = 0;    //主机被抬起后，向附机发出报警信号的次数
tByte fell_alarm_count=0;        //主机倒地后，向附机发出报警信号的次数
tWord timer0_count=0;		// counter for timer0, increment every ticket 			
tByte received_data_buffer[7]={0x00,0x00,0x00,0x00,0x00,0x00,0x00};		//接收数据缓存
bit receive_data_finished_flag = 0;		//接收这一串数据完成后，此标志位置1
tByte data_count = 0;				//接收数据缓存的位数，即表明第几个缓存数据
tByte one_receive_byte = 0;		//接收数据的一个字节，接收完后将其赋值给received_data_buffer相对应的字节
tByte one_receive_byte_count = 0;			//one_receive_byte有8位，此计数表明接收到第几位，每次计数到8的时候表明一个字节接收完成。
bit receive_wire_flag = 1;		//接收通信线的标志位，1表明高电平，0表明低电平，每次timer1溢出时，判断P1.1一次。以此来表明是否为一次正确的下降沿
tByte receive_HV_count = 0;		//定时器T1在没有信号到来的时候，对高电平计数，一旦超过某个值，则将one_receive_byte_count清0
tByte receive_LV_count = 0;		//每次timer1溢出时判断接收线如果为LV，则计数加1，以此来表明低电平的时间
tByte fell_wire_time=0;          //倒地检测线，检测低电平的时间
tByte raise_wire_time=0;			//抬起检测线，检测低电平的时间
tWord raised_fell_number = 0;				//倒地或者抬起出发后，计数，到达一定数值后，将其与标志位一起清零。
bit raised_fell_once_flag = 0;			//raised_fell_flag是否曾经标志过，如果标志过则置1.然后主机被恢复倒地或者恢复抬起时，此标志位复位。
tByte key_rotated_on_flag=0;			//电动车开启关闭标志位，1表示电动车开启了，0表示电动车关闭了
tByte slave_nearby_speech_count=0;
tByte slave_away_speech_count=0;
tWord ADC_check_result = 0;		//作为AD检测值
bit slave_nearby_EN = 0;			// 作为slave靠近后操作的使能
tByte slave_nearby_operation_count = 0;	// 作为slave靠近后操作的次数
tByte wire_broken_count = 0;		// 作为断线后的时间检测
bit battery_stolen_EN = 0;			// 作为电池被盗的使能端
tByte battery_stolen_count = 0;	// 作为电池被盗的报警次数
bit horizontal_vibration = 0;		// 表示垂直传感器在振动，此时就算关钥匙，也不能执行关钥匙的操作�
tWord horizontal_vibration_count = 0;	//垂直传感器触发后，对时间进行计数。
bit vibration_flag = 0;
tWord vibration_count = 0;
bit wire_broken_flag = 0;			// 剪断锁线的标志位
tByte IDkey5 = IDkey0^IDkey1^IDkey2^IDkey3^IDkey4;		// 由前5位密码组成的异或
bit IDkey_flag = 0;			// 当ID卡靠近时认证通过后置1，
tByte IDkey_count = 0;		// ID卡认证通过后，计时1分钟，使钥匙能转动。
bit sensor_EN = 0;
tByte enable_sensor_delay_count = 0;		// 传感器延迟的时间
extern bit enable_sensor_delayEN;
bit sensor_3rdalarm_flag = 0;
bit wheeled_flag = 0;
tWord wheeled_count = 0;

/*------------------------------------------------------------------
	timerT0()
	operation every ticket.
--------------------------------------------------------------------*/

void timer0() interrupt interrupt_timer_0_overflow
	{
	// manually reload timer0 configuration
	TH0 = timer0_8H;
	TL0 = timer0_8L;
	
	// 检测ID认证通过，同时钥匙打开后的操作，播报语音。
	if((key_rotate == 1)&&(key_rotated_on_flag == 0)&&(IDkey_flag == 1))		
		{                                                                        
		Delay(5);
		// anti-trigger, Delay(5) confirm the key rotation.
		if(key_rotate == 1)
			{
            // speech for slave nearby
			slave_nearby_speech();

			key_rotated_on_flag = 1;
			
			IDkey_count = 0;
			IDkey_flag = 0;				
			}
		}
		
	// 关钥匙的操作，检测是否有轮子在转动
	if((key_rotate == 0)&&(key_rotated_on_flag == 1))
		{
		if((vibration_flag == 0)&&(wheeled_flag == 0))
			{
			Delay(5);
			if(key_rotate == 0)
				{
				// reset key rotation flag
				key_rotated_on_flag=0;
				
				slave_away_speech();	
				}				
			}
		}

	// 测试轮子在转动的时候，不能关锁
	if(wheeled_rotate == 1)
		{
		wheeled_flag = 1;
		wheeled_count = 0;
		}
	if(wheeled_flag == 1)
		{
		if(++wheeled_count >= 2000)
			{
			wheeled_flag = 0;
			wheeled_count = 0;
            }
		}

	// 测试振动传感器
	if((sensor_detect == 0)||(horizontal_sensor == 0))
		{
		if(++sensor_1ststage_count >= 2)			
			{
			sensor_1ststage_count=0;
			
			host_touch_speech();
			}
		}
								
	// 拿掉锁，模拟有人剪断锁线，播报语音
	if(wire_broken == 0) 
		{
		battery_stolen_speech();
		}

			
	// 测试在有电平的情况下，拿掉48V电源，模拟有人偷电瓶的情况，播报语音
	if(ADC_check_result < 0x100)
		{
        battery_stolen_speech();
		}
	
	// 测试水银开关，倒地或者抬起后，播报语音。
	if((raised_sensor_detect == 0)||(fell_sensor_detect == 0))
		{
		stolen_alarm_speech2();
		}
 	}

/*-----------------------------------------------
	用UART端口接收数据，9600波特率，收到后表示
	认证已通过。
-----------------------------------------------*/
void uart_isr() interrupt 4 //中断接收数据
	{
	if(RI)
		{
		RI=0;

		// assign one byte to buffer[i] 
		received_data_buffer[data_count] = SBUF;
		
		// judge whether buffer[0] is CmdHead
		if((data_count == 0) && (received_data_buffer[0] == IDkey0))
			{
			data_count = 1;
			}
		else if((data_count == 1) && (received_data_buffer[1] == IDkey1))
			{
			data_count = 2;
			}
		else if((data_count == 2) && (received_data_buffer[2] == IDkey2))
			{
			data_count = 3;
			}
		else if((data_count == 3) && (received_data_buffer[3] == IDkey3))
			{
			data_count = 4;
			}
		else if((data_count == 4) && (received_data_buffer[4] == IDkey4))
			{
			data_count = 5;
			}
		else if((data_count == 5) && (received_data_buffer[5] == IDkey5))
			{
			data_count = 0;	
			IDkey_flag = 1;
			IDkey_count = 0;
			disable_sensor();
			}
		else 
			{
			data_count = 0;
			}
		}
	}

/*-----------------------------------------------------------------
	初始化T0，在初始化了串口之后
-----------------------------------------------------------------*/
void InitT0(const tByte Tick_ms_T0)
{
	tLong Inc_T0;
	tWord timer0_16;
	
	//计算Timer0的寄存器值
	Inc_T0 = (tLong)Tick_ms_T0 * (OSC_FREQ/1000) / (tLong)OSC_PER_INST;
	timer0_16 = (tWord) (65536UL - Inc_T0);
	timer0_8H = (tByte) (timer0_16 / 256);
	timer0_8L = (tByte) (timer0_16 % 256);
	TMOD = 0x21;
	TR0=1;
}

/*---------------------------------------------------------------------
	sEos_Go_To_Sleep()
	将MCU进入休眠状态
----------------------------------------------------------------------*/
void sEOS_Go_To_Sleep(void)
   {
   PCON |= 0x01;    // Enter idle mode (generic 8051 version)
   }
	
/*---------------------------------------------------
	end of file
----------------------------------------------------*/