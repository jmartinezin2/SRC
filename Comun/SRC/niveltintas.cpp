/** @addtogroup NivelTintas
 *  @{*/
/** @file NivelTintas.cpp
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright Cemitec. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Pantalla de parametros de impresion
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: NivelTintas.cpp
 *    @li Versión: x.0
 *    @li Fecha: 05/05/08
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */

#include "niveltintas.h"
#include "globals.h"
#include "inputdata.h"
#include <QMessageBox>
#include <QFileDialog>
#include "principal.h"
#include <QIcon>
#include <QPixmap>

#include "inkjetform.h"
//#include "dbgmem.h"
#include "omp.h"
#include "XCL_Library.h"

static XCL_ADC ADC_Res_Temp;
static XCL_ADC ADC_Return_Temp;
static XCL_ADC ADC_Supply_Pr;
static XCL_ADC ADC_Return_Pr;
static XCL_Errors error;

NivelTintas::NivelTintas(QWidget *parent)
    : ui(new Ui::FluidicaUI)
{
	
    ui->setupUi(this);
    GlobalSetScreenSize(this);

    this->SetDataCommon();

    connect(ui->m_StartHydra,SIGNAL(clicked()),this,SLOT(StartHydra()));
    connect(ui->m_ShutdownHydra,SIGNAL(clicked()),this,SLOT(ShutdownHydra()));
    connect(ui->m_ResetHydra,SIGNAL(clicked()),this,SLOT(RebootHydra()));

    if ((flag_comunicacion_Hydra_ok==1) && (memoryMap->m_mapa.m_maquina.m_fluidica.Estado==1)){
        ui->m_StartHydra->setEnabled(true);
        ui->m_ShutdownHydra->setEnabled(true);
        ui->m_ResetHydra->setEnabled(true);
    }
    else{
        ui->m_StartHydra->setEnabled(false);
        ui->m_ShutdownHydra->setEnabled(false);
        ui->m_ResetHydra->setEnabled(false);
    }


    for(unsigned char i=0;i<memoryMap->m_mapa.m_maquina.m_fluidica.NumEquipos;i++){
        ui->m_Equipo->addItem(QString("Equipo %1").arg(i+1));
    }
    ui->m_Equipo->setCurrentIndex(0);
    UpdateNumeroEquipo();

    if (memoryMap->m_mapa.m_maquina.m_fluidica.Estado==1){
        if (memoryMap->m_mapa.m_maquina.m_fluidica.Tipo==FLUIDICA_HYDRA){
            LeerHydra();
            ShowEstado();
        }
    }
    else{
        QMessageBox::warning(0,"Atención","No se ha podido comunicar con Hydra",QMessageBox::Ok);
    }

    connect(ui->m_AceptarButton,SIGNAL(clicked()),this,SLOT(AceptarSeleccion()));
    connect(GlobalGetMainWindow(), SIGNAL(ActualizaData()),this, SLOT(ActualizaDataSlot()));
    connect(ui->m_ButtonShowLog,SIGNAL(clicked()),GlobalGetMainWindow(),SLOT(ShowLogScreen()));
    connect(ui->m_Equipo,SIGNAL(activated (QString)),this,SLOT(UpdateNumeroEquipo()));
    connect(ui->m_Used,SIGNAL(clicked()),this,SLOT(UpdateUsarEquipo()));
    connect(ui->m_PauseHydra,SIGNAL(clicked()),this,SLOT(PausarEquipo()));


    connect(this, SIGNAL(SendDebugInfo(unsigned char,QString)),GlobalGetMainWindow(), SLOT(ReceiveDebugInfo(unsigned char,QString)));


	TestRoot();
	

}
/** Pausamos el equipo*/
void NivelTintas::PausarEquipo(){
    XCL_PID ControladorHydra;
    unsigned char indice=ui->m_Equipo->currentIndex();
    XCL_Read_Control(memoryMap->m_mapa.m_maquina.m_fluidica.ID[indice],XCL_PID_Temp,&ControladorHydra);
    if (ControladorHydra.setp1>STANDBY_HYDRATEMPERATURE+5){ //Es un float, mejor no poner la compración con el valor definitivo
        ControladorHydra.setp1=STANDBY_HYDRATEMPERATURE; //Inicializo a 25
    }
    else{
        ControladorHydra.setp1=WORKING_HYDRATEMPERATURE; //lo pongo a 45
    }
    XCL_Write_Control(memoryMap->m_mapa.m_maquina.m_fluidica.ID[indice],XCL_PID_Temp,&ControladorHydra);
    QMessageBox::warning(0,"Atención",QString("Nueva temperatura %1").arg(ControladorHydra.setp1),QMessageBox::Ok);
}



NivelTintas::~NivelTintas()
{

}

//Acepta la selección realizada y vuelve a la pantalla de Configuracion
void NivelTintas::AceptarSeleccion(){
    GlobalShowMainWindow();
	this->deleteLater();
}


/** Actualiza los datos de pantalla*/
void NivelTintas::ActualizaDataSlot(){

    DataCommon->ShowComms();
    DataCommon->ShowEstadoImpresora();

    if (memoryMap->m_mapa.m_maquina.m_fluidica.Estado==1){
        if (memoryMap->m_mapa.m_maquina.m_fluidica.Tipo==FLUIDICA_HYDRA){
            ShowEstado();
            ShowPHInfo(SENSOR_SupplyTemp);
            ShowPHInfo(SENSOR_LevelRes);
            ShowPHInfo(SENSOR_SupplyPr);
            ShowPHInfo(SENSOR_ReturnPr);
            ShowPHInfo(SENSOR_MeniscusPr);
            ShowPHInfo(SENSOR_DifferentialPr);
            ShowPHInfo(SENSOR_ReturnTemp);
            ShowPHInfo(SENSOR_ResTemp);
            ShowPHInfo(LEDs);
            ShowPHInfo(SWITCHES);
        }
    }


}


void NivelTintas::UpdateNumeroEquipo(){

    QPixmap tmp;

    tmp.load(":res/images/Ledambar.png");
    ui->lbl_Estado->setPixmap((QPixmap)tmp);

    ui->lbl_EstadoActual->setText("--");
    ui->lbl_ErrorMode->setText("--");
    ui->lbl_State->setText("--");
    ui->m_FillTankOK->setText("--");

    ui->value_SupplyTemp->setText("--");
    ui->value_LevelRes->setText("--");
    ui->value_ReturnPr->setText("--");
    ui->value_SupplyPr->setText("--");
    ui->value_MeniscusPr->setText("--");
    ui->value_DifferentialPr->setText("--");
    ui->value_ReturnTemp->setText("--");
    ui->value_ResTemp->setText("--");
    ui->value_ResTemp->setText("--");

    int indice_equipo;
    if (memoryMap->m_mapa.m_maquina.m_fluidica.Estado==1){
        indice_equipo=ui->m_Equipo->currentIndex();
        ui->m_Descripcion->setText(memoryMap->m_mapa.m_maquina.m_fluidica.Descripcion.at(indice_equipo));
        QPalette paleta;
        paleta.setColor(QPalette::Base,memoryMap->m_mapa.m_maquina.m_fluidica.color[indice_equipo]);
        ui->m_Color->setPalette(paleta);

        if (memoryMap->m_mapa.m_maquina.m_fluidica.Used[indice_equipo]==true) ui->m_Used->setChecked(true);
        else ui->m_Used->setChecked(false);

        ui->m_ResetHydra->setEnabled(false);
    }




}

void NivelTintas::UpdateUsarEquipo(){

    int indice_equipo;

    indice_equipo=ui->m_Equipo->currentIndex();

    if (ui->m_Used->isChecked())
        memoryMap->m_mapa.m_maquina.m_fluidica.Used[indice_equipo]=true;
    else
        memoryMap->m_mapa.m_maquina.m_fluidica.Used[indice_equipo]=false;
}


/** Si no es root, bloqueo funcionalidad.*/
void NivelTintas::TestRoot(){
    if (memoryMap->getUserRights()<ACCESS_PRINTER){
        ui->m_StartHydra->setEnabled(false);
        ui->m_ShutdownHydra->setEnabled(false);
        ui->m_Used->setEnabled(false);
	}
}


//Carga el Led que se le indique con el pixmap indicado
void NivelTintas::ShowEstado(){

    QPixmap tmp;
    unsigned char indice;

    indice=ui->m_Equipo->currentIndex();

    if (memoryMap->m_mapa.m_maquina.m_fluidica.Modo[indice]==XCL_MODE_Stopped) ui->lbl_EstadoActual->setText("Stopped");
    else if (memoryMap->m_mapa.m_maquina.m_fluidica.Modo[indice]==XCL_MODE_Filling) ui->lbl_EstadoActual->setText("Filling");
    else if (memoryMap->m_mapa.m_maquina.m_fluidica.Modo[indice]==XCL_MODE_Starting_MP) ui->lbl_EstadoActual->setText("Starting MP");
    else if (memoryMap->m_mapa.m_maquina.m_fluidica.Modo[indice]==XCL_MODE_Starting_DP) ui->lbl_EstadoActual->setText("Starting DP");
    else if (memoryMap->m_mapa.m_maquina.m_fluidica.Modo[indice]==XCL_MODE_Starting_T) ui->lbl_EstadoActual->setText("Starting T");
    else if (memoryMap->m_mapa.m_maquina.m_fluidica.Modo[indice]==XCL_MODE_Started) ui->lbl_EstadoActual->setText("Started");
    else if (memoryMap->m_mapa.m_maquina.m_fluidica.Modo[indice]==XCL_MODE_Idling) ui->lbl_EstadoActual->setText("Idling");
    else if (memoryMap->m_mapa.m_maquina.m_fluidica.Modo[indice]==XCL_MODE_Stopping_T) ui->lbl_EstadoActual->setText("Stopping T");
    else if (memoryMap->m_mapa.m_maquina.m_fluidica.Modo[indice]==XCL_MODE_Stopping_DP) ui->lbl_EstadoActual->setText("Stopping DP");
    else if (memoryMap->m_mapa.m_maquina.m_fluidica.Modo[indice]==XCL_MODE_Stopping_MP) ui->lbl_EstadoActual->setText("Stopping MP");
    else if (memoryMap->m_mapa.m_maquina.m_fluidica.Modo[indice]==XCL_MODE_Venting) ui->lbl_EstadoActual->setText("Venting");
    else ui->lbl_EstadoActual->setText("Unknown");

    if (memoryMap->m_mapa.m_maquina.m_fluidica.Modo[indice]==XCL_MODE_Started) tmp.load(":res/images/Ledverde.png");
    else tmp.load(":res/images/Ledrojo.png");
    ui->lbl_Estado->setPixmap((QPixmap)tmp);

    if ((memoryMap->m_mapa.m_maquina.m_fluidica.Modo[indice]==XCL_MODE_Stopped) &&
        (memoryMap->m_mapa.m_maquina.m_fluidica.fault[indice]!=0))
        ui->m_ResetHydra->setEnabled(true);
    else
        ui->m_ResetHydra->setEnabled(false);

}



void NivelTintas::ShowPHInfo(int index){

    float dummy_f;
    float dummy_f1,dummy_f2;
    short dummy_sw;

    unsigned char indice;
    indice=ui->m_Equipo->currentIndex();

    switch (index){
        case SENSOR_SupplyTemp:
                //error=XCL_Read_Sensor(memoryMap->m_mapa.m_maquina.m_fluidica.ID[indice],XCL_SENS_Supply_Temp,&ADC_Supply_Temp);
                //dummy_f=ADC_Supply_Temp.val;
                dummy_f=memoryMap->m_mapa.m_maquina.m_fluidica.Supply_Temp[indice];
                ui->value_SupplyTemp->setText(QString("%1").arg(dummy_f).left(4));
                break;
        case SENSOR_LevelRes:
                //error=XCL_Read_Sensor (memoryMap->m_mapa.m_maquina.m_fluidica.ID[indice],XCL_SENS_Level_Res,&ADC_Level_Res);
                //dummy_f=ADC_Level_Res.val;
                dummy_f=memoryMap->m_mapa.m_maquina.m_fluidica.Level_Res[indice];
                ui->value_LevelRes->setText(QString("%1").arg(dummy_f).left(3));
                break;
        case SENSOR_ReturnPr:
                error=XCL_Read_Sensor (memoryMap->m_mapa.m_maquina.m_fluidica.ID[indice],XCL_SENS_Return_Pr,&ADC_Return_Pr);
                dummy_f=ADC_Return_Pr.val;
                ui->value_ReturnPr->setText(QString("%1").arg(dummy_f).left(6));
                break;
        case SENSOR_SupplyPr:
                error=XCL_Read_Sensor (memoryMap->m_mapa.m_maquina.m_fluidica.ID[indice],XCL_SENS_Supply_Pr,&ADC_Supply_Pr);
                dummy_f=ADC_Supply_Pr.val;
                ui->value_SupplyPr->setText(QString("%1").arg(dummy_f).left(6));
                break;
        case SENSOR_DifferentialPr:
                dummy_f1=ADC_Supply_Pr.val;
                dummy_f2=ADC_Return_Pr.val;
                dummy_f=dummy_f1-dummy_f2;
                ui->value_DifferentialPr->setText(QString("%1").arg(dummy_f).left(6));
                break;
        case SENSOR_MeniscusPr:
                dummy_f1=ADC_Supply_Pr.val;
                dummy_f2=ADC_Return_Pr.val;
                dummy_f=((dummy_f1+dummy_f2)/2)+((memoryMap->m_mapa.m_maquina.m_fluidica.supply_offset[indice]+memoryMap->m_mapa.m_maquina.m_fluidica.return_offset[indice])/2);
                ui->value_MeniscusPr->setText(QString("%1").arg(dummy_f).left(6));
                break;
        case SENSOR_ReturnTemp:
                error=XCL_Read_Sensor (memoryMap->m_mapa.m_maquina.m_fluidica.ID[indice],XCL_SENS_Return_Temp,&ADC_Return_Temp);
                dummy_f=ADC_Return_Temp.val;
                ui->value_ReturnTemp->setText(QString("%1").arg(dummy_f).left(4));
                break;
        case SENSOR_ResTemp:
                error=XCL_Read_Sensor (memoryMap->m_mapa.m_maquina.m_fluidica.ID[indice],XCL_SENS_Res_Temp,&ADC_Res_Temp);
                dummy_f=ADC_Res_Temp.val;
                ui->value_ResTemp->setText(QString("%1").arg(dummy_f).left(4));
                break;
        case LEDs:
                short bits;
                short fault;
                bits=memoryMap->m_mapa.m_maquina.m_fluidica.bits[indice];
                //bits
                //#define XCL_LED_Warning 0x010
                //#define XCL_LED_Ready   0x020
                //#define XCL_LED_Fault   0x040
                //#define XCL_LED_Spare   0x080
                //#define XCL_LED_Heat    0x100
                //#define XCL_LED_Valve   0x200
                if ((bits&0x010)==0x010) ui->lbl_State->setText("<font color=\"orange\">Warning</font>");
                else if ((bits&0x020)==0x020) ui->lbl_State->setText("<font color=\"black\">Ready</font>");
                else if ((bits&0x040)==0x040) ui->lbl_State->setText("<font color=\"red\">Fault</font>");
                else ui->lbl_State->setText("Unknown");
                //fault
                //#define XCL_FLT_Sens            0x000F  /* Mask for last sensor in error */
                //#define XCL_FLT_FLASH           0x0010  /* FLASH error */
                //#define XCL_FLT_NOIMP1          0x0020  /* not implemented*/
                //#define XCL_FLT_TACHO_BOUNDS    0x0040  /* Tacho bounds exceeded. */
                //#define XCL_FLT_ESTOP           0x0080  /* Stop commanded */
                //#define XCL_FLT_OVER_TEMP       0x0200  /* Over temperature */
                //#define XCL_FLT_NOIMP2          0x1000  /* not implemented*/
                //#define XCL_FLT_NO_INK          0x2000  /* Ink errors */
                //#define XCL_FLT_XS_INK          0x4000
                //#define XCL_FLT_PUMP_STALL      0x8000  /* Pump stalled during operating cycle */
                fault=memoryMap->m_mapa.m_maquina.m_fluidica.fault[indice];
                if ((fault&0x0040)==0x0040) ui->lbl_ErrorMode->setText("<font color=\"red\">Tacho bounds exceeded</font>");
                else if ((fault&0x0200)==0x0200) ui->lbl_ErrorMode->setText("<font color=\"red\">Over temperature</font>");
                else if ((fault&0x2000)==0x2000) ui->lbl_ErrorMode->setText("<font color=\"red\">No ink</font>");
                else if ((fault&0x8000)==0x8000) ui->lbl_ErrorMode->setText("<font color=\"red\">Pump stalled</font>");
                else ui->lbl_ErrorMode->setText("");               
                break;
       case SWITCHES:
                dummy_sw=memoryMap->m_mapa.m_maquina.m_fluidica.switches[indice];
                if ((dummy_sw&0x0080)!=0x0080) ui->m_FillTankOK->setText(QString("<font color=\"red\">Fill Tank Ink Error</font>"));
                else ui->m_FillTankOK->setText(QString("<font color=\"black\">Fill Tank Ink OK</font>"));
                break;
        default:
            break;
	}
}

void NivelTintas::RebootHydra(){

    unsigned char indice=ui->m_Equipo->currentIndex();

    int respuesta;
    respuesta=QMessageBox::question(this,"ATENCION","¿Está seguro que desea resetear Hydra?","Si","No",QString::null, 0, 1 );
    if (respuesta==0){
        if (memoryMap->m_mapa.m_maquina.m_fluidica.Estado==1){
            error=XCL_Send_Command(memoryMap->m_mapa.m_maquina.m_fluidica.ID[indice],XCL_CMD_Reboot);
        }
    }
}


void NivelTintas::StartHydra(){

    unsigned char indice=ui->m_Equipo->currentIndex();

    QString msg=QString("¿Está seguro que desea iniciar\nEquipo %1 (%2)?").arg(indice+1).arg(memoryMap->m_mapa.m_maquina.m_fluidica.Descripcion.at(indice));

    int respuesta;
    respuesta=QMessageBox::question(this,"ATENCION",msg,"Si","No",QString::null, 0, 1 );
    if (respuesta==0){
        if (memoryMap->m_mapa.m_maquina.m_fluidica.Estado==1){
            error=XCL_Send_Command(memoryMap->m_mapa.m_maquina.m_fluidica.ID[indice],XCL_CMD_Start);
            flag_error_iniciando[indice]=0;
        }
    }
}


void NivelTintas::StopHydra(){

    unsigned char indice=ui->m_Equipo->currentIndex();

    QString msg=QString("¿Está seguro que desea parar\nEquipo %1 (%2)?").arg(indice+1).arg(memoryMap->m_mapa.m_maquina.m_fluidica.Descripcion.at(indice));

    int respuesta;
    respuesta=QMessageBox::question(this,"ATENCION",msg,"Si","No",QString::null, 0, 1 );
    if (respuesta==0){
        if (memoryMap->m_mapa.m_maquina.m_fluidica.Estado==1){
            error=XCL_Send_Command(memoryMap->m_mapa.m_maquina.m_fluidica.ID[indice],XCL_CMD_Stop);
        }
    }
}


void NivelTintas::ShutdownHydra(){

    unsigned char indice=ui->m_Equipo->currentIndex();

    QString msg=QString("¿Está seguro que desea apagar\nEquipo %1 (%2)?").arg(indice+1).arg(memoryMap->m_mapa.m_maquina.m_fluidica.Descripcion.at(indice));

    int respuesta;
    respuesta=QMessageBox::question(this,"ATENCION",msg,"Si","No",QString::null, 0, 1 );
    if (respuesta==0){
        if (memoryMap->m_mapa.m_maquina.m_fluidica.Estado==1){
            error=XCL_Send_Command(memoryMap->m_mapa.m_maquina.m_fluidica.ID[indice],XCL_CMD_Shutdown);
        }
    }
}

/** Cargamos datos comunes*/
void NivelTintas::SetDataCommon(){
    struct stCommon dummyDataCommon;
    dummyDataCommon.fecha=this->ui->fecha;
    //dummyDataCommon.hora=this->ui->hora;
    dummyDataCommon.led_EstadoLampara=this->ui->led_EstadoLampara;
    dummyDataCommon.led_EstadoMaquina=this->ui->led_EstadoMaquina;
    dummyDataCommon.led_EstadoTemperatura=this->ui->led_EstadoTemperatura;
    dummyDataCommon.m_Imagen=NULL;
    dummyDataCommon.m_NombreFicheroConfig=this->ui->m_NombreFicheroConfig;
    dummyDataCommon.m_RutaFicheroConfig=this->ui->m_RutaFicheroConfig;
    dummyDataCommon.m_TextoStatus=this->ui->m_TextoStatus;
    dummyDataCommon.m_User=this->ui->m_User;
    dummyDataCommon.usuario_actual=this->ui->usuario_actual;
    dummyDataCommon.m_MostrarInicio=this->ui->m_MostrarInicio;
    dummyDataCommon.m_MostrarAyuda=this->ui->m_MostrarAyuda;
    dummyDataCommon.m_MostrarMantenimiento=this->ui->m_MostrarMantenimiento;
    dummyDataCommon.lbl_EstadoLampara=ui->lblLamp;
    dummyDataCommon.lbl_EstadoMaquina=ui->lblMaquina;
    dummyDataCommon.lbl_EstadoTemperatura=ui->lblFluid;

    DataCommon=new InkjetCommon(this,dummyDataCommon);

//Actualizamos
    DataCommon->ShowInfo();
    DataCommon->ShowEstadoImpresora();
    DataCommon->ShowComms();

}

