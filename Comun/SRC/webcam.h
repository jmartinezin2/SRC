/** @addtogroup Webcam
 *  @{*/
/** @file Webcam.h
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright In2 Printing Solutions. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Pantalla de parametros de impresion
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: Webcam.h
 *    @li Versión: x.0
 *    @li Fecha: 05/05/08
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */

#ifndef WEBCAM_H
#define WEBCAM_H

#include "inkjetform.h"
#include <QComboBox>
#include <QTextEdit>
#include <QTimer>
#include <QLabel>
#include <QCheckBox>
#include "status.h"
#include "cv.h"
#include "cvaux.h"
#include "cxcore.h"
#include "highgui.h"
#include "ui_WebCamUI.h"
#include "inkjetcommon.h"

class Webcam : public QWidget
{
	Q_OBJECT

public:
	//Constructor
	Webcam(QWidget *parent);
	//Destructor
	~Webcam();
	CvCapture* capture;
	bool existe_camara;
	int width;
	int height;
	int widthStep;
	int channels;
	int bytesperline;
	unsigned char *qImageBuffer;

private slots:
	//Acepta la seleccion de parametros
    void CerrarVentana();
	void UpdateTemporizador();

public slots:


private:
    Ui::WebCamUI *ui;
    InkjetCommon *DataCommon;
	QTimer *m_Temporizador;
};

#endif // WEBCAM_H
