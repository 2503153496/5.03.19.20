#ifndef __DS1302_H__
#define __DS1302_H__
#define uchar unsigned char
sbit RST=P2^5;
sbit IO=P2^6;
sbit CLK=P2^7;
sbit ACC0=ACC^0;
sbit ACC7=ACC^7;
//*******启动函数********
void rst1(void)
{
	RST=0;
	CLK=0;
	RST=1;
}
//*******停止函数********
void rst0(void)
{
	CLK=1;
	RST=0;
}
//*******时钟脉冲函数********
void dsclk(void)
{
	CLK=1;
	CLK=0;
}
//*******写入一字节函数********
void wrbyte(uchar d)
{
	uchar i;
	ACC=d;
	for(i=8;i>0;i--)
	{
		IO=ACC0;
		dsclk();
		ACC=ACC>>1;
	}
}
//*******读出一字节函数********
uchar rebyte(void)
{
	uchar i;
	for(i=8;i>0;i--)
	{
		ACC=ACC>>1;
		ACC7=IO; 
		dsclk();
	}
	return(ACC);
}
//*******单字节写入数据子函数********
void setds1302(uchar dizhi,uchar shuju)
{
	rst1();
	wrbyte(0x8e);
	wrbyte(0x00);
	rst0();
	rst1();
	wrbyte(dizhi);
	wrbyte(shuju);
	rst0();
	rst1();
	wrbyte(0x8e);
	wrbyte(0x80);
	rst0();
}
//*******单字节读取子函数********
uchar getds1302(uchar dizhi)
{
	uchar shuju;
	rst1();
	wrbyte(dizhi);
	shuju=rebyte();
	rst0();
	return(shuju);
}
#endif