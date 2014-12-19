/*---------------------------------------------------
	main.c (v1.00)
	
	DDC-F program, for electrocar.
----------------------------------------------------*/

#include "main.h"
#include "port.h"

#include "AD.h"
#include "Timer.h"
#include "voice.h"
#include "pwm.h"
#include "Delay.h"
#include "communication.h"
#include "Other.h"

/*------- Public variable declarations --------------------------*/
extern tByte timer0_8H, timer0_8L, timer1_8H, timer1_8L;
extern bit enable_sensor_delayEN;

// ------ Private variable definitions -----------------------------
bit stolen_alarm_flag = 0;					// when host been touch 3 times, this flag 1 before alarm voice present, not to detect sensor for 1st voice alarm.
bit position_sensor_EN=0;  		//位置传感器，即倒地抬起传感器总开关，1的时候，检测这两个传感器
bit host_stolen_alarm1_EN = 0;      //判断为被盗报警后的第一段语音使能
bit host_stolen_alarm2_EN = 0;      //判断为被盗报警后的第二段语音使能
tByte host_stolen_alarm1_count = 0;		//判断为被盗报警后的第一段语音次数
tByte host_stolen_alarm2_count = 0;		//判断为被盗报警后的第二段语音次数
bit fell_flag=0;						//判断主机倒下后，置1
bit raised_flag=0;					//判断主机被抬起后，置1
tByte sensor_2ndstage_count = 0;		//传感器进入第二阶段检测时，计算低电平的时间
tWord sensor_3rdstage_time = 0;			//传感器进入第三阶段的时间，
tByte sensor_3rdstage_effcount = 0;		//传感器进入第三阶段后，有效触碰次数的计数
tByte sensor_3rdstage_count = 0;			//传感器进入第三阶段后，低电平的计数
tWord sensor_3rdstage_interval = 0;		//传感器在第三阶段中，每次有效低电平计数之间的时间间隔。在这期间的低电平不认为有效。
bit raised_fell_flag = 0;					//倒地或者抬起触发后，此标志位置1
tByte sensor_trigger_count=0;		//传感器触发计数
tWord sensor_2ndstage_time=0;		//传感器进入第二阶段后流逝时间的计数
tByte sensor_1ststage_count=0;	//传感器第一阶段判断低电平的计数
tByte nearby_away_interval = 0;		//附机离开和靠近时，语音提示和开关锁的时间间隔

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
tWord ADC_check_result = 0;		//作为AD检测值
tByte wire_broken_count = 0;		// 作为断线后的时间检测
bit battery_stolen_EN = 0;			// 作为电池被盗的使能端
tByte battery_stolen_count = 0;	// 作为电池被盗的报警次数
bit horizontal_vibration = 0;		// 表示垂直传感器在振动，此时就算关钥匙，也不能执行关钥匙的操作�
tWord horizontal_vibration_count = 0;	//垂直传感器触发后，对时间进行计数。
bit vibration_flag = 0;
tWord vibration_count = 0;
bit wire_broken_flag = 0;			// 剪断锁线的标志位
extern tByte IDkey5;		// 由前5位密码组成的异或
bit IDkey_flag = 0;			// 当ID卡靠近时认证通过后置1，
tByte IDkey_count = 0;		// ID卡认证通过后，计时1分钟，使钥匙能转动。
bit sensor_EN = 0;
tByte enable_sensor_delay_count = 0;		// 传感器延迟的时间
bit sensor_3rdalarm_flag = 0;
bit wheeled_flag = 0;
tWord wheeled_count = 0;

tByte lock_time = 0;

/*-----------------------------------------------------------*/
void main(void)
	{
	InitTimer(50, 100);

	Lock_EN = 0;
	SC_RST = 1;
	
	TR0 = 1;
	
	while(1)
		{
		sEOS_Go_To_Sleep();
		}
	}

/*------------------------------------------------------------------
	timerT0()
	定时器0每次溢出后执行的操作
--------------------------------------------------------------------*/

void timer0() interrupt interrupt_timer_0_overflow
	{
	// 重装在定时器的时间设置
	TH0 = timer0_8H;
	TL0 = timer0_8L;

	// 设置一个每3s的操作
	if(++timer0_count >= 10)		
		{
		// 将计数清0
		timer0_count = 0;
        transmiter_EN = ~transmiter_EN;
		}		
		
	if(Lock_EN == 1)
		{
		if(++lock_time >= 50)
			{
			lock_time = 0;
			Lock_EN = 0;
			}
		}
	}

/*------------------------------------------------------------------

 	// 主机被盗报警
	if(stolen_alarm_flag == 1)		
		{
		// 主机被盗报警次数，每报一次加1，如果大于2次，则继续加1，到一定时间后，将flag和count清0.以此来保证附机每次报警后，在短时间内
		// 再次收到报警信号，就不会报警了。
		if(++stolen_alarm_count < 3)
			{			
			// 语音提示，马达振动
			Alarm_stolen_speech();		
			Moto_Vibration();          
			}
		else
			{
			if(stolen_alarm_count >= 1200)
				{
				stolen_alarm_count = 0;
				stolen_alarm_flag = 0;
				}
			}
		}	

	// 抬起信号报警，每次报完后清0，如果再次接到则继续报。一般来说，主机每次抬起只发4遍。
	if(raised_alarm_flag == 1)	
		{
		Alarm_raised_speech();		
		Moto_Vibration();         
		raised_alarm_flag = 0;
		}

	// 倒地信号报警，每次报完后清0，如果再次接到则继续报。一般来说，主机每次倒地只发4遍。
	if(fell_alarm_flag == 1)
		{
		Alarm_fell_speech();		  
		Moto_Vibration();         
		fell_alarm_flag=0;
		}

	// 主机电量充满提示
	if(Host_battery_high_flag == 1)
		{
		Host_battery_high_alarm_speech();		
		Host_battery_high_flag = 0;
		}
		
	if(battery_stolen_EN == 1)
		{
		battery_stolen_speech();
		battery_stolen_EN = 0;
		}
	
	if(wire_broken_EN == 1)
		{
		wire_broken_speech();
		wire_broken_EN = 0;
		}
--------------------------------------------------------------------*/


/*------------------------------------------------------------------
	timerT1()
	定时器1每次溢出后执行的操作
--------------------------------------------------------------------*/
	
void timerT1() interrupt interrupt_timer_1_overflow 			
	{
	// 重装在定时器1的设置
	TH1 = timer1_8H;				
	TL1 = timer1_8L;

	receive_byte();

	receive_word();

	if(receive_data_finished_flag==1)	//说明接收到了数据，开始处理
		{
		receive_data_finished_flag=0;	//清接收标志
		switch(received_data_buffer[2])//解析指令
			{
			case ComMode_1:
				{	
                Lock_EN = 1;
				lock_time = 0;
				}
			break;
			}
		}
	}

/*---------------------------------------------------
	end of file
----------------------------------------------------*/