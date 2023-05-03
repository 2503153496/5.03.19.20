#include <reg51.H> 
#include "intrins.h"
#include "ds1302.h"	 //���DS1302ͷ�ļ�
#define uint unsigned int
#define uchar unsigned char
#define ulong unsigned long
#define     LCDIO      P0	 //Һ�������ݿ�
//ADC0832������
bit flag_300ms ;
sbit ADCLK =P1^1;  //ADC0832 clock signal
sbit ADDIO =P1^3;  //ADC0832 k in
sbit ADCS =P1^4;   //ADC0832 chip seclect

sbit rs=P1^0;    //����1602 RS
sbit lcden=P1^2; //����1602 EN
sbit key1=P3^0;  //�趨
sbit key2=P3^1;  //��
sbit key3=P3^2;  //��
sbit motor=P3^7; //�̵����ӿ�
sbit speak=P1^5; //�������ӿ�
uchar key;       //�趨ָ��
uint RH=400,RL=200;//ˮλ������
float temp_f;
ulong temp;
uchar v;
uchar count,s1num;
uchar code table[]= "          :          ";
uchar code table1[]="RH:  %              ";
uchar getdata; //��ȡADCת��������ֵ

/*************��ʱ��0��ʼ������***************/
void time_init()	  
{
	EA   = 1;	 	  //�����ж�
	TMOD = 0X01;	//��ʱ��0����ʱ��1������ʽ1
	ET0  = 1;		  //����ʱ��0�ж� 
	TR0  = 1;		  //����ʱ��0��ʱ
}
/*********************************************/
void delay(uint z)		  //��ʱ
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
��������:ADת���ӳ���
****************************************************************************/
uchar Adc0832()     //ADת�������ؽ��
{
    uchar i;
    uchar dat=0;

    ADCLK=0;
    ADDIO=1;
    ADCS=0;                  //����CS��
    ADCLK=1;                 
    ADCLK=0;                 //����CLK��,�γ��½���1

    ADDIO=1;//ָ��ת��ͨ����CH1����CH2��ָ��ֵλ��0x1��ȡ���һλ��ֵ
    ADCLK=1;    
    ADCLK=0;                 //����CLK��,�γ��½���2

    ADDIO=0;//ָ��ֵ����һλ����ȡ���һλ��ֵ
    ADCLK=1;
    ADCLK=0;                //����CLK��,�γ��½���3

    ADDIO=1;                
	  for(i=0;i<8;i++)
    {
        ADCLK=1;
        ADCLK=0;           //�γ�һ��ʱ������
        if(ADDIO) 
		    dat|= 0x80>>i;  //������
    }
    ADCS=1;                //����CS��
    ADCLK=1;
    ADDIO=1;               //�������ݶ�,�ص���ʼ״̬
    return(dat);           //return dat
}
/***************************************************************************/
/********************************************************/
void displayRH()			//������ʾ
 {
	 write_com(0xc0+3);
	 write_date(RH/100%10+0x30);//���ް�λ
   write_date(RH/10%10+0x30);//����ʮλ
   //write_date('.');
   //write_date(RH%10+0x30);
 }
void displayRL()	  //������ʾ
 {
	 write_com(0xca);
   write_date('R');
   write_date('L');
   write_date(':');
   write_date(RL/100%10+0x30);//���ް�λ
   write_date(RL/10%10+0x30);//����ʮλ
   write_date('%');
}
void displaytime()	 //ʱ����ʾ 
 {
	  uchar years=0x23,month=0x03,day=0x18;//ʱ�䡢���ڱ���
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
/*******************��������***********************/
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
  {case 1:{write_com(0x0f);write_com(0xce); //�����˸
	 while(key1!=0)	//�ȴ������ɿ�
	{if(key2==0)		//key2������
   {delay(30);		//������ʱ����
    if(key2==0)		//ȷ��key2����
    {while(key2==0); //�ȴ��ɿ�
	   if(RL>=998)
     {RL=999;}		//RL�����������Ϊ99
	   else
	   {RL+=10;}		//RL��1
    }
    displayRL();		//����RL������ʾ����
    write_com(0xce);
   }   
   if(key3==0)		//key3����
   {delay(30);		//������ʱ����
    if(key3==0)		//ȷ��key3����
    {while(key3==0);	 //�ȴ�key3�����ɿ�
	   if(RL<=1)		     //RL��С����Ϊ1
	   {RL=0;}
	   else
	   {RL-=10;}		  //RL���޼�1
    }
    displayRL();		//����RL������ʾ����
    write_com(0xce);
   }
  }while(key1==0);	
}
	case 2:
	{write_com(0x0f);write_com(0xc4);  //RH�������ݣ������˸
   while(key1==1)
   {if(key2==0)	     //key2����
    {delay(30);	     //������ʱ����
     if(key2==0)	   //ȷ��key2����
     {while(key2==0);//�ȴ��ɿ�
	    if(RH>=998)		 //RH�������Ϊ99
	    {RH=999;}
	    else
	    {RH+=10;}		//RH��1
     }
     displayRH(); //RH������ʾ����
     write_com(0xc4);
    } 
   if(key3==0)	  //key3����
   {delay(30);	  //������ʱ����
    if(key3==0)	  //ȷ������
    {while(key3==0);//�ȴ��ɿ�
	   if(RH<=1)	    //RH��С����Ϊ1
	   {RH=0;}
	   else
	   {RH-=10;}		//RH��1
    }
    displayRH();		//����RH��ʾ����
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
/*****************�����������ת��********************/
void Conut(void)	
{	  
    v=Adc0832();
	  temp=v;
    temp_f=temp*9.90/2.55;
    temp=temp_f;
	  temp=1000-temp;	 
	  write_com(0x80+11);
	  write_date(temp/100%10+0x30);//ǧλ
	  write_date(temp/10%10+0x30);//��λ
	  write_date('.');
	  write_date(temp%10+0x30);
	  write_date('%');//��ʾ����λ
}
/********************************************************/
void main(void)
{
	lcdinit();
	init();
  displaytime();
	displayRH();   //��ʾ����
	displayRL();   //��ʾ����
	delay(200); 	 //�����ȴ�����LCD���빤��״̬
	Conut();	     //��ʾ����
	delay(150); 	
	while(1)
	{ 	
	   Conut();	//��ʾ��ǰʪ��
		 keyscan();
		 if(temp>RH)  //���ʪ�ȴ�������ֹͣ��ˮ
		 {motor=1;	  //�رռ̵���
		 }
		 else if(temp<RL)//���ʪ��С��RL����������ˮ
		 {motor=0;		   //�����̵���
		 }
		 if(temp<RL)   	//С������������������ˮ
		 {speak=0;		  //��������
		  delay(150); 	//��ʱ
		  speak=1;
		 }
		 keyscan();		 //�������
		 delay(150); 	 //��ʱ50MS 
	}	
}