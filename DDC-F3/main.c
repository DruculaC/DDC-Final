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
tByte key_rotated_on_flag=0;			//�綯�������رձ�־λ��1��ʾ�綯�������ˣ�0��ʾ�綯���ر���
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

// ------ Private variable definitions -----------------------------
tWord stolen_alarm_count = 0;
bit battery_HV_flag = 0;
bit raised_alarm_flag = 0;
bit fell_alarm_flag = 0;
bit Host_battery_high_flag = 0;
bit wire_broken_EN = 0;
tWord Mode_detect_count = 0;
bit Mode_select = 0;	// ģʽѡ��0��ʾ�Զ�ģʽ��1��ʾ�ֶ�ģʽ

bit powersave_enable = 0;
tWord receiver_EN_count = 0;

bit itrpt_EN = 1;
tByte itrpt_count = 0;
bit idle_EN = 1;

bit Auto_transmit = 0;			// 0��ʾ���Զ��������ݣ�1��ʾ�Զ��������ݽ�����֤

/*--------------------------------------------------------------*/

void main(void)
	{
	noVoice();
	InitTimer(50, 100);
    TR0 = 1;

	// �����жϳ�ʼ��
	press_open_button = 1;
	press_close_button = 1;
	
	KBLS1 |= 0x03;
	KBLS0 |= 0x03;
	KBIF &= 0xfc;
	KBIE |= 0x03;
	EA = 1;

	Moto_EN = 1;		//��ʼ�����ر����
	transmit_wire = 1;
	voice_EN = 0;		  	//����ʱ�������Źر�
	
	ADC_check_result = GetADCResult(6);		//�ϵ�ʱ����ص������һ��
	
	stolen_alarm_count = 0;			//�屨��������
	stolen_alarm_flag = 0;			//�屨����־

	transmiter_EN = 0;		// turn off the transmitter
	receiver_EN = 0;		// turn on the receiver

	transceiver_power_enable = 1;         // �ϵ�ʱ����ģ���Դ�ر�
	
	ComMode_Data(ComMode_8, 27);

	while(1)
		{				
		if(idle_EN == 1)
			{
			EKB = 1;
			idle_EN = 0;
			PCON |= 0x02;			
			}
		
//		sEOS_Go_To_Sleep();			
		}  
	}


/*------------------------------------------------------------------
	timerT0()
	��ʱ��0ÿ�������ִ�еĲ���
--------------------------------------------------------------------*/

void timer0() interrupt interrupt_timer_0_overflow
	{
	// ��װ�ڶ�ʱ����ʱ������
	TH0 = timer0_8H;
	TL0 = timer0_8L;

	// ����һ��ÿ3s�Ĳ���
	if(++timer0_count >= 60)		
		{
			
		// ÿ��3s��һ�ε�����⣬��������صĵ�����ʾ
		ADC_check_result = GetADCResult(6);
		
		// �жϵ�ص�ѹ�����С��3.4V��ԭ�ȱ����ߵ�ѹʱ���򱨵������㡣�����ѹ����4.2V����ԭ�ȱ�����������ʱ������ʾ��ѹ����
		if((battery_HV_flag == 1)&&(ADC_check_result <= 0x365))
			{
			// ��λ��0����ʾ��������
			battery_HV_flag = 0;
			Battery_low_alarm_speech();		
			}
		else if((battery_HV_flag == 0)&&(ADC_check_result >= 0x370))
			{
			// ��λ��1����ʾ��������
			battery_HV_flag = 1;
			Battery_high_alarm_speech();
			}	

		// ��������0
		timer0_count = 0;
		}
		
	if(press_open_button == 0)
		{
		transceiver_power_enable = 0;
		RXD = 1;
		ComMode_Data(ComMode_7, 27);		
		}
	
	if(press_close_button == 0)
		{
		transceiver_power_enable = 0;
		RXD = 1;
		ComMode_Data(ComMode_8, 27);
		}

	if(toggle_button == 1)
		{
		transceiver_power_enable = 1;
		receiver_EN = 0;
		transmiter_EN = 0;
		RXD = 0;
		TXD = 0;
		idle_EN = 1;
		}
			
 	// ������������
	if(stolen_alarm_flag == 1)		
		{
		// ������������������ÿ��һ�μ�1���������2�Σ��������1����һ��ʱ��󣬽�flag��count��0.�Դ�����֤����ÿ�α������ڶ�ʱ����
		// �ٴ��յ������źţ��Ͳ��ᱨ���ˡ�
		if(++stolen_alarm_count < 3)
			{			
			// ������ʾ�������
			Alarm_stolen_speech();		
			Moto_Vibration();          
			}
		else
			{
			if(stolen_alarm_count >= 120)
				{
				stolen_alarm_count = 0;
				stolen_alarm_flag = 0;
				}
			}
		}	

	// ̧���źű�����ÿ�α������0������ٴνӵ����������һ����˵������ÿ��̧��ֻ��4�顣
	if(raised_alarm_flag == 1)	
		{
		Alarm_raised_speech();		
		Moto_Vibration();         
		raised_alarm_flag = 0;
		}

	// �����źű�����ÿ�α������0������ٴνӵ����������һ����˵������ÿ�ε���ֻ��4�顣
	if(fell_alarm_flag == 1)
		{
		Alarm_fell_speech();		  
		Moto_Vibration();         
		fell_alarm_flag=0;
		}

	// ��������������ʾ
	if(Host_battery_high_flag == 1)
		{
		Host_battery_high_alarm_speech();		
		Host_battery_high_flag = 0;
		}
		
	if(battery_stolen_EN == 1)
		{
		battery_stolen_speech_F3();
		battery_stolen_EN = 0;
		}
	
	if(wire_broken_EN == 1)
		{
		wire_broken_speech();
		wire_broken_EN = 0;
		}
	}

/*------------------------------------------------------------------
	timerT1()
	��ʱ��1ÿ�������ִ�еĲ���
--------------------------------------------------------------------*/
	
void timerT1() interrupt interrupt_timer_1_overflow 			
	{
	// ��װ�ڶ�ʱ��1������
	TH1 = timer1_8H;				
	TL1 = timer1_8L;

	// receive a tyte
	receive_byte();
	
	// receive a word after every byte
	receive_word();

	if(receive_data_finished_flag==1)	//˵�����յ������ݣ���ʼ����
		{
		receive_data_finished_flag=0;	//����ձ�־
		switch(received_data_buffer[2])//����ָ��
			{
			case ComMode_1://���յ������������͹����ı���1���źţ�˵��������3M�ڣ���������
				{	
				Moto_Vibration();
				ComMode_Data(ComMode_1, 42);
				}
			break;
			
			case ComMode_2:
				{
				battery_stolen_EN = 1;
				Moto_Vibration();          
				}
		   break;
			
			case ComMode_3:
				{
				stolen_alarm_flag = 1;
				Moto_Vibration();         

				raised_alarm_count=0;
				raised_alarm_flag=0;
				fell_alarm_count=0;
				fell_alarm_flag=0;
				}
			break;
		
			case ComMode_4:
				{
				raised_alarm_flag=1;

				stolen_alarm_count=0;
				stolen_alarm_flag=0;
				fell_alarm_count=0;
				fell_alarm_flag=0;
				}
			break;

			case ComMode_5:
				{
				fell_alarm_flag=1;	

				stolen_alarm_count=0;
				stolen_alarm_flag=0;
				raised_alarm_count=0;
				raised_alarm_flag=0;
				}
			break;

			case ComMode_6:
				{
				wire_broken_EN = 1;
				Moto_Vibration();
				}
			break;
			
			case ComMode_7:
				{
				lock_rotated_on_speech();
				}
			break;

			case ComMode_8:
				{
				Moto_Vibration();          			
				Mode_select = 0;
				}
			break;
			
			case ComMode_9:
				{
				lock_rotated_off_speech();
				}
			break;

			case ComMode_10:
				{
				Moto_Vibration();          			
				Mode_select = 1;
				}
			break;
			}
		}
	}

/*-----------------------------------------------------------
	KBI_ISR()
	�����жϣ�ʹоƬ��ʡ��ģʽ�л���
-----------------------------------------------------------*/
void KBI_ISR(void) interrupt 7
	{
	EKB = 0;
	KBIF &= 0xfc;
	EKB = 1;
	}
		
/*---------------------------------------------------
	end of file
----------------------------------------------------*/