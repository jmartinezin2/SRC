
#include "gestormaquina.h"
#include <QTimer>
#include "in2message.h"
#include <QMessageBox>
#include "in2log.h"
#include "in2managerdiametros.h"
#include "in2gestorlampara.h"
#include "in2gestormaquina.h"

//Velocidad para hacer soft stop
#define SOFT_STOP_SPEED 1500

//Cada 500 msg se pregunta
#define TIMEOUT_PRINT_MONITOR 500
//Gota por defecto en la tabla por defecto
#define DEFAULT_DROP 3

//Numero de valores sobre los que filtrar el diametro de entrada
#define VALUES_INPUT_DIAM 10
//Timeout de monitorizacion
#define TIMEOUT_MON 750
//Timeout de interbloqueo
#define TIME_INTERBLOQUEO 20000

#define INIT_COUNTER_INTERBLOQUEOS 500
//Minima velocidad de encoder...sino a parar
#define MINIMA_VELOCIDAD_ENCODER 1

#ifdef HAPPA
    #define JOINT_STOP_TIME 18000
#else
    #define JOINT_STOP_TIME 20000
#endif


GestorMaquina *GestorMaquina::mInstance  = NULL;



/** Singleton*/
GestorMaquina *GestorMaquina::request(QObject *parent){
    if(mInstance == NULL){
        mInstance = new GestorMaquina(parent);
    }
    return mInstance;

}
/** Constructor*/
GestorMaquina::GestorMaquina(QObject *parent) :
    QObject(parent)
{
    m_version=2; //Por defecto version 2
    m_ClienteOPC=NULL;
    m_ClienteOPC = ClienteOPC::request(0);
    m_GestorImpresion=GestorImpresion::request(this);
    m_MonDiametro=In2ManagerDiametros::request(0);
    m_MonDiametro->initMonitor(IN2MANAGER_DIN);
    m_MonDiametro->initMonitor(IN2MANAGER_DOUT);
    m_ContadorArranque=0;
    m_memoryMap=MemoryMap::request(0);
    QTimer::singleShot(0,this,SLOT(InitGestor()));
}


/** Inicializa el gestor*/
bool GestorMaquina::InitGestor(){

    bool rtn=true;

    m_ClienteOPC->Init();
    if (!m_GestorImpresion->InitGestor()){
        QMessageBox::warning(0,"WARNING","No hay electrónica conectada",QMessageBox::Ok);
    }
    MonitorDiametro *p=m_MonDiametro->getMonitor(IN2MANAGER_DIN);
    if (p->isEnabled()){
         connect(p,SIGNAL(sgnWenglorDiameter(double)),this,SLOT(sltWenglorDiameter(double)));
         emit SendDebugInfo(LOGGER_WARNING,"USING WENGLOR SENSOR DIAMETER DATA (INPUT)");
    }
    else{
        emit SendDebugInfo(LOGGER_WARNING,"USING PLC DIAMETER DATA (INPUT)");
    }

    MonitorDiametro *p2=m_MonDiametro->getMonitor(IN2MANAGER_DOUT);
    if (p2->isEnabled()){
         connect(p2,SIGNAL(sgnWenglorDiameter(double)),this,SLOT(sltWenglorOutputDiameter(double)));
         emit SendDebugInfo(LOGGER_WARNING,"USING WENGLOR SENSOR DIAMETER DATA (OUTPUT)");
    }
    else{
        emit SendDebugInfo(LOGGER_WARNING,"USING PLC DIAMETER DATA (OUTPUT)");
    }

    Leer_Tabla_RefVelocidad_Intensidad();
    sltMonitor();
    QTimer::singleShot(0,this,SLOT(sltEnableCondicionesMovimiento()));
    return rtn;
}
/** Monitorizacion de datos*/
void GestorMaquina::sltMonitor(){
    sltFiltrarDiametroEntrada();
    sltAjustarIntensidadLamparaDinamico();
    QTimer::singleShot(TIMEOUT_MON,this,SLOT(sltMonitor()));
}
/** Fuerza una actualizacion de velocidad+ajuste de lampara*/
void GestorMaquina::sltForceUpdateVelocidadAndLampara(){
    //Forzamos actualizar la velocidad
    emit SendDebugInfo(LOGGER_STORE,QString("Track speed [%1] & lamp [%2]").arg(m_memoryMap->m_mapa.m_maquina.velocidad/100).arg(m_memoryMap->m_mapa.m_maquina.intensidad_lampara));
    m_ClienteOPC->sltUpdateVelocidad();
    sltAjustarIntensidadLamparaDinamico();
}

/** Filtro diametro entrada*/
void GestorMaquina::sltFiltrarDiametroEntrada(){
    MonitorDiametro *p=m_MonDiametro->getMonitor(IN2MANAGER_DIN);
    static signed short prev[VALUES_INPUT_DIAM];
    static int pos=0;
    if (!p->isEnabled()){
        if (!p->isPLCEnabled()){
            signed short newValue=0;
            prev[pos]=m_memoryMap->m_mapa.m_maquina.diametro_bobina_entrada_inst;
            for (int i=0;i<VALUES_INPUT_DIAM;i++){
                newValue+=prev[i];
            }
            pos++;
            pos=pos%VALUES_INPUT_DIAM;
            m_memoryMap->m_mapa.m_maquina.diametro_bobina_entrada=newValue/VALUES_INPUT_DIAM;
            sltSetFilteredDiameter(m_memoryMap->m_mapa.m_maquina.diametro_bobina_entrada);
        }
        else{
            signed short val=(signed short)p->getCurrentDiameter(0);
            signed short newValue=0;
            prev[pos]=val;
            for (int i=0;i<VALUES_INPUT_DIAM;i++){
                newValue+=prev[i];
            }
            pos++;
            pos=pos%VALUES_INPUT_DIAM;
            m_memoryMap->m_mapa.m_maquina.diametro_bobina_entrada=newValue/VALUES_INPUT_DIAM;
            sltSetFilteredDiameter(m_memoryMap->m_mapa.m_maquina.diametro_bobina_entrada);
        }
    }
 }


/** recoge el diametro publicado por el sensor de wenglor*/
void GestorMaquina::sltWenglorDiameter(double x){
    m_memoryMap->m_mapa.m_maquina.diametro_bobina_entrada=(signed short)x;
    sltSetFilteredDiameter(m_memoryMap->m_mapa.m_maquina.diametro_bobina_entrada);
}
/** recoge el diametro publicado por el sensor de wenglor*/
void GestorMaquina::sltWenglorOutputDiameter(double x){
    m_memoryMap->m_mapa.m_maquina.diametro_bobina_salida_inst=(signed short)x;
    sltSetOutputFilteredDiameter(m_memoryMap->m_mapa.m_maquina.diametro_bobina_salida_inst);
}


/** Habilita/Deshabilita freno electromagnetico*/
void GestorMaquina::sltHabilitarFreno(bool x){
//#ifdef HAPPA
    if (x){
        m_ClienteOPC->EscribirDato(ITEM_FRENO_ONOFF,0);
    }
    else{
        m_ClienteOPC->EscribirDato(ITEM_FRENO_ONOFF,1);
    }
//#endif
}


/** Manda informacion de debug*/
/*void GestorMaquina::sltSendDebugInfo(unsigned char level,QString data){
    emit SendDebugInfo(level,data);
}*/

/** Actualiza la info*/
void GestorMaquina::sltRefreshInfo(){

}


/** Configura la máquina para empezar a imprimir */
void GestorMaquina::sltIniciarImpresion(){

}

/** Configura la máquina para parar la impresión */
void GestorMaquina::sltFinalizarImpresion(){
    sltHabilitarCalculoDiametro(0);
    sltMonitorSpeed();
    this->sltDecelerarMaquina();
    QTimer::singleShot(0,this,SLOT(sltCheckSoftStopSpeed()));
    //Chequeamos velocidad de salida


}
/** Monitoriza la velocidad*/
void GestorMaquina::sltMonitorSpeed(){
    QTimer::singleShot(0,this,SLOT(sltForceUpdateVelocidadAndLampara()));
    QTimer::singleShot(500,this,SLOT(sltForceUpdateVelocidadAndLampara()));
    QTimer::singleShot(1500,this,SLOT(sltForceUpdateVelocidadAndLampara()));
    QTimer::singleShot(2000,this,SLOT(sltForceUpdateVelocidadAndLampara()));
    QTimer::singleShot(2500,this,SLOT(sltForceUpdateVelocidadAndLampara()));
    QTimer::singleShot(3000,this,SLOT(sltForceUpdateVelocidadAndLampara()));
    QTimer::singleShot(3500,this,SLOT(sltForceUpdateVelocidadAndLampara()));
    QTimer::singleShot(4000,this,SLOT(sltForceUpdateVelocidadAndLampara()));
    QTimer::singleShot(4500,this,SLOT(sltForceUpdateVelocidadAndLampara()));
    QTimer::singleShot(5000,this,SLOT(sltForceUpdateVelocidadAndLampara()));
    QTimer::singleShot(5500,this,SLOT(sltForceUpdateVelocidadAndLampara()));
    QTimer::singleShot(6000,this,SLOT(sltForceUpdateVelocidadAndLampara()));
    QTimer::singleShot(6500,this,SLOT(sltForceUpdateVelocidadAndLampara()));
    QTimer::singleShot(7000,this,SLOT(sltForceUpdateVelocidadAndLampara()));
    QTimer::singleShot(7500,this,SLOT(sltForceUpdateVelocidadAndLampara()));
    QTimer::singleShot(8000,this,SLOT(sltForceUpdateVelocidadAndLampara()));
    QTimer::singleShot(8500,this,SLOT(sltForceUpdateVelocidadAndLampara()));
    QTimer::singleShot(9000,this,SLOT(sltForceUpdateVelocidadAndLampara()));
    QTimer::singleShot(9500,this,SLOT(sltForceUpdateVelocidadAndLampara()));
    QTimer::singleShot(10000,this,SLOT(sltForceUpdateVelocidadAndLampara()));


}


/** Chequea que se de la condicion de velocidad para hacer softstop*/
void GestorMaquina::sltCheckSoftStopSpeed(){
    if (m_memoryMap->m_mapa.m_maquina.velocidad<SOFT_STOP_SPEED){
        sltExecFullSoftStop();
        int TiempoParada=TIMEOUT_PRINT_COMPLETE;
        if (m_memoryMap->m_mapa.m_maquina.metros_trabajo>=m_memoryMap->m_mapa.m_maquina.ref_metros_trabajo){
            TiempoParada=getTiempoParada();
        }
        else{
            if (m_memoryMap->m_mapa.m_maquina.velocidad<1000)
                TiempoParada=TIMEOUT_PRINT_COMPLETE_LONG;
            else
                TiempoParada=TIMEOUT_PRINT_COMPLETE;

        }

        if (m_memoryMap->m_FilePath.isEmpty()) TiempoParada=TIMEOUT_PRINT_COMPLETE_LONG;
        emit SendDebugInfo(LOGGER_WARNING,QString("Waiting %1 msg to stop").arg(TiempoParada));
        QTimer::singleShot(TiempoParada,this,SLOT(sltImpresionFinalizada()));

    }
    else{
        QTimer::singleShot(1000,this,SLOT(sltCheckSoftStopSpeed()));
    }
}

/** A borrar cuando se tenga el gestorMaquina*/
void GestorMaquina::sltImpresionFinalizada(){
    if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==1){
        m_ClienteOPC->EscribirDato(ITEM_PERMISO_PARAR,QVariant(1));
        m_ClienteOPC->timedEscribeDato(ITEM_PERMISO_PARAR,QVariant(0));
    }
    else if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2){
        if (m_memoryMap->m_mapa.m_maquina.EmpalmePerformed){
            emit sgnGesMaquinaMessage(tr("Verificar estado correcto de impresión y cabezales"));
            //In2Message *p=new In2Message(0,"Verificar estado correcto de impresión y cabezales",1);
            //p->show();
            //connect(this,SIGNAL(sgnKillIn2Message()),p,SLOT(sltDelete()));
        }
        sltPararMaquina();
        //Insertar_Evento(FIN_IMPRESION,QString("Fin impresion"));
        ChangeStyle(THEME_NORMAL);
        sltEnableCondicionesMovimiento();
        sltCentrarBalluf();
        if (m_memoryMap->m_mapa.m_machinedefaults.MaculaTrasPrint){
            QTimer::singleShot(5000,this,SLOT(sltBuscarMacula()));
        }

        emit sgnGestorMaquinaFinImpresion();

    }
}
/** Hace un paro por metros impresos*/
void GestorMaquina::sltParoPorMetrosImpresos(){
/*    if (m_memoryMap->m_mapa.m_machinedefaults.MaculaTrasPrint){
        emit SendDebugInfo(LOGGER_ERROR,"sltParoPorMetrosImpresos:MaculaTrasPrint");
        sltBuscarMacula();
        sltDisableParoMovimiento();
    }
    else{*/
        emit SendDebugInfo(LOGGER_ERROR,"sltParoPorMetrosImpresos:No se busca macula");
        #ifdef HAPPA
            sltDisableParoMovimiento();
        #endif
        QTimer::singleShot(DELAY_OPC_SIGNAL,this,SLOT(sltEnableCondicionesMovimiento()));
    //}
}


/** Para la maquina en funcion de ciertas condicuiones*/
void GestorMaquina::sltPararMaquina(){

    if (m_memoryMap->m_mapa.m_maquina.metros_trabajo>=m_memoryMap->m_mapa.m_maquina.ref_metros_trabajo){
        sltParoPorMetrosImpresos();
    }
    else{
        if (m_memoryMap->m_mapa.m_maquina.EmpalmePerformed){
            sltParoPorEmpalme();
        }
        else{
            sltDisableParoMovimiento();
            if (m_memoryMap->m_mapa.m_maquina.ParadaDiametro){
                QTimer::singleShot(TIMEOUT_PARADA_DIAMETRO,this,SLOT(sltParoPorDiametroEntrada()));
            }
        }
    }
}


/** Enciende XUSB */
void GestorMaquina::sltEncenderXUSB(){


}

/** Apaga XUSB */
void GestorMaquina::sltApagarXUSB(){


}

/** Configura la máquina para realizar un cambio en la bobina de entrada */
void GestorMaquina::sltCambioBobinaEntrada(){


}

/** Configura la máquina para realizar un cambio en la bobina de salida */
void GestorMaquina::sltCambioBobinaSalida(){


}	

/** A borrar cuando se tenga el gestor maquina*/
void GestorMaquina::sltDecelerarMaquina(){
    if (m_version==1){
        m_ClienteOPC->EscribirDato(ITEM_CONDICIONES_MOVIMIENTO,QVariant(0));
    }
    else if (m_version==2){
        m_ClienteOPC->EscribirDato(ITEM_PARO_MAQUINA,QVariant(1));
        m_ClienteOPC->timedEscribeDato(ITEM_PARO_MAQUINA,QVariant(0));
    }
}
/** ParoMovimiento 0->1*/
void GestorMaquina::sltDisableParoMovimiento(){
    m_ClienteOPC->timedEscribeDato(ITEM_PARO_MOVIMIENTO,QVariant(0));
    m_ClienteOPC->timedEscribeDato(ITEM_PARO_MOVIMIENTO,QVariant(1));    
}
/** CondicionesMovimento 0->1*/
void GestorMaquina::sltEnableCondicionesMovimiento(){
    m_ClienteOPC->EscribirDato(ITEM_LAMPARA_ON,0x00);
    m_ClienteOPC->timedEscribeDato(ITEM_CONDICIONES_MOVIMIENTO,QVariant(0));
    m_ClienteOPC->timedEscribeDato(ITEM_CONDICIONES_MOVIMIENTO,QVariant(1));
}
/** MarchaMovimiento 1-->0 */
void GestorMaquina::sltDisableMarchaMovimiento(){
    m_ClienteOPC->timedEscribeDato(ITEM_MARCHA_MOVIMIENTO,QVariant(1));
    m_ClienteOPC->timedExtEscribeDato(5000,ITEM_MARCHA_MOVIMIENTO,QVariant(0));
}
/** MarchaMovimiento 1-->0 */
void GestorMaquina::sltEnableMarchaMovimiento(){
    m_ClienteOPC->timedEscribeDato(ITEM_MARCHA_MOVIMIENTO,QVariant(0));
    m_ClienteOPC->timedEscribeDato(ITEM_MARCHA_MOVIMIENTO,QVariant(1));
}



/** Paro por empalme*/
void GestorMaquina::sltParoPorEmpalme(){
#ifdef HAPPA
    sltDisableParoMovimiento();
#endif
    QTimer::singleShot(DELAY_OPC_SIGNAL,this,SLOT(sltEnableCondicionesMovimiento()));
    m_memoryMap->m_mapa.m_maquina.EmpalmePerformed=false;
    //QMessageBox::warning(0,"INFORMACION","Verificar estado correcto de impresión y cabezales",QMessageBox::Ok);
}

/** Paro por por error encoder*/
void GestorMaquina::sltParoPorErrorEncoder(){
#ifdef HAPPA
    sltDisableParoMovimiento();
#endif
    QTimer::singleShot(DELAY_OPC_SIGNAL,this,SLOT(sltEnableCondicionesMovimiento()));
    m_memoryMap->m_mapa.m_maquina.EmpalmePerformed=false;
    emit sgnGesMaquinaMessage(tr("No se detecta velocidad. Aplique SETA DE EMERGENCIA, revise paso de aluminio por encoder y posición de Balluf"));
    //In2Message *p=new In2Message(0,"No se detecta velocidad. Aplique SETA DE EMERGENCIA y revise paso de aluminio por encoder",1);
    //p->show();
    //connect(this,SIGNAL(sgnKillIn2Message()),p,SLOT(sltDelete()));
    //QMessageBox::warning(0,"INFORMACION","Verificar estado correcto de impresión y cabezales",QMessageBox::Ok);
}


/** Paro por diametro de entrada*/
void GestorMaquina::sltParoPorDiametroEntrada(){
    if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2){
        sltDisableParoMovimiento();
#ifdef HAPPA
        QTimer::singleShot(DELAY_OPC_SIGNAL,this,SLOT(sltEnableCondicionesMovimiento()));
        QTimer::singleShot(TIMEOUT_BUSCAMACULA+DELAY_OPC_SIGNAL,this,SLOT(sltBuscarMacula()));
#endif
    }
}


/** Inicia el proceso de buscar mácula  */
void GestorMaquina::sltBuscarMacula(){
    sltHabilitarFreno(false);
    m_ClienteOPC->EscribirDato(ITEM_BUSCAR_MACULA,QVariant(1));
    m_ClienteOPC->timedEscribeDato(ITEM_BUSCAR_MACULA,QVariant(0));
}	

/** Establece el tiempo de busqueda de mácula  */
void GestorMaquina::sltSetTiempoBusquedaMacula(QVariant valor){
    m_ClienteOPC->EscribirDato(ITEM_TIEMPO_BUSQUEDA_MACULA,valor);
}


/** Establece la velocidad de busqueda de mácula   */
void GestorMaquina::sltSetVelocidadBusquedaMacula(QVariant valor){
    m_ClienteOPC->EscribirDato(ITEM_VELOCIDAD_BUSQUEDA_MACULA,valor);
}

/** Establece la velocidad de ajuste de mácula   */
void GestorMaquina::sltSetVelocidadAjusteMacula(QVariant valor){
    m_ClienteOPC->EscribirDato(ITEM_VELOCIDAD_AJUSTE_MACULA,valor);
}

/** Habilita el freno electromagnético */
void GestorMaquina::sltFrenoOn(){
    m_ClienteOPC->EscribirDato(ITEM_FRENO_ONOFF,QVariant(1));
}	

/** Deshabilita el freno electromagnético */
void GestorMaquina::sltFrenoOff(){
    m_ClienteOPC->EscribirDato(ITEM_FRENO_ONOFF,QVariant(0));
}

/** Inicia un proceso de jog en dirección de avance */
void GestorMaquina::sltJogMasOn(bool conLampara){
    //JORGE ((Principal*)dummy)->AjustarIntensidadLampara(MOVIMIENTO_JOGMAS);
    if (conLampara)  m_ClienteOPC->EscribirDato(ITEM_LAMPARA_ON,0x01);
    sltHabilitarFreno(true);
    m_ClienteOPC->EscribirDato(ITEM_JOG_MAS,0x01);
}


/** Para un proceso de jog en dirección de avance */
void GestorMaquina::sltJogMasOff(){
    m_ClienteOPC->EscribirDato(ITEM_LAMPARA_ON,0x00);
    m_ClienteOPC->EscribirDato(ITEM_JOG_MAS,0x00);
}

/** Hace un tensado automatico...lo bajamos durante 1.5 segundos y lo subimos*/
void GestorMaquina::sltTensadoAutomatico(){
    /*sltTensarMasOn();
    QTimer::singleShot(10000,this,SLOT(sltTensarMasOff()));*/
    sltDestensarMasOn();
    QTimer::singleShot(1500,this,SLOT(sltDestensarMasOff()));
    QTimer::singleShot(1750,this,SLOT(sltCentrarBalluf()));
}

/** Inicia un proceso de tensar */
void GestorMaquina::sltTensarMasOn(){
    m_ClienteOPC->EscribirDato(ITEM_TENSAR_LAMINA,0x01);
}

/** Inicia un proceso de tensar */
void GestorMaquina::sltTensarMasOff(){
    m_ClienteOPC->EscribirDato(ITEM_TENSAR_LAMINA,0x00);
}
/** Inicia un proceso de destensar */
void GestorMaquina::sltDestensarMasOn(){
    m_ClienteOPC->EscribirDato(ITEM_DESTENSAR_LAMINA,0x01);
}
/** Inicia un proceso de destensar */
void GestorMaquina::sltDestensarMasOff(){
    m_ClienteOPC->EscribirDato(ITEM_DESTENSAR_LAMINA,0x00);
}

/** Inicia un proceso de jog en dirección de retroceso */
void GestorMaquina::sltJogMenosOn(bool conLampara){
    //((Principal*)dummy)->AjustarIntensidadLampara(MOVIMIENTO_JOGMENOS);
    if (conLampara) m_ClienteOPC->EscribirDato(ITEM_LAMPARA_ON,0x01);
    sltHabilitarFreno(false);
    m_ClienteOPC->EscribirDato(ITEM_JOG_MENOS,0x01);
}

/** Impresion reverse true/false */
void GestorMaquina::sltSetImpresionReverse(bool reverse){
    if (reverse) m_ClienteOPC->EscribirDato(ITEM_IMPRESION_REVERSE,0x01);
    else m_ClienteOPC->EscribirDato(ITEM_IMPRESION_REVERSE,0x00);
}

/** Para un proceso de jog en dirección de retroceso */
void GestorMaquina::sltJogMenosOff(){
    m_ClienteOPC->EscribirDato(ITEM_LAMPARA_ON,0x00);
    m_ClienteOPC->EscribirDato(ITEM_JOG_MENOS,0x00);
}

/** Establece la velocidad de jog en dirección de avance */
void GestorMaquina::sltSetVelocidadJogMas(QVariant valor){
    m_ClienteOPC->EscribirDato(ITEM_VELOCIDAD_JOG_MAS,valor);
}

/** Establece la velocidad de jog en dirección de retroceso */
void GestorMaquina::sltSetVelocidadJogMenos(QVariant valor){
    m_ClienteOPC->EscribirDato(ITEM_VELOCIDAD_JOG_MENOS,valor);
}

/** ACK del inicio de impresión */
void GestorMaquina::iniciarImpresionDone(){

	emit sglIniciarImpresionDone();

}

/** ACK de la finalización de impresión*/
void GestorMaquina::finalizarImpresionDone(){

	emit sglFinalizarImpresionDone();

}

/** ACK del encendido del XUSB*/
void GestorMaquina::encenderXUSBDone(){

	emit sglEncenderXUSBDone();

}

/** ACK del apagado del XUSB */
void GestorMaquina::apagarXUSBDone(){

	emit sglApagarXUSBDone();

}

/** ACK del proceso de cambio de bobina de entrada*/
void GestorMaquina::cambioBobinaEntradaDone(){

    emit sglCambioBobinaEntradaDone();

}

/** ACK del proceso de cambio de bobina de salida*/
void GestorMaquina::cambioBobinaSalidaDone(){

    emit sglCambioBobinaSalidaDone();

}	


/** ACK del proceso de búsqueda de mácula*/
void GestorMaquina::buscarMaculaDone(){

	emit sglBuscarMaculaDone();

}	


/** ACK de la habilitación del freno */
void GestorMaquina::habilitarFrenoDone(){

	emit sglHabilitarFrenoDone();

}	

/** ACK de la deshabilitación del freno */
void GestorMaquina::deshabilitarFrenoDone(){

	emit sglDeshabilitarFrenoDone();

}	


/** ACK de la activación del proceso de jog en dirección de avance*/
void GestorMaquina::jogMasOnDone(){

	emit sglJogMasOnDone();

}	


/** ACK de la desactivación del proceso de jog en dirección de avance*/
void GestorMaquina::jogMasOffDone(){

	emit sglJogMasOffDone();

}	

/** ACK de la activación del proceso de jog en dirección de retroceso*/
void GestorMaquina::jogMenosOnDone(){

	emit sglJogMenosOnDone();

}	

/** ACK de la desactivación del proceso de jog en dirección de retroceso*/
void GestorMaquina::jogMenosOffDone(){
	emit sglJogMenosOffDone();
}	


/** Lee la referencia de velocidad de la máquina */
signed short GestorMaquina::sltGetReferenciaVelocidad(){

    signed short refVelocidad;
    refVelocidad=m_ClienteOPC->LeerDato(ITEM_VELOCIDAD_REF).toInt();
    //emit sglGetReferenciaVelocidadRtn(refVelocidad);
    return refVelocidad;
}

/** establece referencia de velocidad de la máquina */
void GestorMaquina::sltSetReferenciaVelocidad(int value){
    m_ClienteOPC->EscribirDato(ITEM_VELOCIDAD_REF,QVariant(value));
}
/** Controla la velocidad de referencia*/
void GestorMaquina::sltSetReferenciaVelocidadControlled(){
    sltMonitorSpeed();
    if ((m_memoryMap->m_mapa.m_maquina.ref_velocidad>MIN_CONTROL_VELOCIDAD)&&
        (m_memoryMap->m_mapa.m_maquina.ref_velocidad>m_memoryMap->m_mapa.m_maquina.velocidad/100)&&
         (m_memoryMap->m_mapa.m_maquina.velocidad/100<MIN_CONTROL_VELOCIDAD)){

        sltHabilitarCalculoDiametro(0); //Deshabilitamos calculo diametro mientras acelera
        m_memoryMap->m_mapa.m_maquina.ref_velocidad_actual=MIN_CONTROL_VELOCIDAD;
        sltSetReferenciaVelocidad(m_memoryMap->m_mapa.m_maquina.ref_velocidad_actual);
        emit SendDebugInfo(LOGGER_WARNING,QString("CONTROL SOBREVELOCIDAD iniciado.Nueva Vel. %1").arg(m_memoryMap->m_mapa.m_maquina.ref_velocidad_actual));
        m_memoryMap->m_mapa.m_maquina.contador_velocidad=0;
        QTimer::singleShot(2500,this,SLOT(sltControlReferenciaVelocidad()));
    }
   else{
        //sltHabilitarCalculoDiametro(1); //Velocidad alcanzada, habilitamos calculo diametro
        emit SendDebugInfo(LOGGER_WARNING,QString("CONTROL SOBREVELOCIDAD no iniciado  [VRef %1][Vact %2][Vmin] %3").arg(m_memoryMap->m_mapa.m_maquina.ref_velocidad).arg(m_memoryMap->m_mapa.m_maquina.velocidad).arg(MIN_CONTROL_VELOCIDAD));
        sltSetReferenciaVelocidad(m_memoryMap->m_mapa.m_maquina.ref_velocidad);
        m_memoryMap->m_mapa.m_maquina.ref_velocidad_actual=m_memoryMap->m_mapa.m_maquina.ref_velocidad;
        QTimer::singleShot(0,this,SLOT(sltControlReferenciaVelocidad()));
    }
}
/** Controla la referencia de velocidad*/
void GestorMaquina::sltControlReferenciaVelocidad(){

    bool bVelocidadAlcanzada=false;
    signed short velocidad_actual=m_memoryMap->m_mapa.m_maquina.velocidad; //velocidad actual en 100*m/min
    static signed short prevVelocidad=0;
    static int counter_desbloqueo=0;
    if (m_memoryMap->m_mapa.m_maquina.ref_velocidad_actual!=m_memoryMap->m_mapa.m_maquina.ref_velocidad){
        if (velocidad_actual>=(m_memoryMap->m_mapa.m_maquina.ref_velocidad_actual-1)*100){
            m_memoryMap->m_mapa.m_maquina.contador_velocidad++;
            //sltHabilitarCalculoDiametro(1);
            if (m_memoryMap->m_mapa.m_maquina.contador_velocidad>=COUNTER_VELOCIDAD){
                m_memoryMap->m_mapa.m_maquina.contador_velocidad=0;
                if (m_memoryMap->m_mapa.m_maquina.ref_velocidad_actual<m_memoryMap->m_mapa.m_maquina.ref_velocidad){
                    m_memoryMap->m_mapa.m_maquina.ref_velocidad_actual++;
                    //miCliente->EscribirDato(ITEM_VELOCIDAD_REF,QVariant(m_memoryMap->m_mapa.m_maquina.ref_velocidad_actual));
                    sltSetReferenciaVelocidad(m_memoryMap->m_mapa.m_maquina.ref_velocidad_actual);
                    //AjustarIntensidadLampara(MOVIMIENTO_AUTOMATICO);
                    emit SendDebugInfo(LOGGER_WARNING,QString("Nueva referencia velocidad: %1").arg(m_memoryMap->m_mapa.m_maquina.ref_velocidad_actual));
                }
                else{
                    //sltHabilitarCalculoDiametro(1);
                    QTimer::singleShot(0,this,SLOT(sltEnableCalculoDiametro()));
                    emit SendDebugInfo(LOGGER_WARNING,QString("Velocidad: %1").arg(m_memoryMap->m_mapa.m_maquina.ref_velocidad_actual));
                    emit SendDebugInfo(LOGGER_WARNING,QString("CONTROL SOBREVELOCIDAD DESACTIVADO"));
                    bVelocidadAlcanzada=true;
                }
            }
        }
    }
    else{ //Solo cuando llego a una horquilla
        if ((velocidad_actual>=(m_memoryMap->m_mapa.m_maquina.ref_velocidad_actual-1)*100)&& (velocidad_actual<=(m_memoryMap->m_mapa.m_maquina.ref_velocidad_actual+1)*100)) {
            QTimer::singleShot(1000,this,SLOT(sltEnableCalculoDiametro()));
            emit SendDebugInfo(LOGGER_WARNING,QString("Alcanzada Velocidad Referencia: %1").arg(m_memoryMap->m_mapa.m_maquina.ref_velocidad));
            bVelocidadAlcanzada=true;
        }
        else if (prevVelocidad==velocidad_actual/100){
            counter_desbloqueo++;
            if (counter_desbloqueo>=3){
                bVelocidadAlcanzada=true;
                counter_desbloqueo=0;
                if (prevVelocidad>0){
                    QTimer::singleShot(1000,this,SLOT(sltEnableCalculoDiametro()));
                    emit SendDebugInfo(LOGGER_WARNING,QString("Alcanzada Velocidad ESTABLE: %1").arg(prevVelocidad));
                }
                else{
                    if (isPrinting()){
                        /*QString txt=QString("WARNING:Maquina sin velocidad");
                        emit SendDebugInfo(LOGGER_ERROR,txt);
                        QMessageBox::warning(0,"ERROR",txt,QMessageBox::Ok);*/
                    }
                    return;
                }
            }

        }
        prevVelocidad=velocidad_actual/100;
    }
    if (!bVelocidadAlcanzada) QTimer::singleShot(1000,this,SLOT(sltControlReferenciaVelocidad()));
    else counter_desbloqueo=0;
}

/** Ajsuta la intensidad de lámpara dinamicamente*/
void GestorMaquina::sltAjustarIntensidadLamparaDinamico(){
    static int PrevReferencia=0;
    int vel=m_memoryMap->m_mapa.m_maquina.velocidad/100;
    if (PrevReferencia!=vel){
        In2GestorLampara *p=In2GestorLampara::request(0);
        if ((p->enabled())&&(vel)){ //Si el gestor está habilitado
            p->newSpeed(vel);
        }
        else{ //Si es el control antiguo...para mantener compatibilidad
            int intensidad=Relacion_VelocidadIntensidadLampara(vel);
            int prevIntensidad=m_memoryMap->m_mapa.m_maquina.intensidad_lampara;
            if (intensidad!=prevIntensidad){
                sltSetIntensidadLampara(QVariant(intensidad));
                m_memoryMap->m_mapa.m_maquina.intensidad_lampara=intensidad;
            }
        }
    }
    PrevReferencia=vel;
}

/** Busca la relación entre veloccidad e intensidad*/
int GestorMaquina::Relacion_VelocidadIntensidadLampara(int Velocidad){

    double I_d=0;
    int ret=0;

    int x1=0,y1=0;
    int x2=0,y2=0;

    QMap<int,int> RefV_I=RefV_Intensidad;

    if (m_memoryMap->m_mapa.m_variables.NumColores==2){
        RefV_I=RefV_Intensidad_2C;
    }

    //Si devuelve -1 es que no está en la lista
    int valor=RefV_I.value(Velocidad,-1);

    if (valor!=-1){
        I_d=valor;
    }
    else{ //Aqui va buscando...no debieramos entrar por tablas nunc aqui
        QMap<int, int>::iterator x1_i = RefV_I.lowerBound(Velocidad);
        QMap<int, int>::iterator x2_i = x1_i--;

        x1=x1_i.key();
        x2=x2_i.key();

        y1=x1_i.value();
        y2=x2_i.value();

        if (x1_i==RefV_I.end()){
            I_d=x2_i.value();
        }
        else if(x2_i==RefV_I.end()){
            I_d=x1_i.value();
        }
        else{
            if (x2==x1) x2=x1+1;
            I_d=y1+(((y2-y1)*(Velocidad-x1))/(x2-x1));
        }

    }

    ret=(int) I_d;
    //((Principal*)GlobalGetMainWindow())->ReceiveDebugInfo(LOGGER_WARNING,QString("[V:%1][I:%2]").arg(Velocidad).arg(ret));
    if (ret>100) ret=100;
    if (ret<0) ret=0;

    return ret;
}

/** Lee las tablas de IntensidadVsVelocidad*/
void GestorMaquina::Leer_Tabla_RefVelocidad_Intensidad(){

    int index=0;
    QString sql=QString("select * from CurvaLampara order by RefVelocidad");


    In2Log db;
    QStringList rslt=db.execSQL(sql);
    if (!rslt.count()) {
        QMessageBox::warning(0,"ERROR (RefVelocidad_Intensidad)","Error accediendo a base de datos",QMessageBox::Ok);
    }
    else{
        RefV_Intensidad.clear();
        emit SendDebugInfo(LOGGER_WARNING,"TABLA INTENSIDAD LAMPARA VS VELOCIDAD OK");
        for (index=0;index<rslt.count();index=index+2){
            RefV_Intensidad.insert(rslt.at(index).toInt(),rslt.at(index+1).toInt());
        }
    }

    sql=QString("select * from CurvaLampara2C order by RefVelocidad");
    rslt=db.execSQL(sql);
    if (!rslt.count()) {
        QMessageBox::warning(0,"ERROR (RefVelocidad_Intensidad2C)","Error accediendo a base de datos",QMessageBox::Ok);
    }
    else{        
        RefV_Intensidad_2C.clear();
        emit SendDebugInfo(LOGGER_WARNING,"TABLA INTENSIDAD LAMPARA (2C) VS VELOCIDAD OK");
        for (index=0;index<rslt.count();index=index+2){
            RefV_Intensidad_2C.insert(rslt.at(index).toInt(),rslt.at(index+1).toInt());
        }
    }
}

/** Desconecta del servidor */
void GestorMaquina::sltDesconectarServidorOPC(){
    m_ClienteOPC->MiServidorOPC->desconectaServidor();
    delete (m_ClienteOPC);
}
/** Habilita/Deshabilita el calculo de diametro*/
void GestorMaquina::sltHabilitarCalculoDiametro(int x){
    //Lo deshabilitamos de momento
    //m_ClienteOPC->EscribirDato(ITEM_ENABLE_CALCULO_DIAMETRO,QVariant(x));
}
/** Habilita/Deshabilita el calculo de diametro*/
void GestorMaquina::sltEnableCalculoDiametro(){
    sltHabilitarCalculoDiametro(1);
}


/** Publica el diametro filtrado*/
void GestorMaquina::sltSetFilteredDiameter(int x){
    m_ClienteOPC->EscribirDato(ITEM_DIAMETRO_ENTRADA_FILTERED,QVariant(x));
}

/** Publica el diametro filtrado*/
void GestorMaquina::sltSetOutputFilteredDiameter(int x){
    m_ClienteOPC->EscribirDato(ITEM_DIAMETRO_SALIDA_FILTERED,QVariant(x));
}


/** Cambia la fuerza del freno electromagnético */
void GestorMaquina::sltSetFuerzaFreno(QVariant valor){
    m_ClienteOPC->EscribirDato(ITEM_FUERZA_FRENO,valor);
}

/** Dexvuelve si está imprimendo*/
bool GestorMaquina::isPrinting(){
    return m_ClienteOPC->LeerDato(ITEM_IMPRESION).toBool();

}

/** Cambia los metros de trabajo */
void GestorMaquina::sltSetMetros(QVariant valor){
    m_ClienteOPC->EscribirDato(ITEM_METROS_REF,valor);
}


/** Cambia diámetro de entrada */
void GestorMaquina::sltSetDiametroEntrada(QVariant valor){
    m_ClienteOPC->EscribirDato(ITEM_DIAMETRO_ENTRADA_REF,valor);
    m_ClienteOPC->EscribirDato(ITEM_FIJAR_DIAMETRO_ENTRADA,QVariant(true));
    m_ClienteOPC->timedEscribeDato(ITEM_FIJAR_DIAMETRO_ENTRADA,QVariant(false));

}

/** Cambia diámetro de salida */
void GestorMaquina::sltSetDiametroSalida(QVariant valor){
    m_ClienteOPC->EscribirDato(ITEM_DIAMETRO_SALIDA_REF,valor);
    m_ClienteOPC->EscribirDato(ITEM_FIJAR_DIAMETRO_SALIDA,QVariant(true));
    m_ClienteOPC->timedEscribeDato(ITEM_FIJAR_DIAMETRO_SALIDA,QVariant(false));
}

/** Cambia diámetro de paro */
void GestorMaquina::sltSetDiametroParo(QVariant valor){
    m_ClienteOPC->EscribirDato(ITEM_DIAMETRO_PARAR,valor);
}

/** Trabajo nuevo de impresión */
void GestorMaquina::sltTrabajoNuevo(){
    m_ClienteOPC->EscribirDato(ITEM_TRABAJO_NUEVO,QVariant(1));
    m_ClienteOPC->timedEscribeDato(ITEM_TRABAJO_NUEVO,QVariant(0));
}

/** Establece la intensidad de lámparara */
void GestorMaquina::sltSetIntensidadLampara(QVariant valor){
    m_ClienteOPC->EscribirDato(ITEM_INTENSIDAD_LAMPARA,valor);
}

/** Establece la tensión de bobina */
void GestorMaquina::sltSetTensionBobina(QVariant valor){
    m_ClienteOPC->EscribirDato(ITEM_TENSION_BOBINA,valor);
}

/** Establece la rampa de aceleración */
void GestorMaquina::sltSetRampaAceleracion(QVariant valor){
    m_ClienteOPC->EscribirDato(ITEM_RAMPA_ACELERACION,valor);
}

/** Establece la rampa de deceleracion */
void GestorMaquina::sltSetRampaDeceleracion(QVariant valor){
    m_ClienteOPC->EscribirDato(ITEM_RAMPA_DECELERACION,valor);
}

/** Establece la distancia de empalme */
void GestorMaquina::sltSetDistanciaEmpalme(QVariant valor){
    m_ClienteOPC->EscribirDato(ITEM_DISTANCIA_EMPALME,valor);
}


/** Ejecuta un empalme */
void GestorMaquina::sltEjecutarEmpalme(){
    emit SendDebugInfo(LOGGER_STORE,"Inicio secuencia empalme");
    emit sgnGesMaquinaMessage(tr("Bajando Balluf...espere"));
    //In2Message *message=new In2Message(0,"Bajando Balluf.Por favor espere...",0);
    //connect(this,SIGNAL(sgnKillIn2Message()),message,SLOT(sltDelete()));
    sltDestensarMasOn();
    QTimer::singleShot(5000,this,SLOT(sltEjecutarEmpalme_1()));

}

/** Ejecuta un empalme */
void GestorMaquina::sltEjecutarEmpalme_1(){
    //emit sgnKillIn2Message();
    emit sgnGesMaquinaMessage(tr(""));


    sltDestensarMasOff();
    m_ClienteOPC->EscribirDato(ITEM_DESHABILITAR_SERVO_ENTRADA,QVariant(1));
    m_ClienteOPC->EscribirDato(ITEM_DESHABILITAR_SERVO_SALIDA,QVariant(1));
    QMessageBox::warning(0,"ATENCION","Pulse continuar cuando termine de empalmar la bobina","Continuar","Cancelar",0,1);
    m_ClienteOPC->EscribirDato(ITEM_DESHABILITAR_SERVO_ENTRADA,QVariant(0));
    m_ClienteOPC->EscribirDato(ITEM_DESHABILITAR_SERVO_SALIDA,QVariant(0));
    QTimer::singleShot(100,this, SLOT(sltEjecutarEmpalme_2()));
}



/** Ejecuta un empalme */
void GestorMaquina::sltEjecutarEmpalme_2(){
    //int respuesta=0;
    int respuesta=QMessageBox::warning(0,"ATENCION","¿Ha cambiado la bobina de SALIDA?Al terminar el proceso compruebe que el diametro de salida es correcto","SI","NO",0, 1 );
    if (respuesta==0){
        m_memoryMap->m_mapa.m_maquina.ref_diametro_bobina_salida=DEFAULT_DIAMETRO_SALIDA;
        ReloadDiametroSalida();
        QString txt=tr("Bobina salida cambiada");
        emit SendDebugInfo(LOGGER_STORE,txt);
        emit sgnGesMaquinaFinEmpalmeSalida();
    }
    else{
        respuesta=QMessageBox::warning(0,"ATENCION","¿Ha cambiado la bobina de ENTRADA?Al terminar el proceso compruebe que el diametro de salida es correcto","SI","NO",0, 1 );
        if (respuesta==0){
            m_memoryMap->m_mapa.m_maquina.ref_diametro_bobina_entrada=DEFAULT_DIAMETRO_ENTRADA;
            m_memoryMap->m_mapa.m_maquina.diametro_bobina_entrada=DEFAULT_DIAMETRO_ENTRADA;
            ReloadDiametroEntrada();
            QString txt=tr("Bobina entrada cambiada");
            emit SendDebugInfo(LOGGER_STORE,txt);
            emit sgnGesMaquinaFinEmpalmeEntrada();
        }
    }
    QTimer::singleShot(100,this, SLOT(sltEjecutarEmpalme_3()));
}

/** Ejecuta un empalme */
void GestorMaquina::sltEjecutarEmpalme_3(){
    /*m_ClienteOPC->EscribirDato(ITEM_DESPLAZAR_FILM,QVariant(1));
    m_ClienteOPC->timedEscribeDato(ITEM_DESPLAZAR_FILM,QVariant(0));*/
    this->sltCentrarBalluf();
    emit sgnGesMaquinaMessage(tr("Centrando Balluf...espere"));
    //In2Message *message=new In2Message(0,"Centrando Balluf.Por favor espere...",0);
    //connect(this,SIGNAL(sgnKillIn2Message()),message,SLOT(sltDelete()));
    QTimer::singleShot(DELAY_OPC_SIGNAL,this, SLOT(sltEjecutarEmpalme_4()));
}
/** Ejecuta un empalme */
void GestorMaquina::sltEjecutarEmpalme_4(){
    int rtn=m_ClienteOPC->LeerDato(ITEM_TENSAR_LAMINA).toInt();
    if (rtn) QTimer::singleShot(500,this, SLOT(sltEjecutarEmpalme_4()));
    else {        
        //emit sgnKillIn2Message();
        emit sgnGesMaquinaMessage(tr(""));
        QTimer::singleShot(500,this, SLOT(sltEjecutarEmpalme_5()));
    }


}
/** Ejecuta un empalme */
void GestorMaquina::sltEjecutarEmpalme_5(){
    //int respuesta=0;
    int respuesta=QMessageBox::question(0,"ATENCION","¿Desea buscar mácula antes de continuar?","Si","No", 0, 1 );
    if (respuesta==0){
    //Hay que buscar mácula
        sltHabilitarFreno(false);
        m_ClienteOPC->EscribirDato(ITEM_BUSCAR_MACULA,QVariant(1));
        m_ClienteOPC->timedEscribeDato(ITEM_BUSCAR_MACULA,QVariant(0));
        emit sgnGesMaquinaMessage(tr("Por favor espere..."));
        //In2Message *message=new In2Message(0,"Por favor espere...",0);
        //connect(this,SIGNAL(sgnKillIn2Message()),message,SLOT(sltDelete()));
        QTimer::singleShot(100,this, SLOT(sltEjecutarEmpalme_6()));
    }
}

/** Ejecuta un empalme */
void GestorMaquina::sltEjecutarEmpalme_6(){
    bool rtn=m_ClienteOPC->LeerDato(ITEM_BUSCANDO_MACULA).toBool();
    if (rtn==false) {
        //emit sgnKillIn2Message();
        emit sgnGesMaquinaMessage(tr(""));        
    }
    else {
        QTimer::singleShot(500,this, SLOT(sltEjecutarEmpalme_6()));
    }
}



/** Bobina nueva */
void GestorMaquina::sltCheckMaxBobina(){
    In2GestorMaquina *p=In2GestorMaquina::request(0);
    p->sltCheckMaxBobina();
}

/** Encender lámpara */
void GestorMaquina::sltLamparaOn(){
    m_ClienteOPC->EscribirDato(ITEM_LAMPARA_ON,0x01);
}

/** Apagar lámpara */
void GestorMaquina::sltLamparaOff(){
    m_ClienteOPC->EscribirDato(ITEM_LAMPARA_ON,0x00);
}

/** Devuelve el valor de tiempo de busqueda de mácula */
int GestorMaquina::sltGetTiempoBusquedaMacula(){
    int rtn=0;
    rtn=m_ClienteOPC->LeerDato(ITEM_TIEMPO_BUSQUEDA_MACULA).toInt();
    return rtn;

}

/** Devuelve el valor de velocidad de busqueda de macula */
int GestorMaquina::sltGetVelocidadBusquedaMacula(){
    int rtn=0;
    rtn=m_ClienteOPC->LeerDato(ITEM_VELOCIDAD_BUSQUEDA_MACULA).toInt();
    return rtn;
}

/** Devuelve el valor de velocidad de ajuste de macula */
int GestorMaquina::sltGetVelocidadAjusteMacula(){
    int rtn=0;
    rtn=m_ClienteOPC->LeerDato(ITEM_VELOCIDAD_AJUSTE_MACULA).toInt();
    return rtn;
}

/** Devuelve el valor de velocidad de jog mas */
int GestorMaquina::sltGetVelocidadJogMas(){
    int rtn=0;
    rtn=m_ClienteOPC->LeerDato(ITEM_VELOCIDAD_JOG_MAS).toInt();
    return rtn;
}

/** Devuelve el valor de velocidad de jog menos */
int GestorMaquina::sltGetVelocidadJogMenos(){
    int rtn=0;
    rtn=m_ClienteOPC->LeerDato(ITEM_VELOCIDAD_JOG_MENOS).toInt();
    return rtn;
}

/** Devuelve el valor de fuerza de freno */
int GestorMaquina::sltGetFuerzaFreno(){
    int rtn=0;
    rtn=m_ClienteOPC->LeerDato(ITEM_FUERZA_FRENO).toInt();
    return rtn;
}

/** Devuelve el valor de metros */
int GestorMaquina::sltGetMetros(){
    int rtn=0;
    rtn=m_ClienteOPC->LeerDato(ITEM_METROS_ACT).toInt();
    return rtn;
}

/** Devuelve el valor de diámetro de entrada */
int GestorMaquina::sltGetDiametroEntrada(){
    int rtn=0;
    rtn=m_ClienteOPC->LeerDato(ITEM_DIAMETRO_ENTRADA_ACT).toInt();
    return rtn;
}

/** Devuelve el valor de diámetro de salida */
int GestorMaquina::sltGetDiametroSalida(){
    int rtn=0;
    rtn=m_ClienteOPC->LeerDato(ITEM_DIAMETRO_SALIDA_ACT).toInt();
    return rtn;
}

/** Devuelve el valor de diámetro de paro */
int GestorMaquina::sltGetDiametroParo(){
    int rtn=0;
    rtn=m_ClienteOPC->LeerDato(ITEM_DIAMETRO_PARAR).toInt();
    return rtn;
}

/** Devuelve el valor de intensidad de lámpara */
int GestorMaquina::sltGetIntensidadLampara(){
    int rtn=0;
    rtn=m_ClienteOPC->LeerDato(ITEM_INTENSIDAD_LAMPARA).toInt();
    return rtn;
}

/** Devuelve el valor de tensión de bobina */
int GestorMaquina::sltGetTensionBobina(){
    int rtn=0;
    rtn=m_ClienteOPC->LeerDato(ITEM_TENSION_BOBINA).toInt();
    return rtn;
}

/** Devuelve el valor de rampa de aceleracion */
int GestorMaquina::sltGetRampaAceleracion(){
    int rtn=0;
    rtn=m_ClienteOPC->LeerDato(ITEM_RAMPA_ACELERACION).toInt();
    return rtn;
}

/** Devuelve el valor de rampa de deceleración */
int GestorMaquina::sltGetRampaDeceleracion(){
    int rtn=0;
    rtn=m_ClienteOPC->LeerDato(ITEM_RAMPA_DECELERACION).toInt();
    return rtn;
}

/** Devuelve el valor de distancia de empalme */
int GestorMaquina::sltGetDistanciaEmpalme(){
    int rtn=0;
    rtn=m_ClienteOPC->LeerDato(ITEM_DISTANCIA_EMPALME).toInt();
    return rtn;
}

/** Devuelve el valor de el estado de la lámpara */
bool GestorMaquina::sltGetLampara(){
    bool rtn=false;
    rtn=m_ClienteOPC->LeerDato(ITEM_LAMPARA_ON).toBool();
    return rtn;
}

/** Devuelve el valor de el estado del freno electromagnético */
bool GestorMaquina::sltGetFreno(){
    bool rtn=false;
    rtn=m_ClienteOPC->LeerDato(ITEM_FRENO_ONOFF).toBool();
    return rtn;
}
/** Ejecuta un full soft stop*/
void GestorMaquina::sltExecFullSoftStop(){
    emit sgnSoftStop();
    m_GestorImpresion->ExecFullSoftStop();
    emit SendDebugInfo(LOGGER_ERROR,"SoftStop");
}

/** Inicia la impresion*/
void GestorMaquina::initImpresion(){
    int numCards=m_GestorImpresion->getNumCards();
    for (int x=0;x<numCards;x++){
        m_GestorImpresion->GeneratePD(x);
    }
    m_ContadorArranque=0;
    //Tensamos automatico
    emit sgnGesMaquinaMessage(tr("INICIANDO MOVIMIENTO..espere"));
    //In2Message *p=new In2Message(0,"INICIANDO MOVIMIENTO");
    //connect(this,SIGNAL(sgnKillIn2Message()),p,SLOT(sltDelete()));
    //connect(this,SIGNAL(sgnUpdateIn2MessageBar(int)),p,SLOT(sltUpdateProgressBar(int)));
    //connect(this,SIGNAL(sgnUpdateIn2Message(QString)),p,SLOT(sltUpdateMessage(QString)));
    //connect(this,SIGNAL(setAutoIn2AutoPB(int)),p,SLOT(sltEnableAutomaticPB(int)));

    //emit sgnUpdateIn2MessageBar(10);
    //connect(this,SIGNAL(sltUpdateIn2Message(int)),p,SLOT(sltUpdateProgressBar(int)));


    //emit sltUpdateIn2Message(10);

    sltHabilitarCalculoDiametro(0);
    TestDiametroMinimoVelocidad();

    int VelArranque=m_memoryMap->m_mapa.m_machinedefaults.VelArranque;
    int TimeArranque=m_memoryMap->m_mapa.m_machinedefaults.TiempoArranque;

    /*if (m_memoryMap->m_mapa.m_maquina.EmpalmePerformed){
        sltSetReferenciaVelocidad(VelArranque);
        sltDisableMarchaMovimiento();
        Insertar_Evento(INICIO_IMPRESION,QString("Inicio impresion"));
        m_memoryMap->m_mapa.m_maquina.EstadoMaquina=ST_IMPRIMIENDO;
        ChangeStyle(THEME_PRINTING);
        emit SendDebugInfo(LOGGER_WARNING,QString("Velocidad arranque %1 durante %2 msg").arg(VelArranque).arg(m_memoryMap->m_mapa.m_machinedefaults.JointTime));
        QTimer::singleShot(JOINT_STOP_TIME,this,SLOT(sltFinalizarImpresion()));
    }
    else{
        emit SendDebugInfo(LOGGER_WARNING,QString("Velocidad arranque %1 durante %2 msg").arg(VelArranque).arg(TimeArranque));
        sltSetReferenciaVelocidad(VelArranque);
        sltDisableMarchaMovimiento();
        Insertar_Evento(INICIO_IMPRESION,QString("Inicio impresion"));
        m_memoryMap->m_mapa.m_maquina.EstadoMaquina=ST_IMPRIMIENDO;
        ChangeStyle(THEME_PRINTING);
        //if (TimeArranque)
        QTimer::singleShot(0,this,SLOT(initImpresion0()));

        //sltHabilitarCalculoDiametro(1);
    }*/

    if (TimeArranque){
        sltSetReferenciaVelocidad(VelArranque);
        emit SendDebugInfo(LOGGER_WARNING,QString("Velocidad arranque %1 durante %2 msg").arg(VelArranque).arg(TimeArranque));
    }
    else{
        emit SendDebugInfo(LOGGER_WARNING,QString("Velocidad arranque deshabilitada"));
    }
    //sltEnableMarchaMovimiento();

    sltDisableMarchaMovimiento();

    //Insertar_Evento(INICIO_IMPRESION,QString("Inicio impresion"));
    m_memoryMap->m_mapa.m_maquina.EstadoMaquina=ST_IMPRIMIENDO;
    ChangeStyle(THEME_PRINTING);
    //if (TimeArranque)
    QTimer::singleShot(500,this,SLOT(initImpresion0()));


}
/** Inicia la impresion-0*/
void GestorMaquina::initImpresion0(){

    //sltDisableMarchaMovimiento();
    int TimeArranque=m_memoryMap->m_mapa.m_machinedefaults.TiempoArranque;
    //int step=(counter*100)/20;

    if (m_memoryMap->m_mapa.m_maquina.velocidad>=(m_memoryMap->m_mapa.m_machinedefaults.VelArranque*100-20)){

        QTimer::singleShot(500,this,SLOT(sltEnableCalculoDiametro()));
        QTimer::singleShot(TimeArranque,this,SLOT(initImpresion1()));
        emit sgnGesMaquinaMessage(tr("[0]INICIANDO MOVIMIENTO..espere %1").arg(m_memoryMap->m_mapa.m_machinedefaults.TiempoArranque/1000));

    }
    else if (TimeArranque==0){

            QTimer::singleShot(500,this,SLOT(sltEnableCalculoDiametro()));
            QTimer::singleShot(TimeArranque,this,SLOT(initImpresion1()));
            QTimer::singleShot(5000,this,SLOT(sltTestVelocidadInicial()));
            emit sgnGesMaquinaMessage(tr("[1]INICIANDO MOVIMIENTO..espere %1").arg(m_memoryMap->m_mapa.m_machinedefaults.TiempoArranque/1000));


    }
    else {
        if (m_ContadorArranque>=INIT_COUNTER_INTERBLOQUEOS*10){
            if (!sltTestVelocidadInicial()){
                m_ContadorArranque=0;
                return;
            }
            emit sgnGesMaquinaMessage(tr("CALCULANDO DIAMETRO durante %1").arg(TIME_INTERBLOQUEO));
            QTimer::singleShot(0,this,SLOT(sltEnableCalculoDiametro()));
            QTimer::singleShot(TIME_INTERBLOQUEO,this,SLOT(initImpresion1()));
            m_ContadorArranque=0;
        }
        else{
            m_ContadorArranque+=INIT_COUNTER_INTERBLOQUEOS;
            QTimer::singleShot(INIT_COUNTER_INTERBLOQUEOS,this,SLOT(initImpresion0()));
        }
    }

    /*int TimeArranque=m_memoryMap->m_mapa.m_machinedefaults.TiempoArranque;
    QTimer::singleShot(500,this,SLOT(sltEnableCalculoDiametro()));
    QTimer::singleShot(TimeArranque,this,SLOT(initImpresion1()));
    QTimer::singleShot(5000,this,SLOT(sltTestVelocidadInicial()));
    emit sgnGesMaquinaMessage(tr("INICIANDO MOVIMIENTO..espere %1").arg(m_memoryMap->m_mapa.m_machinedefaults.TiempoArranque/1000));*/

}
/** Testea la velocidad inicial*/
bool GestorMaquina::sltTestVelocidadInicial(){
    bool rtn=true;
    if (m_memoryMap->m_mapa.m_maquina.velocidad/100<MINIMA_VELOCIDAD_ENCODER){
        emit sgnKillIn2Message();
        sltParoPorErrorEncoder();
        ChangeStyle(THEME_NORMAL);
        sltEnableCondicionesMovimiento();
        rtn=false;
    }
    return rtn;
}

/** Inicia la impresion-1*/
void GestorMaquina::initImpresion1(){
    //emit sgnKillIn2Message();
    emit sgnGesMaquinaMessage(tr(""));
    if (m_memoryMap->m_mapa.m_maquina.EmpalmePerformed){
        emit SendDebugInfo(LOGGER_STORE,"PARO POR EMPALME");
        sltFinalizarImpresion();
    }
    else{
        sltHabilitarCalculoDiametro(0);
        //emit SendDebugInfo(LOGGER_WARNING,QString("Dfiltered[%1]-[%2]").arg(m_memoryMap->m_mapa.m_maquina.diametro_bobina_entrada).arg(m_memoryMap->m_mapa.m_maquina.diametro_bobina_entrada_inst));
        //sltMonitorSpeed();
        sltSetReferenciaVelocidadControlled();

    }
}
/** Testea si hay un diametro mínimo y reajusta la velocidad de referencia*/
void GestorMaquina::TestDiametroMinimoVelocidad(){
    if (m_memoryMap->m_mapa.m_maquina.diametro_bobina_entrada<=DIAMETRO_MINIMO_ARRANQUE){
        emit SendDebugInfo(LOGGER_WARNING,QString("WARNING: Diametro de entrada menor de %1").arg(DIAMETRO_MINIMO_ARRANQUE));
        if (m_memoryMap->m_mapa.m_maquina.ref_velocidad>MAX_VELOCIDAD_DIAMETER_LIMITED){
            emit SendDebugInfo(LOGGER_WARNING,QString("WARNING: Velocidad limitada a %1").arg(MAX_VELOCIDAD_DIAMETER_LIMITED));
            m_memoryMap->m_mapa.m_maquina.ref_velocidad=MAX_VELOCIDAD_DIAMETER_LIMITED;
            sltSetReferenciaVelocidad(m_memoryMap->m_mapa.m_maquina.ref_velocidad);
        }
    }
}

/** Recarga diametro de entrada de bobina*/
void GestorMaquina::ReloadDiametroEntrada(){
    //QLabel *InfoLabel=ShowMensaje("Por favor espere");
    emit sgnGesMaquinaMessage(tr("Fijando diametro de entrada"));
    //In2Message *p=new In2Message(0,"Fijando diametro entrada",1);
    //connect(this,SIGNAL(sgnKillIn2Message()),p,SLOT(sltDelete()));
    //p->show();
    m_ClienteOPC->EscribirDato(ITEM_DESHABILITAR_SERVO_ENTRADA,QVariant(1));
    _Sleep(500);
    QVariant valor=QVariant(m_memoryMap->m_mapa.m_maquina.diametro_bobina_entrada);
    m_ClienteOPC->EscribirDato(ITEM_DIAMETRO_ENTRADA_REF,valor);
    //m_ClienteOPC->EscribirDato(ITEM_DIAMETRO_ENTRADA_ACT,valor);
    _Sleep(500);
    valor=QVariant(true);
    m_ClienteOPC->EscribirDato(ITEM_FIJAR_DIAMETRO_ENTRADA,valor);
    _Sleep(500);
    valor=QVariant(false);
    m_ClienteOPC->EscribirDato(ITEM_FIJAR_DIAMETRO_ENTRADA,valor);
    _Sleep(500);
    m_ClienteOPC->EscribirDato(ITEM_DESHABILITAR_SERVO_ENTRADA,QVariant(0));
    //delete (p);
    //emit sgnKillIn2Message();
    emit sgnGesMaquinaMessage(tr(""));

    //delete InfoLabel;
}


/** Recarga diametro de salida de bobina*/
void GestorMaquina::ReloadDiametroSalida(){
    /*In2Message *p=new In2Message(0,"Fijando diametro salida",1);
    connect(this,SIGNAL(sgnKillIn2Message()),p,SLOT(sltDelete()));
    p->show();*/
    emit sgnGesMaquinaMessage(tr("Fijando diametro salida"));
    m_ClienteOPC->EscribirDato(ITEM_DESHABILITAR_SERVO_SALIDA,QVariant(1));
    _Sleep(500);
    QVariant valor=QVariant(m_memoryMap->m_mapa.m_maquina.ref_diametro_bobina_salida);
    m_ClienteOPC->EscribirDato(ITEM_DIAMETRO_SALIDA_REF,valor);
    _Sleep(500);
    valor=QVariant(true);
    m_ClienteOPC->EscribirDato(ITEM_FIJAR_DIAMETRO_SALIDA,valor);
    _Sleep(500);
    valor=QVariant(false);
    m_ClienteOPC->EscribirDato(ITEM_FIJAR_DIAMETRO_SALIDA,valor);
    _Sleep(500);
    m_ClienteOPC->EscribirDato(ITEM_DESHABILITAR_SERVO_SALIDA,QVariant(0));
    //delete InfoLabel;
    //emit sgnKillIn2Message();
    emit sgnGesMaquinaMessage(tr(""));
    //delete (p);
}

/** Centra el balluf*/
void GestorMaquina::sltCentrarBalluf(){
#ifdef BALLUF
    emit SendDebugInfo(LOGGER_WARNING,QString("GestorMaquina:Centrando Balluf"));
    sltTensarMasOn();
    QTimer::singleShot(500,this,SLOT(sltCentrarBalluf1()));
#endif
}
/** Devuelve balluf centrado o no*/
bool GestorMaquina::sltBallufCentrado(){
#ifdef BALLUF
    return m_ClienteOPC->LeerDato(ITEM_BALLUF_CENTRADO).toBool();
#else
    return true;
#endif
}

/** Comprueba el  centrado del balluf*/
void GestorMaquina::sltCentrarBalluf1(){
    bool centrado=m_ClienteOPC->LeerDato(ITEM_BALLUF_CENTRADO).toBool();
    if (!centrado){
        QTimer::singleShot(500,this,SLOT(sltCentrarBalluf1()));
    }
    else{
        emit SendDebugInfo(LOGGER_WARNING,QString("GestorMaquina:Balluf centrado"));
        sltTensarMasOff();
        In2GestorLampara *p=In2GestorLampara::request(0);
        //p->init();
        QTimer::singleShot(1000,p,SLOT(init()));//No se ha hecho el setup...lo reintento
    }
}
/** Establece condiciones iniciales*/
void GestorMaquina::sltSetCondicionesIniciales(){
    m_ClienteOPC->EscribirDato(ITEM_JOG_MAS,0);
    m_ClienteOPC->EscribirDato(ITEM_JOG_MENOS,0);
    m_ClienteOPC->EscribirDato(ITEM_TENSAR_LAMINA,0);
    m_ClienteOPC->EscribirDato(ITEM_DESTENSAR_LAMINA,0);
    m_ClienteOPC->EscribirDato(ITEM_BUSCAR_MACULA,0);
    m_ClienteOPC->EscribirDato(ITEM_PARO_MAQUINA,0);
    m_ClienteOPC->EscribirDato(ITEM_PARO_MOVIMIENTO,0);
}

