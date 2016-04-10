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
#include "in2gestormaquina.h"
#include "inkjetdebug.h"
#include "in2gestorfluidica.h"
#include "in2counters.h"
#include "in2message.h"
#include "in2managerreport.h"
#include "in2gestorchiller.h"
#include <QMessageBox>
#include <QTimer>
#include <QRgb>
#include "in2gestorerp.h"
#include "in2managerdiametros.h"
#include "in2managererp.h"
#include "in2gestorais.h"
#include "in2gestormantenimiento.h"
#include "in2gestorlampara.h"
#include "in2configuratorlampara.h"

//Timeout de monitorización
#define TIMEOUT_MONITOR_LOOP 1000
#define TIMEOUT_MONITOR_LOOP_SG 1
//Contador de temperatura de lampara
#define TEMP_LAMPARA_COUNTER 1


In2GestorMaquina *In2GestorMaquina::mInstance  = NULL;


/** Singleton*/
In2GestorMaquina *In2GestorMaquina::request(QObject *parent){
    if(mInstance == NULL){
        mInstance = new In2GestorMaquina(parent);
    }
    return mInstance;

}

/** Constructor*/
In2GestorMaquina::In2GestorMaquina(QObject *parent) :
    QObject(parent)
{
    m_memoryMap=MemoryMap::request(0);
    InitGestor();
    m_Printing=false;
    m_startDelayed=false;

}
/** Devuelve si está imprimendo*/
bool In2GestorMaquina::isPrinting(){
    return m_gestorMaquina->isPrinting();
}

/** Inicializa el gestor*/
void In2GestorMaquina::InitGestor(){
    m_MonitorPrinting=false;
    m_gestorMaquina=GestorMaquina::request(this);    
    ClienteOPC *opc=ClienteOPC::request(0);
    connect(opc,SIGNAL(sgnOPCFlanco(int)),this,SLOT(sltFlancoOPC(int)));
    connect(this,SIGNAL(sgnChangeState(int)),this,SLOT(sltChangeTheme(int)));
    connect(m_gestorMaquina,SIGNAL(sgnSoftStop()),this,SLOT(sltSoftStopPerformed()));
    connect(m_gestorMaquina,SIGNAL(sgnGesMaquinaFinEmpalmeEntrada()),this,SIGNAL(sgnGesMaquinaFinEmpalmeEntrada()));
    connect(m_gestorMaquina,SIGNAL(sgnGesMaquinaFinEmpalmeSalida()),this,SIGNAL(sgnGesMaquinaFinEmpalmeSalida()));
    //connect(m_gestorMaquina,SIGNAL(sgnGestorMaquinaFinImpresion()),this,SIGNAL(sgnGestorMaquinaFinImpresion()));
    connect(m_gestorMaquina,SIGNAL(sgnGestorMaquinaFinImpresion()),this,SLOT(sltReportFinImpresion()));

    GestorImpresion *m_print= GestorImpresion::request(0);
    connect(m_print,SIGNAL(errorImpresion(int)),this,SLOT(sltFinalizarImpresion(int)));
    m_PrevTheme=-1;
    sltChangeTheme(THEME_NORMAL);


}



/** Recibe señales del clienteOPC*/
void In2GestorMaquina::sltFlancoOPC(int ID){
    switch (ID){
        case ITEM_SECUENCIA_ON:
        _sltOPCSecuenciaOn();
        break;
        case ITEM_CODIGO_PARADA:
        _sltOPCCodigoParada();
        break;

    }
}


/** Flanco en secuencia_On*/
void In2GestorMaquina::_sltOPCSecuenciaOn(){
    //Comienza la impresion
    if (m_memoryMap->m_mapa.m_maquina.secuencia_on){

        emit SendDebugInfo(LOGGER_WARNING,"_sltOPCSecuenciaOn 1");
        m_gestorMaquina->sltHabilitarFreno(true);
        m_memoryMap->m_mapa.m_maquina.InicioSecuenciaAutomatico=0x01;
        m_memoryMap->m_mapa.m_maquina.EstadoSecuenciaAutomatico=0x01;
        m_memoryMap->m_mapa.m_maquina.ParadaDiametro=false;
        bool condiciones=ComprobacionesPreviasImpresion();
        if (condiciones){
            //Tiene que haber imagen enviada
            if (m_memoryMap->m_ImagenEnviada==false) {
                int respuesta=QMessageBox::question(0,"ATENCION","No hay imagen enviada\n¿Está seguro que desea continuar?","Si","No",QString::null, 0, 1 );
                if (respuesta==0){
                    //En eset caso obviamos el filtro de maculas
                    //m_memoryMap->m_mapa.m_impresion.m_fltrMacula.bMonitorMacula=false;
                    sltLaunchPrint();
                }
                else{
                    //Tiramos de nuevo las condiciones de movimiento
                    m_gestorMaquina->sltEnableCondicionesMovimiento();
                }
            }
            else{
                sltLaunchPrint();
            }
        }
        else{ //No se dan las condiciones de impresion
            //Tiramos de nuevo las condiciones de movimiento
            m_gestorMaquina->sltEnableCondicionesMovimiento();
        }

    }
    else{ //Paramos
        //emit sgnInitImpresion(false);
        emit SendDebugInfo(LOGGER_WARNING,"_sltOPCSecuenciaOn 0");
        m_memoryMap->m_ImagenEnviada=false;
        m_startDelayed=false;
        //Es un softstop
        if (m_memoryMap->m_mapa.m_maquina.codigo_parada==0){
            //Aqui softstopear
            SoftStop();
        }
        else{ //Es una parada de emergencia
            //QString valor_codigo_parada=QString("%1").arg(m_memoryMap->m_mapa.m_maquina.codigo_parada);
            //emit SendDebugInfo(LOGGER_WARNING,QString("Realizando parada %1").arg(valor_codigo_parada));
            //Insertar_Evento(ERROR_MAQUINA,valor_codigo_parada);            
            EmergencyStop(FIN_EMERGENCIA);
            //Se quitan los jogs por si acasol
            GestorMaquina *gM= GestorMaquina::request(this);
            if (gM){
                gM->sltJogMenosOff();
                gM->sltJogMenosOff();
            }
        }
    }
}
/** SoftStop*/
void In2GestorMaquina::SoftStop(){
    if (m_Printing){
        //m_Printing=false;
        //emit SendDebugInfo(LOGGER_WARNING,"SOFTSTOP");
        sltFinalizarImpresion(FIN_PULSADOR);
        //emit sgnChangeState(THEME_NORMAL);
    }
}

/** Ha habidopor alguna otra razón un softstop*/
void In2GestorMaquina::sltSoftStopPerformed(){
    m_Printing=false;
    emit sgnChangeState(THEME_NORMAL);
}

/** EmergencyStop*/
void In2GestorMaquina::EmergencyStop(int mode){
    this->sltFinalizarImpresion(mode);
}

/** Comienza la impresion*/
void In2GestorMaquina::sltLaunchPrint(){    
    emit SendDebugInfo(LOGGER_WARNING,"sltLaunchImpresion");
    emit sgnChangeState(THEME_PRINTING);
    m_Printing=true;
    m_elapsedTimer.start();
    m_memoryMap->m_mapa.m_maquina.metros_Offset=m_memoryMap->m_mapa.m_maquina.metros_trabajo;

    //Un primer diametro correcto
    /*if (m_memoryMap->m_mapa.m_maquina.diametro_bobina_salida<DIAMETRO_ARRANQUE){
        m_gestorMaquina->sltSetDiametroSalida(DIAMETRO_ARRANQUE);
    }*/

    //m_gestorMaquina->sltSetDiametroSalida(DIAMETRO_ARRANQUE);
    In2ManagerDiametros *manager=In2ManagerDiametros::request(0);
    MonitorDiametro *m=manager->getMonitor(IN2MANAGER_DIN);
    if (!m->isEnabled()){
        m_gestorMaquina->sltSetDiametroEntrada(DIAMETRO_ARRANQUE);
    }

    m=manager->getMonitor(IN2MANAGER_DOUT);
    if (!m->isEnabled()){
        m_gestorMaquina->sltSetDiametroSalida(DIAMETRO_ARRANQUE);
    }

    QTimer::singleShot(1000,m_gestorMaquina,SLOT(initImpresion()));
    //m_gestorMaquina->initImpresion();
    QTimer::singleShot(1250,this,SLOT(sltEnableMonitoring()));
}

void In2GestorMaquina::sltEnableMonitoring(){
    sltEnableMonitorPrint(true);
}

/** Devuelve el error de maquina*/
QString In2GestorMaquina::getCodeErrorMaquina(){
    QString txt;
    unsigned char cCodigoParada=(unsigned char)((m_memoryMap->m_mapa.m_maquina.codigo_parada&0xFF00)>>8);
    unsigned char cCodigoParada2=(unsigned char)(((char)m_memoryMap->m_mapa.m_maquina.codigo_parada&0x00FF));
    unsigned int code=cCodigoParada+(cCodigoParada2*256);
    txt.clear();
    if (code!=0) {
        if (code&EMERGENCIA_SETA) txt.append("\nSeta de emergencia");
        if (code&EMERGENCIA_VARSALIDA) txt.append("\nFallo en variador salida");
        if (code&EMERGENCIA_VARENTRADA) txt.append("\nFallo en variador entrada");
        if (code&EMERGENCIA_ARRASTRE) txt.append("\nFallo en arrastre");
        if (code&EMERGENCIA_BANDEJA) txt.append("\nBandeja fuera de posición");
        if (code&EMERGENCIA_MESA) txt.append("\nMesa de impresión fuera de posición");
        if (code&EMERGENCIA_CAPOTA) txt.append("\nCapota abierta");
        if (code&EMERGENCIA_PRESION) txt.append("\nPresión de aire insuficiente");
        if (code&EMERGENCIA_LAMPARA_ELECTRICO) txt.append("\nFallo eléctrico de lámpara ");
        if (code&EMERGENCIA_LAMPARA_APAGADA) txt.append("\nLámpara apagada");
        if (code&EMERGENCIA_DESPLAZADOR_LAMPARA) txt.append("\nDesplazador lámpara");
    }
    return txt;
}

/** Obligamos a que todo esté correcto*/
bool In2GestorMaquina::ComprobacionesPreviasImpresion(){
    bool rtn=true;
    QString comprobaciones;    
    comprobaciones=getCodeErrorMaquina();
    if (!comprobaciones.isEmpty()) rtn=false;

    if (m_memoryMap->m_mapa.m_maquina.metros_trabajo>=m_memoryMap->m_mapa.m_maquina.ref_metros_trabajo){
       comprobaciones.append("\nLongitud de trabajo finalizada");
       rtn=false;
    }

    if (m_memoryMap->m_mapa.m_maquina.diametro_bobina_entrada<m_memoryMap->m_mapa.m_maquina.diametro_parar){
       comprobaciones.append("\nDiámetro de entrada insuficiente");
       rtn=false;
    }

    if (m_memoryMap->m_mapa.m_maquina.temperatura_lampara>m_memoryMap->m_mapa.m_maquina.TemperaturaMaximaLampara){
       comprobaciones.append("\nTemperatura de lámpara excesiva");
       rtn=false;
    }

    In2GestorFluidica *fluidGestor=In2GestorFluidica::request(this);

    for (int x=0; x<fluidGestor->numEquipos();x++){
        if (!fluidGestor->deviceReadyToPrint(x)){
            QString txt=QString("\nLa fluidica %1 no está preparada").arg(x);
            comprobaciones.append(txt);
            rtn=false;
        }
    }

    In2GestorChiller *chillerGestor=In2GestorChiller::request(0,1);
    if (!chillerGestor->isReadyToPrint()){
        QString txt=QString("\nEl refrigerador no está preparado");
        comprobaciones.append(txt);
        rtn=false;
    }

    In2GestorLampara *lampara=In2GestorLampara::request(0);
    if (!lampara->isReadyToPrint()){
        QString txt=QString("\nLa lámpara no está preparada");
        comprobaciones.append(txt);
        rtn=false;
    }

    if (!rtn) QMessageBox::warning(0,"Atención",QString("%1").arg(comprobaciones),QMessageBox::Ok);

    //rtn=true;

    return rtn;
}


/** Flanco en codigo de parada*/
void In2GestorMaquina::_sltOPCCodigoParada(){

    //Es diferente de cero...emergencia
    if (m_memoryMap->m_mapa.m_maquina.codigo_parada){
        //El codigo viene del reves
        unsigned char cCodigoParada=(unsigned char)((m_memoryMap->m_mapa.m_maquina.codigo_parada&0xFF00)>>8);
        unsigned char cCodigoParada2=(unsigned char)(((char)m_memoryMap->m_mapa.m_maquina.codigo_parada&0x00FF));
        unsigned int code=cCodigoParada+(cCodigoParada2*256);
        m_gestorMaquina->sltHabilitarCalculoDiametro(0); //Deshabilitra por sia acaso
        m_gestorMaquina->sltSetCondicionesIniciales();
        //Quitamos imagen ante una emergencia
        GestorImpresion *gI=GestorImpresion::request(0);
        gI->FullDisablePrintMode();
       if (code&EMERGENCIA_SETA) emit SendDebugInfo(LOGGER_ERROR,QString("Seta de emergencia [%1]").arg(code));
        else{
            if (code&EMERGENCIA_VARSALIDA) emit SendDebugInfo(LOGGER_ERROR,QString("Fallo en variador salida [%1]").arg(code));
            if (code&EMERGENCIA_VARENTRADA) emit SendDebugInfo(LOGGER_ERROR,QString("Fallo en variador entrada [%1]").arg(code));
            if (code&EMERGENCIA_ARRASTRE) emit SendDebugInfo(LOGGER_ERROR,QString("Fallo en arrastre [%1]").arg(code));
            if (code&EMERGENCIA_BANDEJA) emit SendDebugInfo(LOGGER_ERROR,QString("Bandeja fuera de posición [%1]").arg(code));
            if (code&EMERGENCIA_MESA) emit SendDebugInfo(LOGGER_ERROR,QString("Mesa de impresión fuera de posición [%1]").arg(code));
            if (code&EMERGENCIA_CAPOTA) emit SendDebugInfo(LOGGER_ERROR,QString("Capota abierta [%1]").arg(code));
            if (code&EMERGENCIA_PRESION) emit SendDebugInfo(LOGGER_ERROR,QString("Presión de aire insuficiente [%1]").arg(code));
            if (code&EMERGENCIA_LAMPARA_ELECTRICO) emit SendDebugInfo(LOGGER_ERROR,QString("Fallo eléctrico de lámpara [%1]").arg(code));
            if (code&EMERGENCIA_LAMPARA_APAGADA) emit SendDebugInfo(LOGGER_ERROR,QString("Lámpara apagada [%1]").arg(code));
            if (code&EMERGENCIA_DESPLAZADOR_LAMPARA) emit SendDebugInfo(LOGGER_ERROR,QString("Desplazador lámpara [%1]").arg(code));
        }
        //QString valor_codigo_parada=QString("%1").arg(code);
        //Insertar_Evento(PARO_MAQUINA,valor_codigo_parada);
        emit sgnShowLog();
        //ChangeStyle(THEME_EMERGENCY);
        emit sgnChangeState(THEME_EMERGENCY);

    }
    else{
        emit SendDebugInfo(LOGGER_ERROR,QString("Las alarmas han sido reestablecidas"));
        emit sgnChangeState(THEME_NORMAL);
        m_gestorMaquina->sltTensadoAutomatico();
    }
}


/** Secuencia de empalme*/
void In2GestorMaquina::InitEmpalme(){
    m_gestorMaquina->sltEjecutarEmpalme();
}

/** Habilita/Deshabilita la monitorizacion de impresion*/
void In2GestorMaquina::sltEnableMonitorPrint(bool val){
    m_MonitorPrinting=val;
    if (m_MonitorPrinting){
        emit SendDebugInfo(LOGGER_ERROR,"sltEnableMonitorPrint TRUE");
        emit SendDebugInfo(LOGGER_STORE,QString("MonitorDelayed: %1 msg").arg(m_memoryMap->m_mapa.m_machinedefaults.delayedMonitorTime));
        m_memoryMap->m_mapa.m_maquina.iContadorParada=0;
        m_startDelayed=false;
        m_maxMaculaCounter=0;
        m_minMaculaCounter=0;
        QTimer::singleShot(0,this,SLOT(sltMonitorPrintLoop()));
        QTimer::singleShot(m_memoryMap->m_mapa.m_machinedefaults.delayedMonitorTime,this,SLOT(sltMonitorDelayed()));
    }
    else{
        emit SendDebugInfo(LOGGER_ERROR,"sltEnableMonitorPrint FALSE");
    }

    if (m_memoryMap->m_mapa.m_impresion.bMonitorHead){
        GestorImpresion *gestorImpresion=GestorImpresion::request(0);
        gestorImpresion->sltMonitorHeads(val);

    }
}
/** Inicia el monitor delayed*/
void In2GestorMaquina::sltMonitorDelayed(){
    m_startDelayed=true;
    emit SendDebugInfo(LOGGER_STORE,QString("MonitorDelayed:TRUE"));
}


/** Bucle de monitorizacion de impresion*/
void In2GestorMaquina::sltMonitorPrintLoop(){
    static int counter=0;
    if (m_MonitorPrinting){
        //_sltMonitorUsoLampara();
        _sltMonitorMetrosImpresos();
        _sltMonitorDiametroEntrada();
        _sltMonitorDistanciaMaculas();
        //_sltMonitorSeguridadLampara();
        /*if (counter%5) {
            _sltMonitorElectronica();
            counter=0;
        }*/

        _sltMonitorTemperaturaLampara();

        //Testea si hay que seguir monitorizando
        if (m_MonitorPrinting){
            counter++;
            QTimer::singleShot(TIMEOUT_MONITOR_LOOP,this,SLOT(sltMonitorPrintLoop()));
        }
    }
}
/** Monitoriza por seguridad que la velocidad no haya caido*/
/*void In2GestorMaquina::_sltMonitorSeguridadLampara(){
    if (m_memoryMap->m_mapa.m_maquina.velocidad<150){
        ClienteOPC *opc=ClienteOPC::request(0);
        opc->EscribirDato(ITEM_LAMPARA_ON,0x00);
        sltEnableMonitorPrint(false);
        EmergencyStop(FIN_LAMINA_ROTA);
    }
}*/

/** Filtra la distancia rntre maculas*/
void In2GestorMaquina::_sltMonitorDistanciaMaculas(){
        //if ((m_memoryMap->m_mapa.m_maquina.iMaxMacula>MIN_MACULA)||(m_memoryMap->m_mapa.m_maquina.iMinMacula>MIN_MACULA)) return;
        double targetSup=_getMaculaMax();
        double targetMin=_getMaculaMin();
        bool rtn=false;
        int type=MACULA_MAXIMA;

        if ((m_memoryMap->m_mapa.m_maquina.iMaxMacula+MACULA_OFFSET)>targetSup){
            if (m_memoryMap->m_mapa.m_impresion.m_fltrMacula.bMonitorMacula){
                if (m_memoryMap->m_mapa.m_variables.MaculaIzqdaRepeticiones>1){
                    //m_memoryMap->m_mapa.m_impresion.m_fltrMacula.fMaxFiltered=targetSup;
                    type=MACULA_MAXIMA;
                    rtn=true;
                }
                else{
                    /*double foo=m_memoryMap->m_mapa.m_impresion.m_fltrMacula.fMax-ERROR_MACULA_MAX;
                    m_memoryMap->m_mapa.m_impresion.m_fltrMacula.fMaxFiltered=foo;
                    m_maxMaculaCounter++;*/
                }
            }
        }
        else{
            double foo=m_memoryMap->m_mapa.m_impresion.m_fltrMacula.fMax-ERROR_MACULA_MAX;
            m_memoryMap->m_mapa.m_impresion.m_fltrMacula.fMaxFiltered=foo;
        }
        if (((m_memoryMap->m_mapa.m_maquina.iMinMacula+MACULA_OFFSET)<targetMin)&&(m_memoryMap->m_mapa.m_maquina.iMinMacula>0)){
            m_memoryMap->m_mapa.m_impresion.m_fltrMacula.fMinFiltered=(m_memoryMap->m_mapa.m_maquina.iMinMacula)/1000;
            //emit SendDebugInfo(LOGGER_ERROR,tr("Limite inferior macula [%1/%2]").arg(m_memoryMap->m_mapa.m_impresion.m_fltrMacula.fMinFiltered).arg(targetMin/1000));
            if (m_memoryMap->m_mapa.m_impresion.m_fltrMacula.bMonitorMacula){
                if (m_memoryMap->m_mapa.m_variables.MaculaIzqdaRepeticiones>1){

                    type=MACULA_MINIMA;
                    rtn=true;
                }
                else{
                    /*double foo=m_memoryMap->m_mapa.m_impresion.m_fltrMacula.fMin+ERROR_MACULA_MIN;
                    m_memoryMap->m_mapa.m_impresion.m_fltrMacula.fMinFiltered=foo/1000;*/
                }
            }
        }
        else{
            double foo=m_memoryMap->m_mapa.m_impresion.m_fltrMacula.fMin+ERROR_MACULA_MIN;
            m_memoryMap->m_mapa.m_impresion.m_fltrMacula.fMinFiltered=foo;
        }

        //emit SendDebugInfo(LOGGER_WARNING,QString("%1-%2-%3-%4").arg(m_memoryMap->m_mapa.m_maquina.iMaxMacula+MACULA_OFFSET).arg(targetSup).arg(m_memoryMap->m_mapa.m_maquina.iMinMacula+MACULA_OFFSET).arg(targetMin));

        if (rtn){
            if ((m_memoryMap->m_mapa.m_machinedefaults.filtroMaculas)&&(m_startDelayed)){
                sltFinalizarImpresion(FIN_MACULA);
                emit sgnFinMacula(type);
                if (type==MACULA_MINIMA)
                    emit SendDebugInfo(LOGGER_ERROR,tr("Limite inferior macula [%1/%2]").arg(m_memoryMap->m_mapa.m_impresion.m_fltrMacula.fMinFiltered).arg(targetMin/1000));
                else
                    SendDebugInfo(LOGGER_ERROR,tr("Limite superior macula [%1/%2]").arg(m_memoryMap->m_mapa.m_impresion.m_fltrMacula.fMaxFiltered).arg(targetSup/1000));

            }
        }
}


/** Filtra la distancia rntre maculas*/
/*void In2GestorMaquina::_sltMonitorDistanciaMaculas(){
        //if ((m_memoryMap->m_mapa.m_maquina.iMaxMacula>MIN_MACULA)||(m_memoryMap->m_mapa.m_maquina.iMinMacula>MIN_MACULA)) return;
        double targetSup=_getMaculaMax();
        double targetMin=_getMaculaMin();
        bool rtn=false;
        int type=MACULA_MAXIMA;

        if ((m_memoryMap->m_mapa.m_maquina.iMaxMacula+MACULA_OFFSET)>targetSup){
            m_memoryMap->m_mapa.m_impresion.m_fltrMacula.fMaxFiltered=(m_memoryMap->m_mapa.m_maquina.iMaxMacula)/1000;
            //emit SendDebugInfo(LOGGER_ERROR,tr("Limite superior macula [%1/%2]").arg(m_memoryMap->m_mapa.m_impresion.m_fltrMacula.fMaxFiltered).arg(targetSup/1000));
            if (m_memoryMap->m_mapa.m_impresion.m_fltrMacula.bMonitorMacula){
                if (m_maxMaculaCounter>=MACULA_FILTER){
                    m_maxMaculaCounter=0;
                    m_minMaculaCounter=0;
                    type=MACULA_MAXIMA;
                    rtn=true;
                }
                else{
                    double foo=m_memoryMap->m_mapa.m_impresion.m_fltrMacula.fMax-ERROR_MACULA_MAX;
                    m_memoryMap->m_mapa.m_impresion.m_fltrMacula.fMaxFiltered=foo;
                    m_maxMaculaCounter++;
                }
            }
        }
        else{
            double foo=m_memoryMap->m_mapa.m_impresion.m_fltrMacula.fMax-ERROR_MACULA_MAX;
            m_memoryMap->m_mapa.m_impresion.m_fltrMacula.fMaxFiltered=foo;


        }
        if (((m_memoryMap->m_mapa.m_maquina.iMinMacula+MACULA_OFFSET)<targetMin)&&(m_memoryMap->m_mapa.m_maquina.iMinMacula>0)){
            m_memoryMap->m_mapa.m_impresion.m_fltrMacula.fMinFiltered=(m_memoryMap->m_mapa.m_maquina.iMinMacula)/1000;
            //emit SendDebugInfo(LOGGER_ERROR,tr("Limite inferior macula [%1/%2]").arg(m_memoryMap->m_mapa.m_impresion.m_fltrMacula.fMinFiltered).arg(targetMin/1000));
            if (m_memoryMap->m_mapa.m_impresion.m_fltrMacula.bMonitorMacula){
                if (m_minMaculaCounter>=MACULA_FILTER){
                    m_minMaculaCounter=0;
                    m_maxMaculaCounter=0;
                    type=MACULA_MINIMA;
                    rtn=true;
                }
                else{
                    m_minMaculaCounter++;
                    double foo=m_memoryMap->m_mapa.m_impresion.m_fltrMacula.fMin+ERROR_MACULA_MIN;
                    m_memoryMap->m_mapa.m_impresion.m_fltrMacula.fMinFiltered=foo;
                }
            }
        }
        else{
            double foo=m_memoryMap->m_mapa.m_impresion.m_fltrMacula.fMin+ERROR_MACULA_MIN;
            m_memoryMap->m_mapa.m_impresion.m_fltrMacula.fMinFiltered=foo;
        }

        //emit SendDebugInfo(LOGGER_WARNING,QString("%1-%2-%3-%4").arg(m_memoryMap->m_mapa.m_maquina.iMaxMacula+MACULA_OFFSET).arg(targetSup).arg(m_memoryMap->m_mapa.m_maquina.iMinMacula+MACULA_OFFSET).arg(targetMin));

        if (rtn){
            if ((m_memoryMap->m_mapa.m_machinedefaults.filtroMaculas)&&(m_startDelayed)){
                sltFinalizarImpresion(FIN_MACULA);
                emit sgnFinMacula(type);
                if (type==MACULA_MINIMA)
                    emit SendDebugInfo(LOGGER_ERROR,tr("Limite inferior macula [%1/%2]").arg(m_memoryMap->m_mapa.m_impresion.m_fltrMacula.fMinFiltered).arg(targetMin/1000));
                else
                    SendDebugInfo(LOGGER_ERROR,tr("Limite superior macula [%1/%2]").arg(m_memoryMap->m_mapa.m_impresion.m_fltrMacula.fMaxFiltered).arg(targetSup/1000));

            }
        }    
}*/
/** Devuelve la distancia máxima entre maculas permitida*/
double In2GestorMaquina::_getMaculaMax(){
    return 1000*(m_memoryMap->m_mapa.m_impresion.m_fltrMacula.fMax);
}
/** Devuelve la distancia minima entre maculas permitida*/
double In2GestorMaquina::_getMaculaMin(){
    return 1000*(m_memoryMap->m_mapa.m_impresion.m_fltrMacula.fMin);
}


/** Testea si la electronica va bien*/
void In2GestorMaquina::_sltMonitorElectronica(){
    if (m_memoryMap->m_mapa.m_maquina.velocidad){
        GestorImpresion *gestor=GestorImpresion::request(0);
        int cards=gestor->getNumCards();
        int heads=m_memoryMap->m_mapa.m_variables.DefaultNumHeads;
        unsigned int rtn=1;
        for (int x=0;x<cards;x++){
            if (rtn==0) break;
            for (int y=0;y<heads;y++){
                rtn=gestor->checkSerialHead(x,y);
                if (rtn==0) break;
            }
        }
        if (rtn==0){
            QString txt=QString("Fallo en electrónica");
            emit SendDebugInfo(LOGGER_ERROR,txt);
            //Insertar_Evento(ERROR_LAMPARA,txt);
            EmergencyStop(FIN_ELECTRONICA);
        }
    }
}


/** Finalizar la impresion en todos sus modos*/
void In2GestorMaquina::sltFinalizarImpresion(int mode){
    if (!m_MonitorPrinting) return;
    QString txt;
    bool isEmergency=true;
    m_gestorMaquina->sltHabilitarCalculoDiametro(0);
    m_memoryMap->m_mapa.m_maquina.modoParo=mode;
    sltEnableMonitorPrint(false);
    unsigned char cCodigoParada=(unsigned char)((m_memoryMap->m_mapa.m_maquina.codigo_parada&0xFF00)>>8);
    unsigned char cCodigoParada2=(unsigned char)(((char)m_memoryMap->m_mapa.m_maquina.codigo_parada&0x00FF));
    unsigned int code=cCodigoParada+(cCodigoParada2*256);
    double maxMacula=_getMaculaMax();
    double minMacula=_getMaculaMin();
    switch (mode){
        case FIN_EMERGENCIA:
            txt=QString("Parada:Emergencia [%1]").arg(code);
            if (code&EMERGENCIA_SETA) txt.append(tr("[SETA]"));
            if (code&EMERGENCIA_VARSALIDA) txt.append(tr("[VARSALIDA]"));
            if (code&EMERGENCIA_VARENTRADA) txt.append(tr("[VARENTRADA]"));
            if (code&EMERGENCIA_ARRASTRE) txt.append(tr("[ARRASTRE]"));
            if (code&EMERGENCIA_BANDEJA) txt.append(tr("[BANDEJA]"));
            if (code&EMERGENCIA_MESA) txt.append(tr("[MESA]"));
            if (code&EMERGENCIA_CAPOTA) txt.append(tr("[CAPOTA]"));
            if (code&EMERGENCIA_PRESION) txt.append(tr("[PRESION AIRE]"));
            if (code&EMERGENCIA_LAMPARA_ELECTRICO) txt.append(tr("[LAMPARA.FALLO ELECTRICO]"));
            if (code&EMERGENCIA_LAMPARA_APAGADA) txt.append(tr("[LAMPARA APAGADA]"));
            if (code&EMERGENCIA_DESPLAZADOR_LAMPARA) txt.append(tr("[DESPLAZADOR LAMPARA]"));
        break;
        case FIN_LAMPARA:           
            txt=QString("Parada:Temperatura Lámpara");
            break;
        case FIN_SAI:
            txt=QString("Parada: SAI");
            break;
        case FIN_PULSADOR:
            txt=QString("Parada: Pulsador");
            isEmergency=false;
            break;
        case FIN_METROS:
            txt=QString("Parada: Metros");
            isEmergency=false;
            emit sgnFinTrabajo();
            break;
        case FIN_DIAMETRO:
            txt=QString("Parada: Diametro");
            isEmergency=false;
            break;
         case FIN_ELECTRONICA:
            txt=QString("Parada: Electrónica");
            break;
         case FIN_CABEZALES:
            txt=QString("Parada: Cabezales");
            break;
         case FIN_MACULA:
            txt=QString("Parada: Macula.MAX (%1/%2); Macula.MIN (%3/%4)")
                .arg(m_memoryMap->m_mapa.m_maquina.iMaxMacula)
                .arg(maxMacula)
                .arg(m_memoryMap->m_mapa.m_maquina.iMinMacula)
                .arg(minMacula);
            isEmergency=false;
            break;
         case FIN_VISION:
            txt=QString("Parada: VISION [%1]").arg(m_extMessage);
            isEmergency=false;
            break;
         case FIN_LAMINA_ROTA:
           txt=QString("Parada: LAMINA ROTA");
           break;

    }
    emit sgnFinImpresion(mode);
    emit sgnIn2GestorMaquinaMessage(txt);
    emit SendDebugInfo(LOGGER_ERROR,txt);
    GestorImpresion *p=GestorImpresion::request(this);
    if (isEmergency){
        ChangeStyle(THEME_EMERGENCY);        
        p->FullDisablePrintMode();
        m_gestorMaquina->sltEnableCondicionesMovimiento();
        emit sgnChangeState(THEME_EMERGENCY);
    }
    else{
        m_gestorMaquina->sltFinalizarImpresion();
        emit sgnChangeState(THEME_NORMAL);
    }
    //Guardo en el historico
    if (m_Printing){
        //Genero una entrada de parada

        //m_MonitorPrinting=false;
        m_Printing=false;
        In2Counters newRecord;
        In2Trabajo p;
        p.bActive=true;
        p.strCompoFile=m_memoryMap->m_mapa.m_variables.NombreRutaIni;
        p.iPrinted=m_memoryMap->m_mapa.m_maquina.metros_trabajo-m_memoryMap->m_mapa.m_maquina.metros_Offset;
        //p.iPrinted=m_memoryMap->m_mapa.m_maquina.metros_trabajo;
        p.m_OT=m_memoryMap->m_mapa.m_variables.OrdenFabricacion;
        p.elapsedTime=m_elapsedTimer.elapsed();
        m_memoryMap->m_mapa.m_maquina.seg_lampara_on+=p.elapsedTime/1000;
        newRecord.addRecord(p.strCompoFile,p.elapsedTime,p.iPrinted,1,txt,p.m_OT);
        In2GestorMantenimiento g;
        g.updateSegundosLampara(m_memoryMap->m_mapa.m_maquina.seg_lampara_on);

        QString resumen=QString("%1 [OT %2] [%3 sg] [%4 m]").arg(p.strCompoFile).arg(p.m_OT).arg(p.elapsedTime/1000).arg(p.iPrinted);
        emit SendDebugInfo(LOGGER_ERROR,resumen);
        //Hacemos una marca de parada

        int iPrinted=(m_memoryMap->m_mapa.m_variables.WebHeight*m_memoryMap->m_mapa.m_maquina.iCounter)/1000;
        int iUsed=p.iPrinted;
        if (iPrinted<iUsed*CORRECION_METROS) iPrinted=iUsed*CORRECION_METROS;


        QString dateTime=QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss");
        QString parada=QString("%1<@>User:%2<@>Tiempo:%3 sg<@>%4<@>Metros Gastados:%5<@>Metros impresos:%6")
                .arg(dateTime)
                .arg(m_memoryMap->getUser())
                .arg(p.elapsedTime/1000)
                .arg(txt)
                .arg(iUsed)
                .arg(iPrinted);

        emit SendDebugInfo(LOGGER_PARADA,parada);

        In2ManagerReport *report=new In2ManagerReport(0);
        report->setTrabajo(p);
        if (report->getAuto()){
            QThread *t=new QThread;
            t->start();
            connect(report,SIGNAL(In2ReportFinished()),report,SLOT(deleteLater()));
            report->moveToThread(t);
            QTimer::singleShot(0,report,SLOT(sltGenerateReport()));
        }
    }
    p->deleteLater();
    //Devilvemos el esatdo original del filtro de maculas
    m_memoryMap->m_mapa.m_impresion.m_fltrMacula.bMonitorMacula=m_memoryMap->m_mapa.m_machinedefaults.filtroMaculas;

}

void In2GestorMaquina::sltReportFinImpresion(){
    emit SendDebugInfo(LOGGER_STORE,tr("Metros maquina %1").arg(m_memoryMap->m_mapa.m_maquina.metros_trabajo));
    emit SendDebugInfo(LOGGER_STORE,tr("Metros impresos %1").arg(m_memoryMap->m_mapa.m_variables.WebHeight*m_memoryMap->m_mapa.m_maquina.iCounter/1000));
    emit sgnGestorMaquinaFinImpresion();
}


/** Testea metros impresos*/
void In2GestorMaquina::_sltMonitorMetrosImpresos(){
    //Testea paro por metros
    if (m_memoryMap->m_mapa.m_maquina.metros_trabajo>=m_memoryMap->m_mapa.m_maquina.ref_metros_trabajo){
        sltFinalizarImpresion(FIN_METROS);
    }


}
/** Testea diametro entrada*/
void In2GestorMaquina::_sltMonitorDiametroEntrada(){
    //Testea paro por diametro de entrada
    if (m_memoryMap->m_mapa.m_maquina.diametro_bobina_entrada<=m_memoryMap->m_mapa.m_maquina.diametro_parar){
        if ((qAbs(m_memoryMap->m_mapa.m_maquina.velocidad/100-m_memoryMap->m_mapa.m_maquina.ref_velocidad)>=DISCREPANCIA_VELOCIDAD)&&(m_memoryMap->m_mapa.m_maquina.velocidad/100>5)){
            SendDebugInfo(LOGGER_WARNING,QString("Discrepancia diametro entrada acelerando"));
            m_memoryMap->m_mapa.m_maquina.iContadorParada=0;
        }
        else{
            m_memoryMap->m_mapa.m_maquina.iContadorParada++;
            emit SendDebugInfo(LOGGER_ERROR,QString("Diametro de entrada insuficiente %1").arg(m_memoryMap->m_mapa.m_maquina.iContadorParada));
            //3 errores consecutivos hacen parada
            if (m_memoryMap->m_mapa.m_maquina.iContadorParada>=CONTADOR_ERROR_DIAMETRO){
                //emit SendDebugInfo(LOGGER_ERROR,"PARADA Diametro de entrada");
                m_memoryMap->m_mapa.m_maquina.ParadaDiametro=true;
                //Insertar_Evento(PARO_MAQUINA,QString("PARADA Diametro de entrada"));
                m_memoryMap->m_mapa.m_maquina.iContadorParada=0;
                //sltEnableMonitorPrint(false);
                sltFinalizarImpresion(FIN_DIAMETRO);
            }
        }
    }
    else m_memoryMap->m_mapa.m_maquina.iContadorParada=0;
}

/** Testea uso de lámpara*/
void In2GestorMaquina::_sltMonitorUsoLampara(){
    if (m_memoryMap->m_mapa.m_maquina.m_opc.estado==1){
        if (m_memoryMap->m_mapa.m_maquina.intensidad_lampara){
            m_memoryMap->m_mapa.m_maquina.seg_lampara_on+=TIMEOUT_MONITOR_LOOP_SG;
        }
    }
}

/** Testea temperatura de lámpara*/
void In2GestorMaquina::_sltMonitorTemperaturaLampara(){
    if (m_memoryMap->m_mapa.m_maquina.m_opc.estado==1){
        In2GestorLampara *lamp=In2GestorLampara::request(0);
        if (lamp->vendor()==PHOSEON){
            if (m_memoryMap->m_mapa.m_maquina.temperatura_lampara>m_memoryMap->m_mapa.m_maquina.TemperaturaMaximaLampara){
                m_memoryMap->m_mapa.m_maquina.ContadorTempLampara++;
                if (m_memoryMap->m_mapa.m_maquina.ContadorTempLampara>=TEMP_LAMPARA_COUNTER){
                    //A los 5 segundos de estar por encima de una Tª, stopeamos
                    EmergencyStop(FIN_LAMPARA);
                    emit SendDebugInfo(LOGGER_ERROR,"Temperatura de lámpara excesiva");
                    //Insertar_Evento(ERROR_LAMPARA,"PARADA Temperatura excesiva");
                    m_memoryMap->m_mapa.m_maquina.ContadorTempLampara=0;
                }
            }
        }
        else{
            m_memoryMap->m_mapa.m_maquina.ContadorTempLampara=0;
        }
    }
}

/** Devuelve el theme*/
int In2GestorMaquina::getTheme(){
    return m_PrevTheme;
}

/** Cambia los colores de aplicacion*/
void In2GestorMaquina::sltChangeTheme(int theme){
    static int Flagfluid=THEME_FLUID_OK;
    //if (m_PrevTheme!=theme){
    QPalette palette=QApplication::palette();
    QColor newColor=palette.color(QPalette::Background);
    QColor foreColor=palette.color(QPalette::Foreground);
        switch (theme){
            case THEME_CLEANING:
                newColor=QColor(Qt::darkBlue);
                foreColor=QColor(Qt::white);
                break;
            case THEME_EMERGENCY:
                newColor=QColor(Qt::red);
                foreColor=QColor(Qt::black);
                break;
            case THEME_NORMAL:
                foreColor=QColor(Qt::black);
                if (Flagfluid==THEME_FLUID_OK)
                    newColor=QColor(Qt::lightGray);
                else
                    newColor=QColor(255,165,37);
                break;
            case THEME_PRINTING:
                newColor=QColor(Qt::darkGreen);
                foreColor=QColor(Qt::white);
                break;
            case THEME_FLUID_WARNING:
                if ((m_PrevTheme==THEME_EMERGENCY)||(m_PrevTheme==THEME_CLEANING)){
                    theme=m_PrevTheme;
                }
                else newColor=QColor(255,165,37);
                Flagfluid=THEME_FLUID_WARNING;
                foreColor=QColor(Qt::black);
                break;
             case THEME_FLUID_OK:
                if ((m_PrevTheme==THEME_EMERGENCY)||(m_PrevTheme==THEME_CLEANING)){
                    theme=m_PrevTheme;
                }
                else newColor=QColor(Qt::lightGray);
                Flagfluid=THEME_FLUID_OK;
                foreColor=QColor(Qt::black);
            break;
        }


        palette.setColor(QPalette::Background,newColor);
        //palette.setColor(QPalette::Foreground,foreColor);
        QApplication::setPalette(palette);
        m_PrevTheme=theme;
    //}
}

/** Cambia los metros de referencia*/
void In2GestorMaquina::sltChangeMetrosReferencia(int metros){
    m_gestorMaquina->sltSetMetros(QVariant(metros));
    m_memoryMap->m_mapa.m_maquina.ref_metros_trabajo=metros;
    emit sgnUpdateReferences();
}

/** Trabajo nuevo*/
void In2GestorMaquina::sltTrabajoNuevo(){

    m_gestorMaquina->sltTrabajoNuevo();
}

/** Analiza vision*/
void In2GestorMaquina::sltVisionMessage(QString txt){
    m_extMessage=txt;
    if ((m_memoryMap->m_mapa.m_impresion.bMonitorVision)&&(m_MonitorPrinting)){
        if (m_startDelayed){
            //sltEnableMonitorPrint(false);
            sltFinalizarImpresion(FIN_VISION);
        }
    }
}
/** Chequea maximo bobina*/
void In2GestorMaquina::sltCheckMaxBobina(){
    GestorMaquina *p=GestorMaquina::request(0);
    p->sltCheckMaxBobina();
}
/** Ha empezado o mo*/
bool In2GestorMaquina::startDelayed(){
    return m_startDelayed;
}
