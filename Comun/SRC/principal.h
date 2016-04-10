/** @addtogroup InkjetForm
 *  @{*/
/** @file Principal.h
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright In2 Printing Solutions. Línea Tecnológica: Diseño Electrónico
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

#ifndef PRINCIPAL_H
#define PRINCIPAL_H

#include "GestorDB_ODBC.h"
#include "GestorDB_SQLITE.h"
//#include "comthread.h"
#include "inkjetform.h"
#include <QTimer>
#include "status.h"
#include "clienteopc.h"
#include "globals.h"
#include "ui_principalui.h"
#include "inkjetcommon.h"
#include "gestorethernet.h"

/*namespace Ui {
    class PrincipalUI;
}*/


class Principal : /*public InkjetForm*/ public QWidget
{
	Q_OBJECT

public:
    explicit Principal(QWidget *parent=0);
	~Principal();
    //ComThread *ThreadMaquina;
    ClienteOPC* miCliente;

signals:
	void ActualizaData(); //Señal para actualiza datos
	void ActualizaDataOPC(); //Señal para actualiza datos del OPC
    void SendDebugInfo(unsigned char code, QString msg);
    void FullStopPerformed(); //Se ha ejecutado un fullstop
    void Refresh(); //Para refrescar datos

private:
    void setDataCommon();
    void LaunchSWWatchdog();
public slots:	
	//Lanza la pantalla de configuracion
	void LaunchGestion();
	//Lanza la pantalla de maquina
	void LaunchMaquina();
	//Lanza la pantalla de fluidica
	void LaunchFluidica();
	//Lanza la pantalla de parametros de impresion
	void LaunchComposicion();
    //Lanza la pantalla de seleccionar y enviar imagen
    void LaunchParImpresion();
	//Muestra la imagen central
	void MuestraImagenPrincipal(QPixmap Imagen);
    //Conecta con XUSB
    int ConectarConXUSB();
    void EvData(int&, QVariantList&, QVariantList&, QVariantList&);
    void EvDataExt1(int&, QVariantList&, QVariantList&, QVariantList&);
	void EjecutarSoftStop(int Card);
    void EjecutarFullSoftStop();
    void EjecutarStop(int Card);
    void EnableHeads(int Card);
    void ReceiveDebugInfo(unsigned char codigo, QString mensaje);
    //Enseña la ventana de log
    void ShowLogScreen();
    //Cambia el orden de los XUSB
    void ChangeXUSBOrder();
    void ChangeXUSBOrderAuto();
    void AjustarIntensidadLampara(unsigned char tipo_Movimiento);
    void AjustarAceleracion(unsigned char tipo_Movimiento);
    void AjustarDeceleracion(unsigned char tipo_Movimiento);
    void Asignacion_Referencia_Velocidad();
    void Control_Referencia_Velocidad();
    void AjustarTiempoBusquedaMacula(unsigned char tipo_Movimiento);
    bool ComprobacionesPreviasImpresion(void);
    bool ComprobacionFicherosAplicacion();
    void ControlVelocidadArranque();
    void ReloadDiametroEntrada();
    void ReloadDiametroSalida();
    void ControlHydra();
    void MonitorTemperaturaLampara();
    void SetNuevaVelocidadReferencia(int valor);
    void SecuenciaEmpalme_1();
    void SecuenciaEmpalme_2();
    void SecuenciaEmpalme_3();
    void SecuenciaEmpalme_4();
    void SecuenciaEmpalme_5();
    void SecuenciaEmpalme_6();
    void SecuenciaEmpalme_7();
    void SecuenciaBobinaNueva_1();
    void SecuenciaBobinaNueva_2();
    void SecuenciaBobinaNueva_3();
    void SecuenciaBobinaNueva_4();
    void SecuenciaBobinaNueva_5();
    void TestDiametroMinimoVelocidad();
    bool TestSerialXusb(int pos);

private slots:
//Actualiza parametros de temporización
	void UpdateTemporizador();
	void ActualizaDataSlot();
    void UsuarioIntroducido();
    void TestRoot();

//Salimos
	void Exit();


private:
	//Timer de eventos
	QTimer *m_Temporizador;
	//Logo
	QPixmap Logo;
    //Datos Comunes
    InkjetCommon *dataCommon;
    //Numero de XUSB
    int m_NumXUsb;
    unsigned char flag_usuario_logeado;
    //Gestor de comunicaciones Ethernet
    GestorEthernet *m_GestorEthernet;

private:
    Ui::PrincipalUI *ui;

protected:
};

extern char isRoot;
extern QString InkjetPath;

#endif // PRINCIPAL_H
