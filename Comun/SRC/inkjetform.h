/** @addtogroup InkjetForm
 *  @{*/
/** @file Inkjetform.h
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright In2 Printing Solutions. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Dialogos para la aplicación inkjet
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: inkjetform.h
 *    @li Versión: x.0
 *    @li Fecha: 01/04/08
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */

#ifndef INKJETFORM_H
#define INKJETFORM_H

#include "inkjetwidgets.h"
#include <QBasicTimer>
#include <QPolygonF>
#include <QLabel>
#include <QLayout>
#include <QPainter>
#include <QPainterPath>

class QLineEdit;

class InkjetForm : public QWidget
{
    Q_OBJECT
protected:
	//Zona central
	InkjetFrame *m_CentralFrame;
	//Zona superior
	QGroupBox *m_InfoZone;
	//Zona inferior
	QGroupBox *m_StatusZone;
	//Zona Opciones
	QGroupBox *m_OptionZone;
	//Imagen central a volcar
	QLabel	*m_ImgCentral;
	//Layout de la zona de opciones
	QVBoxLayout *m_OptionGroupLayout;
	//Estilo del form
	QStyle *inkjetStyle;
	//Imagen a mostrar de fondo
	QLabel *m_Imagen;
	//Texto de informacion de status
	QLabel *m_TextoStatus;

	//Labels del fichero
	QLabel *m_NombreFicheroConfig;
	QLabel *m_RutaFicheroConfig;

	QLabel *m_User;

	QLabel *fecha;
	QLabel *hora;
	QLabel *usuario_actual;

    QLabel *lbl_EstadoImpresora;
    QLabel *lbl_EstadoMaquina;
    QLabel *led_EstadoMaquina;
    QLabel *lbl_EstadoTemperatura;
    QLabel *led_EstadoTemperatura;
    QLabel *lbl_EstadoLampara;
    QLabel *led_EstadoLampara;

	QPushButton *m_MostrarInicio;
	QPushButton *m_MostrarAyuda;
	
public slots:
	//Avisa de que se ha introducido algo por teclado. La implementaran las pantallas que quieran
	//virtual void InputTeclado();
	//Muestra la barra de tareas de windows
	void MostrarInicio(void);
	void MostrarAyuda(void);

public:
    // Constructor
	InkjetForm(QWidget *parent);
	// Destructor
	~InkjetForm();
	//Cambia elestilo del dialogo
	void CambiaEstilo();
	// Cambia el titulo de la zona de informacion
     void SetTitleInfo(QString Titulo);
	// Cambia el titulo de la zona de status
    void SetTitleStatus(QString Titulo);
	// Cambia el titulo de la zona de opciones
     void SetTitleOptions(QString Titulo);
	// Ordena los botones de la zona de opciones
	void OrdenaBotonesOpciones();
	//Establece el dialogo principal de la aplicacion
	void SetMainWindow(QWidget *pMainWindow);
	//Muestra el dialogo principal
	void ShowMainWindow();
	void HideMainWindow();
	//Devuelve un puntero a la aplicacion principal
	QWidget* GetMainWindow();
	//Muestra informacion en la ventana superior
	void ShowInfo();
	//Muestra informacion relativa al estado del sistema en la ventana inferior
	void ShowStatus(int iEstado);
	//Muestra el estado de las comunicaciones
	void ShowComms();
	//Carga un LED especifico
	void ShowLED(int index);

    void ShowEstadoImpresora();

	//Refresca el modo de funcionamiento
	void RefreshUserMode(void);


private:

};


#endif // XFORM_H
