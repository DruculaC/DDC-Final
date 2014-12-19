/*----------------------------------------------------
	port.h (v1.00)

	'port header' (see chap 5) for DDC-Z
----------------------------------------------------*/

#ifndef _PORT_H
#define _PORT_H

#include "main.h"

/*----------------------------------------------------
	Ö÷»ú¶Ë¿Ú¶¨Òå
-----------------------------------------------------*/

// P2.5£¬PIN16£¬Á¬½Óµ½Ô¿³××ª¶¯£¬Ô¿³××ª¿ªÊ±£¬´ËpinÎª1£»Ô¿³×¹Ø±ÕÊ±£¬´ËpinÎª0
sbit key_rotate = P2^5;			

// P2.6£¬PIN27£¬·¢Éä»úµÄ¹¦ÂÊ¿ØÖÆ¡£1±íÊ¾Ğ¡¹¦ÂÊ£¬0±íÊ¾´ó¹¦ÂÊ¡£´ó¹¦ÂÊ·¢Éä¾àÀëÔ¶¡£
sbit transmiter_power = P2^6;

// P2.7£¬PIN28£¬·¢Éä»úÊ¹ÄÜ¶Ë¿Ú¡£0±íÊ¾¿ª£¬1±íÊ¾¹Ø
sbit transmiter_EN = P2^7;


// P0.0, PIN3, horizontal sensor input
sbit sensor_detect = P0^0;

// P2.2£¬P2.3£¬·Ö±ğÎªPIN13£¬PIN14£¬µç´ÅÌú,  Æ½Ê±ÖµÎª00£¬¿ªËøÓÃ10£¬¹ØËøÓĞ01£¬È»ºó¶¼»Ö¸´µ½00
sbit MagentControl_1 = P2^2;
sbit MagentControl_2 = P2^3;

// P0.4£¬PIN23£¬Ì§Æğ´«¸ĞÆ÷µÄ¼ì²â
sbit raised_sensor_detect = P0^4;

// P0.3£¬PIN24£¬µ¹µØ´«¸ĞÆ÷µÄ¼ì²â
sbit fell_sensor_detect = P0^3;

// P1.4£¬PIN10£¬¹¦·Å¿ª¹Ø¿ØÖÆ£¬1Îª´ò¿ª¹¦·Å£¬0Îª¹Ø±Õ¹¦·Å
sbit voice_EN = P1^4;

// P2.0£¬P2.1·Ö±ğÎªPIN1£¬PIN2£¬¶ÔÓ¦ÓïÒôĞ¾Æ¬µÄ¸´Î»ºÍÊı¾İ¹Ü½Å
sbit SC_RST=P2^0;
sbit SC_DATA=P2^1;

// P0.7£¬PIN19£¬´«¸ĞÆ÷µÄ¼ì²â½á¹û£¬Í¨¹ı´ËPIN´«¸øMCU£¬È»ºóMCUÅĞ¶Ï
sbit horizontal_sensor = P0^7; 

// P1.3, PIN11, receiver enable£¬0ÊÇ´ò¿ª£¬1ÊÇ¹Ø±Õ
sbit receiver_EN = P1^3;

// P0.5, PIN22, Íâ²¿µç¶¯³µµç»úËøËÀĞÅºÅÊä³ö£¬·ÀÖ¹Ëø»¹Ã»ÍêÈ«´ò¿ªµÄÊ±ºò£¬³µÊÖ¼Óµçµ¼Ö ÂÂÖ×ÓÓëËøµÄ¸æËßÅö×²¡£0ÊÇ´ò¿ªÍâ²¿ÂÖ×Ó£¬1ÊÇ¹Ø±ÕÍâ²¿ÂÖ×Ó 
sbit motor_lock = P0^5;

// P0.2, PIN25, µç´ÅÌú×´Ì¬¼ÇÒäÊäÈë
sbit magnet_status = P0^2;

// P0.1, PIN26, ¶ÏÏß±¨¾¯¼ì²â¹Ü½Å£¬³£Ì¬Îª1£¬µ±Îª0Ê±£¬Ôò±íÊ¾ÓĞÈË¼ôÏß¡£
sbit wire_broken = P0^1;
sbit lock_code = P0^1;

// P1.2, PIN12, µç´ÅÌú¹ıÁ÷¼ì²â
sbit magnet_overcurrent = P1^2;

// P0.2, PIN25, ³µÂÖ×ª¶¯¼ì²â
sbit wheeled_rotate = P0^2;

// P2.4, PIN15, ÓÃÓÚËøÌå³ÌĞò£¬¿ØÖÆËøµÄ¿ª¹Ø
sbit Lock_EN = P2^4;

// P2.4, PIN15, ÓÃÓÚF3£¬¿ØÖÆÕñ¶¯Âí´ï
sbit Moto_EN = P2^4;

// P3.0, PIN9, ½ÓÊÕ¿ªËøÃÜÂë
sbit receive_wire = P3^0;

// P1.0, PIN18, ·¢ÉäĞÅºÅÊä³öÏß
sbit transmit_wire = P1^0;

// P0.6, PIN20, µçÁ¿ÊäÈë
sbit battery_input = P0^6;

/*----------------------------------------------------
	¸½»ú¶Ë¿Ú¶¨Òå
-----------------------------------------------------*/
// P2.6, PIN27, ²¦¶¯¿ª¹ØÊäÈë
sbit toggle_button = P2^6;

// P0.1, PIN26, ¹ØËø°´Å¥
sbit press_close_button = P0^1;

// P0.0, PIN3, ¿ªËø°´Å¥
sbit press_open_button = P0^0;

// P0.2, PIN25, ÎŞÏßÄ£¿éµçÔ´¿ØÖÆ
sbit transceiver_power_enable = P0^2;

#endif
											
/*---------------------------------------------------
	end of file
----------------------------------------------------*/