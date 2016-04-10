/** @addtogroup viewIni
 *  @{*/
/** @file viewIni.h
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright In2 Printing Solutions
 *
 *    @brief Pantalla de parametros de impresion
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: viewIni.h
 *    @li Versión: x.0
 *    @li Fecha: 05/05/08
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */

#ifndef viewIni_H
#define viewIni_H

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
#include "ui_ViewIniUI.h"
#include "inkjetcommon.h"

class viewIni : public QWidget
{
	Q_OBJECT

public:
	//Constructor
    viewIni(QWidget *parent);

    //Destructor
    ~viewIni();

private:


private slots:

	//Acepta la seleccion de parametros
    void Salir();
    void MostrarIni();
    QString formateaLinea(QString entrada);

public slots:


private:
    Ui::ViewIniUI *ui;
    InkjetCommon *DataCommon;

signals:


};

#endif // viewIni_H
