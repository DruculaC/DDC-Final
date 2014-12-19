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
bit battery_check=0;							// 2��1ʱ��ִ��һ�ε���ת����ִ����󣬽�����0
bit position_sensor_EN=0;  		//λ�ô�������������̧�𴫸����ܿ��أ�1��ʱ�򣬼��������������
bit slave_away_speech_EN=0;      //�жϸ����뿪��������ʾ����main�������
bit magnet_CW_EN = 0;					//�����˳ʱ��ת��ʹ�ܣ�ת��һ�κ�λΪ0
bit magnet_ACW_EN=0;						//�������ʱ��ת��ʹ�ܣ�ת��һ�κ�λΪ0
bit comm_whole_control=0;				//ͨ���ܿ��أ�0�ر�ͨ�ţ�1��ͨ��
tWord host_stolen_speech_count=0;   //�ж�����������������ʾ�Ĵ���
bit slave_nearby_speech_EN=0;       //�жϸ���������������ʾ����main�������
bit host_stolen_alarm1_EN = 0;      //�ж�Ϊ����������ĵ�һ������ʹ��
bit host_stolen_alarm2_EN = 0;      //�ж�Ϊ����������ĵڶ�������ʹ��
tByte host_stolen_alarm1_count = 0;		//�ж�Ϊ����������ĵ�һ����������
tByte host_stolen_alarm2_count = 0;		//�ж�Ϊ����������ĵڶ�����������
bit fell_flag=0;						//�ж��������º���1
bit slave_away = 1;					// flag for slave, 1 means away, 0 means nearby
tByte leave_count=0;					// increment every 2s, reset for every success communication
bit raised_flag=0;					//�ж�������̧�����1
tByte sensor_2ndstage_LV_time=0; 	//����������ڶ��׶μ��ʱ������͵�ƽ��ʱ��
tByte sensor_2ndstage_count = 0;		//����������ڶ��׶μ��ʱ������͵�ƽ��ʱ��
tByte host_2ndtouch_speech_count = 0;		//�������ڶ��δ�����������ʾ�Ĵ���
tWord sensor_3rdstage_time = 0;			//��������������׶ε�ʱ�䣬
tByte sensor_3rdstage_effcount = 0;		//��������������׶κ���Ч���������ļ���
tByte sensor_3rdstage_count = 0;			//��������������׶κ󣬵͵�ƽ�ļ���
tWord sensor_3rdstage_interval = 0;		//�������ڵ����׶��У�ÿ����Ч�͵�ƽ����֮���ʱ�����������ڼ�ĵ͵�ƽ����Ϊ��Ч��
bit host_touch_speech_EN = 0;				//��һ�δ���������ʹ��
bit host_2ndtouch_speech_EN = 0;			//�ڶ��δ���������ʹ��
bit raised_fell_flag = 0;					//���ػ���̧�𴥷��󣬴˱�־λ��1
tByte sensor_trigger_count=0;		//��������������
tWord sensor_2ndstage_time=0;		//����������ڶ��׶κ�����ʱ��ļ���
tByte sensor_1ststage_count=0;	//��������һ�׶��жϵ͵�ƽ�ļ���
tByte nearby_away_interval = 0;		//�����뿪�Ϳ���ʱ��������ʾ�Ϳ�������ʱ����
bit magnet_ACW_flag=0;

// ------ Private variable definitions -----------------------------
tByte timer0_8H, timer0_8L, timer1_8H, timer1_8L;		// register value of timer0 and timer1, caculated from InitTimer()

tByte host_touch_speech_count=0;
tByte host_touched_flag=0;			//��������������1�����к���Ĵ������ж�
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
tByte key_rotated_on_flag=0;			//�綯�������رձ�־λ��1��ʾ�綯�������ˣ�0��ʾ�綯���ر���
tByte slave_nearby_speech_count=0;
tByte slave_away_speech_count=0;
tWord ADC_check_result = 0;		//��ΪAD���ֵ
bit slave_nearby_EN = 0;			// ��Ϊslave�����������ʹ��
tByte slave_nearby_operation_count = 0;	// ��Ϊslave����������Ĵ���
tByte wire_broken_count = 0;		// ��Ϊ���ߺ��ʱ����
bit battery_stolen_EN = 0;			// ��Ϊ��ر�����ʹ�ܶ�
tByte battery_stolen_count = 0;	// ��Ϊ��ر����ı�������
bit horizontal_vibration = 0;		// ��ʾ��ֱ���������񶯣���ʱ�����Կ�ף�Ҳ����ִ�й�Կ�׵Ĳ����
tWord horizontal_vibration_count = 0;	//��ֱ�����������󣬶�ʱ����м�����
bit vibration_flag = 0;
tWord vibration_count = 0;
bit wire_broken_flag = 0;			// �������ߵı�־λ
tByte IDkey5 = IDkey0^IDkey1^IDkey2^IDkey3^IDkey4;		// ��ǰ5λ������ɵ����
bit IDkey_flag = 0;			// ��ID������ʱ��֤ͨ������1��
tByte IDkey_count = 0;		// ID����֤ͨ���󣬼�ʱ1���ӣ�ʹԿ����ת����
bit sensor_EN = 0;
tByte enable_sensor_delay_count = 0;		// �������ӳٵ�ʱ��
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
	
	// ���ID��֤ͨ����ͬʱԿ�״򿪺�Ĳ���������������
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
		
	// ��Կ�׵Ĳ���������Ƿ���������ת��
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

	// ����������ת����ʱ�򣬲��ܹ���
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

	// �����񶯴�����
	if((sensor_detect == 0)||(horizontal_sensor == 0))
		{
		if(++sensor_1ststage_count >= 2)			
			{
			sensor_1ststage_count=0;
			
			host_touch_speech();
			}
		}
								
	// �õ�����ģ�����˼������ߣ���������
	if(wire_broken == 0) 
		{
		battery_stolen_speech();
		}

			
	// �������е�ƽ������£��õ�48V��Դ��ģ������͵��ƿ���������������
	if(ADC_check_result < 0x100)
		{
        battery_stolen_speech();
		}
	
	// ����ˮ�����أ����ػ���̧��󣬲���������
	if((raised_sensor_detect == 0)||(fell_sensor_detect == 0))
		{
		stolen_alarm_speech2();
		}
 	}

/*-----------------------------------------------
	��UART�˿ڽ������ݣ�9600�����ʣ��յ����ʾ
	��֤��ͨ����
-----------------------------------------------*/
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

/*-----------------------------------------------------------------
	��ʼ��T0���ڳ�ʼ���˴���֮��
-----------------------------------------------------------------*/
void InitT0(const tByte Tick_ms_T0)
{
	tLong Inc_T0;
	tWord timer0_16;
	
	//����Timer0�ļĴ���ֵ
	Inc_T0 = (tLong)Tick_ms_T0 * (OSC_FREQ/1000) / (tLong)OSC_PER_INST;
	timer0_16 = (tWord) (65536UL - Inc_T0);
	timer0_8H = (tByte) (timer0_16 / 256);
	timer0_8L = (tByte) (timer0_16 % 256);
	TMOD = 0x21;
	TR0=1;
}

/*---------------------------------------------------------------------
	sEos_Go_To_Sleep()
	��MCU��������״̬
----------------------------------------------------------------------*/
void sEOS_Go_To_Sleep(void)
   {
   PCON |= 0x01;    // Enter idle mode (generic 8051 version)
   }
	
/*---------------------------------------------------
	end of file
----------------------------------------------------*/