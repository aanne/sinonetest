#include "Function_Init.h"
#include "SensorMethod.h"

unsigned long int tk;

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

void main(void){
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
    LED1_ON();
    LED2_ON();
    LED3_ON();
    TouchKeyInit();
    while(1){
        if(SOCAPI_TouchKeyStatus&0x80){
            SOCAPI_TouchKeyStatus&=0x80;
            tk = TouchKeyScan();
            if(tk){
                beep();
                LED1_OFF();
                LED2_OFF();
                LED3_OFF();
            }else{
                beepStop();
                LED1_ON();
                LED2_ON();
                LED3_ON();
            }
            TouchKeyRestart();
        }
    }
}