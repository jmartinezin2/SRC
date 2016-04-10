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
#include "in2hydracontrol.h"
#define DEFAULT_LEN 256


/** Estas van a una clse superior
int XCL_Initialise (short *Found);
int XCL_Colour_Remap(int *Uncompressed, short *Compressed,bool Compress);*/


In2HydraControl::In2HydraControl(QObject *parent,int id) :
    QObject(parent)
{
    m_ID=id;
    //m_mode=0;
    ReadMode(&m_mode);
    m_used=true;
    m_autostart=false;

}
/** Lo inicializa*/
void In2HydraControl::init(int id){
    m_ID=id;
    //m_mode=0;
    m_used=true;
    m_autostart=false;
    ReadMode(&m_mode);
}

/** Arranca el hydra*/
int In2HydraControl::Start(){
   int rtn=XCL_Send_Command(m_ID,XCL_CMD_Start);
   return rtn;
}
/** Para el hydra*/
int In2HydraControl::Stop(){
   int rtn=true;
   if (m_used)
    XCL_Send_Command(m_ID,XCL_CMD_Shutdown);
   return rtn;
}
/** Cierra el hydra*/
int In2HydraControl::Close(){
   int rtn=XCL_Close(m_ID);
   return rtn;
}
/** Reinicia el hydra*/
int In2HydraControl::Restart(){
   int rtn=XCL_Restart(m_ID);
   return rtn;
}
/** Envía comando*/
int In2HydraControl::SendCommand(XCL_Commands command){
   int rtn=XCL_Send_Command(m_ID,command);
   return rtn;
}
/** Lee modo*/
int In2HydraControl::ReadMode(XCL_Modes *mode){
   int rtn=XCL_Read_Mode(m_ID,mode);       
   m_mode=*mode;
   return rtn;
}
/** Lee coficientes F*/
int In2HydraControl::ReadFcoeff(float *coefArray){
    int rtn=XCL_Read_Fcoeff(m_ID,coefArray);
    m_SupplyOffset=coefArray[MSP_HEAD1];
    m_ReturnOffset=coefArray[MSP_HEAD2];
    return rtn;
}
/** Lee coficientes I*/
int In2HydraControl::ReadIcoeff(short *coefArray){
    int rtn=XCL_Read_Icoeff(m_ID,coefArray);
    short dummy_color_comp=coefArray[MSP_COLOUR];
    int dummy_color_uncomp;
    XCL_Colour_Remap(&dummy_color_uncomp,&dummy_color_comp,false);
    int r=(int)((dummy_color_uncomp&0x00FF0000)>>16);
    int g=(int)((dummy_color_uncomp&0x0000FF00)>>8);
    int b=(int)((dummy_color_uncomp&0x000000FF)>>0);
    m_ColorHydra=QColor(r,g,b,0xFF);
    return rtn;
}
/** Lee coficientes F*/
int In2HydraControl::WriteFcoeff(float *coefArray){
    int rtn=XCL_Write_Fcoeff(m_ID,coefArray);
    return rtn;
}
/** Lee coficientes I*/
int In2HydraControl::WriteIcoeff(short *coefArray){
    int rtn=XCL_Write_Icoeff(m_ID,coefArray);
    return rtn;
}

/** Lee el control*/
int In2HydraControl::ReadControl(XCL_Controls control,XCL_PID *output){
    int rtn=XCL_Read_Control(m_ID,control,output);
    return rtn;
}
/** Escribe el control*/
int In2HydraControl::WriteControl(XCL_Controls control,XCL_PID *output){
    int rtn=XCL_Write_Control(m_ID,control,output);
    return rtn;
}
/** Lee el sensor*/
int In2HydraControl::ReadSensor(XCL_Sensors sensor, XCL_ADC *output){
    int rtn=XCL_Read_Sensor(m_ID,sensor,output);
    return rtn;
}
/** Escribe el sensor*/
int In2HydraControl::WriteSensor(XCL_Sensors sensor, XCL_ADC *output){
    int rtn=XCL_Write_Sensor(m_ID,sensor,output);
    return rtn;
}
/** Lee los switches*/
int In2HydraControl::ReadSwitches(short *Bits){
    int rtn=XCL_Read_Switches(m_ID,Bits);
    return rtn;
}
/** Lee los LEDs*/
int In2HydraControl::ReadLEDS(short *Bits, short *Fault){
    int rtn=XCL_Read_LEDs(m_ID,Bits,Fault);
    return rtn;
}
/** Lee el Filtro*/
int In2HydraControl::ReadFilter(XCL_Sensors Select, XCL_Filters *TimeConstant){
    int rtn=XCL_Read_Filter(m_ID,Select,TimeConstant);
    return rtn;
}
/** Escribe el Filtro*/
int In2HydraControl::WriteFilter(XCL_Sensors Select, XCL_Filters *TimeConstant){
    int rtn=XCL_Write_Filter(m_ID,Select,TimeConstant);
    return rtn;
}
/** Escribe la desripcion*/
int In2HydraControl::WriteDescription(const QString & desc){
    char *p=desc.toUtf8().data();
    int rtn=XCL_Write_HYDRA_Description(m_ID,p,desc.length());
    m_Descripcion=QString(p);
    return rtn;
}
/** Lee la desripcion*/
QString In2HydraControl::ReadDescription(){
    char dummy[1024];
    XCL_Read_HYDRA_Description(m_ID,&dummy[0],1024);
    QString rtn=QString(dummy);
    return rtn;
}
/** Lee el PID*/
int In2HydraControl::ReadPid(XCL_Controls Select, XCL_PID *Details){
    int rtn=XCL_Read_Control(m_ID,Select,Details);
    return rtn;
}

/** Lee el Drive*/
int In2HydraControl::ReadDrive(XCL_Drives Select, XCL_DAC *Details){
    int rtn=XCL_Read_Drive(m_ID,Select,Details);
    return rtn;
}
/** Escribe el Drive*/
int In2HydraControl::WriteDrive(XCL_Drives Select, XCL_DAC *Details){
    int rtn=XCL_Write_Drive(m_ID,Select,Details);
    return rtn;
}
/** Recoge informacion*/
int In2HydraControl::GetInfo(char SerialNum [], char Length, short *CurrentBootBlock){
    int rtn=XCL_Get_Info(m_ID,SerialNum,Length,CurrentBootBlock);
    return rtn;
}
/** Recoge descripcion de bloque*/
int In2HydraControl::GetBlockDesc(char Descrip [],short Length, short DescBlock){
    int rtn=XCL_Get_Block_Desc(m_ID,Descrip,Length,DescBlock);
    return rtn;
}
/** Lee log*/
int In2HydraControl::ReadLog(float *Farray){
    int rtn=XCL_Read_Log(m_ID,Farray);
    return rtn;
}
/** Guarda en la Flash*/
int In2HydraControl::SaveToFlash(short BlockToWrite, short NewBootBlock, char *NewDescrip,short DescSize){
    int rtn=XCL_Save_To_FLASH(m_ID,BlockToWrite,NewBootBlock,NewDescrip,DescSize);
    return rtn;
}
/** Guarda en disco*/
int In2HydraControl::SaveToDisk(LPCWSTR Filename,LPCWSTR Description,short DescSize){
    int rtn=XCL_Save_To_Disk(m_ID,Filename,Description,DescSize);
    return rtn;
}
/** Carga de disco*/
int In2HydraControl::LoadFromDisk(LPCWSTR Filename,LPCWSTR Description,short DescSize){
    int rtn=XCL_Load_From_Disk(m_ID,Filename,Description,DescSize);
    return rtn;
}
/** Devuelve el supplyOfsset*/
float In2HydraControl::getSupplyOffset(){
    return m_SupplyOffset;
}

/** Devuelve el returnOfsset*/
float In2HydraControl::getReturnOffset(){
    return m_ReturnOffset;
}
/** Devuelve el color del Hydra*/
QColor In2HydraControl::getColor(){
    return m_ColorHydra;
}
/** Establece el autostart*/
void In2HydraControl::setAutoStart(bool value){
    m_autostart=value;
}
/** Establece el used*/
void In2HydraControl::setUsed(bool value){
    m_used=value;
}
/** devuelve el autostart*/
bool In2HydraControl::getAutoStart(){
    return m_autostart;
}
/** devuelve el used*/
bool In2HydraControl::getUsed(){
    return m_used;
}
/** Devuelve la descripcion*/
QString In2HydraControl::getDescription(){
    return m_Descripcion;
}
/** Activa un flag*/
void In2HydraControl::setFlag(char flag,char value){
    if (flag==FLAG_ERROR_INICIANDO) m_FlagErrorIniciando=value;

}
/** Devuelve un flag*/
char In2HydraControl::getFlag(char flag){
    char rtn=-1;
    if (flag==FLAG_ERROR_INICIANDO) rtn=m_FlagErrorIniciando;
    return rtn;
}
/** Devuelve el modo*/
XCL_Modes In2HydraControl::getMode(){
    ReadMode(&m_mode);
    return m_mode;
}









