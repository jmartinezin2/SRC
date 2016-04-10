/** @addtogroup Usuarios
 *  @{*/
/** @file FicherosConf.h
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright In2 Printing Solutions. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Dialogo de configuracion de la aplicacion inkjet
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: FicherosConf.h
 *    @li Versión: x.0
 *    @li Fecha: 01/04/08
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */

#ifndef FICHEROSCONF_H
#define FICHEROSCONF_H

#include "inkjetform.h"
#include "globals.h"
#include <QLabel>
#include <QPixmap>

class FicherosConf : public InkjetForm
{
	Q_OBJECT

public:
	FicherosConf(QWidget *parent);
	~FicherosConf();

private slots:
	//Lanza la pantalla principal
	//void Aceptar();
	//void Cancelar();
	void Atras();
	void LaunchPrincipal();
	void CargaDatos();

	void CargarConfiguracion();
	void GuardarConfiguracion();
	int CuadroDialogoCargarConfiguracion();
	int CuadroDialogoGuardarConfiguracion();
	void TestRoot();

	//Actualiza la info
	void ActualizaDataSlot();
	//Muestra logo central
	void MuestraImagen(QPixmap Imagen);
private:
	//QPushButton *m_AceptarButton;
	//QPushButton *m_CancelarButton;
	QPushButton *m_BackButton;
	QPushButton *m_LoadConfig;
	QPushButton *m_SaveConfig;

	//Logo central
	QPixmap Logo;
	//Imagen
	QLabel *m_Imagen;
	//Texto
	QLabel *m_Texto;
	
};


#endif // FICHEROSCONF_H
