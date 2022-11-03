/* ����ͷ�ļ� */
#include <ioCC2530.h>
#include <stdio.h>
#include <stdlib.h>

#include "hal_i2c.h"
#include "sht.h"
#include "sht1x.h"
#include "sht3x.h" 

/*�궨��*/
#define LED1 P1_0 
#define LED2 P1_1
#define SW1  P1_2

/*�������*/
uint8 counter = 0; //ͳ�ƶ�ʱ���������
uint8 sflag = 0;//���ڱ�ʶλ
//int16 tem,hum;//ֱ��ʱ�õģ� ��ʪ����ֵ��temΪ�¶ȣ�humΪʪ��
int8 tem;//�¶���ֵ
uint8 hum;//ʪ����ֵ

//uint8 buff[10];
unsigned char buff[2];
/*��������*/
void Delay1Ms(uint8 time);//��ʱ������32MHzϵͳʱ���£�Լ1ms��ʱ����
void InitCLK(void);//ϵͳʱ�ӳ�ʼ��������Ϊ32MHz
void InitTime1(void);//��ʱ��1��ʼ�������������������Ϊ50ms
void InitUart0(void);//����0��ʼ������
void UART0SendByte(unsigned char c);//UART0����һ���ֽں���
void UART0SendData(unsigned char *str,int len);//UART0����ָ�������ֽ�����

/*���庯��*/
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
  P1SEL &= ~0x07;//����P1_0��P1_1��P1_2ΪGPIO��
  P1DIR |= 0x03;//����P1_0��P1_1Ϊ���
  
  /*.......������1��ʼ��LED�Ƴ�ʼ״̬����...........*/
  LED1 = 0;//����LED1�ĳ�ʼ״̬
  LED2 = 0;//����LED2�ĳ�ʼ״̬
  /*.......������1����...........*/
}

void InitSW1()
{
  /*.......������4��ʼ������SW1����....................*/
  P1DIR &= ~0x04;//����P1_2Ϊ����
  P1INP &= ~0x04;//����P1_2�˿�Ϊ������/������ģʽ
  P2INP &= ~0x40;//��������P1�˿�Ϊ��������
  IEN2 |= 0x10;//ʹ��P1�˿��ж�
  P1IEN |= 0x04;//ʹ��P1_2�˿��ж�
  PICTL |= 0x02;//����P1_2�˿��жϴ�����ʽΪ���½��ش���
  /*.......������4����.......................................*/
}

void InitCLK(void)
{
  CLKCONCMD &= 0x80;
  while(CLKCONSTA & 0x40);
}

void InitTime1(void)
{
  /*.......������2��ʼ����ʱ��1����....................*/
  T1CTL = 0X09;//32��Ƶ����������ģʽ ��1001��
  TIMIF|= 0X40;//ʹ�ܶ�ʱ������ж�
  IEN1 |= 0X02;//��ʱ��1�ж�ʹ�� �� T1IE=1
  /*.......������2����.......................................*/
}

void InitUart0(void)
{	
  U0CSR |= 0XC0;//����ģʽ,�����ý�����ʹ��
  /*.......������3��ʼ����������....................*/
   
  PERCFG|= 0x00;//USART0ʹ�ñ���λ��1 P0_2 P0_3
  P0SEL |= 0X0C;//����P0_2 P0_3Ϊ����
  U0UCR |= 0X80;//������ 8λ����λ ����żУ�� 1λֹͣλ

  U0GCR = 11; //���ò�����Ϊ115200 �������϶�Ӧ��
  U0BAUD = 216;
  /*.......������3����.......................................*/
  
  UTX0IF = 0;// ����UART0 TX�жϱ�־ 
  URX0IF = 0;// ����UART0 RX�жϱ�־
  URX0IE = 1;// ʹ��UART0 RX�ж�
}

void UART0SendByte(unsigned char c)
{
  U0DBUF = c;// ��Ҫ���͵�1�ֽ�����д��U0DBUF
  while (!UTX0IF) ;// �ȴ�TX�жϱ�־����U0DBUF����
  UTX0IF = 0;// ����TX�жϱ�־
}

void UART0SendData(unsigned char *str,int len)
{
  for(int i=0;i<len;i++)
  {
    U0DBUF = str[i];		// ��Ҫ���͵�1�ֽ�����д��U0DBUF
    while (!UTX0IF) ;  // �ȴ�TX�жϱ�־����U0DBUF����
    UTX0IF = 0;       // ����TX�жϱ�־UART0SendByte(*str++);   // ����һ�ֽ�
  }
}


/*������*/
void main(void)
{
  InitLED();
  InitCLK();
  InitTime1();
  InitUart0();
  InitSW1();
  SHT_Init();
  EA = 1;//ʹ�����ж�
    
  while(1)
  {  
  }
}

/*�жϷ�����*/

#pragma vector = P1INT_VECTOR
__interrupt void P1_ISR(void)
{
/*.......������7��ʼ�������жϷ�����...........*/

   if(P1IFG & 0x04)//�жϰ����Ƿ���
   {
      LED2 = ~LED2;//�л�LED2��״̬
      P1IFG &= ~0x04;//���P1_2�˿��жϱ�־λ
   }
   
   P1IF = 0;//���P1�˿��жϱ�־λ
   
/*.......������7����...........*/
}


#pragma vector = T1_VECTOR
__interrupt void T1_ISR(void)
{
  counter++;
  T1STAT &= ~0x01;  //���ͨ��0�жϱ�־
  
  /*.......������6��ʼ�������ڱ�ʶλΪ1ʱ��ÿ��2�������ڻظ���LED1������...........*/
  if(counter == 30)//��ʱ2s
  {
    if(sflag == 1)//Ҫ���͵���LED1��
    {
      UART0SendData("LED1��",sizeof("LED1��"));
    }
    
    counter = 0;
  }
  /*.......������6����...........*/
}

#pragma vector = URX0_VECTOR
__interrupt void UART0_ISR(void)
{
  /*.......������5��ʼ����������0xAAʱ����LED1�������ô��ڱ�ʶλΪ1��
  ��������0xBBʱϨ��LED1�����ô��ڱ�ʶλΪ0���������ڻظ�һ�Ρ�LED1�𡱡�...........*/
  if(U0DBUF == 0xAA)
  {
    sflag = 1;
    LED1 = 1;
  }
  else if(U0DBUF == 0xBB)
  {
    sflag = 0;
    LED1 = 0;
    UART0SendData("LED1��",sizeof("LED1��"));
  }
  /*.......������5����...........*/  
  URX0IF = 0;// ����UART0 RX�жϱ�־
}
