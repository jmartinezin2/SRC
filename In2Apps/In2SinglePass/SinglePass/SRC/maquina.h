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
#include "ui_MaquinaSPassUI.h"
#include "inkjetcommon.h"
#include "inkjetxusbctrl.h"
#include "inkjetxusbvarctrl.h"
#include "imagevarxaarsender.h"
#include "imagevarxaarcreator.h"
#include "in2bufferimages.h"
#include "inkjetcommonext.h"
#include "gestormaquina.h"
#include <QVector>
#include "in2graphicsscene.h"
#include "in2graphicsview.h"


class Maquina : /*public InkjetForm*/ public QMainWindow
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
    bool TestXusbToClean(int Card);
    int CalculateXaarBufferSize();
    QByteArray getPaleta(int index);
    QByteArray getGrayScalePaleta(int index);


private slots:
    void sltFinImpresion(int mode);
    void MuestraImagenCentral();
    void MuestraImagenColor1();
    void MuestraImagenColor2();
    void sltChangeUser();
    void sltLaunchFluidica();
    void sltLaunchChiller();
    void sltLaunchPrintConfig();
    void sltMonitoringData();
    void sltUpdateTipoImpresion();
    void sltPaletteItemChanged(int row,int column);
    void sltShowLogScreen();
    void updateVelocidadActual();
    void updateMetrosImpresos();
    void updateMaculaMax();
    void updateMaculaMin();
    void updateDBobinaEntrada();
    void updateDBobinaSalida();
    void updateTemperaturaLampara();
    void updateIntensidadLampara();
    void updateImpresiones();
    void updateTemperaturaChiller(int equipo);
    void updateSetPointChiller(int equipo);
    void updateAlarmaChiller(int equipo);
    void updatePresionChiller(int equipo);
    void updateCOMChiller(int value);
    void sltUpdateReferencias();
    void sltSetImpresionReverse(bool reverse);
    void sltSetImpresionNormal(bool reverse);
    void sltAutoTensionado();
    void sltSendIni0();
    void sltSendIni1();
    void sltSendIni2();
    void sltFlancoOPC(int ID);
    void sltUpdateInfoImagen();
    void sltChangeModoImpresion(int modo);
    void sltChangeTipoPD(int modo);
    void sltLoadFileColor1();
    void sltDeleteFileColor1();
    void sltLoadFileColor2();
    void sltDeleteFileColor2();
    void sltLoadFileColor3();
    void sltDeleteFileColor3();
    void sltLoadFileColor4();
    void sltDeleteFileColor4();
    void sltChangeOffsetPoints();
    void sltChangeOffsetMm();
    void updateBalluf();
    void sltUpdateMessage(const QString & txt);
    void sltChangeState(int state);
    void sltFinTrabajo();
    void sltNoVisionComm();
    void sltVisionSocketFail();
    void sltVisionSocketConnected();


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

    void AceptarValorBobina();
    void AceptarBobina(QString value);

    void TestValorCBBobina();
    void TestCBBobina(QString value);

	//Acepta la seleccion de parametros
	void AceptarSeleccion();
	//Cancela la selección de parámetros
	void CancelarSeleccion();

	// Si no es root, bloqueo funcionalidad.
	void TestRoot();
    void _MuestraPermisosIn2();
    void _MuestraPermisosEsteve();
    void _MuestraPermisosGramar();

	void LaunchPrincipal();
    void LaunchColaImpresion();

    void ShowCam();

    void EjecutarJogMasOn();
    void EjecutarJogMenosOn();
    void EjecutarJogMasOff();
    void EjecutarJogMenosOff();

    void EjecutarTensionMasOn();
    void EjecutarTensionMenosOn();
    void EjecutarTensionMasOff();
    void EjecutarTensionMenosOff();

    void EjecutarBuscarMacula();
    void EjecutarEmpalme();
    void CambiarTrabajo();

    // Genera el PD
    void GeneratePD();
    // Lanza el PD en multihilo si han acabado todos los threads
    void sltLaunchPD();
    // Maneja los errores
    void sltErrorString(QString,int);
    //Cambia el estado de impresion
    void sltPrintStatusChange(bool monitor);
    //Cambia un parametro
    void sltChangeParameter(int row, int column);
    //Entra en modo inspeccion
    void sltInspeccion();
    //Restaura la velocidad
    void sltRestorePrevSpeed();

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
    //Actualiza el freno de manera automatica, segun el ini
    void SetFuerzaFrenoAuto();
    //Numero de impresiones a realizar
    void SetPrintCount();
    //Pregunta por donde va
    void AskPrintCount();

    void sltSimulateMaculas();
    void UpdateScreenStreamingMode();

    void PreCreateImagesToXaar();
    void PreLoadImagesToXaar();
    void sltPreCreateImagesToXaarSync();
    void sltPreLoadImagesToXaarSync();

    void LaunchImpresionVariable();
    void sltVariableDataSoftStop();
    void sltVariableDataEmergencyStop();
    void CambiarFreno(QString txt);

    void sltLoadFromCola(bool val);
    void sltCheckFinImpresion();
    void sltErrorMacula(int type);
    void sltCleanMessage();
    void sltEmpalmeERP();
    void sltEmpalmeERP1();
    void sltUpdateImagenVision();
    void sltShowFoto(const QPixmap & dib);
    void sltSetRefMaculas();
    void sltSetRefMaculasLimits();
    void sltConfigVision();
    void sltChangeToVision(QString txt);
    void sltMessageFromVision(int val, int command);
    void sltShowCompileInfo();
    void sltEndERPWork();
    void sltEndERPWorkMode(int mode);
    void sltChangeCentralTab(int index);
    void sltLoadVisionProgram(QString txt);
    void sltResetWork();
    bool _checkTrabajoImpreso();
    bool _checkCB();
    bool _checkCondicionesFisicas();
    bool _sltCheckVision();
    void sltLoadIni();
    void sltConfigLampara();
    void sltConfigCorte();
    void sltTestCorte();
    void sltToogleCorte();





public :
    //QList<struct In2VarImage> BufferVarImages;
    In2BufferImages BufferVarImages;

public slots:
    void CargaDatos();
    bool SelImagen();
    void SelImagenFija();
    void SendToXaar();
    void SendToVarXaar();
    void SendIniToXaar();
    void StopPrinting();
    void MuestraImagen(QPixmap Imagen);
    void MuestraImagenFromFile();
    void UpdateTotalVarXarSentSwathe();
    void UpdateVarXarSentSwathe(int last);
    void ForceRefreshValues();

    void enableMonitoring(bool value);

private:
    Ui::MaquinaSPass *ui;
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
    int m_ActualHeight;
    DWORD m_trimValues[MAXSCORPIONS][MAXHEADS][2]; //Valores de limpieza
    int m_ModeClean;
    signed short m_prevVelocidad;
    bool m_MonitoringEnabled;
    QVector<In2GraphicsScene *> m_Scene;
    QVector<In2GraphicsView *> m_View;
    In2GraphicsScene *m_sceneVision;
    In2GraphicsView *m_ViewVision;
    QString m_GoodVisionPath;
    QString m_lastVisionFile;
    bool m_VisionEnabled;
    QVector<QRgb> table;
    QString m_nombrefichero;
    //MemoryMap *m_memoryMap;
    QWidget *m_parent;
    bool m_TrabajoNuevo;
    bool m_bLoadFromCola;
    bool m_bCodigoCorrecto;


signals:
	void DataChange(int status);
    void CurrentPrintSwathe(int swathe);
    void StartPrecreate(int Origin);
    void StartPreload();
    void StartLoading();
    void StartGenerating();
    void PauseGenerating();
    void StopThreadCreator();
    void StopThreadSender();
    void SendDebugInfo(unsigned char code, QString msg);
    void SendInformerInfo(unsigned char code, QString msg);
    void sgnMaxBobina();
    void sglSetFuerzaFreno(QVariant valor);
    void sglSetMetros(QVariant valor);
    void sglSetDiametroEntrada(QVariant valor);
    void sglSetDiametroSalida(QVariant valor);
    void sglSetDiametroParo(QVariant valor);
    void sglTrabajoNuevo();
    void sglJogMasOn(bool valor);
    void sglJogMasOff();
    void sglJogMenosOn(bool valor);
    void sglJogMenosOff();
    void sglBuscarMacula();
    void sglHabilitarCondicionesMovimiento(unsigned char version);
    void sgnShowLog();
    void KillIn2Msg();
    void sgnUpdateIn2Message(QString);
    void sgnNewIn2AutoPB(int);


};

#endif // MAQUINA_H
