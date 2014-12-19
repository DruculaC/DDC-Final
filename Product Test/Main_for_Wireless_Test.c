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
tByte fell_wire_time=0;          //µ¹µØ¼ì²âÏß£¬¼ì²âµÍµçÆ½µÄÊ±¼ä
tByte raise_wire_time=0;			//Ì§Æğ¼ì²âÏß£¬¼ì²âµÍµçÆ½µÄÊ±¼ä
tWord raised_fell_number = 0;				//µ¹µØ»òÕßÌ§Æğ³ö·¢ºó£¬¼ÆÊı£¬µ½´ïÒ»¶¨ÊıÖµºó£¬½«ÆäÓë±êÖ¾Î»Ò»ÆğÇåÁã¡£
bit raised_fell_once_flag = 0;			//raised_fell_flagÊÇ·ñÔø¾­±êÖ¾¹ı£¬Èç¹û±êÖ¾¹ıÔòÖÃ1.È»ºóÖ÷»ú±»»Ö¸´µ¹µØ»òÕß»Ö¸´Ì§ÆğÊ±£¬´Ë±êÖ¾Î»¸´Î»¡£
tByte key_rotated_on_flag=0;			//µç¶¯³µ¿ªÆô¹Ø±Õ±êÖ¾Î»£¬1±íÊ¾µç¶¯³µ¿ªÆôÁË£¬0±íÊ¾µç¶¯³µ¹Ø±ÕÁË
tWord ADC_check_result = 0;		//×÷ÎªAD¼ì²âÖµ
tByte wire_broken_count = 0;		// ×÷Îª¶ÏÏßºóµÄÊ±¼ä¼ì²â
bit battery_stolen_EN = 0;			// ×÷Îªµç³Ø±»µÁµÄÊ¹ÄÜ¶Ë
tByte battery_stolen_count = 0;	// ×÷Îªµç³Ø±»µÁµÄ±¨¾¯´ÎÊı
bit horizontal_vibration = 0;		// ±íÊ¾´¹Ö±´«¸ĞÆ÷ÔÚÕñ¶¯£¬´ËÊ±¾ÍËã¹ØÔ¿³×£¬Ò²²»ÄÜÖ´ĞĞ¹ØÔ¿³×µÄ²Ù×÷¡
tWord horizontal_vibration_count = 0;	//´¹Ö±´«¸ĞÆ÷´¥·¢ºó£¬¶ÔÊ±¼ä½øĞĞ¼ÆÊı¡£
bit vibration_flag = 0;
tWord vibration_count = 0;
bit wire_broken_flag = 0;			// ¼ô¶ÏËøÏßµÄ±êÖ¾Î»
tByte IDkey5 = IDkey0^IDkey1^IDkey2^IDkey3^IDkey4;		// ÓÉÇ°5Î»ÃÜÂë×é³ÉµÄÒì»ò
bit IDkey_flag = 0;			// µ±ID¿¨¿¿½üÊ±ÈÏÖ¤Í¨¹ıºóÖÃ1£¬
tByte IDkey_count = 0;		// ID¿¨ÈÏÖ¤Í¨¹ıºó£¬¼ÆÊ±1·ÖÖÓ£¬Ê¹Ô¿³×ÄÜ×ª¶¯¡£
bit sensor_EN = 0;
tByte enable_sensor_delay_count = 0;		// ´«¸ĞÆ÷ÑÓ³ÙµÄÊ±¼ä
bit sensor_3rdalarm_flag = 0;
bit wheeled_flag = 0;
tWord wheeled_count = 0;

bit test_flag = 0;		// ÉèÖÃ²âÊÔ¿ªÊ¼µÄ±êÖ¾Î»

/*------- Private variable declarations --------------------------*/

void main()
	{
	ADC_check_result = 0x3FF;
	
	// initialize the magnet, 
	MagentControl_1 = 1;
	MagentControl_2 = 1;
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
		
	
	// ³õÊ¼»¯Íê±Ïºó£¬¿ªÊ¼²âÊÔ³ÌĞò
	test_flag = 1;
	
		
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
	if(++timer0_count >= 2000)
		{
		// reset timer0 ticket counter every 2s
		timer0_count=0;
		
		if(test_flag == 1)
			{

			// ·¢±¨¾¯ĞÅºÅ³öÈ¥
			ComMode_3_Data();

			// ¼ì²âADCµÄµçÑ¹Öµ
			ADC_check_result = GetADCResult(6);	
			
			Delay(40);

			// ·¢±¨¾¯ĞÅºÅ³öÈ¥
			ComMode_3_Data();

			Delay(40);
			// ·¢±¨¾¯ĞÅºÅ³öÈ¥
			ComMode_3_Data();
			}
		}

	// ¼ì²âIDÈÏÖ¤Í¨¹ı£¬Í¬Ê±Ô¿³×´ò¿ªºóµÄ²Ù×÷£¬²¥±¨ÓïÒô¡£
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
		
	// ¹ØÔ¿³×µÄ²Ù×÷£¬¼ì²âÊÇ·ñÓĞÂÖ×ÓÔÚ×ª¶¯
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


	// ²âÊÔÂÖ×ÓÔÚ×ª¶¯µÄÊ±ºò£¬²»ÄÜ¹ØËø
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

	// ²âÊÔÕñ¶¯´«¸ĞÆ÷
	if((sensor_detect == 0)||(horizontal_sensor == 0))
		{
		if(++sensor_1ststage_count >= 2)			
			{
			sensor_1ststage_count=0;
			
			host_touch_speech();
			}
		}
								
	// ÄÃµôËø£¬Ä£ÄâÓĞÈË¼ô¶ÏËøÏß£¬²¥±¨ÓïÒô
	if(wire_broken == 0) 
		{
		battery_stolen_speech();
		}

	// ²âÊÔË®Òø¿ª¹Ø£¬µ¹µØ»òÕßÌ§Æğºó£¬²¥±¨ÓïÒô¡£
	if((raised_sensor_detect == 0)||(fell_sensor_detect == 0))
		{
		stolen_alarm_speech2();
		}

/*			
	// ²âÊÔÔÚÓĞµçÆ½µÄÇé¿öÏÂ£¬ÄÃµô48VµçÔ´£¬Ä£ÄâÓĞÈËÍµµçÆ¿µÄÇé¿ö£¬²¥±¨ÓïÒô
	if(ADC_check_result < 0x2c1)
		{
		battery_stolen_speech();
		}
*/
 	}


/*-----------------------------------------------
	ÓÃUART¶Ë¿Ú½ÓÊÕÊı¾İ£¬9600²¨ÌØÂÊ£¬ÊÕµ½ºó±íÊ¾
	ÈÏÖ¤ÒÑÍ¨¹ı¡£
-----------------------------------------------*/
void uart_isr() interrupt 4 //ÖĞ¶Ï½ÓÊÕÊı¾İ
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