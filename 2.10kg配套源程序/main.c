/**********************************************************************
----------------1.开发环境:Keil v4					   ----------------
----------------2.使用单片机型号：STC89C52RC		   ----------------
----------------3.单片机使用晶振：11.0592M			   ----------------
***********************************************************************/
#include "main.h"
#include "HX711.h"
#include "uart.h"
#include "LCD1602.h"
unsigned long HX711_Buffer = 0;
unsigned long Weight_Maopi = 0;
long Weight_Shiwu = 0;
unsigned char flag = 0;
bit Flag_ERROR = 0;

//校准参数
//因为不同的传感器特性曲线不是很一致，因此，每一个传感器需要矫正这里这个参数才能使测量值很准确。
//当发现测试出来的重量偏大时，增加该数值。
//如果测试出来的重量偏小时，减小改数值。
//该值可以为小数
#define GapValue 210//不同量程的传感器此参数值大小不同，参数值的推导参考本店提供的原理教程说明部分
//****************************************************
//主函数
//****************************************************
void main()
{
    Uart_Init();
    Send_Word("Welcome to use!\n");	 //串口初始化输出内容
	Init_LCD1602();
	LCD1602_write_com(0x80);
	LCD1602_write_word("Welcome to use!");	//液晶初始化界面显示
	Delay_ms(3000);		 //延时,等待传感器稳定
	Get_Maopi();				//称毛皮重量
	Delay_ms(3000);		 //延时,等待传感器稳定
	Get_Maopi();
	Get_Maopi();
	while(1)
	{
		EA = 0;
		Get_Weight();			//称重
		EA = 1;
	//显示当前重量
		if( Flag_ERROR == 1)	//超重或者负重情况下输出error
		{
            Send_Word("ERROR\n");
			LCD1602_write_com(0x80+0x40);
			LCD1602_write_word("ERROR ");
		}		
		else	//以下是重量显示各个位对应值进行换算
		{	//串口输出
            Send_ASCII(Weight_Shiwu/1000 + 0X30);
            Send_ASCII(Weight_Shiwu%1000/100 + 0X30);
            Send_ASCII(Weight_Shiwu%100/10 + 0X30);
            Send_ASCII(Weight_Shiwu%10 + 0X30);
            Send_Word(" g\n");
			//LCD1602液晶显示
			LCD1602_write_com(0x80+0x40);
			LCD1602_write_data(Weight_Shiwu/1000 + 0X30);
            LCD1602_write_data(Weight_Shiwu%1000/100 + 0X30);
            LCD1602_write_data(Weight_Shiwu%100/10 + 0X30);
            LCD1602_write_data(Weight_Shiwu%10 + 0X30);
			LCD1602_write_word(" g");
		}
		
	}
}
//****************************************************
//称重
//****************************************************
void Get_Weight()
{
	Weight_Shiwu = HX711_Read();
	Weight_Shiwu = Weight_Shiwu - Weight_Maopi;		//获取净重
	if(Weight_Shiwu > 0)			
	{	
		Weight_Shiwu = (unsigned int)((float)Weight_Shiwu/GapValue); 	//计算实物的实际重量																																
		if(Weight_Shiwu > 3000)		//超重报警
		{	
			Flag_ERROR = 1;	
		}
		else
		{  
			Flag_ERROR = 0;
		}
	}
	else
	{
		Weight_Shiwu = 0;			
	}	
}

//****************************************************
//获取毛皮重量
//****************************************************
void Get_Maopi()
{
	Weight_Maopi = HX711_Read();	
} 

//****************************************************
//MS延时函数(12M晶振下测试)
//****************************************************
void Delay_ms(unsigned int n)
{
	unsigned int  i,j;
	for(i=0;i<n;i++)
		for(j=0;j<123;j++);
}