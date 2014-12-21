/*---------------------------------------------------
	main.c (v1.00)
	
	DDC-Z program, for electrocar.
----------------------------------------------------*/

//µç¶¯³µ
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
#include "ISP_DataFlash.h"
#include "schedular.h"
#include "ElecMotor.h"

                                        
/*------- Public variable declarations --------------------------*/
extern tByte timer0_8H, timer0_8L, timer1_8H, timer1_8L;
extern bit enable_sensor_delayEN;

// ------ Private variable definitions -----------------------------
bit stolen_alarm_flag = 0;					// when host been touch 3 times, this flag 1 before alarm voice present, not to detect sensor for 1st voice alarm.
bit position_sensor_EN=0;  		//Î»ÖÃ´«¸ĞÆ÷£¬¼´µ¹µØÌ§Æğ´«¸ĞÆ÷×Ü¿ª¹Ø£¬1µÄÊ±ºò£¬¼ì²âÕâÁ½¸ö´«¸ĞÆ÷
bit host_stolen_alarm1_EN = 0;      //ÅĞ¶ÏÎª±»µÁ±¨¾¯ºóµÄµÚÒ»¶ÎÓïÒôÊ¹ÄÜ
bit host_stolen_alarm2_EN = 0;      //ÅĞ¶ÏÎª±»µÁ±¨¾¯ºóµÄµÚ¶ş¶ÎÓïÒôÊ¹ÄÜ
tByte host_stolen_alarm1_count = 0;		//ÅĞ¶ÏÎª±»µÁ±¨¾¯ºóµÄµÚÒ»¶ÎÓïÒô´ÎÊı
tByte host_stolen_alarm2_count = 0;		//ÅĞ¶ÏÎª±»µÁ±¨¾¯ºóµÄµÚ¶ş¶ÎÓïÒô´ÎÊı
bit fell_flag=0;						//ÅĞ¶ÏÖ÷»úµ¹ÏÂºó£¬ÖÃ1
bit raised_flag=0;					//ÅĞ¶ÏÖ÷»ú±»Ì§Æğºó£¬ÖÃ1
tByte sensor_2ndstage_count = 0;		//´«¸ĞÆ÷½øÈëµÚ¶ş½×¶Î¼ì²âÊ±£¬¼ÆËãµÍµçÆ½µÄÊ±¼ä
tWord sensor_3rdstage_time = 0;			//´«¸ĞÆ÷½øÈëµÚÈı½×¶ÎµÄÊ±¼ä£¬
tByte sensor_3rdstage_effcount = 0;		//´«¸ĞÆ÷½øÈëµÚÈı½×¶Îºó£¬ÓĞĞ§´¥Åö´ÎÊıµÄ¼ÆÊı
tByte sensor_3rdstage_count = 0;			//´«¸ĞÆ÷½øÈëµÚÈı½×¶Îºó£¬µÍµçÆ½µÄ¼ÆÊı
tWord sensor_3rdstage_interval = 0;		//´«¸ĞÆ÷ÔÚµÚÈı½×¶ÎÖĞ£¬Ã¿´ÎÓĞĞ§µÍµçÆ½¼ÆÊıÖ®¼äµÄÊ±¼ä¼ä¸ô¡£ÔÚÕâÆÚ¼äµÄµÍµçÆ½²»ÈÏÎªÓĞĞ§¡£
bit raised_fell_flag = 0;					//µ¹µØ»òÕßÌ§Æğ´¥·¢ºó£¬´Ë±êÖ¾Î»ÖÃ1
tByte sensor_trigger_count=0;		//´«¸ĞÆ÷´¥·¢¼ÆÊı
tWord sensor_2ndstage_time=0;		//´«¸ĞÆ÷½øÈëµÚ¶ş½×¶ÎºóÁ÷ÊÅÊ±¼äµÄ¼ÆÊı
tByte sensor_1ststage_count=0;	//´«¸ĞÆ÷µÚÒ»½×¶ÎÅĞ¶ÏµÍµçÆ½µÄ¼ÆÊı
tByte nearby_away_interval = 0;		//¸½»úÀë¿ªºÍ¿¿½üÊ±£¬ÓïÒôÌáÊ¾ºÍ¿ª¹ØËøµÄÊ±¼ä¼ä¸ô

tByte raised_alarm_count = 0;    //Ö÷»ú±»Ì§Æğºó£¬Ïò¸½»ú·¢³ö±¨¾¯ĞÅºÅµÄ´ÎÊı
tByte fell_alarm_count=0;        //Ö÷»úµ¹µØºó£¬Ïò¸½»ú·¢³ö±¨¾¯ĞÅºÅµÄ´ÎÊı
tWord timer0_count=0;		// counter for timer0, increment every ticket 			
tByte received_data_buffer[7]={0x00,0x00,0x00,0x00,0x00,0x00,0x00};		//½ÓÊÕÊı¾İ»º´æ
bit receive_data_finished_flag = 0;		//½ÓÊÕÕâÒ»´®Êı¾İÍê³Éºó£¬´Ë±êÖ¾Î»ÖÃ1
tByte data_count = 0;				//½ÓÊÕÊı¾İ»º´æµÄÎ»Êı£¬¼´±íÃ÷µÚ¼¸¸ö»º´æÊı¾İ
tByte one_receive_byte = 0;		//½ÓÊÕÊı¾İµÄÒ»¸ö×Ö½Ú£¬½ÓÊÕÍêºó½«Æä¸³Öµ¸øreceived_data_bufferÏà¶ÔÓ¦µÄ×Ö½Ú
tByte one_receive_byte_count = 0;			//one_receive_byteÓĞ8Î»£¬´Ë¼ÆÊı±íÃ÷½ÓÊÕµ½µÚ¼¸Î»£¬Ã¿´Î¼ÆÊıµ½8µÄÊ±ºò±íÃ÷Ò»¸ö×Ö½Ú½ÓÊÕÍê³É¡£
bit receive_wire_flag = 1;		//½ÓÊÕÍ¨ĞÅÏßµÄ±êÖ¾Î»£¬1±íÃ÷¸ßµçÆ½£¬0±íÃ÷µÍµçÆ½£¬Ã¿´Îtimer1Òç³öÊ±£¬ÅĞ¶ÏP1.1Ò»´Î¡£ÒÔ´ËÀ´±íÃ÷ÊÇ·ñÎªÒ»´ÎÕıÈ·µÄÏÂ½µÑØ
tByte receive_HV_count = 0;		//¶¨Ê±Æ÷T1ÔÚÃ»ÓĞĞÅºÅµ½À´µÄÊ±ºò£¬¶Ô¸ßµçÆ½¼ÆÊı£¬Ò»µ©³¬¹ıÄ³¸öÖµ£¬Ôò½«one_receive_byte_countÇå0
tByte receive_LV_count = 0;		//Ã¿´Îtimer1Òç³öÊ±ÅĞ¶Ï½ÓÊÕÏßÈç¹ûÎªLV£¬Ôò¼ÆÊı¼Ó1£¬ÒÔ´ËÀ´±íÃ÷µÍµçÆ½µÄÊ±¼ä
tByte fell_wire_time=0;         //µ¹µØ¼ì²âÏß£¬¼ì²âµÍµçÆ½µÄÊ±¼ä
tByte raise_wire_time=0;			//Ì§Æğ¼ì²âÏß£¬¼ì²âµÍµçÆ½µÄÊ±¼ä
tWord raised_fell_number = 0;				//µ¹µØ»òÕßÌ§Æğ³ö·¢ºó£¬¼ÆÊı£¬µ½´ïÒ»¶¨ÊıÖµºó£¬½«ÆäÓë±êÖ¾Î»Ò»ÆğÇåÁã¡£
bit raised_fell_once_flag = 0;			//raised_fell_flagÊÇ·ñÔø¾­±êÖ¾¹ı£¬Èç¹û±êÖ¾¹ıÔòÖÃ1.È»ºóÖ÷»ú±»»Ö¸´µ¹µØ»òÕß»Ö¸´Ì§ÆğÊ±£¬´Ë±êÖ¾Î»¸´Î»¡£
tByte key_rotated_on_flag = 0;			//µç¶¯³µ¿ªÆô¹Ø±Õ±êÖ¾Î»£¬1±íÊ¾µç¶¯³µ¿ªÆôÁË£¬0±íÊ¾µç¶¯³µ¹Ø±ÕÁË
tWord ADC_check_result = 0;		//×÷ÎªAD¼ì²âÖµ
tByte wire_broken_count = 0;		// ×÷Îª¶ÏÏßºóµÄÊ±¼ä¼ì²â
bit battery_stolen_EN = 0;			// ×÷Îªµç³Ø±»µÁµÄÊ¹ÄÜ¶Ë
tByte battery_stolen_count = 0;	// ×÷Îªµç³Ø±»µÁµÄ±¨¾¯´ÎÊı
bit horizontal_vibration = 0;		// ±íÊ¾´¹Ö±´«¸ĞÆ÷ÔÚÕñ¶¯£¬´ËÊ±¾ÍËã¹ØÔ¿³×£¬Ò²²»ÄÜÖ´ĞĞ¹ØÔ¿³×µÄ²Ù×÷¡
tWord horizontal_vibration_count = 0;	//´¹Ö±´«¸ĞÆ÷´¥·¢ºó£¬¶ÔÊ±¼ä½øĞĞ¼ÆÊı¡£
bit vibration_flag = 0;
tWord vibration_count = 0;
bit wire_broken_flag = 0;			// ¼ô¶ÏËøÏßµÄ±êÖ¾Î»
bit IDkey_flag = 0;			// µ±ID¿¨¿¿½üÊ±ÈÏÖ¤Í¨¹ıºóÖÃ1£¬
tByte IDkey_count = 0;		// ID¿¨ÈÏÖ¤Í¨¹ıºó£¬¼ÆÊ±1·ÖÖÓ£¬Ê¹Ô¿³×ÄÜ×ª¶¯¡£
bit sensor_EN = 0;
tByte enable_sensor_delay_count = 0;		// ´«¸ĞÆ÷ÑÓ³ÙµÄÊ±¼ä
bit sensor_3rdalarm_flag = 0;
bit wheeled_flag = 0;
tWord wheeled_count = 0;
bit IDkey_speech_flash = 0;

code tByte IDkey6 _at_ 0x003000;
code tByte IDkey7 _at_ 0x003001;
code tByte IDkey8 _at_ 0x003002;
code tByte IDkey9 _at_ 0x003003;
code tByte IDkey10 _at_ 0x003004;
code tByte IDkey11 _at_ 0x003005;

tWord IDkey_selflearn_HVcount = 0;
tWord IDkey_selflearn_LVcount = 0;
bit IDkey_selflearn_flag1 = 0;
bit IDkey_selflearn_flag2 = 0;
bit IDkey_selflearn_flag3 = 0;
bit IDkey_selflearn_flag4 = 0;
bit IDkey_selflearn_flag5 = 0;
bit IDkey_selflearn_flag6 = 0;
tByte IDkey_selflearn_flag4count = 0;
bit IDkey_flash_EN = 0;

tByte IDkey_certificated_times = 0;
bit Silence_Flag = 0;
bit flashing_flag = 0;

/*------- Private variable declarations --------------------------*/

void main()
	{	

	noVoice();
	
	InitUART();
	InitT0(1);
	TI = 0;
	RI = 0;
	ES = 1;
	ET0 = 1;
	PS = 1;
	EA = 1;

	sensor_EN = 0;
 	position_sensor_EN = 0;
	enable_sensor_delayEN = 1;
	
	raised_sensor_detect = 1;
	fell_sensor_detect = 1;

	P10=1;
	
//	horizontal_sensor = 1;
	
	// turn off transmitter, turn on receiver
	transmiter_EN = 1;
	receiver_EN = 0;


	nearby_away_interval = 6;			//³õÊ¼»¯Ê¹¸½»úºÍÖ÷»ú¿ª¹Ø»úµÄ¼ÆÊıÎª¿ÉÒÔÖ´ĞĞµÄ×´Ì¬
	
	// initialize the magnet, 
	MagentControl_1 = 1;
	MagentControl_2 = 1;
	
	transmiter_power = 1; 
	voice_EN=0;		//½«¹¦·Å¹Ø±Õ
   
	// lock the external motor, ·ÀÖ¹Ëø»¹Ã»ÍêÈ«´ò¿ªµÄÊ±ºò£¬³µÊÖ¼Óµçµ¼ÖÂÂÖ×ÓÓëËøµÄ¸æËßÅö×²¡£ 
	motor_lock = 1;
	TR0 = 1;
	
	while(1)
		{
      SCH_Dispatch_Tasks();
		
		// whether host has been touched 3 times, if yes, then alarm 2 speech alternantively.
		if((host_stolen_alarm1_EN == 1)&&(host_stolen_alarm1_count < 4))
			{
			stolen_alarm_flag = 1;
			if(key_rotate == 0)
				{
				SCH_Add_Task(stolen_alarm_speech1, 0, 0);
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
				SCH_Add_Task(stolen_alarm_speech2, 0, 0);
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
		
		// Check Battery's voltage
		if(motor_lock == 0)
			SCH_Add_Task(CheckADC, 0, 0);
		
		if(IDkey_flag == 1)
			{
			if(++IDkey_count >= 5)
				{
				IDkey_count = 0;
				IDkey_flag = 0;
				IDkey_certificated_times = 0;
				if(key_rotated_on_flag == 0)
					{
					enable_sensor();					
					}
				}			
			}
					
		if((enable_sensor_delayEN == 1)&&(key_rotate == 0))
			{
			if(++enable_sensor_delay_count >= 3)
				{
				enable_sensor_delay_count = 0;
				enable_sensor_delayEN = 0;
				enable_sensor();
				}
			}

		if(IDkey_selflearn_flag4 == 1)
			{
			if(++IDkey_selflearn_flag4count > 5)
				{
				IDkey_selflearn_HVcount = 0;
				IDkey_selflearn_LVcount = 0;
				IDkey_selflearn_flag1 = 0;
				IDkey_selflearn_flag2 = 0;
				IDkey_selflearn_flag3 = 0;
				IDkey_selflearn_flag4 = 0;
				IDkey_selflearn_flag4count = 0;				
				}			
			}
		}	

	if(IDkey_speech_flash == 1)
		{
		IDkey_speech_flash = 0;
//		SCH_Add_Task(ID_speech, 0, 0);
      ID_speech();
		SCH_Delete_Program(stolen_alarm_speech1);
		SCH_Delete_Program(stolen_alarm_speech2);
		}

	if(key_rotate == 1)
		{
		IDkey_selflearn_LVcount = 0;
					
		if(++IDkey_selflearn_HVcount > 4000)
			{
			IDkey_selflearn_HVcount = 4002;
			IDkey_selflearn_flag1 = 0;
			IDkey_selflearn_flag2 = 0;
			IDkey_selflearn_flag3 = 0;
			IDkey_selflearn_flag4 = 0;
			IDkey_selflearn_flag5 = 0;
			}
		else
			{
			IDkey_selflearn_flag1 = 1;
			if(IDkey_selflearn_flag2 == 1)
				IDkey_selflearn_flag3 = 1;
			if(IDkey_selflearn_flag4 == 1)
				IDkey_selflearn_flag5 = 1;
			}
		}
	else
		{
		IDkey_selflearn_HVcount = 0;
		
		if(IDkey_selflearn_flag1 == 1)
			IDkey_selflearn_flag2 = 1;
			
		if(IDkey_selflearn_flag3 == 1)
			IDkey_selflearn_flag4 = 1;
		
		if(IDkey_selflearn_flag5 == 1)
			IDkey_selflearn_flag6 = 1;
		
		if(++IDkey_selflearn_LVcount > 4000)
			{
			IDkey_selflearn_LVcount = 4002;
			IDkey_selflearn_flag1 = 0;
			IDkey_selflearn_flag2 = 0;
			IDkey_selflearn_flag3 = 0;
			IDkey_selflearn_flag4 = 0;
			IDkey_selflearn_flag5 = 0;
			}
		}



	if(IDkey_flash_EN == 1)
		{
		IDkey_flash_EN = 0;
		flashing_flag = 1;
		IDkey_selflearn_flag1 = 0;
		IDkey_selflearn_flag2 = 0;
		IDkey_selflearn_flag3 = 0;
		IDkey_selflearn_flag4 = 0;
		IDkey_selflearn_flag5 = 0;
		IDkey_selflearn_flag6 = 0;
		SCH_Add_Task(Self_learn_programming, 0, 0);
		SCH_Add_Task(Self_learn_speech, 0, 0);
		}
	
	// detect whether key is rotated on,  
	if((key_rotate == 1)&&(key_rotated_on_flag == 0)&&(IDkey_flag == 1))		
		{
		Delay_1ms();
		// anti-trigger, Delay(5) confirm the key rotation.
		if(key_rotate == 1)
			{
			disable_sensor();

			IDkey_count = 0;
			IDkey_flag = 0;
			IDkey_certificated_times = 0;

			SCH_Add_Task(ElecMotor_CW, 0, 0);

			SCH_Add_Task(slave_nearby_operation, 0, 0);
			}
		} 		
				
	// detect whether key is rotated off
	if((key_rotate == 0)&&(key_rotated_on_flag == 1))
		{
		if((vibration_flag == 0)&&(wheeled_flag == 0))
			{
			Delay_1ms();
			if(key_rotate == 0)
				{
				// reset key rotation flag
				key_rotated_on_flag=0;
				SCH_Add_Task(slave_away_operation, 0, 0);
									
				}				
			}
		}
		
	if((sensor_detect == 0)||(horizontal_sensor == 0))
		{
		vibration_flag = 1;
		vibration_count = 0;
		}
    if(vibration_flag == 1)
		{
		if(++vibration_count >= 2000)
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
	
			
	if((sensor_detect == 0)||(horizontal_sensor == 0))
		{
		vibration_flag = 1;
		vibration_count = 0;			
		}		
		
	
	// judge whether battery has been stolen, 0.4V/4V
	if((ADC_check_result < 0x096) && (key_rotate == 0))
		{
		battery_stolen_EN = 1;
		}			
	else if(ADC_check_result > 0x100)
		{
		battery_stolen_EN = 0;
		battery_stolen_count = 0;
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
//					if((sensor_detect == 0)&&(stolen_alarm_flag == 0)&&(transmiter_EN == 1))		
					if(((sensor_detect == 0)||(horizontal_sensor == 0))&&(stolen_alarm_flag == 0)&&(flashing_flag == 0))		
						{
						// judge LV is more than 2ms, if yes, it means a effective touch
						if(++sensor_1ststage_count >= 1)			
							{
							sensor_1ststage_count=0;
							
							// sensor trigge status progress to case 1.
							sensor_trigger_count = 1;
							// alarm speech for first touch
//							SCH_Add_Task(host_touch_speech, 0, 0);
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
					if((sensor_detect == 0)||(horizontal_sensor == 0))
						{
						// LV for 2s, means a effective touch
						if(++sensor_2ndstage_count >= 1)
							{
							sensor_2ndstage_count = 0;
							sensor_trigger_count = 2;
							// alarm speech for 2nd touch
//							SCH_Add_Task(host_2ndtouch_speech, 0, 0);
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
					}	
				}
			else if((wire_broken == 1)&&(sensor_3rdalarm_flag == 0))
				{
				wire_broken_count = 0;
				host_stolen_alarm1_EN = 0;
				host_stolen_alarm2_EN = 0;
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
			if(++raise_wire_time==10)
				{
				// flag raised, and reset fell
				raised_flag=1;
				fell_flag=0;
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
			if(++fell_wire_time==10)	
				{
				// flag fell, and reset raised
				fell_flag=1;			
				raised_flag=0;
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
	UART interrupt
-----------------------------------------------*/
void uart_isr() interrupt 4 
	{
	if(RI)
		{
		RI=0;
		received_data_buffer[data_count] = SBUF;

		// assign one byte to buffer[i] 
		
		if(IDkey_selflearn_flag6 == 0)
			{
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
				IDkey_speech_flash = 1;
				
				if(IDkey_certificated_times++ >= 1)
					{
					Silence_Flag = 1;
					}
				}
			else 
				{
				data_count = 0;
				}			
			}
		else
			{
			
			if(++data_count >= 6)
				{
				data_count = 0;
				IDkey_flash_EN = 1;
				}
			}
		}
	}


/*---------------------------------------------------
	end of file
----------------------------------------------------*/