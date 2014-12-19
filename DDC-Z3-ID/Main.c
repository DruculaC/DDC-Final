/*---------------------------------------------------
	main.c (v1.00)
	
	DDC-Z program, for electrocar, with ID card.
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
                                        
/*------- Public variable declarations --------------------------*/
extern tByte timer0_8H, timer0_8L, timer1_8H, timer1_8L;
extern bit enable_sensor_delayEN;

// ------ Private variable definitions -----------------------------
bit stolen_alarm_flag = 0;					// when host been touch 3 times, this flag 1 before alarm voice present, not to detect sensor for 1st voice alarm.
bit position_sensor_EN=0;  		//Î»ÖÃ´«¸ÐÆ÷£¬¼´µ¹µØÌ§Æð´«¸ÐÆ÷×Ü¿ª¹Ø£¬1µÄÊ±ºò£¬¼ì²âÕâÁ½¸ö´«¸ÐÆ÷
bit host_stolen_alarm1_EN = 0;      //ÅÐ¶ÏÎª±»µÁ±¨¾¯ºóµÄµÚÒ»¶ÎÓïÒôÊ¹ÄÜ
bit host_stolen_alarm2_EN = 0;      //ÅÐ¶ÏÎª±»µÁ±¨¾¯ºóµÄµÚ¶þ¶ÎÓïÒôÊ¹ÄÜ
tByte host_stolen_alarm1_count = 0;		//ÅÐ¶ÏÎª±»µÁ±¨¾¯ºóµÄµÚÒ»¶ÎÓïÒô´ÎÊý
tByte host_stolen_alarm2_count = 0;		//ÅÐ¶ÏÎª±»µÁ±¨¾¯ºóµÄµÚ¶þ¶ÎÓïÒô´ÎÊý
bit fell_flag=0;						//ÅÐ¶ÏÖ÷»úµ¹ÏÂºó£¬ÖÃ1
bit raised_flag=0;					//ÅÐ¶ÏÖ÷»ú±»Ì§Æðºó£¬ÖÃ1
tByte sensor_2ndstage_count = 0;		//´«¸ÐÆ÷½øÈëµÚ¶þ½×¶Î¼ì²âÊ±£¬¼ÆËãµÍµçÆ½µÄÊ±¼ä
tWord sensor_3rdstage_time = 0;			//´«¸ÐÆ÷½øÈëµÚÈý½×¶ÎµÄÊ±¼ä£¬
tByte sensor_3rdstage_effcount = 0;		//´«¸ÐÆ÷½øÈëµÚÈý½×¶Îºó£¬ÓÐÐ§´¥Åö´ÎÊýµÄ¼ÆÊý
tByte sensor_3rdstage_count = 0;			//´«¸ÐÆ÷½øÈëµÚÈý½×¶Îºó£¬µÍµçÆ½µÄ¼ÆÊý
tWord sensor_3rdstage_interval = 0;		//´«¸ÐÆ÷ÔÚµÚÈý½×¶ÎÖÐ£¬Ã¿´ÎÓÐÐ§µÍµçÆ½¼ÆÊýÖ®¼äµÄÊ±¼ä¼ä¸ô¡£ÔÚÕâÆÚ¼äµÄµÍµçÆ½²»ÈÏÎªÓÐÐ§¡£
bit raised_fell_flag = 0;					//µ¹µØ»òÕßÌ§Æð´¥·¢ºó£¬´Ë±êÖ¾Î»ÖÃ1
tByte sensor_trigger_count=0;		//´«¸ÐÆ÷´¥·¢¼ÆÊý
tWord sensor_2ndstage_time=0;		//´«¸ÐÆ÷½øÈëµÚ¶þ½×¶ÎºóÁ÷ÊÅÊ±¼äµÄ¼ÆÊý
tByte sensor_1ststage_count=0;	//´«¸ÐÆ÷µÚÒ»½×¶ÎÅÐ¶ÏµÍµçÆ½µÄ¼ÆÊý
tByte nearby_away_interval = 0;		//¸½»úÀë¿ªºÍ¿¿½üÊ±£¬ÓïÒôÌáÊ¾ºÍ¿ª¹ØËøµÄÊ±¼ä¼ä¸ô

tByte raised_alarm_count = 0;    //Ö÷»ú±»Ì§Æðºó£¬Ïò¸½»ú·¢³ö±¨¾¯ÐÅºÅµÄ´ÎÊý
tByte fell_alarm_count=0;        //Ö÷»úµ¹µØºó£¬Ïò¸½»ú·¢³ö±¨¾¯ÐÅºÅµÄ´ÎÊý
tWord timer0_count=0;		// counter for timer0, increment every ticket 			
tByte received_data_buffer[7]={0x00,0x00,0x00,0x00,0x00,0x00,0x00};		//½ÓÊÕÊý¾Ý»º´æ
bit receive_data_finished_flag = 0;		//½ÓÊÕÕâÒ»´®Êý¾ÝÍê³Éºó£¬´Ë±êÖ¾Î»ÖÃ1
tByte data_count = 0;				//½ÓÊÕÊý¾Ý»º´æµÄÎ»Êý£¬¼´±íÃ÷µÚ¼¸¸ö»º´æÊý¾Ý
tByte one_receive_byte = 0;		//½ÓÊÕÊý¾ÝµÄÒ»¸ö×Ö½Ú£¬½ÓÊÕÍêºó½«Æä¸³Öµ¸øreceived_data_bufferÏà¶ÔÓ¦µÄ×Ö½Ú
tByte one_receive_byte_count = 0;			//one_receive_byteÓÐ8Î»£¬´Ë¼ÆÊý±íÃ÷½ÓÊÕµ½µÚ¼¸Î»£¬Ã¿´Î¼ÆÊýµ½8µÄÊ±ºò±íÃ÷Ò»¸ö×Ö½Ú½ÓÊÕÍê³É¡£
bit receive_wire_flag = 1;		//½ÓÊÕÍ¨ÐÅÏßµÄ±êÖ¾Î»£¬1±íÃ÷¸ßµçÆ½£¬0±íÃ÷µÍµçÆ½£¬Ã¿´Îtimer1Òç³öÊ±£¬ÅÐ¶ÏP1.1Ò»´Î¡£ÒÔ´ËÀ´±íÃ÷ÊÇ·ñÎªÒ»´ÎÕýÈ·µÄÏÂ½µÑØ
tByte receive_HV_count = 0;		//¶¨Ê±Æ÷T1ÔÚÃ»ÓÐÐÅºÅµ½À´µÄÊ±ºò£¬¶Ô¸ßµçÆ½¼ÆÊý£¬Ò»µ©³¬¹ýÄ³¸öÖµ£¬Ôò½«one_receive_byte_countÇå0
tByte receive_LV_count = 0;		//Ã¿´Îtimer1Òç³öÊ±ÅÐ¶Ï½ÓÊÕÏßÈç¹ûÎªLV£¬Ôò¼ÆÊý¼Ó1£¬ÒÔ´ËÀ´±íÃ÷µÍµçÆ½µÄÊ±¼ä
tByte fell_wire_time=0;          //µ¹µØ¼ì²âÏß£¬¼ì²âµÍµçÆ½µÄÊ±¼ä
tByte raise_wire_time=0;			//Ì§Æð¼ì²âÏß£¬¼ì²âµÍµçÆ½µÄÊ±¼ä
tWord raised_fell_number = 0;				//µ¹µØ»òÕßÌ§Æð³ö·¢ºó£¬¼ÆÊý£¬µ½´ïÒ»¶¨ÊýÖµºó£¬½«ÆäÓë±êÖ¾Î»Ò»ÆðÇåÁã¡£
bit raised_fell_once_flag = 0;			//raised_fell_flagÊÇ·ñÔø¾­±êÖ¾¹ý£¬Èç¹û±êÖ¾¹ýÔòÖÃ1.È»ºóÖ÷»ú±»»Ö¸´µ¹µØ»òÕß»Ö¸´Ì§ÆðÊ±£¬´Ë±êÖ¾Î»¸´Î»¡£
tByte key_rotated_on_flag=1;			//µç¶¯³µ¿ªÆô±êÖ¾Î»£¬1±íÊ¾µç¶¯³µ¿ªÆôÁË£¬0±íÊ¾µç¶¯³µ¹Ø±ÕÁË
tByte key_rotated_off_flag=0;			//µç¶¯³µ¹Ø±Õ±êÖ¾Î»£¬1±íÊ¾µç¶¯³µ¿ªÆôÁË£¬0±íÊ¾µç¶¯³µ¹Ø±ÕÁË
tWord ADC_check_result = 0;		//×÷ÎªAD¼ì²âÖµ
tByte wire_broken_count = 0;		// ×÷Îª¶ÏÏßºóµÄÊ±¼ä¼ì²â
bit battery_stolen_EN = 0;			// ×÷Îªµç³Ø±»µÁµÄÊ¹ÄÜ¶Ë
tByte battery_stolen_count = 0;	// ×÷Îªµç³Ø±»µÁµÄ±¨¾¯´ÎÊý
bit horizontal_vibration = 0;		// ±íÊ¾´¹Ö±´«¸ÐÆ÷ÔÚÕñ¶¯£¬´ËÊ±¾ÍËã¹ØÔ¿³×£¬Ò²²»ÄÜÖ´ÐÐ¹ØÔ¿³×µÄ²Ù×÷¡
tWord horizontal_vibration_count = 0;	//´¹Ö±´«¸ÐÆ÷´¥·¢ºó£¬¶ÔÊ±¼ä½øÐÐ¼ÆÊý¡£
bit vibration_flag = 0;
tWord vibration_count = 0;
bit wire_broken_flag = 0;			// ¼ô¶ÏËøÏßµÄ±êÖ¾Î»
tByte IDkey5 = IDkey0^IDkey1^IDkey2^IDkey3^IDkey4;		// ÓÉÇ°5Î»ÃÜÂë×é³ÉµÄÒì»ò
bit IDkey_flag = 0;			// µ±ID¿¨¿¿½üÊ±ÈÏÖ¤Í¨¹ýºóÖÃ1£¬
tByte IDkey_count = 0;		// ID¿¨ÈÏÖ¤Í¨¹ýºó£¬¼ÆÊ±1·ÖÖÓ£¬Ê¹Ô¿³×ÄÜ×ª¶¯¡£
bit sensor_EN = 0;
tByte enable_sensor_delay_count = 0;		// ´«¸ÐÆ÷ÑÓ³ÙµÄÊ±¼ä
bit sensor_3rdalarm_flag = 0;
bit wheeled_flag = 0;
tWord wheeled_count = 0;


void main()
	{
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

	noVoice();

	voice_EN=0;		//½«¹¦·Å¹Ø±Õ

	raised_sensor_detect=1;
	fell_sensor_detect=1;

	P10=1;
	
    horizontal_sensor = 1;
	// open communication
	
	// turn off transmitter, turn on receiver
	transmiter_EN = 1;
	receiver_EN = 0;

	nearby_away_interval = 6;			 //³õÊ¼»¯Ê¹¸½»úºÍÖ÷»ú¿ª¹Ø»úµÄ¼ÆÊýÎª¿ÉÒÔÖ´ÐÐµÄ×´Ì¬
	
	// initialize the magnet, 
	MagentControl_1 = 1;
	MagentControl_2 = 1;

	transmiter_power = 1; 
   
	wire_broken = 1;
	
	// lock the external motor, ·ÀÖ¹Ëø»¹Ã»ÍêÈ«´ò¿ªµÄÊ±ºò£¬³µÊÖ¼Óµçµ¼ÖÂÂÖ×ÓÓëËøµÄ¸æËßÅö×²¡£ 
	motor_lock = 1;
	
	while(1)
		{        
		
		// ÅÐ¶Ï´«¸ÐÆ÷ÊÇ·ñÔÚÕñ¶¯
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
		
		// detect the battery voltage
		ADC_check_result = GetADCResult(6);	
		
		// if fell and raised flag is 1, send alarm signal every 2s.
		if((fell_flag==1)&&(fell_alarm_count<5))
			{
			ComMode_5_Data();
			fell_alarm_count++;
			}
		if((raised_flag==1)&&(raised_alarm_count<5))		
			{
			ComMode_4_Data();
			raised_alarm_count++;
			}
			
		if((battery_stolen_EN == 1)&&(battery_stolen_count < 4))
			{
			if(key_rotate == 0)
				{
				ComMode_2_Data();
				battery_stolen_speech();
				battery_stolen_count++;
				}
			}
		
		if(IDkey_flag == 1)
			{
			if(++IDkey_count >= 15)
				{
				IDkey_count = 0;
				IDkey_flag = 0;
				if(key_rotated_off_flag == 1)
					{
					enable_sensor();					
					}
				}			
			}
									
		if((enable_sensor_delayEN == 1)&&(key_rotate == 0)&&(vibration_flag == 0))
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
				if(wire_broken_flag == 0)
					{
					ComMode_3_Data();                                                                  
					}
				else
					{                                                                     
					ComMode_6_Data();
					}
					
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
				if(wire_broken_flag == 0)
					{
					ComMode_3_Data();
					}
				else
					{
					ComMode_6_Data();
					}

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
	
	// detect whether key is rotated on,  
	if((key_rotate == 1)&&(key_rotated_on_flag == 0)&&(IDkey_flag == 1))		
		{                                                                        
		Delay(5);
		// anti-trigger, Delay(5) confirm the key rotation.
		if(key_rotate == 1)
			{
			slave_nearby_operation();                     
			// flag key rotation status
			key_rotated_on_flag = 1;
			key_rotated_off_flag = 0;
			IDkey_count = 0;
			IDkey_flag = 0;				
			}
		}
		
	// detect whether key is rotated off
	if((key_rotate == 0)&&(key_rotated_off_flag == 0))
		{
		if((vibration_flag == 0)&&(wheeled_flag == 0))
			{
			Delay(5);
			if(key_rotate == 0)
				{
				// handle with battery status
				verifybattery(ADC_check_result);
				// reset key rotation flag
				key_rotated_on_flag = 0;
				key_rotated_off_flag = 1;
				slave_away_operation();
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
//					if(((sensor_detect == 0)||(horizontal_sensor == 0))&&(stolen_alarm_flag == 0))		
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
						// LV for 2s, means a effective touch             8
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
//					if((sensor_detect == 0)||(horizontal_sensor == 0))
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
			else if(ADC_check_result > 0x200)
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
	ÓÃUART¶Ë¿Ú½ÓÊÕÊý¾Ý£¬9600²¨ÌØÂÊ£¬ÊÕµ½ºó±íÊ¾
	ÈÏÖ¤ÒÑÍ¨¹ý¡£
-----------------------------------------------*/
void uart_isr() interrupt 4 //ÖÐ¶Ï½ÓÊÕÊý¾Ý
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

/*--------------------------------------------------------------------
	timerT1()
	¶¨Ê±Æ÷1Ã¿´ÎÒç³öºóÖ´ÐÐµÄ²Ù×÷£¬µÍËÙÂÊÄ£Ê½½ÓÊÕ
	Êý¾Ý¡£

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
				if((nearby_away_interval > 6)||(key_rotate == 0))
					{
					// if receive a ComMode_1 successfully, operation for slave nearby
					nearby_away_interval = 0;					
					}
				
				// turn off vibration and position sensor
				sensor_EN = 0;							
				position_sensor_EN=0; 			
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
				
				// reset the battery stolen flag
				battery_stolen_EN = 0;
				battery_stolen_count = 0;
				}
			break;
			}
		}
	}
--------------------------------------------------------------------*/


/*---------------------------------------------------
	end of file
----------------------------------------------------*/