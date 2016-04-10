#include "in2s5time.h"
/*
00->10 msg
01->100 msg
10->1 sg
11->10sg*/

In2S5Time::In2S5Time()
{
}
/** Devuelve el entero asociado al S57*/
int In2S5Time::getInt(unsigned short val){
    unsigned char nibble0=val&0xFF;
    unsigned char nibble1=(val>>8)&0xFF;

    unsigned char val0=(nibble0&0x0F);
    unsigned char val1=(nibble0>>4)&0x0F;
    unsigned char val2=(nibble1)&0x0F;

    unsigned char escala=(nibble1>>4)&0x03;
    int foo=escalaToInt(escala);

    int rtn=(val0+val1*10+val2*100)*foo;

    return rtn;
}
/** Devuelve la escala de un S5 en formato S5*/
unsigned char In2S5Time::getEscalaFromS5(unsigned short val){
    unsigned char nibble1=(val>>8)&0xFF;
    unsigned char foo=nibble1&0x03;
    return foo;
}

/** Devuelve el valor numerico de la escala*/
int In2S5Time::escalaToInt(unsigned char escala){
    int foo=0;
    if (escala==0) foo=10;
    else if (escala==1) foo=100;
    else if  (escala==2) foo=1000;
    else if (escala==3) foo=10000;
    return foo;
}

/** devuelve un formato s5 asociado a ese int*/
unsigned short In2S5Time::getS5(int val){
    //Hay que sacar primero que escala necesitamos
    unsigned short escala=0;
    unsigned short foo=0;
    if ((val>=0)&&(val<1000*10)) foo=0; //escala=10 msg
    else if ((val>=0)&&(val<1000*100)) foo=1; //escala=100 msg
    else if ((val>=0)&&(val<1000*1000)) foo=2; //escala=1sg
    else foo=3; //escala=10sg

    escala=(foo<<12);

    int num=escalaToInt(foo);
    int tmp=val/num;

    unsigned char nibble2=tmp/100;
    unsigned char nibble1=(tmp%100)/10;
    unsigned char nibble0=((tmp%100)%10);

    unsigned short rtn=escala;
    rtn|=(nibble0&0x0F)+((nibble1&0x0F)<<4)+((nibble2&0x0F)<<8);

    return rtn;
}

