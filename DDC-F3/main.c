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
extern tByte IDkey5;		// ÓÉÇ°5Î»ÃÜÂë×é³ÉµÄÒì»ò
bit IDkey_flag = 0;			// µ±ID¿¨¿¿½üÊ±ÈÏÖ¤Í¨¹ıºóÖÃ1£¬
tByte IDkey_count = 0;		// ID¿¨ÈÏÖ¤Í¨¹ıºó£¬¼ÆÊ±1·ÖÖÓ£¬Ê¹Ô¿³×ÄÜ×ª¶¯¡£
bit sensor_EN = 0;
tByte enable_sensor_delay_count = 0;		// ´«¸ĞÆ÷ÑÓ³ÙµÄÊ±¼ä
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
bit Mode_select = 0;	// Ä£Ê½Ñ¡Ôñ£¬0±íÊ¾×Ô¶¯Ä£Ê½£¬1±íÊ¾ÊÖ¶¯Ä£Ê½

bit powersave_enable = 0;
tWord receiver_EN_count = 0;

bit itrpt_EN = 1;
tByte itrpt_count = 0;
bit idle_EN = 1;

bit Auto_transmit = 0;			// 0±íÊ¾·Ç×Ô¶¯·¢ËÍÊı¾İ£¬1±íÊ¾×Ô¶¯·¢ËÍÊı¾İ½øĞĞÈÏÖ¤

/*--------------------------------------------------------------*/

void main(void)
	{
	noVoice();
	InitTimer(50, 100);
    TR0 = 1;

	// ¼üÅÌÖĞ¶Ï³õÊ¼»¯
	press_open_button = 1;
	press_close_button = 1;
	
	KBLS1 |= 0x03;
	KBLS0 |= 0x03;
	KBIF &= 0xfc;
	KBIE |= 0x03;
	EA = 1;

	Moto_EN = 1;		//³õÊ¼»¯£¬¹Ø±ÕÂí´ï
	transmit_wire = 1;
	voice_EN = 0;		  	//¿ª»úÊ±£¬½«¹¦·Å¹Ø±Õ
	
	ADC_check_result = GetADCResult(6);		//ÉÏµçÊ±£¬µç³ØµçÁ¿¼ì²âÒ»´Î
	
	stolen_alarm_count = 0;			//Çå±¨¾¯¼ÆÊıÆ÷
	stolen_alarm_flag = 0;			//Çå±¨¾¯±êÖ¾

	transmiter_EN = 0;		// turn off the transmitter
	receiver_EN = 0;		// turn on the receiver

	transceiver_power_enable = 1;         // ÉÏµçÊ±ÎŞÏßÄ£¿éµçÔ´¹Ø±Õ
	
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
	¶¨Ê±Æ÷0Ã¿´ÎÒç³öºóÖ´ĞĞµÄ²Ù×÷
--------------------------------------------------------------------*/

void timer0() interrupt interrupt_timer_0_overflow
	{
	// ÖØ×°ÔÚ¶¨Ê±Æ÷µÄÊ±¼äÉèÖÃ
	TH0 = timer0_8H;
	TL0 = timer0_8L;

	// ÉèÖÃÒ»¸öÃ¿3sµÄ²Ù×÷
	if(++timer0_count >= 60)		
		{
			
		// Ã¿¸ö3s×öÒ»´ÎµçÁ¿¼ì²â£¬²¢½øĞĞÏà¹ØµÄµçÁ¿ÌáÊ¾
		ADC_check_result = GetADCResult(6);
		
		// ÅĞ¶Ïµç³ØµçÑ¹£¬Èç¹ûĞ¡ÓÚ3.4VÇÒÔ­ÏÈ±¨¹ı¸ßµçÑ¹Ê±£¬Ôò±¨µçÁ¿²»×ã¡£Èç¹ûµçÑ¹´óÓÚ4.2VÇÒÓëÔ­ÏÈ±¨¹ıµçÁ¿²»×ãÊ±£¬ÔòÌáÊ¾µçÑ¹¹ı¸ß
		if((battery_HV_flag == 1)&&(ADC_check_result <= 0x365))
			{
			// ´ËÎ»ÖÃ0£¬±íÊ¾µçÁ¿¹ıµÍ
			battery_HV_flag = 0;
			Battery_low_alarm_speech();		
			}
		else if((battery_HV_flag == 0)&&(ADC_check_result >= 0x370))
			{
			// ´ËÎ»ÖÃ1£¬±íÊ¾µçÁ¿¹ı¸ß
			battery_HV_flag = 1;
			Battery_high_alarm_speech();
			}	

		// ½«¼ÆÊıÇå0
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
			
 	// Ö÷»ú±»µÁ±¨¾¯
	if(stolen_alarm_flag == 1)		
		{
		// Ö÷»ú±»µÁ±¨¾¯´ÎÊı£¬Ã¿±¨Ò»´Î¼Ó1£¬Èç¹û´óÓÚ2´Î£¬Ôò¼ÌĞø¼Ó1£¬µ½Ò»¶¨Ê±¼äºó£¬½«flagºÍcountÇå0.ÒÔ´ËÀ´±£Ö¤¸½»úÃ¿´Î±¨¾¯ºó£¬ÔÚ¶ÌÊ±¼äÄÚ
		// ÔÙ´ÎÊÕµ½±¨¾¯ĞÅºÅ£¬¾Í²»»á±¨¾¯ÁË¡£
		if(++stolen_alarm_count < 3)
			{			
			// ÓïÒôÌáÊ¾£¬Âí´ïÕñ¶¯
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

	// Ì§ÆğĞÅºÅ±¨¾¯£¬Ã¿´Î±¨ÍêºóÇå0£¬Èç¹ûÔÙ´Î½Óµ½Ôò¼ÌĞø±¨¡£Ò»°ãÀ´Ëµ£¬Ö÷»úÃ¿´ÎÌ§ÆğÖ»·¢4±é¡£
	if(raised_alarm_flag == 1)	
		{
		Alarm_raised_speech();		
		Moto_Vibration();         
		raised_alarm_flag = 0;
		}

	// µ¹µØĞÅºÅ±¨¾¯£¬Ã¿´Î±¨ÍêºóÇå0£¬Èç¹ûÔÙ´Î½Óµ½Ôò¼ÌĞø±¨¡£Ò»°ãÀ´Ëµ£¬Ö÷»úÃ¿´Îµ¹µØÖ»·¢4±é¡£
	if(fell_alarm_flag == 1)
		{
		Alarm_fell_speech();		  
		Moto_Vibration();         
		fell_alarm_flag=0;
		}

	// Ö÷»úµçÁ¿³äÂúÌáÊ¾
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
	¶¨Ê±Æ÷1Ã¿´ÎÒç³öºóÖ´ĞĞµÄ²Ù×÷
--------------------------------------------------------------------*/
	
void timerT1() interrupt interrupt_timer_1_overflow 			
	{
	// ÖØ×°ÔÚ¶¨Ê±Æ÷1µÄÉèÖÃ
	TH1 = timer1_8H;				
	TL1 = timer1_8L;

	// receive a tyte
	receive_byte();
	
	// receive a word after every byte
	receive_word();

	if(receive_data_finished_flag==1)	//ËµÃ÷½ÓÊÕµ½ÁËÊı¾İ£¬¿ªÊ¼´¦Àí
		{
		receive_data_finished_flag=0;	//Çå½ÓÊÕ±êÖ¾
		switch(received_data_buffer[2])//½âÎöÖ¸Áî
			{
			case ComMode_1://½ÓÊÕµ½µÄÊÇÖ÷»ú·¢ËÍ¹ıÀ´µÄ±àÂë1µÄĞÅºÅ£¬ËµÃ÷Ö÷»úÔÚ3MÄÚ£¬ÊÇÕı³£µÄ
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
	¼üÅÌÖĞ¶Ï£¬Ê¹Ğ¾Æ¬´ÓÊ¡µçÄ£Ê½ÖĞ»½ĞÑ
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