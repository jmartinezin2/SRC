#ifndef GESTORMAQUINA_H
#define GESTORMAQUINA_H

#include <QObject>
#include <QThread>
#include "gestorimpresion.h"
#include "monitordiametro.h"
#include "in2managerdiametros.h"
#include "clienteopc.h"

class GestorMaquina : public QObject
{
    Q_OBJECT
public:
    static GestorMaquina *request(QObject *parent);
    bool isPrinting();

 
private:
    GestorMaquina(QObject *parent = 0);
    static GestorMaquina *mInstance;
    ClienteOPC* m_ClienteOPC;
    GestorImpresion *m_GestorImpresion;
    int m_version;
    In2ManagerDiametros *m_MonDiametro;
    QMap<int,int> RefV_Intensidad;
    QMap<int,int> RefV_Intensidad_2C;
    MemoryMap *m_memoryMap;
    int m_ContadorArranque;

private:
    void ReloadDiametroSalida();
    void ReloadDiametroEntrada();
    void TestDiametroMinimoVelocidad();
    void Leer_Tabla_RefVelocidad_Intensidad();
    int Relacion_VelocidadIntensidadLampara(int Velocidad);

signals:
    void sgnSoftStop();
    void SendDebugInfo(unsigned char level,QString data);
	void sglIniciarImpresionDone();
	void sglFinalizarImpresionDone();
	void sglEncenderXUSBDone();
	void sglApagarXUSBDone();
	void sglCambioBobinaEntradaDone();
	void sglCambioBobinaSalidaDone();
	void sglBuscarMaculaDone();
	void sglHabilitarFrenoDone();
	void sglDeshabilitarFrenoDone();
	void sglJogMasOnDone();
	void sglJogMasOffDone();
	void sglJogMenosOnDone();
	void sglJogMenosOffDone();

    void sglGetReferenciaVelocidadRtn(signed short refVelocidad);
    void sglGetTiempoBusquedaMaculaRtn(int valor);
    void sglGetVelocidadBusquedaMaculaRtn(int valor);
    void sglGetVelocidadAjusteMaculaRtn(int valor);
    void sglGetVelocidadJogMasRtn(int valor);
    void sglGetVelocidadJogMenosRtn(int valor);
    void sglGetFuerzaFrenoRtn(int valor);
    void sglGetMetrosRtn(int valor);
    void sglGetDiametroEntradaRtn(int valor);
    void sglGetDiametroSalidaRtn(int valor);
    void sglGetDiametroParoRtn(int valor);
    void sglGetIntensidadLamparaRtn(int valor);
    void sglGetTensionBobinaRtn(int valor);
    void sglGetRampaAceleracionRtn(int valor);
    void sglGetRampaDeceleracionRtn(int valor);
    void sglGetDistanciaEmpalmeRtn(int valor);
    void sglGetLamparaRtn(bool valor);
    void sglGetFrenoRtn(bool valor);
    void sgnKillIn2Message();
    void sgnUpdateIn2MessageBar(int);
    void sgnUpdateIn2Message(QString);
    void setAutoIn2AutoPB(int);
    void sgnGesMaquinaMessage(const QString & txt);
    void sgnGesMaquinaFinEmpalmeEntrada();
    void sgnGesMaquinaFinEmpalmeSalida();
    void sgnGestorMaquinaFinImpresion();


private slots:
    bool InitGestor();
    //void sltSendDebugInfo(unsigned char level,QString data);
	void iniciarImpresionDone();
	void finalizarImpresionDone();
	void encenderXUSBDone();
	void apagarXUSBDone();
	void cambioBobinaEntradaDone();
	void cambioBobinaSalidaDone();
	void buscarMaculaDone();
	void habilitarFrenoDone();
	void deshabilitarFrenoDone();
	void jogMasOnDone();
	void jogMasOffDone();
	void jogMenosOnDone();
	void jogMenosOffDone();
    void sltWenglorDiameter(double x);
    void sltWenglorOutputDiameter(double x);
    void sltMonitor();
    void sltFiltrarDiametroEntrada();
    void sltAjustarIntensidadLamparaDinamico();
    bool sltTestVelocidadInicial();
    void sltMonitorSpeed();


	
public slots:
    void initImpresion();
    void initImpresion0();
    void initImpresion1();
    void sltForceUpdateVelocidadAndLampara();
    //void initImpresion2();
    void sltExecFullSoftStop();
    void sltEnableCondicionesMovimiento();
    void sltDisableParoMovimiento();
    void sltDisableMarchaMovimiento();
    void sltEnableMarchaMovimiento();
    void sltCheckSoftStopSpeed();
    void sltImpresionFinalizada();
    void sltDecelerarMaquina();
    void sltPararMaquina();
    void sltParoPorMetrosImpresos();
    void sltParoPorEmpalme();
    void sltParoPorErrorEncoder();
    void sltParoPorDiametroEntrada();
    void sltHabilitarFreno(bool x);
    void sltHabilitarCalculoDiametro(int x);
    void sltEnableCalculoDiametro();
    void sltSetFilteredDiameter(int x);
    void sltSetOutputFilteredDiameter(int x);
    void sltRefreshInfo();
	void sltIniciarImpresion();
	void sltFinalizarImpresion();
	void sltEncenderXUSB();
	void sltApagarXUSB();
	void sltCambioBobinaEntrada();
	void sltCambioBobinaSalida();
	void sltBuscarMacula();
    void sltSetTiempoBusquedaMacula(QVariant valor);
    void sltSetVelocidadBusquedaMacula(QVariant valor);
    void sltSetVelocidadAjusteMacula(QVariant valor);
    void sltJogMasOn(bool conLampara);
	void sltJogMasOff();
    void sltJogMenosOn(bool conLampara);
	void sltJogMenosOff();
    void sltTensadoAutomatico();
    void sltTensarMasOn();
    void sltTensarMasOff();
    void sltDestensarMasOn();
    void sltDestensarMasOff();
    void sltSetImpresionReverse(bool reverse);
    void sltSetVelocidadJogMas(QVariant valor);
    void sltSetVelocidadJogMenos(QVariant valor);
    //void sltHabilitarCondicionesMovimiento();
    void sltDesconectarServidorOPC();
    void sltSetFuerzaFreno(QVariant valor);
    void sltSetMetros(QVariant valor);
    void sltSetDiametroEntrada(QVariant valor);
    void sltSetDiametroSalida(QVariant valor);
    void sltSetDiametroParo(QVariant valor);
    void sltTrabajoNuevo();

    void sltSetIntensidadLampara(QVariant valor);
    void sltSetTensionBobina(QVariant valor);
    void sltSetRampaAceleracion(QVariant valor);
    void sltSetRampaDeceleracion(QVariant valor);
    void sltSetDistanciaEmpalme(QVariant valor);
    void sltFrenoOn();
    void sltFrenoOff();
    void sltCheckMaxBobina();
    void sltLamparaOn();
    void sltLamparaOff();


    signed short sltGetReferenciaVelocidad();
    void sltSetReferenciaVelocidad(int value);
    void sltSetReferenciaVelocidadControlled();
    void sltControlReferenciaVelocidad();
    int sltGetTiempoBusquedaMacula();
    int sltGetVelocidadBusquedaMacula();
    int sltGetVelocidadAjusteMacula();
    int sltGetVelocidadJogMas();
    int sltGetVelocidadJogMenos();
    int sltGetFuerzaFreno();
    int sltGetMetros();
    int sltGetDiametroEntrada();
    int sltGetDiametroSalida();
    int sltGetDiametroParo();
    int sltGetIntensidadLampara();
    int sltGetTensionBobina();
    int sltGetRampaAceleracion();
    int sltGetRampaDeceleracion();
    int sltGetDistanciaEmpalme();
    bool sltGetLampara();
    bool sltGetFreno();

    void sltEjecutarEmpalme();
    void sltEjecutarEmpalme_1();
    void sltEjecutarEmpalme_2();
    void sltEjecutarEmpalme_3();
    void sltEjecutarEmpalme_4();
    void sltEjecutarEmpalme_5();
    void sltEjecutarEmpalme_6();

    void sltCentrarBalluf();
    void sltCentrarBalluf1();
    bool sltBallufCentrado();
    void sltSetCondicionesIniciales();


};

#endif // GESTORMAQUINA_H












	
	

