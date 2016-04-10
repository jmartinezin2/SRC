
#ifndef PRINCIPAL_H
#define PRINCIPAL_H

//#include "GestorDB_ODBC.h"
#include "GestorDB_SQLITE.h"
#include "comthread.h"
#include "inkjetform.h"
#include <QTimer>
#include "status.h"
#include "clienteopc.h"
#include "globals.h"
#include "ui_PrincipalSPassUI.h"
#include "inkjetcommon.h"
#include "gestorethernet.h"
#include "inkjetdebug.h"
#include "in2logon.h"
#include "inkjetinformer.h"
#include "inkjetcommonext.h"
#include "In2gestormaquina.h"
#include "in2gestorfluidica.h"
#include "In2MachineInfo.h"
#include "in2saicontrol.h"
#include "in2gestorerp.h"
#include "in2configuratorerp.h"
#include "in2esteveerp.h"


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
    //ClienteOPC* miCliente;

signals:
	void ActualizaData(); //Señal para actualiza datos
	void ActualizaDataOPC(); //Señal para actualiza datos del OPC
    void SendDebugInfo(unsigned char code, QString msg);
    void SendInformerInfo(unsigned char code, QString msg);
    void Refresh(); //Para refrescar datos
    void PrintStatusChange(); //Ha cambiado el status de impresion
    void sgnChangeState(int theme);
    void sglLeerReferenciaVelocidad();
    void sglHabilitarCondicionesMovimiento(unsigned char version);
    void sglDesconectarServidorOPC();
    void sgnKillMsg();

private:
    void setDataCommon();
    void LaunchSWWatchdog();
    bool ComprobacionFicherosAplicacion();
public slots:	
    void init();
	//Lanza la pantalla de configuracion
	void LaunchGestion();
	//Lanza la pantalla de maquina
	void LaunchMaquina();
	//Lanza la pantalla de fluidica
    void LaunchFluidica();
    //Lanza la pantalla de chiller
    void sltLaunchChiller();
	//Lanza la pantalla de parametros de impresion
	void LaunchComposicion();
    //Lanza la pantalla de seleccionar y enviar imagen
    //void LaunchParImpresion();
	//Muestra la imagen central
	void MuestraImagenPrincipal(QPixmap Imagen);
    void ReceiveDebugInfo(unsigned char codigo, QString mensaje);
    void EvDataExt1(int& numItems, QVariantList& ClientHandles, QVariantList& ItemValues, QVariantList& Qualities);
    void ReceiveInformerInfo(unsigned char codigo, QString mensaje);
    //Enseña la ventana de log
    void ShowLogScreen();
    //Enseña la ventana de log
    void ForcedShowLogScreen();


private slots:
    void sltLaunchStatus();
    void sltInit0();
    void UsuarioIntroducido();
    void TestRoot();
    void InitMachine();
    void TestLDAP();
    //void sltTestSai();
    void sltHydraError(int error,int equipo, float value);
//Salimos
	void Exit();
    void AutomatedExit();
    void sltFullShutdown();
    void sltApagarSistema();
    void sltApagarSistema1();
    void LaunchLog(QString txt);
    void sltUpdateTableHistorico();
    void sltUpdateDiametros();
    void sltUpdateTableMaquina();
    void sltUpdateTableImpresion();
    void sltUpdateTableImpresion1();
    void sltUpdateTableFluidica();
    void sltUpdateTableChiller();
    void sltFlancoOPC(int signal);
    void sltOPCInitiated();
    void sltCheckCredentials();
    void sltChillerAlarma1(int x);
    void sltChillerAlarma2(int x);
    void sltChillerAlarma3(int x);
    void sltChillerTemperatura(int x);
    void sltChillerPresion(int x);
    void sltChillerSetpoint(int x);
    void sltChillerStatus(int x);
    void sltChillerCom(int x);
    void sltSaiActive();
    void _sltTestSound();
    //De prueba
    void sltLaunchVision();


/*protected:
    void resizeEvent(QResizeEvent *event);*/

private:
	//Logo
	QPixmap Logo;
    //Datos Comunes
    InkjetCommonExt *dataCommon;
    //Gestor de comunicaciones Ethernet
    GestorEthernet *m_GestorEthernet;
    //Informacion de debug
    InkjetDebug *m_InfoDebug;
    //Informacion de errores
    InkjetInformer *m_InfoInformer;
    //numeo de reintentos login
    unsigned char loginCounter;
    //Gestor de máquina
    In2GestorMaquina* m_In2GestorMaquina;
    //Gestor de fluidica
    In2GestorFluidica *m_FluidGestor;
    //Info de maquina
    //In2MachineInfo *m_MachineInfo;
    In2SaiControl *m_gestorSai;
    MemoryMap *m_memoryMap;

private:
    Ui::PrincipalSPassUI *ui;

protected:
    void showEvent ( QShowEvent * event );
};

#endif // PRINCIPAL_H
