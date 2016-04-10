/** @addtogroup Utilidades
 *  @{*/
/** @file Utilidades.h
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright In2 Printing Solutions. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Dialogo de configuracion de la aplicacion inkjet
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: Utilidades.h
 *    @li Versión: x.0
 *    @li Fecha: 01/04/08
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */

#ifndef UTILIDADES_H
#define UTILIDADES_H

#include "inkjetform.h"
#include "globals.h"
#include "ui_UtilidadesUI.h"
#include "inkjetcommon.h"
#include "../../InkConsumeCalculator/inkConsumeCalculator.h"
#include <QLabel>
#include <QPixmap>

class Utilidades : /*public InkjetForm*/public QWidget
{
	Q_OBJECT

public:
    Utilidades(QWidget *parent);
    ~Utilidades();
public slots:
    //Lanza la pantalla de configuracion
    void LaunchConfiguracion();
    //Lanza la calculadora de tinta
    void LaunchInkCalculator();

	//Actualiza la info
	void ActualizaDataSlot();

private:
    //Pantalla
    Ui::UtilidadesUI *ui;
    //Estructura comun
    InkjetCommon *dataCommon;

private:
    //Carga datos comunes
    void setDataCommon();
    //Testea permisos
    void TestRoot();
	
};


#endif // UTILIDADES_H
