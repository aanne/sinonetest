#include "Function_Init.h"
#include "SensorMethod.h"

unsigned long int tk;
unsigned long int tick;
int Status = 0xff;
int buffer = 0xff;

void beep(void){
    P1&=0xEF;
}

void beepStop(void){
    P1|=0x10;
}

void LED1_ON(){
    P0&=0xEF;
}

void LED1_OFF(){
    P0|=0x10;
}

void LED2_ON(){
    P5&=0xFD;
}

void LED2_OFF(){
    P5|=0x02;
}

void LED3_ON(){
    P1&=0x7F;
}

void LED3_OFF(){
    P1|=0x80;
}

int SSI_TWI_ReceiveData(void){
	return SSDAT;
}

void Sys_init(){
    P0CON = 0xFF;
    P0PH  = 0xFF;	
    P1CON = 0xFF;
    P1PH  = 0xFF;
    P2CON = 0xFF;
    P2PH  = 0xFF;
    P5CON = 0xFF;
    P5PH  = 0xFF;

    P0 = 0xFF;
    P1 = 0xFF;
    P5 = 0xFF; 	
	P2 = 0xFF;
    EA = 1;
}

void Timer_Init(){
    TMCON = 0X07;
 /* TMOD |= 0x20;            //0010 0000;Timer1���ù�����ʽ2
	TL1 = 256 - 240;   //���ʱ�䣺ʱ��ΪFsys����240*��1/Fsys��=20us;
	TH1 = 256 - 240;
	TR1 = 0;
	ET1 = 1;//��ʱ��1,inturrupt����
	TR1 = 1;//�򿪶�ʱ��1*/

    TMOD |= 0x01;                 //0000 0001;Timer0���ù�����ʽ1
	TL0 = (65536 - 12000)%256;    //���ʱ�䣺ʱ��ΪFsys����12000*��1/Fsys��=1ms;
	TH0 = (65536 - 12000)/256;
	TR0 = 0;
	ET0 = 1;//��ʱ��0����
	TR0 = 1;//�򿪶�ʱ��0
}

void TWI_Init(void){
	OTCON |= 0x80;  //ѡ��TWIģʽ
	SSCON0 = 0x88;  // ---- x---  0Ϊ��������գ�1Ϊ�������
	SSCON1 = 0x03;  //xxxx xxxy  xΪ��ַ�Ĵ�����yΪ0��ֹ/1����ͨ�õ�ַ��Ӧ
    IE1 |= 0x01;
}

void main(void){
    Sys_init();    
    LED1_OFF();
    LED2_ON();
    LED3_ON();
    TWI_Init();
    Timer_Init();    
    TouchKeyInit();
    while(1){
        if(SOCAPI_TouchKeyStatus&0x80){
            SOCAPI_TouchKeyStatus&=0x80;
            tk = TouchKeyScan();
            if(tk){
                beep();
                LED2_OFF();
            }else{
                beepStop();
                LED2_ON();
            }
            TouchKeyRestart();
        }
    }
}

void timer0() interrupt 1{
    TL0 = (65536 - 12000)%256;
	TH0 = (65536 - 12000)/256;
    tick+=1;
	if(tick>=1000){ //isec
        tick=0;
        P04 = ~P04;       
    }
}

void timer1() interrupt 3{
    tick+=1;
    if(tick>=50000){
        tick=0;
        P04 = ~P04;       
    }        
}

void TWI_Int() interrupt 7{
	if(SSCON0&0x40){
		SSCON0 &= 0xbf;  //�ж�����   
        Status = SSCON0 & 0X07;	   //��ȡTWI״̬��
        switch(Status){
            case 0x00:			   //���յ�����STOP�ź�                
            break;
            case 0x02:			   //���յ�������ַ֡������֡
            if(SSI_TWI_ReceiveData()){
                buffer=SSI_TWI_ReceiveData();
            }                
            break;
            case 0x03:				//�ӻ�������һ֡����                
            break;
            case 0x04:				//�ȴ�RESTART��STOP
            break;
            case 0x05:				//�ȴ�RESTART��STOP
            break;
            default:
            break;
        }		
	}	
}