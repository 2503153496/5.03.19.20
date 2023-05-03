#include <reg51.H> 
#include "intrins.h"
#include "ds1302.h"	 //添加DS1302头文件
#define uint unsigned int
#define uchar unsigned char
#define ulong unsigned long
#define     LCDIO      P0	 //液晶屏数据口
//ADC0832的引脚
bit flag_300ms ;
sbit ADCLK =P1^1;  //ADC0832 clock signal
sbit ADDIO =P1^3;  //ADC0832 k in
sbit ADCS =P1^4;   //ADC0832 chip seclect

sbit rs=P1^0;    //定义1602 RS
sbit lcden=P1^2; //定义1602 EN
sbit key1=P3^0;  //设定
sbit key2=P3^1;  //加
sbit key3=P3^2;  //减
sbit motor=P3^7; //继电器接口
sbit speak=P1^5; //蜂鸣器接口
uchar key;       //设定指针
uint RH=400,RL=200;//水位上下限
float temp_f;
ulong temp;
uchar v;
uchar count,s1num;
uchar code table[]= "          :          ";
uchar code table1[]="RH:  %              ";
uchar getdata; //获取ADC转换回来的值

/*************定时器0初始化程序***************/
void time_init()	  
{
	EA   = 1;	 	  //开总中断
	TMOD = 0X01;	//定时器0、定时器1工作方式1
	ET0  = 1;		  //开定时器0中断 
	TR0  = 1;		  //允许定时器0定时
}
/*********************************************/
void delay(uint z)		  //延时
{
	uint x,y;
	for(x=z;x>0;x--)
		for(y=110;y>0;y--);
}
/**********************************************/
void write_com(uchar com)
{
	rs=0;
//rd=0;
	lcden=0;
	P0=com;
	delay(5);
	lcden=1;
	delay(5);
	lcden=0;	
}
/*********************************************/
void write_date(uchar date)
{
	rs=1;
//rd=0;
	lcden=0;
	P0=date;
	delay(5);
	lcden=1;
	delay(5);
	lcden=0;	
}
void lcdinit()
{
	lcden=0;
	write_com(0x38);
	write_com(0x0c);
	write_com(0x06);
	write_com(0x01);
}
/***********************************************/
void init()
{
	uchar num;
	
 	for(num=0;num<15;num++)
 		{
 			write_date(table[num]);
 			delay(5);
 		}
 	write_com(0x80+0x40);
 	for(num=0;num<15;num++)
		{
 			write_date(table1[num]);
 			delay(5);
 		}
	}
/****************************************************************************
函数功能:AD转换子程序
****************************************************************************/
uchar Adc0832()     //AD转换，返回结果
{
    uchar i;
    uchar dat=0;

    ADCLK=0;
    ADDIO=1;
    ADCS=0;                  //拉低CS端
    ADCLK=1;                 
    ADCLK=0;                 //拉低CLK端,形成下降沿1

    ADDIO=1;//指定转换通道是CH1还是CH2，指定值位与0x1，取最后一位的值
    ADCLK=1;    
    ADCLK=0;                 //拉低CLK端,形成下降沿2

    ADDIO=0;//指定值右移一位，再取最后一位的值
    ADCLK=1;
    ADCLK=0;                //拉低CLK端,形成下降沿3

    ADDIO=1;                
	  for(i=0;i<8;i++)
    {
        ADCLK=1;
        ADCLK=0;           //形成一次时钟脉冲
        if(ADDIO) 
		    dat|= 0x80>>i;  //收数据
    }
    ADCS=1;                //拉低CS端
    ADCLK=1;
    ADDIO=1;               //拉高数据端,回到初始状态
    return(dat);           //return dat
}
/***************************************************************************/
/********************************************************/
void displayRH()			//上限显示
 {
	 write_com(0xc0+3);
	 write_date(RH/100%10+0x30);//上限百位
   write_date(RH/10%10+0x30);//上限十位
   //write_date('.');
   //write_date(RH%10+0x30);
 }
void displayRL()	  //下限显示
 {
	 write_com(0xca);
   write_date('R');
   write_date('L');
   write_date(':');
   write_date(RL/100%10+0x30);//下限百位
   write_date(RL/10%10+0x30);//下限十位
   write_date('%');
}
void displaytime()	 //时间显示 
 {
	  uchar years=0x23,month=0x03,day=0x18;//时间、日期变量
    years=getds1302(0x8D);
		month=getds1302(0x89);
		day=getds1302(0x87);
		write_com(0x80);
		write_date(years/16+0x30);
		write_date(years%16+0x30);
		write_date('-');
		write_date(month/16+0x30);
		write_date(month%16+0x30);
	  write_date('-');
		write_date(day/16+0x30);
		write_date(day%16+0x30);
}
/**************************************************/
/*******************按键处理***********************/
void keyscan()		 
{bit kk1=0,kk2=0;
 if(key1==0)
 {delay(30);
  while(key1==0);
  if(key>=2)
  {key=0;}
  else
  {key++;}
  switch(key)
  {case 1:{write_com(0x0f);write_com(0xce); //光标闪烁
	 while(key1!=0)	//等待按键松开
	{if(key2==0)		//key2按键下
   {delay(30);		//按键延时消抖
    if(key2==0)		//确定key2按下
    {while(key2==0); //等待松开
	   if(RL>=998)
     {RL=999;}		//RL下限最大设置为99
	   else
	   {RL+=10;}		//RL加1
    }
    displayRL();		//调用RL下限显示函数
    write_com(0xce);
   }   
   if(key3==0)		//key3按下
   {delay(30);		//按键延时消抖
    if(key3==0)		//确定key3按下
    {while(key3==0);	 //等待key3按键松开
	   if(RL<=1)		     //RL最小设置为1
	   {RL=0;}
	   else
	   {RL-=10;}		  //RL下限减1
    }
    displayRL();		//调用RL下限显示函数
    write_com(0xce);
   }
  }while(key1==0);	
}
	case 2:
	{write_com(0x0f);write_com(0xc4);  //RH设置数据，光标闪烁
   while(key1==1)
   {if(key2==0)	     //key2按下
    {delay(30);	     //按键延时消抖
     if(key2==0)	   //确定key2按下
     {while(key2==0);//等待松开
	    if(RH>=998)		 //RH最大设置为99
	    {RH=999;}
	    else
	    {RH+=10;}		//RH加1
     }
     displayRH(); //RH上限显示函数
     write_com(0xc4);
    } 
   if(key3==0)	  //key3按下
   {delay(30);	  //按键延时消抖
    if(key3==0)	  //确定按下
    {while(key3==0);//等待松开
	   if(RH<=1)	    //RH最小设置为1
	   {RH=0;}
	   else
	   {RH-=10;}		//RH减1
    }
    displayRH();		//调用RH显示函数
    write_com(0xc4);
   }
  }
  while(key1==0);
 }
	case 0:
	{write_com(0x0c);
	 motor=kk2;
   break;}
  }
 }
}
/*****************土壤检测数据转换********************/
void Conut(void)	
{	  
    v=Adc0832();
	  temp=v;
    temp_f=temp*9.90/2.55;
    temp=temp_f;
	  temp=1000-temp;	 
	  write_com(0x80+11);
	  write_date(temp/100%10+0x30);//千位
	  write_date(temp/10%10+0x30);//百位
	  write_date('.');
	  write_date(temp%10+0x30);
	  write_date('%');//显示符号位
}
/********************************************************/
void main(void)
{
	lcdinit();
	init();
  displaytime();
	displayRH();   //显示上限
	displayRL();   //显示下限
	delay(200); 	 //启动等待，等LCD讲入工作状态
	Conut();	     //显示函数
	delay(150); 	
	while(1)
	{ 	
	   Conut();	//显示当前湿度
		 keyscan();
		 if(temp>RH)  //如果湿度大于上限停止浇水
		 {motor=1;	  //关闭继电器
		 }
		 else if(temp<RL)//如果湿度小于RL下限启动浇水
		 {motor=0;		   //启动继电器
		 }
		 if(temp<RL)   	//小于下限启动报警并浇水
		 {speak=0;		  //启动报警
		  delay(150); 	//延时
		  speak=1;
		 }
		 keyscan();		 //按键检测
		 delay(150); 	 //延时50MS 
	}	
}