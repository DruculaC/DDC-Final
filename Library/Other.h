/*-----------------------------------------------------
	Other.h (v1.00)
	
	header file for Other.c
-----------------------------------------------------*/

#ifndef _OTHER_H
#define _OTHER_H

//-----public constants-------------------------------

//-----public function prototype----------------------

void magnet_CW(tWord x, y, tByte z);
void magnet_CW_Taili(tWord x, y, tByte z);
void magnet_ACW(tWord x, y);
void magnet_ACW_Taili(tWord x, y);
void disable_sensor(void);
void enable_sensor(void);

void Moto_Vibration(void);

#endif
											
/*---------------------------------------------------
	end of file
----------------------------------------------------*/