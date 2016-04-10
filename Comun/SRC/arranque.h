/** @addtogroup Arranque
 *  @{*/
/** @file Arranque.h
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright In2 Printing Solutions. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Pantalla de arranque de la aplicacion inkjet
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: Arranque.h
 *    @li Versión: x.0
 *    @li Fecha: 01/04/08
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */


#ifndef ARRANQUE_H
#define ARRANQUE_H

#include <QObject>
#include <QProgressBar>
#include "inkjetform.h"

class Arranque : public InkjetForm
{

	Q_OBJECT

public:
	Arranque(QWidget *parent);
	~Arranque();
private:


private:
	//Barra de estado de arranque
	QProgressBar *m_BarraEstado;	
	QLabel	*m_InfoEstado;
};

#endif // ARRANQUE_H
