//backup

if(detect_stolen_flag==0)
		{
			if(ReceWave==1)
			{
				time0Count_2++;
				if(time0Count_2==10)
				{
					time0Count_2=20;
					SensorCount=1;
					alarmFlag2=1;
					detect_stolen_flag=1;
					detect_stolen_count=0;
				}
			}
		}
		else if(detect_stolen_flag==1)
		{
			detect_stolen_count++;
			if((detect_stolen_count>=20)&&(detect_stolen_count<40))
			{
				if(ReceWave==1)
				{
					time0Count_6++;
					if(time0Count_6==10)
					{
						SensorCount=2;
						alarmFlag2=1;
					}
				}
			}
			else if((detect_stolen_count>=240)&&(detect_stolen_count<360))
			{
				if(ReceWave==1)
				{
					time0Count_7++;
					if(time0Count_7==10)
					{
						SensorCount=3;
						alarmFlag2=1;
					}
				}	
			}
			else
			{
				time0Count_2=0;
				time0Count_6=0;
				time0Count_7=0;
				SensorCount=0;
				alarmFlag2=0;
				detect_stolen_count=0;
				detect_stolen_flag=0;
				speech1_count=0;
				speech2_count=0;
			}
		}




if(ModeFlag==2&&SensorCount>=1)//三轴传感器脉冲的相应报警
	{
		if(SensorCount==1&&alarmFlag2==1)//三轴传感器一次触发,alarmFlag2控制发声1次
		{					
			if((upSignal!=0)&&(downSignal!=0))
			{
//				sendspeech1=1;
				alarmFlag2=0;

				PAshutdown=1;
				SC_Speech(17);  //关机语言提醒
				Delay(80);
				PAshutdown=0;
			}
		}

		if((SensorCount==2)&&(alarmFlag2==1))
		{
			if((upSignal!=0)&&(downSignal!=0))
			{
				sendspeech2=1;
				alarmFlag2=0;

				PAshutdown=1;
				SC_Speech(18);  //关机语言提醒
				Delay(80);
				SC_Speech(16);  //关机语言提醒
				Delay(80);
				PAshutdown=0;
			}			
		}

		if(SensorCount>=3)
		{
			sendspeech3=1;
			alarmFlag2=0;
			stolenflag=1;
			alarmFlag=1;//置语音报警位
			alarmFlag2=0;
			downFlag=0;
			upFlag=0;
			SensorCount=0; //脉冲计数清零
		}
/*
		if(SensorCount>=4)//三轴传感器一次触发
		{
			ModeFlag=3;//三轴传感器已经有2次触发了，要改变发射模式了
			sendspeech4=1;
			stolenflag=1;

			alarmFlag=1;//置语音报警位
			alarmFlag2=0;
			downFlag=0;
			upFlag=0;
			SensorCount=0; //脉冲计数清零
		}
*/
	}




/*
	if(ModeFlag==3)
	{



		alarmCount++;
		if(alarmCount>=2000) //调节语音的段数
		{
			alarmCount=0;//清报警计数器
			alarmFlag=0;//清报警标志
		}
	}
*/