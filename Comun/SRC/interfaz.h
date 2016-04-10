/** @addtogroup Interfaz
 *  @{*/
/** @file Interfaz.h
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright In2 Printing Solutions. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Interfaz de control de Inkjet
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: interfaz.h
 *    @li Versión: x.0
 *    @li Fecha: 01/04/08
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */

#ifndef INTERFAZ_H
#define INTERFAZ_H

#include <QtGui/QMainWindow>
#include "ui_interfaz.h"

class Interfaz : public QMainWindow
{
	Q_OBJECT

public:
	Interfaz(QWidget *parent = 0, Qt::WFlags flags = 0);
	~Interfaz();

private:
	Ui::InterfazClass ui;
};

#endif // INTERFAZ_H
