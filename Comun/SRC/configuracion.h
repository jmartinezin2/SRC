/** @addtogroup Configuracion
 *  @{*/
/** @file Configuracion.h
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright In2 Printing Solutions. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Dialogo de configuracion de la aplicacion inkjet
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: Configuracion.h
 *    @li Versión: x.0
 *    @li Fecha: 01/04/08
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */

#ifndef CONFIGURACION_H
#define CONFIGURACION_H

#include "inkjetform.h"
#include "globals.h"
#include "ui_ConfiguracionUI.h"
#include "inkjetcommon.h"
#include <QLabel>
#include <QPixmap>

class Configuracion : /*public InkjetForm*/public QWidget
{
	Q_OBJECT

public:
	Configuracion(QWidget *parent);
	~Configuracion();
    void setIn2Parent(QWidget *p);
public slots:
	//Lanza la pantalla principal
	void LaunchPrincipal();
	//Lanza la config de root
	void LaunchConfUser();
    //Lanza la informacion de eventos
    void LaunchEventos();
    void LaunchMantenimiento();
    void LaunchHistorico();
    void LaunchCalculadoraTinta();
    void LaunchVersion();
    void LaunchPermisos();

private:
    //Pantalla
    Ui::ConfiguracionUI *ui;


private:
    //Testea permisos
    void TestRoot();
    QWidget *m_parent;
	
};


#endif // CONFIGURACION_H
