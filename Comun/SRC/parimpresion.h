/** @addtogroup ParImpresion
 *  @{*/
/** @file ParImpresion.h
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright In2 Printing Solutions. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Pantalla de parametros de impresion
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: ParImpresion.h
 *    @li Versión: x.0
 *    @li Fecha: 05/05/08
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */

#ifndef PARIMPRESION_H
#define PARIMPRESION_H

//#include "MemoryMap.h"
#include "inkjetform.h"
#include <QComboBox>
#include <QTextEdit>
#include <QCheckBox>
#include <QTimer>
#include <QLabel>
#include "status.h"
#include <QMouseEvent>
#include <QRubberBand>
#include "inkjetxusbctrl.h"
#include "inkjetxusbvarctrl.h"
#include "imagevarxaarsender.h"
#include "imagevarxaarcreator.h"
#include "globals.h"



class ParImpresion : public InkjetForm
{
	Q_OBJECT

public:
	//Constructor
	ParImpresion(QWidget *parent);
	//Destructor
	~ParImpresion();

private:
	//Carga los datos que hay en memoria
	void CargaDatos();
	//Guarda los datos que hay en memoria
	void GuardaDatos();
	//Actualiza la pantalla y llama a la base de datos
	void CheckConfig();
    void MuestraImagen(QPixmap Imagen);
    QImage createSubImage(QImage* image, const QRect & rect);
    //Mata hilos
    void StopThreads();

private slots:
	//Acepta la seleccion de parametros
	void AceptarSeleccion();
	//Cancela la selección de parámetros
	void CancelarSeleccion();
	//Imprime
	void Print();
	//Actualiza Info de impresion
	void UpdateTemporizador();
	//actualizar datos
	void ActualizaDataSlot();
	// Si no es root, bloqueo funcionalidad.
	void TestRoot();
    // Genera el PD
    void GeneratePD();
    // Lanza el PD en multihilo si han acabado todos los threads
    void sltLaunchPD();
    // Maneja los errores
    void sltErrorString(QString,int);
    //Pide infor al xaar
    int AskXaar();

    virtual void mousePressEvent(QMouseEvent * event);
    virtual void mouseReleaseEvent(QMouseEvent * event);
    virtual void mouseMoveEvent(QMouseEvent *event);
signals:
    void CurrentPrintSwathe(int swathe);
    void StartGenerating();
    void PauseGenerating();
    void StopThread();
public slots:
	//Actualiza informacion de status
	void ActualizaCompletado(int idone);
	bool SelImagen();
	void SelImagenFija();
    void SendToXaar();
    void SendToVarXaar();
    void UpdateVarXarSentSwathe(int);
    void UpdateTotalVarXarSentSwathe();
    void StopPrinting();
    void SoftStopPrinting();
public :
    struct In2VarImage BufferVarImages[SIZE_BUFFER_IMAGES];
private:
	//Boton de aceptar
	QPushButton *m_AceptarButton;
	//Boton de selección de imagen fija
	QPushButton *SelImgFijaButton;    

    QGroupBox *m_OpcionesImpresion;
    QPushButton *m_SendToXaarButton;
    QPushButton *m_SendPDButton;
    QPushButton *m_StopButton;
    QPushButton *m_SoftStopButton;
    QLabel *m_ImagenParImpresion;
    QTextEdit *m_qlSwatheActual;
    QTextEdit *m_qlLastSwatheSent;
    QTextEdit *m_qlSwatheTotal;

    QRubberBand *rubberBand;
    QPoint origin;

	//Status del programa
	Status *status;
	//Temporizador de eventos
	QTimer *m_Temporizador;
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
    //Contador total de swathes enviadas
    int m_TotalSwatheSent;
    //Swathe actual
    int m_ActualSwathe;

signals:
    void SendDebugInfo(unsigned char code, QString msg);

};


#endif // PARIMPRESION_H
