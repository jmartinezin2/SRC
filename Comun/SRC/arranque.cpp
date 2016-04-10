/** @addtogroup Arranque
 *  @{*/
/** @file Arranque.cpp
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright Cemitec. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Pantalla de arranque de la aplicacion inkjet
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: Arranque.cpp
 *    @li Versión: x.0
 *    @li Fecha: 01/04/08
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */

#include "arranque.h"
#include "globals.h"


Arranque::Arranque(QWidget *parent)
	: InkjetForm(parent)
{
	unsigned char index=0;
	
	int Delay;
	m_BarraEstado=new QProgressBar(this->m_InfoZone);
	m_BarraEstado->setMaximum(100);
	m_BarraEstado->setMinimum(0);
	m_BarraEstado->setGeometry(320,25,250,25);
	
	m_InfoEstado=new QLabel(this->m_InfoZone);
	m_InfoEstado->setGeometry(350,45,250,25);
	m_InfoEstado->setText("Cargando parametros");

        this->SetTitleInfo("INICIALIZACION DE SISTEMA...ESPERE POR FAVOR");
	//Muestra imagen central
	m_Imagen=new QLabel(this);
    m_Imagen->setGeometry(IMG_X+150,IMG_Y+15,IMG_WIDTH-150,IMG_HEIGHT-20);

	this->CambiaEstilo();
	this->show();
	//Aqui hay que hacer las comprobaciones
	for (index=0;index<m_BarraEstado->maximum()+1;index++){
		m_BarraEstado->setValue(index);
		for (Delay=0;Delay<10000000;Delay++);// 		
		
	}
	
}

Arranque::~Arranque()
{

}

