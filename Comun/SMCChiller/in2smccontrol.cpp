/*  Copyright (C) 2013 In2 Printing Solutions <info@in2ps.com>
    Author: Jose Maria Martinez

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
#include "in2smccontrol.h"
#include "in2char.h"

In2SMCControl::In2SMCControl(QObject *parent,int id) :
    QObject(parent)
{
    myID=id+1;
    slaveAddress=0x3030+myID;

    init();

}

/** Lo inicializa*/
void In2SMCControl::init(){

    dTemperatura=20.0;
    dRefTemperatura=20.0;
    dPresion=1.0;

    sTemperatura=0x0000;
    sRefTemperatura=0x0000;
    sPresion=0x0000;

    statusFlags=0x0000;
    alarm1Flags=0x0000;
    alarm2Flags=0x0000;
    alarm3Flags=0x0000;

    m_ok=true;

}


void In2SMCControl::convertTemperature(){
    dTemperatura=convertToTemperature(sTemperatura);
}

void In2SMCControl::convertPressure(){
    dPresion=convertToPressure(sPresion);
}

double In2SMCControl::convertSetTemperature2double(quint16 registro){
    double ret;
    ret=convertToSetTemperature(registro);
    return (ret);
}

double In2SMCControl::convertTemperature2double(quint16 registro){
    double ret;
    ret=convertToTemperature(registro);
    return (ret);
}

double In2SMCControl::convertPressure2double(quint16 registro){
    double ret;
    ret=convertToPressure(registro);
    return (ret);
}


void In2SMCControl::convertSetTemperature(){
    sRefTemperatura=convertToSetTemperatureRegister(dRefTemperatura);
}

double In2SMCControl::convertToTemperature(quint16 registro){
    double dRet;
    double dummy;
    if ((statusFlags&SMC_ST_FLAG_TEMP_UNIT)==SMC_ST_FLAG_TEMP_UNIT){
        //Unidades ºF
        dummy=registro/10;//-166.0+0.1*registro;
        dRet=(dummy-32)/1.8; //Para pasar de ºF a ºC
    }
    else{
        //Unidades ºC
        dRet=registro;//-110.0+0.1*registro;
    }
    return(dRet);
}

double In2SMCControl::convertToPressure(quint16 registro){
    double dRet;
    double dummy;
    if ((statusFlags&SMC_ST_FLAG_PRESS_UNIT)==SMC_ST_FLAG_PRESS_UNIT){
        //Unidades PSI
        dummy=registro;
        dRet=0.00689*dummy; //Para pasar de PSI a MPa
    }
    else{
        //Unidades MPa
        dRet=0.0+0.01*registro;
    }
    return(dRet);
}

double In2SMCControl::convertToResistivity(quint16 registro){
    double dRet=0;

    return(dRet);
}

double In2SMCControl::convertToSetTemperature(quint16 registro){
    double dRet;
    double dummy;
    if ((statusFlags&SMC_ST_FLAG_TEMP_UNIT)==SMC_ST_FLAG_TEMP_UNIT){
        //Unidades ºF
        dummy=41.0+0.1*registro;
        dRet=(dummy-32)/1.8; //Para pasar de ºF a ºC
    }
    else{
        //Unidades ºC
        dRet=5.0+0.1*registro;
    }
    return(dRet);
}


quint16 In2SMCControl::convertToSetTemperatureRegister(double valor){
    quint16 sRet;
    double dummy;
    if ((statusFlags&SMC_ST_FLAG_TEMP_UNIT)==SMC_ST_FLAG_TEMP_UNIT){
        //Unidades ºF
        dummy=(valor-32)/1.8;
        sRet=(quint16)(10.0*dummy-41.0);
    }
    else{
        //Unidades ºC
        sRet=(quint16)(10.0*valor-5.0);
    }
    return(sRet);
}

unsigned char In2SMCControl::getMode(){
    unsigned char iRet;
    if ((statusFlags&SMC_ST_FLAG_RUN)==SMC_ST_FLAG_RUN)
        iRet=SMC_MODE_Started;
    else
        iRet=SMC_MODE_Stopped;
    return (iRet);
}

void In2SMCControl::setMode(unsigned char valor){
    m_mode=valor;
}



void In2SMCControl::setUsed(bool used){
    m_used=used;
}



void In2SMCControl::setFlag(char flag, unsigned char value){

}

char In2SMCControl::getFlag(char flag){
    char rtn=-1;
    return rtn;
}

/** Establece el autostart*/
void In2SMCControl::setAutoStart(bool value){
    m_autostart=value;
}


void In2SMCControl::readTemperature(quint16 address){

    QByteArray datos;
    //slave address
    datos[0]=((slaveAddress&0xFF00)>>8);
    datos[1]=((slaveAddress&0x00FF)>>0);
    //function
    datos[2]=((SMC_SERIAL_READ_MULTIPLE&0xFF00)>>8);
    datos[3]=((SMC_SERIAL_READ_MULTIPLE&0x00FF)>>0);
    //read address
    datos[4]=((SMC_SERIAL_TEMPERATURE_ADD&0xFF000000)>>24);
    datos[5]=((SMC_SERIAL_TEMPERATURE_ADD&0x00FF0000)>>16);
    datos[6]=((SMC_SERIAL_TEMPERATURE_ADD&0x0000FF00)>>8);
    datos[7]=((SMC_SERIAL_TEMPERATURE_ADD&0x000000FF)>>0);
    //quantity to read
    datos[8]=0x30;
    datos[9]=0x30;
    datos[10]=0x30;
    datos[11]=0x33;

    emit sglNewDataToSend(datos);
}

void In2SMCControl::readStatus(quint16 address){

    QByteArray datos;
    //slave address
    datos[0]=((slaveAddress&0xFF00)>>8);
    datos[1]=((slaveAddress&0x00FF)>>0);
    //function
    datos[2]=((SMC_SERIAL_READ_MULTIPLE&0xFF00)>>8);
    datos[3]=((SMC_SERIAL_READ_MULTIPLE&0x00FF)>>0);
    //read address
    datos[4]=((SMC_SERIAL_STATUS_ADD&0xFF000000)>>24);
    datos[5]=((SMC_SERIAL_STATUS_ADD&0x00FF0000)>>16);
    datos[6]=((SMC_SERIAL_STATUS_ADD&0x0000FF00)>>8);
    datos[7]=((SMC_SERIAL_STATUS_ADD&0x000000FF)>>0);
    //quantity to read
    datos[8]=0x30;
    datos[9]=0x30;
    datos[10]=0x30;
    datos[11]=0x34;

    emit sglNewDataToSend(datos);
}


void In2SMCControl::readAllData(quint16 address){

    QByteArray datos;
    //slave address
    datos[0]=((slaveAddress&0xFF00)>>8);
    datos[1]=(slaveAddress&0x00FF);
    //function
    datos[2]=0x30;//((SMC_SERIAL_READ_MULTIPLE&0xFF00)>>8);
    datos[3]=0x33;//((SMC_SERIAL_READ_MULTIPLE&0x00FF)>>0);
    //read address
    datos[4]=0x30;//((SMC_SERIAL_TEMPERATURE_ADD&0xFF000000)>>24);
    datos[5]=0x30;//((SMC_SERIAL_TEMPERATURE_ADD&0x00FF0000)>>16);
    datos[6]=0x30;//((SMC_SERIAL_TEMPERATURE_ADD&0x0000FF00)>>8);
    datos[7]=0x30;//((SMC_SERIAL_TEMPERATURE_ADD&0x000000FF)>>0);
    //quantity to read
    datos[8]=0x30;
    datos[9]=0x30;
    datos[10]=0x30;
    datos[11]=0x43;

    emit sglNewDataToSend(datos);

}

void In2SMCControl::readSetPoint(quint16 address){

    QByteArray datos;
    //slave address
    datos[0]=((slaveAddress&0xFF00)>>8);
    datos[1]=(slaveAddress&0x00FF);
    //function
    datos[2]=0x30;//((SMC_SERIAL_READ_MULTIPLE&0xFF00)>>8);
    datos[3]=0x33;//((SMC_SERIAL_READ_MULTIPLE&0x00FF)>>0);
    //read address
    datos[4]=0x30;//((SMC_SERIAL_TEMPERATURE_ADD&0xFF000000)>>24);
    datos[5]=0x30;//((SMC_SERIAL_TEMPERATURE_ADD&0x00FF0000)>>16);
    datos[6]=0x30;//((SMC_SERIAL_TEMPERATURE_ADD&0x0000FF00)>>8);
    datos[7]=0x42;//((SMC_SERIAL_TEMPERATURE_ADD&0x000000FF)>>0);
    //quantity to read
    datos[8]=0x30;
    datos[9]=0x30;
    datos[10]=0x30;
    datos[11]=0x31;

    emit sglNewDataToSend(datos);

}


void In2SMCControl::writeSetpoint(quint16 address){

    QByteArray datos;
    //slave address
    datos[0]=((slaveAddress&0xFF00)>>8);
    datos[1]=((slaveAddress&0x00FF)>>0);
    //function
    datos[2]=((SMC_SERIAL_WRITE&0xFF00)>>8);
    datos[3]=((SMC_SERIAL_WRITE&0x00FF)>>0);
    //write address
    datos[4]=((SMC_SERIAL_SETPOINT_ADD&0xFF000000)>>24);
    datos[5]=((SMC_SERIAL_SETPOINT_ADD&0x00FF0000)>>16);
    datos[6]=((SMC_SERIAL_SETPOINT_ADD&0x0000FF00)>>8);
    datos[7]=((SMC_SERIAL_SETPOINT_ADD&0x000000FF)>>0);
    //write data
    In2Char z;
    datos[8]=z.fromAscii((sRefTemperatura&0xF000)>>12);
    datos[9]=z.fromAscii((sRefTemperatura&0x0F00)>>8);
    datos[10]=z.fromAscii((sRefTemperatura&0x00F0)>>4);
    datos[11]=z.fromAscii((sRefTemperatura&0x000F)>>0);

    emit sglNewDataToSend(datos);

}

void In2SMCControl::readSetpoint(quint16 address){

    QByteArray datos;
    //slave address
    datos[0]=((slaveAddress&0xFF00)>>8);
    datos[1]=((slaveAddress&0x00FF)>>0);
    //function
    datos[2]=((SMC_SERIAL_READ_MULTIPLE&0xFF00)>>8);
    datos[3]=((SMC_SERIAL_READ_MULTIPLE&0x00FF)>>0);
    //read address
    datos[4]=((SMC_SERIAL_SETPOINT_ADD&0xFF000000)>>24);
    datos[5]=((SMC_SERIAL_SETPOINT_ADD&0x00FF0000)>>16);
    datos[6]=((SMC_SERIAL_SETPOINT_ADD&0x0000FF00)>>8);
    datos[7]=((SMC_SERIAL_SETPOINT_ADD&0x000000FF)>>0);
    //quantity to read
    datos[8]=0x30;
    datos[9]=0x30;
    datos[10]=0x30;
    datos[11]=0x31;

    emit sglNewDataToSend(datos);

}

void In2SMCControl::runThisChiller(quint16 address){

    QByteArray datos;
    //slave address
    datos[0]=((slaveAddress&0xFF00)>>8);
    datos[1]=((slaveAddress&0x00FF)>>0);
    //function
    datos[2]=((SMC_SERIAL_WRITE&0xFF00)>>8);
    datos[3]=((SMC_SERIAL_WRITE&0x00FF)>>0);
    //write address
    datos[4]=0x30;//((SMC_SERIAL_COMMAND_ADD&0xFF000000)>>24);
    datos[5]=0x30;//((SMC_SERIAL_COMMAND_ADD&0x00FF0000)>>16);
    datos[6]=0x30;//((SMC_SERIAL_COMMAND_ADD&0x0000FF00)>>8);
    datos[7]=0x43;//((SMC_SERIAL_COMMAND_ADD&0x000000FF)>>0);
    //write data
    datos[8]=0x30;
    datos[9]=0x30;
    datos[10]=0x30;
    datos[11]=0x31;

    emit sglNewDataToSend(datos);
}


void In2SMCControl::stopThisChiller(quint16 address){

    QByteArray datos;
    //slave address
    datos[0]=((slaveAddress&0xFF00)>>8);
    datos[1]=((slaveAddress&0x00FF)>>0);
    //function
    datos[2]=((SMC_SERIAL_WRITE&0xFF00)>>8);
    datos[3]=((SMC_SERIAL_WRITE&0x00FF)>>0);
    //write address
    datos[4]=0x30;//((SMC_SERIAL_COMMAND_ADD&0xFF000000)>>24);
    datos[5]=0x30;//((SMC_SERIAL_COMMAND_ADD&0x00FF0000)>>16);
    datos[6]=0x30;//((SMC_SERIAL_COMMAND_ADD&0x0000FF00)>>8);
    datos[7]=0x43;//((SMC_SERIAL_COMMAND_ADD&0x000000FF)>>0);
    //write data
    datos[8]=0x30;
    datos[9]=0x30;
    datos[10]=0x30;
    datos[11]=0x30;

    emit sglNewDataToSend(datos);
}

void In2SMCControl::setRefTemperature(double valor){
    dRefTemperatura=valor;
}

