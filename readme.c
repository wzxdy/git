#include<reg51.h>
#include <intrins.h>

#define uchar unsigned char
#define uint unsigned int

sbit pcf8563_scl=P0^4;
sbit pcf8563_sda=P0^3;


uchar num[10]={0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39}; //数字表

sbit clk=P2^0;
sbit dio=P2^1;
sbit cfd=P2^2;  //充放电开关，
sbit en=P2^3;
sbit rw=P2^4;
sbit rs=P2^5;
sbit deng=P2^6;  //测试灯
sbit adcs=P2^7;

bit busy=0;

uchar sg;
uchar sd;

uchar fg;
uchar fd;

uchar mg;
uchar md;

uchar hou=0;
uchar min=0;
uchar sec=0;

uchar subadd;
uchar dat;

uchar number;

void start_pcf8563();
void send_pcf8563_byte();

void LcdIni(void);
void WrOp(uchar dat);
void WrDat(uchar dat);
void ChkBusy(void);

void display0(void);
void display1(void);

void stop_pcf8563();
void receive_pcf8563_byte();
void spit_time();



void LcdIni()//初始化LCD
{WrOp(0x01);
 WrOp(0x38);   
 WrOp(0x0c);   
 WrOp(0x06);   
}


void WrOp(uchar dat)//写LCD显示地址
{

 P1 =dat;
 rs=0;
 rw=0;
 en=0;
 ChkBusy();
 en=1; 
}


void WrDat(uchar dat)//写LCD显示数据
{ P1=dat;
  rs=1; 
  rw=0; 
  en=0; 
  ChkBusy();
  en=1; 
}


void ChkBusy() //检查LCD是否忙
 {
 P1=0xff;
 rs=0; 
 rw=1; 
 en=0; 
 _nop_();
 en=1; 
 while(P1&0x80);  
}


void display0() //显示必要的背景
{
 WrOp(0x80);
 WrDat('T');
 
 WrOp(0x81);
 WrDat(':');
 
 WrOp(0x84);
 WrDat(':');
 
 WrOp(0x87);
 WrDat(':');
}

void spit_time()
{
 sg=(int)hou/10;
 sd=(int)hou%10;

 fg=(int)min/10;
 fd=(int)min%10;

 mg=(int)sec/10;
 md=(int)sec%10;
}

void display1()//显示时间六位
{
 uchar temp6;
 uchar temp5;
 uchar temp4;
 uchar temp3;
 uchar temp2;
 uchar temp1;
 
 WrOp(0x82);
 temp6=num[sg];
 WrDat(temp6);  
 
 WrOp(0x83);
 temp5=num[sd];
 WrDat(temp5);  
 
 WrOp(0x85);
 temp4=num[fg];
 WrDat(temp4);  
 
 WrOp(0x86);
 temp3=num[fd];
 WrDat(temp3);  
 
 WrOp(0x88);
 temp2=num[mg];
 WrDat(temp2);  
 
 WrOp(0x89);
 temp1=num[md];
 WrDat(temp1); 
}

void Send_pcf8563_byte(uchar bb) //向PCF8563发送一个字节
{
  uchar aa;
    pcf8563_scl=0;
 for(aa=0;aa<8;aa++)
 {  
  if((bb&0x80)==0x80)
   {
    pcf8563_sda=1;
   }
   else
   {
    pcf8563_sda=0;
   }

       pcf8563_scl=1;

      pcf8563_scl=0;
     bb=bb<<1;
 }
    _nop_();
    _nop_();
 pcf8563_sda=1;

 pcf8563_scl=1;

    busy=0;
 if(pcf8563_sda)
 {
 busy=1;
 }
    else
 {
    _nop_();
    _nop_();
 pcf8563_scl=0;
    busy=0;
 }
}

void write_pcf8563(uchar subadd,uchar dat)// 向PCF8563对应地址写数据
{
 start_pcf8563();
 Send_pcf8563_byte(0xa2);
 if(!busy)
 {
   Send_pcf8563_byte(subadd);
   if(!busy)
 {
   Send_pcf8563_byte(dat);  

 }
  }
  stop_pcf8563();
}

void read_pcf8563()          //读当时的时，分，钞
{
 start_pcf8563();
 Send_pcf8563_byte(0xa2);

if(!busy)
{
  Send_pcf8563_byte(0x02);
  if(!busy)
  { 
  start_pcf8563();
  Send_pcf8563_byte(0xa3);
  receive_pcf8563_byte();
  sec=number&0x7f;


  start_pcf8563();
  Send_pcf8563_byte(0xa3);
  receive_pcf8563_byte();
  min=number&0x7f;


  start_pcf8563();
  Send_pcf8563_byte(0xa3);
  receive_pcf8563_byte();
  hou=number&0x3f;

 }
}
stop_pcf8563();
}

void receive_pcf8563_byte()   //从PCF8563接受一个字节
{uchar cc;
pcf8563_sda=1;
 number=0;
 for(cc=0;cc<8;cc++)
  {
  number<<=1;
  pcf8563_scl=0;

  pcf8563_scl=1;
  _nop_();
  _nop_();
  number= number|pcf8563_sda;
 }
  pcf8563_scl=0;
 _nop_();
 _nop_();

}



void start_pcf8563() //开启PCF8563IIC
{
pcf8563_sda=1;
pcf8563_scl=1;
pcf8563_sda=0;//SCL为低，SDA执行一个上跳

pcf8563_scl=0;//SCL为低，嵌住数据线
}

void stop_pcf8563()   //关闭PCF8563IIC
{
pcf8563_sda=0;
pcf8563_scl=1;
pcf8563_sda=1;//SCL为高，SDA执行一个上跳

pcf8563_scl=0;//SCL为低，嵌住数据线
}


void main(void)
{
LcdIni();    //初始化屏
display0();  //显示必要的背影

write_pcf8563(0x02,sec);   //写钞
write_pcf8563(0x03,min);   //写分
write_pcf8563(0x04,hou);   //写时


while(1)
{
    read_pcf8563();//读当前时间
 spit_time();   //切害时间，为显示做准备
 display1(); //显示当前时间
}
} 