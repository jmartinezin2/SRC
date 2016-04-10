/** @addtogroup Usuarios
 *  @{*/
/** @file FicherosConf.cpp
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright Cemitec. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Dialogo de configuracion de la aplicacion inkjet
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: FicherosConf.cpp
 *    @li Versión: x.0
 *    @li Fecha: 01/04/08
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */

#include "usuarios.h"
#include "configuracion.h"
#include "ficherosconf.h"
#include "principal.h"
#include "globals.h"
#include "inputdata.h"
#include <QMessageBox>
#include <QFileDialog>

//Constructor
FicherosConf::FicherosConf(QWidget *parent)
	: InkjetForm(parent)
{
	//Titulos de las secciones
	this->SetTitleInfo("In2 Printing Solutions");
	this->SetTitleStatus("Estado");
	this->SetTitleOptions("Opciones");

	//Boton para control de usuarios
	m_LoadConfig = new QPushButton(this->m_OptionZone);
	m_LoadConfig->setText("Cargar\nConfiguración");    	
	m_LoadConfig->setIcon(QPixmap(":/res/images/load.png"));
	m_LoadConfig->setIconSize(QSize(WICONSIZE,HICONSIZE));

	m_SaveConfig = new QPushButton(this->m_OptionZone);
	m_SaveConfig->setText("Guardar\nConfiguración");    	
	m_SaveConfig->setIcon(QPixmap(":/res/images/save.png"));
	m_SaveConfig->setIconSize(QSize(WICONSIZE,HICONSIZE));

	m_BackButton = new QPushButton(this->m_OptionZone);
    m_BackButton->setText("Atrás");    	
	m_BackButton->setIcon(QPixmap(":res/images/back.png"));
	m_BackButton->setIconSize(QSize(WICONSIZE,HICONSIZE));

	this->ShowInfo();

	this->OrdenaBotonesOpciones();
	CargaDatos();
	QObject::connect(m_BackButton, SIGNAL(clicked()),this, SLOT(Atras()));
	QObject::connect(m_LoadConfig, SIGNAL(clicked()),this, SLOT(CargarConfiguracion()));
	QObject::connect(m_SaveConfig, SIGNAL(clicked()),this, SLOT(GuardarConfiguracion()));
	//connect((Principal*)this->GetMainWindow(), SIGNAL(ActualizaData()),this, SLOT(ActualizaDataSlot()));
	TestRoot();
}
//Destructor
FicherosConf::~FicherosConf(){
	delete (m_BackButton);
	delete (m_LoadConfig);
	delete (m_SaveConfig);
	delete (m_Imagen);
	delete (m_Texto);
}


void FicherosConf::Atras(){
	Configuracion *ScrConfiguracion;
	ScrConfiguracion=new Configuracion(0);
	ScrConfiguracion->show();
	this->deleteLater();
}

// Actualiza la info
void FicherosConf::ActualizaDataSlot(){
	this->ShowComms();
}

//Lanza la pantalla principal
void FicherosConf::LaunchPrincipal(){	
	this->ShowMainWindow();
	this->deleteLater();
}


//Carga la informacion de pantalla
void FicherosConf::CargaDatos(){
	//Muestra imagen central
	m_Imagen=new QLabel(this);
	m_Imagen->setGeometry(IMG_X+DELTA_X,IMG_Y+DELTA_Y,IMG_WIDTH-DELTA_WIDTH,IMG_HEIGHT-DELTA_HEIGHT);
	m_Texto=new QLabel(this);
	m_Texto->setGeometry(TEXT_X,TEXT_Y,TEXT_W,TEXT_H);

	Logo.load(":/res/images/utilities.png");

	QPixmap temp(Logo.size());
	temp.fill(Qt::transparent);
	QPainter p(&temp);
	p.setCompositionMode(QPainter::CompositionMode_Source);
	p.drawPixmap(0, 0,Logo);
	p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
	p.fillRect(temp.rect(), QColor(0, 0, 0, 75));

	Logo = temp;

	MuestraImagen(Logo);
	
}


//Muestra la imagen central
void FicherosConf::MuestraImagen(QPixmap Imagen){
	QString dummy;
	//Escalo y muestro imagen central 
	QPixmap dummy2=Imagen.scaledToHeight(IMG_CENTRAL_SIZE);
	m_Imagen->setPixmap((QPixmap)(dummy2));
	m_Texto->setFont(QFont( "Arial", QFONT10, QFont::Bold ));
	dummy.append("- CARGAR CONFIGURACIÓN\n");
	dummy.append("Cargar los datos existentes en un fichero de configuración\n\n");
	dummy.append("- GUARDAR CONFIGURACIÓN\n");
	dummy.append("Guardar los datos existentes en un fichero de configuración\n\n");
	m_Texto->setText(dummy);
	m_Texto->show();
}


void FicherosConf::TestRoot(){
    if (memoryMap->getUserRights()==ACCESS_STRANGE){
		m_SaveConfig->setEnabled(false);
	}
}

void FicherosConf::CargarConfiguracion(){

	int dummy;

	m_BackButton->setEnabled(false);
	m_LoadConfig->setEnabled(false);
	m_SaveConfig->setEnabled(false);

    if (memoryMap->getUserRights()==ACCESS_PRINTER){
		dummy=CuadroDialogoCargarConfiguracion();
		if (dummy==0) {
			AbrirFicheroIni();
			GeneracionRutaPlantilla();
			this->ShowInfo();
		}
		else QMessageBox::warning(0,"ERROR","Error durante el proceso\nde carga de configuración",QMessageBox::Ok);
	}

	m_BackButton->setEnabled(true);
	m_LoadConfig->setEnabled(true);
	m_SaveConfig->setEnabled(true);
}


void FicherosConf::GuardarConfiguracion(){
	
	int dummy;

	m_BackButton->setEnabled(false);
	m_LoadConfig->setEnabled(false);
	m_SaveConfig->setEnabled(false);

    if (memoryMap->getUserRights()==ACCESS_PRINTER){
		dummy=CuadroDialogoGuardarConfiguracion();
		if (dummy!=0) QMessageBox::warning(0,"ERROR","Error durante el proceso\nde guardado de configuración",QMessageBox::Ok);
	}

	m_BackButton->setEnabled(true);
	m_LoadConfig->setEnabled(true);
	m_SaveConfig->setEnabled(true);


}


int FicherosConf::CuadroDialogoCargarConfiguracion(){
	
	QFileDialog fileName(this);
	QString tmp;
	QStringList Ficheros;
	QByteArray x;

	fileName.setFileMode(QFileDialog::ExistingFile);
	fileName.setViewMode(QFileDialog::List);
	fileName.setFilter(tr("Configuration Files (*.ini)"));
	QList<QPushButton *> widgets = qFindChildren<QPushButton *>(&fileName);
	foreach (QPushButton *w, widgets){
			w->setAutoDefault(false);
	 }
	if (fileName.exec()){
		Ficheros=fileName.selectedFiles();
		if (!Ficheros.isEmpty()){
	
			tmp=Ficheros.at(0);//en tmp esta la ruta y el nombre del fichero

			QString ruta,nombre;
			QString nombre_pdf,nombre_bmp;
			int posicion_barra;
			int posicion_contrabarra;
			int ultima_posicion;
			posicion_barra=tmp.lastIndexOf("\\");
			posicion_contrabarra=tmp.lastIndexOf("/");
			if (posicion_contrabarra==-1) posicion_contrabarra=0;
			if (posicion_barra>posicion_contrabarra) ultima_posicion=posicion_barra;
			else ultima_posicion=posicion_contrabarra;
			
			ruta.clear();
			nombre.clear();
			ruta = tmp.left(ultima_posicion+1);
			nombre = tmp.mid(ultima_posicion+1);

			nombre_pdf=nombre;
			nombre_pdf.replace(".ini",".pdf");

			nombre_bmp=nombre;
			nombre_bmp.replace(".ini",".bmp");

            memoryMap->m_mapa.m_variables.NombreIni=nombre;
            memoryMap->m_mapa.m_variables.RutaIni=ruta;
            memoryMap->m_mapa.m_variables.NombreRutaIni=tmp;

            memoryMap->m_mapa.m_variables.NombrePdf=nombre_pdf;
            memoryMap->m_mapa.m_variables.NombreBmp=nombre_bmp;

			QFile File(tmp);

			if (!File.exists())	return -1;
			if (!File.open(QIODevice::ReadOnly)) return -1;
			x=File.readAll();
			if (x.size()==0) return -1;
			File.close();

			QMessageBox::information(0,"INFO","Configuración cargada",QMessageBox::Ok);
			//CargaPlantilla();

			memoryMap->m_ConfiguracionCargada=true;
			return 0;

		}
		else return -1;//JORGE
	}
	else return -1;//JORGE
}


int FicherosConf::CuadroDialogoGuardarConfiguracion(){
			   
	QFileDialog fileName(this);
	QStringList Ficheros;
	fileName.setAcceptMode(QFileDialog::AcceptSave);		
	fileName.setFileMode(QFileDialog::AnyFile);
	fileName.setViewMode(QFileDialog::List);
	fileName.setFilter(tr("Configuration Files (*.ini)"));
	fileName.setDefaultSuffix("ini");
	QList<QPushButton *> widgets = qFindChildren<QPushButton *>(&fileName);
	foreach (QPushButton *w, widgets){
			w->setAutoDefault(false);
	 }
	if (fileName.exec()){
		Ficheros=fileName.selectedFiles();
		
		if (!Ficheros.isEmpty()){
            QString tmp;
            tmp=Ficheros.at(0);
			QFile File(tmp);
			if (!File.open(QIODevice::WriteOnly)) return -1;
            memoryMap->m_mapa.m_variables.NombreRutaIni=tmp;


            QString ruta,nombre;

            int ultima_posicion;
            int posicion_barra=tmp.lastIndexOf("\\");
            int posicion_contrabarra=tmp.lastIndexOf("/");
			if (posicion_contrabarra==-1) posicion_contrabarra=0;
			if (posicion_barra>posicion_contrabarra) ultima_posicion=posicion_barra;
			else ultima_posicion=posicion_contrabarra;
			
			ruta.clear();
			nombre.clear();
			ruta = tmp.left(ultima_posicion+1);
			nombre = tmp.mid(ultima_posicion+1);

            memoryMap->m_mapa.m_variables.RutaIni=ruta;
            memoryMap->m_mapa.m_variables.NombreIni=nombre;
			
			QString nombre_pdf=nombre;
			nombre_pdf.replace(".ini",".pdf");

			QString nombre_bmp=nombre;
			nombre_bmp.replace(".ini",".bmp");

            memoryMap->m_mapa.m_variables.NombreIni=nombre;
            memoryMap->m_mapa.m_variables.RutaIni=ruta;
            memoryMap->m_mapa.m_variables.NombreRutaIni=tmp;

            memoryMap->m_mapa.m_variables.NombrePdf=nombre_pdf;
            memoryMap->m_mapa.m_variables.NombreBmp=nombre_bmp;

			//File.write((const char*)&memoryMap->m_mapa,sizeof(memoryMap->m_mapa));
			GuardarFicheroIni();
			File.close();
			QMessageBox::information(0,"INFO","Configuración guardada",QMessageBox::Ok);
			return 0;
		}
		else return -1;//JORGE
	}
	else return -1;//JORGE

}
