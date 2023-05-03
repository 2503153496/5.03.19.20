#ifndef __LCD1602_H__
#define __LCD1602_H__
#include"delay.h"  		  //添加延时函数头文件
#define LCD_PINDATA P0	  //数据端口定义 
sbit RS  =  P1^0;		  //RS 
sbit E   =  P1^2;		  //E  
void WriteCOMDATA(uchar LCD_DATA,uchar N)
{
	Delay(5);E=1;RS=N;
	LCD_PINDATA=LCD_DATA;
	E=0;
}
void LCD_Init(void)
{
  WriteCOMDATA(0x01,0);
	Delay(500);
	WriteCOMDATA(0x38,0);
	Delay(10);
	WriteCOMDATA(0x06,0);
	Delay(10);
	WriteCOMDATA(0x0C,0);
	Delay(10);
}
void WriteChar(uchar Row,uchar Col,uchar Num,uchar *pBuffer)
{
	uchar i;
	if(Row==1)Row=0x80+Col;
	else Row=0xC0+Col;
	WriteCOMDATA(Row,0);
	for(i=Num;i!=0;i--)
	{WriteCOMDATA(*pBuffer,1);	
   pBuffer++;}
}
#endif