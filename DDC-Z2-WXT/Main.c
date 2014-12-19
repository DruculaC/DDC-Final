/*---------------------------------------------------
	main.c (v1.00)
	
	DDC-Z program, for electrocar, with ID card.
----------------------------------------------------*/

//�綯��
#include "Main.h"
#include "port.h"

#include "AD.h"
#include "voice.h"
#include "pwm.h"
#include "Timer.h"
#include "Delay.h"
#include "communication.h"
#include "Battery.h"
#include "Other.h"
#include "operation.h"
#include "UART.h"
                                        
/*------- Public variable declarations --------------------------*/
extern tByte timer0_8H, timer0_8L, timer1_8H, timer1_8L;
extern bit enable_sensor_delayEN;

// ------ Private variable definitions -----------------------------
bit stolen_alarm_flag = 0;					// when host been touch 3 times, this flag 1 before alarm voice present, not to detect sensor for 1st voice alarm.
bit position_sensor_EN=0;  		//λ�ô�������������̧�𴫸����ܿ��أ�1��ʱ�򣬼��������������
bit host_stolen_alarm1_EN = 0;      //�ж�Ϊ����������ĵ�һ������ʹ��
bit host_stolen_alarm2_EN = 0;      //�ж�Ϊ����������ĵڶ�������ʹ��
tByte host_stolen_alarm1_count = 0;		//�ж�Ϊ����������ĵ�һ����������
tByte host_stolen_alarm2_count = 0;		//�ж�Ϊ����������ĵڶ�����������
bit fell_flag=0;						//�ж��������º���1
bit raised_flag=0;					//�ж�������̧�����1
tByte sensor_2ndstage_count = 0;		//����������ڶ��׶μ��ʱ������͵�ƽ��ʱ��
tWord sensor_3rdstage_time = 0;			//��������������׶ε�ʱ�䣬
tByte sensor_3rdstage_effcount = 0;		//��������������׶κ���Ч���������ļ���
tByte sensor_3rdstage_count = 0;			//��������������׶κ󣬵͵�ƽ�ļ���
tWord sensor_3rdstage_interval = 0;		//�������ڵ����׶��У�ÿ����Ч�͵�ƽ����֮���ʱ�����������ڼ�ĵ͵�ƽ����Ϊ��Ч��
bit raised_fell_flag = 0;					//���ػ���̧�𴥷��󣬴˱�־λ��1
tByte sensor_trigger_count=0;		//��������������
tWord sensor_2ndstage_time=0;		//����������ڶ��׶κ�����ʱ��ļ���
tByte sensor_1ststage_count=0;	//��������һ�׶��жϵ͵�ƽ�ļ���
tByte nearby_away_interval = 0;		//�����뿪�Ϳ���ʱ��������ʾ�Ϳ�������ʱ����

tByte raised_alarm_count = 0;    //������̧����򸽻����������źŵĴ���
tByte fell_alarm_count=0;        //�������غ��򸽻����������źŵĴ���
tWord timer0_count=0;		// counter for timer0, increment every ticket 			
tByte received_data_buffer[7]={0x00,0x00,0x00,0x00,0x00,0x00,0x00};		//�������ݻ���
bit receive_data_finished_flag = 0;		//������һ��������ɺ󣬴˱�־λ��1
tByte data_count = 0;				//�������ݻ����λ�����������ڼ�����������
tByte one_receive_byte = 0;		//�������ݵ�һ���ֽڣ���������丳ֵ��received_data_buffer���Ӧ���ֽ�
tByte one_receive_byte_count = 0;			//one_receive_byte��8λ���˼����������յ��ڼ�λ��ÿ�μ�����8��ʱ�����һ���ֽڽ�����ɡ�
bit receive_wire_flag = 1;		//����ͨ���ߵı�־λ��1�����ߵ�ƽ��0�����͵�ƽ��ÿ��timer1���ʱ���ж�P1.1һ�Ρ��Դ��������Ƿ�Ϊһ����ȷ���½���
tByte receive_HV_count = 0;		//��ʱ��T1��û���źŵ�����ʱ�򣬶Ըߵ�ƽ������һ������ĳ��ֵ����one_receive_byte_count��0
tByte receive_LV_count = 0;		//ÿ��timer1���ʱ�жϽ��������ΪLV���������1���Դ��������͵�ƽ��ʱ��
tByte fell_wire_time=0;          //���ؼ���ߣ����͵�ƽ��ʱ��
tByte raise_wire_time=0;			//̧�����ߣ����͵�ƽ��ʱ��
tWord raised_fell_number = 0;				//���ػ���̧������󣬼���������һ����ֵ�󣬽������־λһ�����㡣
bit raised_fell_once_flag = 0;			//raised_fell_flag�Ƿ�������־���������־������1.Ȼ���������ָ����ػ��߻ָ�̧��ʱ���˱�־λ��λ��
tByte key_rotated_on_flag = 1;			//�綯��������־λ��1��ʾ�綯�������ˣ�0��ʾ�綯���ر���
tByte key_rotated_off_flag = 0;			//�綯���رձ�־λ��1��ʾ�綯�������ˣ�0��ʾ�綯���ر���
tWord ADC_check_result = 0;		//��ΪAD���ֵ
tByte wire_broken_count = 0;		// ��Ϊ���ߺ��ʱ����
bit battery_stolen_EN = 0;			// ��Ϊ��ر�����ʹ�ܶ�
tByte battery_stolen_count = 0;	// ��Ϊ��ر����ı�������
bit horizontal_vibration = 0;		// ��ʾ��ֱ���������񶯣���ʱ�����Կ�ף�Ҳ����ִ�й�Կ�׵Ĳ����
tWord horizontal_vibration_count = 0;	//��ֱ�����������󣬶�ʱ����м�����
bit vibration_flag = 0;
tWord vibration_count = 0;
bit wire_broken_flag = 0;			// �������ߵı�־λ
extern tByte IDkey5;		// ��ǰ5λ������ɵ����
bit IDkey_flag = 0;			// ��ID������ʱ��֤ͨ������1��
tByte IDkey_count = 0;		// ID����֤ͨ���󣬼�ʱ1���ӣ�ʹԿ����ת����
bit sensor_EN = 0;
tByte enable_sensor_delay_count = 0;		// �������ӳٵ�ʱ��
bit sensor_3rdalarm_flag = 0;
bit wheeled_flag = 0;
tWord wheeled_count = 0;

tByte key_rotate_count = 0;
bit key_rotate_flag = 0;
bit lock_rotate_on_flag = 1;
bit lock_rotate_off_flag = 0;
bit slave_away_flag = 1;
bit magnet_CW_flag = 1;
bit open_lock_EN = 0;
bit close_lock_EN = 1;
bit recovery_from_alarm = 0;
bit Auto_mode = 0;
tByte slave_nearby_count = 0;

void main()
	{

/*	InitUART();
	InitT0(1);
	
	TI = 0;
	RI = 0;
	ES = 1;
	ET0 = 1;
	PS = 1;
	EA = 1;
*/
	
	InitTimer(1,100);

	sensor_EN = 0;
 	position_sensor_EN = 0;

	noVoice();

	voice_EN=0;		//�����Źر�

	raised_sensor_detect=1;
	fell_sensor_detect=1;

	P10=1;
	
    horizontal_sensor = 1;
	
	// turn off transmitter, turn on receiver
	transmiter_EN = 1;
	receiver_EN = 0;
	
	// initialize the magnet, 
	MagentControl_1 = 1;
	MagentControl_2 = 1;

	transmiter_power = 1; 
	
	wire_broken = 1;
	TR0 = 1;
	
	// lock the external motor, ��ֹ����û��ȫ�򿪵�ʱ�򣬳��ּӵ絼�����������ĸ�����ײ�� 
	motor_lock = 1;
	while(1)
		{        		
		// �жϴ������Ƿ�����
		if((sensor_detect == 0)||(horizontal_sensor == 0))
			{
			vibration_flag = 1;
			vibration_count = 0;			
			}
		}
	}

/*------------------------------------------------------------------
	timerT0()
	operation every ticket.
--------------------------------------------------------------------*/

void timer0() interrupt interrupt_timer_0_overflow
	{
	// manually reload timer0 configuration
	TH0 = timer0_8H;
	TL0 = timer0_8L;
	
	// timer0 is 1ms ticket, count the time flow of timer0, then operate every 2s.
	if(++timer0_count >= 2000)
		{
		// reset timer0 ticket counter every 2s
		timer0_count=0;
		
		
		if(Auto_mode == 1)
			{
			if(++slave_nearby_count > 4)
				{
				slave_nearby_count = 6;
				if(key_rotate == 0)
					{
					close_lock_EN = 1;
					}
				}			
			}
        

/*
		if(++key_rotate_count < 16)		
			{
			if((key_rotated_on_flag == 1)||(key_rotated_off_flag == 1))
				{
				ComMode_Data(ComMode_1, 28);				
				}
			}
		else
			key_rotate_count = 31;
			
		if(lock_rotate_off_flag == 0)
			{
			if(++IDkey_count > 4)
				{
				IDkey_count = 7;
				slave_away_flag = 1;
				}			
			}
*/
		// detect the battery voltage
		ADC_check_result = GetADCResult(6);	
		

		// if fell and raised flag is 1, send alarm signal every 2s.
		if((fell_flag==1)&&(fell_alarm_count<5))
			{
			transmiter_power = 0;
			ComMode_Data(ComMode_5, 28);
			transmiter_power = 1;
			fell_alarm_count++;
			}
		if((raised_flag==1)&&(raised_alarm_count<5))		
			{
			transmiter_power = 0;
			ComMode_Data(ComMode_4, 28);
			transmiter_power = 1;
			raised_alarm_count++;
			}
			
		if((battery_stolen_EN == 1)&&(battery_stolen_count < 4))
			{
			if(key_rotate == 0)
				{
				transmiter_power = 0;
				ComMode_Data(ComMode_2, 28);
				transmiter_power = 1;
				battery_stolen_speech();
				battery_stolen_count++;
				}
			}
/*		
		if(IDkey_flag == 1)
			{
			if(++IDkey_count >= 12)
				{
				IDkey_count = 0;
				IDkey_flag = 0;
				if(key_rotated_off_flag == 1)
					{
					enable_sensor();					
					}
				}			
			}
*/
									
		if(enable_sensor_delayEN == 1)
			{
			if(++enable_sensor_delay_count >= 5)
				{
				enable_sensor_delay_count = 0;
				enable_sensor_delayEN = 0;
				enable_sensor();
				}
			}
					
		// whether host has been touched 3 times, if yes, then alarm 2 speech alternantively.
		if((host_stolen_alarm1_EN == 1)&&(host_stolen_alarm1_count < 4))
			{
			stolen_alarm_flag = 1;
			if(key_rotate == 0)
				{
				transmiter_power = 0;
				if(wire_broken_flag == 0)
					{
					ComMode_Data(ComMode_3, 28);                                                                  
					}
				else
					{                                                                     
					ComMode_Data(ComMode_6, 28);
					}
				transmiter_power = 1;
					
				stolen_alarm_speech1();
				}
			if(++host_stolen_alarm1_count >= 4)
				{                                                                                       
				host_stolen_alarm1_count = 0;
				host_stolen_alarm1_EN = 0;
				stolen_alarm_flag = 0;
				sensor_3rdalarm_flag = 0;
				}
			}
		if((host_stolen_alarm2_EN == 1)&&(host_stolen_alarm2_count < 4))
			{
			stolen_alarm_flag = 1;
			if(key_rotate == 0)
				{
				transmiter_power = 0;
				if(wire_broken_flag == 0)
					{
					ComMode_Data(ComMode_3, 28);
					}
				else
					{
					ComMode_Data(ComMode_6, 28);
					}
				transmiter_power = 1;
				stolen_alarm_speech2();
				}
			if(++host_stolen_alarm2_count >= 4)
				{
				host_stolen_alarm2_count = 0;
				host_stolen_alarm2_EN = 0;
				stolen_alarm_flag = 0;
				sensor_3rdalarm_flag = 0;
				}
			}			
		}
	
	if((key_rotate == 1)&&(key_rotated_on_flag == 0))
		{		
		Delay(5);
		if(key_rotate == 1)
			{
			// ���ÿ�Կ�׺͹�Կ�׵ı�־λ
			key_rotated_on_flag = 1;
			key_rotated_off_flag = 0;
			lock_rotate_on_flag = 0;
			
			key_rotate_count = 0;			
			disable_sensor();	
			}
		}

	if((Auto_mode == 1)&&(key_rotated_on_flag == 1))
		{
		open_lock_EN = 1;
//		Auto_mode = 0;
		}

	if(open_lock_EN == 1)
		{
		if((magnet_CW_flag == 0)&&(stolen_alarm_flag == 0))
			{
			magnet_CW(1000, 5200, 28);
			magnet_CW_flag = 1;			
			slave_nearby_operation();
			}
		if(stolen_alarm_flag == 1)
			{
			recovery_from_alarm = 1;
			}
		open_lock_EN = 0;
		slave_nearby_count = 0;
		disable_sensor();
		}
/*		
	// detect whether key is rotated on,  
	if((key_rotated_on_flag == 1)&&(IDkey_flag == 1)&&(lock_rotate_on_flag == 0))		
		{                                                                        
		magnet_CW(2000, 4000, 28);
		
		slave_nearby_operation();                     
		IDkey_count = 0;
		IDkey_flag = 0;
		lock_rotate_on_flag = 1;
		key_rotate_count = 31;
		}
*/		
	if((key_rotate == 0)&&(key_rotated_off_flag == 0)&&(vibration_flag == 0)&&(wheeled_flag == 0))
		{
		Delay(5);
		if(key_rotate == 0)
			{
			// ���ÿ�Կ�׺͹�Կ�׵ı�־λ
			key_rotated_on_flag = 0;
			key_rotated_off_flag = 1;
			lock_rotate_off_flag = 0;
			// enable_sensor();	
//			enable_sensor_delayEN = 1;
//			enable_sensor_delay_count = 0;

			key_rotate_count = 10;
			}		
		}
		
	if(close_lock_EN == 1)
		{
		if((magnet_CW_flag == 1)&&(vibration_flag == 0)&&(wheeled_flag == 0))
			{
			verifybattery(ADC_check_result);
			magnet_ACW(1000, 5000);			
			magnet_CW_flag = 0;
			slave_away_operation();
			}
		if(recovery_from_alarm == 1)
			{
			recovery_from_alarm = 0;
			// enable_sensor();	
			enable_sensor_delayEN = 1;
			enable_sensor_delay_count = 0;			
			}
		close_lock_EN = 0;
		Auto_mode = 0;
		}
/*
	if((key_rotated_off_flag == 1)&&(slave_away_flag == 1)&&(lock_rotate_off_flag == 0))
		{
		if((vibration_flag == 0)&&(wheeled_flag == 0))
			{
			// handle with battery status
			verifybattery(ADC_check_result);
			magnet_ACW(6000, 10000);			
			
			// reset key rotation flag
			slave_away_operation();
			
			key_rotate_count = 31;
			slave_away_flag = 0;
			lock_rotate_off_flag = 1;
			IDkey_flag = 0;
			}
		}
*/	
	if((sensor_detect == 0)||(horizontal_sensor == 0))
		{
		vibration_flag = 1;
		vibration_count = 0;
		}		
	if(vibration_flag == 1)
		{
		if(++vibration_count > 2000)
			{
			vibration_flag = 0;
			vibration_count = 0;
			}
		}		
	
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


	// judge host is fell or raised every 1ms?
	if((raised_sensor_detect == 1)&&(fell_sensor_detect == 1))
		{
		// judge vibration sensor is enable?
		if(sensor_EN == 1)	
			{			
			// sensor trigger status, 0, 1 or 2?
			switch(sensor_trigger_count)
				{
				// case 0, it means host is in waiting status. waiting for first touch.
				case 0:
					{					
					// judge host been touched and also not in vibration alarm
					if(((sensor_detect == 0)||(horizontal_sensor == 0))&&(stolen_alarm_flag == 0))		
						{
						// judge LV is more than 2ms, if yes, it means a effective touch
						if(++sensor_1ststage_count >= 2)			
							{
							sensor_1ststage_count=0;
							
							// sensor trigge status progress to case 1.
							sensor_trigger_count = 1;
							// alarm speech for first touoch
							host_touch_speech();
							}
						}
					else
						{
						// if no LV, reset the count.
						sensor_1ststage_count = 0;
						}
					}
				break;
				
				// waiting for next touch, 
				case 1:
					{
//					if((sensor_detect == 0)||(horizontal_sensor == 0))
					if((sensor_detect == 0)||(horizontal_sensor == 0))
						{
						// LV for 2s, means a effective touch             
						if(++sensor_2ndstage_count >= 2)
							{
							sensor_2ndstage_count = 0;
							sensor_trigger_count = 2;
							// alarm speech for 2nd touch
							host_2ndtouch_speech();
							}
						}
					else
						{
						sensor_2ndstage_count = 0;
						}
					
					// if there is no touch in 4s, reset sensor trigger status, etc.
					if(++sensor_2ndstage_time >= 4000)
						{
						sensor_trigger_count = 0;
						sensor_2ndstage_count = 0;
						sensor_1ststage_count = 0;
						sensor_2ndstage_time = 0;
						}
					}
				break;
				
				// waiting for 3rd touch
				case 2:
					{
					if((sensor_detect == 0)||(horizontal_sensor == 0))
						{
						// 2s LV is a effective touch
						if(++sensor_3rdstage_count >= 1)
							{
							sensor_3rdstage_count = 0;
							// stolen alarm speech enable
							host_stolen_alarm1_EN = 1;
							host_stolen_alarm2_EN = 1;						
							sensor_3rdalarm_flag = 1;							
							}
						}
					else
						{
						sensor_3rdstage_count = 0;
						}
					
					// if there is no touch in 4s, reset all.
					if(++sensor_3rdstage_time >= 4000)
						{
						sensor_trigger_count = 0;
						sensor_1ststage_count = 0;
						sensor_2ndstage_count = 0;
						sensor_2ndstage_time = 0;
						sensor_3rdstage_time = 0;
						sensor_3rdstage_interval = 800;
						sensor_3rdstage_count = 0;
						sensor_3rdstage_effcount = 0;					
						}
					}
				break;
				}
			
			// judge the wire broken, if yes, it means someone has cut the wire of magnet lock
			if((wire_broken == 0) && (wire_broken_count < 51))  
				{
				if(++wire_broken_count > 50)
					{
					host_stolen_alarm1_EN = 1;
					host_stolen_alarm2_EN = 1;	
					wire_broken_count = 51;
					wire_broken_flag = 1;
					}
				}
			else if((wire_broken == 1)&&(sensor_3rdalarm_flag == 0))
				{
				wire_broken_count = 0;
				host_stolen_alarm1_EN = 0;
				host_stolen_alarm2_EN = 0;
				}
			
			// judge whether battery has been stolen, 0.4V/4V
			if(ADC_check_result < 0x100)
				{
				battery_stolen_EN = 1;
				}
			else if(ADC_check_result > 0x100)
				{
				battery_stolen_EN = 0;
				battery_stolen_count = 0;
				}			
			}
		}
	
	// judge whether position sensor is enable
	if(position_sensor_EN==1)		
		{
		// judge whether there is a LV
		if(raised_sensor_detect == 0)	
			{
			// LV > 0.5s means a effective input
			if(++raise_wire_time >= 3)
				{
				// flag raised, and reset fell
				raised_flag=1;
				fell_flag=0;
				raise_wire_time = 11;
				// judge whether there once been a raised or fell.
				if(raised_fell_once_flag == 0)
					{
					// if no, flag raised and fell flag
					raised_fell_flag = 1;					
					}
				}		
			}
		else
			{
			raised_flag=0;
			raised_alarm_count=0;
			raise_wire_time=0;
			raised_fell_flag = 0;
			}
      // judge whether there is a LV
		if(fell_sensor_detect==0)
			{
			// LV > 0.5s means a effective input
			if(++fell_wire_time >= 3)	
				{
				// flag fell, and reset raised
				fell_flag=1;			
				raised_flag=0;
				fell_wire_time = 11;
				// judge whether there once been a raised or fell
				if(raised_fell_once_flag == 0)
					{
					raised_fell_flag = 1;					
					}
				}		
			}
		else
			{
			fell_flag=0;
			fell_alarm_count=0;
			fell_wire_time=0;
			raised_fell_flag = 0;
			}
		}
	
	// judge whether raised or fell, if yes, reset all after 10s.
	if(raised_fell_flag == 1)
		{
		if(++raised_fell_number >= 4000)
			{
			raised_fell_flag = 0;
			raised_fell_number = 0;
			raised_fell_once_flag = 1;
			
			sensor_trigger_count = 0;
			sensor_1ststage_count = 0;
			sensor_2ndstage_count = 0;
			sensor_2ndstage_time = 0;
			sensor_3rdstage_time = 0;
			sensor_3rdstage_interval = 800;
			sensor_3rdstage_count = 0;
			sensor_3rdstage_effcount = 0;					
			}
		}
		
	// detect the horizontal sensor
	if(((horizontal_sensor == 0)||(sensor_detect == 0))&&(horizontal_vibration_count > 5000))
		{
		Delay(3);
		if((horizontal_sensor == 0)||(sensor_detect == 0))
			{
			horizontal_vibration = 1;
			horizontal_vibration_count = 0;
			}
		}
	if(++horizontal_vibration_count >= 5000)
		{
		horizontal_vibration_count = 5001;
		horizontal_vibration = 0;
		}
 	}

/*-----------------------------------------------
	��UART�˿ڽ������ݣ�9600�����ʣ��յ����ʾ
	��֤��ͨ����
void uart_isr() interrupt 4 //�жϽ�������
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
-----------------------------------------------*/

/*--------------------------------------------------------------------
	timerT1()
	��ʱ��1ÿ�������ִ�еĲ�����������ģʽ����
	���ݡ�
--------------------------------------------------------------------*/

void timerT1() interrupt interrupt_timer_1_overflow
	{
   // reload timer1
	TH1 = timer1_8H;				
	TL1 = timer1_8L;
	
	// receive a tyte
	receive_byte();
	
	// receive a word after every byte
	receive_word();
	
	// judge whether a byte has received successfully
	if(receive_data_finished_flag == 1)
		{
		receive_data_finished_flag = 0;
		// judge which mode is received.
		switch(received_data_buffer[2]) 		
			{
			case ComMode_1:  
				{
				IDkey_flag = 1;
				IDkey_count = 0;
				disable_sensor();
				}
			break;
			
			case ComMode_7:
				{
				open_lock_EN = 1;
				disable_sensor();
				}
			break;

			case ComMode_8:
				{
				close_lock_EN = 1;
				}
			break;
			
			case ComMode_9:
				{
				Auto_mode = 1;
				slave_nearby_count = 0;
				disable_sensor();
				}
			break;
			}
		}
	}


/*---------------------------------------------------
	end of file
----------------------------------------------------*/