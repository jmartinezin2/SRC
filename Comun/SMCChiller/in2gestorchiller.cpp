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
#include "in2gestorchiller.h"
#include "in2smccontrol.h"
#include "qextserialport.h"
#include "in2defines.h"
#include <QTimer>
#include <QApplication>
#include <QDesktopWidget>
#include <QCoreApplication>
#include <QLabel>
#include <in2char.h>
#include <QSettings>

//Para los carteles auxiliares
#define AUX_INFO_WIDTH 200
#define AUX_INFO_HEIGHT 75

// temperatura minima de funcionamiento
#define MIN_TEMPERATURE 4.0
// temperatura maxima de funcionamiento
#define MAX_TEMPERATURE 87.0

// presion minima de funcionamiento
#define MIN_PRESSURE 0.0
// presion maxima de funcionamiento
#define MAX_PRESSURE 47.0


#define CHILLER_ERROR   0
#define CHILLER_WARNING 1

//Mensajes
#define READ_HOLDING_REGISTERS 0x03
#define PRESET_SINGLE_REGISTERS 0x06
#define PRESET_MULTIPLE_REGISTERS 0x10
#define READ_WRITE_4x_REGISTERS 0x17

#define DEFAULT_CONFIG_FILE "In2Chiller.ini"

#define CHILLER_TIMEOUT_MONITORING 5000

//Una sola instancia
In2GestorChiller *In2GestorChiller::mInstance  = NULL;


/** Lllamda a instancia unica*/
In2GestorChiller *In2GestorChiller::request(QObject *parent,int equipos)
{
    if(mInstance == NULL){
        mInstance = new In2GestorChiller(parent,equipos);
    }
    return mInstance;
}
/** Devuelve si está activo*/
bool In2GestorChiller::isEnabled(){
    return m_bEnabled;
}
/** Devuelve si está activo*/
bool In2GestorChiller::isAutoStart(){
    return m_bAutoStart;
}

/** Constructor oculto*/
In2GestorChiller::In2GestorChiller(QObject *parent,int equipos):
    QObject(parent)
{

    QString path=QString("%1\\Config\\%2").arg(QApplication::applicationDirPath()).arg(DEFAULT_CONFIG_FILE);
    QSettings FicheroIni(path,QSettings::IniFormat,0);
    FicheroIni.beginGroup(QString("SMC"));
    m_bEnabled=FicheroIni.value(QString("Enabled"),false).toBool();
    m_iPuerto=FicheroIni.value(QString("Puerto"),1).toInt();
    m_bAutoStart=FicheroIni.value(QString("AutoStart"),false).toBool();
    FicheroIni.endGroup();


    m_numEquipos=1;
    estadoPuerto=false;
    m_Descripcion.clear();
    SetFlag(FLAG_ESTADO_CHILLER,0);
    SetFlag(FLAG_CHILLER_OK,0);
    SetFlag(FLAG_COMCHILLER_OK,CHILLER_COMNOK);
    //Inicializamos a stop
    memset(&m_Modes,0,sizeof(m_Modes));


    /*estadoPuerto=MiGestorModbus->getStateConnection();
    if (estadoPuerto)
        Init();*/
    MiGestorModbus=NULL;
    if (m_bEnabled){
        MiGestorModbus = new GestorModbus();
        connect(MiGestorModbus,SIGNAL(sglModbusDataSent(QByteArray)),this,SLOT(sltModbusDataSent(QByteArray)));
        connect(MiGestorModbus,SIGNAL(sglModbusDataReceived(QByteArray)),this,SLOT(sltModbusDataReceived(QByteArray)));
        MiGestorModbus->setComPort(m_iPuerto);
        MiGestorModbus->setConfig(BAUD19200,QextSerialPort::EventDriven,FLOW_OFF,PAR_EVEN,DATA_7,STOP_1,250,QIODevice::ReadWrite);

        estadoPuerto=MiGestorModbus->OpenConnection();
        if (! estadoPuerto){
            emit SendDebugInfo(LOGGER_WARNING,"Chiller-No se pudo abrir puerto");
            return;
        }
        Init();
        if (m_bAutoStart){
            QTimer::singleShot(5000,this,SLOT(_sltInitDefaultChiller()));
        }
    }
    DWORD id=(DWORD)QThread::currentThreadId();
    emit SendDebugInfo(LOGGER_ERROR,QString("Thread Chiller ID:%1").arg(id));

}
/** Arranca el chiller*/
void In2GestorChiller::_sltInitDefaultChiller(){
    this->sltRunChiller(0);
    emit SendDebugInfo(LOGGER_ERROR,QString("Starting default chiller"));
}

/** Inicializa la fluidica*/
void In2GestorChiller::Init(){
    if (m_numEquipos>=1){
        //Asigno IDs a los equipos conectados
        for(unsigned char i=0;i<m_numEquipos;i++){
            In2SMCControl *equipo=new In2SMCControl(this,i);
            connect(equipo,SIGNAL(sglNewDataToSend(QByteArray)),this,SLOT(sltNewDataToSend(QByteArray)));
            setDischargePressure(i,0x0000);
            setDischargeTemp(i,0x0000);
            setSetpoint(i,0x0000);
            setStatus(i,0x0000);
            setStatus(i,0x0000);
            setAlarm1(i,0x0000);
            setAlarm2(i,0x0000);
            setAlarm3(i,0x0000);
            setTemperatureChange(i,false);
            setPressureChange(i,false);
            setSetpointChange(i,false);
            setStatusChange(i,false);
            setAlarm1Change(i,false);
            setAlarm2Change(i,false);
            setAlarm3Change(i,false);
            m_equipos.append(equipo);
            equipo->readSetpoint(i);
        }
        SetFlag(FLAG_CHILLER_OK,1); //Hemos encontrado equipos y se ha inicializado
        sltMonitoring();

     }
     else{
        SetFlag(FLAG_ESTADO_CHILLER,0);
        SetFlag(FLAG_CHILLER_OK,0);
        SetFlag(FLAG_COMCHILLER_OK,CHILLER_COMNOK);
    }
}
/** Devuelve el estado del puerto*/
bool In2GestorChiller::getPortState(){
   return estadoPuerto;
}

/** Monitoriza valores*/
void In2GestorChiller::sltMonitoring(){
    static char token=1;
    estadoPuerto=MiGestorModbus->getStateConnection();
    if (estadoPuerto) sltGetFluidicData();
    //sltMonitoring();
    if (token%3==0){
        token=0;        
        char flag=GetFlag(FLAG_COMCHILLER_OK);
        if (flag==CHILLER_COMPENDING) SetFlag(FLAG_COMCHILLER_OK,CHILLER_COMNOK);
        else if (flag==CHILLER_COMOK) SetFlag(FLAG_COMCHILLER_OK,CHILLER_COMPENDING);
    }
    token++;

    QTimer::singleShot(CHILLER_TIMEOUT_MONITORING,this,SLOT(sltMonitoring()));
}

/** Coge datos de fluidica*/
void In2GestorChiller::sltGetFluidicData(){
#ifdef DEPURACION_IN2
    for (int x=0;x<m_numEquipos;x++){
        In2SMCControl *equipo=m_equipos.at(x);
        equipo->readAllData(x);

        /*filterTemperatura(x);
        filterPresion(x);
        filterAlarm1(x);
        filterAlarm2(x);
        filterAlarm3(x);

        if (getTemperatureChange(x)){
            setTemperatureChange(x,false);
            emit sglCambioTemperatura(x);
        }

        if (getPressureChange(x)){
            setPressureChange(x,false);
            emit sglCambioPresion(x);
        }

        if (getReadyChange(x)){
            setReadyChange(x,false);
            emit sglCambioReady(x);
        }

        if (getSetpointChange(x)){
            setSetpointChange(x,false);
            emit sglCambioSetpoint(x);
        }

        if (getStatusChange(x)){
            setStatusChange(x,false);
            emit sglCambioStatus(x);
        }

        if (getAlarm1Change(x)){
            setAlarm1Change(x,false);
            emit sglCambioAlarm1(x);
        }

        if (getAlarm2Change(x)){
            setAlarm2Change(x,false);
            emit sglCambioAlarm2(x);
        }

        if (getAlarm3Change(x)){
            setAlarm3Change(x,false);
            emit sglCambioAlarm3(x);
        }*/
    }
#endif
}


/** Inicializa un equipo*/
void In2GestorChiller::InitEquipo(int x){
 if (x<m_numEquipos){
    In2SMCControl *equipo=m_equipos.at(x);
    if (equipo->getMode()==SMC_MODE_Stopped){
        equipo->setAutoStart(true);
        equipo->setUsed(true);
        equipo->setFlag(FLAG_ERROR_INICIANDO,0);
     }
  }
}

/** Para un equipo*/
void In2GestorChiller::StopEquipo(int x){
 if (x<m_numEquipos){
    In2SMCControl *equipo=m_equipos.at(x);
    if (equipo->getMode()!=SMC_MODE_Stopped){
        equipo->setUsed(true);
        equipo->setFlag(FLAG_ERROR_INICIANDO,0);
     }
  }
}


/** Devueleve el numero de equipos*/
int In2GestorChiller::numEquipos(){
    return m_numEquipos;
}


/** Activa un flag*/
void In2GestorChiller::SetFlag(char flag,char value){
    if (flag==FLAG_ESTADO_CHILLER){
        if (m_FlagEstado!=value){
            m_FlagEstado=value;
            emit ChillerStateNewValue(value);
        }
    }
    else if (flag==FLAG_CHILLER_OK) m_FlagChillerOK=value;
    else if (flag==FLAG_COMCHILLER_OK){
        if ((m_FlagComunicacionChillerOK==CHILLER_COMPENDING)&&(value==CHILLER_COMNOK)){
            emit sgnChillerCom(CHILLER_COMNOK);
        }
        else if ((m_FlagComunicacionChillerOK==CHILLER_COMNOK)&&(value==CHILLER_COMOK)){
            emit sgnChillerCom(CHILLER_COMOK);
        }
            m_FlagComunicacionChillerOK=value;
    }
}

/** devuelve un flag*/
char In2GestorChiller::GetFlag(char flag){
    char rtn=-1;
    if (flag==FLAG_ESTADO_CHILLER) rtn=m_FlagEstado;
    else if (flag==FLAG_CHILLER_OK) rtn=m_FlagChillerOK;
    else if (flag==FLAG_COMCHILLER_OK) rtn=m_FlagComunicacionChillerOK;
    return rtn;
}


/** Devuelve estado*/
unsigned char In2GestorChiller::getMode(int index){
    unsigned char rtn=0x00;
    if (index<m_numEquipos){
       In2SMCControl *equipo=m_equipos.at(index);
       rtn=equipo->getMode();
       m_Modes[index]=rtn;
    }
    return rtn;
}

void In2GestorChiller::setMode(int index,unsigned char valor){
    unsigned char rtn;
    if (index<m_numEquipos){
       In2SMCControl *equipo=m_equipos.at(index);
       rtn=equipo->getMode();
       if (rtn!=valor){
            equipo->setMode(valor);
       }
    }
}

/** Devuelve la presion*/
quint16 In2GestorChiller::getDischargePressure(int index){
    /*In2SMCControl *equipo=m_equipos.at(index);
    Discharge_Pr[index]=equipo->convertPressure2double(sDischarge_Pr[index]);*/
    return sDischarge_Pr[index];
}

/** Devuelve la temperatura*/
quint16 In2GestorChiller::getDischargeSetTemp(int index){
    /*In2SMCControl *equipo=m_equipos.at(index);
    Discharge_SetTemp[index]=equipo->convertSetTemperature2double(sDischarge_SetTemp[index]);*/
    return sDischarge_SetTemp[index];
}

/** Devuelve la referencia de temperatura*/
quint16 In2GestorChiller::getDischargeTemp(int index){
    /*In2SMCControl *equipo=m_equipos.at(index);
    Discharge_Temp[index]=equipo->convertTemperature2double(sDischarge_Temp[index]);*/
    return sDischarge_Temp[index];
}

/** Devuelve el estado del equipo indicado*/
quint16 In2GestorChiller::getStatus(int x){
    return  status[x];
}

/** Devuelve el switches del equipo indicado*/
quint16 In2GestorChiller::getAlarm1(int x){
    return  alarm1[x];
}

quint16 In2GestorChiller::getAlarm2(int x){
    return  alarm2[x];
}

quint16 In2GestorChiller::getAlarm3(int x){
    return  alarm3[x];
}

bool In2GestorChiller::getReady(int x){
    return readyToPrint[x];
}

void In2GestorChiller::setDischargeTemp(int x,quint16 value){
    if (sDischarge_Temp[x]!=value) emit sglCambioTemperatura(x);
    sDischarge_Temp[x]=value;
}

void In2GestorChiller::setDischargePressure(int x,quint16 value){
    if (sDischarge_Pr[x]!=value) emit sglCambioPresion(x);
    sDischarge_Pr[x]=value;
}


void In2GestorChiller::setSetpoint(int x,quint16 value){
    if (sDischarge_SetTemp[x]!=value) emit sglCambioSetpoint(x);
    sDischarge_SetTemp[x]=value;
}

void In2GestorChiller::setStatus(int x,quint16 value){
    if (status[x]!=value) emit sglCambioStatus(x);
    status[x]=value;
    m_Modes[x]=(unsigned char) (value&0x01);
}

/** Devuelve el switches del equipo indicado*/
void In2GestorChiller::setAlarm1(int x,quint16 value){
    if (alarm1[x]!=value) {
        emit sglCambioAlarm1(x);
        if (value==0) SetFlag(FLAG_CHILLER_OK,1);
        else SetFlag(FLAG_CHILLER_OK,0);
    }
    alarm1[x]=value;
}

void In2GestorChiller::setAlarm2(int x,quint16 value){
    if (alarm2[x]!=value){
        if (value==0) SetFlag(FLAG_CHILLER_OK,1);
        else SetFlag(FLAG_CHILLER_OK,0);
        emit sglCambioAlarm2(x);
    }
    alarm2[x]=value;
}

void In2GestorChiller::setAlarm3(int x, quint16 value){
    if (alarm3[x]!=value){
        if (value==0) SetFlag(FLAG_CHILLER_OK,1);
        else SetFlag(FLAG_CHILLER_OK,0);
        emit sglCambioAlarm3(x);
    }
    alarm3[x]=value;
}

void In2GestorChiller::setReady(int x, bool value){
    if (readyToPrint[x]!=value) emit sglCambioReady(x);
    readyToPrint[x]=value;
}


bool In2GestorChiller::getTemperatureChange(int x){
    return temperatureChange[x];
}

bool In2GestorChiller::getPressureChange(int x){
    return pressureChange[x];
}

bool In2GestorChiller::getReadyChange(int x){
    return readyChange[x];
}

bool In2GestorChiller::getSetpointChange(int x){
    return setpointChange[x];
}

bool In2GestorChiller::getStatusChange(int x){
    return statusChange[x];
}

bool In2GestorChiller::getAlarm1Change(int x){
    return alarm1Change[x];
}

bool In2GestorChiller::getAlarm2Change(int x){
    return alarm2Change[x];
}

bool In2GestorChiller::getAlarm3Change(int x){
    return alarm3Change[x];
}


void In2GestorChiller::setTemperatureChange(int x,bool valor){
    temperatureChange[x]=valor;
}

void In2GestorChiller::setPressureChange(int x,bool valor){
    pressureChange[x]=valor;
}

void In2GestorChiller::setSetpointChange(int x,bool valor){
    setpointChange[x]=valor;
}

void In2GestorChiller::setStatusChange(int x,bool valor){
    statusChange[x]=valor;
}

void In2GestorChiller::setAlarm1Change(int x,bool valor){
    alarm1Change[x]=valor;
}

void In2GestorChiller::setAlarm2Change(int x,bool valor){
    alarm2Change[x]=valor;
}

void In2GestorChiller::setAlarm3Change(int x,bool valor){
    alarm3Change[x]=valor;
}

void In2GestorChiller::setReadyChange(int x,bool valor){
    readyChange[x]=valor;
}


/** Devuele descripcion del estado*/

QString In2GestorChiller::getModeDescription(int x){
    unsigned char localMode=m_Modes[x];
    QString modeStr;
    if (localMode==SMC_MODE_Started) modeStr=QString("Iniciado");
    else modeStr=QString("Parado");
    return (modeStr);
}

QString In2GestorChiller::getStatusDescription(int x){
    quint16 localSwitch=status[x];

    QString statusStr=QString::number(localSwitch,16).toUpper().rightJustified(4,'0',true);
    QString rtn=QString("Código status: 0x%1\n").arg(statusStr);

    if (localSwitch&0x0001) rtn.append("Run flag: Run\n");
    else rtn.append("Run flag: Stop\n");

    if (localSwitch&0x0002) rtn.append("Operation stop alarm flag: given off\n");
    else rtn.append("Operation stop alarm flag: not ocurred\n");

    if (localSwitch&0x0004) rtn.append("Operation continued alarm flag: given off\n");
    else rtn.append("Operation continued alarm flag: not ocurred\n");

    if (localSwitch&0x0010) rtn.append("Press unit flag: PSI\n");
    else rtn.append("Press unit flag: MPa\n");

    if (localSwitch&0x0020) rtn.append("Remote status flag: Serial mode\n");
    else rtn.append("Remote status flag: Not serial mode\n");

    if (localSwitch&0x0200) rtn.append("Temp ready flag: ok\n");
    else rtn.append("Temp ready flag: no ok\n");

    if (localSwitch&0x0400) rtn.append("Temperature unit flag: ºF\n");
    else rtn.append("Temperature unit flag: ºC\n");

    if (localSwitch&0x0800) rtn.append("Run timer flag: set\n");
    else rtn.append("Run timer flag: not set\n");

    if (localSwitch&0x1000) rtn.append("Stop timer flag: set\n");
    else rtn.append("Stop timer flag: not set\n");

    if (localSwitch&0x2000) rtn.append("Reset after power failure flag: set\n");
    else rtn.append("Reset after power failure flag: not set\n");

    if (localSwitch&0x4000) rtn.append("Anti-freezing flag: set\n");
    else rtn.append("Anti-freezing flag: not set\n");

    if (localSwitch&0x8000) rtn.append("Automatic fluid filling flag: fluid\n");
    else rtn.append("Automatic fluid filling flag: stop\n");

    return rtn;
}

/** Devuelve descripcion de fallo*/
QString In2GestorChiller::getAlarm1Description(int x){
    /*quint16 LocalFault=alarm1[x];
    QString rtn=QString("%1").arg(LocalFault);
    return rtn;*/
    return QString();

}

QString In2GestorChiller::getAlarm2Description(int x){
    quint16 LocalFault=alarm2[x];
    QString rtn=QString("%1").arg(LocalFault);
    return rtn;
}

QString In2GestorChiller::getAlarm3Description(int x){
    quint16 LocalFault=alarm3[x];
    QString rtn=QString("%1").arg(LocalFault);
    return rtn;
}

/** Filtro de estado*/
void In2GestorChiller::filterStatus(int x){
    //quint16 localBits=status[x];
    //emit ChillerError(CHILLER_ERROR,x,localBits);
}

/** Filtro faults*/
void In2GestorChiller::filterAlarm1(int x){
    if (x<m_numEquipos){
        quint16 LocalFault=alarm1[x];
        if (LocalFault!=0) setReady(x,false);
        else setReady(x,true);
    }
}

void In2GestorChiller::filterAlarm2(int x){
    if (x<m_numEquipos){
        quint16 LocalFault=alarm2[x];
        if (LocalFault!=0) setReady(x,false);
        else setReady(x,true);
    }
}

void In2GestorChiller::filterAlarm3(int x){
    if (x<m_numEquipos){
        quint16 LocalFault=alarm3[x];
        if (LocalFault!=0) setReady(x,false);
        else setReady(x,true);
    }
}



/** Filtro de cambio de estado*/
void In2GestorChiller::filterCambioEstado(int x){
    if (x<m_numEquipos){
        In2SMCControl *equipo=m_equipos.at(x);
        int tmp=equipo->getMode();
        if (tmp!=m_Modes[x]){
            //emit ChillerError(0,x,(float)tmp);
            m_Modes[x]=tmp;
            char state=1;
            //Revisa todos los equipos para avisar del cmabio
            for (int i=0;i<m_numEquipos;i++){
                if (m_Modes[x]!=SMC_MODE_Started){
                    state=0;
                    break;
                }
            }
            SetFlag(FLAG_ESTADO_CHILLER,state);
        }
    }
}

/** Filtro de temperatura*/
void In2GestorChiller::filterTemperatura(int x){
    QString str;
    if (x<m_numEquipos){
        if (m_Modes[x]==SMC_MODE_Started){
            if ((sDischarge_Temp[x]<MIN_TEMPERATURE)||(sDischarge_Temp[x]>MAX_TEMPERATURE)){
                str=QString("Equipo %1 fuera de rango de temperatura").arg(x+1);
                setReady(x,false);
                emit ChillerError(str);
            }
            else{
                setReady(x,true);
            }
        }
    }
}

void In2GestorChiller::filterPresion(int x){
    QString str;
    if (x<m_numEquipos){
        if (m_Modes[x]==SMC_MODE_Started){
            if ((sDischarge_Pr[x]<MIN_PRESSURE)||(sDischarge_Pr[x]>MAX_PRESSURE)){
                str=QString("Equipo %1 fuera de rango de temperatura").arg(x+1);
                setReady(x,false);
                emit ChillerError(str);
            }
            else{
                setReady(x,true);
            }
        }
    }
}


/** Devuelve si la fluidica puede imprimir*/
bool In2GestorChiller::isReadyToPrint(){
    bool rtn=true;
    if (m_bEnabled){
        if (GetFlag(FLAG_COMCHILLER_OK)==CHILLER_COMNOK) rtn=false;
        if (GetFlag(FLAG_CHILLER_OK)==0) rtn=false;
    }

    //if (GetFlag(FLAG_CHILLER_OK)){
        //if (!GetFlag(FLAG_ESTADO_CHILLER)) rtn=0;
    //}
    return rtn;
}

void In2GestorChiller::sltNewSetpointValue(int equipo,double valor){

    In2SMCControl *chiller=m_equipos.at(equipo);
    chiller->setRefTemperature(valor);
    chiller->convertSetTemperature();
    chiller->writeSetpoint(equipo);
}


void In2GestorChiller::sltNewDataAvailable(QByteArray datos){

    int bytesRecibidos=datos.size();
    if  (bytesRecibidos<6) return;


    In2Char z;
    char equipo=((z.fromAscii(datos.at(1)))<<4)+(z.fromAscii(datos.at(2)));
    char funcion=((z.fromAscii(datos.at(3)))<<4)+(z.fromAscii(datos.at(4)));
    char byteCount=((z.fromAscii(datos.at(5)))<<4)+(z.fromAscii(datos.at(6)));

    SetFlag(FLAG_COMCHILLER_OK,CHILLER_COMOK);

    //return;

    switch (funcion){
        case READ_HOLDING_REGISTERS:
        //Respuesta a un mensaje tipo "Read multiple registers"
        //Puede ser uno de estos tres mensajes
        //Temperatura + Reserved + Presion + Reserved + Status + Alarm 1 + Alarm 2 + Alarm3
        //Temperatura + Reserved + Presion
        //Status + Alarm 1 + Alarm 2 + Alarm3
        if ((byteCount==0x18)){
            unsigned char nibbleHigh=((z.fromAscii(datos.at(7)))<<4)+z.fromAscii(datos.at(8));
            unsigned char nibbleLow=((z.fromAscii(datos.at(9)))<<4)+z.fromAscii(datos.at(10));
            quint16 value=nibbleHigh*256+nibbleLow;
            setDischargeTemp(equipo-1,value);

            nibbleHigh=((z.fromAscii(datos.at(15)))<<4)+z.fromAscii(datos.at(16));
            nibbleLow=((z.fromAscii(datos.at(17)))<<4)+z.fromAscii(datos.at(18));
            value=nibbleHigh*256+nibbleLow;
            setDischargePressure(equipo-1,value);

            nibbleHigh=((z.fromAscii(datos.at(23)))<<4)+z.fromAscii(datos.at(24));
            nibbleLow=((z.fromAscii(datos.at(25)))<<4)+z.fromAscii(datos.at(26));
            value=nibbleHigh*256+nibbleLow;
            setStatus(equipo-1,value);

            nibbleHigh=((z.fromAscii(datos.at(27)))<<4)+z.fromAscii(datos.at(28));
            nibbleLow=((z.fromAscii(datos.at(29)))<<4)+z.fromAscii(datos.at(30));
            value=nibbleHigh*256+nibbleLow;
            setAlarm1(equipo-1,value);

            nibbleHigh=((z.fromAscii(datos.at(31)))<<4)+z.fromAscii(datos.at(32));
            nibbleLow=((z.fromAscii(datos.at(33)))<<4)+z.fromAscii(datos.at(34));
            value=nibbleHigh*256+nibbleLow;
            setAlarm2(equipo-1,value);

            nibbleHigh=((z.fromAscii(datos.at(35)))<<4)+z.fromAscii(datos.at(36));
            nibbleLow=((z.fromAscii(datos.at(37)))<<4)+z.fromAscii(datos.at(38));
            value=nibbleHigh*256+nibbleLow;
            setAlarm3(equipo-1,value);

            nibbleHigh=((z.fromAscii(datos.at(51)))<<4)+z.fromAscii(datos.at(52));
            nibbleLow=((z.fromAscii(datos.at(53)))<<4)+z.fromAscii(datos.at(54));
            value=nibbleHigh*256+nibbleLow;
            setSetpoint(equipo-1,value);

        }

        break;
        case PRESET_SINGLE_REGISTERS:


        break;
        case PRESET_MULTIPLE_REGISTERS:
        break;
        case READ_WRITE_4x_REGISTERS:
        break;
    }
    emit sglNewReceivedMessage(datos);


    /*else emit sglNewReceivedMessage(datos);

    return;

    data1=datos[0];
    data2=datos[1];
    data1_hex=mapaHex.at(mapaAsc.indexOf(data1));
    data2_hex=mapaHex.at(mapaAsc.indexOf(data2));
    direccionSlave=(unsigned char)( ((data1_hex&0x0F)<<4) | ((data2_hex&0x0F)<<0) );


    data1=datos[2];
    data2=datos[3];
    data1_hex=mapaHex.at(mapaAsc.indexOf(data1));
    data2_hex=mapaHex.at(mapaAsc.indexOf(data2));
    funcion=(unsigned char)((data1_hex<<4)|(data2_hex<<0));



    ModbusMessage *msg= new ModbusMessage(0,datos);
    QByteArray trama;
    trama=datos.left(datos.size()-2);
    crcCalculated=msg->calculateLRC(trama);

    switch (funcion){
        case 0x03:        
            data1=datos[4];
            data2=datos[5];
            data1_hex=mapaHex.at(mapaAsc.indexOf(data1));
            data2_hex=mapaHex.at(mapaAsc.indexOf(data2));
            byteCount=(unsigned char)((data1_hex<<4)|(data2_hex<<0));
            //Respuesta a un mensaje tipo "Read multiple registers"
            //Puede ser uno de estos tres mensajes
            //Temperatura + Reserved + Presion + Reserved + Status + Alarm 1 + Alarm 2 + Alarm3
            //Temperatura + Reserved + Presion
            //Status + Alarm 1 + Alarm 2 + Alarm3
            if (byteCount==0x0F){
                if  (bytesRecibidos<18) return;

                data1=datos[38];
                data2=datos[39];
                data1_hex=mapaHex.at(mapaAsc.indexOf(data1));
                data2_hex=mapaHex.at(mapaAsc.indexOf(data2));
                lrc=(unsigned char)((data1_hex<<4)|(data2_hex<<0));

                //if (lrc!=crcCalculated) return;

                data1=datos[6];
                data2=datos[7];
                data3=datos[8];
                data4=datos[9];
                data1_hex=mapaHex.at(mapaAsc.indexOf(data1));
                data2_hex=mapaHex.at(mapaAsc.indexOf(data2));
                data3_hex=mapaHex.at(mapaAsc.indexOf(data3));
                data4_hex=mapaHex.at(mapaAsc.indexOf(data4));
                value=(quint16)((data1_hex<<12)|(data2_hex<<8)|(data3_hex<<4)|(data4_hex<<0));
                //sDischarge_Temp[direccionSlave-1]=(quint16) value;
                setDischargeTemp(direccionSlave-1,value);

                data1=datos[14];
                data2=datos[15];
                data3=datos[16];
                data4=datos[17];
                data1_hex=mapaHex.at(mapaAsc.indexOf(data1));
                data2_hex=mapaHex.at(mapaAsc.indexOf(data2));
                data3_hex=mapaHex.at(mapaAsc.indexOf(data3));
                data4_hex=mapaHex.at(mapaAsc.indexOf(data4));
                value=(quint16)((data1_hex<<12)|(data2_hex<<8)|(data3_hex<<4)|(data4_hex<<0));
                //sDischarge_Pr[direccionSlave-1]=(quint16) value;
                setDischargePressure(direccionSlave-1,value);

                data1=datos[22];
                data2=datos[23];
                data3=datos[24];
                data4=datos[25];
                data1_hex=mapaHex.at(mapaAsc.indexOf(data1));
                data2_hex=mapaHex.at(mapaAsc.indexOf(data2));
                data3_hex=mapaHex.at(mapaAsc.indexOf(data3));
                data4_hex=mapaHex.at(mapaAsc.indexOf(data4));
                value=(quint16)((data1_hex<<12)|(data2_hex<<8)|(data3_hex<<4)|(data4_hex<<0));
                setStatus(direccionSlave-1,value);
                //status[direccionSlave-1]=(quint16) value;
                //m_Modes[direccionSlave-1]=(unsigned char) (data4_hex&0x01);

                data1=datos[26];
                data2=datos[27];
                data3=datos[28];
                data4=datos[29];
                data1_hex=mapaHex.at(mapaAsc.indexOf(data1));
                data2_hex=mapaHex.at(mapaAsc.indexOf(data2));
                data3_hex=mapaHex.at(mapaAsc.indexOf(data3));
                data4_hex=mapaHex.at(mapaAsc.indexOf(data4));
                value=(quint16)((data1_hex<<12)|(data2_hex<<8)|(data3_hex<<4)|(data4_hex<<0));
                //alarm1[direccionSlave-1]=(quint16) value;
                setAlarm1(direccionSlave-1,value);

                data1=datos[30];
                data2=datos[31];
                data3=datos[32];
                data4=datos[33];
                data1_hex=mapaHex.at(mapaAsc.indexOf(data1));
                data2_hex=mapaHex.at(mapaAsc.indexOf(data2));
                data3_hex=mapaHex.at(mapaAsc.indexOf(data3));
                data4_hex=mapaHex.at(mapaAsc.indexOf(data4));
                value=(quint16)((data1_hex<<12)|(data2_hex<<8)|(data3_hex<<4)|(data4_hex<<0));
                //alarm2[direccionSlave-1]=(quint16) value;
                setAlarm2(direccionSlave-1,value);

                data1=datos[34];
                data2=datos[35];
                data3=datos[36];
                data4=datos[37];
                data1_hex=mapaHex.at(mapaAsc.indexOf(data1));
                data2_hex=mapaHex.at(mapaAsc.indexOf(data2));
                data3_hex=mapaHex.at(mapaAsc.indexOf(data3));
                data4_hex=mapaHex.at(mapaAsc.indexOf(data4));
                value=(quint16)((data1_hex<<12)|(data2_hex<<8)|(data3_hex<<4)|(data4_hex<<0));
                //alarm3[direccionSlave-1]=(quint16) value;
                setAlarm3(direccionSlave-1,value);
            }
            else if (byteCount==0x06){
                if  (bytesRecibidos<18) return;

                data1=datos[18];
                data2=datos[19];
                data1_hex=mapaHex.at(mapaAsc.indexOf(data1));
                data2_hex=mapaHex.at(mapaAsc.indexOf(data2));
                lrc=(unsigned char)((data1_hex<<4)|(data2_hex<<0));

                //if (lrc!=crcCalculated) return;

                data1=datos[6];
                data2=datos[7];
                data3=datos[8];
                data4=datos[9];
                data1_hex=mapaHex.at(mapaAsc.indexOf(data1));
                data2_hex=mapaHex.at(mapaAsc.indexOf(data2));
                data3_hex=mapaHex.at(mapaAsc.indexOf(data3));
                data4_hex=mapaHex.at(mapaAsc.indexOf(data4));
                value=(short)((data1_hex<<12)|(data2_hex<<8)|(data3_hex<<4)|(data4_hex<<0));
                //sDischarge_Temp[direccionSlave-1]=(quint16) value;
                setDischargeTemp(direccionSlave-1,value);

                data1=datos[14];
                data2=datos[15];
                data3=datos[16];
                data4=datos[17];
                data1_hex=mapaHex.at(mapaAsc.indexOf(data1));
                data2_hex=mapaHex.at(mapaAsc.indexOf(data2));
                data3_hex=mapaHex.at(mapaAsc.indexOf(data3));
                data4_hex=mapaHex.at(mapaAsc.indexOf(data4));
                value=(short)((data1_hex<<12)|(data2_hex<<8)|(data3_hex<<4)|(data4_hex<<0));
                //sDischarge_Pr[direccionSlave-1]=(quint16) value;
                setDischargePressure(direccionSlave-1,value);

            }
            else if (byteCount==0x08){
                if  (bytesRecibidos<22) return;

                data1=datos[22];
                data2=datos[23];
                data1_hex=mapaHex.at(mapaAsc.indexOf(data1));
                data2_hex=mapaHex.at(mapaAsc.indexOf(data2));
                lrc=(unsigned char)((data1_hex<<4)|(data2_hex<<0));
                //if (lrc!=crcCalculated) return;

                data1=datos[6];
                data2=datos[7];
                data3=datos[8];
                data4=datos[9];
                data1_hex=mapaHex.at(mapaAsc.indexOf(data1));
                data2_hex=mapaHex.at(mapaAsc.indexOf(data2));
                data3_hex=mapaHex.at(mapaAsc.indexOf(data3));
                data4_hex=mapaHex.at(mapaAsc.indexOf(data4));
                value=(short)((data1_hex<<12)|(data2_hex<<8)|(data3_hex<<4)|(data4_hex<<0));
                setStatus(direccionSlave-1,value);
                //status[direccionSlave-1]=(quint16) value;
                //m_Modes[direccionSlave-1]=(unsigned char) (data4_hex&0x01);

                data1=datos[10];
                data2=datos[11];
                data3=datos[12];
                data4=datos[13];
                data1_hex=mapaHex.at(mapaAsc.indexOf(data1));
                data2_hex=mapaHex.at(mapaAsc.indexOf(data2));
                data3_hex=mapaHex.at(mapaAsc.indexOf(data3));
                data4_hex=mapaHex.at(mapaAsc.indexOf(data4));
                value=(quint16)((data1_hex<<12)|(data2_hex<<8)|(data3_hex<<4)|(data4_hex<<0));
                //alarm1[direccionSlave-1]=(quint16) value;
                setAlarm1(direccionSlave-1,value);

                data1=datos[14];
                data2=datos[15];
                data3=datos[16];
                data4=datos[17];
                data1_hex=mapaHex.at(mapaAsc.indexOf(data1));
                data2_hex=mapaHex.at(mapaAsc.indexOf(data2));
                data3_hex=mapaHex.at(mapaAsc.indexOf(data3));
                data4_hex=mapaHex.at(mapaAsc.indexOf(data4));
                value=(quint16)((data1_hex<<12)|(data2_hex<<8)|(data3_hex<<4)|(data4_hex<<0));
                //alarm2[direccionSlave-1]=(quint16) value;
                setAlarm2(direccionSlave-1,value);

                data1=datos[18];
                data2=datos[19];
                data3=datos[20];
                data4=datos[21];
                data1_hex=mapaHex.at(mapaAsc.indexOf(data1));
                data2_hex=mapaHex.at(mapaAsc.indexOf(data2));
                data3_hex=mapaHex.at(mapaAsc.indexOf(data3));
                data4_hex=mapaHex.at(mapaAsc.indexOf(data4));
                value=(quint16)((data1_hex<<12)|(data2_hex<<8)|(data3_hex<<4)|(data4_hex<<0));
                //alarm3[direccionSlave-1]=(quint16) value;
                setAlarm2(direccionSlave-1,value);
            }
            else{
                return;
            }
            break;
    case 0x06:
        //Respuesta a un mensaje tipo "Writing registers"
        //Puede ser uno de estos
        //Escritura de setpoint de temperatura
        //Escritura de comando run/stop

        data1=datos[12];
        data2=datos[13];
        data1_hex=mapaHex.at(mapaAsc.indexOf(data1));
        data2_hex=mapaHex.at(mapaAsc.indexOf(data2));
        lrc=(unsigned char)((data1_hex<<4)|(data2_hex<<0));

        //if (lrc!=crcCalculated) return;

        data1=datos[4];
        data2=datos[5];
        data3=datos[6];
        data4=datos[7];
        if ((data1==0x30) && (data2==0x30) && (data3==0x30) && (data4==0x43)){
            //Escritura de comando run/stop
            data1=datos[8];
            data2=datos[9];
            data3=datos[10];
            data4=datos[11];
            data1_hex=mapaHex.at(mapaAsc.indexOf(data1));
            data2_hex=mapaHex.at(mapaAsc.indexOf(data2));
            data3_hex=mapaHex.at(mapaAsc.indexOf(data3));
            data4_hex=mapaHex.at(mapaAsc.indexOf(data4));
            value=(quint16)((data1_hex<<12)|(data2_hex<<8)|(data3_hex<<4)|(data4_hex<<0));
            //setMode(direccionSlave-1,(unsigned char) (data4_hex&0x01));
            m_Modes[direccionSlave-1]=(unsigned char) (data4_hex&0x01);
        }
        else if ((data1==0x30) && (data2==0x30) && (data3==0x30) && (data4==0x42)){
            //Escritura de setpoint de temperatura
            data1=datos[8];
            data2=datos[9];
            data3=datos[10];
            data4=datos[11];
            data1_hex=mapaHex.at(mapaAsc.indexOf(data1));
            data2_hex=mapaHex.at(mapaAsc.indexOf(data2));
            data3_hex=mapaHex.at(mapaAsc.indexOf(data3));
            data4_hex=mapaHex.at(mapaAsc.indexOf(data4));
            value=(quint16)((data1_hex<<12)|(data2_hex<<8)|(data3_hex<<4)|(data4_hex<<0));
            //Discharge_SetTemp[direccionSlave-1]=(quint16) value;
            setSetpoint(direccionSlave-1,value);
        }
        else{
            return;
        }
        break;

        default:
            return;
            break;
    }

    emit sglNewReceivedMessage(datos);*/


}

void In2GestorChiller::sltRunChiller(int x){
    In2SMCControl *equipo=m_equipos.at(x);
    /*int rtn=equipo->getMode();
    if (rtn==SMC_MODE_Stopped){
        equipo->runThisChiller(x);
    }*/
    equipo->runThisChiller(x);

}

void In2GestorChiller::sltStopChiller(int x){
    if (m_equipos.count()){
        In2SMCControl *equipo=m_equipos.at(x);
        equipo->stopThisChiller(x);
    }
}

void In2GestorChiller::sltOpenModbusPort(QString port){
    bool bRet=false;
    if (MiGestorModbus)
        bRet=MiGestorModbus->openModbusPort(port);
    if (bRet)
        Init();
    bool portOpen=MiGestorModbus->isOpen();
    emit sglEstadoPuertoModbus(portOpen);
}


void In2GestorChiller::sltCloseModbusPort(){
    if (MiGestorModbus)
        MiGestorModbus->CloseConnection();
    emit sglEstadoPuertoModbus(false);
}


void In2GestorChiller::sltNewDataToSend(QByteArray datos){
    if (MiGestorModbus)
        MiGestorModbus->sendModbusData(datos);
}

void In2GestorChiller::sltModbusDataSent(QByteArray data){
    emit sglNewSentMessage(data);
}

void In2GestorChiller::sltModbusDataReceived(QByteArray data){
    sltNewDataAvailable(data);
    //emit sglNewReceivedMessage(data);
}


QLabel *In2GestorChiller::ShowMensaje(QString texto){
    QLabel *InfoLabel=new QLabel(texto);
    Qt::WindowFlags flags=Qt::Window;
    QDesktopWidget d;
    InfoLabel->setWindowFlags(flags|Qt::CustomizeWindowHint);
    InfoLabel->setGeometry((d.width()-AUX_INFO_WIDTH)/2,(d.height()-AUX_INFO_HEIGHT)/2,AUX_INFO_WIDTH,AUX_INFO_HEIGHT);
    InfoLabel->setWindowOpacity(0.75);
    QFont font;
    font.setPointSize(14);
    font.setBold(true);
    InfoLabel->setFont(font);
    InfoLabel->setAlignment(Qt::AlignVCenter |Qt:: AlignHCenter);
    InfoLabel->show();
    QCoreApplication::processEvents();

    return InfoLabel;
}
