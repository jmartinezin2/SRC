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
#include "in2gestorfluidica.h"
#include <QTimer>
#include <QThread>
#include "in2defines.h"

//+-Rango del menisco
#define RANGE_MENISCUS 5
// temperatura minima de funcionamiento
#define MIN_TEMPERATURE 40.0
// temperatura maxima de funcionamiento
#define MAX_TEMPERATURE 47.0
//Veces que ha de tocar la presion de menisco para lanzar alarma
#define MENISCUS_TIMES 3

//Una sola instancia
In2GestorFluidica *In2GestorFluidica::mInstance  = NULL;

/** Constructor oculto*/
In2GestorFluidica::In2GestorFluidica(QObject *parent) :
    QObject(parent)
{
    m_numEquipos=0;
    bMonitoringEnabled=false;
    m_Descripcion.clear();
    SetFlag(FLAG_ESTADO_FLUIDICA,THEME_FLUID_WARNING);
    SetFlag(FLAG_FLUIDICA_OK,0);
    SetFlag(FLAG_COMFLUID_OK,0);
    //Inicializamos a stop
    memset(&m_Modes,XCL_MODE_Stopped,sizeof(m_Modes));
    memset(&m_MeniscusCounter,0x00,sizeof(m_MeniscusCounter));
    memset(&m_MeniscusRangeFault,0x00,sizeof(m_MeniscusRangeFault));
    memset(&m_MeniscusFault,0x00,sizeof(m_MeniscusFault));
    memset(&m_supplyOffset,0x00,sizeof(m_supplyOffset));
    memset(&m_returnOffset,0x00,sizeof(m_returnOffset));

    //Filtro de nivel
    for (int x=0;x<MAXHYDRAS;x++){
        memset(&m_FiltroNivel[x].level,0xFF,sizeof(m_FiltroNivel[x].level));
        m_FiltroNivel[x].index=0;
    }

}
DWORD In2GestorFluidica::sltGetCurrentThread(){
    DWORD id=(DWORD)QThread::currentThreadId();
    emit SendDebugInfo(LOGGER_ERROR,QString("Thread Fluidica ID:%1").arg(id));
    return id;
}



/** Devueleve el numero de equipos*/
int In2GestorFluidica::numEquipos(){
    return m_numEquipos;
}

/** Lllamda a instancia unica*/
In2GestorFluidica *In2GestorFluidica::request(QObject *parent)
{
    if(mInstance == NULL){
        mInstance = new In2GestorFluidica(parent);
    }
    return mInstance;
}
/** Activa un flag*/
void In2GestorFluidica::SetFlag(char flag,char value){
    if (flag==FLAG_ESTADO_FLUIDICA){
        if (m_FlagEstado!=value){
            m_FlagEstado=value;
            emit sgnChangeState(int(m_FlagEstado));
            //emit FluidStateNewValue(value);
        }
    }
    else if (flag==FLAG_FLUIDICA_OK) m_FlagFluidOK=value;
    else if (flag==FLAG_COMFLUID_OK) m_FlagComunicacionFluidOK=value;
}
/** devuelve un flag*/
char In2GestorFluidica::GetFlag(char flag){
    char rtn=-1;
    if (flag==FLAG_ESTADO_FLUIDICA) rtn=m_FlagEstado;
    else if (flag==FLAG_FLUIDICA_OK) rtn=m_FlagFluidOK;
    else if (flag==FLAG_COMFLUID_OK) rtn=m_FlagComunicacionFluidOK;
    return rtn;
}

/** Devuelve la descripcion de un equipo*/
QString In2GestorFluidica::getDescription(int index){
    QString name;
    if (index<m_equipos.count()){
        In2HydraControl *equipo=m_equipos.at(index);
        name=equipo->ReadDescription();
    }
    return name;
}

/** Lee PID de presion*/
int In2GestorFluidica::readPIDPresure(int x){
    int rtn=0;
    if (x<m_numEquipos){
        In2HydraControl *equipo=m_equipos.at(x);
        rtn=equipo->ReadPid(XCL_PID_Press,&XCLPidPresure[x]);
    }
    return rtn;
}
/** Calcula el setpoint de Menisco*/
void In2GestorFluidica::CalculateMeniscusSetPoint(int x){

    XCL_PID XCLPidPresure;
    if (x<m_numEquipos){
        In2HydraControl *equipo=m_equipos.at(x);
        equipo->ReadPid(XCL_PID_Vac,&XCLPidPresure);
        m_MeniscusSetPoint[x]=XCLPidPresure.setp1;
    }

}

/** Actualiza la info*/
void In2GestorFluidica::sltUpdateInfo(){
    emit sgnChangeState(int(m_FlagEstado));
}


/** Inicializa la fluidica*/
void In2GestorFluidica::Init(){
    short Founded=0;
    XCL_Initialise (&Founded);
    m_numEquipos=Founded;
    if (m_numEquipos>=1){
        //Asigno IDs a los equipos conectados
        for(unsigned char i=0;i<m_numEquipos;i++){
            In2HydraControl *equipo=new In2HydraControl(this,i);
            int rtn=equipo->ReadFcoeff(CoeficientesF[i]);
            if (rtn!=XCL_NO_ERROR) emit SendDebugInfo(LOGGER_ERROR,QString("Init Hydra %1 error %2").arg(i).arg(rtn));
            rtn=equipo->ReadIcoeff(CoeficientesI[i]);
            if (rtn!=XCL_NO_ERROR) emit SendDebugInfo(LOGGER_ERROR,QString("Init Hydra %1 error %2").arg(i).arg(rtn));
            m_equipos.append(equipo);
            m_supplyOffset[i]=CoeficientesF[i][MSP_HEAD1];
            m_returnOffset[i]=CoeficientesF[i][MSP_HEAD2];
            CalculateMeniscusSetPoint(i);
        }
        //SetFlag(FLAG_ESTADO_FLUIDICA,1);
        SetFlag(FLAG_FLUIDICA_OK,1); //Hemos encontrado equipos y se ha inicializado
     }
     else{
        SetFlag(FLAG_ESTADO_FLUIDICA,THEME_FLUID_WARNING);
        SetFlag(FLAG_FLUIDICA_OK,0);
        SetFlag(FLAG_COMFLUID_OK,0);
    }
}
/** Devuelve descripcion de  estado*/
QString In2GestorFluidica::getModeDescription(int index){
    QString state;
    if (index<m_numEquipos){
       In2HydraControl *equipo=m_equipos.at(index);
       XCL_Modes rtn=equipo->getMode();
       if (rtn==XCL_MODE_Stopped) state=QString("Stopped");
       else if (rtn==XCL_MODE_Filling) state=QString("Filling");
       else if (rtn==XCL_MODE_Starting_MP) state=QString("Starting MP");
       else if (rtn==XCL_MODE_Starting_DP) state=QString("Starting DP");
       else if (rtn==XCL_MODE_Starting_T) state=QString("Starting T");
       else if (rtn==XCL_MODE_Started) state=QString("Started");
       else if (rtn==XCL_MODE_Idling) state=QString("Idling");
       else if (rtn==XCL_MODE_Stopping_T) state=QString("Stopping T");
       else if (rtn==XCL_MODE_Stopping_DP) state=QString("Stopping DP");
       else if (rtn==XCL_MODE_Stopping_MP) state=QString("Stopping MP");
       else if (rtn==XCL_MODE_Venting) state=QString("Venting");
    }
    return state;
}
/** Devuelve estado*/
XCL_Modes In2GestorFluidica::getMode(int index){
    XCL_Modes rtn=XCL_MODE_Stopped;
    if (index<m_numEquipos){
       In2HydraControl *equipo=m_equipos.at(index);
       rtn=equipo->getMode();
    }
    return rtn;

}

/** Inicializa un equipo*/
void In2GestorFluidica::InitEquipo(int x){
 if (x<m_numEquipos){
    In2HydraControl *equipo=m_equipos.at(x);
    if (equipo->getMode()==XCL_MODE_Stopped){
        equipo->setAutoStart(true);
        equipo->SendCommand(XCL_CMD_Start);
        equipo->setUsed(true);
        equipo->setFlag(FLAG_ERROR_INICIANDO,0);
     }
  }
}

/** Para un equipo*/
void In2GestorFluidica::StopEquipo(int x){
 if (x<m_numEquipos){
    In2HydraControl *equipo=m_equipos.at(x);
    if (equipo->getMode()!=XCL_MODE_Stopped){
        equipo->SendCommand(XCL_CMD_Stop);
        equipo->setUsed(false);
        equipo->setFlag(FLAG_ERROR_INICIANDO,0);
     }
  }
}
/** Hace el autoShudown*/
void In2GestorFluidica::sltAutoShutdown(){
    MemoryMap *memoryMap=MemoryMap::request(0);
    for (int x=0;x<m_numEquipos;x++){
        if (memoryMap->m_mapa.m_maquina.m_fluidica.bAutoShutdown[x]) ShutdownEquipo(x);
    }
}


/** Shutdown un equipo*/
void In2GestorFluidica::ShutdownEquipo(int x){
 if (x<m_numEquipos){
    In2HydraControl *equipo=m_equipos.at(x);
    equipo->SendCommand(XCL_CMD_Shutdown);
    equipo->setUsed(false);
    equipo->setFlag(FLAG_ERROR_INICIANDO,0);
     }
}

/** Apaga el sistema completamente*/
void In2GestorFluidica::CompleteShutdown(){
    for (int x=0;x<m_numEquipos;x++){
        ShutdownEquipo(x);
    }
    QTimer::singleShot(0,this,SLOT(sltWaitForCompleteShutdown()));
}

/** Espera a que todo el sistema se apague*/
void In2GestorFluidica::sltWaitForCompleteShutdown(){
    bool bret=true;
    for (int x=0;x<m_numEquipos;x++){
        In2HydraControl *equipo=m_equipos.at(x);
        if (equipo->getMode()!=XCL_MODE_Stopped){
            bret=false;
            break;
        }
    }
    //Todavia no ha terminado
    if (!bret){
        QTimer::singleShot(1000,this,SLOT(sltWaitForCompleteShutdown()));
    }//Ya ha terminado...informo de ello
    else{
        stopMonitoring();
        emit CompleteShutdownFinished();
    }
}

/** Error de control*/
void In2GestorFluidica::sltError(int val){
    sltSetNumEquipos(val);
}

/** Reboot un equipo*/
void In2GestorFluidica::RebootEquipo(int x){
 if (x<m_numEquipos){
    In2HydraControl *equipo=m_equipos.at(x);
        equipo->SendCommand(XCL_CMD_Reboot);
        equipo->setUsed(true);
        equipo->setFlag(FLAG_ERROR_INICIANDO,0);
     }
}
/** Inicializa la monitorizacion*/
void In2GestorFluidica::startMonitoring(){
    bMonitoringEnabled=true;
    sltMonitoring();
    /*while (1){
    }*/

}
/** Para la monitorizacion*/
void In2GestorFluidica::stopMonitoring(){
    bMonitoringEnabled=false;
}

/** Monitoriza valores*/
void In2GestorFluidica::sltMonitoring(){
    if (bMonitoringEnabled){
        sltGetFluidicData();
        QTimer::singleShot(FLUID_TIMEOUT_MONITORING,this,SLOT(sltMonitoring()));
        //emit SendDebugInfo(LOGGER_WARNING,QString("Monitoring %1").arg((int)QThread::currentThreadId()));
    }
}

/** Coge datos de fluidica*/
void In2GestorFluidica::sltGetFluidicData(){
    int rtn=0;
    for (int x=0;x<m_numEquipos;x++){
        In2HydraControl *equipo=m_equipos.at(x);
        rtn=equipo->ReadSensor(XCL_SENS_Supply_Temp,&ADC_Supply_Temp[x]);
        if (rtn!=XCL_NO_ERROR)emit SendDebugInfo(LOGGER_ERROR,QString("Supply Temp Hydra %1 error %2").arg(x).arg(rtn));
        rtn=equipo->ReadSensor(XCL_SENS_Level_Res,&ADC_Level_Res[x]);
        if (rtn!=XCL_NO_ERROR)emit SendDebugInfo(LOGGER_ERROR,QString("Level_Res Hydra %1 error %2").arg(x).arg(rtn));
        rtn=equipo->ReadSensor(XCL_SENS_Return_Pr,&ADC_Return_Pr[x]);
        if (rtn!=XCL_NO_ERROR)emit SendDebugInfo(LOGGER_ERROR,QString("Return_Pr Hydra %1 error %2").arg(x).arg(rtn));
        rtn=equipo->ReadSensor(XCL_SENS_Supply_Pr,&ADC_Supply_Pr[x]);
        if (rtn!=XCL_NO_ERROR)emit SendDebugInfo(LOGGER_ERROR,QString("Supply_Pr Hydra %1 error %2").arg(x).arg(rtn));
        rtn=equipo->ReadSensor(XCL_SENS_Return_Temp,&ADC_Return_Temp[x]);
        if (rtn!=XCL_NO_ERROR)emit SendDebugInfo(LOGGER_ERROR,QString("Return_temp Hydra %1 error %2").arg(x).arg(rtn));
        rtn=equipo->ReadSensor(XCL_SENS_Res_Temp,&ADC_Res_Temp[x]);
        if (rtn!=XCL_NO_ERROR)emit SendDebugInfo(LOGGER_ERROR,QString("Res_temp Hydra %1 error %2").arg(x).arg(rtn));
        rtn=equipo->ReadSwitches(&switches[x]);
        if (rtn!=XCL_NO_ERROR)emit SendDebugInfo(LOGGER_ERROR,QString("Switches Hydra %1 error %2").arg(x).arg(rtn));
        rtn=equipo->ReadLEDS(&bits[x],&faults[x]);
        if (rtn!=XCL_NO_ERROR)emit SendDebugInfo(LOGGER_ERROR,QString("LEDS Hydra %1 error %2").arg(x).arg(rtn));
        m_PreDif[x]=ADC_Supply_Pr[x].val-ADC_Return_Pr[x].val;
        m_PreMeniscus[x]=((ADC_Supply_Pr[x].val+ADC_Return_Pr[x].val)/2)+((m_supplyOffset[x]+m_returnOffset[x])/2);
        //if (equipo->getUsed()){
            filterCambioEstado(x);
            filterBits(x);
            filterFaults(x);
            filterSwitches(x);
            filterMeniscusPositive(x);
            filterMeniscusRange(x);
            filterTemperatura(x);
            filterNivel(x);
        //}
    }
}
/** Cambia el numero de equipos*/
void In2GestorFluidica::sltSetNumEquipos(int val){
    m_numEquipos=val;
}

/** Devuelve la presion diferencial*/
float In2GestorFluidica::getDiferentialPresure(int index){
    return m_PreDif[index];
}
/** Devuelve la presion de menisco*/
float In2GestorFluidica::getMeniscusPresure(int index){
    return m_PreMeniscus[index];
}


/** Devuelve el supplyTemp del equipo indicado*/
XCL_ADC In2GestorFluidica::getSupplyTemp(int x){
    return  ADC_Supply_Temp[x];
}
/** Devuelve el Levelres del equipo indicado*/
XCL_ADC In2GestorFluidica::getLevelRes(int x){
    return  ADC_Level_Res[x];
}
/** Devuelve el returnPr del equipo indicado*/
XCL_ADC In2GestorFluidica::getReturnPre(int x){
    return  ADC_Return_Pr[x];
}
/** Devuelve el supplyPr del equipo indicado*/
XCL_ADC In2GestorFluidica::getSupplyPre(int x){
    return  ADC_Supply_Pr[x];
}
/** Devuelve el returnTemp del equipo indicado*/
XCL_ADC In2GestorFluidica::getReturnTemp(int x){
    return  ADC_Return_Temp[x];
}
/** Devuelve el returnTemp del equipo indicado*/
XCL_ADC In2GestorFluidica::getResTemp(int x){
    return  ADC_Res_Temp[x];
}
/** Devuelve el switches del equipo indicado*/
short In2GestorFluidica::getSwitches(int x){
    return  switches[x];
}
/** Devuelve el switches del equipo indicado*/
short In2GestorFluidica::getBits(int x){
    return  bits[x];
}
/** Devuelve el switches del equipo indicado*/
short In2GestorFluidica::getFaults(int x){
    return  faults[x];
}
/** Devuele descripcion del switch*/
QString In2GestorFluidica::getSwitchesDescription(int x){
    short localSwitch=switches[x];
    QString rtn;
    if (localSwitch&XCL_SW_Supp1) rtn=QString("Fill:Error Supply 1");
    else if (localSwitch&XCL_SW_Supp2) rtn=QString("Fill:Error Supply 2");
    else if (localSwitch&XCL_SW_Ret1) rtn=QString("Fill:Error Ret 1");
    else if (localSwitch&XCL_SW_Ret2) rtn=QString("Fill:Error Ret 2");
    else if (localSwitch&XCL_SW_Start) rtn=QString("Fill:Start");
    else if (localSwitch&XCL_SW_Stop) rtn=QString("Fill:Stop");
    else if (localSwitch&XCL_SW_Shutdn)rtn=QString("Fill:Shutdown");
    else if (localSwitch&XCL_SW_Fill) rtn=QString("Fill OK");
    else if (localSwitch&XCL_SW_UPS) rtn=QString("Fill:UPS");
    else rtn=QString("Fill:Unknown");
    return rtn;
}

/** Devuelve descripcion de fallo*/
QString In2GestorFluidica::getFaultsDescription(int x){
    short LocalFault=faults[x];
    QString rtn;
    if (!LocalFault) rtn=QString("No Fault");
    else{
        if (LocalFault&XCL_FLT_Sens) rtn.append(QString("Mask for last sensor in error"));
        if (LocalFault&XCL_FLT_FLASH) rtn.append(QString("FLASH error"));
        if (LocalFault&XCL_FLT_NOIMP1) rtn.append(QString("Not implemented 1"));
        if (LocalFault&XCL_FLT_TACHO_BOUNDS) rtn.append(QString("Tacho bounds exceed"));
        if (LocalFault&XCL_FLT_ESTOP) rtn.append(QString("Stop commanded"));
        if (LocalFault&XCL_FLT_OVER_TEMP) rtn.append(QString("Over temperature"));
        if (LocalFault&XCL_FLT_NOIMP2) rtn.append(QString("Not implemented 2"));
        if (LocalFault&XCL_FLT_NO_INK) rtn.append(QString("No ink"));
        if (LocalFault&XCL_FLT_XS_INK) rtn.append(QString("xs ink"));
        if (LocalFault&XCL_FLT_PUMP_STALL) rtn.append(QString("Pump stalled during operating cycle"));
    }
    return rtn;
}
/** Descripcion de bits*/
QString In2GestorFluidica::getBitsDescription(int x){
    short localBit=bits[x];
    QString rtn="";
    if (localBit&XCL_LED_Warning) rtn=QString("WARNING");
    else if (localBit&XCL_LED_Ready) rtn=QString("READY");
    else if (localBit&XCL_LED_Fault) rtn=QString("FAULT");
    else if (localBit&XCL_LED_Spare) rtn=QString("SPARE");
    else if (localBit&XCL_LED_Heat) rtn=QString("HEAT");
    else if (localBit&XCL_LED_Valve) rtn=QString("VALVE");
    return rtn;
}

/** Devueleve los coeficientes F*/
float * In2GestorFluidica::getCoeffF(int x){
    return this->CoeficientesF[x];
}
/** Devueleve los coeficientes I*/
short * In2GestorFluidica::getCoeffI(int x){
    return this->CoeficientesI[x];
}
/** Offset de suminidtro*/
float In2GestorFluidica::getSupplyOffset(int index){
    return m_supplyOffset[index];
}
/** Offset de retorno*/
float In2GestorFluidica::getReturnOffset(int index){
    return m_returnOffset[index];
}
/** Color de un equipo*/
QColor In2GestorFluidica::getColor(int index){
    QColor color=QColor(Qt::white);
    if (index<m_numEquipos){
        In2HydraControl *equipo=m_equipos.at(index);
        color=equipo->getColor();
    }
    return color;
}
/** Filtro de bits*/
void In2GestorFluidica::filterBits(int x){
    short localBits=bits[x];
    if ((localBits&XCL_LED_Warning)||(localBits&XCL_LED_Ready));
    else emit HydraError(HYDRAERROR_BITS_ERROR,x,(float)localBits);
}

/** Filtro faults*/
void In2GestorFluidica::filterFaults(int x){
    short LocalFault=faults[x];
    if (LocalFault) emit HydraError(HYDRAERROR_FAULTS_ERROR,x,(float)LocalFault);
}

/** Filtro switches*/
void In2GestorFluidica::filterSwitches(int x){
    short LocalFault=switches[x];
    if (!LocalFault&XCL_SW_Fill) emit HydraError(HYDRAERROR_SWITCHES_ERROR,x,(float)LocalFault);
}


/** Filtro de menisco positivo*/
void In2GestorFluidica::filterMeniscusPositive(int x){
    if (x<m_numEquipos){
        if (this->m_PreMeniscus[x]>0){
            m_MeniscusCounter[x]++;
            if (m_MeniscusCounter[x]>=MENISCUS_TIMES){
                emit HydraError(HYDRAERROR_MENISCUS_POSITIVE,x,m_PreMeniscus[x]);
                m_MeniscusCounter[x]=1;
                m_MeniscusFault[x]=1;
            }
        }
        else{
            m_MeniscusCounter[x]=0;
            m_MeniscusFault[x]=0;
        }
    }
}
/** Filtro de menisco fuera de rango*/
void In2GestorFluidica::filterMeniscusRange(int x){
    if (x<m_numEquipos){
        //In2HydraControl *equipo=m_equipos.at(x);
        if (m_Modes[x]==XCL_MODE_Started){
            if ((m_PreMeniscus[x]<m_MeniscusSetPoint[x]-RANGE_MENISCUS)||(m_PreMeniscus[x]>m_MeniscusSetPoint[x]+RANGE_MENISCUS)){
                m_MeniscusRangeFault[x]=1;
                emit HydraError(HYDRAERROR_MENISCUS_RANGE,x,m_PreMeniscus[x]);
            }
            else{
                m_MeniscusRangeFault[x]=0;
            }
        }
    }
}
/** Filtro del nivel del hydra*/
void In2GestorFluidica::filterNivel(int x){
    int index=m_FiltroNivel[x].index%COUNTER;
    m_FiltroNivel[x].level[index]=ADC_Level_Res[x].yn;
    if (index==COUNTER-1){
        float foo=0;
        for (int y=0;y<COUNTER;y++) foo+=m_FiltroNivel[y].level[index];
        foo=foo/COUNTER;
        if (foo<1) emit HydraError(HYDRAERROR_LOW_LEVEL,x,foo);
    }
    m_FiltroNivel[x].index=index+1;
}

/** Filtro de cambio de estado*/
void In2GestorFluidica::filterCambioEstado(int x){
    if (x<m_numEquipos){
        In2HydraControl *equipo=m_equipos.at(x);
        XCL_Modes tmp=equipo->getMode();
        if (tmp!=m_Modes[x]){
            emit HydraError(HYDRAERROR_STATECHANGE_ERROR,x,(float)tmp);
            m_Modes[x]=tmp;
            sltTestAllDevicesState();
        }
    }
}

/** Filtro de temperatura*/
void In2GestorFluidica::filterTemperatura(int x){
    if (x<m_numEquipos){
        if (m_Modes[x]==XCL_MODE_Started){
            if ((ADC_Supply_Temp[x].val<MIN_TEMPERATURE)||(ADC_Supply_Temp[x].val>MAX_TEMPERATURE)){
                emit HydraError(HYDRAERROR_TEMPERATURE_ERROR,x,(float)ADC_Supply_Temp[x].val);
            }
        }
    }
}
/** Indica si un equipo en particular puede imprimir*/
bool In2GestorFluidica::deviceReadyToPrint(int index){
    bool rtn=true;
    MemoryMap *memoryMap=MemoryMap::request(0);
    //Miramos que no esté forzado
    if (memoryMap->m_mapa.m_maquina.m_fluidica.Used[index]==false){
        if (m_Modes[index]!=XCL_MODE_Started) rtn=false;
    }
    /*else{ //Si está forzado, que no tenga errores
        if ((faults[index]!=0)||(m_MeniscusRangeFault[index]!=0)||(m_MeniscusFault[index])) rtn=false;
    }*/



    return rtn;
}

/** Devuelve si las fluidicas estan ready*/
void In2GestorFluidica::sltTestAllDevicesState(){
    char state=THEME_FLUID_OK;
    //Revisa todos los equipos para avisar del cmabio
    for (int i=0;i<m_numEquipos;i++){
        if (!deviceReadyToPrint(i)){
            state=THEME_FLUID_WARNING;
            break;
        }
    }
    SetFlag(FLAG_ESTADO_FLUIDICA,state);
}

