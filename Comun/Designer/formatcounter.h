/** @addtogroup formatCounter
 *  @{*/
/** @file formatCounter.h
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright In2 Printing Solutions.
 *
 *    @brief Pantalla de parametros de impresion
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: formatCounter.h
 *    @li Versión: x.0
 *    @li Fecha: 05/05/13
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */

#ifndef FORMATCOUNTER_H
#define FORMATCOUNTER_H

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
#include "ui_EditCounterUI.h"
#include "inkjetcommon.h"

class formatCounter : public QWidget
{
	Q_OBJECT

public:
	//Constructor
    formatCounter(QWidget *parent,struct m_CounterParameters & data);

    //Destructor
    ~formatCounter();

private:


private slots:

	//Acepta la seleccion de parametros
	void AceptarSeleccion();
	//Cancela la selección de parámetros
	void CancelarSeleccion();

	void AceptarValoresCampos();
    bool ComprobacionesCounter();

public slots:


private:
    Ui::EditCounterUI *ui;
    InkjetCommon *DataCommon;
    //Temporizador de eventos
    QTimer *m_Temporizador;
    //QPixmap pixmapPlantilla;

    //QPixmap ImagenUnitaria;
    QPixmap rotatedpixmap;
    //Variables de Escalado
    unsigned char escalado;
    int offset_x;
    int offset_y;
    struct m_CounterParameters m_Counter;


signals:
    void sglCounterConfigured(const struct m_CounterParameters & data);

};

#endif // FORMATCOUNTER_H
