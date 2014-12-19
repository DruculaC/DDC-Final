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

bit test_flag = 0;		// ���ò��Կ�ʼ�ı�־λ

/*------- Private variable declarations --------------------------*/

void main()
	{
	ADC_check_result = 0x3FF;
	
	// initialize the magnet, 
	MagentControl_1 = 0;
	MagentControl_2 = 0;
	wire_broken = 1;
	
	InitUART();
	InitT0(1);
	
	TI = 0;
	RI = 0;
	ES = 1;
	ET0 = 1;
	PS = 1;
	EA = 1;

	P10=1;
	
	lock_code = 1;
	
	// ��ʼ����Ϻ󣬿�ʼ���Գ���
	test_flag = 1;
	TR0 = 1;
		
	while(1)
		{        
		sEOS_Go_To_Sleep();
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
	if(++timer0_count >= 3000)
		{
		// reset timer0 ticket counter every 2s
		timer0_count=0;
		
		if(test_flag == 1)
			{
			// ����
			magnet_CW(5000, 1800, 42);
			Delay(40);

			// �������źų�ȥ
			ComMode_Data(ComMode_3, 28);
			// ���ADC�ĵ�ѹֵ
			ADC_check_result = GetADCResult(6);	
			
			// �������źų�ȥ
			ComMode_Data(ComMode_3, 28);			// ����
			magnet_ACW(5000, 1800);
			Delay(40);
			// �������źų�ȥ
			ComMode_Data(ComMode_3, 28);			
			}
		}

/*	
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

								
	// �õ�����ģ�����˼������ߣ���������
	if(wire_broken == 0) 
		{
		battery_stolen_speech();
		}

	// ����ˮ�����أ����ػ���̧��󣬲���������
	if((raised_sensor_detect == 0)||(fell_sensor_detect == 0))
		{
		stolen_alarm_speech2();
		}

			
	// �������е�ƽ������£��õ�48V��Դ��ģ������͵��ƿ���������������
	if(ADC_check_result < 0x2c1)
		{
		battery_stolen_speech();
		}
*/
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


/*---------------------------------------------------
	end of file
----------------------------------------------------*/