/* 包含头文件 */
#include <ioCC2530.h>
#include <stdio.h>
#include <stdlib.h>

#include "hal_i2c.h"
#include "sht.h"
#include "sht1x.h"
#include "sht3x.h" 

/*宏定义*/
#define LED1 P1_0 
#define LED2 P1_1
#define SW1  P1_2

/*定义变量*/
uint8 counter = 0; //统计定时器溢出次数
uint8 sflag = 0;//串口标识位
//int16 tem,hum;//直播时用的： 温湿度数值，tem为温度，hum为湿度
int8 tem;//温度数值
uint8 hum;//湿度数值

//uint8 buff[10];
unsigned char buff[2];
/*声明函数*/
void Delay1Ms(uint8 time);//延时函数，32MHz系统时钟下，约1ms延时函数
void InitCLK(void);//系统时钟初始化函数，为32MHz
void InitTime1(void);//定时器1初始化函数，数据溢出周期为50ms
void InitUart0(void);//串口0初始化函数
void UART0SendByte(unsigned char c);//UART0发送一个字节函数
void UART0SendData(unsigned char *str,int len);//UART0发送指定数量字节数据

/*定义函数*/
void Delay1Ms(uint8 time)
{
  unsigned int i,j;
  for(i=0;i<=time;i++)
  {
    for(j=0;j<1100;j++);
  }
}

void InitLED()
{
  P1SEL &= ~0x07;//设置P1_0、P1_1、P1_2为GPIO口
  P1DIR |= 0x03;//设置P1_0和P1_1为输出
  
  /*.......答题区1开始：LED灯初始状态设置...........*/
  LED1 = 0;//设置LED1的初始状态
  LED2 = 0;//设置LED2的初始状态
  /*.......答题区1结束...........*/
}

void InitSW1()
{
  /*.......答题区4开始：按键SW1设置....................*/
  P1DIR &= ~0x04;//设置P1_2为输入
  P1INP &= ~0x04;//设置P1_2端口为“上拉/下拉”模式
  P2INP &= ~0x40;//设置所有P1端口为“上拉”
  IEN2 |= 0x10;//使能P1端口中断
  P1IEN |= 0x04;//使能P1_2端口中断
  PICTL |= 0x02;//设置P1_2端口中断触发方式为：下降沿触发
  /*.......答题区4结束.......................................*/
}

void InitCLK(void)
{
  CLKCONCMD &= 0x80;
  while(CLKCONSTA & 0x40);
}

void InitTime1(void)
{
  /*.......答题区2开始：定时器1设置....................*/
  T1CTL = 0X09;//32分频，自由运行模式 （1001）
  TIMIF|= 0X40;//使能定时器溢出中断
  IEN1 |= 0X02;//定时器1中断使能 或 T1IE=1
  /*.......答题区2结束.......................................*/
}

void InitUart0(void)
{	
  U0CSR |= 0XC0;//串口模式,且配置接收器使能
  /*.......答题区3开始：串口设置....................*/
   
  PERCFG|= 0x00;//USART0使用备用位置1 P0_2 P0_3
  P0SEL |= 0X0C;//设置P0_2 P0_3为外设
  U0UCR |= 0X80;//流控无 8位数据位 无奇偶校验 1位停止位

  U0GCR = 11; //设置波特率为115200 （见书上对应表）
  U0BAUD = 216;
  /*.......答题区3结束.......................................*/
  
  UTX0IF = 0;// 清零UART0 TX中断标志 
  URX0IF = 0;// 清零UART0 RX中断标志
  URX0IE = 1;// 使能UART0 RX中断
}

void UART0SendByte(unsigned char c)
{
  U0DBUF = c;// 将要发送的1字节数据写入U0DBUF
  while (!UTX0IF) ;// 等待TX中断标志，即U0DBUF就绪
  UTX0IF = 0;// 清零TX中断标志
}

void UART0SendData(unsigned char *str,int len)
{
  for(int i=0;i<len;i++)
  {
    U0DBUF = str[i];		// 将要发送的1字节数据写入U0DBUF
    while (!UTX0IF) ;  // 等待TX中断标志，即U0DBUF就绪
    UTX0IF = 0;       // 清零TX中断标志UART0SendByte(*str++);   // 发送一字节
  }
}


/*主函数*/
void main(void)
{
  InitLED();
  InitCLK();
  InitTime1();
  InitUart0();
  InitSW1();
  SHT_Init();
  EA = 1;//使能总中断
    
  while(1)
  {  
  }
}

/*中断服务函数*/

#pragma vector = P1INT_VECTOR
__interrupt void P1_ISR(void)
{
/*.......答题区7开始：按键中断服务函数...........*/

   if(P1IFG & 0x04)//判断按键是否按下
   {
      LED2 = ~LED2;//切换LED2的状态
      P1IFG &= ~0x04;//清除P1_2端口中断标志位
   }
   
   P1IF = 0;//清除P1端口中断标志位
   
/*.......答题区7结束...........*/
}


#pragma vector = T1_VECTOR
__interrupt void T1_ISR(void)
{
  counter++;
  T1STAT &= ~0x01;  //清除通道0中断标志
  
  /*.......答题区6开始：当串口标识位为1时，每隔2秒往串口回复“LED1亮”。...........*/
  if(counter == 30)//定时2s
  {
    if(sflag == 1)//要发送的是LED1亮
    {
      UART0SendData("LED1亮",sizeof("LED1亮"));
    }
    
    counter = 0;
  }
  /*.......答题区6结束...........*/
}

#pragma vector = URX0_VECTOR
__interrupt void UART0_ISR(void)
{
  /*.......答题区5开始：接收数据0xAA时点亮LED1、并设置串口标识位为1；
  接收数据0xBB时熄灭LED1，设置串口标识位为0，并往串口回复一次“LED1灭”。...........*/
  if(U0DBUF == 0xAA)
  {
    sflag = 1;
    LED1 = 1;
  }
  else if(U0DBUF == 0xBB)
  {
    sflag = 0;
    LED1 = 0;
    UART0SendData("LED1灭",sizeof("LED1灭"));
  }
  /*.......答题区5结束...........*/  
  URX0IF = 0;// 清零UART0 RX中断标志
}
