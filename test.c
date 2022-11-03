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
 
  /*.......������1����...........*/
}

void InitSW1()
{
  /*.......������4��ʼ������SW1����....................*/

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

  /*.......������2����.......................................*/
}

void InitUart0(void)
{	
  U0CSR |= 0XC0;//����ģʽ,�����ý�����ʹ��
  /*.......������3��ʼ����������....................*/
   

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
    U0DBUF = str[i];    // ��Ҫ���͵�1�ֽ�����д��U0DBUF
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

/*.......������7����...........*/
}


#pragma vector = T1_VECTOR
__interrupt void T1_ISR(void)
{
  counter++;
  T1STAT &= ~0x01;  //���ͨ��0�жϱ�־
  
  /*.......������6��ʼ�������ڱ�ʶλΪ1ʱ��ÿ��2�������ڻظ���LED1����...........*/

  /*.......������6����...........*/
}

#pragma vector = URX0_VECTOR
__interrupt void UART0_ISR(void)
{
  /*.......������5��ʼ����������0xAAʱ����LED1�������ô��ڱ�ʶλΪ1��
  ��������0xBBʱϨ��LED1�����ô��ڱ�ʶλΪ0���������ڻظ�һ�Ρ�LED1�𡱡�...........*/

  /*.......������5����...........*/  
  URX0IF = 0;// ����UART0 RX�жϱ�־
}
