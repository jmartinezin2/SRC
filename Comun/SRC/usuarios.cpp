/*  Copyright (C) 2013 In2 Printing Solutions <info@in2ps.com>
    Author: Jose Maria Martinez

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
#include "usuarios.h"
#include "configuracion.h"
#include "confusers.h"
//#include "principal.h"
#include "globals.h"
#include "inputdata.h"
#include <QMessageBox>
#include <QFile>
#include "status.h"
#include "in2logon.h"


//Pantalla de datos
//static InputData *tmpSCR;
//Password
//static QString RootUsername="In2su";	//Nombre de usuario de Superusuario
//static QString RootPassword="ABC123";	//Contraseña de Superusuario
//static QString QClientePassword;        //Contraseña de Cliente (se guarda en users.bin y se puede cambiar)
//static QString temp_password1;
//static QString temp_password2;

unsigned char introduciendo_clave;

//Constructor
Usuarios::Usuarios(QWidget *parent)
    : /*InkjetForm(parent)*/ ui(new Ui::UsuariosUI)
{
    ui->setupUi(this);
    GlobalSetScreenSize(this);
    m_memoryMap=MemoryMap::request(0);
    MuestraImagenPrincipal();

	//Boton para control de usuarios
	introduciendo_clave=0;

	TestRoot();


    QObject::connect(ui->m_BackButton, SIGNAL(clicked()),this, SLOT(Atras()));
    QObject::connect(ui->m_InputPassword, SIGNAL(clicked()),this, SLOT(InputPassword()));
    //QObject::connect(m_ChangePassword, SIGNAL(clicked()),this, SLOT(ChangePassword()));
    QObject::connect(ui->m_AdminUsers, SIGNAL(clicked()),this, SLOT(AdministrarUsuarios()));
    //connect(GlobalGetMainWindow(), SIGNAL(ActualizaData()),this, SLOT(ActualizaDataSlot()));
    //connect(ui->m_ButtonShowLog,SIGNAL(clicked()),GlobalGetMainWindow(),SLOT(ShowLogScreen()));
    //#ifdef CON_OPC
    //connect((Principal*)this->GetMainWindow(), SIGNAL(ActualizaDataOPC()),this, SLOT(ActualizaDataSlotOPC()));
    //#endif
}
//Destructor
Usuarios::~Usuarios(){
}

void Usuarios::AdministrarUsuarios(){
    ConfUsers *ScrConfUsers;
    ScrConfUsers=new ConfUsers(0);
    ScrConfUsers->show();
    //this->deleteLater();
}

void Usuarios::Atras(){
	SaveUsersMap();
	LaunchPrincipal();
    this->deleteLater();
}

void Usuarios::InputPassword(){

    m_memoryMap->setUserRights(ACCESS_STRANGE);
    //Insertar_Evento(FIN_SESION,m_memoryMap->getUser());
    TestRoot();
    Status *status;
    status=new Status(this);
    status->EnableWindow();
    connect(status,SIGNAL(StatusOculto()),this,SLOT(UsuarioIntroducido()));
    status->show();

}


void Usuarios::UsuarioIntroducido(){
    /*In2Logon p;
    p.TestLocalUser();
    TestRoot();
    flag_cambio_user_mode=1;*/
}

//Lanza la pantalla principal
void Usuarios::LaunchPrincipal(){	
	this->deleteLater();
}

//Chequea el password
/*void Usuarios::CheckRootPassword(QString Password){
    if ( (RootPassword.compare(Password)==0) ){
        //QMessageBox::information(0,"INFO","Password correcto.\nModo funcionamiento cambiado a GOD",QMessageBox::Ok);
        isRoot=3;
        flag_cambio_user_mode=1;
        TestRoot();
        dataCommon->RefreshUserMode();
	}
	else{
        //QMessageBox::warning(0,"ERROR!","Password incorrecto!",QMessageBox::Ok);
	}

	introduciendo_clave=0;
}
*/
//Muestra la imagen central
void Usuarios::TestRoot(){
    if (m_memoryMap->getUserRights()<ACCESS_ROOT){
        ui->m_AdminUsers->setEnabled(false);
    }
    else{
        ui->m_AdminUsers->setEnabled(true);
    }

}

//Guarda en un fichero el mapa de memoria actual
int Usuarios::SaveUsersMap(){
	int x;
	QFile File("./users/users.bin");
	if (!File.open(QIODevice::WriteOnly))
         return -1;
	x=File.write(PasswordUsuario,sizeof(PasswordUsuario));
	//x=File.write((const char*)&this->MapaUsuarios,sizeof(this->MapaUsuarios));
	File.close();
	return x;

}

//Carga el fichero de memoria
int Usuarios::LoadUsersMap(){
	QByteArray x;
	QFile File("./users/users.bin");
	memset(&PasswordUsuario,0x00,sizeof(PasswordUsuario));
	
	if (!File.exists())
		return -1;
	if (!File.open(QIODevice::ReadOnly))
         return -1;
	x=File.readAll();
	if (x.size()==0) 
		return -1;
	memcpy(&PasswordUsuario,x.constData(),sizeof(PasswordUsuario));
	File.close();
    //QClientePassword=QString(PasswordUsuario);

	return 0;
}
/** Imagen central*/
void Usuarios::MuestraImagenPrincipal(){

        unsigned char escalado;

        int offset_x;
        int offset_y;
        QPixmap Imagen(":/res/images/In2HRGS.png");
        int lw=this->ui->m_Imagen->width();
        int lh=this->ui->m_Imagen->height();
        int iw=Imagen.width();
        int ih=Imagen.height();

        if ((iw==0)||(ih==0)) return;
        escalado=EscaladoDeImagenEnQLabel(lw,lh,iw,ih);

        QPixmap pic(this->ui->m_Imagen->size());
        QPainter p(&pic);   // create a QPainter for it
        //pic.fill(Qt::gray);

        QLinearGradient lg(0, 0, 0, this->ui->m_Imagen->height());
        lg.setColorAt(0, Qt::gray);
        //lg.setColorAt(0, QColor(18,18,18));
        lg.setColorAt(1, Qt::lightGray);
        p.setPen(Qt::NoPen);
        p.setBrush(lg);
        p.drawRect(ui->m_Imagen->rect());
        QPixmap dummy;

        switch (escalado){
        case IMAGEN_SINESCALAR:
            //dummy=Imagen;
            //Reescalo tambien esta parte para hacer zoom
            if ((1000*Imagen.height()/this->ui->m_Imagen->height())>(1000*Imagen.width()/this->ui->m_Imagen->width())){
                dummy=Imagen.scaledToHeight(this->ui->m_Imagen->height(),Qt::SmoothTransformation);
            }
            else{
                dummy=Imagen.scaledToWidth(this->ui->m_Imagen->width(),Qt::SmoothTransformation);
            }

            offset_x=(this->ui->m_Imagen->width()-dummy.width())/2;
            offset_y=(this->ui->m_Imagen->height()-dummy.height())/2;
            p.drawPixmap(offset_x,offset_y,dummy); // and draw your original pixmap on it
            this->ui->m_Imagen->setPixmap((QPixmap)(pic));
            break;
        case IMAGEN_S2W:
            dummy=Imagen.scaledToWidth(this->ui->m_Imagen->width(),Qt::SmoothTransformation);
            offset_x=(this->ui->m_Imagen->width()-dummy.width())/2;
            offset_y=(this->ui->m_Imagen->height()-dummy.height())/2;
            p.drawPixmap(offset_x,offset_y,dummy); // and draw your original pixmap on it
            this->ui->m_Imagen->setPixmap((QPixmap)(pic));
            break;
        case IMAGEN_S2H:
            dummy=Imagen.scaledToHeight(this->ui->m_Imagen->height(),Qt::SmoothTransformation);
            offset_x=(this->ui->m_Imagen->width()-dummy.width())/2;
            offset_y=(this->ui->m_Imagen->height()-dummy.height())/2;
            p.drawPixmap(offset_x,offset_y,dummy); // and draw your original pixmap on it
            this->ui->m_Imagen->setPixmap((QPixmap)(pic));
            break;
    }

    p.end();
    this->ui->m_Imagen->show();
}



