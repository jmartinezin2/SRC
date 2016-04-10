/** @addtogroup formatRandom
 *  @{*/
/** @file formatRandom.h
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright In2 Printing Solutions.
 *
 *    @brief Pantalla de parametros de impresion
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: formatRandom.h
 *    @li Versión: x.0
 *    @li Fecha: 05/05/13
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */

#ifndef formatRandom_H
#define formatRandom_H

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
#include "ui_EditRandomUI.h"
#include "inkjetcommon.h"

class formatRandom : public QWidget
{
	Q_OBJECT

public:
	//Constructor
    formatRandom(QWidget *parent,m_RandomNumberParameters &data);

    //Destructor
    ~formatRandom();

private:


private slots:

	//Acepta la seleccion de parametros
	void AceptarSeleccion();
	//Cancela la selección de parámetros
	void CancelarSeleccion();

	void AceptarValoresCampos();
    void ComprobacionesCounter();

public slots:


private:
    Ui::EditRandomUI *ui;
    InkjetCommon *DataCommon;

    struct m_RandomNumberParameters m_Random;


signals:
    void sglRandomConfigured(const struct m_RandomNumberParameters & data);

};

#endif // formatRandom_H
