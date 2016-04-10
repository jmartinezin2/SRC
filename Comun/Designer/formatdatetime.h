/** @addtogroup formatDateTime
 *  @{*/
/** @file formatDateTime.h
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright In2 Printing Solutions.
 *
 *    @brief Pantalla de parametros de impresion
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: formatDateTime.h
 *    @li Versión: x.0
 *    @li Fecha: 05/05/13
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */

#ifndef FORMATDATETIME_H
#define FORMATDATETIME_H

#include "InkjetForm.h"
#include "MemoryMap.h"
#include <QComboBox>
#include <QTextEdit>
#include <QCheckBox>
#include <QTimer>
#include <QButtonGroup>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QFontComboBox>
#include "ui_EditDateTimeUI.h"
#include "inkjetcommon.h"

#define CONFIG_FECHA        0
#define CONFIG_HORA         1
#define CONFIG_FECHA_HORA   2
#define CONFIG_HORA_FECHA   3

class formatDateTime : public QWidget
{
	Q_OBJECT

public:
	//Constructor
    formatDateTime(QWidget *parent,struct m_DateTimeParameters & data);

    //Destructor
    ~formatDateTime();

private:

    QDateTime datetime;

    QString formatoDateTime;

private slots:

	//Acepta la seleccion de parametros
	void AceptarSeleccion();
	//Cancela la selección de parámetros
	void CancelarSeleccion();

	void AceptarValoresCampos();

public slots:


private:
    Ui::EditDateTimeUI *ui;
    InkjetCommon *DataCommon;
    QPixmap rotatedpixmap;
    //Variables de Escalado
    unsigned char escalado;
    int offset_x;
    int offset_y;
    struct m_DateTimeParameters m_DateTime;


signals:
    void sglDateTimeConfigured(const struct m_DateTimeParameters & data);

};

#endif // FORMATDATETIME_H
