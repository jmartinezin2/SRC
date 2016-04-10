/** @addtogroup Utilidades
 *  @{*/
/** @file Utilidades.cpp
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright Cemitec. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Dialogo de Utilidades de la aplicacion inkjet
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: Utilidades.cpp
 *    @li Versión: x.0
 *    @li Fecha: 01/04/08
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */

#include "utilidades.h"
#include "configuracion.h"
#include "inkjetcommon.h"
#include "inkjetform.h"
#include "inputdata.h"
#include "../../InkConsumeCalculator/inkConsumeCalculator.h"
#include <QMessageBox>


//Constructor
Utilidades::Utilidades(QWidget *parent)
    : ui(new Ui::UtilidadesUI)
{

    ui->setupUi(this);
    GlobalSetScreenSize(this);

    setDataCommon();

    QObject::connect(ui->m_PrincipalButton, SIGNAL(clicked()),this, SLOT(LaunchConfiguracion()));
    QObject::connect(ui->m_InkCalculator, SIGNAL(clicked()),this, SLOT(LaunchInkCalculator()));
    connect(ui->m_ButtonShowLog,SIGNAL(clicked()),GlobalGetMainWindow(),SLOT(ShowLogScreen()));
    QObject::connect(GlobalGetMainWindow(), SIGNAL(ActualizaData()),this, SLOT(ActualizaDataSlot()));

    TestRoot();

}
//Destructor
Utilidades::~Utilidades()
{
}

void Utilidades::TestRoot(){
    if (memoryMap->getUserRights()<ACCESS_PRINTER){

    }
}

// Actualiza la info
void Utilidades::ActualizaDataSlot(){
    dataCommon->ShowComms();
    dataCommon->ShowEstadoImpresora();
}


//Lanza la pantalla principal
void Utilidades::LaunchConfiguracion(){
    Configuracion *ScrConfiguracion;
    ScrConfiguracion=new Configuracion(0);
    ScrConfiguracion->show();
    this->deleteLater();
}

//Lanza la pantalla de Utilidades de usuario
void Utilidades::LaunchInkCalculator(){
    inkConsumeCalculator *ScrinkConsumeCalculator;
    ScrinkConsumeCalculator=new inkConsumeCalculator(0);
    ScrinkConsumeCalculator->show();
    //this->deleteLater();
}



/** Cargamos datos comunes*/
void Utilidades::setDataCommon(){
    struct stCommon dummyDataCommon;
    dummyDataCommon.fecha=this->ui->fecha;
    //dummyDataCommon.hora=this->ui->hora;
    dummyDataCommon.led_EstadoLampara=this->ui->led_EstadoLampara;
    dummyDataCommon.led_EstadoMaquina=this->ui->led_EstadoMaquina;
    dummyDataCommon.led_EstadoTemperatura=this->ui->led_EstadoTemperatura;
    dummyDataCommon.m_NombreFicheroConfig=this->ui->m_NombreFicheroConfig;
    dummyDataCommon.m_RutaFicheroConfig=this->ui->m_RutaFicheroConfig;
    dummyDataCommon.m_TextoStatus=this->ui->m_TextoStatus;
    dummyDataCommon.m_User=this->ui->m_User;
    dummyDataCommon.usuario_actual=this->ui->usuario_actual;
    dummyDataCommon.m_MostrarInicio=this->ui->m_MostrarInicio;
    dummyDataCommon.m_MostrarAyuda=this->ui->m_MostrarAyuda;
    dummyDataCommon.m_MostrarMantenimiento=this->ui->m_MostrarMantenimiento;
    dummyDataCommon.lbl_EstadoLampara=this->ui->lblLamp;
    dummyDataCommon.lbl_EstadoMaquina=this->ui->lblMaquina;
    dummyDataCommon.lbl_EstadoTemperatura=this->ui->lblFluid;
    dataCommon=new InkjetCommon(this,dummyDataCommon);

//Actualizamos
    dataCommon->ShowInfo();
    dataCommon->ShowEstadoImpresora();
    dataCommon->ShowComms();

}
