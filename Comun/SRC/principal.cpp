/** @addtogroup Principal
 *  @{*/
/** @file Principal.cpp
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright Cemitec. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Dialogo principal para la aplicación inkjet
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: Principal.cpp
 *    @li Versión: x.0
 *    @li Fecha: 01/04/08
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */
#include <QMouseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QTime>
#include <QWaitCondition>
#include <QStringList>
#include "principal.h"
#include "configuracion.h"
#include "teclado.h"
#include <Windows.h>
#include <string.h>
#include <QSettings>
#include "MemoryMap.h"
#include "dbimpresion.h"
//#include "dbgmem.h"
#include <QPalette>
#include "truco.h"
#include "maquina.h"
#include "serialport.h"
#include "comthread.h"
#include "niveltintas.h"
#include "smtp.h"
#include <QProcess>
#include <QSharedMemory>
#include "status.h"
#include "clienteopc.h"
#include "parimpresion.h"
#include "imgfixgenerator.h"
#include "imgvargenerator.h"
#include "inkjetdebug.h"
#include "webcam.h"
#include "XCL_Library.h"
#include <QProcess>
#include "curvas.h"
QString usuario;
QString password;

QPalette paleta_textedit_enabled;
QPalette paleta_textedit_disabled;

QString mensaje_error_maquina;

bool LastItemAdded;

QVariant CondicionesImpresion_ch_v;
QVariant SecuenciaOn_ch_v;
QVariant Impresion_ch_v;
QVariant MetrosAct_ch_v;
QVariant MetrosRef_ch_v;
QVariant DiametroEntradaActual_ch_v;
QVariant DiametroEntradaParo_ch_v;
QVariant CodigoParada_ch_v;

InkjetDebug *InfoDebug;
unsigned int contador_signal=0;
QString TablaDebug[N_ITEMS_TABLA_DEBUG];

static unsigned int contador_tlamp_our=0;

//Constructor
 Principal::Principal(QWidget *parent)
    : /*InkjetForm(parent),*/ui(new Ui::PrincipalUI)
{
     ui->setupUi(this);
     ui->DbImprButton->setVisible(false);
     GlobalSetScreenSize(this);
     GlobalSetMainWindow(this);
     m_NumXUsb=0;
     memoryMap->m_mapa.m_maquina.EstadoMaquina=ST_REPOSO;
     NivelDebug=LOGGER_WARNING;
     InfoDebug=new InkjetDebug(0);
miCliente=NULL;
    this->setDataCommon();
    m_Temporizador = new QTimer(this);

    QObject::connect(m_Temporizador, SIGNAL(timeout()),this, SLOT(UpdateTemporizador()));
    QObject::connect(ui->GestionButton, SIGNAL(clicked()),this, SLOT(LaunchGestion()));
    QObject::connect(ui->FluidicaButton, SIGNAL(clicked()),this, SLOT(LaunchFluidica()));
    QObject::connect(ui->MachineButton, SIGNAL(clicked()),this, SLOT(LaunchMaquina()));
    QObject::connect(ui->DbImprButton, SIGNAL(clicked()),this, SLOT(LaunchParImpresion()));
    QObject::connect(ui->DbComposicionButton, SIGNAL(clicked()),this, SLOT(LaunchComposicion()));
    QObject::connect(ui->ExitButton, SIGNAL(clicked()),this, SLOT(Exit()));
    connect(ui->m_ButtonShowLog,SIGNAL(clicked()),this,SLOT(ShowLogScreen()));
    connect(this, SIGNAL(SendDebugInfo(unsigned char,QString)),this, SLOT(ReceiveDebugInfo(unsigned char,QString)));

    LastItemAdded=false;

    MuestraImagenPrincipal(memoryMap->m_Imagen);

    usuario=QString("STRANGE");

    //ComprobarIntegridadTablaUsuarios();

    AbrirFicheroIniImpresora();
    AbrirFicheroParametrosImpresora();

    if (memoryMap->m_mapa.m_maquina.VersionMaquina==2){
        DetectSiemensWindow();
    }

    AbrirBaseDeDatosLog();

    //Lo cambiamos de posicion
    #ifdef DLL_XUSB_INTEGRADA
    if (!memoryMap->m_mapa.m_variables.DisablePrinting){
        //Borramos el fichero previo de dll
        QFile dummyFile(XUSBDLL);
        if (dummyFile.exists()){
            dummyFile.remove();
            emit SendDebugInfo(LOGGER_WARNING,QString("Eliminado XUSBDLL.xml previo"));
        }

        m_NumXUsb=ConectarConXUSB();
       for (int Card=0;Card<this->m_NumXUsb;Card++)
           EjecutarStop(Card);////Hacemos el stop
    }
    else{
        emit SendDebugInfo(LOGGER_WARNING,QString("COMUNICACION XUSB DESHABILITADA EN FICHERO INI!!!"));
    }
    #endif


#ifdef __WINDOWS__
	ShowWindow(FindWindow(LPWSTR("Shell_TrayWnd"),NULL), SW_HIDE);	
#endif


#ifdef __WINDOWS__
    #ifdef USER_WINDOWS
        flag_usuario_logeado=1;
        QStringList environment = QProcess::systemEnvironment();
        QStringList usernames=environment.filter(QString("USERNAME"));
        usuario=usernames.at(0);
        usuario.remove(QString("USERNAME="));
        Consultar_usuario_registrado();
        Insertar_Evento(INICIO_SESION,usuario);
        TestRoot();
    #endif

    #ifdef USER_INPUT
        flag_usuario_logeado=0;
        Status *status;
        status=new Status(this);
        status->EnableWindow();
        TestRoot();
        connect(status,SIGNAL(StatusOculto()),this,SLOT(UsuarioIntroducido()));
        status->show();
    #endif

#else
        usuario=QString("Linux");
#endif

	Inicializar_textos();
	Inicializar_factores_conversion();

	paleta_textedit_enabled.setColor(QPalette::Base,QColor(R_TEXTEDIT_ON, G_TEXTEDIT_ON, B_TEXTEDIT_ON, A_TEXTEDIT_ON));
	paleta_textedit_disabled.setColor(QPalette::Base,QColor(R_TEXTEDIT_OFF, G_TEXTEDIT_OFF, B_TEXTEDIT_OFF, A_TEXTEDIT_OFF));

    flag_Hydra_ok=1;
    flag_Hydra_ok_ant=1;

    flag_comunicacion_Hydra_ok=1;
    flag_temperatura_Hydra_ok=1;
    flag_nivel_Hydra_ok=1;
    flag_fill_Hydra_ok=1;

    flag_Maquina_ok=1;
    flag_Maquina_ok_ant=1;

    flag_esperar_PrintComplete=0;

    flag_cambio_user_mode=0;

    flag_led_ok=1;

    flag_led_Maquina_rojo=0;
    flag_led_Lampara_rojo=0;


    index_nivel_Hydra=0;
    for (unsigned char i=0;i<memoryMap->m_mapa.m_maquina.m_fluidica.NumEquipos;i++){
        memoryMap->m_mapa.m_maquina.m_fluidica.Buffer_Nivel[i][0]=0xFFFF;
        memoryMap->m_mapa.m_maquina.m_fluidica.Buffer_Nivel[i][1]=0xFFFF;
        memoryMap->m_mapa.m_maquina.m_fluidica.Buffer_Nivel[i][2]=0xFFFF;
        memoryMap->m_mapa.m_maquina.m_fluidica.Buffer_Nivel[i][3]=0xFFFF;
        memoryMap->m_mapa.m_maquina.m_fluidica.Buffer_Nivel[i][4]=0xFFFF;
    }

    contador_comunicacion_Hydra_ko=0;

    memoryMap->ImagenVariable=0x00;

    if (memoryMap->m_mapa.m_maquina.m_fluidica.Conectado==1){
        InicializarHydra();
    }
    //Inicializo siempre
    miCliente = new ClienteOPC();
    if (memoryMap->m_mapa.m_maquina.m_opc.estado==1){

       //miCliente->Init();
       if (memoryMap->m_mapa.m_maquina.VersionMaquina==1){
            QObject::connect(miCliente->MiServidorOPC ,SIGNAL(EvDataChange(int&, QVariantList&, QVariantList&, QVariantList&)),this,SLOT(EvData(int&, QVariantList&, QVariantList&, QVariantList&)));
       }
       else if (memoryMap->m_mapa.m_maquina.VersionMaquina==2){
           QObject::connect(miCliente->MiServidorOPC ,SIGNAL(EvDataChange(int&, QVariantList&, QVariantList&, QVariantList&)),this,SLOT(EvDataExt1(int&, QVariantList&, QVariantList&, QVariantList&)));
       }
       else{

       }
       LastItemAdded=true;
       emit SendDebugInfo(LOGGER_WARNING,"LastItemAdded=true");

       if (memoryMap->m_mapa.m_maquina.VersionMaquina==1){
           miCliente->EscribirDato(ITEM_CONDICIONES_MOVIMIENTO,QVariant(1));
       }
       else if (memoryMap->m_mapa.m_maquina.VersionMaquina==2){
           miCliente->EscribirDato(ITEM_CONDICIONES_MOVIMIENTO,QVariant(1));
           miCliente->EscribirDato(ITEM_CONDICIONES_MOVIMIENTO,QVariant(1));
           miCliente->EscribirDato(ITEM_CONDICIONES_MOVIMIENTO,QVariant(0));
       }
    }

   MantenimientoPendiente=0;
   memoryMap->m_mapa.m_maquina.seg_lampara_on=0;

   ComprobarOperacionesMantenimiento();

   ComprobacionFicherosAplicacion();

   Leer_Tabla_RefVelocidad_Intensidad();
   Leer_Tabla_RefVelocidad_Intensidad_2C();

   //int mydummy=sizeof(memoryMap->m_mapa);

   m_Temporizador->start(1000);

   //Preparamos las comunicaciones
   m_GestorEthernet=new GestorEthernet();
   //LaunchSWWatchdog();


}


//Destructor
Principal::~Principal()
{
    InfoDebug->deleteLater();
    miCliente->MiServidorOPC->desconectaServidor();
    delete (miCliente);
}
/** Lanzamos SW de Watchdog*/
void Principal::LaunchSWWatchdog(){
    QProcess WdgProcess;
    WdgProcess.startDetached(WDG_FILE);
    if (!WdgProcess.waitForStarted(3000)){
        emit SendDebugInfo(LOGGER_WARNING,QString("Timeout expired launching InterfazWdg"));
    }
    else
        emit SendDebugInfo(LOGGER_WARNING,QString("InterfazWdg launched"));
}

/** Cargamos datos comunes*/
void Principal::setDataCommon(){
    struct stCommon dummyDataCommon;
    dummyDataCommon.fecha=this->ui->fecha;
    //dummyDataCommon.hora=this->ui->hora;
    dummyDataCommon.led_EstadoLampara=this->ui->led_EstadoLampara;
    dummyDataCommon.led_EstadoMaquina=this->ui->led_EstadoMaquina;
    dummyDataCommon.led_EstadoTemperatura=this->ui->led_EstadoTemperatura;
    dummyDataCommon.m_Imagen=this->ui->m_Imagen;
    dummyDataCommon.m_NombreFicheroConfig=this->ui->m_NombreFicheroConfig;
    dummyDataCommon.m_RutaFicheroConfig=this->ui->m_RutaFicheroConfig;
    dummyDataCommon.m_TextoStatus=this->ui->m_TextoStatus;
    dummyDataCommon.m_User=this->ui->m_User;
    dummyDataCommon.usuario_actual=this->ui->usuario_actual;
    dummyDataCommon.m_MostrarInicio=this->ui->m_MostrarInicio;
    dummyDataCommon.m_MostrarAyuda=this->ui->m_MostrarAyuda;
    dummyDataCommon.m_MostrarMantenimiento=this->ui->m_MostrarMantenimiento;
    dataCommon=new InkjetCommon(this,dummyDataCommon);

//Actualizamos
    dataCommon->ShowInfo();
    dataCommon->ShowEstadoImpresora();
    dataCommon->ShowComms();

}

void Principal::TestRoot(){

    ui->MachineButton->setEnabled(true);

    if (isRoot>=1){
        ui->DbImprButton->setEnabled(true);
        ui->DbComposicionButton->setEnabled(true);
        ui->GestionButton->setEnabled(true);
        if (memoryMap->m_mapa.m_maquina.m_fluidica.Conectado==1){
            ui->FluidicaButton->setEnabled(true);
        }
        if (memoryMap->m_mapa.m_maquina.m_opc.estado==1){
            ui->MachineButton->setEnabled(true);
        }
    }
    else{
        ui->DbImprButton->setEnabled(false);
        ui->DbComposicionButton->setEnabled(false);
        ui->GestionButton->setEnabled(true);
        ui->FluidicaButton->setEnabled(false);
        ui->MachineButton->setEnabled(false);
    }

}

void Principal::UsuarioIntroducido(){
    QString dummy_usuario=QString("Usuario: %1").arg(usuario);
    this->ui->usuario_actual->setText(dummy_usuario);
    this->ui->usuario_actual->show();
    dataCommon->ShowInfo();
    Insertar_Evento(INICIO_SESION,dummy_usuario);
    Consultar_derechos_usuario();
    dataCommon->RefreshUserMode();
    flag_usuario_logeado=1;
    TestRoot();
}



//Muestra la imagen central
void Principal::MuestraImagenPrincipal(QPixmap Imagen){

    unsigned char escalado;

    int offset_x;
    int offset_y;

    int lw=this->ui->m_Imagen->width();
    int lh=this->ui->m_Imagen->height();
    int iw=Imagen.width();
    int ih=Imagen.height();

    if ((iw==0)||(ih==0)) return;
    escalado=EscaladoDeImagenEnQLabel(lw,lh,iw,ih);

    QPixmap pic(this->ui->m_Imagen->size());
    QPainter p(&pic);   // create a QPainter for it
    //pic.fill(Qt::gray);

    QLinearGradient lg(0, 0, 0, this->ui->m_Imagen->height());
    lg.setColorAt(0, Qt::gray);
    //lg.setColorAt(0, QColor(18,18,18));
    lg.setColorAt(1, Qt::lightGray);
    p.setPen(Qt::NoPen);
    p.setBrush(lg);
    p.drawRect(ui->m_Imagen->rect());
    QPixmap dummy;

    switch (escalado){
        case IMAGEN_SINESCALAR:
            //dummy=Imagen;
            //Reescalo tambien esta parte para hacer zoom
            if ((1000*Imagen.height()/this->ui->m_Imagen->height())>(1000*Imagen.width()/this->ui->m_Imagen->width())){
                dummy=Imagen.scaledToHeight(this->ui->m_Imagen->height(),Qt::SmoothTransformation);
            }
            else{
                dummy=Imagen.scaledToWidth(this->ui->m_Imagen->width(),Qt::SmoothTransformation);
            }

            offset_x=(this->ui->m_Imagen->width()-dummy.width())/2;
            offset_y=(this->ui->m_Imagen->height()-dummy.height())/2;
            p.drawPixmap(offset_x,offset_y,dummy); // and draw your original pixmap on it
            this->ui->m_Imagen->setPixmap((QPixmap)(pic));
            break;
        case IMAGEN_S2W:
            dummy=Imagen.scaledToWidth(this->ui->m_Imagen->width(),Qt::SmoothTransformation);
            offset_x=(this->ui->m_Imagen->width()-dummy.width())/2;
            offset_y=(this->ui->m_Imagen->height()-dummy.height())/2;
            p.drawPixmap(offset_x,offset_y,dummy); // and draw your original pixmap on it
            this->ui->m_Imagen->setPixmap((QPixmap)(pic));
            break;
        case IMAGEN_S2H:
            dummy=Imagen.scaledToHeight(this->ui->m_Imagen->height(),Qt::SmoothTransformation);
            offset_x=(this->ui->m_Imagen->width()-dummy.width())/2;
            offset_y=(this->ui->m_Imagen->height()-dummy.height())/2;
            p.drawPixmap(offset_x,offset_y,dummy); // and draw your original pixmap on it
            this->ui->m_Imagen->setPixmap((QPixmap)(pic));
            break;
	}

    p.end();
    this->ui->m_Imagen->show();

}

//Lanza la pantalla de parametros de impresion
void Principal::LaunchComposicion(){
	DbImpresion *ScrDbImpresion;
	ScrDbImpresion=new DbImpresion(0);
	ScrDbImpresion->show();
	this->hide();	
}

void Principal::LaunchGestion(){
	Configuracion *ScrConfiguracion;
	ScrConfiguracion=new Configuracion(0);
	ScrConfiguracion->show();
	this->hide();	
}

void Principal::LaunchMaquina(){
	Maquina *ScrMaquina;
	ScrMaquina=new Maquina(0);
	ScrMaquina->show();
    ScrMaquina->ForceRefreshValues();
	this->hide();	
}

void Principal::LaunchParImpresion(){
    ParImpresion *ScrParImpresion;
    ScrParImpresion=new ParImpresion(0);
    ScrParImpresion->show();
    this->hide();
}


void Principal::LaunchFluidica(){
	NivelTintas *ScrNivelTintas;
	ScrNivelTintas=new NivelTintas(0);
	ScrNivelTintas->show();
	this->hide();
}

//Actualiza parametros de temporizador
void Principal::UpdateTemporizador(){
	
	static unsigned char iRefresh=0;


    if (memoryMap->m_mapa.m_maquina.m_opc.estado==1){
        emit ActualizaDataOPC();
        if (memoryMap->m_mapa.m_maquina.VersionMaquina==2){
            DetectSiemensWindow();
        }

        if (flag_referencia_velocidad_alcanzada==0) Control_Referencia_Velocidad();
    }

    ActualizaDataSlot();
    emit ActualizaData();

    if (flag_cambio_user_mode==1){
        dataCommon->RefreshUserMode();
        TestRoot();
        flag_cambio_user_mode=0;
    }

    if (iRefresh==5) {

        dataCommon->ShowComms();
        //if (flag_cambio_user_mode==1){
        //    dataCommon->RefreshUserMode();
        //    TestRoot();
        //    flag_cambio_user_mode=0;
        //}

		iRefresh=0;
	}
	iRefresh++;
}	


void Principal::Exit(){	
	
	int respuesta;
    respuesta=QMessageBox::question(this,"ATENCION","¿Está seguro que desea salir?","Si","No",QString::null, 0, 1 );
	if (respuesta==0){
        //Avisamos al demon
        EthMessage *ans=new EthMessage(0,UDP_ANS_STATE);
        QByteArray p;
        p[0]=(char)ST_EXIT;
        ans->CreateMessageWithData(UDP_ANS_STATE,p);
        m_GestorEthernet->SendControlDatagram(ans,QHostAddress(QHostAddress::LocalHost),DEFAULT_SUPERVISION_PORT);

#ifdef DLL_XUSB_INTEGRADA
        for (int Card=0;Card<this->m_NumXUsb;Card++)
            EjecutarStop(Card);////Hacemos el stop
#endif
        Insertar_Evento(FIN_SESION,usuario);
        emit SendDebugInfo(LOGGER_WARNING,QString("Segundos de funcionamiento de la lámpara %1").arg(memoryMap->m_mapa.m_maquina.seg_lampara_on));
        Insertar_Horas_Funcionamiento_Lampara(memoryMap->m_mapa.m_maquina.seg_lampara_on);
        CerrarBaseDeDatosLog();

        GuardarFicheroParametrosImpresora();

        //HacerCopiaDeSeguridadLog();
        this->close();
        InfoDebug->close();
	}
}


//Evento DataChange para el caso de VersionMaquina=1
void Principal::EvData(int& numItems, QVariantList& ClientHandles, QVariantList& ItemValues, QVariantList& Qualities){

    int codigo_parada;
    unsigned char ucodigo_parada;
    QString valor_codigo_parada;
    QVariant permiso_impresion;
    QVariant permiso_impresion_bool;
    static bool ci_bool=false;
    static bool ci_bool_ant=false;

    if (LastItemAdded==true){
        bool dummy_b;
        dummy_b=ClientHandles.contains(CondicionesImpresion_ch_v);
        if (dummy_b==true){
            //permiso_impresion=miCliente->MiServidorOPC->leeDato(OPC_Bobinadora.CondicionesImpresion.client_handle);
            permiso_impresion=miCliente->LeerDato(ITEM_CONDICIONES_IMPRESION);
            //permiso_impresion_bool=permiso_impresion.toBool();
            //if (permiso_impresion_bool==false) {
            ci_bool=permiso_impresion.toBool();
            if ((ci_bool==false) && (ci_bool_ant==true)) {
                #ifdef DLL_XUSB_NO_INTEGRADA
                NombreVentana();//Pulsamos una P sobre laventana de XUSB para hacer un soft-stop
                #endif
                #ifdef DLL_XUSB_INTEGRADA
                /*for (int Card=0;Card<m_NumXUsb;Card++)
                    EjecutarSoftStop(Card);////Hacemos el soft-stop*/
                EjecutarFullSoftStop();
                #endif
                miCliente->DecelerarMaquina();
                flag_esperar_PrintComplete=1;
                emit SendDebugInfo(LOGGER_ERROR,"Ejecutado soft stop por Condiciones Impresion");
                codigo_parada=miCliente->LeerDato(ITEM_CODIGO_PARADA).toInt();
                ucodigo_parada=(unsigned char)((codigo_parada&0xFF00)>>8);
                valor_codigo_parada=QString("%1").arg(ucodigo_parada);
                Insertar_Evento(ERROR_MAQUINA,valor_codigo_parada);
                flag_Maquina_ok=0;
                flag_led_Maquina_rojo=1;
            }
        }
        ci_bool_ant=ci_bool;
    }

}


//Evento DataChange para el caso de VersionMaquina=2
void Principal::EvDataExt1(int& numItems, QVariantList& ClientHandles, QVariantList& ItemValues, QVariantList& Qualities){

    QString valor_codigo_parada;
    QVariant secuencia_on;
    static bool secuencia_on_bool=false;
    static bool secuencia_on_bool_ant=false;
    bool dummy_so,dummy_im;
    bool dummy_ma;
    bool dummy_da;
    bool dummy_cp;
    int codigo_parada;
    static bool impresion_on_bool=false;
    static bool impresion_on_bool_ant=false;

    if (LastItemAdded==true){

        dummy_so=ClientHandles.contains(SecuenciaOn_ch_v);
        dummy_ma=ClientHandles.contains(MetrosAct_ch_v);
        dummy_da=ClientHandles.contains(DiametroEntradaActual_ch_v);
        dummy_im=ClientHandles.contains(Impresion_ch_v);
        dummy_cp=ClientHandles.contains(CodigoParada_ch_v);

       /* unsigned char dummy_codigo=0;
        if ((dummy_cp)||(dummy_so)){
            codigo_parada=miCliente->LeerDato(ITEM_CODIGO_PARADA).toInt();
            dummy_codigo=(unsigned char)((codigo_parada&0xFF00)>>8);
            if (dummy_codigo==8) emit SendDebugInfo(LOGGER_WARNING,"Fallo en arrastre...deleted");
            else if (dummy_codigo==0) emit SendDebugInfo(LOGGER_WARNING,"Detectado boton softstop");
        }*/
        if ((dummy_so==true) || (dummy_cp==true)){
            //Ha cambiado el valor de SecuenciaOn o CodigoParada
            secuencia_on=miCliente->LeerDato(ITEM_SECUENCIA_ON);
            secuencia_on_bool=secuencia_on.toBool();
            emit SendDebugInfo(LOGGER_WARNING,QString("SecuenciaOn=%1").arg(secuencia_on_bool));

            codigo_parada=miCliente->LeerDato(ITEM_CODIGO_PARADA).toInt();
            //memoryMap->m_mapa.m_maquina.codigo_parada=(unsigned char)((codigo_parada&0xFF00)>>8);
            memoryMap->m_mapa.m_maquina.codigo_parada=(unsigned char)((codigo_parada&0xFF00)>>8);
            if ((memoryMap->m_mapa.m_maquina.codigo_parada&0x01)==0x01) emit SendDebugInfo(LOGGER_ERROR,"Seta de emergencia");
            else{
                 if ((memoryMap->m_mapa.m_maquina.codigo_parada&0x02)==0x02) emit SendDebugInfo(LOGGER_ERROR,"Fallo en variador salida");
                 if ((memoryMap->m_mapa.m_maquina.codigo_parada&0x04)==0x04) emit SendDebugInfo(LOGGER_ERROR,"Fallo en variador entrada");
                 if ((memoryMap->m_mapa.m_maquina.codigo_parada&0x08)==0x08) emit SendDebugInfo(LOGGER_ERROR,"Fallo en arrastre");
                 if ((memoryMap->m_mapa.m_maquina.codigo_parada&0x10)==0x10) emit SendDebugInfo(LOGGER_ERROR,"Bandeja fuera de posición");
                 if ((memoryMap->m_mapa.m_maquina.codigo_parada&0x20)==0x20) emit SendDebugInfo(LOGGER_ERROR,"Mesa de impresión fuera de posición");
                 if ((memoryMap->m_mapa.m_maquina.codigo_parada&0x40)==0x40) emit SendDebugInfo(LOGGER_ERROR,"Capota abierta");
                 if ((memoryMap->m_mapa.m_maquina.codigo_parada&0x80)==0x80) emit SendDebugInfo(LOGGER_ERROR,"Presión de aire insuficiente");

            }
            //emit SendDebugInfo(LOGGER_ERROR,QString("Código parada: %1").arg(memoryMap->m_mapa.m_maquina.codigo_parada));

            if ((secuencia_on_bool==true) && (secuencia_on_bool_ant==false)){
                //Hemos pasado de false a true = hemos entrado en modo automático
                memoryMap->m_mapa.m_maquina.InicioSecuenciaAutomatico=0x01;
                //memoryMap->m_mapa.m_maquina.MaquinaEnAutomatico=0x01;
                memoryMap->m_mapa.m_maquina.EstadoSecuenciaAutomatico=0x01;
                emit SendDebugInfo(LOGGER_WARNING,"Maquina en automático");
            }

            if ( (secuencia_on_bool==false) && (secuencia_on_bool_ant==true) ){
                 memoryMap->m_ImagenEnviada=false; //ESTA ES NUEVA
                 emit SendDebugInfo(LOGGER_WARNING,"Maquina en manual");
            }

            if ( (secuencia_on_bool==false) && (secuencia_on_bool_ant==true) &&
                (memoryMap->m_mapa.m_maquina.codigo_parada==0) && (memoryMap->m_mapa.m_maquina.impresion==true)){
                //Hemos pasado de true a false = hemos entrado en modo manual
                //HAY QUE HACER UN SOFT-STOP!!!
                valor_codigo_parada=QString("%1").arg(memoryMap->m_mapa.m_maquina.codigo_parada);
                Insertar_Evento(PARO_MAQUINA,valor_codigo_parada);
                memoryMap->m_mapa.m_maquina.softstopear=true;
                //memoryMap->m_ImagenEnviada=false; //ESTA ES NUEVA
                QApplication::setStyle(new InkjetStyle(THEME_EMERGENCY));
                emit SendDebugInfo(LOGGER_ERROR,"Ejecutado soft stop por Botón rojo");
            }


            if ((memoryMap->m_mapa.m_maquina.codigo_parada!=0) && (memoryMap->m_mapa.m_maquina.impresion==true)){
                //Ha cambiado SecuenciaOn de true a false ó
                //el CodigoParada es distinto de cero
                valor_codigo_parada=QString("%1").arg(memoryMap->m_mapa.m_maquina.codigo_parada);
                emit SendDebugInfo(LOGGER_WARNING,QString("Realizando parada %1").arg(valor_codigo_parada));
                Insertar_Evento(ERROR_MAQUINA,valor_codigo_parada);
                memoryMap->m_mapa.m_maquina.stopear=true;
                memoryMap->m_ImagenEnviada=false; //ESTA ES NUEVA
                QApplication::setStyle(new InkjetStyle(THEME_EMERGENCY));
                /*if ((memoryMap->m_mapa.m_maquina.codigo_parada&0x01)==0x01) emit SendDebugInfo(LOGGER_ERROR,"Seta de emergencia");
                else{
                     if ((memoryMap->m_mapa.m_maquina.codigo_parada&0x02)==0x02) emit SendDebugInfo(LOGGER_ERROR,"Fallo en variador salida");
                     if ((memoryMap->m_mapa.m_maquina.codigo_parada&0x04)==0x04) emit SendDebugInfo(LOGGER_ERROR,"Fallo en variador entrada");
                     if ((memoryMap->m_mapa.m_maquina.codigo_parada&0x08)==0x08) emit SendDebugInfo(LOGGER_ERROR,"Fallo en arrastre");
                     if ((memoryMap->m_mapa.m_maquina.codigo_parada&0x10)==0x10) emit SendDebugInfo(LOGGER_ERROR,"Bandeja fuera de posición");
                     if ((memoryMap->m_mapa.m_maquina.codigo_parada&0x20)==0x20) emit SendDebugInfo(LOGGER_ERROR,"Mesa de impresión fuera de posición");
                }*/
                //HAY QUE HACER UNA PARADA INMEDIATA
                #ifdef DLL_XUSB_INTEGRADA
                for (int Card=0;Card<this->m_NumXUsb;Card++)
                    EjecutarStop(Card);////Hacemos el stop
                #endif
                miCliente->EscribirDato(ITEM_CONDICIONES_MOVIMIENTO,QVariant(0));
                miCliente->EscribirDato(ITEM_CONDICIONES_MOVIMIENTO,QVariant(0));
                miCliente->EscribirDato(ITEM_CONDICIONES_MOVIMIENTO,QVariant(1));

            }
            //Muy mal...obviamos el error 8. A revisar!!!
            secuencia_on_bool_ant=secuencia_on_bool;
        } //if (dummy_so==true)

        if (dummy_ma==true) memoryMap->m_mapa.m_maquina.metros_trabajo=miCliente->LeerDato(ITEM_METROS_ACT).toInt();

        if (dummy_da==true) memoryMap->m_mapa.m_maquina.diametro_bobina_entrada=miCliente->LeerDato(ITEM_DIAMETRO_ENTRADA_ACT).toInt();

        if (dummy_im==true){
           memoryMap->m_mapa.m_maquina.impresion=miCliente->LeerDato(ITEM_IMPRESION).toBool();
           impresion_on_bool=memoryMap->m_mapa.m_maquina.impresion;
           emit SendDebugInfo(LOGGER_WARNING,QString("Impresion: %1").arg(memoryMap->m_mapa.m_maquina.impresion));
           if (memoryMap->m_mapa.m_maquina.impresion==false){
               if (memoryMap->m_mapa.m_maquina.stopear) {
                   //emit SendDebugInfo(LOGGER_ERROR,"Parada por Error");
                   flag_Maquina_ok=0;
                   flag_led_Maquina_rojo=1;
                   memoryMap->m_mapa.m_maquina.stopear=false;
               }
               memoryMap->m_mapa.m_maquina.codigo_parada=0;
               //emit SendDebugInfo(LOGGER_WARNING,"Maquina en manual");
           } //if (memoryMap->m_mapa.m_maquina.impresion==false)

           if ((impresion_on_bool==false) && (impresion_on_bool_ant==true)){
               emit SendDebugInfo(LOGGER_WARNING,"Impresion ha pasado de true a false");
               miCliente->EscribirDato(ITEM_CONDICIONES_MOVIMIENTO,QVariant(0));
               miCliente->EscribirDato(ITEM_CONDICIONES_MOVIMIENTO,QVariant(0));
               miCliente->EscribirDato(ITEM_CONDICIONES_MOVIMIENTO,QVariant(1));
           }
           impresion_on_bool_ant=impresion_on_bool;
        } //if (dummy_im==true)

    } //if (LastItemAdded==true){

}
/** Testeo y conbtrol de Hydras*/
void Principal::ControlHydra(){
        if (memoryMap->m_mapa.m_maquina.m_fluidica.Conectado==1){
        if (memoryMap->m_mapa.m_maquina.m_fluidica.Estado==1) LeerHydra();
        else{
            flag_Hydra_ok_ant=flag_Hydra_ok;
            if (contador_comunicacion_Hydra_ko>=5){
                flag_Hydra_ok=0;
                flag_temperatura_Hydra_ok=0;
                flag_nivel_Hydra_ok=0;
                flag_fill_Hydra_ok=0;
            }
            else{
                flag_Hydra_ok=1;
                flag_temperatura_Hydra_ok=1;
                flag_nivel_Hydra_ok=1;
                flag_fill_Hydra_ok=1;
                contador_comunicacion_Hydra_ko++;
            }
        }
    }
    //Hay un error al iniciar algun hydra
    if (flag_error_iniciando_hydra==1){
        for(int i=0;i<memoryMap->m_mapa.m_maquina.m_fluidica.NumEquipos;i++){
            if (flag_error_iniciando[i]==1){
                Insertar_Evento(ERROR_HYDRA,QString("Error iniciando equipo %1").arg(i+1));
                emit SendDebugInfo(LOGGER_WARNING,QString("Fallo iniciando Hydra").arg(i+1));
            }
        }
        QApplication::setStyle(new InkjetStyle(THEME_EMERGENCY));
        ShowLogScreen();
        flag_error_iniciando_hydra=0;
    }

    //Salimos del rango de temperatura
    if ((flag_Hydra_ok_ant==1) && (flag_Hydra_ok==0)){
        memoryMap->m_mapa.m_maquina.softstopear=true;
        if (flag_comunicacion_Hydra_ok==0){
            Insertar_Evento(ERROR_HYDRA,QString("Comunicación"));
            emit SendDebugInfo(LOGGER_WARNING,"Fallo de comunicación con Hydra");
        }
        else if (flag_temperatura_Hydra_ok==0){
            Insertar_Evento(ERROR_HYDRA,QString("Temperatura"));
            emit SendDebugInfo(LOGGER_WARNING,"Temperatura de tinta incorrecta");
        }
        else if (flag_nivel_Hydra_ok==0){
            Insertar_Evento(ERROR_HYDRA,QString("Nivel"));
            emit SendDebugInfo(LOGGER_WARNING,"Nivel interno de tinta incorrecto");
        }
        else{
            Insertar_Evento(ERROR_HYDRA,QString("Tanque"));
            emit SendDebugInfo(LOGGER_WARNING,"Falta tinta en tanque externo");
            Insertar_Aviso_Tinta();
        }

        emit SendDebugInfo(LOGGER_WARNING,"Soft-stop ejecutado por Hydra");

        QApplication::setStyle(new InkjetStyle(THEME_EMERGENCY));

    }

    //Entramos en el rango de temperatura
    if ((flag_Hydra_ok_ant==0) && (flag_Hydra_ok==1)){
        if (memoryMap->m_mapa.m_maquina.m_opc.estado==1){
            //Entramos en el rango de temperatura: hay que habilitar el movimiento
            //QVariant valor=QVariant(1);
            //miCliente->MiServidorOPC->escribeDato(OPC_Bobinadora.CondicionesMovimiento.client_handle,valor);
            miCliente->DentroRangoTemperatura();
        }
    }
//#endif //Hydra
}

/** Monitoriza temperatura lampara*/
void Principal::MonitorTemperaturaLampara(){
    if (memoryMap->m_mapa.m_maquina.m_opc.estado==1){
        memoryMap->m_mapa.m_maquina.temperatura_lampara=miCliente->LeerDato(ITEM_TEMPERATURA_LAMPARA).toInt();
        if (memoryMap->m_mapa.m_maquina.temperatura_lampara>memoryMap->m_mapa.m_maquina.TemperaturaMaximaLampara){
            contador_tlamp_our++;
            if (contador_tlamp_our==5){
                //A los 5 segundos de estar por encima de una Tª, stopeamos
                emit SendDebugInfo(LOGGER_ERROR,"Temperatura de lámpara excesiva");
                Insertar_Evento(ERROR_LAMPARA,"Temperatura excesiva");
                QApplication::setStyle(new InkjetStyle(THEME_EMERGENCY));
                //HAY QUE HACER UNA PARADA INMEDIATA
                #ifdef DLL_XUSB_INTEGRADA
                for (int Card=0;Card<this->m_NumXUsb;Card++)
                    EjecutarStop(Card);////Hacemos el stop
                #endif
                miCliente->EscribirDato(ITEM_CONDICIONES_MOVIMIENTO,QVariant(0));
                miCliente->EscribirDato(ITEM_CONDICIONES_MOVIMIENTO,QVariant(0));
                miCliente->EscribirDato(ITEM_CONDICIONES_MOVIMIENTO,QVariant(1));
             }
        }
        else{
            contador_tlamp_our=0;
        }
    }
}

void Principal::ActualizaDataSlot(){

    //bool softstopear;
    dataCommon->ShowEstadoImpresora();
    static bool softstoprealizado=false;
    static int contadorParadaDiametro=0;
    bool macula_detectada;
    int respuesta;
    bool condiciones_ok;

    ControlHydra();

    //Inicio de secuencia automático
    if (memoryMap->m_mapa.m_maquina.InicioSecuenciaAutomatico==0x01){
        contadorParadaDiametro=0;
        switch (memoryMap->m_mapa.m_maquina.EstadoSecuenciaAutomatico){
            case 1:
                emit SendDebugInfo(LOGGER_DEBUG,"Secuencia Automático: Caso 1");
                condiciones_ok=ComprobacionesPreviasImpresion();
                if (condiciones_ok==true){
                    //Tiene que haber imagen enviada
                    if (memoryMap->m_ImagenEnviada==false) {
                        //if (!dummyFlag){
                            int respuesta=QMessageBox::question(this,"ATENCION","No hay imagen enviada\n¿Está seguro que desea continuar?","Si","No",QString::null, 0, 1 );
                            if (respuesta==0){
                                memoryMap->m_mapa.m_maquina.EstadoSecuenciaAutomatico=2;
                            }
                            else{
                                memoryMap->m_mapa.m_maquina.InicioSecuenciaAutomatico=0x00;
                                memoryMap->m_mapa.m_maquina.EstadoSecuenciaAutomatico=1;
                                //Añado estas lineas para que el PLC eche abajo la secuencia de impresión
                                miCliente->EscribirDato(ITEM_CONDICIONES_MOVIMIENTO,QVariant(0));
                                miCliente->EscribirDato(ITEM_CONDICIONES_MOVIMIENTO,QVariant(0));
                                miCliente->EscribirDato(ITEM_CONDICIONES_MOVIMIENTO,QVariant(1));
                            }
                         //}
                    }
                    else{
                        //Hay que enviar la última imagen seleccionada
                        memoryMap->m_mapa.m_maquina.EstadoSecuenciaAutomatico=2;
                    }
                }
                else{
                    //No se dan las condiciones previas a la impresión
                    memoryMap->m_mapa.m_maquina.InicioSecuenciaAutomatico=0x00;
                    memoryMap->m_mapa.m_maquina.EstadoSecuenciaAutomatico=1;
                    //Añado estas lineas para que el PLC eche abajo la secuencia de impresión
                    miCliente->EscribirDato(ITEM_CONDICIONES_MOVIMIENTO,QVariant(0));
                    miCliente->EscribirDato(ITEM_CONDICIONES_MOVIMIENTO,QVariant(0));
                    miCliente->EscribirDato(ITEM_CONDICIONES_MOVIMIENTO,QVariant(1));
                }

                break;
            case 2:
                //Hay que leer el detector de macula
                respuesta=QMessageBox::question(this,"ATENCION","¿Desea buscar mácula antes de continuar?","Si","No",QString::null, 0, 1 );
                if (respuesta==0){
                    //Hay que buscar mácula
                    miCliente->EscribirDato(ITEM_BUSCAR_MACULA,QVariant(1));
                    miCliente->EscribirDato(ITEM_BUSCAR_MACULA,QVariant(0));
                    memoryMap->m_mapa.m_maquina.EstadoSecuenciaAutomatico=3;
                }
                else{
                    //Continuamos sin buscar mácula
                    memoryMap->m_mapa.m_maquina.EstadoSecuenciaAutomatico=4;
                }
                ///No se necesita qui no?
                //Asignacion_Referencia_Velocidad();
                /*emit SendDebugInfo(LOGGER_WARNING,QString("Velocidad arranque %1 durante %2 msg").arg(REF_VELOCIDAD_MIN).arg(TIEMPO_ARRANQUE));
                miCliente->EscribirDato(ITEM_VELOCIDAD_REF,QVariant((signed short)(REF_VELOCIDAD_MIN)));*/
                break;
            case 3:
                //Hay que esperar a que se encuentre mácula
                macula_detectada=miCliente->LeerDato(ITEM_BUSCANDO_MACULA).toBool();
                if (macula_detectada==false){
                    memoryMap->m_mapa.m_maquina.EstadoSecuenciaAutomatico=4;
                }
                break;
            case 4:
                for (int Card=0;Card<this->m_NumXUsb;Card++){
                    bXaarScorpionGenerateProductDetect(Card);
                }
                #ifdef DLL_XUSB_INTEGRADA
                    for (int Card=0;Card<this->m_NumXUsb;Card++)
                        EnableHeads(Card);////Hacemos el stop
                #endif
                //Decimos al PLC que empiece a moverse...primero a minima velocidad, luego ya veremos
                TestDiametroMinimoVelocidad();
                emit SendDebugInfo(LOGGER_WARNING,QString("Velocidad arranque %1 durante %2 msg").arg(VEL_MIN_ARRANQUE).arg(TIEMPO_ARRANQUE));
                //miCliente->EscribirDato(ITEM_VELOCIDAD_REF,QVariant((signed short)(VEL_MIN_ARRANQUE)));
                SetNuevaVelocidadReferencia(VEL_MIN_ARRANQUE);
                miCliente->EscribirDato(ITEM_MARCHA_MOVIMIENTO,QVariant(1));
                miCliente->EscribirDato(ITEM_MARCHA_MOVIMIENTO,QVariant(0));
                memoryMap->m_mapa.m_maquina.InicioSecuenciaAutomatico=0x00;
                memoryMap->m_mapa.m_maquina.EstadoSecuenciaAutomatico=1;
                //dummyFlag=0;
                memoryMap->m_mapa.m_maquina.EstadoMaquina=ST_IMPRIMIENDO;
                QApplication::setStyle(new InkjetStyle(THEME_PRINTING));
                //La nueva referencia entrará en TIEMPO_ARRANQUE, para dar tiempo a revcalcular diametros
                QTimer::singleShot(TIEMPO_ARRANQUE,this,SLOT(ControlVelocidadArranque()));

                AjustarIntensidadLampara(MOVIMIENTO_AUTOMATICO);
                flag_led_Maquina_rojo=0;
                break;
        } // switch (memoryMap->m_mapa.m_maquina.EstadoSecuenciaAutomatico)
    } // if (memoryMap->m_mapa.m_maquina.InicioSecuenciaAutomatico==0x01)


    //Ver si hemos acabado por longitud de trabajo o por diametro de entrada
    //if (memoryMap->m_mapa.m_maquina.MaquinaEnAutomatico==0x01){
    if (memoryMap->m_mapa.m_maquina.impresion==true){

        if (memoryMap->m_mapa.m_maquina.metros_trabajo>=memoryMap->m_mapa.m_maquina.ref_metros_trabajo){
            if (softstoprealizado==false){
                emit SendDebugInfo(LOGGER_ERROR,"Longitud de trabajo finalizada");

                memoryMap->m_mapa.m_maquina.softstopear=true;
                //memoryMap->m_ImagenEnviada=false; //ESTA ES NUEVA
                Insertar_Evento(ERROR_MAQUINA,QString("Fin de trabajo"));
            }
        }

        if (memoryMap->m_mapa.m_maquina.diametro_bobina_entrada<=memoryMap->m_mapa.m_maquina.diametro_parar){
            if (softstoprealizado==false){ 
                //Si estamos 2 metros por encima o por debajo de la velocidad de referencia, considero que estamos acelerando
                if (qAbs(memoryMap->m_mapa.m_maquina.velocidad/100-memoryMap->m_mapa.m_maquina.ref_velocidad)>=DISCREPANCIA_VELOCIDAD){
                    SendDebugInfo(LOGGER_WARNING,QString("Discrepancia diametro entrada acelerando"));
                    contadorParadaDiametro=0;
                }
                else {
                    contadorParadaDiametro++;
                    emit SendDebugInfo(LOGGER_ERROR,QString("Diametro de entrada insuficiente %1").arg(contadorParadaDiametro));
                }
                //3 errores consecutivos hacen parada
                if (contadorParadaDiametro>=CONTADOR_ERROR_DIAMETRO){
                    emit SendDebugInfo(LOGGER_ERROR,"PARADA Diametro de entrada insuficiente");
                    memoryMap->m_mapa.m_maquina.softstopear=true;
                    //memoryMap->m_ImagenEnviada=false; //ESTA ES NUEVA
                    Insertar_Evento(ERROR_MAQUINA,QString("Diametro de entrada"));
                    contadorParadaDiametro=0;
                }
            }
        }
        else{
            contadorParadaDiametro=0;
        }

    } //if (memoryMap->m_mapa.m_maquina.impresion==true){
    else{
        softstoprealizado=false;
    }

    //Siempre hay que monitorizar la temperatura de la lámpara
    MonitorTemperaturaLampara();



    if (memoryMap->m_mapa.m_maquina.m_opc.estado==1){
        memoryMap->m_mapa.m_maquina.velocidad=miCliente->LeerDato(ITEM_VELOCIDAD_ACT).toInt(); //velocidad actual en 100*m/min
        memoryMap->m_mapa.m_maquina.lampara_on=miCliente->LeerDato(ITEM_LAMPARA_ON).toBool();
        memoryMap->m_mapa.m_maquina.temperatura_lampara=miCliente->LeerDato(ITEM_TEMPERATURA_LAMPARA).toInt();
        if (memoryMap->m_mapa.m_maquina.lampara_on==true){
            memoryMap->m_mapa.m_maquina.seg_lampara_on++;
        }
    }

    static unsigned char flag_decelerar_maquina=0;
    if (memoryMap->m_mapa.m_maquina.softstopear){
        if (memoryMap->m_mapa.m_maquina.m_opc.estado==1){
            if (flag_decelerar_maquina==0){
                miCliente->DecelerarMaquina();
                flag_decelerar_maquina=1;
            }

        }
        //Introduzco la condición de velocidad para hacer el soft-stop
        if (memoryMap->m_mapa.m_maquina.velocidad<1500){ //15 m/min
            #ifdef DLL_XUSB_NO_INTEGRADA
            NombreVentana();//Pulsamos una P sobre la ventana de XUSB para hacer un soft-stop
            #endif

            #ifdef DLL_XUSB_INTEGRADA
            /*for (int Card=0;Card<this->m_NumXUsb;Card++)
                EjecutarSoftStop(Card);//Hacemos el soft-stop*/
            EjecutarFullSoftStop();
            #endif

            emit SendDebugInfo(LOGGER_ERROR,"Soft-stop realizado");

            memoryMap->m_mapa.m_maquina.softstopear=false;
            softstoprealizado=true;
            flag_esperar_PrintComplete=1;
            flag_decelerar_maquina=0;
        }
    } //if (softstopear)


#ifdef DLL_XUSB_NO_INTEGRADA
    if (flag_esperar_PrintComplete==1){
        HandleXUSBVentanaPrintComplete();
        if (NumeroVentanas>=2){
            //Ya ha salido la ventana de PrintComplete
            if (memoryMap->m_mapa.m_maquina.m_opc.estado==1){
                miCliente->ImpresionFinalizada();
            }
        }
        QApplication::setStyle(new InkjetStyle(THEME_NORMAL));
        flag_Maquina_ok=1;
        flag_led_Maquina_rojo=0;
        emit SendDebugInfo(LOGGER_ERROR,"Parada normal");
    } //if (flag_esperar_PrintComplete==1)
#endif


#ifdef DLL_XUSB_INTEGRADA
    BOOL bPC;
    BOOL bPC_i;
    if (flag_esperar_PrintComplete==1){
        //bPC=bXaarScorpionPrintComplete(0x00);
        bPC=TRUE;
        for (int Card=0;Card<this->m_NumXUsb;Card++){
            bPC_i=bXaarScorpionPrintComplete(Card);
            if (bPC_i==FALSE) bPC=FALSE;
        }
        if (bPC){
            emit SendDebugInfo(LOGGER_ERROR,"PrintComplete");
            flag_esperar_PrintComplete=0;
            //Ya ha salido la ventana de PrintComplete
            if (memoryMap->m_mapa.m_maquina.m_opc.estado==1){
                 miCliente->ImpresionFinalizada();
            }
            QApplication::setStyle(new InkjetStyle(THEME_NORMAL));
            flag_Maquina_ok=1;
            flag_led_Maquina_rojo=0;
            //emit SendDebugInfo(LOGGER_ERROR,"Parada normal");
        }
    } //if (flag_esperar_PrintComplete==1)
#endif

    //QIcon icono;
    /*static unsigned char contador_mantenimiento_pendiente=0;
    if (MantenimientoPendiente!=0){
        static bool on_off=false;
        contador_mantenimiento_pendiente++;
        if (contador_mantenimiento_pendiente>2){
            contador_mantenimiento_pendiente=0;
            if (on_off){
                QIcon icono(":res/images/light_on.png");
                ui->m_MostrarMantenimiento->setIcon(icono);
                on_off=false;
            }
            else{
                QIcon icono(":res/images/light_off.png");
                ui->m_MostrarMantenimiento->setIcon(icono);
                on_off=true;
            }
        }
    }
    else{
        contador_mantenimiento_pendiente=0;
    }*/

}
/** Testea si hay un diametro mínimo y reajusta la velocidad de referencia*/
void Principal::TestDiametroMinimoVelocidad(){
    //
    if (memoryMap->m_mapa.m_maquina.diametro_bobina_entrada<=DIAMETRO_MINIMO_ARRANQUE){
        emit SendDebugInfo(LOGGER_WARNING,QString("WARNING: Diametro de entrada menor de %1").arg(DIAMETRO_MINIMO_ARRANQUE));
        if (memoryMap->m_mapa.m_maquina.ref_velocidad>MAX_VELOCIDAD_DIAMETER_LIMITED){
            emit SendDebugInfo(LOGGER_WARNING,QString("WARNING: Velocidad limitada a %1").arg(MAX_VELOCIDAD_DIAMETER_LIMITED));
            memoryMap->m_mapa.m_maquina.ref_velocidad=MAX_VELOCIDAD_DIAMETER_LIMITED;
            SetNuevaVelocidadReferencia(memoryMap->m_mapa.m_maquina.ref_velocidad);
            emit Refresh();
        }
    }
}

/** Ajusta la velocidad de referencia*/
void Principal::SetNuevaVelocidadReferencia(int valor){

    /*int dummy=(miCliente->LeerDato(ITEM_DIAMETRO_SALIDA_ACT).toInt());
    emit SendDebugInfo(LOGGER_WARNING,QString("vel %1 diam %2").arg(valor).arg(dummy));
    if (dummy==0) dummy=90;
    float diam=(float)dummy/1000;
    float rpmValue=(float)valor*10/(diam*3.141592);
    emit SendDebugInfo(LOGGER_WARNING,QString("vel %1 diam %2").arg(rpmValue).arg(diam));
    if (rpmValue>1800) rpmValue=1800;
    else if (rpmValue<50) rpmValue=50;
    emit SendDebugInfo(LOGGER_WARNING,QString("RPM %1").arg((int)rpmValue));
    miCliente->EscribirDato(ITEM_VELOCIDAD_REF,QVariant((int)rpmValue));*/
    miCliente->EscribirDato(ITEM_VELOCIDAD_REF,QVariant(valor));
}

/** Controla el arranque*/
void Principal::ControlVelocidadArranque(){
    /*emit SendDebugInfo(LOGGER_WARNING,QString("Velocidad arranque %1 durante %2 msg").arg(VEL_MIN_ARRANQUE+1).arg(TIEMPO_ARRANQUE));
    miCliente->EscribirDato(ITEM_VELOCIDAD_REF,QVariant((signed short)(VEL_MIN_ARRANQUE+1)));
    AjustarIntensidadLampara(MOVIMIENTO_AUTOMATICO);
    QTimer::singleShot(TIEMPO_ARRANQUE,this,SLOT(Asignacion_Referencia_Velocidad()));*/
    Asignacion_Referencia_Velocidad();

}
/** Testea que el serial de xusb sea el esperado*/
bool Principal::TestSerialXusb(int pos){
    char buffer[128];
    memset(&buffer,0x00,sizeof(buffer));
    bXaarScorpionGetSerialNoMapped(pos,&buffer[0]);
    emit SendDebugInfo(LOGGER_WARNING,QString("Xusb %1 --> %2 (expected %3)").arg(pos).arg(buffer).arg(memoryMap->m_mapa.m_maquina.m_xusbs[pos].SN));
    if (QString(buffer).compare(memoryMap->m_mapa.m_maquina.m_xusbs[pos].SN)!=0) return false;
    else return true;
}

int Principal::ConectarConXUSB(){

    BOOL dummyError;
    DWORD LastError;
    int TotalXUSB=0;
    //Inicializamos XUSB
    dummyError=bXaarScorpionDriverLoaded();
    if (dummyError==0){
        LastError=XaarScorpionGetErrorValue();
        QString dummy=QString("Error de conexión con XUSB. Last Error:%1").arg(LastError);
        emit SendDebugInfo(LOGGER_ERROR,dummy);
        if (memoryMap->m_mapa.m_maquina.m_opc.estado==0x01) miCliente->ComunicacionConXUSBKO();
    }
    else{
        QString dummy=QString("XUSB conectado correctamente:%1").arg(dummyError);
        emit SendDebugInfo(LOGGER_WARNING,dummy);
    }
    TotalXUSB=(int)XaarScorpionGetXUSBCount();
    //Testeamos los numeros de serie de los XUSB
    bool result=true;
    for (int x=0;x<TotalXUSB;x++){
        result=this->TestSerialXusb(x);
        if (!result) {
            ChangeXUSBOrderAuto();
            break;
        }
    }
    //Comprobaciones para cabezales
    DWORD StatusUsb;
    for (int Card = 0; Card < TotalXUSB; Card++){
        dummyError = bXaarScorpionGetUsbStatus(Card,StatusUsb);
        QString dummy=QString("Status XUSB %1=%2").arg(Card).arg(StatusUsb);
        emit SendDebugInfo(LOGGER_DEBUG,dummy);
        /*for (int head=0;head<MAXHEADS;head++){
            if (!bXaarScorpionBootHead(Card,head))emit SendDebugInfo(LOGGER_DEBUG,QString("BootHead error card %1 head %2").arg(Card).arg(head));
        }*/
    }


    DWORD HeadStatus;
    HeadStatus = bXaarScorpionGetHeadStatus(0x00,0x00);
    for (int Card = 0; Card < TotalXUSB; Card++){
        for (DWORD Head = 0; Head < MAXHEADS; Head++){
            HeadStatus = bXaarScorpionGetHeadStatus(Card,Head);
            QString dummy=QString("Status Head %1 XUSB %2=%3").arg(Head).arg(Card).arg(HeadStatus);
            emit SendDebugInfo(LOGGER_DEBUG,dummy);
            //0: = No Head, 1: = Boot in Progress, 2: = Running,
            //3: = Failed boot, 4: = TC Error, 5: = Parameter error
        }
    }

    BOOL FPGAConfigured;
    for (int Card = 0; Card < TotalXUSB; Card++){
        FPGAConfigured=bXaarScorpionFPGAConfigured(Card);
        QString dummy=QString("FPGAConfigured XUSB %1=%2").arg(Card).arg(FPGAConfigured);
        emit SendDebugInfo(LOGGER_DEBUG,dummy);
    }

    for (int Card = 0; Card < TotalXUSB; Card++){
        for (DWORD Head = 0; Head < MAXHEADS; Head++){
            dummyError = bXaarScorpionGetHeadConfig(Card,Head);
            QString dummy=QString("Config Head %1 XUSB %2=%3").arg(Head).arg(Card).arg(dummyError);
            emit SendDebugInfo(LOGGER_DEBUG,dummy);
        }
    }

    DWORD HeadType;
    for (int Card = 0; Card < TotalXUSB; Card++){
        for (DWORD Head = 0; Head < MAXHEADS; Head++){
            HeadType = bXaarScorpionGetHeadType(Card,Head);
            QString dummy=QString("Type Head %1 XUSB %2=%3").arg(Head).arg(Card).arg(HeadType);
            emit SendDebugInfo(LOGGER_DEBUG,dummy);
        }
    }

    return TotalXUSB;

}

void Principal::EjecutarFullSoftStop(){
    DWORD SwatheCountArray[2*MAXHEADS];
    DWORD maxSwathe=0;
    if (!memoryMap->m_mapa.m_variables.DisablePrinting){
    //Petición deSoft-Stop
    //for (int Card=0;Card<this->m_NumXUsb;Card++){
        //Leer los swathes actuales
     bXaarScorpionReadSwathePrintCount(0,SwatheCountArray);

    //Aumentar estos valores en 1
        for(DWORD i=0;i<2*MAXHEADS;i++){
            if (SwatheCountArray[i]>maxSwathe) maxSwathe=SwatheCountArray[i];
        }
        maxSwathe+=1;
        for(DWORD i=0;i<2*MAXHEADS;i++){
            SwatheCountArray[i]=maxSwathe;
        }
        for (int Card=0;Card<this->m_NumXUsb;Card++)
            bXaarScorpionWriteSwathePrintCount(Card,SwatheCountArray);

    //}
    //Petición deSoft-Stop
     for (int Card=0;Card<this->m_NumXUsb;Card++)
          bXaarScorpionPrintStopEndSequence(Card);
          emit FullStopPerformed();
    }

}

void Principal::EjecutarSoftStop(int Card){

    DWORD SwatheCountArray[2*MAXHEADS];

    //Leer los swathes actuales
    bXaarScorpionReadSwathePrintCount(Card,SwatheCountArray);
    //qDebug()<< "Valor leido: " << SwatheCountArray[0];

    //Aumentar estos valores en 2
    for(DWORD i=0;i<2*MAXHEADS;i++) SwatheCountArray[i]+=2;

    //Escribire los valores de parada
    bXaarScorpionWriteSwathePrintCount(Card,SwatheCountArray);

    //Petición deSoft-Stop
    bXaarScorpionPrintStopEndSequence(Card);

    emit FullStopPerformed();

}


void Principal::EjecutarStop(int Card){
    while (bXaarScorpionXUSBBusy(Card)) _Sleep(25);
    bXaarScorpionEnablePrintMode(Card,0xFFFF,false);
}

void Principal::EnableHeads(int Card){
    /*while (bXaarScorpionXUSBBusy(Card)) _Sleep(25);
    bXaarScorpionEnablePrintMode(Card,0xFFFF,true);*/

}

void Principal::ReceiveDebugInfo(unsigned char codigo, QString mensaje){

    QString Message;

    mensaje.prepend(QString("%1 ").arg(QTime::currentTime().toString("hh:mm:ss")));

    TablaDebug[contador_signal]=QString("%1,%2").arg(codigo).arg(mensaje);
    contador_signal++;
    if (contador_signal==N_ITEMS_TABLA_DEBUG) contador_signal=0;

    if (codigo>=NivelDebug){
        switch (codigo){
           case LOGGER_DEBUG:
               Message = QString("<font color=\"green\">%1</font>\r\n").arg(mensaje);
               break;
           case LOGGER_WARNING:
               Message = QString("<font color=\"blue\">%1</font>\r\n").arg(mensaje);
               break;
           case LOGGER_ERROR:
               Message = QString("<font color=\"red\">%1</font>\r\n").arg(mensaje);
               break;
        }
        InfoDebug->ui->m_Info->append(Message);
    }
}
/**Enseña la ventana de log*/
void Principal::ShowLogScreen(){
    InfoDebug->showLog();
}
/** Cambia el orden de los xusb de manera manual*/
void Principal::ChangeXUSBOrder(){
    char buffer[128];

    int TotalXUSB=(int)XaarScorpionGetXUSBCount();
    QMessageBox::warning(0,"ERROR","Apaga todos los xusb y pulsa OK",QMessageBox::Ok);

    for (int x=0;x<TotalXUSB;x++){
        QMessageBox::warning(0,"ERROR",QString("Reinicia XUSB %1 y pulsa OK").arg(x),QMessageBox::Ok);
        QLabel *InfoLabel=ShowMensaje("Procesando...\nPor favor espere");

        //Sleep(5000);

        XaarScorpionReInitialiseSystem();
        delete InfoLabel;

     //QMessageBox::warning(0,"ERROR","Test de numeros de serie",QMessageBox::Ok);
    }
    QMessageBox::warning(0,"ATENCIÓN","Reasignacion completada.",QMessageBox::Ok);
    for (int x=0;x<TotalXUSB;x++){
        memset(&buffer,0x00,sizeof(buffer));
        bXaarScorpionGetSerialNoMapped(x,&buffer[0]);
        emit SendDebugInfo(LOGGER_WARNING,QString("Xusb %1 --> %2").arg(x).arg(buffer));
    }
}
/** Reinicia los XUSB de manera automatica*/
void Principal::ChangeXUSBOrderAuto(){

    int TotalXUSB=(int)XaarScorpionGetXUSBCount();
    QLabel *dummyLabel=ShowMensaje("Discrepancia xusb\nPor favor espere");

    //QMessageBox::warning(0,"ERROR","Apaga todos los xusb y pulsa OK",QMessageBox::Ok);
    if (miCliente){
        miCliente->EscribirDato(ITEM_APAGAR_XUSB1,QVariant(1));
        miCliente->EscribirDato(ITEM_APAGAR_XUSB2,QVariant(1));
    }
    Sleep(5000);
    delete (dummyLabel);
    for (int x=0;x<TotalXUSB;x++){
        QLabel *dummyLabel=ShowMensaje(QString("XUSB%1").arg(x));
        if (miCliente){
            if (x==0) miCliente->EscribirDato(ITEM_APAGAR_XUSB1,QVariant(0));
            else if (x==1) miCliente->EscribirDato(ITEM_APAGAR_XUSB2,QVariant(0));
        }
        Sleep(10000);
        XaarScorpionReInitialiseSystem();
        delete (dummyLabel);
    }

    bool rtn;
    for (int x=0;x<TotalXUSB;x++){
        rtn=TestSerialXusb(x);
        if (rtn==false){
            QMessageBox::warning(0,"ATENCIÓN","No se pudo reasignar automaticamente los XUSB. Proceda manualmente por favor",QMessageBox::Ok);
            return;
        }
    }
    QMessageBox::warning(0,"ATENCIÓN","Reasignacion completada",QMessageBox::Ok);

}


void Principal::AjustarIntensidadLampara(unsigned char tipo_Movimiento){

    int referencia=0;
    int intensidad=0;

    switch (tipo_Movimiento){
        case MOVIMIENTO_AUTOMATICO:
            referencia=miCliente->LeerDato(ITEM_VELOCIDAD_REF).toInt(); //referencia en m/min
            break;
        case MOVIMIENTO_JOGMAS:
            referencia=miCliente->LeerDato(ITEM_VELOCIDAD_JOG_MAS).toInt(); //referencia en mm/min
            referencia=referencia/1000;
            break;
        case MOVIMIENTO_JOGMENOS:
            referencia=miCliente->LeerDato(ITEM_VELOCIDAD_JOG_MENOS).toInt(); //referencia en mm/min
            referencia=referencia/1000;
            break;
    }

    intensidad=Relacion_VelocidadIntensidadLampara(referencia);
    QVariant valor=QVariant(intensidad);
    miCliente->EscribirDato(ITEM_INTENSIDAD_LAMPARA,valor);
}

void Principal::AjustarAceleracion(unsigned char tipo_Movimiento){

    int referencia=0;
    int aceleracion=0;

    switch (tipo_Movimiento){
        case MOVIMIENTO_AUTOMATICO:
            referencia=miCliente->LeerDato(ITEM_VELOCIDAD_REF).toInt(); //referencia en m/min
            break;
        case MOVIMIENTO_JOGMAS:
            referencia=miCliente->LeerDato(ITEM_VELOCIDAD_JOG_MAS).toInt(); //referencia en mm/min
            referencia=referencia/1000;
            break;
        case MOVIMIENTO_JOGMENOS:
            referencia=miCliente->LeerDato(ITEM_VELOCIDAD_JOG_MENOS).toInt(); //referencia en mm/min
            referencia=referencia/1000;
            break;
    }

    aceleracion=Relacion_VelocidadRampaAceleracion(referencia);
    QVariant valor=QVariant(aceleracion);
    miCliente->EscribirDato(ITEM_RAMPA_ACELERACION,valor);
}

void Principal::AjustarDeceleracion(unsigned char tipo_Movimiento){

    int referencia=0;
    int deceleracion=0;

    switch (tipo_Movimiento){
        case MOVIMIENTO_AUTOMATICO:
            referencia=miCliente->LeerDato(ITEM_VELOCIDAD_REF).toInt(); //referencia en m/min
            break;
        case MOVIMIENTO_JOGMAS:
            referencia=miCliente->LeerDato(ITEM_VELOCIDAD_JOG_MAS).toInt(); //referencia en mm/min
            referencia=referencia/1000;
            break;
        case MOVIMIENTO_JOGMENOS:
            referencia=miCliente->LeerDato(ITEM_VELOCIDAD_JOG_MENOS).toInt(); //referencia en mm/min
            referencia=referencia/1000;
            break;
    }

    deceleracion=Relacion_VelocidadRampaDeceleracion(referencia);
    QVariant valor=QVariant(deceleracion);
    miCliente->EscribirDato(ITEM_RAMPA_DECELERACION,valor);


}

void Principal::AjustarTiempoBusquedaMacula(unsigned char tipo_Movimiento){

    int referencia=0;
    int tiempo=0;

    switch (tipo_Movimiento){
        case MOVIMIENTO_AUTOMATICO:
            referencia=miCliente->LeerDato(ITEM_VELOCIDAD_REF).toInt(); //referencia en m/min
            break;
    }

    tiempo=Relacion_VelocidadTiempoBusqueda(referencia);
    QVariant valor=QVariant(tiempo);
    miCliente->EscribirDato(ITEM_TIEMPO_BUSQUEDA_MACULA,tiempo);

}

bool Principal::ComprobacionesPreviasImpresion(void){

    QString comprobaciones;
    bool ok=true;
    int codigo_parada;

    comprobaciones=QString("Alguna de las condiciones de impresión no se cumple:");

    codigo_parada=miCliente->LeerDato(ITEM_CODIGO_PARADA).toInt();
    memoryMap->m_mapa.m_maquina.codigo_parada=(unsigned char)((codigo_parada&0xFF00)>>8);
    if (memoryMap->m_mapa.m_maquina.codigo_parada!=0) ok=false;
    if ((memoryMap->m_mapa.m_maquina.codigo_parada&0x01)==0x01) comprobaciones.append("\nSeta de emergencia");
    else{
         if ((memoryMap->m_mapa.m_maquina.codigo_parada&0x02)==0x02) comprobaciones.append("\nFallo en variador salida");
         if ((memoryMap->m_mapa.m_maquina.codigo_parada&0x04)==0x04) comprobaciones.append("\nFallo en variador entrada");
         if ((memoryMap->m_mapa.m_maquina.codigo_parada&0x08)==0x08) comprobaciones.append("\nFallo en arrastre");
         if ((memoryMap->m_mapa.m_maquina.codigo_parada&0x10)==0x10) comprobaciones.append("\nBandeja fuera de posición");
         if ((memoryMap->m_mapa.m_maquina.codigo_parada&0x20)==0x20) comprobaciones.append("\nMesa de impresión fuera de posición");
         if ((memoryMap->m_mapa.m_maquina.codigo_parada&0x40)==0x40) comprobaciones.append("\nCapota abierta");
         if ((memoryMap->m_mapa.m_maquina.codigo_parada&0x80)==0x80) comprobaciones.append("\nPresión de aire insuficiente");
    }

    if (memoryMap->m_mapa.m_maquina.metros_trabajo>=memoryMap->m_mapa.m_maquina.ref_metros_trabajo){
       comprobaciones.append("\nLongitud de trabajo finalizada");
       ok=false;
    }

    if (memoryMap->m_mapa.m_maquina.diametro_bobina_entrada<memoryMap->m_mapa.m_maquina.diametro_parar){
       comprobaciones.append("\nDiámetro de entrada insuficiente");
       ok=false;
    }

    if (memoryMap->m_mapa.m_maquina.temperatura_lampara>memoryMap->m_mapa.m_maquina.TemperaturaMaximaLampara){
        comprobaciones.append("\nTemperatura de lámpara excesiva");
        ok=false;
    }

    //Comprobar que haya tinta en el tanque exterior
    for(int i=0;i<memoryMap->m_mapa.m_maquina.m_fluidica.NumEquipos;i++){
        //short dummy=memoryMap->m_mapa.m_maquina.m_fluidica.switches[i];
        if (flag_fill_Hydra_ok==0){
            ok=false;
            comprobaciones.append(QString("\nFalta tinta en tanque del equipo %1: %2").arg(i+1).arg(memoryMap->m_mapa.m_maquina.m_fluidica.Descripcion.at(i)));
        }
        if ((memoryMap->m_mapa.m_maquina.m_fluidica.Modo[i]!=XCL_MODE_Started) && (memoryMap->m_mapa.m_maquina.m_fluidica.Used[i]==true)){
            ok=false;
            comprobaciones.append(QString("\nEquipo %1: %2 no está en modo Started").arg(i+1).arg(memoryMap->m_mapa.m_maquina.m_fluidica.Descripcion.at(i)));
        }
        if (flag_temperatura_Hydra_ok==0){
            ok=false;
            comprobaciones.append(QString("\nEquipo %1: %2 está fuera del rango de temperatura").arg(i+1).arg(memoryMap->m_mapa.m_maquina.m_fluidica.Descripcion.at(i)));
        }
    }

    if (!ok) QMessageBox::warning(0,"Atención",QString("%1").arg(comprobaciones),QMessageBox::Ok);

    return ok;
}

bool Principal::ComprobacionFicherosAplicacion(){

    //Los ficheros que deben existir son los siguientes:
    //db Log
    //sqldrivers
    //config.ini

    bool bRet;

    bool b_Log;
    QString path_log;
    bool b_sql_drivers;
    QString path_sql_drivers;
    bool b_config_ini;
    QString path_config_ini;
    bool b_params_ini;
    QString path_params_ini;
    bool b_gs;
    bool b_ghost_script[5];
    QString path_ghost_script[5];

    path_log=QString("%1").arg(AbsolutePathDbLog);
    b_Log=QFile::exists(path_log);
    if (!b_Log) emit SendDebugInfo(LOGGER_ERROR,QString("No existe el archivo: %1").arg(path_log));

    path_sql_drivers=QString("%1\\sqldrivers\\qsqlite4.dll").arg(InkjetPath);
    b_sql_drivers=QFile::exists(path_sql_drivers);
    if (!b_sql_drivers) emit SendDebugInfo(LOGGER_ERROR,QString("No existe el archivo: %1").arg(path_sql_drivers));

    path_config_ini=QString("%1\\Config\\In2.ini").arg(InkjetPath);
    b_config_ini=QFile::exists(path_config_ini);
    if (!b_config_ini) emit SendDebugInfo(LOGGER_ERROR,QString("No existe el archivo: %1").arg(path_config_ini));

    path_params_ini=QString("%1\\Config\\In2Parameters.ini").arg(InkjetPath);
    b_params_ini=QFile::exists(path_config_ini);
    if (!b_params_ini) emit SendDebugInfo(LOGGER_ERROR,QString("No existe el archivo: %1").arg(path_params_ini));


    path_ghost_script[0]=QString("C:\\Program Files\\gs\\gs9.06\\bin\\gswin64c.exe");
    path_ghost_script[1]=QString("C:\\Program Files\\gs\\gs9.05\\bin\\gswin32c.exe");
    path_ghost_script[2]=QString("C:\\Archivos de programa\\gs\\gs8.53\\bin\\gswin32c.exe");
    path_ghost_script[3]=QString("C:\\Archivos de programa\\gs\\gs8.51\\bin\\gswin32c.exe");
    path_ghost_script[4]=QString("C:\\gs\\gs8.14\\bin\\gswin32c.exe");

    b_gs=false;
    for(int i=0;i<5;i++){
        b_ghost_script[i]=QFile::exists(path_ghost_script[i]);
        if (b_ghost_script[i]) b_gs=true;
    }

    if (!b_gs){
        QString dummy;
        dummy=QString("No existe ninguno de los siguientes archivos:");
        for(int i=0;i<5;i++){
            dummy.append(QString("\n%1").arg(path_ghost_script[i]));
        }
        emit SendDebugInfo(LOGGER_ERROR,QString("%1").arg(dummy));
    }

    bRet= b_Log & b_sql_drivers & b_config_ini & b_params_ini & b_gs;

    return bRet;

}


void Principal::Asignacion_Referencia_Velocidad(){

    if (memoryMap->m_mapa.m_maquina.ref_velocidad>MIN_CONTROL_VELOCIDAD){
        memoryMap->m_mapa.m_maquina.ref_velocidad_actual=MIN_CONTROL_VELOCIDAD;
        //miCliente->EscribirDato(ITEM_VELOCIDAD_REF,QVariant(memoryMap->m_mapa.m_maquina.ref_velocidad_actual));
        SetNuevaVelocidadReferencia(memoryMap->m_mapa.m_maquina.ref_velocidad_actual);
        flag_referencia_velocidad_alcanzada=0;
        emit SendDebugInfo(LOGGER_WARNING,QString("CONTROL SOBREVELOCIDAD ACTIVADO"));
        emit SendDebugInfo(LOGGER_WARNING,QString("Nueva referencia velocidad: %1").arg(memoryMap->m_mapa.m_maquina.ref_velocidad_actual));
    }
    else{
        flag_referencia_velocidad_alcanzada=1;
        emit SendDebugInfo(LOGGER_WARNING,QString("Velocidad: %1").arg(memoryMap->m_mapa.m_maquina.ref_velocidad));
        emit SendDebugInfo(LOGGER_WARNING,QString("CONTROL SOBREVELOCIDAD DESACTIVADO"));
        //miCliente->EscribirDato(ITEM_VELOCIDAD_REF,QVariant(memoryMap->m_mapa.m_maquina.ref_velocidad));
        SetNuevaVelocidadReferencia(memoryMap->m_mapa.m_maquina.ref_velocidad);
    }
    //Necesario hacerlo aqui, ya que antes hemos arrancado con velocidad mínimo
    AjustarIntensidadLampara(MOVIMIENTO_AUTOMATICO);
}

void Principal::Control_Referencia_Velocidad(){

    int velocidad_actual;

    velocidad_actual=memoryMap->m_mapa.m_maquina.velocidad; //velocidad actual en 100*m/min

    if (velocidad_actual>=(memoryMap->m_mapa.m_maquina.ref_velocidad_actual-1)*100){
        memoryMap->m_mapa.m_maquina.contador_velocidad++;
        if (memoryMap->m_mapa.m_maquina.contador_velocidad>=COUNTER_VELOCIDAD){
            memoryMap->m_mapa.m_maquina.contador_velocidad=0;
            if (memoryMap->m_mapa.m_maquina.ref_velocidad_actual<memoryMap->m_mapa.m_maquina.ref_velocidad){
                memoryMap->m_mapa.m_maquina.ref_velocidad_actual++;
                //miCliente->EscribirDato(ITEM_VELOCIDAD_REF,QVariant(memoryMap->m_mapa.m_maquina.ref_velocidad_actual));
                SetNuevaVelocidadReferencia(memoryMap->m_mapa.m_maquina.ref_velocidad_actual);
                AjustarIntensidadLampara(MOVIMIENTO_AUTOMATICO);
                emit SendDebugInfo(LOGGER_WARNING,QString("Nueva referencia velocidad: %1").arg(memoryMap->m_mapa.m_maquina.ref_velocidad_actual));
            }
            else{
                flag_referencia_velocidad_alcanzada=1;
                emit SendDebugInfo(LOGGER_WARNING,QString("Velocidad: %1").arg(memoryMap->m_mapa.m_maquina.ref_velocidad_actual));
                emit SendDebugInfo(LOGGER_WARNING,QString("CONTROL SOBREVELOCIDAD DESACTIVADO"));

            }
        }
    }
}
/** Recarga diametro de entrada de bobina*/
void Principal::ReloadDiametroEntrada(){
    QLabel *InfoLabel=ShowMensaje("Por favor espere");
    miCliente->EscribirDato(ITEM_DESHABILITAR_SERVO_ENTRADA,QVariant(1));
    _Sleep(500);
    QVariant valor=QVariant(memoryMap->m_mapa.m_maquina.diametro_bobina_entrada);
    miCliente->EscribirDato(ITEM_DIAMETRO_ENTRADA_REF,valor);
    _Sleep(500);
    valor=QVariant(true);
    miCliente->EscribirDato(ITEM_FIJAR_DIAMETRO_ENTRADA,valor);
    _Sleep(500);
    valor=QVariant(false);
    miCliente->EscribirDato(ITEM_FIJAR_DIAMETRO_ENTRADA,valor);
    _Sleep(500);
    miCliente->EscribirDato(ITEM_DESHABILITAR_SERVO_ENTRADA,QVariant(0));
    delete InfoLabel;
}
/** Recarga diametro de salida de bobina*/
void Principal::ReloadDiametroSalida(){
    QLabel *InfoLabel=ShowMensaje("Por favor espere");
    miCliente->EscribirDato(ITEM_DESHABILITAR_SERVO_SALIDA,QVariant(1));
    _Sleep(500);
    QVariant valor=QVariant(memoryMap->m_mapa.m_maquina.ref_diametro_bobina_salida);
    miCliente->EscribirDato(ITEM_DIAMETRO_SALIDA_REF,valor);
    _Sleep(500);
    valor=QVariant(true);
    miCliente->EscribirDato(ITEM_FIJAR_DIAMETRO_SALIDA,valor);
    _Sleep(500);
    valor=QVariant(false);
    miCliente->EscribirDato(ITEM_FIJAR_DIAMETRO_SALIDA,valor);
    _Sleep(500);
    miCliente->EscribirDato(ITEM_DESHABILITAR_SERVO_SALIDA,QVariant(0));
    delete InfoLabel;
}


/** Paso 1 secuencia empalme*/
void Principal::SecuenciaEmpalme_1(){
    emit SendDebugInfo(LOGGER_WARNING,"Inicio secuencia empalme");
    miCliente->EscribirDato(ITEM_DESHABILITAR_SERVO_ENTRADA,QVariant(1));
    QMessageBox::warning(this,"ATENCION","Pulse continuar cuando termine de empalmar la bobina","Continuar","Cancelar",QString::null, 0, 1 );
    memoryMap->m_mapa.m_maquina.EstadoSecuenciaEmpalme=2;
    miCliente->EscribirDato(ITEM_DESHABILITAR_SERVO_ENTRADA,QVariant(0));
    QTimer::singleShot(100,this, SLOT(SecuenciaEmpalme_2()));

}
/** Paso 2 secuencia empalme*/
void Principal::SecuenciaEmpalme_2(){
    int respuesta=QMessageBox::warning(this,"ATENCION","¿Ha cambiado la bobina de SALIDA?Al terminar el proceso compruebe que el diametro de salida es correcto","SI","NO",QString::null, 0, 1 );
    if (respuesta==0){
        memoryMap->m_mapa.m_maquina.ref_diametro_bobina_salida=DEFAULT_DIAMETRO_SALIDA;
        ReloadDiametroSalida();
    }
    else{
        respuesta=QMessageBox::warning(this,"ATENCION","¿Ha cambiado la bobina de ENTRADA?Al terminar el proceso compruebe que el diametro de salida es correcto","SI","NO",QString::null, 0, 1 );
        if (respuesta==0){
            memoryMap->m_mapa.m_maquina.ref_diametro_bobina_entrada=DEFAULT_DIAMETRO_ENTRADA;
                memoryMap->m_mapa.m_maquina.diametro_bobina_entrada=DEFAULT_DIAMETRO_ENTRADA;
                ReloadDiametroEntrada();
            }
        }
        //miCliente->EscribirDato(ITEM_DESHABILITAR_SERVO_ENTRADA,QVariant(0));
        memoryMap->m_mapa.m_maquina.EstadoSecuenciaEmpalme=3;
        QTimer::singleShot(100,this, SLOT(SecuenciaEmpalme_3()));
}
/** Paso 3 secuencia empalme*/
void Principal::SecuenciaEmpalme_3(){
    miCliente->EscribirDato(ITEM_DESPLAZAR_FILM,QVariant(1));
    miCliente->EscribirDato(ITEM_DESPLAZAR_FILM,QVariant(0));
    memoryMap->m_mapa.m_maquina.EstadoSecuenciaEmpalme=4;
    QTimer::singleShot(100,this, SLOT(SecuenciaEmpalme_4()));
}
/** Paso 4 secuencia empalme*/
void Principal::SecuenciaEmpalme_4(){
    bool fin_desplazamiento;
    static QLabel *dummy=NULL;
    fin_desplazamiento=miCliente->LeerDato(ITEM_DESPLAZANDO_FILM).toBool();
    if (fin_desplazamiento==false) {
        memoryMap->m_mapa.m_maquina.EstadoSecuenciaEmpalme=5;
        if (dummy){
            delete (dummy);
            dummy=NULL;
        }
        QTimer::singleShot(100,this, SLOT(SecuenciaEmpalme_5()));
    }
    else {
        if (!dummy) dummy=ShowMensaje("Por favor espere");
        QTimer::singleShot(500,this, SLOT(SecuenciaEmpalme_4()));
    }
}
/** Paso 5 secuencia empalme*/
void Principal::SecuenciaEmpalme_5(){
    int respuesta=QMessageBox::question(this,"ATENCION","¿Desea buscar mácula antes de continuar?","Si","No",QString::null, 0, 1 );
    if (respuesta==0){
    //Hay que buscar mácula

        miCliente->EscribirDato(ITEM_BUSCAR_MACULA,QVariant(1));
        miCliente->EscribirDato(ITEM_BUSCAR_MACULA,QVariant(0));
        memoryMap->m_mapa.m_maquina.EstadoSecuenciaEmpalme=6;
        QTimer::singleShot(100,this, SLOT(SecuenciaEmpalme_6()));
    }
    else{
    //Continuamos sin buscar mácula
        memoryMap->m_mapa.m_maquina.EstadoSecuenciaEmpalme=7;
        QTimer::singleShot(100,this, SLOT(SecuenciaEmpalme_7()));

    }
}
/** Paso 6 secuencia empalme*/
void Principal::SecuenciaEmpalme_6(){
    //Hay que esperar a que se encuentre mácula
    static QLabel *dummy=NULL;
    bool macula_detectada=miCliente->LeerDato(ITEM_BUSCANDO_MACULA).toBool();
    if (macula_detectada==false) {
        if (dummy){
            delete (dummy);
            dummy=NULL;
        }
        memoryMap->m_mapa.m_maquina.EstadoSecuenciaEmpalme=7;
        QTimer::singleShot(100,this, SLOT(SecuenciaEmpalme_7()));
    }
    else {
        if (!dummy) dummy=ShowMensaje("Por favor espere");
        QTimer::singleShot(500,this, SLOT(SecuenciaEmpalme_6()));
    }
}
/** Paso 7 secuencia empalme*/
void Principal::SecuenciaEmpalme_7(){
    memoryMap->m_mapa.m_maquina.EstadoSecuenciaEmpalme=1;
    memoryMap->m_mapa.m_maquina.InicioSecuenciaEmpalme=0x00;
    emit Refresh();
}

/**Paso 1 Secuencia Bobina Nueva*/
void Principal::SecuenciaBobinaNueva_1(){
    emit SendDebugInfo(LOGGER_WARNING,"Inicio secuencia bobina nueva");
    miCliente->EscribirDato(ITEM_DESHABILITAR_SERVO_ENTRADA,QVariant(1));
    //miCliente->EscribirDato(ITEM_DESHABILITAR_SERVO_SALIDA,QVariant(1));
    QMessageBox::warning(this,"ATENCION","Pulse continuar cuando termine de empalmar la bobina","Continuar","Cancelar",QString::null, 0, 1 );
    /*if (respuesta==0){
        QTimer::singleShot(100,this, SLOT(SecuenciaBobinaNueva_2()));
    }
    else if (respuesta==1){
        QTimer::singleShot(100,this, SLOT(SecuenciaBobinaNueva_2()));
    }*/
    miCliente->EscribirDato(ITEM_DESHABILITAR_SERVO_ENTRADA,QVariant(0));
    memoryMap->m_mapa.m_maquina.EstadoSecuenciaBobinaNueva=2;
    QTimer::singleShot(100,this, SLOT(SecuenciaBobinaNueva_2()));

}
/**Paso 2 Secuencia Bobina Nueva*/
void Principal::SecuenciaBobinaNueva_2(){
    int respuesta=QMessageBox::warning(this,"ATENCION","¿Ha cambiado la bobina de SALIDA? Al terminar el proceso compruebe que el diametro de salida es correcto","SI","NO",QString::null, 0, 1 );
    if (respuesta==0){
        memoryMap->m_mapa.m_maquina.ref_diametro_bobina_salida=DEFAULT_DIAMETRO_SALIDA;
          ReloadDiametroSalida();
        }
        else{
            respuesta=QMessageBox::warning(this,"ATENCION","¿Ha cambiado la bobina de ENTRADA?Al terminar el proceso compruebe que el diametro de salida es correcto","SI","NO",QString::null, 0, 1 );
            if (respuesta==0){
                memoryMap->m_mapa.m_maquina.ref_diametro_bobina_entrada=DEFAULT_DIAMETRO_ENTRADA;
                memoryMap->m_mapa.m_maquina.diametro_bobina_entrada=DEFAULT_DIAMETRO_ENTRADA;
                ReloadDiametroEntrada();
            }
        }
                //miCliente->EscribirDato(ITEM_DESHABILITAR_SERVO_ENTRADA,QVariant(0));
        //miCliente->EscribirDato(ITEM_DESHABILITAR_SERVO_SALIDA,QVariant(0));
        memoryMap->m_mapa.m_maquina.EstadoSecuenciaBobinaNueva=3;
        QTimer::singleShot(100,this, SLOT(SecuenciaBobinaNueva_3()));
}
/**Paso 3 Secuencia Bobina Nueva*/
void Principal::SecuenciaBobinaNueva_3(){
    int respuesta=QMessageBox::question(this,"ATENCION","¿Desea buscar mácula antes de continuar?","Si","No",QString::null, 0, 1 );
    if (respuesta==0){
    //Hay que buscar mácula
        miCliente->EscribirDato(ITEM_BUSCAR_MACULA,QVariant(1));
        miCliente->EscribirDato(ITEM_BUSCAR_MACULA,QVariant(0));
        memoryMap->m_mapa.m_maquina.EstadoSecuenciaBobinaNueva=4;
        QTimer::singleShot(100,this, SLOT(SecuenciaBobinaNueva_4()));
    }
    else{
    //Continuamos sin buscar mácula
        memoryMap->m_mapa.m_maquina.EstadoSecuenciaBobinaNueva=5;
        QTimer::singleShot(100,this, SLOT(SecuenciaBobinaNueva_5()));
    }
}
/**Paso 4 Secuencia Bobina Nueva*/
void Principal::SecuenciaBobinaNueva_4(){
    //Hay que esperar a que se encuentre mácula
    static QLabel *dummy=NULL;
    bool macula_detectada=miCliente->LeerDato(ITEM_BUSCANDO_MACULA).toBool();
    if (macula_detectada==false) {
        memoryMap->m_mapa.m_maquina.EstadoSecuenciaBobinaNueva=5;
        if (dummy){
            delete (dummy);
            dummy=NULL;
        }
        QTimer::singleShot(100,this, SLOT(SecuenciaBobinaNueva_5()));

    }
    else {
        if (!dummy) dummy=ShowMensaje("Por favor espere");
        QTimer::singleShot(500,this, SLOT(SecuenciaBobinaNueva_4()));
    }
}
/**Paso 5 Secuencia Bobina Nueva*/
void Principal::SecuenciaBobinaNueva_5(){
    memoryMap->m_mapa.m_maquina.EstadoSecuenciaBobinaNueva=1;
    memoryMap->m_mapa.m_maquina.InicioSecuenciaBobinaNueva=0x00;
    emit Refresh();
}





