/** @addtogroup MaquinaAdv
 *  @{*/
/** @file MaquinaAdv.h
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright In2 Printing Solutions. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Pantalla de máquina avanzada
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: MaquinaAdv.h
 *    @li Versión: x.0
 *    @li Fecha: 05/05/08
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */

#ifndef MAQUINAADV_H
#define MAQUINAADV_H

#include "InkjetForm.h"
#include "MemoryMap.h"
#include <QComboBox>
#include <QPushButton>
#include <QTextEdit>
#include <QCheckBox>
#include <QTimer>
#include <QButtonGroup>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QFontComboBox>
#include <QAxObject>
#include <QMouseEvent>
#include <QRubberBand>
#include "ui_MaquinaAdvUI.h"
#include "inkjetcommon.h"
#include <QVector>
#include "gestormaquina.h"


class MaquinaAdv : /*public InkjetForm*/ public QWidget
{
	Q_OBJECT

public:
	//Constructor
    MaquinaAdv(QWidget *parent);
	//Destructor
    ~MaquinaAdv();


private slots:

    void AceptarValorTiempoBusqueda();
    void AceptarTiempoBusqueda(QString value);

    void AceptarValorVelocidadBusqueda();
    void AceptarVelocidadBusqueda(QString value);

    void AceptarValorVelocidadAjuste();
    void AceptarVelocidadAjuste(QString value);

    void AceptarValorVelocidadJogMas();
    void AceptarVelocidadJogMas(QString value);

    void AceptarValorVelocidadJogMenos();
    void AceptarVelocidadJogMenos(QString value);

    void AceptarValorFreno();
    void AceptarFreno(QString value);

    void AceptarValorIntensidadLampara();
    void AceptarIntensidadLampara(QString value);

    void AceptarValorTensionBobina();
    void AceptarTensionBobina(QString value);

    void AceptarValorRampaAceleracion();
    void AceptarRampaAceleracion(QString value);

    void AceptarValorRampaDeceleracion();
    void AceptarRampaDeceleracion(QString value);

    void AceptarValorDistanciaEmpalme();
    void AceptarDistanciaEmpalme(QString value);

    void CambiarTrabajoAdv();
    void CambiarTiempoBusqueda();
    void CambiarVelocidadBusqueda();
    void CambiarVelocidadAjuste();
    void CambiarVelocidadJogMas();
    void CambiarVelocidadJogMenos();
    void CambiarFuerzaFreno();
    void CambiarIntensidadLampara();
    void CambiarTensionBobina();
    void CambiarRampaAceleracion();
    void CambiarRampaDeceleracion();
    void CambiarDistanciaEmpalme();

	//Acepta la seleccion de parametros
	void AceptarSeleccion();
	//Cancela la selección de parámetros
	void CancelarSeleccion();


	void ActualizaDataSlotOPC();


	void LaunchPrincipal();
    void LaunchMaquina();

    void ShowCam();

    void EjecutarJogMasOn();
    void EjecutarJogMenosOn();
    void EjecutarJogMasOff();
    void EjecutarJogMenosOff();
    void EjecutarFrenoOn();
    void EjecutarFrenoOff();
    void EjecutarBuscarMacula();
    void EjecutarEmpalme();
    void EjecutarBobinaNueva();
    void EjecutarEncenderLampara();
    void EjecutarApagarLampara();
    void EjecutarLampOnJogMas();
    void EjecutarLampOnJogMenos();

    void showCompileVersion();
    void sltUpdateReinicio(int x);
    void sltCheckSerials();
    void sltReiniciarSistema();
    void sltActualizaLedsDesbobinador(int value);
    void sltActualizaLedsBobinador(int value);


private:
    Ui::MaquinaAdvUI *ui;    
    bool freno_on;
    bool lamp_on;
    MemoryMap *m_memoryMap;
	//Temporizador de eventos
signals:
	void DataChange(int status);
    void SendDebugInfo(unsigned char code, QString msg);

    void sglSetTiempoBusquedaMacula(QVariant valor);
    void sglSetVelocidadBusquedaMacula(QVariant valor);
    void sglSetVelocidadAjusteMacula(QVariant valor);
    void sglSetVelocidadJogMas(QVariant valor);
    void sglSetVelocidadJogMenos(QVariant valor);
    void sglSetFuerzaFreno(QVariant valor);
    void sglSetIntensidadLampara(QVariant valor);
    void sglSetTensionBobina(QVariant valor);
    void sglSetRampaAceleracion(QVariant valor);
    void sglSetRampaDeceleracion(QVariant valor);
    void sglTrabajoNuevo();
    void sglSetDistanciaEmpalme(QVariant valor);
    void sglJogMasOn(bool conLampara);
    void sglJogMasOff();
    void sglJogMenosOn(bool conLampara);
    void sglJogMenosOff();
    void sglFrenoOn();
    void sglFrenoOff();
    void sglBuscarMacula();
    void sglEjecutarEmpalme();
    void sglBobinaNueva();
    void sglLamparaOn();
    void sglLamparaOff();

    void sglGetTiempoBusquedaMacula();
    void sglGetVelocidadBusquedaMacula();
    void sglGetVelocidadAjusteMacula();
    void sglGetVelocidadJogMas();
    void sglGetVelocidadJogMenos();
    void sglGetFuerzaFreno();
    void sglGetMetros();
    void sglGetDiametroEntrada();
    void sglGetDiametroSalida();
    void sglGetDiametroParo();
    void sglGetIntensidadLampara();
    void sglGetTensionBobina();
    void sglGetRampaAceleracion();
    void sglGetRampaDeceleracion();
    void sglGetDistanciaEmpalme();
    void sglGetLampara();
    void sglGetFreno();
};

#endif // MAQUINAADV_H
