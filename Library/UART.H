/*
函数名： void InitUART()
功能： 	初始化UART
说明：
入口参数：无
返回值：  无
*/
void InitUART();




/*
函数名： void SendData(BYTE dat)
功能： 	发送函数
说明：
入口参数：要发送的字节
返回值：  无
*/
void SendData(tByte dat);




/*
函数名：void SendNByte(BYTE *buf,BYTE len)
功能： 发送多个字节
说明：
入口参数：要发送的数据地址，发送的字节长度
返回值： 无
*/
void SendNByte(tByte *buf, tByte len);














