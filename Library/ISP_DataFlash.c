#include "ISP_DataFlash.h"
#include "main.h"

extern tByte received_data_buffer[7];		


//-----------------------------------------------------------------------------------------------------------
void Enable_ISP(void)
{   
	/* Enable ISP  */ 
	ISPCN=0x30;
	EA=0;
	TA = 0xAA;
	TA = 0x55;
	CHPCON |= 0x01; 
	EA=1;
	Trigger_ISP();     
}
//-----------------------------------------------------------------------------------------------------------
void Disable_ISP(void)
{   
	/* Disable ISP */ 
	ISPCN=0x30;
	Trigger_ISP(); 
	EA=0;
	TA = 0xAA;
	TA = 0x55;
	CHPCON &= 0xFE; 
	EA=1;     
}
//-----------------------------------------------------------------------------------------------------------
void Trigger_ISP(void)
{       
	EA=0;
	TA = 0xAA;
	TA = 0x55;
	ISPTRG |= 0x01;
	EA=1;      
}

void Flash_Page_Erase(unsigned int address)
{
	Enable_ISP();
	ISPCN=0x22;
	ISPAH=address/256;
	ISPAL=address%256;
	Trigger_ISP();
	Disable_ISP();	
}

void Flash_Write_Data(unsigned int address,unsigned char dat)
{
	Enable_ISP();
	ISPCN=0x21;
	ISPAH=address/256;
	ISPAL=address%256;
	ISPFD=dat;
	Trigger_ISP();
	Disable_ISP();	
}

unsigned char Flash_Read_Data(unsigned int address)
{
	unsigned char dat;
	
	Enable_ISP();
	ISPCN=0x00;
	ISPAH=address/256;
	ISPAL=address%256;
	Trigger_ISP();
	dat=ISPFD;
	Disable_ISP();
	return dat;		
}

/*---------------------------------------------------------------
	Self_learn_programming(void)
	自学习功能，将密码烧录到flash ROM中
----------------------------------------------------------------*/

void Self_learn_programming(void)
	{
	Flash_Page_Erase(0x3000);
	Flash_Write_Data(0x3000, received_data_buffer[0]);		
	Flash_Write_Data(0x3001, received_data_buffer[1]);		
	Flash_Write_Data(0x3002, received_data_buffer[2]);		
	Flash_Write_Data(0x3003, received_data_buffer[3]);		
	Flash_Write_Data(0x3004, received_data_buffer[4]);
	Flash_Write_Data(0x3005, received_data_buffer[5]);
	}
	
/*---------------------------------------------------
	end of file
----------------------------------------------------*/