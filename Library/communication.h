/*-----------------------------------------------------
	communication.h (v1.00)
	
	header file for communication.c
-----------------------------------------------------*/

#ifndef _COMMUNICATION_H
#define _COMMUNICATION_H

//-----public constants--------------------------------
// 定义IDcard的密码
#define IDkey0 0x00
#define IDkey1 0x00
#define IDkey2 0x00
#define IDkey3 0x13
#define IDkey4 0xEC
/*
#define IDkey6 0x00
#define IDkey7 0x00
#define IDkey8 0x00
#define IDkey9 0x13
#define IDkey10 0x22
*/
#define CmdHead 0xc8
#define MyAddress 0xe0

#define ComMode_1 0xc1	// the verification/confirmation signal. once slave receive this signal, slave transmit a confirmation signal.
						// once host receive this signal, it knows slave is here.
#define ComMode_2 0xc2 	// battery stolen signal out
#define ComMode_3 0xc3 	// host stolen alarm signal 
#define ComMode_4 0xc4 	// raised alarm signal
#define ComMode_5 0xc5	// fell alarm signal
#define ComMode_6 0xc6	// 断线报警
#define ComMode_7 0xc7	// 发送开锁信号
#define ComMode_8 0xe8  // 发送关锁信号
#define ComMode_9 0xc9	// 发送自动模式的开锁信号
#define ComMode_10 0xc10	

//---------public function prototype

void initsignal(void);
void Send_Data(tByte x);
void ComMode_Data(tByte ComMode, x);
void ComMode_Data_WX(tByte ComMode, x);
void receive_byte(void);
void receive_word(void);
void send_code_to_lock(tByte x, y);	
void send_code_to_lock_Taili(tByte x, y);	
void UART_Send_Data(void);


#endif
											
/*---------------------------------------------------
	end of file
----------------------------------------------------*/