/** @addtogroup Maquina
/** @addtogroup Maquina
 *  @{*/
/** @file Maquina.h
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright In2 Printing Solutions. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Pantalla de parametros de impresion
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: Maquina.h
 *    @li Versión: x.0
 *    @li Fecha: 05/05/08
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */

#ifndef MAQUINA_H
#define MAQUINA_H

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
#include "ui_MaquinaUI.h"
#include "inkjetcommon.h"
#include "inkjetxusbctrl.h"
#include "inkjetxusbvarctrl.h"
#include "imagevarxaarsender.h"
#include "imagevarxaarcreator.h"
#include <QVector>



class Maquina : /*public InkjetForm*/ public QWidget
{
	Q_OBJECT

public:
	//Constructor
	Maquina(QWidget *parent);
	//Destructor
	~Maquina();

private:
    void InitFixFrame();
    void InitVariableFrame();


private slots:

    void AceptarValorVelocidad();
    void AceptarVelocidad(QString value);

    void AceptarValorMetros();
    void AceptarMetros(QString value);

    void AceptarValorDiametro();
    void AceptarDiametro(QString value);

    void AceptarValorDiametroParo();
    void AceptarDiametroParo(QString value);

    void AceptarValorDiametroSalida();
    void AceptarDiametroSalida(QString value);

    void CambiarVelocidad();
    void CambiarMetros();
    void CambiarTrabajo();
    void CambiarDiametro();
    void CambiarDiametroSalida();
    void CambiarDiametroParo();

	//Acepta la seleccion de parametros
	void AceptarSeleccion();
	//Cancela la selección de parámetros
	void CancelarSeleccion();

	void ActualizaDataSlot();
	void ActualizaDataSlotOPC();

	// Si no es root, bloqueo funcionalidad.
	void TestRoot();

	void LaunchPrincipal();
    void LaunchColaImpresion();

    void ShowCam();

    void EjecutarJogMasOn();
    void EjecutarJogMenosOn();
    void EjecutarJogMasOff();
    void EjecutarJogMenosOff();
    void EjecutarBuscarMacula();
    void EjecutarEmpalme();
    void EjecutarBobinaNueva();

    // Genera el PD
    void GeneratePD();
    // Lanza el PD en multihilo si han acabado todos los threads
    void sltLaunchPD();
    // Maneja los errores
    void sltErrorString(QString,int);
    //Inicializacion datos comunes
    void setDataCommon();

    QImage createSubImage(QImage* image, const QRect & rect);
    //Pide infor al xaar
    int AskXaar();
    //Hace el soft stop
    void SoftStop();
    //Se ha hecho un external soft stop
    void ExternalSoftStop();
    //Termina hilos
    void StopThreads();
    //Pregunta por la impresion
    void AskXaarPrintState();
    //Lanza maquina 2
    void LaunchMaquina2();
    //Actualiza el freno segun el Ancho
    void changeFreno(int i);
    //Actualiza el freno de manera automatica, segun el ini
    void SetFuerzaFrenoAuto();

public :
    struct In2VarImage BufferVarImages[SIZE_BUFFER_IMAGES];

public slots:
    void CargaDatos();
    bool SelImagen();
    void SelImagenFija();
    void SendToXaar();
    void SendToVarXaar();
    void SendIniToXaar();
    void StopPrinting();
    void MuestraImagen(QPixmap Imagen);
    void UpdateTotalVarXarSentSwathe();
    void UpdateVarXarSentSwathe(int last);
    void ForceRefreshValues();
    void CambiarVolumenGota1(int index);
    void CambiarVolumenGota2(int index);
private:
    Ui::MaquinaUI *ui;
    InkjetCommon *DataCommon;
	//Temporizador de eventos
	QTimer *m_Temporizador;
    InkjetXusbCtrl *newCard;
    //InkjetXusbCtrl *newCard2;
    //Contador de threads
    int m_threads;
    //Contador de XUSB
    int m_iTotalXUSB;
    //Nuestro controlador de xusb
    InkjetXusbVarCtrl *newVarCard;
    //Sender de imagenes
    ImageVarXaarSender *ImageSender;
    //Creator de imagenes
    ImageVarXaarCreator *ImageCreator;
    //Swathes totales enviadas
    int m_TotalSwatheSent;
    //Swathe actual
    int m_ActualSwathe;
    QVector<int> m_xUSBWorking;
    QThread *threadSender;
    QThread *threadCreator;


signals:
	void DataChange(int status);
    void CurrentPrintSwathe(int swathe);
    void StartGenerating();
    void PauseGenerating();
    void StopThread();
    void SendDebugInfo(unsigned char code, QString msg);

};

#endif // MAQUINA_H
