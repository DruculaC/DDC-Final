#ifndef __ISP_DATAFLASH_H__
#define __ISP_DATAFLASH_H__


void ISP_Enable(void);
void ISP_Disable(void);
void ISP_Trigger(void);
void Flash_Page_Erase(unsigned int address);
void Flash_Write_Data(unsigned int address,unsigned char dat);
unsigned char Flash_Read_Data(unsigned int address);
void Trigger_ISP(void);
void Self_learn_programming(void);

#endif
