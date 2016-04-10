/** @addtogroup InkjetForm
 *  @{*/
/** @file inkjetform.cpp
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright Cemitec. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Dialogos base para la aplicación inkjet
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: inkjetform.cpp
 *    @li Versión: x.0
 *    @li Fecha: 01/04/08
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */

#include "inkjetform.h"
#include "globals.h"
#include <math.h>
#include <time.h>
#include <QDateTimeEdit>
#include "principal.h"
#include <QKeySequence>
#include <QAction>
#include <QMessageBox>
#include <QDir>


//Puntero a la aplicacion principal
static QWidget *MainWindow;




/** Constructor*/
InkjetForm::InkjetForm(QWidget *parent)
    : QWidget(parent)
{
    QLabel title1;
        //char tmpbuf[128];
    Qt::WindowFlags flags=Qt::Window;
	this->setWindowFlags(flags|Qt::CustomizeWindowHint);
    this->setGeometry(0,0,APP_WIDTH,APP_HEIGHT);

	setWindowTitle("In2 Printing Solutions");
	
	m_CentralFrame=new InkjetFrame(this);
	m_CentralFrame->setGeometry(IMG_X,IMG_Y,IMG_WIDTH,IMG_HEIGHT);
	m_InfoZone=new QGroupBox(this);
	m_InfoZone->setGeometry(INFO_X,INFO_Y,INFO_WIDTH,INFO_HEIGHT);
	
	m_StatusZone=new QGroupBox(this);
	m_StatusZone->setGeometry(STATUS_X,STATUS_Y,STATUS_WIDTH,STATUS_HEIGHT);
	
	m_OptionZone = new QGroupBox(this);
	m_OptionZone->setGeometry(MAIN_X,MAIN_Y,MAIN_WIDTH,MAIN_HEIGHT);
	
	m_OptionGroupLayout = new QVBoxLayout(m_OptionZone);
	
	//Labels del fichero
	m_RutaFicheroConfig=new QLabel(this->m_InfoZone);
	m_NombreFicheroConfig=new QLabel(this->m_InfoZone);
	
	m_RutaFicheroConfig->setFont( QFont( "Arial", QFONT8, QFont::Bold ) );
	m_NombreFicheroConfig->setFont( QFont( "Arial", QFONT8, QFont::Bold ) );

	m_NombreFicheroConfig->setText("Fichero Ini:");
	m_RutaFicheroConfig->setText("Ruta:");

	m_NombreFicheroConfig->setGeometry(FILE_X,this->m_InfoZone->height()/3,FILE_W,FILE_H);
	m_RutaFicheroConfig->setGeometry(FILE_X,(this->m_InfoZone->height()*2)/3,FILE_W,FILE_H);

	m_TextoStatus=new QLabel(this->m_StatusZone);
	m_TextoStatus->move(TEXT_X,this->m_StatusZone->height()*1/4);

	ShowStatus(ST_DESCONECTADA);
		
	fecha=new QLabel(this->m_StatusZone);
	fecha->move(12*DELTA,2*DELTA);
   	QDate dummyD=QDate::currentDate();
    QString dummy=dummyD.toString("dd'/'MM'/'yyyy");
	fecha->setText(dummy);

	hora=new QLabel(this->m_StatusZone);
	hora->move(12*DELTA,4*DELTA);
    QTime dummyT=QTime::currentTime();
    dummy=dummyT.toString("hh:mm");
	hora->setText(dummy);

	usuario_actual=new QLabel(this->m_StatusZone);
	usuario_actual->move(12*DELTA,6*DELTA);
	
	m_User=new QLabel(this->m_StatusZone);
	m_User->move(12*DELTA,8*DELTA);
	RefreshUserMode();

	QIcon icono_MostrarInicio(":res/images/desktop.png");
	m_MostrarInicio=new QPushButton(this->m_StatusZone);
	m_MostrarInicio->setFlat(true);
	m_MostrarInicio->setIcon(icono_MostrarInicio);
	connect(m_MostrarInicio,SIGNAL(clicked()),this,SLOT(MostrarInicio()));

	QIcon icono_MostrarAyuda(":res/images/contacto.png");
	m_MostrarAyuda=new QPushButton(this->m_StatusZone);
	m_MostrarAyuda->setFlat(true);
	m_MostrarAyuda->setIcon(icono_MostrarAyuda);
	connect(m_MostrarAyuda,SIGNAL(clicked()),this,SLOT(MostrarAyuda()));

#ifdef TFT_1024x600
	m_MostrarInicio->setGeometry(85*DELTA,2*DELTA,EDIT_H,EDIT_H);
	m_MostrarAyuda->setGeometry(85*DELTA,5*DELTA,EDIT_H,EDIT_H);
#endif

#ifdef TFT_800x600
	m_MostrarInicio->setGeometry(61*DELTA,2*DELTA,EDIT_H,EDIT_H);
	m_MostrarAyuda->setGeometry(61*DELTA,5*DELTA,EDIT_H,EDIT_H);	
#endif

    lbl_EstadoImpresora=new QLabel(this->m_StatusZone);
    lbl_EstadoImpresora->setText("Estado Impresora");
    lbl_EstadoImpresora->move(30*DELTA,2*DELTA);

    lbl_EstadoMaquina=new QLabel(this->m_StatusZone);
    lbl_EstadoMaquina->setText("Máquina");
    lbl_EstadoMaquina->move(30*DELTA,4*DELTA);

    led_EstadoMaquina=new QLabel(this->m_StatusZone);
    led_EstadoMaquina->move(30*DELTA,6*DELTA);
    led_EstadoMaquina->resize(2*DELTA,2*DELTA);

    lbl_EstadoTemperatura=new QLabel(this->m_StatusZone);
    lbl_EstadoTemperatura->setText("Hydra");
    lbl_EstadoTemperatura->move(40*DELTA,4*DELTA);

    led_EstadoTemperatura=new QLabel(this->m_StatusZone);
    led_EstadoTemperatura->move(40*DELTA,6*DELTA);
    led_EstadoTemperatura->resize(2*DELTA,2*DELTA);

    lbl_EstadoLampara=new QLabel(this->m_StatusZone);
    lbl_EstadoLampara->setText("Lámpara");
    lbl_EstadoLampara->move(50*DELTA,4*DELTA);

    led_EstadoLampara=new QLabel(this->m_StatusZone);
    led_EstadoLampara->move(50*DELTA,6*DELTA);
    led_EstadoLampara->resize(2*DELTA,2*DELTA);


}	
//Destructor
InkjetForm::~InkjetForm(){
	delete (m_OptionGroupLayout);
	delete (m_CentralFrame);
	delete (m_StatusZone);
	delete (m_OptionZone);
}

/** refresca el modo de funcionamiento*/
void InkjetForm::RefreshUserMode(){
    if (memoryMap->getUserRights()==ACCESS_STRANGE) m_User->setText("Modo: STRANGE");
    else if (memoryMap->getUserRights()==ACCESS_PRINTER) m_User->setText("Modo: PRINTER");
    else if (memoryMap->getUserRights()==ACCESS_ROOT) m_User->setText("Modo: ROOT");
    else if (memoryMap->getUserRights()==ACCESS_GOD) m_User->setText("Modo: GOD");
}

//Cambia el estilo de todo el dialogo y sus widgets
void InkjetForm::CambiaEstilo(){
}

// Cambia el titulo de la zona de informacion
void InkjetForm::SetTitleInfo(QString Titulo){
	m_InfoZone->setTitle(Titulo);
}
// Cambia el titulo de la zona de status
void InkjetForm::SetTitleStatus(QString Titulo){
	m_StatusZone->setAlignment(Qt::AlignLeft); 
	m_StatusZone->setTitle(Titulo);
}
// Cambia el titulo de la zona de opciones
void InkjetForm::SetTitleOptions(QString Titulo){
	 m_OptionZone->setTitle(Titulo);
}

//Ordena los botones de la zona de opciones
void InkjetForm::OrdenaBotonesOpciones(){
	//Busca todos los hijos de la zona de opciones y los establece en el Layout
	QList<QWidget *> widgets = qFindChildren<QWidget *>(m_OptionZone);
	foreach (QWidget *w, widgets){
		w->setMinimumHeight(BUTTONH);
		w->setMinimumWidth(BUTTONW);
		m_OptionGroupLayout->addWidget(w);
	}
	m_OptionZone->setLayout(m_OptionGroupLayout);

}
//Establece cual es la pantalla principal
void InkjetForm::SetMainWindow(QWidget *pMainWindow){
	MainWindow=pMainWindow;
}
//Muestra la pantalla principal
void InkjetForm::ShowMainWindow(){
    //MainWindow->show();
    GlobalShowMainWindow();
}

//Oculta la pantalla principal
void InkjetForm::HideMainWindow(){
    //MainWindow->showMinimized();
    GlobalHideMainWindow();
}


//Devuelve un puntero a la aplicacion principal
QWidget* InkjetForm::GetMainWindow(){
    //return MainWindow;
    return GlobalGetMainWindow();
}
//Muestra informacion en la ventana superior
void InkjetForm::ShowInfo(){

    //if (memoryMap->m_ConfiguracionCargada==true) {
    if (memoryMap->m_UltimaAccion!=UA_NONE) {
        QString /*nombreruta,*/ruta,nombre;

        if (memoryMap->m_UltimaAccion==UA_CONFIGURACION_CARGADA){
            nombre=memoryMap->m_mapa.m_variables.NombreIni;
            ruta=memoryMap->m_mapa.m_variables.RutaIni;
        }
        if (memoryMap->m_UltimaAccion==UA_IMAGEN_ENVIADA){
            nombre=memoryMap->m_mapa.m_variables.NombreBmpEnviado;
            ruta=memoryMap->m_mapa.m_variables.RutaBmpEnviado;
        }

		m_NombreFicheroConfig->setText("Fichero:  " + nombre);
		m_RutaFicheroConfig->setText("      Ruta:  " + ruta);
	}
	else{
		m_NombreFicheroConfig->setText("Fichero:  ----");
		m_RutaFicheroConfig->setText("      Ruta:  ----");
	}

	m_NombreFicheroConfig->show();
	m_RutaFicheroConfig->show();

        QString dummy_usuario=QString("Usuario: %1").arg(memoryMap->getUser());
        usuario_actual->setText(dummy_usuario);
        usuario_actual->resize(24*DELTA,2*DELTA);
        usuario_actual->show();
        this->RefreshUserMode();

}


//Muestra informacion en la ventana inferior
void InkjetForm::ShowStatus(int iEstado){
	QPixmap Logo;	

	#ifdef MADEIN_IN2
	Logo.load(":/res/images/folder_logo.png");
	#endif
	
	#ifdef MADEIN_CEMITEC
	Logo.load(":/res/images/folder_cemitec.png");
	#endif
	
	QPixmap temp(Logo.size());
	temp.fill(Qt::transparent);
	QPainter p(&temp);
	p.setCompositionMode(QPainter::CompositionMode_Source);
	p.drawPixmap(0, 0,Logo);
	p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
	p.fillRect(temp.rect(), QColor(0, 0, 0, 75));

    //p.end(); Creo que habria que ponerlo

	Logo = temp;
	QPixmap dummy=Logo.scaledToHeight(LOGO_SIZE);
	m_TextoStatus->setPixmap((QPixmap)(dummy));	
	m_TextoStatus->show();
	m_TextoStatus->setEnabled(false);
		
}	

void InkjetForm::ShowEstadoImpresora(){

    QPixmap tmp;

    if (flag_led_Maquina_rojo==1) tmp.load(":res/images/Ledrojo.png");
    else tmp.load(":res/images/Ledverde.png");
    led_EstadoMaquina->setPixmap((QPixmap)tmp);

    if (flag_Hydra_ok==0) tmp.load(":res/images/Ledrojo.png");
    else tmp.load(":res/images/Ledverde.png");
    led_EstadoTemperatura->setPixmap((QPixmap)tmp);

    if (flag_led_Lampara_rojo==1) tmp.load(":res/images/Ledrojo.png");
    else tmp.load(":res/images/Ledverde.png");
    led_EstadoLampara->setPixmap((QPixmap)tmp);


}


//Muestra el estado de las comunicaciones
void InkjetForm::ShowComms(){		
	QDate dummyD=QDate::currentDate();
    QString dummy_date=dummyD.toString("dd'/'MM'/'yyyy");
	fecha->setText(dummy_date);
	
	QTime dummyT=QTime::currentTime();
    QString dummy_time=dummyT.toString("hh:mm");
	hora->setText(dummy_time);

    ShowInfo();
}


void InkjetForm::MostrarInicio(void){
	this->showMinimized();
}

void InkjetForm::MostrarAyuda(void){
#ifdef MADEIN_IN2
        QMessageBox::information(0,"Contacto","In2 Printing Solutions\nCarlos Corchón\nTfno 636648974\nwww.in2ps.com",QMessageBox::Ok);
#endif

#ifdef MADEIN_CEMITEC
		QMessageBox::information(0,"Contacto","CEMITEC\nTfno 848420800\nwww.cemitec.com",QMessageBox::Ok);
#endif

}


//Carga el Led que se le indique con el pixmap indicado
void InkjetForm::ShowLED(int index){

}

