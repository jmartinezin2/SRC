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

#include <QMouseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QTime>
#include <QStringList>
#include <Windows.h>
#include <QPalette>
#include <QProcess>
#include "principal.h"
#include "configuracion.h"
#include "teclado.h"
#include <string.h>
#include <QSettings>
#include "memoryMap.h"
#include "composicion.h"
#include "maquina.h"
#include "Fluidica.h"
#include "smtp.h"
#include "status.h"
#include "clienteopc.h"
#include "inkjetdebug.h"
#include "inkjetinformer.h"
#include "logger.h"
#include "composicion.h"
#include "in2machineinfo.h"
#include "in2counters.h"
#include <QNetworkInterface>
#include "in2message.h"
#include "in2gestorchiller.h"
#include "chiller.h"
#include "in2configuratorparams.h"
#include "in2configuratorprinter.h"
#include "in2translator.h"
#include "in2gestormantenimiento.h"
#include "in2store.h"
#include "In2GestorDB.h"
#include "in2managererp.h"
#include "in2aisvisionselector.h"
#include "in2gestorvision.h"
#include "in2aisstringgen.h"
#include <QDebug>
#include "in2ping.h"
#include "in2opcserver.h"
#include "in2gestorlampara.h"
#include "in2gestorcorte.h"
#include "in2configuratorpermisos.h"
#include "in2gsmonitor.h"

//Hojas de estilo
#define STYLE_CENTRAL_FRAME "QFrame#CentralFrame {background-image: url(:res/images/In2HR2.png); background-repeat: no-repeat;background-position:center;}"
#define STYLE_TAB_VIEW "background-color: rgba(125,125,125,65);"
#define STYLE_VERTICAL_HEADER "background-color:lightblue;opacity:0.5;"
#define STYLE_HORIZONTAL_HEADER "background-color:darkblue;opacity:0.5;color: white;"


//Tiempode inicio
#ifndef NO_LOGON
    #define INIT_TIME 5000
#else
    #define INIT_TIME 0
#endif

/** Constructor */
Principal::Principal(QWidget *parent)
    : ui(new Ui::PrincipalSPassUI)
{
    m_memoryMap=MemoryMap::request(0);

    m_memoryMap->Init();

    In2Translator trans;
    if (!trans.changeLanguaje(QString("%1\\Languajes\\Spanish.qm").arg(QApplication::applicationDirPath()))){
        QMessageBox::warning(0,tr("ERROR"),tr("No existe el fichero de idioma ./Languajes/Spanish.qm"),QMessageBox::Ok);
    }

    //Crea la tabla OFFLINE si no existe
    In2Store store;
    store.createTable("OFFLINE");

}
/** Inicializa la pantalla*/
void Principal::init(){


    ui->setupUi(this);
#ifndef NO_LOGON
    /*In2Message *m=new In2Message(this,"Iniciando...por favor espere");
    connect(this,SIGNAL(sgnKillMsg()),m,SLOT(deleteLater()));
    m->sltEnableAutomaticPB(INIT_TIME);
    m->show();*/
#endif
    /*QProcess *p=new QProcess();
    QString app=QString("%1\\conexion.bat").arg(GetApplicationDirPath());
    p->start("cmd.exe", QStringList()<<"/c" << app);*/
    In2GsMonitor gsMonitor;
    if (!gsMonitor.isLoaded()){
        QMessageBox::warning(this,tr("Error"),tr("No existe gsdll32.dll. No se puede continuar"),QMessageBox::Ok);
        QApplication::exit();
    }
    m_memoryMap->m_mapa.m_maquina.EstadoMaquina=ST_REPOSO;
    m_InfoDebug=InkjetDebug::request(0);
    connect(this,SIGNAL(destroyed()),m_InfoDebug,SLOT(deleteLater()));
    //qWarning() << "Warning Message";
    QTimer::singleShot(INIT_TIME,this,SLOT(sltInit0()));
    In2GestorChiller *gestorChiller=In2GestorChiller::request(0,1);
    //El chiller a su bola
    if (gestorChiller->isEnabled()){
        //ui->Chiller->setEnabled(true);
        connect(gestorChiller,SIGNAL(SendDebugInfo(unsigned char,QString)),m_InfoDebug,SLOT(ProcessMsg(unsigned char,QString)));
        connect(gestorChiller,SIGNAL(sglCambioAlarm1(int)),this,SLOT(sltChillerAlarma1(int)));
        connect(gestorChiller,SIGNAL(sglCambioAlarm2(int)),this,SLOT(sltChillerAlarma2(int)));
        connect(gestorChiller,SIGNAL(sglCambioAlarm3(int)),this,SLOT(sltChillerAlarma3(int)));
        connect(gestorChiller,SIGNAL(sglCambioTemperatura(int)),this,SLOT(sltChillerTemperatura(int)));
        connect(gestorChiller,SIGNAL(sglCambioPresion(int)),this,SLOT(sltChillerPresion(int)));
        connect(gestorChiller,SIGNAL(sglCambioSetpoint(int)),this,SLOT(sltChillerSetpoint(int)));
        connect(gestorChiller,SIGNAL(sglCambioStatus(int)),this,SLOT(sltChillerStatus(int)));
        connect(gestorChiller,SIGNAL(sgnChillerCom(int)),this,SLOT(sltChillerCom(int)));
        sltChillerSetpoint(0);
        sltChillerPresion(0);
        QThread *t=new QThread;
        t->start();
        gestorChiller->moveToThread(t);
        //QTimer::singleShot(0,this,SLOT(sltUpdateTableChiller()));
        QTableWidgetItem *p=ui->m_twChiller->item(0,0);

        quint16 rtn=gestorChiller->getStatus(0);

        p->setText(QString("%1").arg(rtn));
        if (gestorChiller->getPortState()){
            p=ui->m_twChiller->item(1,0);
            p->setText("SI");
        }
        p=ui->m_twChiller->item(2,0);
        p->setText("SI");
        if (gestorChiller->isAutoStart()){
            p=ui->m_twChiller->item(3,0);
            p->setText("SI");
        }
    }

    //AbrirFicheroIniImpresora();
    //AbrirFicheroParametrosImpresora();
    In2ConfiguratorPrinter configPrinter;
    configPrinter.readConfig();
    In2ConfiguratorParams configParams;
    configParams.readConfig();

    //QPixmap prueba("D:\\foo.png","BMP");

    //AbrirBaseDeDatosLog();
    m_gestorSai=new In2SaiControl();
    if (m_gestorSai->isEnabled()){
        connect(m_gestorSai,SIGNAL(sgnSAIActive()),this,SLOT(sltSaiActive()));
        QThread *p=new QThread();
        p->start();
        m_gestorSai->moveToThread(p);
        connect(m_gestorSai,SIGNAL(destroyed()),p,SLOT(deleteLater()));
    }
    else {
        delete (m_gestorSai);
        m_gestorSai=NULL;
    }
    //this->show();
    //_sltTestSound();
}

void Principal::_sltTestSound(){
    /*QSound::play(QString("%1\\Config\\ReadyToPrint.wav").arg(QApplication::applicationDirPath()));
    QTimer::singleShot(5500,this,SLOT(_sltTestSound()));*/
    QApplication::beep();
    QTimer::singleShot(5500,this,SLOT(_sltTestSound()));

}

/** Se ha activado el SAI*/
void Principal::sltSaiActive(){
    m_InfoDebug->show();
    //Insertar_Evento(MENSAJE_DEBUG,"MAIN->AC FAIL->CERRANDO");
    //Salimos
    AutomatedExit();
}

/** Cambio alarma 1*/
void Principal::sltChillerAlarma1(int x){
    In2GestorChiller *gestor=In2GestorChiller::request(0,1);
    QTableWidgetItem *p=ui->m_twChiller->item(7,0);
    quint16 val=gestor->getAlarm1(x);
    p->setText(QString("%1").arg(val));
    emit SendDebugInfo(LOGGER_ERROR,QString("Chiller alarm1 %1").arg(val));
    if (val) m_InfoDebug->showLog();
}
/** Cambio alarma 2*/
void Principal::sltChillerAlarma2(int x){
    In2GestorChiller *gestor=In2GestorChiller::request(0,1);
    QTableWidgetItem *p=ui->m_twChiller->item(8,0);
    quint16 val=gestor->getAlarm2(x);
    p->setText(QString("%1").arg(val));
    emit SendDebugInfo(LOGGER_ERROR,QString("Chiller alarm2 %1").arg(val));
    if (val) m_InfoDebug->showLog();
}
/** Cambio alarma 3*/
void Principal::sltChillerAlarma3(int x){
    In2GestorChiller *gestor=In2GestorChiller::request(0,1);
    QTableWidgetItem *p=ui->m_twChiller->item(9,0);
    quint16 val=gestor->getAlarm3(x);
    p->setText(QString("%1").arg(val));
    emit SendDebugInfo(LOGGER_WARNING,QString("Chiller alarm3 %1").arg(val));
    if (val) m_InfoDebug->showLog();
}
/** Cambio de temperatura en chiller*/
void Principal::sltChillerTemperatura(int x){
    In2GestorChiller *gestor=In2GestorChiller::request(0,1);
    QTableWidgetItem *p=ui->m_twChiller->item(4,0);
    quint16 val=gestor->getDischargeTemp(x);
    p->setText(QString("%1.%2").arg(val/10).arg(val%10));

}
/** Cambio de presi�n en chiller*/
void Principal::sltChillerPresion(int x){
    In2GestorChiller *gestor=In2GestorChiller::request(0,1);
    QTableWidgetItem *p=ui->m_twChiller->item(6,0);
    quint16 val=gestor->getDischargePressure(x);
    p->setText(QString("%1.%2").arg(val/10).arg(val%10));

}
/** Cambio de setpoint en chiller*/
void Principal::sltChillerSetpoint(int x){
    In2GestorChiller *gestor=In2GestorChiller::request(0,1);
    QTableWidgetItem *p=ui->m_twChiller->item(5,0);
    quint16 val=gestor->getDischargeSetTemp(x);
    p->setText(QString("%1.%2").arg(val/10).arg(val%10));
    emit SendDebugInfo(LOGGER_WARNING,QString("Chiller setPoint %1.%2").arg(val/10).arg(val%10));
}
/** Cambio de status en chiller*/
void Principal::sltChillerStatus(int x){
    In2GestorChiller *gestor=In2GestorChiller::request(0,1);
    QTableWidgetItem *p=ui->m_twChiller->item(0,0);
    quint16 val=gestor->getStatus(x);
    p->setText(QString("%1").arg(val));
    emit SendDebugInfo(LOGGER_WARNING,QString("Chiller status %1").arg(val));
}
/** Cambio de status en chiller*/
void Principal::sltChillerCom(int x){
    In2GestorChiller *gestor=In2GestorChiller::request(0,1);
    QTableWidgetItem *p=ui->m_twChiller->item(10,0);
    char val=gestor->GetFlag(FLAG_COMCHILLER_OK);
    if (val==CHILLER_COMNOK) {
        p->setText(QString("NOK"));
        p->setTextColor(QColor(Qt::darkRed));
        In2GestorMaquina *m=In2GestorMaquina::request(0);
        m->SoftStop();
    }
    else {
        p->setText(QString("OK"));
        p->setTextColor(QColor(Qt::white));
    }
    emit SendDebugInfo(LOGGER_WARNING,QString("Chiller COM %1").arg((int)val));
}
/** De prueba*/
void Principal::sltLaunchVision(){
    In2AISVisionSelector *p=new In2AISVisionSelector(this);
    p->show();
}

/** Inicio*/
void Principal::sltInit0(){
   //this->setDataCommon();

   connect(ui->GestionButton, SIGNAL(clicked()),this, SLOT(LaunchGestion()));
   connect(ui->FluidicaButton, SIGNAL(clicked()),this, SLOT(LaunchFluidica()));
   connect(ui->MachineButton, SIGNAL(clicked()),this, SLOT(LaunchMaquina()));
   connect(ui->DbComposicionButton, SIGNAL(clicked()),this, SLOT(LaunchComposicion()));
   connect(ui->ExitButton, SIGNAL(clicked()),this, SLOT(Exit()));
   connect(this, SIGNAL(SendDebugInfo(unsigned char,QString)),m_InfoDebug,SLOT(ProcessMsg(unsigned char,QString)));
   connect(m_memoryMap,SIGNAL(SendDebugInfo(unsigned char,QString)),m_InfoDebug,SLOT(ProcessMsg(unsigned char,QString)));
   connect(ui->m_pbLog,SIGNAL(clicked()),this,SLOT(ForcedShowLogScreen()));
   connect(ui->Chiller,SIGNAL(clicked()),this,SLOT(sltLaunchChiller()));
   connect(ui->m_pbUsuario,SIGNAL(clicked()),this,SLOT(sltLaunchStatus()));
    m_InfoInformer=new InkjetInformer(0);
    if (m_InfoInformer->estado==0x01){
        connect(this, SIGNAL(SendInformerInfo(unsigned char,QString)),m_InfoDebug,SLOT(ProcessMsg(unsigned char,QString)));
        emit SendInformerInfo(INFORMER_DEBUG,QString("Inicio sesion"));
    }

   MuestraImagenPrincipal(m_memoryMap->m_Imagen);
   m_memoryMap->setUser("In2Daemon");

    //Conecta las se�ales relativas  a maquina
   m_In2GestorMaquina=In2GestorMaquina::request(this);

   connect(m_In2GestorMaquina,SIGNAL(SendDebugInfo(unsigned char,QString)),m_InfoDebug,SLOT(ProcessMsg(unsigned char,QString)));
   connect(m_In2GestorMaquina,SIGNAL(sgnShowLog()),m_InfoDebug,SLOT(showLog()));

   //Inicializao la vision
   In2GestorVision *vision=In2GestorVision::request(this);
   if (vision->isEnabled()){
       m_memoryMap->m_mapa.m_impresion.bMonitorVision=true;
       connect(vision,SIGNAL(sgnVisionAlarm(QString)),m_In2GestorMaquina,SLOT(sltVisionMessage(QString)));
       if (!vision->connect()){
            vision->wakeUp();
       }
   }

   GestorMaquina *gestorMaquina=GestorMaquina::request(this);
   connect(gestorMaquina,SIGNAL(SendDebugInfo(unsigned char,QString)),m_InfoDebug,SLOT(ProcessMsg(unsigned char,QString)));
   //Conecto las señales de info
   GestorImpresion *gestorImpresion=GestorImpresion::request(this);
   connect(gestorImpresion,SIGNAL(SendDebugInfo(unsigned char,QString)),m_InfoDebug,SLOT(ProcessMsg(unsigned char,QString)));

   In2ConfiguratorERP confERP;
   confERP.readConfig();
   //Inicializa el gestor de ERP que corresponda
   In2ManagerERP *erp=In2ManagerERP::request(0);
   connect(erp,SIGNAL(SendDebugInfo(unsigned char,QString)),m_InfoDebug,SLOT(ProcessMsg(unsigned char,QString)));
   connect(erp,SIGNAL(sgnERPNewMetrosRef(int)),m_In2GestorMaquina,SLOT(sltChangeMetrosReferencia(int)));
   connect(erp,SIGNAL(sgnERPTrabajoNuevo()),m_In2GestorMaquina,SLOT(sltTrabajoNuevo()));

   connect(erp,SIGNAL(sgnERPShowLog()),m_InfoDebug,SLOT(showLog()));
   //connect(m_In2GestorMaquina,SIGNAL(sgnGestorMaquinaFinImpresion()),erp,SLOT(sltFinImpresion()));
   connect(m_In2GestorMaquina,SIGNAL(sgnGesMaquinaFinEmpalmeEntrada()),erp,SLOT(sltFinEmpalmeEntrada()));
   connect(m_In2GestorMaquina,SIGNAL(sgnGesMaquinaFinEmpalmeSalida()),erp,SLOT(sltFinEmpalmeSalida()));




   //Cliente de OPC
   ClienteOPC *opc=ClienteOPC::request(0);
   connect(opc,SIGNAL(sgnOPCFlanco(int)),this,SLOT(sltFlancoOPC(int)));
   connect(opc,SIGNAL(OPCInitiated()),this,SLOT(sltOPCInitiated()));
   connect(opc, SIGNAL(SendDebugInfo(unsigned char,QString)),m_InfoDebug,SLOT(ProcessMsg(unsigned char,QString)));
   connect(opc,SIGNAL(EvDataChange(int&, QVariantList&, QVariantList&, QVariantList&)),this,SLOT(EvDataExt1(int&, QVariantList&, QVariantList&, QVariantList&)));

   In2GestorLampara *lampara=In2GestorLampara::request(0);
   connect(lampara,SIGNAL(sgnNewDataLampara(int,QVariant)),opc,SLOT(sltMessageFromLampara(int,QVariant)));
   connect(lampara, SIGNAL(SendDebugInfo(unsigned char,QString)),m_InfoDebug,SLOT(ProcessMsg(unsigned char,QString)));
   connect(opc,SIGNAL(sgnOPCNewLamparaValue(int,QVariant)),lampara,SLOT(sltAckFromOPC(int,QVariant)));
   //Cargo por defecto las tablas de intensidad y posicion
   if (lampara->posDinamica()) {
       if (!lampara->loadPosTable(DEFAULT_POSICION_TABLE)){
           QMessageBox::warning(0,"ERROR","No se pudo cargar tabla de posicion de lampara!",QMessageBox::Ok);
       }
   }
   if (lampara->intDinamica()) {
       if (!lampara->loadIntTable(DEFAULT_INTENSIDAD_TABLE)){
            QMessageBox::warning(0,"ERROR","No se pudo cargar tabla de intensidad de lampara!",QMessageBox::Ok);
       }
   }

   //QTimer::singleShot(1500,lampara,SLOT(init()));

   In2GestorCorte * corte=In2GestorCorte::request(this);
   connect(corte,SIGNAL(sgnNewDataCorte(int,QVariant)),opc,SLOT(sltMessageFromCorte(int,QVariant)));
   connect(corte, SIGNAL(SendDebugInfo(unsigned char,QString)),m_InfoDebug,SLOT(ProcessMsg(unsigned char,QString)));
   //Desactivamos las cuchillas
   QTimer::singleShot(1500,corte,SLOT(sltDeActivate()));


   //lampara->init();
   //Lo mandamos a otro thread
   /*QThread *opcTh=new QThread;
   opcTh->start();
   opc->moveToThread(opcTh);*/

   //Inicializa la fluidica
   m_FluidGestor=In2GestorFluidica::request(0);
   connect(m_FluidGestor,SIGNAL(SendDebugInfo(unsigned char,QString)),m_InfoDebug,SLOT(ProcessMsg(unsigned char,QString)));
   connect(m_FluidGestor,SIGNAL(HydraError(int,int,float)),this,SLOT(sltHydraError(int,int,float)));
   connect(m_FluidGestor,SIGNAL(sgnChangeState(int)),m_In2GestorMaquina,SLOT(sltChangeTheme(int)));
   connect(m_gestorSai,SIGNAL(sgnError(int)),m_FluidGestor,SLOT(sltError(int)));
   //connect(m_FluidGestor,SIGNAL(FluidStateNewValue(char)),m_In2GestorMaquina,SLOT(sltEstadoFluidica(char)));
   //connect(m_FluidGestor,SIGNAL(CompleteShutdownFinished()),this,SLOT(sltFullShutdown()));
   if (m_memoryMap->m_mapa.m_maquina.m_fluidica.Conectado==1){
       //InicializarHydra();
       m_FluidGestor->Init();
       for (int x=0;x<m_FluidGestor->numEquipos();x++){
           QString name=m_FluidGestor->getDescription(x);
           QTableWidgetItem *p=ui->m_twFluidica->item(1,x);
           if (p) p->setText(name);
           QColor color=m_FluidGestor->getColor(x);
           p=ui->m_twFluidica->item(0,x);
           if (p){
                p->setBackgroundColor(color);
           }
           if (m_memoryMap->m_mapa.m_maquina.m_fluidica.bAutostart[x]){
               SendDebugInfo(LOGGER_ERROR,QString("AutoInit Hydra %1").arg(x));
               m_FluidGestor->InitEquipo(x);
           }
       }
       if (m_FluidGestor->numEquipos()==0) {
           QString txt=QString("Error de com. con Fluidica. Revise conexiones y reinicie equipo");
           //QMessageBox::warning(this,"ERROR",txt,QMessageBox::Ok);
           emit SendDebugInfo(LOGGER_ERROR,txt);
           //m_FluidGestor->SetFlag(FLAG_ESTADO_FLUIDICA,THEME_FLUID_OK);
           QThread *th=new QThread;
           connect(m_FluidGestor,SIGNAL(destroyed()),th,SLOT(quit()));
           th->start();
           m_FluidGestor->moveToThread(th);
           QTimer::singleShot(0,m_FluidGestor,SLOT(sltGetCurrentThread()));
           //m_FluidGestor->getCurrentThread();

       }
       else{
           //Lanza la monitorizacion...deberia pasarla a otro thread
           //m_FluidGestor->startMonitoring();
           QThread *th=new QThread;
           connect(m_FluidGestor,SIGNAL(destroyed()),th,SLOT(quit()));
           th->start();
           m_FluidGestor->moveToThread(th);
           QTimer::singleShot(0,m_FluidGestor,SLOT(startMonitoring()));
           QTimer::singleShot(0,m_FluidGestor,SLOT(sltGetCurrentThread()));
       }
   }
   else {
    emit SendDebugInfo(LOGGER_ERROR,"FLUIDICA SIN CONFIGURAR");
    m_FluidGestor->SetFlag(FLAG_ESTADO_FLUIDICA,THEME_FLUID_OK);
   }

   //Cargo dibujos
   for (int x=0;x<ui->m_twXusb->columnCount();x++){
        QLabel *led=new QLabel();
        led->setPixmap(QPixmap(":/res/images/Ledrojo.png"));
        led->setAlignment(Qt::AlignCenter);
        ui->m_twXusb->setCellWidget(0,x,led);
   }

#ifndef NO_LOGON
    //ComprobarOperacionesMantenimiento();
   In2GestorMantenimiento gestorMant;
   m_memoryMap->m_mapa.m_maquina.seg_lampara_on=gestorMant.checkSegundosLampara();
#endif
   QString dateChangeFiltro=gestorMant.checkCambioFiltros();
   if (gestorMant.getCambioFiltros()) emit SendDebugInfo(LOGGER_ERROR,tr("CAMBIO DE FILTROS PENDIENTE DE %1").arg(dateChangeFiltro));

   //Cargamos los valores de tablas
   //emit sgnKillMsg();
   //QTimer::singleShot(0,this,SLOT(sltUpdateTableMaquina()));
   QTimer::singleShot(1000,this,SLOT(sltUpdateTableHistorico()));
   QTimer::singleShot(1000,this,SLOT(sltUpdateTableImpresion()));
   QTimer::singleShot(1000,this,SLOT(sltUpdateTableFluidica()));

   GlobalSetScreenSize(this);

   DWORD id=(DWORD)QThread::currentThreadId();
   emit SendDebugInfo(LOGGER_ERROR,QString("Thread Principal ID:%1").arg(id));

   QTimer::singleShot(1000,this,SLOT(InitMachine()));


   //gestorMaquina->sltEnableCondicionesMovimiento();
   //gestorMaquina->sltHabilitarCalculoDiametro(0);
}

/** Errores hydra*/
void Principal::sltHydraError(int error,int equipo, float value){
    QString str;
    QString name=m_FluidGestor->getDescription(equipo);
    switch (error){
    case HYDRAERROR_BITS_ERROR:
        str=m_FluidGestor->getBitsDescription(equipo);
        emit SendDebugInfo(LOGGER_ERROR,QString("Hydra (%1-%2) bit error [%3]:%4").arg(equipo).arg(name).arg((short)value).arg(str));
        break;
    case HYDRAERROR_FAULTS_ERROR:
        str=m_FluidGestor->getFaultsDescription(equipo);
        emit SendDebugInfo(LOGGER_ERROR,QString("Hydra (%1-%2) fault error [%3]:%4").arg(equipo).arg(name).arg((short)value).arg(str));
        //Si estamos imprimendo haremos un softstop
        m_In2GestorMaquina->SoftStop();
        //Hay que hacer un shutdown??
        m_FluidGestor=In2GestorFluidica::request(this);
        m_FluidGestor->ShutdownEquipo(equipo);

        break;
    case HYDRAERROR_SWITCHES_ERROR:
        str=m_FluidGestor->getSwitchesDescription(equipo);
        emit SendDebugInfo(LOGGER_ERROR,QString("Hydra (%1-%2) switch error [%3]:%4").arg(equipo).arg(name).arg((short)value).arg(str));
        break;
    case HYDRAERROR_MENISCUS_RANGE:
        emit SendDebugInfo(LOGGER_ERROR,QString("Presion de menisco en (%1-%2) fuera de rango [%3]").arg(equipo).arg(name).arg(value));
        m_InfoDebug->showLog();
        break;
    case HYDRAERROR_LOW_LEVEL:
        emit SendDebugInfo(LOGGER_ERROR,QString("Nivel de tinta en (%1-$2) bajo [%3]").arg(equipo).arg(name).arg(value));
        m_InfoDebug->showLog();
        break;

    case HYDRAERROR_MENISCUS_POSITIVE:
        emit SendDebugInfo(LOGGER_ERROR,QString("Presion de menisco en (%1-%2) positiva [%3]").arg(equipo).arg(name).arg(value));
        m_InfoDebug->showLog();
        m_In2GestorMaquina->SoftStop();
        //Hay que hacer un shutdown??
        m_FluidGestor=In2GestorFluidica::request(this);
        m_FluidGestor->ShutdownEquipo(equipo);
        break;
    case HYDRAERROR_STATECHANGE_ERROR:
        str=m_FluidGestor->getModeDescription(equipo);
        emit SendDebugInfo(LOGGER_ERROR,QString("Hydra (%1-%2) state change-->[%3]").arg(equipo).arg(name).arg(str));
        break;
    case HYDRAERROR_TEMPERATURE_ERROR:
        emit SendDebugInfo(LOGGER_ERROR,QString("Error temperatura en (%1-%2):[%3]").arg(equipo).arg(name).arg(value));
        m_InfoDebug->showLog();
        break;
    }
}
/** Testea si existe el fichero de control de SAI para apagar m�quina*/
/*void Principal::sltTestSai(){
    //Testeamos que existe el fichero de SAI
    QFile file(m_memoryMap->m_mapa.m_sai.tokenFile);
    if (file.exists()){
        file.remove(); //Borramos el fichero para la proxima vez
        m_InfoDebug->show();
        Insertar_Evento(MENSAJE_DEBUG,"FALLO DE AC. SAI TRABAJANDO OK");
        //Salimos
        AutomatedExit();
    }
    else{
        QTimer::singleShot(TIMEOUT_TEST_SAI,this,SLOT(sltTestSai()));
    }
}*/


/** Inicializa la maquina*/
void Principal::InitMachine(){

    //Inicializar_textos();
    emit sglLeerReferenciaVelocidad();

   //Preparamos las comunicaciones
   m_GestorEthernet=new GestorEthernet();
   connect(m_GestorEthernet,SIGNAL(SendDebugInfo(unsigned char,QString)),m_InfoDebug,SLOT(ProcessMsg(unsigned char,QString)));
   //Control de la SAI

   /*if ((m_memoryMap->m_mapa.m_sai.testSai)&&(!m_memoryMap->m_mapa.m_sai.tokenFile.isEmpty())){
       QTimer::singleShot(0,this,SLOT(sltTestSai()));
   }*/

  //Actualizamos la info del gestor de fluidica
   m_FluidGestor->sltUpdateInfo();
}
/** Inicializacion de items de maquina*/
void Principal::sltOPCInitiated(){
    QString txt=QString("sltOPCInitiated");
    emit SendDebugInfo(LOGGER_ERROR,txt);
    GestorMaquina *gestorMaquina=GestorMaquina::request(this);
    gestorMaquina->sltSetCondicionesIniciales();
    gestorMaquina->sltEnableCondicionesMovimiento();
    gestorMaquina->sltHabilitarCalculoDiametro(0);
    //Solo hace autotensado si no hay emergencia
    if (!m_memoryMap->m_mapa.m_maquina.codigo_parada)
        gestorMaquina->sltTensadoAutomatico();
}

/** Destructor*/
Principal::~Principal()
{
    delete (m_GestorEthernet);
    //Informacion de debug
    m_InfoDebug->deleteLater();
    //Gestor de m�quina
    delete (m_In2GestorMaquina);
    //Gestor de fluidica
    delete (m_FluidGestor);
    //Info de maquina
    //In2MachineInfo *m_MachineInfo;
    if (m_gestorSai) delete (m_gestorSai);

}
/** Lanzamos SW de Watchdog*/
void Principal::LaunchSWWatchdog(){
   /* QProcess WdgProcess;
    WdgProcess.startDetached(WDG_FILE);
    if (!WdgProcess.waitForStarted(3000)){
        emit SendDebugInfo(LOGGER_WARNING,QString("Timeout expired launching InterfazWdg"));
    }
    else
        emit SendDebugInfo(LOGGER_WARNING,QString("InterfazWdg launched"));*/
}

/** Cargamos datos comunes*/
void Principal::setDataCommon(){
/*    struct stCommonExt dummyDataCommon;
    dummyDataCommon.fecha=this->ui->fecha;
    dummyDataCommon.led_EstadoLampara=this->ui->led_EstadoLampara;
    dummyDataCommon.led_EstadoMaquina=this->ui->led_EstadoMaquina;
    dummyDataCommon.led_EstadoTemperatura=this->ui->led_EstadoTemperatura;
    dummyDataCommon.info=NULL;
    dummyDataCommon.m_Imagen=NULL;
    dummyDataCommon.m_User=this->ui->m_User;
    dummyDataCommon.usuario_actual=this->ui->usuario_actual;
    dummyDataCommon.m_MostrarAyuda=NULL;
    dummyDataCommon.lbl_EstadoLampara=ui->lblLamp;
    dummyDataCommon.lbl_EstadoMaquina=ui->lblMaquina;
    dummyDataCommon.lbl_EstadoTemperatura=ui->lblFluid;
    dummyDataCommon.lbl_IP=ui->m_ip;
    dummyDataCommon.lblLamp=ui->lblLamp;
    dataCommon=new InkjetCommonExt(this,dummyDataCommon);
//Actualizamos
    dataCommon->ShowInfo();
    dataCommon->ShowEstadoImpresora();
    dataCommon->ShowComms();

    //Se testea si hay fichero
    if (!m_memoryMap->m_mapa.m_variables.NombreIni.isEmpty()){
        QString status=QString("%1\\%2").arg(m_memoryMap->m_mapa.m_variables.RutaIni).arg(m_memoryMap->m_mapa.m_variables.NombreIni);
        ui->groupBox->setTitle(QString("FILE:%1").arg(status));
    }
    else ui->groupBox->setTitle(QString("STATUS:No file"));

    //Inicializa el estado de la fluidica*/
/*    char fluidValue=m_FluidGestor->GetFlag(FLAG_ESTADO_FLUIDICA);
    dataCommon->sltEstadoFluidica(fluidValue);
    connect(m_FluidGestor,SIGNAL(FluidStateNewValue(char)),dataCommon,SLOT(sltEstadoFluidica(char)));*/


}
/** Testea si es root o no*/
void Principal::TestRoot(){
    /*if (!m_memoryMap->getClient().compare(CLIENT_GRAMAR)){
        _loadPermisosGramar();
    }
    else{
        _loadPermisosIn2();
    }*/

    In2ConfiguratorPermisos permisos;
    if (!permisos.loadPermisos("PRINCIPAL")){
        QMessageBox::warning(this,"ERROR",tr("No se han definido permisos para esta pantalla/usuario"),QMessageBox::Ok);
        ui->ExitButton->setEnabled(true);
        return;
    }
    bool val=permisos.permiso(PRMS_COMPOSICION);
    ui->DbComposicionButton->setEnabled(val);
    val=permisos.permiso(PRMS_GESTION);
    ui->GestionButton->setEnabled(val);
    val=permisos.permiso(PRMS_MAQUINA);
    ui->MachineButton->setEnabled(val);
    val=permisos.permiso(PRMS_CHILLER);
    ui->Chiller->setEnabled(val);
    val=permisos.permiso(PRMS_FLUIDICA);
    ui->FluidicaButton->setEnabled(val);
    val=permisos.permiso(PRMS_LOG);
    ui->m_pbLog->setEnabled(val);
    val=permisos.permiso(PRMS_USUARIO);
    ui->m_pbUsuario->setEnabled(val);
    val=permisos.permiso(PRMS_SALIR);
    ui->ExitButton->setEnabled(val);

}


/** Testea el usuario introducido*/
void Principal::UsuarioIntroducido(){
    if (!m_memoryMap->m_mapa.m_config.m_LDAPActive){
        if (!TestSuperUser()){
            In2Logon *m_logon=new In2Logon();
            m_logon->TestLocalUser();
            delete (m_logon);
            /*loginCounter++;
            if (loginCounter<3){
                Status *status;
                status=new Status(this);
                status->EnableWindow();
                TestRoot();
                connect(status,SIGNAL(StatusOculto()),this,SLOT(UsuarioIntroducido()));
                status->show();
            }
            //m_memoryMap->setUserRights(ACCESS_PRINTER);*/
            TestRoot();

        }
        else{
            m_memoryMap->setUserRights(ACCESS_GOD);
            TestRoot();
        }
        sltUpdateTableMaquina();
    }
    else{
        TestLDAP();
    }
}

/** Comprueba conexion contra LDAP*/
void Principal::TestLDAP(){
    QString dummy_usuario=QString("Usuario: %1").arg(m_memoryMap->getUser());
    //this->ui->usuario_actual->setText(dummy_usuario);

    if (TestSuperUser()){
        m_memoryMap->setUserRights(ACCESS_GOD);
        //Insertar_Evento(INICIO_SESION,dummy_usuario);
    }
    else{
        QString msg;
        In2Logon *m_logon=new In2Logon();
        int rtn=m_logon->CheckDARoot();
        //int rtn=m_logon->CheckDA("");
        if (rtn!=LOGGER_USER_INCLUDED){
            rtn=m_logon->CheckDAPrinter();
            switch (rtn){
            case LOGGER_USER_GRANTED:
                msg=QString("Usuario %1 sin derechos").arg(dummy_usuario);
                QMessageBox::warning(0,"DENEGADO",msg,QMessageBox::Ok);
                //Insertar_Evento(FALLO_LOG,QString("%1-%2").arg(rtn).arg(msg));
                m_memoryMap->setUserRights(ACCESS_STRANGE);
                break;
            case LOGGER_USER_NOT_GRANTED:
                msg=QString("Usuario %1 NO incluido en DA").arg(dummy_usuario);
                QMessageBox::warning(0,"DENEGADO",msg,QMessageBox::Ok);
                //Insertar_Evento(FALLO_LOG,QString("%1-%2").arg(rtn).arg(msg));
                m_memoryMap->setUserRights(ACCESS_STRANGE);
                break;
            case LOGGER_USER_INCLUDED:
                msg=QString("Usuario %1 incluido en IN2PRINTER").arg(dummy_usuario);
                QMessageBox::warning(0,"PERMITIDO",msg,QMessageBox::Ok);
                //Insertar_Evento(INICIO_SESION,QString("%1-%2").arg(rtn).arg(msg));
                m_memoryMap->setUserRights(ACCESS_PRINTER);
                break;
            case LOGGER_USER_NOT_INCLUDED:
                msg=QString("Usuario %1 no incluido en grupos IN2").arg(dummy_usuario);
                QMessageBox::warning(0,"DENEGADO",msg,QMessageBox::Ok);
                //Insertar_Evento(FALLO_LOG,QString("%1-%2").arg(rtn).arg(msg));
                m_memoryMap->setUserRights(ACCESS_STRANGE);
                break;
            case LOGGER_NO_INIT:
                msg=QString("Usuario %1 - No se pudo iniciar LDAP").arg(dummy_usuario);
                QMessageBox::warning(0,"ERROR",msg,QMessageBox::Ok);
                //Insertar_Evento(FALLO_LOG,QString("%1-%2").arg(rtn).arg(msg));
                m_memoryMap->setUserRights(ACCESS_STRANGE);
                break;
            case LOGGER_NO_CONNECTION:
                msg=QString("Usuario %1 - LDAP: Error de conexion").arg(dummy_usuario);
                QMessageBox::warning(0,"ERROR",msg,QMessageBox::Ok);
                //Insertar_Evento(FALLO_LOG,QString("%1-%2").arg(rtn).arg(msg));
                m_memoryMap->setUserRights(ACCESS_STRANGE);
                break;
            }
        }
        else{
            msg=QString("Usuario %1 incluido en IN2ROOT").arg(dummy_usuario);
            m_memoryMap->setUserRights(ACCESS_ROOT);
            QMessageBox::warning(0,"PERMITIDO",msg,QMessageBox::Ok);
            //Insertar_Evento(INICIO_SESION,QString("%1-%2").arg(rtn).arg(msg));

        }
        delete (m_logon);
    }

    /*this->ui->usuario_actual->show();
    dataCommon->ShowInfo();
    dataCommon->RefreshUserMode();*/
    TestRoot();

}

/** Actualiza la tabla de chiller*/
void Principal::sltUpdateTableChiller(){
    if (this->isVisible()){
        In2GestorChiller *chiller=In2GestorChiller::request(0,1);
        if (chiller){
            quint16 val=chiller->getDischargeTemp(0);
            QTableWidgetItem *p=ui->m_twChiller->item(4,0);
            p->setText(QString("%1.%2").arg(val/10).arg(val%10));
            val=chiller->getDischargePressure(0);
            p=ui->m_twChiller->item(5,0);
            p->setText(QString("%1.%2").arg(val/10).arg(val%10));
        }
    }
    QTimer::singleShot(5000,this,SLOT(sltUpdateTableChiller()));
}

/** Actualiza la tabla de fluidica*/
void Principal::sltUpdateTableFluidica(){
    static char flag=0;
    flag^=1;
    if (this->isVisible()){
        int num=m_FluidGestor->numEquipos();
        for (int x=0;x<num;x++){
            QString str=m_FluidGestor->getModeDescription(x);
            QTableWidgetItem *p=ui->m_twFluidica->item(2,x);
            if (p) p->setText(str);
            XCL_Modes mode=m_FluidGestor->getMode(x);
            Qt::GlobalColor color;
            if (mode==XCL_MODE_Started) color=Qt::blue;
            else if (mode==XCL_MODE_Stopped) color=Qt::white;
            else {
                if (flag) color=Qt::black;
                else color=Qt::white;
            }
            p->setTextColor(color);
            float fData=m_FluidGestor->getMeniscusPresure(x);
            p=ui->m_twFluidica->item(3,x);                        
            if (p) {
                p->setText(QString("%1").arg(fData));
                if ((mode==XCL_MODE_Starting_MP)||(mode==XCL_MODE_Stopping_MP))
                    p->setTextColor(color);
                else p->setTextColor(Qt::black);
            }
            fData=m_FluidGestor->getDiferentialPresure(x);
            p=ui->m_twFluidica->item(4,x);
            if (p){
                p->setText(QString("%1").arg(fData));
                if ((mode==XCL_MODE_Starting_DP)||(mode==XCL_MODE_Stopping_DP))
                    p->setTextColor(color);
                else p->setTextColor(Qt::black);

            }
            fData=m_FluidGestor->getSupplyTemp(x).val;
            p=ui->m_twFluidica->item(5,x);
            if (p){
                p->setText(QString("%1").arg(fData));
                if ((mode==XCL_MODE_Starting_T)||(mode==XCL_MODE_Stopping_T))
                    p->setTextColor(color);
                else p->setTextColor(Qt::black);

            }
            str=m_FluidGestor->getBitsDescription(x);
            p=ui->m_twFluidica->item(6,x);
            if (p) p->setText(str);
            str=m_FluidGestor->getSwitchesDescription(x);
            p=ui->m_twFluidica->item(7,x);
            if (p) p->setText(str);
            str=m_FluidGestor->getFaultsDescription(x);
            p=ui->m_twFluidica->item(8,x);
            if (p) p->setText(str);
            fData=m_FluidGestor->getLevelRes(x).val;
            p=ui->m_twFluidica->item(9,x);
            if (p) p->setText(QString("%1").arg(fData));
        }
    }

    QTimer::singleShot(2500,this,SLOT(sltUpdateTableFluidica()));


}
/** Carga la latbal de XUSB*/
void Principal::sltUpdateTableImpresion(){
     GestorImpresion *gestorImpresion=GestorImpresion::request(this);
    int time=0;
    int cards=0;
    if (gestorImpresion->isDriverLoaded()){
        cards=gestorImpresion->getNumCards();
        time=m_memoryMap->m_mapa.m_variables.DefaultNumHeads*cards*TIME_HEAD_TEST+5000;
    }
    for (int x=0;x<cards;x++){
        QTableWidgetItem *p=ui->m_twXusb->item(1,x);
        if (p) p->setText(QString("Checking..."));
    }
    emit SendDebugInfo(LOGGER_ERROR,QString("INICIANDO %1 CABEZALES en %2 XUSB en %3 msg").arg(m_memoryMap->m_mapa.m_variables.DefaultNumHeads).arg(cards).arg(time));
    gestorImpresion->setControlHead(0,m_memoryMap->m_mapa.m_machinedefaults.ControlHead);
    //if (cards==0) time=0;
    QTimer::singleShot(time,this,SLOT(sltUpdateTableImpresion1()));
}

/** Chequea credenciales*/
void Principal::sltCheckCredentials(){
    QStringList environment = QProcess::systemEnvironment();
    QStringList usernames=environment.filter(QString("USERNAME"));
    QString usuario=usernames.at(0);
    usuario.remove(QString("USERNAME="));
    m_memoryMap->setUser(usuario);
    //Consultar_usuario_registrado();
    //Insertar_Evento(INICIO_SESION,usuario);

#ifndef NO_LOGON
            //TestRoot();
    sltLaunchStatus();
 #else
    m_memoryMap->setUserRights(ACCESS_GOD);
    TestRoot();
    sltUpdateTableMaquina();
#endif

}

/** Lanza la pantalla de status*/
void Principal::sltLaunchStatus(){
    Status *status;
    status=new Status(this);
    status->EnableWindow();
    //connect(status,SIGNAL(StatusOculto()),this,SLOT(UsuarioIntroducido()));
    connect(status,SIGNAL(StatusOculto()),this,SLOT(sltUpdateTableMaquina()));
    status->show();
}


/** Carga la tabla de XUSB*/
void Principal::sltUpdateTableImpresion1(){
    emit sgnKillMsg();
//#ifndef NO_LOGON //esto solo va a maquina
    sltCheckCredentials();
//#else
    //m_memoryMap->setUser("Admin");

//#endif



    GestorImpresion *gestorImpresion=GestorImpresion::request(this);
    int iNumCards=gestorImpresion->getNumCards();
    for (int x=0;x<iNumCards;x++){
        char buffer[128];
        memset(&buffer,0x00,sizeof(buffer));
        gestorImpresion->getSerial(x,buffer);
        DWORD numHeads=gestorImpresion->getNumConnectedHeads(x);
        for (DWORD y=0;y<numHeads;y++){
            char buffer[25];
            memset(&buffer[0],0x00,sizeof(buffer));
            gestorImpresion->getSerialHead(x,y,&buffer[0]);
            unsigned int s1=(unsigned char)buffer[0];
            unsigned int s2=(unsigned char)buffer[1];
            unsigned int s3=(unsigned char)buffer[2];
            unsigned int s4=(unsigned char)buffer[3];
            unsigned int serial=s1+s2*(1<<8)+s3*(1<<16)+s4*(1<<24);
            QString txt=QString("Card[%1] Head[%2] Serial[%3]").arg(x).arg(y).arg(serial);
            emit SendDebugInfo(LOGGER_WARNING,txt);
        }

        //QString serial=QString(buffer);
        QTableWidgetItem *p=ui->m_twXusb->item(0,x);
        if (p){
            QLabel *pLabel=static_cast<QLabel*>(ui->m_twXusb->cellWidget(0,x));
            if (pLabel) pLabel->setPixmap(QPixmap(":res/images/Ledverde.png"));
        }//p->setText(serial);
        p=ui->m_twXusb->item(1,x);
        if (p) p->setText(QString("%1").arg(numHeads));

        if (numHeads!=(DWORD)m_memoryMap->m_mapa.m_variables.DefaultNumHeads){
            QString txt=QString("XUSB %1 - Numero de cabezales encontrados [%2], difiere de cabezales configurados [%3].Por favor reinicie aplicacion").arg(x).arg(numHeads).arg(m_memoryMap->m_mapa.m_variables.DefaultNumHeads);
            emit SendDebugInfo(LOGGER_ERROR,txt);
            QMessageBox::warning(this,"ERROR",txt,QMessageBox::Ok);
        }
        else{
            QString txt=QString("XUSB %1 - Numero de cabezales encontrados [%2] OK").arg(x).arg(numHeads);
            emit SendDebugInfo(LOGGER_ERROR,txt);
        }

        /*p=ui->m_twXusb->item(2,x);
        if (p) p->setText("1001");*/
    }

}


/** Carga la tabla de Maquina*/
void Principal::sltUpdateTableMaquina(){

    TestRoot();

    QTableWidgetItem *p=ui->m_twMaquina->item(0,0);
    if (p) p->setText(m_memoryMap->getUser());

    p=ui->m_twMaquina->item(1,0);
    if (p){
        if (m_memoryMap->getUserRights()==ACCESS_STRANGE) p->setText("STRANGE");
        else if (m_memoryMap->getUserRights()==ACCESS_PRINTER) p->setText("PRINTER");
        else if (m_memoryMap->getUserRights()==ACCESS_ROOT) p->setText("ROOT");
        else if (m_memoryMap->getUserRights()==ACCESS_GOD) p->setText("SUPERUSER");
    }

    p=ui->m_twMaquina->item(2,0);
    if (p){
        foreach (const QHostAddress &address, QNetworkInterface::allAddresses()) {
            if (address.toString().contains("192.")){
                 p->setText(address.toString());
                 p=ui->m_twMaquina->item(3,0);
            }
        }
    }
    p=ui->m_twMaquina->item(4,0);
    //if (p) p->setText("OK");
    QString txt;
    if (m_memoryMap->m_mapa.m_maquina.codigo_parada)
        txt=m_In2GestorMaquina->getCodeErrorMaquina();
    else
        txt=QString("OK");
    if (p){
        p->setText(txt);
        p->setTextColor(QColor(Qt::white));
    }


}

/** Carga la tabla de historico*/
void Principal::sltUpdateTableHistorico(){

    In2GestorMantenimiento gestorMant;
    QString dateChangeFiltro=gestorMant.checkCambioFiltros();


    In2Counters newRecord;
    int iPrintedMeters=newRecord.getTotalMeters();

    QTableWidgetItem *p=ui->m_twHistorico->item(1,0);
    if (p) p->setText(QString("%1").arg(iPrintedMeters));

    int iPrintedWorks=newRecord.getTotalPrinted();
    p=ui->m_twHistorico->item(2,0);
    if (p) p->setText(QString("%1").arg(iPrintedWorks));

    p=ui->m_twHistorico->item(5,0);
    if (p) p->setText(QString("%1").arg(dateChangeFiltro));
    if (gestorMant.getCambioFiltros()){
        //emit SendDebugInfo(LOGGER_ERROR,tr("CAMBIO DE FILTROS PENDIENTE DE %1").arg(dateChangeFiltro));
        p->setBackgroundColor(QColor(Qt::red));
    }
    //else p->setBackgroundColor(QColor(Qt::black));


}
/** Cuando se va a hacer visible la pantalla*/
void Principal::showEvent ( QShowEvent * event ){
    //Leer_Horas_Funcionamiento_Lampara();
    QTableWidgetItem *p=ui->m_twHistorico->item(0,0);
    int horas=m_memoryMap->m_mapa.m_maquina.seg_lampara_on/3600;
    int minutos=(m_memoryMap->m_mapa.m_maquina.seg_lampara_on%3600)/60;
    int segundos=(m_memoryMap->m_mapa.m_maquina.seg_lampara_on%3600)%60;
    if (p) p->setText(QString("%1:%2:%3").arg(horas,2,10,QChar('0')).toUpper().arg(minutos,2,10,QChar('0')).toUpper().arg(segundos,2,10,QChar('0')).toUpper());

    sltUpdateDiametros();
}

/** Actualiza los diametros*/
void Principal::sltUpdateDiametros(){
    if (this->isVisible()){
        QTableWidgetItem *p=ui->m_twHistorico->item(3,0);
        In2ManagerDiametros *man=In2ManagerDiametros::request(0);
        MonitorDiametro *t=man->getMonitor(IN2MANAGER_DIN);
        if (t->isEnabled()){
            p->setText(QString("%1").arg(m_memoryMap->m_mapa.m_maquina.diametro_bobina_entrada));
        }
        else{
            if (t->isPLCEnabled())
                p->setText(QString("%1").arg(t->getCurrentDiameter(0)));
            else
                p->setText(QString("%1").arg(m_memoryMap->m_mapa.m_maquina.diametro_bobina_entrada));
        }

        p=ui->m_twHistorico->item(4,0);


        t=man->getMonitor(IN2MANAGER_DOUT);
        if (t){
            if (t->isEnabled()){
                p->setText(QString("%1").arg(m_memoryMap->m_mapa.m_maquina.diametro_bobina_salida_inst));
            }
            else{
                if (t->isPLCEnabled())
                    p->setText(QString("%1").arg(t->getCurrentDiameter(0)));
                else
                    p->setText(QString("%1").arg(m_memoryMap->m_mapa.m_maquina.diametro_bobina_salida));
            }
        }
        QTimer::singleShot(2000,this,SLOT(sltUpdateDiametros()));
    }

}


/** Muestra la imagen central*/
void Principal::MuestraImagenPrincipal(QPixmap Imagen){

    ui->m_centralFrame->setObjectName("CentralFrame");
    ui->m_centralFrame->setStyleSheet(STYLE_CENTRAL_FRAME);

    ui->m_twFluidica->verticalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->m_twFluidica->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->m_twFluidica->setStyleSheet(STYLE_TAB_VIEW);
    ui->m_twFluidica->horizontalHeader()->setStyleSheet(STYLE_HORIZONTAL_HEADER);
    ui->m_twFluidica->verticalHeader()->setStyleSheet(STYLE_VERTICAL_HEADER);



    ui->m_twHistorico->verticalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->m_twHistorico->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->m_twHistorico->setStyleSheet(STYLE_TAB_VIEW);
    ui->m_twHistorico->horizontalHeader()->setStyleSheet(STYLE_HORIZONTAL_HEADER);
    ui->m_twHistorico->verticalHeader()->setStyleSheet(STYLE_VERTICAL_HEADER);

    ui->m_twChiller->verticalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->m_twChiller->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->m_twChiller->setStyleSheet(STYLE_TAB_VIEW);
    ui->m_twChiller->horizontalHeader()->setStyleSheet(STYLE_HORIZONTAL_HEADER);
    ui->m_twChiller->verticalHeader()->setStyleSheet(STYLE_VERTICAL_HEADER);


    ui->m_twXusb->verticalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->m_twXusb->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->m_twXusb->setStyleSheet(STYLE_TAB_VIEW);
    ui->m_twXusb->horizontalHeader()->setStyleSheet(STYLE_HORIZONTAL_HEADER);
    ui->m_twXusb->verticalHeader()->setStyleSheet(STYLE_VERTICAL_HEADER);

    ui->m_twMaquina->verticalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->m_twMaquina->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->m_twMaquina->setStyleSheet(STYLE_TAB_VIEW);
    ui->m_twMaquina->horizontalHeader()->setStyleSheet(STYLE_HORIZONTAL_HEADER);
    ui->m_twMaquina->verticalHeader()->setStyleSheet(STYLE_VERTICAL_HEADER);


    int iNumFilas=ui->m_twHistorico->rowCount();
    int iNumColumnas=ui->m_twHistorico->columnCount();
    for (int x=0; x<iNumFilas;x++){
        for (int y=0;y<iNumColumnas;y++){
            QTableWidgetItem *p=new QTableWidgetItem("-");
            p->setTextAlignment(Qt::AlignCenter);
            Qt::ItemFlags Flags;
            Flags=p->flags();
            Flags&=~(Qt::ItemIsEditable);
            Flags&=~(Qt::ItemIsSelectable);
            p->setFlags(Flags);
            ui->m_twHistorico->setItem(x,y,p);
        }
    }
    iNumFilas=ui->m_twFluidica->rowCount();
    iNumColumnas=ui->m_twFluidica->columnCount();
    for (int x=0; x<iNumFilas;x++){
        for (int y=0;y<iNumColumnas;y++){
            QTableWidgetItem *p=new QTableWidgetItem();
            if (x==0)
                p->setText("");
            else
                p->setText("-");

            p->setTextAlignment(Qt::AlignCenter);
            Qt::ItemFlags Flags;
            Flags=p->flags();
            Flags&=~(Qt::ItemIsEditable);
            Flags&=~(Qt::ItemIsSelectable);
            p->setFlags(Flags);
            ui->m_twFluidica->setItem(x,y,p);
        }

    }
    iNumFilas=ui->m_twXusb->rowCount();
    iNumColumnas=ui->m_twXusb->columnCount();
    for (int x=0; x<iNumFilas;x++){
        for (int y=0;y<iNumColumnas;y++){
            QTableWidgetItem *p=new QTableWidgetItem("-");
            p->setTextAlignment(Qt::AlignCenter);
            Qt::ItemFlags Flags;
            Flags=p->flags();
            Flags&=~(Qt::ItemIsEditable);
            Flags&=~(Qt::ItemIsSelectable);
            p->setFlags(Flags);
            ui->m_twXusb->setItem(x,y,p);
        }

    }
    iNumFilas=ui->m_twMaquina->rowCount();
    iNumColumnas=ui->m_twMaquina->columnCount();
    for (int x=0; x<iNumFilas;x++){
        for (int y=0;y<iNumColumnas;y++){
            QTableWidgetItem *p=new QTableWidgetItem("-");
            p->setTextAlignment(Qt::AlignCenter);
            Qt::ItemFlags Flags;
            Flags=p->flags();
            Flags&=~(Qt::ItemIsEditable);
            Flags&=~(Qt::ItemIsSelectable);
            p->setFlags(Flags);
            ui->m_twMaquina->setItem(x,y,p);
        }
    }
}

/** Lanza la pantalla de parametros de impresion*/
void Principal::LaunchComposicion(){

    m_memoryMap->InitComposicion();
    Composicion *ScrComposicion;
    ScrComposicion=new Composicion(this);
    ScrComposicion->show();
    ScrComposicion->setIn2Parent(this);
    connect(ScrComposicion, SIGNAL(SendDebugInfo(unsigned char,QString)),m_InfoDebug,SLOT(ProcessMsg(unsigned char,QString)));
    connect(ScrComposicion, SIGNAL(SendInformerInfo(unsigned char,QString)),m_InfoDebug,SLOT(ProcessMsg(unsigned char,QString)));
    this->hide();
}

/** Lanza la pantalla de Gestion*/
void Principal::LaunchGestion(){


    if (m_memoryMap->getUserRights()<ACCESS_GOD){
        QMessageBox::warning(this,"ERROR","Acceso restringido",QMessageBox::Ok);
        return;
    }
    Configuracion *ScrConfiguracion;
    ScrConfiguracion=new Configuracion(this);
	ScrConfiguracion->show();
    ScrConfiguracion->setIn2Parent(this);
    this->hide();
}
/** Lanza la pantalla de Maquina*/
void Principal::LaunchMaquina(){
    Maquina *ScrMaquina;
    ScrMaquina=new Maquina(this);
    connect(ScrMaquina, SIGNAL(SendDebugInfo(unsigned char,QString)),m_InfoDebug,SLOT(ProcessMsg(unsigned char,QString)));
    connect(ScrMaquina,SIGNAL(destroyed()),this,SLOT(sltUpdateTableHistorico()));

	ScrMaquina->show();
    emit Refresh();
    this->hide();
    connect(ScrMaquina,SIGNAL(sgnShowLog()),this,SLOT(ForcedShowLogScreen()));
    if (m_gestorSai){
        //connect(m_gestorSai,SIGNAL(sgnSAIActive()),ScrMaquina,SLOT(deleteLater()));
    }

}
/** Lanza el chiller*/
void Principal::sltLaunchChiller(){
    Chiller *ScrChiller;
    ScrChiller=new Chiller(this);
    ScrChiller->show();
    this->hide();
}

/** Lanza la pantalla de Fluidica*/
void Principal::LaunchFluidica(){
    Fluidica *ScrFluidica;
    ScrFluidica=new Fluidica(this);
    ScrFluidica->show();
	this->hide();
}

/** Lanza la pantalla de log*/
void Principal::LaunchLog(QString txt){
    m_InfoDebug->showLog();
    //this->ShowLogScreen();
}

/** Salida automatica*/
void Principal::AutomatedExit(){
    In2GestorLampara *lampara=In2GestorLampara::request(0);
    if (lampara->posDinamica()){
        lampara->moveToReposo();
    }
    //Avisamos al demon
    for (int x=0;x<m_FluidGestor->numEquipos();x++){
        emit SendDebugInfo(LOGGER_ERROR,QString("Apagando equipo %1").arg(x));
        m_FluidGestor->ShutdownEquipo(x);
    }

    m_In2GestorMaquina->EmergencyStop(FIN_SAI);
    emit SendDebugInfo(LOGGER_WARNING,QString("Segundos de funcionamiento UV %1").arg(m_memoryMap->m_mapa.m_maquina.seg_lampara_on));
    m_InfoDebug->deleteLater();
    this->deleteLater();
}
/** Se�alizamos que estamos esperando la salida
void Principal::waitExit(){

}*/

/** Salida*/
void Principal::Exit(){	



    MemoryMap *memoryMap=MemoryMap::request(0);
    if (memoryMap->m_mapa.m_maquina.m_fluidica.bAutoShutdown[0]){
        int respuesta;
        respuesta=QMessageBox::question(this,"ATENCION","Se apagara la fluidica y el equipo automaticamente. Esta accion requiere 5 minutos y no puede revertirse","Si","No",QString::null, 0, 1 );
        if (respuesta==0){
            In2GestorLampara *lampara=In2GestorLampara::request(0);
            if (lampara->posDinamica()){
                if (m_memoryMap->m_mapa.m_maquina.codigo_parada){
                  QMessageBox::warning(this,"ATENCION","Elimine la condicion de alarma para que la lampara se posicione en reposo",QMessageBox::Ok );
                }
                lampara->moveToReposo();
            }
            QTimer::singleShot(0,m_FluidGestor,SLOT(CompleteShutdown()));            
            connect(m_FluidGestor,SIGNAL(CompleteShutdownFinished()),this,SLOT(sltApagarSistema1()));
        }
        else{
            int respuesta;
            respuesta=QMessageBox::question(this,"ATENCION","Quiere salir?Apague a fluidica si va a cerrar el equipo","Si","No",QString::null, 0, 1 );
            if (respuesta==0){
                In2GestorLampara *lampara=In2GestorLampara::request(0);
                if (lampara->posDinamica()){
                    if (m_memoryMap->m_mapa.m_maquina.codigo_parada){
                      QMessageBox::warning(this,"ATENCION","Elimine la condicion de alarma para que la lampara se posicione en reposo",QMessageBox::Ok );
                    }
                    lampara->moveToReposo();
                }
                QTimer::singleShot(0,this,SLOT(sltFullShutdown()));
            }
        }
   }
   else{
        int respuesta;
        respuesta=QMessageBox::question(this,"ATENCION","Quiere salir?Apague la fluidica si va a cerrar el equipo","Si","No",QString::null, 0, 1 );
        if (respuesta==0){
            In2GestorLampara *lampara=In2GestorLampara::request(0);
            if (lampara->posDinamica()){
                if (m_memoryMap->m_mapa.m_maquina.codigo_parada){
                  QMessageBox::warning(this,"ATENCION","Elimine la condicion de alarma para que la lampara se posicione en reposo",QMessageBox::Ok );
                }
                lampara->moveToReposo();
            }
            QTimer::singleShot(0,this,SLOT(sltFullShutdown()));
        }
    }
}
/** Cierra todo menos fluidica*/
void Principal::sltFullShutdown(){
    //Apagamos lampara
    ClienteOPC *opc=ClienteOPC::request(0);
    opc->EscribirDato(ITEM_LAMPARA_VISION,QVariant(0));
    //Apagamos chiller
    In2GestorChiller *gestorChiller=In2GestorChiller::request(0,1);
    if (gestorChiller->isEnabled()){
        gestorChiller->sltStopChiller(0);
    }
    m_In2GestorMaquina->EmergencyStop(FIN_EMERGENCIA);
    //Insertar_Evento(FIN_SESION,m_memoryMap->getUser());
    emit SendDebugInfo(LOGGER_WARNING,QString("Segundos de funcionamiento de UV %1").arg(m_memoryMap->m_mapa.m_maquina.seg_lampara_on));

    m_InfoDebug->deleteLater();


    this->close();

}

//Evento DataChange para el caso de VersionMaquina=2
void Principal::EvDataExt1(int& numItems, QVariantList& ClientHandles, QVariantList& ItemValues, QVariantList& Qualities){

}


/** Recibe la info*/
void Principal::ReceiveDebugInfo(unsigned char codigo, QString mensaje){

    m_InfoDebug->ProcessMsg(codigo,mensaje);

}

/** Recibe la info*/
void Principal::ReceiveInformerInfo(unsigned char codigo, QString mensaje){

    m_InfoInformer->ProcessMsg(codigo,mensaje);

}

/** Ense�a la ventana de log*/
void Principal::ShowLogScreen(){
    m_InfoDebug->showLog();
}
/** Ense�a la ventana de log*/
void Principal::ForcedShowLogScreen(){
    m_InfoDebug->forcedShowLog();
}

/** Comprueba los ficheros de la aplicacion*/
bool Principal::ComprobacionFicherosAplicacion(){
    bool bRet=true;
    QString appDir=QApplication::applicationDirPath();
    QString pathTotest=QString("%1\\Database\\log.sqlite").arg(appDir);
    bool bol=QFile::exists(pathTotest);
    if (!bol){
        emit SendDebugInfo(LOGGER_ERROR,QString("No existe el archivo: %1").arg(pathTotest));
        bRet=false;
    }

    pathTotest=QString("%1\\sqldrivers\\qsqlite4.dll").arg(appDir);
    bol=QFile::exists(pathTotest);
    if (!bol) {
        emit SendDebugInfo(LOGGER_ERROR,QString("No existe el archivo: %1").arg(pathTotest));
        bRet=false;
    }

    pathTotest=QString("%1\\Config\\In2.ini").arg(appDir);
    bol=QFile::exists(pathTotest);
    if (!bol){
        emit SendDebugInfo(LOGGER_ERROR,QString("No existe el archivo: %1").arg(pathTotest));
        bRet=false;
    }
    pathTotest=QString("%1\\Config\\In2Parameters.ini").arg(appDir);
    bol=QFile::exists(pathTotest);
    if (!bol){
        emit SendDebugInfo(LOGGER_ERROR,QString("No existe el archivo: %1").arg(pathTotest));
        bRet=false;
    }
    return bRet;
}

/** Apagamos el sistema*/
void Principal::sltApagarSistema(){
    int rtn=QMessageBox::warning(this,"WARNING","Se apagaran fluidica y maquina . Desea continuar?",QMessageBox::Ok | QMessageBox::Cancel);
    if (rtn==QMessageBox::Ok){
        In2Message *p=new In2Message(this,"APAGANDO FLUIDICA...");
        connect(this,SIGNAL(sgnKillMsg()),p,SLOT(deleteLater()));
        connect(m_FluidGestor,SIGNAL(CompleteShutdownFinished()),this,SLOT(sltApagarSistema1()));
        m_FluidGestor->CompleteShutdown();
    }
}
/** Esperamos a la fluidica*/
void Principal::sltApagarSistema1(){
    emit sgnKillMsg();
    //Faltaria apagar ordenador
    emit SendDebugInfo(LOGGER_WARNING,QString("SYSTEM SHUTDOWN in 20 sg"));
    QString app=QString("%1\\Config\\batch\\In2Shutdown.bat").arg(QApplication::applicationDirPath());

    QProcess m_signProcess;
    m_signProcess.startDetached("cmd.exe", QStringList() << "/c" << app);
    if (m_signProcess.waitForStarted()){
        app.clear();
    }

    sltFullShutdown();
}
/** Hay un flanco OPC**/
void Principal::sltFlancoOPC(int signal){
    QString txt="OK";
    if (signal==ITEM_CODIGO_PARADA){
        if (m_memoryMap->m_mapa.m_maquina.codigo_parada){
            txt=m_In2GestorMaquina->getCodeErrorMaquina();
            QTableWidgetItem *p=ui->m_twMaquina->item(4,0);
            if (p){
                p->setText(txt);
                p->setTextColor(QColor(Qt::white));
            }
        }
        else{
            QTableWidgetItem *p=ui->m_twMaquina->item(4,0);
            if (p){
                p->setText(txt);
                p->setTextColor(QColor(Qt::black));
            }
        }
    }
}
