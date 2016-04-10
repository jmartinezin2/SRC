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
#include "in2gestordiametro.h"
#include "in2wenglorcontrol.h"
#include "WenglorMessage.h"
#include "qextserialport.h"
#include <QTimer>
#include <QDesktopWidget>
#include <QCoreApplication>
#include <QLabel>


//Para los carteles auxiliares
#define AUX_INFO_WIDTH 200
#define AUX_INFO_HEIGHT 75

// temperatura minima de funcionamiento
#define MIN_DIAMETER 4.0
// temperatura maxima de funcionamiento
#define MAX_DIAMETER 87.0

#define In2GD_ERROR   0
#define IN2GD_WARNING 1

//extern QList<unsigned char> mapaHex;
//extern QList<unsigned char> mapaAsc;


//Una sola instancia
//In2GestorDiametro *In2GestorDiametro::mInstance  = NULL;


/** Lllamda a instancia unica*/
/*In2GestorDiametro *In2GestorDiametro::request(QObject *parent,int equipos)
{
    if(mInstance == NULL){
        mInstance = new In2GestorDiametro(parent,equipos);
    }
    return mInstance;
}*/


/** Constructor oculto*/
In2GestorDiametro::In2GestorDiametro(QObject *parent,int equipos):
    QObject(parent)
{
    m_numEquipos=equipos;
    estadoPuerto=false;

    nElementosBuffer=3;
    calibracion_s1=0,0;
    calibracion_db1=0.0;
    calibracion_s2=0.0;
    calibracion_db2=0.0;

    MiGestorSerie = new GestorSerie();
    connect(MiGestorSerie,SIGNAL(sglDataReceived(QByteArray)),this,SLOT(sltDataReceived(QByteArray)));
    //connect(this,SIGNAL(sglDiameterSendData()),MiGestorSerie,SLOT(sltDiameterSendData()));

    estadoPuerto=MiGestorSerie->getStateConnection();

    if (estadoPuerto)
        Init();

}

/** Inicializa la fluidica*/
void In2GestorDiametro::Init(){
    if (m_numEquipos>=1){
        //Asigno IDs a los equipos conectados
        for(unsigned char i=0;i<m_numEquipos;i++){
            diametroActual.clear();
            diametroActual.resize(nElementosBuffer);
            diametroActual.fill(0x0000);
            indiceVector=0;
            In2WenglorControl *equipo=new In2WenglorControl(this,i);
            setMaxDiameter(i,0.0);
            setMinDiameter(i,1000.0);
            setCurrentChange(i,false);
            setMaxChange(i,false);
            setMinChange(i,false);
            setFilteredChange(i,false);
            m_equipos.append(equipo);
        }
        sltMonitoring();
     }

}


/** Monitoriza valores*/
void In2GestorDiametro::sltMonitoring(){
    estadoPuerto=MiGestorSerie->getStateConnection();
    if (estadoPuerto) sltGetDiameterData();
    QTimer::singleShot(TIMEOUT_MONITORING,this,SLOT(sltMonitoring()));
}

/** Coge datos de fluidica*/
void In2GestorDiametro::sltGetDiameterData(){

    for (int x=0;x<m_numEquipos;x++){

        filterCurrent(x);

        if (getCurrentChange(x)){
            setCurrentChange(x,false);
            emit sglCambioActual(x);
        }

        if (getMaxChange(x)){
            setMaxChange(x,false);
            //emit sglCambioMaximo(x);
        }

        if (getMinChange(x)){
            setMinChange(x,false);
            //emit sglCambioMinimo(x);
        }

        if (getFilteredChange(x)){
            setFilteredChange(x,false);
            //emit sglCambioFiltrado(x);
        }

    }
}


/** Devueleve el numero de equipos*/
int In2GestorDiametro::numEquipos(){
    return m_numEquipos;
}


/** Activa un flag*/
void In2GestorDiametro::SetFlag(char flag,char value){
    if (flag==FLAG_ESTADO_IN2GD){
        if (m_FlagEstado!=value){
            m_FlagEstado=value;
            emit In2GDStateNewValue(value);
        }
    }
    else if (flag==FLAG_IN2GD_OK) m_FlagIn2GDOK=value;
    else if (flag==FLAG_COMIN2GD_OK) m_FlagComunicacionIn2GDOK=value;
}

/** devuelve un flag*/
char In2GestorDiametro::GetFlag(char flag){
    char rtn=-1;
    if (flag==FLAG_ESTADO_IN2GD) rtn=m_FlagEstado;
    else if (flag==FLAG_IN2GD_OK) rtn=m_FlagIn2GDOK;
    else if (flag==FLAG_COMIN2GD_OK) rtn=m_FlagComunicacionIn2GDOK;
    return rtn;
}

double In2GestorDiametro::getCurrentDiameter(int index){
    //In2WenglorControl *equipo=m_equipos.at(index);
    //diameterCurrent_d[index]=equipo->convertDiameter2double(diameterCurrent[index]);
    return diameterCurrent_d[index];
}

double In2GestorDiametro::getMaxDiameter(int index){
    //In2WenglorControl *equipo=m_equipos.at(index);
    //diameterMax_d[index]=equipo->convertDiameter2double(diameterMax[index]);
    return diameterMax_d[index];
}

double In2GestorDiametro::getMinDiameter(int index){
    //In2WenglorControl *equipo=m_equipos.at(index);
    //diameterMin_d[index]=equipo->convertDiameter2double(diameterMin[index]);
    return diameterMin_d[index];
}

double In2GestorDiametro::getFilteredDiameter(int index){
    //In2WenglorControl *equipo=m_equipos.at(index);
    //diameterFilter_d[index]=equipo->convertDiameter2double(diameterFilter[index]);
    return diameterFilter_d[index];
}

bool In2GestorDiametro::getReady(int x){
    return readyToPrint[x];
}

void In2GestorDiametro::setCurrentDiameter(int x,double value){
    //double oldValue=diameterCurrent_d[x];
    diameterCurrent_d[x]=value;
    /*if (qAbs(oldValue-value)>0.05)*/ diameterCurrentChange[x]=true;
    //else diameterCurrentChange[x]=false;
}


void In2GestorDiametro::setMaxDiameter(int x,double value){
    double oldValue=diameterMax_d[x];
    diameterMax_d[x]=value;
    if (qAbs(oldValue-value)>0.05) diameterMaxChange[x]=true;
    else diameterMaxChange[x]=false;
}


void In2GestorDiametro::setMinDiameter(int x,double value){
    double oldValue=diameterMin_d[x];
    diameterMin_d[x]=value;
    if (qAbs(oldValue-value)>0.05) diameterMinChange[x]=true;
    else diameterMinChange[x]=false;
}


void In2GestorDiametro::setFilteredDiameter(int x,double value){
    //double oldValue=diameterFilter_d[x];
    diameterFilter_d[x]=value;
    if (diameterFilter_d[x]>diameterMax_d[x]) setMaxDiameter(x,value);
    if (diameterFilter_d[x]<diameterMin_d[x]) setMinDiameter(x,value);
    /*if (qAbs(oldValue-value)>0.05)*/ diameterFilterChange[x]=true;
   // else diameterFilterChange[x]=false;
}


void In2GestorDiametro::setReady(int x, bool value){
    bool oldValue=readyToPrint[x];
    readyToPrint[x]=value;
    if (oldValue!=value) readyChange[x]=true;
    else readyChange[x]=false;
}


bool In2GestorDiametro::getCurrentChange(int x){
    return diameterCurrentChange[x];
}

bool In2GestorDiametro::getMaxChange(int x){
    return diameterMaxChange[x];
}

bool In2GestorDiametro::getMinChange(int x){
    return diameterMinChange[x];
}

bool In2GestorDiametro::getFilteredChange(int x){
    return diameterFilterChange[x];
}

void In2GestorDiametro::setCurrentChange(int x,bool valor){
    diameterCurrentChange[x]=valor;
}

void In2GestorDiametro::setMaxChange(int x,bool valor){
    diameterMaxChange[x]=valor;
}

void In2GestorDiametro::setMinChange(int x,bool valor){
    diameterMinChange[x]=valor;
}

void In2GestorDiametro::setFilteredChange(int x,bool valor){
    diameterFilterChange[x]=valor;
}


/** Filtro de temperatura*/
void In2GestorDiametro::filterCurrent(int x){
    QString str;
    if (x<m_numEquipos){
        if ((diameterCurrent_d[x]<MIN_DIAMETER)||(diameterCurrent_d[x]>MAX_DIAMETER)){
            str=QString("Equipo %1 fuera de rango de diámetro").arg(x+1);
            setReady(x,false);
            emit In2GDError(str);
        }
        else{
            setReady(x,true);
        }
    }
}

void In2GestorDiametro::filterMax(int x){
    QString str;
    if (x<m_numEquipos){
        if ((diameterCurrent_d[x]<MIN_DIAMETER)||(diameterCurrent_d[x]>MAX_DIAMETER)){
            str=QString("Equipo %1 fuera de rango de diámetro").arg(x+1);
            setReady(x,false);
            emit In2GDError(str);
        }
        else{
            setReady(x,true);
        }
    }
}

void In2GestorDiametro::filterMin(int x){
    QString str;
    if (x<m_numEquipos){
        if ((diameterCurrent_d[x]<MIN_DIAMETER)||(diameterCurrent_d[x]>MAX_DIAMETER)){
            str=QString("Equipo %1 fuera de rango de diámetro").arg(x+1);
            setReady(x,false);
            emit In2GDError(str);
        }
        else{
            setReady(x,true);
        }
    }
}

void In2GestorDiametro::filterFilter(int x){
    QString str;
    if (x<m_numEquipos){
        if ((diameterCurrent_d[x]<MIN_DIAMETER)||(diameterCurrent_d[x]>MAX_DIAMETER)){
            str=QString("Equipo %1 fuera de rango de diámetro").arg(x+1);
            setReady(x,false);
            emit In2GDError(str);
        }
        else{
            setReady(x,true);
        }
    }
}

void In2GestorDiametro::sltDataReceived(QByteArray datos){

    QString diametrosLeidos_str;
    quint16 datoactual;
    double converted=0.0;

    //datoactual es el primer valor de todos los recibidos en cada tanda
    diametrosLeidos_str=QString(datos.left(5));
    if (diametrosLeidos_str.compare("hhhhh",Qt::CaseInsensitive)==0){
        converted=600.0;
    }
    else if (diametrosLeidos_str.compare("lllll",Qt::CaseInsensitive)==0){
        converted=6.0;
    }
    else{
        datoactual=datos.left(5).toInt();
        converted=convert2Diameter(datoactual);
    }

    setCurrentDiameter(0,converted);
    setRawDiameter(diametrosLeidos_str);


    //Metemos el ultimo valor adquirido en la posicion 0
    /*for(int i=nElementosBuffer-2;i>-1;i--){
        dummy=diametroActual.at(i);
        diametroActual.replace(i+1,dummy);
    }
    diametroActual.replace(0,converted);

    for(int i=0;i<nElementosBuffer;i++){
        dummy=diametroActual.at(i);
        suma+=dummy;
    }
    media=(double) (suma/nElementosBuffer);

    setFilteredDiameter(0,media);*/

    emit sglNewReceivedMessage(datos);


}
/** Establece el diametro raw*/
void In2GestorDiametro::setRawDiameter(QString txt){
    m_rawDiameter=txt;
}
/** Devuelve el diametro raw*/
QString In2GestorDiametro::getRawDiameter(){
    return m_rawDiameter;
}

void In2GestorDiametro::sltOpenPort(QString port){
    bool bRet;
    bRet=MiGestorSerie->openPort(port);
    if (bRet)
        Init();
    emit sglEstadoPuerto(MiGestorSerie->portOpen);
}


void In2GestorDiametro::sltClosePort(){
    MiGestorSerie->CloseConnection();
    emit sglEstadoPuerto(false);
}

void In2GestorDiametro::sltResetStatistics(int equipo){
    setMinDiameter(equipo,1000.0);
    setMaxDiameter(equipo,0.0);
}


void In2GestorDiametro::sltReadWenglorDiameter(){
    //emit sglDiameterSendData();
    QByteArray data("020D0e0C");
    WenglorMessage msg;
    QByteArray datosEnviar=msg.CreateMessageWithData(data);
    MiGestorSerie->sendData(datosEnviar);
}


/** Convierte de valor leido por puerto serie a medida de diámetro de bobina*/
double In2GestorDiametro::convert2Diameter(quint16 valorLeido){

    double Ret=0.0;

    double valorLeido_mm=0.01*valorLeido;

    Ret=calibracion_db1+(((calibracion_db2-calibracion_db1)*(valorLeido_mm-calibracion_s1))/(calibracion_s2-calibracion_s1));

    return (Ret);

}


void In2GestorDiametro::sltDatosCalibracion(double x1, double y1, double x2, double y2){

    calibracion_s1=x1;
    calibracion_db1=y1;
    calibracion_s2=x2;
    calibracion_db2=y2;

}
