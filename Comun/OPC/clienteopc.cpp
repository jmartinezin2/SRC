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
#include "clienteopc.h"
#include "MemoryMap.h"
#include "in2defines.h"
//#include "principal.h"
#include <QDebug>
#include <QMessageBox>
#include <QThread>
#include <QTimer>
#include <QMutex>
#include "in2configuratordiameter.h"
#include "in2managerdiametros.h"


#define TIME_MONITOR 1000 //Timeout para leer items OPC
#define TIME_MONITOR_LONG 5000

//Lo haremos singleton
ClienteOPC *ClienteOPC::mInstance  = NULL;



/** Singleton*/
ClienteOPC *ClienteOPC::request(QObject *parent){
    if(mInstance == NULL){
        mInstance = new ClienteOPC(parent);
    }
    return mInstance;

}
ClienteOPC::ClienteOPC(QObject *parent) :
    QObject(parent)
{
    m_bInitiated=false;
    m_memoryMap=MemoryMap::request(0);
    m_PendingItems.clear();
    m_PendingValues.clear();
    m_enabled=true;
    m_bPLCSensorDiameterIN=false;
    m_bPLCSensorDiameterOUT=false;

    In2ConfiguratorDiameter configDiamIn(IN2MANAGER_DIN);
    bool monitor=configDiamIn.getEnabled();
    if (!monitor) m_bPLCSensorDiameterIN=configDiamIn.getPLCEnabled();

    In2ConfiguratorDiameter configDiamOut(IN2MANAGER_DOUT);
    monitor=configDiamOut.getEnabled();
    if (!monitor) m_bPLCSensorDiameterOUT=configDiamOut.getPLCEnabled();
}
/** Inicializa el cliente OPC*/
void ClienteOPC::Init(){
    /*DWORD *dummy;
    *dummy=1000;*/


    m_Monitor=false;
    QString nombre_servidor=m_memoryMap->m_mapa.m_maquina.m_opc.Nombre;
    QString group_name = QString("Bobinado");

    MiServidorOPC = new CemitecObjectOPC();
    //Inicializamos siempre, pero testeamos si hay que añadir algo
    if (m_memoryMap->m_mapa.m_maquina.m_opc.estado==1){
        QVariant x=MiServidorOPC->conectaServidor(nombre_servidor);

        connect(MiServidorOPC,SIGNAL(exception(int,QString,QString,QString)),this,SLOT(debugDLL(int,QString,QString,QString)));
        emit SendDebugInfo(LOGGER_WARNING, QString("CONEXION Servidor OPC %1 (%2)").arg(nombre_servidor).arg(x.toInt()));

        x=MiServidorOPC->adicionaGrupo(group_name);
        emit SendDebugInfo(LOGGER_WARNING, QString("Grupo %1 (%2)").arg(group_name).arg(x.toInt()));

        contador_client_handle=0;

        InicializarItemsExt1();

        //Comienza a monitorizar
        sltEnableMonitoring(true);
    }
    //QTimer::singleShot(0,this,SLOT(slTTestThreadID()));
}

void ClienteOPC::desconectaServidor(){
    MiServidorOPC->desconectaServidor();
}

/** test de ID*/
void ClienteOPC::slTTestThreadID(){
    DWORD id=(DWORD)QThread::currentThreadId();
    emit SendDebugInfo(LOGGER_ERROR,QString("Thread ClienteOPC ID:%1").arg(id));
    QTimer::singleShot(5000,this,SLOT(slTTestThreadID()));
}

/** Escritura demorada en OPC*/
void ClienteOPC::timedEscribeDato(int referenciaItem,QVariant valor){
    m_PendingItems.append(referenciaItem);
    m_PendingValues.append(valor);
    QTimer::singleShot(DELAY_OPC_SIGNAL,this,SLOT(sltUpdatePendingValue()));
    //QTimer::singleShot(msg,this,SLOT(EscribirDato(referenciaItem,valor)));
}

/** Escritura demorada en OPC*/
void ClienteOPC::timedExtEscribeDato(int delay,int referenciaItem,QVariant valor){
    m_PendingItems.append(referenciaItem);
    m_PendingValues.append(valor);
    QTimer::singleShot(delay,this,SLOT(sltUpdatePendingValue()));
    //QTimer::singleShot(msg,this,SLOT(EscribirDato(referenciaItem,valor)));
}
/** Habilita/Deshabilita*/
void ClienteOPC::setEnabled(bool val){
    m_enabled=val;
}

/** Actualiza el dato*/
void ClienteOPC::sltUpdatePendingValue(){
    if ((!m_PendingItems.isEmpty())&&(!m_PendingValues.isEmpty())){
        int item=m_PendingItems.first();
        QVariant value=m_PendingValues.first();
        m_PendingItems.remove(0);
        m_PendingValues.remove(0);
        EscribirDato(item,value);
    }
    else emit SendDebugInfo(LOGGER_WARNING,QString("SYNC error: No OPC pending values"));
}
/** Emite info de debug*/
void ClienteOPC::debugDLL(int a, QString b, QString c, QString d){
    this->deleteLater();
    QString dummy=QString("[%1]-[%2]-[%3]-[%4]").arg((unsigned)a).arg(b).arg(c).arg(d);
    emit SendDebugInfo(LOGGER_WARNING, dummy);
}

ClienteOPC::~ClienteOPC(){

}
/** Craga nuevo item*/
void ClienteOPC::setNewItem(struct OPC_Item *p,QString variable,QString name){
    static int contador=0;
    p->client_handle=contador;
    p->ID=variable;
    p->name=name;
    int rtn=MiServidorOPC->adicionaItem(p->ID);
    emit SendDebugInfo(LOGGER_WARNING, QString("[%1][%2][%3](%4)").arg(contador).arg(p->name).arg(p->ID).arg(rtn));
    if (rtn==contador) contador++;
}
/** Inicializa los items*/
void ClienteOPC::InicializarItems(){

    setNewItem(&OPC_Bobinadora->VelocidadActual,ID_VELOCIDAD_ACT,STRING_ITEM_VELOCIDAD_ACT);
    setNewItem(&OPC_Bobinadora->VelocidadRef,ID_VELOCIDAD_REF,STRING_ITEM_VELOCIDAD_REF);
    setNewItem(&OPC_Bobinadora->LongitudActual,ID_METROS_ACT,STRING_ITEM_METROS_ACT);
    setNewItem(&OPC_Bobinadora->LongitudRef,ID_METROS_REF,STRING_ITEM_METROS_REF);
    setNewItem(&OPC_Bobinadora->CondicionesImpresion,ID_CONDICIONES_IMPRESION,STRING_ITEM_CONDICIONES_IMPRESION);
    setNewItem(&OPC_Bobinadora->TrabajoNuevo,ID_TRABAJO_NUEVO,STRING_ITEM_TRABAJO_NUEVO);
    setNewItem(&OPC_Bobinadora->CondicionesMovimiento,ID_CONDICIONES_MOVIMIENTO,STRING_ITEM_CONDICIONES_MOVIMIENTO);
    setNewItem(&OPC_Bobinadora->DiametroActual,ID_DIAMETRO_ENTRADA_ACT,STRING_ITEM_DIAMETRO_ENTRADA_ACT);
    setNewItem(&OPC_Bobinadora->DiametroRef,ID_DIAMETRO_ENTRADA_REF,STRING_ITEM_DIAMETRO_ENTRADA_REF);
    setNewItem(&OPC_Bobinadora->CodigoParada,ID_CODIGO_PARADA,STRING_ITEM_CODIGO_PARADA);
    setNewItem(&OPC_Bobinadora->PermisoParar,ID_PERMISO_PARAR,STRING_ITEM_PERMISO_PARAR);
    setNewItem(&OPC_Bobinadora->DiametroParar,ID_DIAMETRO_PARAR,STRING_ITEM_DIAMETRO_PARAR);
    setNewItem(&OPC_Bobinadora->FijarDiametro,ID_FIJAR_DIAMETRO_ENTRADA,STRING_ITEM_FIJAR_DIAMETRO_ENTRADA);
    //m_CondicionesImpresion_ch_v=QVariant(OPC_Bobinadora->CondicionesImpresion.client_handle);

}

/** Inicializa los items de maquina=2*/
void ClienteOPC::InicializarItemsExt1(){

    /*int wait=DELAY_OPC;
    int rtn=0;
    QString dummyString;*/
    
    setNewItem(&OPC_Bobinadora_Ext1->VelocidadActual,ID_VELOCIDAD_ACT_EXT,STRING_ITEM_VELOCIDAD_ACT);
    setNewItem(&OPC_Bobinadora_Ext1->VelocidadRef,ID_VELOCIDAD_REF_EXT,STRING_ITEM_VELOCIDAD_REF);
    setNewItem(&OPC_Bobinadora_Ext1->LongitudActual,ID_METROS_ACT_EXT,STRING_ITEM_METROS_ACT);
    setNewItem(&OPC_Bobinadora_Ext1->LongitudRef,ID_METROS_REF_EXT,STRING_ITEM_METROS_REF);
    setNewItem(&OPC_Bobinadora_Ext1->CondicionesImpresion,ID_CONDICIONES_IMPRESION_EXT,STRING_ITEM_CONDICIONES_IMPRESION);
    setNewItem(&OPC_Bobinadora_Ext1->TrabajoNuevo,ID_TRABAJO_NUEVO_EXT,STRING_ITEM_TRABAJO_NUEVO);
    setNewItem(&OPC_Bobinadora_Ext1->CondicionesMovimiento,ID_CONDICIONES_MOVIMIENTO_EXT,STRING_ITEM_CONDICIONES_MOVIMIENTO);

    /*QVariant valor=QVariant(1);
    MiServidorOPC->escribeDato(OPC_Bobinadora_Ext1->CondicionesMovimiento.client_handle,valor);*/

    setNewItem(&OPC_Bobinadora_Ext1->DiametroEntradaActual,ID_DIAMETRO_ENTRADA_ACT_EXT,STRING_ITEM_DIAMETRO_ENTRADA_ACT);
    setNewItem(&OPC_Bobinadora_Ext1->DiametroEntradaRef,ID_DIAMETRO_ENTRADA_REF_EXT,STRING_ITEM_DIAMETRO_ENTRADA_REF);
    setNewItem(&OPC_Bobinadora_Ext1->DiametroSalidaActual,ID_DIAMETRO_SALIDA_ACT_EXT,STRING_ITEM_DIAMETRO_SALIDA_ACT);
    setNewItem(&OPC_Bobinadora_Ext1->DiametroSalidaRef,ID_DIAMETRO_SALIDA_REF_EXT,STRING_ITEM_DIAMETRO_SALIDA_REF);
    setNewItem(&OPC_Bobinadora_Ext1->CodigoParada,ID_CODIGO_PARADA_EXT,STRING_ITEM_CODIGO_PARADA);
    setNewItem(&OPC_Bobinadora_Ext1->DiametroParar,ID_DIAMETRO_PARAR_EXT,STRING_ITEM_DIAMETRO_PARAR);
    setNewItem(&OPC_Bobinadora_Ext1->FijarDiametroEntrada,ID_FIJAR_DIAMETRO_ENTRADA_EXT,STRING_ITEM_FIJAR_DIAMETRO_ENTRADA);
    setNewItem(&OPC_Bobinadora_Ext1->FijarDiametroSalida,ID_FIJAR_DIAMETRO_SALIDA_EXT,STRING_ITEM_FIJAR_DIAMETRO_SALIDA);
    setNewItem(&OPC_Bobinadora_Ext1->ParoMaquina,ID_PARO_MAQUINA_EXT,STRING_ITEM_PARO_MAQUINA);
    setNewItem(&OPC_Bobinadora_Ext1->MarchaMovimiento,ID_MARCHA_MOVIMIENTO_EXT,STRING_ITEM_MARCHA_MOVIMIENTO);
    setNewItem(&OPC_Bobinadora_Ext1->ParoMovimiento,ID_PARO_MOVIMIENTO_EXT,STRING_ITEM_PARO_MOVIMIENTO);
    setNewItem(&OPC_Bobinadora_Ext1->SecuenciaOn,ID_SECUENCIA_ON_EXT,STRING_ITEM_SECUENCIA_ON);
    setNewItem(&OPC_Bobinadora_Ext1->DetectorMacula,ID_DETECTOR_MACULA_EXT,STRING_ITEM_DETECTOR_MACULA);
    setNewItem(&OPC_Bobinadora_Ext1->JogMas,ID_JOG_MAS_EXT,STRING_ITEM_JOG_MAS);
    setNewItem(&OPC_Bobinadora_Ext1->JogMenos,ID_JOG_MENOS_EXT,STRING_ITEM_JOG_MENOS);
    setNewItem(&OPC_Bobinadora_Ext1->BuscarMacula,ID_BUSCAR_MACULA_EXT,STRING_ITEM_BUSCAR_MACULA);
    setNewItem(&OPC_Bobinadora_Ext1->DeshabilitarServoEntrada,ID_DESHABILITAR_SERVO_ENTRADA_EXT,STRING_ITEM_DESHABILITAR_SERVO_ENTRADA);
    setNewItem(&OPC_Bobinadora_Ext1->DeshabilitarServoSalida,ID_DESHABILITAR_SERVO_SALIDA_EXT,STRING_ITEM_DESHABILITAR_SERVO_SALIDA);
    setNewItem(&OPC_Bobinadora_Ext1->DesplazarFilm,ID_DESPLAZAR_FILM_EXT,STRING_ITEM_DESPLAZAR_FILM);
    setNewItem(&OPC_Bobinadora_Ext1->DesplazandoFilm,ID_DESPLAZANDO_FILM_EXT,STRING_ITEM_DESPLAZANDO_FILM);
    setNewItem(&OPC_Bobinadora_Ext1->LamparaOn,ID_LAMPARA_ON_EXT,STRING_ITEM_LAMPARA_ON);
    setNewItem(&OPC_Bobinadora_Ext1->BuscandoMacula,ID_BUSCANDO_MACULA_EXT,STRING_ITEM_BUSCANDO_MACULA);
    setNewItem(&OPC_Bobinadora_Ext1->Impresion,ID_IMPRESION_EXT,STRING_ITEM_IMPRESION);
    setNewItem(&OPC_Bobinadora_Ext1->ApagarXUSB1,ID_APAGAR_XUSB1_EXT,STRING_ITEM_APAGAR_XUSB1);
    setNewItem(&OPC_Bobinadora_Ext1->ApagarXUSB2,ID_APAGAR_XUSB2_EXT,STRING_ITEM_APAGAR_XUSB2);
    setNewItem(&OPC_Bobinadora_Ext1->EnableCalculoDiametro,ID_ENABLE_CALCULO_DIAMETRO,STRING_ITEM_ENABLE_CALCULO_DIAMETRO);
    setNewItem(&OPC_Bobinadora_Ext1->DiametroEntradaFiltered,ID_DIAMETRO_ENTRADA_FILTERED,STRING_ITEM_DIAMETRO_ENTRADA_FILTERED);
    setNewItem(&OPC_Bobinadora_Ext1->Impresion_Reverse,ID_IMPRESION_REVERSE,STRING_ITEM_IMPRESION_REVERSE);
    setNewItem(&OPC_Bobinadora_Ext1->Tensar_Lamina,ID_TENSAR_LAMINA,STRING_ITEM_TENSAR_LAMINA);
    setNewItem(&OPC_Bobinadora_Ext1->Destensar_Lamina,ID_DESTENSAR_LAMINA,STRING_ITEM_DESTENSAR_LAMINA);
    setNewItem(&OPC_Bobinadora_Ext1->LamparaVision,ID_LAMPARA_VISION,STRING_ITEM_LAMPARA_VISION);
    setNewItem(&OPC_Bobinadora_Ext1->Balluf_Centrado,ID_BALLUF_CENTRADO,STRING_ITEM_BALLUF_CENTRADO);
    setNewItem(&OPC_Bobinadora_Ext1->Lamina_sin_tension,ID_LAMINA_SIN_TENSION,STRING_ITEM_LAMINA_SIN_TENSION);
    setNewItem(&OPC_Bobinadora_Ext1->Error_Bobinador,ID_ERROR_BOBINADOR,STRING_ITEM_ERROR_BOBINADOR);
    setNewItem(&OPC_Bobinadora_Ext1->Error_DesBobinador,ID_ERROR_DESBOBINADOR,STRING_ITEM_ERROR_DESBOBINADOR);

    m_memoryMap->m_mapa.m_maquina.ref_metros_trabajo=LeerDato(ITEM_METROS_REF).toInt();
    m_memoryMap->m_mapa.m_maquina.ref_diametro_bobina_entrada=LeerDato(ITEM_DIAMETRO_ENTRADA_REF).toInt();
    m_memoryMap->m_mapa.m_maquina.ref_diametro_bobina_salida=LeerDato(ITEM_DIAMETRO_SALIDA_REF).toInt();
    m_memoryMap->m_mapa.m_maquina.diametro_parar=LeerDato(ITEM_DIAMETRO_PARAR).toInt();
    m_memoryMap->m_mapa.m_maquina.ref_velocidad=LeerDato(ITEM_VELOCIDAD_REF).toInt(); //velocidad actual en 100*m/min
    m_memoryMap->m_mapa.m_maquina.iContadorMaculas=LeerDato(ITEM_NUMERO_MACULAS).toInt();

    EscribirDato(ITEM_APAGAR_XUSB1,QVariant(0));
    EscribirDato(ITEM_APAGAR_XUSB2,QVariant(0));
    EscribirDato(ITEM_ENABLE_CALCULO_DIAMETRO,QVariant(0));
    EscribirDato(ITEM_JOG_MAS,QVariant(0));
    EscribirDato(ITEM_JOG_MENOS,QVariant(0));
    EscribirDato(ITEM_LAMPARA_VISION,QVariant(1));
    InicializarItemsExt1Advanced();

}

/** Inicializa Items avanzado*/
void ClienteOPC::InicializarItemsExt1Advanced(){

    setNewItem(&OPC_Bobinadora_Ext1->TiempoBusquedaMacula,ID_TIEMPO_BUSQUEDA_MACULA_EXT,STRING_ITEM_TIEMPO_BUSQUEDA_MACULA);
    setNewItem(&OPC_Bobinadora_Ext1->VelocidadBusquedaMacula,ID_VELOCIDAD_BUSQUEDA_MACULA_EXT,STRING_ITEM_VELOCIDAD_BUSQUEDA_MACULA);
    setNewItem(&OPC_Bobinadora_Ext1->VelocidadAjusteMacula,ID_VELOCIDAD_AJUSTE_MACULA_EXT,STRING_ITEM_VELOCIDAD_AJUSTE_MACULA);
    setNewItem(&OPC_Bobinadora_Ext1->VelocidadJogMas,ID_VELOCIDAD_JOG_MAS_EXT,STRING_ITEM_VELOCIDAD_JOG_MAS);
    setNewItem(&OPC_Bobinadora_Ext1->VelocidadJogMenos,ID_VELOCIDAD_JOG_MENOS_EXT,STRING_ITEM_VELOCIDAD_JOG_MENOS);
    setNewItem(&OPC_Bobinadora_Ext1->FuerzaFrenoElectromagnetico,ID_FUERZA_FRENO_EXT,STRING_ITEM_FUERZA_FRENO);
    setNewItem(&OPC_Bobinadora_Ext1->FrenoElectromagneticoOnOff,ID_FRENO_ONOFF_EXT,STRING_ITEM_FRENO_ONOFF);
    setNewItem(&OPC_Bobinadora_Ext1->IntensidadLampara,ID_INTENSIDAD_LAMPARA_EXT,STRING_ITEM_INTENSIDAD_LAMPARA);
    setNewItem(&OPC_Bobinadora_Ext1->TemperaturaLampara,ID_TEMPERATURA_LAMPARA_EXT,STRING_ITEM_TEMPERATURA_LAMPARA);
    setNewItem(&OPC_Bobinadora_Ext1->DistanciaEmpalme,ID_DISTANCIA_EMPALME_EXT,STRING_ITEM_DISTANCIA_EMPALME);
    setNewItem(&OPC_Bobinadora_Ext1->TensionBobina,ID_TENSION_BOBINA_EXT,STRING_ITEM_TENSION_BOBINA);
    setNewItem(&OPC_Bobinadora_Ext1->RampaAceleracion,ID_RAMPA_ACELERACION_EXT,STRING_ITEM_RAMPA_ACELERACION);
    setNewItem(&OPC_Bobinadora_Ext1->RampaDeceleracion,ID_RAMPA_DECELERACION_EXT,STRING_ITEM_RAMPA_DECELERACION);

    setNewItem(&OPC_Bobinadora_Ext1->NumeroMaculas,ID_NUMERO_MACULAS,STRING_NUMERO_MACULAS);
    setNewItem(&OPC_Bobinadora_Ext1->DistanciaMaculaMax,ID_MAX_DISTANCIA_MACULA,STRING_MAX_DISTANCIA_MACULA);
    setNewItem(&OPC_Bobinadora_Ext1->DistanciaMaculaMin,ID_MIN_DISTANCIA_MACULA,STRING_MIN_DISTANCIA_MACULA);
    setNewItem(&OPC_Bobinadora_Ext1->DiametroSalidaFiltered,ID_DIAMETRO_SALIDA_FILTERED,STRING_DIAMETRO_SALIDA_FILTERED);

    /*setNewItem(&OPC_Bobinadora_Ext1->LamparaMover,ID_LAMPARA_MOVER,STRING_ITEM_LAMPARA_MOVER);
    setNewItem(&OPC_Bobinadora_Ext1->LamparaSetUp,ID_LAMPARA_SETUP,STRING_ITEM_LAMPARA_SETUP);
    setNewItem(&OPC_Bobinadora_Ext1->LamparaReset,ID_LAMPARA_RESET,STRING_ITEM_LAMPARA_RESET);
    setNewItem(&OPC_Bobinadora_Ext1->LamparaJogF,ID_LAMPARA_JOGFORWARD,STRING_ITEM_LAMPARA_JOGF);
    setNewItem(&OPC_Bobinadora_Ext1->LamparaJogR,ID_LAMPARA_JOGREVERSE,STRING_ITEM_LAMPARA_JOGR);
    setNewItem(&OPC_Bobinadora_Ext1->LamparaPos,ID_LAMPARA_POS,STRING_ITEM_LAMPARA_POS);
    setNewItem(&OPC_Bobinadora_Ext1->DesplzSet,ID_DESPLAZADOR_SET,STRING_ITEM_DESPLAZ_SET);
    setNewItem(&OPC_Bobinadora_Ext1->DesplzInp,ID_DESPLAZADOR_INP,STRING_ITEM_DESPLAZ_INP);
    setNewItem(&OPC_Bobinadora_Ext1->DiamPLC_Output,ID_DIAM_PLC_OUTPUT,STRING_ITEM_DIAM_PLC_OUTPUT);
    setNewItem(&OPC_Bobinadora_Ext1->DiamPLC_Input,ID_DIAM_PLC_INPUT,STRING_ITEM_DIAM_PLC_INPUT);
    setNewItem(&OPC_Bobinadora_Ext1->BajarCuchillas,ID_BAJAR_CUCHILLAS,STRING_ITEM_BAJAR_CUCHILLAS);
    setNewItem(&OPC_Bobinadora_Ext1->FuerzaCuchillas,ID_FUERZA_CUCHILLAS,STRING_ITEM_FUERZA_CUCHILLAS);*/


    m_bInitiated=true;
    emit OPCInitiated();
}

/** Devuelve si esta inicializado*/
bool ClienteOPC::initiated(){
    return m_bInitiated;
}

/** Devuelve los handles a monitorizar*/
QVariant ClienteOPC::returnOPCHandle(int ReferenciaItem){
    struct OPC_Item *p=NULL;
    QVariant rtn=-1;
    p=ReturnOPCItem(ReferenciaItem);
    if ((p!=NULL)&& (m_memoryMap->m_mapa.m_maquina.m_opc.estado==1)){
        rtn=QVariant(p->client_handle);
    }
   return rtn;
}
/** Devuelve un item OPC*/
struct OPC_Item * ClienteOPC::ReturnOPCItem(int ReferenciaItem){
    struct OPC_Item *p=NULL;
    if ((m_memoryMap->m_mapa.m_maquina.m_opc.estado==1)){
    switch (ReferenciaItem){
        case ITEM_VELOCIDAD_ACT:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==1){
                p=&OPC_Bobinadora->VelocidadActual;
            }
            else if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2){
                p=&OPC_Bobinadora_Ext1->VelocidadActual;
            }
            break;
        case ITEM_VELOCIDAD_REF:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==1){
                p=&OPC_Bobinadora->VelocidadRef;
            }
            else if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2){
                p=&OPC_Bobinadora_Ext1->VelocidadRef;
            }
            break;
        case ITEM_METROS_ACT:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==1){
                p=&OPC_Bobinadora->LongitudActual;
            }
            else if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2) {
                p=&OPC_Bobinadora_Ext1->LongitudActual;
            }
            break;
        case ITEM_METROS_REF:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==1){
                p=&OPC_Bobinadora->LongitudRef;
            }
            else if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2){
                p=&OPC_Bobinadora_Ext1->LongitudRef;
            }
            break;
        case ITEM_CONDICIONES_IMPRESION:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==1){
                p=&OPC_Bobinadora->CondicionesImpresion;
            }
            else if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2){
                p=&OPC_Bobinadora_Ext1->CondicionesImpresion;
            }
            break;
        case ITEM_TRABAJO_NUEVO:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==1){
                p=&OPC_Bobinadora->TrabajoNuevo;
            }
            else if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
                p=&OPC_Bobinadora_Ext1->TrabajoNuevo;
            break;
        case ITEM_CONDICIONES_MOVIMIENTO:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==1){
                p=&OPC_Bobinadora->CondicionesMovimiento;
            }
            else if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
                p=&OPC_Bobinadora_Ext1->CondicionesMovimiento;
            break;
        case ITEM_CODIGO_PARADA:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==1){
                p=&OPC_Bobinadora->CodigoParada;
            }
            else if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2){
                p=&OPC_Bobinadora_Ext1->CodigoParada;
            }
            break;
        case ITEM_PERMISO_PARAR:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==1){
                p=&OPC_Bobinadora->PermisoParar;
            }
            else if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
                p=&OPC_Bobinadora_Ext1->PermisoParar;
            break;
        case ITEM_DIAMETRO_PARAR:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==1){
                p=&OPC_Bobinadora->DiametroParar;
            }
            else if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2){
                p=&OPC_Bobinadora_Ext1->DiametroParar;
            }
           break;
        case ITEM_PARO_MAQUINA:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==1){
            }
            else if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
                p=&OPC_Bobinadora_Ext1->ParoMaquina;
            break;
        case ITEM_MARCHA_MOVIMIENTO:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==1){
            }
            else if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
                p=&OPC_Bobinadora_Ext1->MarchaMovimiento;
            break;
        case ITEM_PARO_MOVIMIENTO:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==1){
            }
            else if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
                p=&OPC_Bobinadora_Ext1->ParoMovimiento;
            break;
        case ITEM_SECUENCIA_ON:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==1){
            }
            else if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2){
                p=&OPC_Bobinadora_Ext1->SecuenciaOn;
            }
            break;
        case ITEM_IMPRESION:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==1){
            }
            else if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2){
                p=&OPC_Bobinadora_Ext1->Impresion;
            }
            break;
        case ITEM_DETECTOR_MACULA:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==1){
            }
            else if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
                p=&OPC_Bobinadora_Ext1->DetectorMacula;
            break;
        case ITEM_JOG_MAS:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==1){
            }
        else if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
            p=&OPC_Bobinadora_Ext1->JogMas;
            break;
        case ITEM_JOG_MENOS:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==1){
            }
            else if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
                p=&OPC_Bobinadora_Ext1->JogMenos;
            break;
        case ITEM_BUSCAR_MACULA:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
                p=&OPC_Bobinadora_Ext1->BuscarMacula;
            break;
        case ITEM_BUSCANDO_MACULA:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
                p=&OPC_Bobinadora_Ext1->BuscandoMacula;
            break;
        case ITEM_DESHABILITAR_SERVO_ENTRADA:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==1){
            }
        else if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
            p=&OPC_Bobinadora_Ext1->DeshabilitarServoEntrada;
            break;
        case ITEM_DESHABILITAR_SERVO_SALIDA:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==1){
            }
            else if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
                 p=&OPC_Bobinadora_Ext1->DeshabilitarServoSalida;
            break;
        case ITEM_DESPLAZAR_FILM:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==1){
            }
            else if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
                p=&OPC_Bobinadora_Ext1->DesplazarFilm;
            break;
        case ITEM_DESPLAZANDO_FILM:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==1){
            }
            else if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
                p=&OPC_Bobinadora_Ext1->DesplazandoFilm;
            break;
        case ITEM_VELOCIDAD_AJUSTE_MACULA:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
                p=&OPC_Bobinadora_Ext1->VelocidadAjusteMacula;
            break;
        case ITEM_VELOCIDAD_BUSQUEDA_MACULA:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
                p=&OPC_Bobinadora_Ext1->VelocidadBusquedaMacula;
            break;
        case ITEM_TIEMPO_BUSQUEDA_MACULA:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
                p=&OPC_Bobinadora_Ext1->TiempoBusquedaMacula;
            break;
        case ITEM_VELOCIDAD_JOG_MAS:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
                p=&OPC_Bobinadora_Ext1->VelocidadJogMas;
            break;
        case ITEM_VELOCIDAD_JOG_MENOS:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2){
                p=&OPC_Bobinadora_Ext1->VelocidadJogMenos;
            }
            break;
        case ITEM_FRENO_ONOFF:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
                p=&OPC_Bobinadora_Ext1->FrenoElectromagneticoOnOff;
            break;
        case ITEM_FUERZA_FRENO:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
                p=&OPC_Bobinadora_Ext1->FuerzaFrenoElectromagnetico;
            break;
        case ITEM_INTENSIDAD_LAMPARA:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
                p=&OPC_Bobinadora_Ext1->IntensidadLampara;            
            break;
        case ITEM_TEMPERATURA_LAMPARA:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
                p=&OPC_Bobinadora_Ext1->TemperaturaLampara;
            break;
        case ITEM_FIJAR_DIAMETRO_ENTRADA:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==1){
                p=&OPC_Bobinadora->FijarDiametro;
            }
            else if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2){
                p=&OPC_Bobinadora_Ext1->FijarDiametroEntrada;
            }
            break;
        case ITEM_FIJAR_DIAMETRO_SALIDA:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2){
                p=&OPC_Bobinadora_Ext1->FijarDiametroSalida;
            }
            break;
        case ITEM_DIAMETRO_ENTRADA_ACT:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==1){
                p=&OPC_Bobinadora->DiametroActual;
            }
            else if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2){
                p=&OPC_Bobinadora_Ext1->DiametroEntradaActual;
            }
            break;
        case ITEM_DIAMETRO_ENTRADA_REF:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==1){
                p=&OPC_Bobinadora->DiametroRef;
            }
            else if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2){
                p=&OPC_Bobinadora_Ext1->DiametroEntradaRef;
            }
            break;
        case ITEM_DIAMETRO_SALIDA_ACT:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2){
                p=&OPC_Bobinadora_Ext1->DiametroSalidaActual;
            }
            break;
        case ITEM_DIAMETRO_SALIDA_REF:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==1){
            }
            else if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2){
                p=&OPC_Bobinadora_Ext1->DiametroSalidaRef;
            }
            break;
        case ITEM_TENSION_BOBINA:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
                p=&OPC_Bobinadora_Ext1->TensionBobina;
            break;
        case ITEM_RAMPA_ACELERACION:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
                p=&OPC_Bobinadora_Ext1->RampaAceleracion;
            break;
        case ITEM_RAMPA_DECELERACION:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
                p=&OPC_Bobinadora_Ext1->RampaDeceleracion;
            break;
        case ITEM_LAMPARA_ON:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==1){
            }
            else if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
                p=&OPC_Bobinadora_Ext1->LamparaOn;
            break;
        case ITEM_DISTANCIA_EMPALME:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
                p=&OPC_Bobinadora_Ext1->DistanciaEmpalme;
            break;
        case ITEM_APAGAR_XUSB1:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
                p=&OPC_Bobinadora_Ext1->ApagarXUSB1;
            break;
        case ITEM_APAGAR_XUSB2:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
                p=&OPC_Bobinadora_Ext1->ApagarXUSB2;
            break;
        case ITEM_ENABLE_CALCULO_DIAMETRO:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
                p=&OPC_Bobinadora_Ext1->EnableCalculoDiametro;
            break;
        case ITEM_DIAMETRO_ENTRADA_FILTERED:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
                p=&OPC_Bobinadora_Ext1->DiametroEntradaFiltered;
            break;            
        case ITEM_IMPRESION_REVERSE:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
            p=&OPC_Bobinadora_Ext1->Impresion_Reverse;
            break;
        case ITEM_TENSAR_LAMINA:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
            p=&OPC_Bobinadora_Ext1->Tensar_Lamina;
            break;
        case ITEM_DESTENSAR_LAMINA:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
            p=&OPC_Bobinadora_Ext1->Destensar_Lamina;
            break;
        case ITEM_BALLUF_CENTRADO:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
            p=&OPC_Bobinadora_Ext1->Balluf_Centrado;
            break;
        case ITEM_LAMINA_SIN_TENSION:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
            p=&OPC_Bobinadora_Ext1->Lamina_sin_tension;
            break;
        case ITEM_ERROR_BOBINADOR:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
            p=&OPC_Bobinadora_Ext1->Error_Bobinador;
            break;
        case ITEM_ERROR_DESBOBINADOR:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
            p=&OPC_Bobinadora_Ext1->Error_DesBobinador;
            break;
        case ITEM_NUMERO_MACULAS:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
            p=&OPC_Bobinadora_Ext1->NumeroMaculas;
            break;
        case ITEM_MAX_DISTANCIA_MACULA:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
            p=&OPC_Bobinadora_Ext1->DistanciaMaculaMax;
            break;
        case ITEM_MIN_DISTANCIA_MACULA:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
            p=&OPC_Bobinadora_Ext1->DistanciaMaculaMin;
            break;
        case ITEM_DIAMETRO_SALIDA_FILTERED:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
            p=&OPC_Bobinadora_Ext1->DiametroSalidaFiltered;
            break;
        case ITEM_LAMPARA_VISION:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
            p=&OPC_Bobinadora_Ext1->LamparaVision;
            break;
        case ITEM_LAMPARA_MOVER:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
            p=&OPC_Bobinadora_Ext1->LamparaMover;
            break;
        case ITEM_LAMPARA_SETUP:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
            p=&OPC_Bobinadora_Ext1->LamparaSetUp;
            break;
        case ITEM_LAMPARA_RESET:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
            p=&OPC_Bobinadora_Ext1->LamparaReset;
            break;
        case ITEM_LAMPARA_JOGF:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
            p=&OPC_Bobinadora_Ext1->LamparaJogF;
            break;
        case ITEM_LAMPARA_JOGR:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
            p=&OPC_Bobinadora_Ext1->LamparaJogR;
            break;
        case ITEM_LAMPARA_POS:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
            p=&OPC_Bobinadora_Ext1->LamparaPos;
            break;
        case ITEM_DESPLAZ_SET:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
            p=&OPC_Bobinadora_Ext1->DesplzSet;
            break;
        case ITEM_DESPLAZ_INP:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
            p=&OPC_Bobinadora_Ext1->DesplzInp;
            break;
        case ITEM_DIAM_PLC_OUTPUT:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
            p=&OPC_Bobinadora_Ext1->DiamPLC_Output;
            break;
        case ITEM_DIAM_PLC_INPUT:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
            p=&OPC_Bobinadora_Ext1->DiamPLC_Input;
            break;
        case ITEM_BAJAR_CUCHILLAS:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
            p=&OPC_Bobinadora_Ext1->BajarCuchillas;
            break;
        case ITEM_FUERZA_CUCHILLAS:
            if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2)
            p=&OPC_Bobinadora_Ext1->FuerzaCuchillas;
            break;
        }


    }
    //Por si acaso no la he inicializado
    if (p!=NULL){
        if (p->ID.isEmpty()) p=NULL;
    }
    return p;
}

/** Mensaje de la lampara*/
void ClienteOPC::sltMessageFromLampara(int ReferenciaItem,QVariant valor){    
    EscribirDato(ReferenciaItem,valor);
    if (ReferenciaItem==ITEM_LAMPARA_POS){ //Mandamos un flanco para mover
        if (m_memoryMap->m_mapa.m_maquina.codigo_parada){
            emit SendDebugInfo(LOGGER_WARNING,tr("No se permite movimiento automatico de lampara"));

        }
        else{
            EscribirDato(ITEM_LAMPARA_MOVER,1);
            timedEscribeDato(ITEM_LAMPARA_MOVER,0);
        }
    }
    emit sgnOPCNewLamparaValue(ReferenciaItem,valor);
}
/** Mensaje de la lampara*/
void ClienteOPC::sltMessageFromCorte(int ReferenciaItem,QVariant valor){
    EscribirDato(ReferenciaItem,valor);
}
/** Escribe dato en OPC*/
void ClienteOPC::EscribirDato(int ReferenciaItem,QVariant valor){
    if (m_enabled){
        struct OPC_Item *p=NULL;
        p=ReturnOPCItem(ReferenciaItem);
        if ((p!=NULL)&& (m_memoryMap->m_mapa.m_maquina.m_opc.estado==1)){
                MiServidorOPC->escribeDato(p->client_handle,valor);
                if ((ReferenciaItem==ITEM_DIAMETRO_ENTRADA_FILTERED)||(ReferenciaItem==ITEM_DIAMETRO_SALIDA_FILTERED)); //No hace falta que lo publique siempre
                else emit SendDebugInfo(LOGGER_WARNING, QString("Set OPC [%1]=%2").arg(p->name).arg(valor.toInt()));
        }
        else{
            if (m_memoryMap->m_mapa.m_maquina.m_opc.estado) emit SendDebugInfo(LOGGER_WARNING, QString("EscribeOPC: No existe item %1").arg(ReferenciaItem));
        }
    }
}
/** Lee un dato OPC*/
QVariant ClienteOPC::LeerDato(int ReferenciaItem){
    QVariant ret=0;
    if (m_enabled){
        struct OPC_Item *p=NULL;
        p=ReturnOPCItem(ReferenciaItem);
        if ((p!=NULL) && (m_memoryMap->m_mapa.m_maquina.m_opc.estado==1)) {

            ret=MiServidorOPC->leeDato(p->client_handle);
            emit SendDebugInfo(LOGGER_WARNING, QString("Lee OPC [%1][%2] %3").arg(p->name).arg(p->ID).arg(ret.toInt()));
        }
        else{
            emit SendDebugInfo(LOGGER_WARNING, QString("LeeOPC: No existe item %1").arg(ReferenciaItem));
        }
    }
    return ret;
}

/** Activa/Desactiva la monutorizacion de valores*/
void ClienteOPC::sltEnableMonitoring(bool val){
    if (m_Monitor==val) return;
    m_Monitor=val;
    if (val){
        sltMonitoring();
        //QTimer::singleShot(0,this,SLOT(sltMonitoring()));
        QTimer::singleShot(1500,this,SLOT(sltMonitoringLong()));
        emit sgnStartOPCMonitor();
    }
}

/** Solo para la velocidad*/
void ClienteOPC::sltUpdateVelocidad(){
    m_memoryMap->m_mapa.m_maquina.velocidad=LeerDato(ITEM_VELOCIDAD_ACT).toInt(); //velocidad actual en 100*m/min
}

/** Monitoriza una serie de valores*/
void ClienteOPC::sltMonitoring(){
    if (m_Monitor){
        unsigned int iCode=(unsigned int)LeerDato(ITEM_CODIGO_PARADA).toInt();
        //emit SendDebugInfo(LOGGER_WARNING,QString("%1-%2").arg(iCode).arg(m_memoryMap->m_mapa.m_maquina.codigo_parada));
        if (iCode!=m_memoryMap->m_mapa.m_maquina.codigo_parada){
            m_memoryMap->m_mapa.m_maquina.codigo_parada=iCode;
            emit sgnOPCFlanco(ITEM_CODIGO_PARADA);
        }
        bool bSecuencia=LeerDato(ITEM_SECUENCIA_ON).toBool();
        if (bSecuencia!=m_memoryMap->m_mapa.m_maquina.secuencia_on){
            m_memoryMap->m_mapa.m_maquina.secuencia_on=bSecuencia;
            emit sgnOPCFlanco(ITEM_SECUENCIA_ON);
        }
        m_memoryMap->m_mapa.m_maquina.diametro_bobina_salida=LeerDato(ITEM_DIAMETRO_SALIDA_ACT).toInt();
        sltUpdateVelocidad();
        m_memoryMap->m_mapa.m_maquina.lampara_on=LeerDato(ITEM_LAMPARA_ON).toBool();
        m_memoryMap->m_mapa.m_maquina.intensidad_lampara=LeerDato(ITEM_INTENSIDAD_LAMPARA).toInt();
        m_memoryMap->m_mapa.m_maquina.diametro_bobina_entrada_inst=LeerDato(ITEM_DIAMETRO_ENTRADA_ACT).toInt();
        unsigned int foo=LeerDato(ITEM_MAX_DISTANCIA_MACULA).toInt();
        if ((foo!=MAX_MACULA)&&(foo!=MIN_MACULA))
            m_memoryMap->m_mapa.m_maquina.iMaxMacula=foo;
        foo=LeerDato(ITEM_MIN_DISTANCIA_MACULA).toInt();
        if ((foo!=MAX_MACULA)&&(foo!=MIN_MACULA))
            m_memoryMap->m_mapa.m_maquina.iMinMacula=foo;
#ifndef HONLE
        m_memoryMap->m_mapa.m_maquina.temperatura_lampara=LeerDato(ITEM_TEMPERATURA_LAMPARA).toInt();
#endif

        //De momento...
        //m_memoryMap->m_mapa.m_maquina.diametro_bobina_entrada=m_memoryMap->m_mapa.m_maquina.diametro_bobina_entrada_inst;
        m_memoryMap->m_mapa.m_maquina.metros_trabajo=LeerDato(ITEM_METROS_ACT).toInt();
#ifdef DEPURACION_IN2
        //emit SendDebugInfo(LOGGER_WARNING,"[OPC-Debug]");

#endif

        //Volvemos a preguntar
        QTimer::singleShot(TIME_MONITOR,this,SLOT(sltMonitoring()));

        //QTimer::singleShot(5000,this,SLOT(sltMonitoring()));
    }
}
/** monitorizacion de referencias*/
void ClienteOPC::sltMonitoringLong(){
    if (m_Monitor){
        In2ManagerDiametros *manager=In2ManagerDiametros::request(0);
        m_memoryMap->m_mapa.m_maquina.ref_metros_trabajo=LeerDato(ITEM_METROS_REF).toInt();
        m_memoryMap->m_mapa.m_maquina.ref_diametro_bobina_entrada=LeerDato(ITEM_DIAMETRO_ENTRADA_REF).toInt();
        m_memoryMap->m_mapa.m_maquina.ref_diametro_bobina_salida=LeerDato(ITEM_DIAMETRO_SALIDA_REF).toInt();
        m_memoryMap->m_mapa.m_maquina.diametro_parar=LeerDato(ITEM_DIAMETRO_PARAR).toInt();

        //Sensores de diametro en el PLC
        if (m_bPLCSensorDiameterIN){
            int diamIn=LeerDato(ITEM_DIAM_PLC_INPUT).toInt();
            manager->storeValueFromPLC(IN2MANAGER_DIN,diamIn);
        }
        if (m_bPLCSensorDiameterOUT){
            int diamOut=LeerDato(ITEM_DIAM_PLC_OUTPUT).toInt();
            manager->storeValueFromPLC(IN2MANAGER_DOUT,diamOut);
        }

        QTimer::singleShot(TIME_MONITOR_LONG,this,SLOT(sltMonitoringLong()));
    }
}


