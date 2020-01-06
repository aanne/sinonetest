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
 /* TMOD |= 0x20;            //0010 0000;Timer1设置工作方式2
	TL1 = 256 - 240;   //溢出时间：时钟为Fsys，则240*（1/Fsys）=20us;
	TH1 = 256 - 240;
	TR1 = 0;
	ET1 = 1;//定时器1,inturrupt允许
	TR1 = 1;//打开定时器1*/

    TMOD |= 0x01;                 //0000 0001;Timer0设置工作方式1
	TL0 = (65536 - 12000)%256;    //溢出时间：时钟为Fsys，则12000*（1/Fsys）=1ms;
	TH0 = (65536 - 12000)/256;
	TR0 = 0;
	ET0 = 1;//定时器0允许
	TR0 = 1;//打开定时器0
}

void TWI_Init(void){
	OTCON |= 0x80;  //选择TWI模式
	SSCON0 = 0x88;  // ---- x---  0为不允许接收，1为允许接收
	SSCON1 = 0x03;  //xxxx xxxy  x为地址寄存器，y为0禁止/1允许通用地址响应
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
		SSCON0 &= 0xbf;  //中断清零   
        Status = SSCON0 & 0X07;	   //获取TWI状态码
        switch(Status){
            case 0x00:			   //接收到主机STOP信号                
            break;
            case 0x02:			   //接收到主机地址帧或数据帧
            if(SSI_TWI_ReceiveData()){
                buffer=SSI_TWI_ReceiveData();
            }                
            break;
            case 0x03:				//从机发送完一帧数据                
            break;
            case 0x04:				//等待RESTART或STOP
            break;
            case 0x05:				//等待RESTART或STOP
            break;
            default:
            break;
        }		
	}	
}