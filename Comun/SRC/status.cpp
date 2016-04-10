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
#include "status.h"
//#include "globals.h"
#include <QPainter>
#include <QMessageBox>
#include <QProcess>
#include "in2logon.h"
#include "in2hash.h"
#include "inkjetdebug.h"
#include "in2defines.h"
#include "MemoryMap.h"

#include <windows.h>

//Tamaño en pixeles de la aplicación


QWidget *Parent;
 
Status::Status(QWidget *parent)
    : /*QWidget(parent)*/ui(new Ui::Data)
{

    ui->setupUi(this);
    m_loginCounter=0;
    InkjetDebug *foo=InkjetDebug::request(0);
    connect(this,SIGNAL(SendDebugInfo(unsigned char,QString)),foo,SLOT(ProcessMsg(unsigned char,QString)));
    Qt::WindowFlags flags=Qt::Window;
    this->setWindowFlags(flags|Qt::CustomizeWindowHint);
    this->setWindowModality(Qt::ApplicationModal);
	Parent=parent;
    //QObject::connect(this->ui->TecladoButton,SIGNAL(clicked()),this,SLOT(sltLaunchTeclado()));
    QObject::connect(this->ui->m_Aceptar, SIGNAL(clicked()),this, SLOT(Salir()));
    QObject::connect(parent,SIGNAL(destroyed()),this,SLOT(deleteLater()));
    QObject::connect(this->ui->Teclado, SIGNAL(bufferTeclado(QString)),this, SLOT(UpdateCampo(QString )));
    QObject::connect(this->ui->Teclado, SIGNAL(Delete()),this,SLOT(deleteLater()));
    QObject::connect(this->ui->Teclado, SIGNAL(Accept()),this, SLOT(Salir()));
    //QObject::connect(this->ui->m_User, SIGNAL(textChanged()),this, SLOT(TestEnter()));
    //QObject::connect(this->ui->m_Password, SIGNAL(textChanged()),this, SLOT(TestEnter()));
    //QObject::connect(this->ui->m_User,SIGNAL(cursorPositionChanged()),this,SLOT(on_m_User_selectionChanged()));
    //QObject::connect(this->ui->m_Password,SIGNAL(cursorPositionChanged(int,int)),this,SLOT(on_m_Password_selectionChanged(int,int)));

    QObject::connect(this->ui->m_Password, SIGNAL(returnPressed()),this, SLOT(Salir()));
    ui->m_User->setAlignment(Qt::AlignVCenter|Qt::AlignCenter);
    ui->m_Password->setAlignment(Qt::AlignVCenter|Qt::AlignCenter);
    ui->m_User->setFocus();
    LastFocus=FOCUS_USER;

    ui->m_User->installEventFilter(this);
    ui->m_Password->installEventFilter(this);
    /*QPalette pal = this->palette();
    pal.setColor(QPalette::Background,QColor(10,10,10));
    this->setPalette(pal);*/

    /*Teclado lanza;
    lanza.show();*/

}
/** Filtro de eventos*/
bool Status::eventFilter(QObject *p, QEvent *e){
    if (e->type() == QEvent::MouseButtonPress){
        if (p==ui->m_User) {
            LastFocus=FOCUS_USER;
            ui->Teclado->setText(ui->m_User->text());
        }
        else if (p==ui->m_Password){
            LastFocus=FOCUS_PASSWORD;
            ui->Teclado->setText(ui->m_Password->text());
        }
    }
    return QWidget::eventFilter(p,e);
}


Status::~Status()
{
    //delete (m_Mensaje);
}

/** Recibe la pulsacion*/
void Status::UpdateCampo(QString data){
    /*if (ui->m_User->hasFocus()) LastFocus=FOCUS_USER;
    else if (ui->m_User->hasFocus()) LastFocus=FOCUS_PASSWORD;*/
    ui->m_User->setAlignment(Qt::AlignVCenter|Qt::AlignCenter);
    ui->m_Password->setAlignment(Qt::AlignVCenter|Qt::AlignCenter);

    switch (LastFocus){
        case FOCUS_USER:
            this->ui->m_User->setText(data);
        break;
    case FOCUS_PASSWORD:
            this->ui->m_Password->setText(data);
        break;
    }
}
/** Salimos*/
void Status::Salir(){
    MemoryMap *memoryMap=MemoryMap::request(0);
    memoryMap->setUserAndPassword(this->ui->m_User->text(),this->ui->m_Password->text());
    //emit StatusOculto();
    //this->deleteLater();
    UsuarioIntroducido();
}

/** Testea el supersuser*/
bool Status::TestSuperUser(){
    MemoryMap *memoryMap=MemoryMap::request(0);
#if QT_VERSION >= 0x050000
    QByteArray usuario_dummy=memoryMap->getUser().toLatin1();
    QByteArray RootUsernameEncrypted_dummy=RootUsernameEncrypted.toLatin1();
    QByteArray password_dummy=memoryMap->getPassword().toLatin1();
    QByteArray RootPasswordEncrypted_dummy=RootPasswordEncrypted.toLatin1();
#else
    QByteArray usuario_dummy=memoryMap->getUser().toAscii();
    QByteArray RootUsernameEncrypted_dummy=QByteArray(ROOTUSERNAMEENCRYPTED);//QString("%1").arg(ROOTUSERNAMEENCRYPTED).toAscii();
    QByteArray password_dummy=memoryMap->getPassword().toAscii();
    QByteArray RootPasswordEncrypted_dummy=QByteArray(ROOTPASSWORDENCRYPTED);//QString("%1").arg(ROOTPASSWORDENCRYPTED).toAscii();
    //MemoryMap *memoryMap=MemoryMap::request(0);
    if (!memoryMap->getClient().compare(CLIENT_ESTEVE)){
        RootPasswordEncrypted_dummy=QByteArray(ROOTPASSWORDENCRYPTED_ESTEVE);//QString("%1").arg(ROOTPASSWORDENCRYPTED).toAscii();
    }

#endif
    bool bUser;
    In2Hash fooHash;
    bUser=fooHash.TestHash(usuario_dummy,RootUsernameEncrypted_dummy);
    bool bPassword;
    bPassword=fooHash.TestHash(password_dummy,RootPasswordEncrypted_dummy);
    //Por si acaso, se testea el password de SuperUser solo para In2
    if (!bPassword){
        if (!memoryMap->getPassword().compare(ROOTPASSWORDIN2)) bPassword=true;
    }

    if (bUser && bPassword) return true;
    else return false;

}
/** Testea el usuario introducido*/
void Status::UsuarioIntroducido(){
    MemoryMap *memoryMap=MemoryMap::request(0);
    if (!memoryMap->m_mapa.m_config.m_LDAPActive){
        if (!this->TestSuperUser()){
            In2Logon p;
            if (!p.TestLocalUser()){
                m_loginCounter++;
                if (m_loginCounter<3){
                    ui->m_User->clear();
                    ui->m_Password->clear();
                    return;
                }
            }
            //memoryMap->setUserRights(ACCESS_PRINTER);
        }
        else{
            memoryMap->setUserRights(ACCESS_GOD);
        }
    }
    else{
        TestLDAP();
    }

    sltExit();
}

/** Comprueba conexion contra LDAP*/
void Status::TestLDAP(){
    MemoryMap *memoryMap=MemoryMap::request(0);
    QString dummy_usuario=QString("Usuario: %1").arg(memoryMap->getUser());
    if (this->TestSuperUser()){
        memoryMap->setUserRights(ACCESS_GOD);
    }
    else{
        QString msg;
        In2Logon *m_logon=new In2Logon();
        int rtn=m_logon->CheckDARoot();
        //int rtn=m_logon->CheckDA("");
        if (rtn!=LOGGER_USER_INCLUDED){
            rtn=m_logon->CheckDAPrinter();
            switch (rtn){
            case LOGGER_USER_GRANTED:
                msg=QString("Usuario %1 sin derechos").arg(dummy_usuario);
                QMessageBox::warning(0,"DENEGADO",msg,QMessageBox::Ok);
                emit SendDebugInfo(LOGGER_STORE,QString("Status:%1").arg(msg));
                memoryMap->setUserRights(ACCESS_STRANGE);
                break;
            case LOGGER_USER_NOT_GRANTED:
                msg=QString("Usuario %1 NO incluido en DA").arg(dummy_usuario);
                QMessageBox::warning(0,"DENEGADO",msg,QMessageBox::Ok);
                emit SendDebugInfo(LOGGER_STORE,QString("Status:%1").arg(msg));
                memoryMap->setUserRights(ACCESS_STRANGE);
                break;
            case LOGGER_USER_INCLUDED:
                msg=QString("Usuario %1 incluido en IN2PRINTER").arg(dummy_usuario);
                QMessageBox::warning(0,"PERMITIDO",msg,QMessageBox::Ok);
                emit SendDebugInfo(LOGGER_STORE,QString("Status:%1").arg(msg));
                memoryMap->setUserRights(ACCESS_PRINTER);
                break;
            case LOGGER_USER_NOT_INCLUDED:
                msg=QString("Usuario %1 no incluido en grupos IN2").arg(dummy_usuario);
                QMessageBox::warning(0,"DENEGADO",msg,QMessageBox::Ok);
                emit SendDebugInfo(LOGGER_STORE,QString("Status:%1").arg(msg));
                memoryMap->setUserRights(ACCESS_STRANGE);
                break;
            case LOGGER_NO_INIT:
                msg=QString("Usuario %1 - No se pudo iniciar LDAP").arg(dummy_usuario);
                QMessageBox::warning(0,"ERROR",msg,QMessageBox::Ok);
                emit SendDebugInfo(LOGGER_STORE,QString("Status:%1").arg(msg));
                memoryMap->setUserRights(ACCESS_STRANGE);
                break;
            case LOGGER_NO_CONNECTION:
                msg=QString("Usuario %1 - LDAP: Error de conexion").arg(dummy_usuario);
                QMessageBox::warning(0,"ERROR",msg,QMessageBox::Ok);
                emit SendDebugInfo(LOGGER_STORE,QString("Status:%1").arg(msg));
                memoryMap->setUserRights(ACCESS_STRANGE);
                break;
            }
        }
        else{
            msg=QString("Usuario %1 incluido en IN2ROOT").arg(dummy_usuario);
            memoryMap->setUserRights(ACCESS_ROOT);
            QMessageBox::warning(0,"PERMITIDO",msg,QMessageBox::Ok);
        }
        delete (m_logon);
    }

}


/** Sale*/
void Status::sltExit(){
    MemoryMap *memoryMap=MemoryMap::request(0);
    QString user=memoryMap->getUser();
    QString right=memoryMap->getUserRightsText();
    emit SendDebugInfo(LOGGER_STORE,QString("Status:[Usuario:%1][Derechos:%2]").arg(user).arg(right));
    emit StatusOculto();
    this->deleteLater();
}

/** Habilita la ventana*/
void Status::EnableWindow(){
    /*QList<QWidget *> widgets = qFindChildren<QWidget *>(Parent);
	foreach (QWidget *w, widgets) 
        w->setEnabled(true);*/
    //CleanQueueMsg();
}

/** Cambio en el password*/
void Status::CambioEnPassword(){
    QMessageBox::information(0,"Contacto",QString("cambio"),QMessageBox::Ok);
}

/** Limpia la cola de mensajes*/
void Status::CleanQueueMsg(){
    MSG message;
    while (PeekMessage( &message, NULL, 0, 0, PM_REMOVE)!=0){
            ::TranslateMessage(&message);
    ::DispatchMessage(&message);
    }
}


/** Ultimo focus el user*/
void Status::on_m_User_selectionChanged()
{
    ui->m_User->setAlignment(Qt::AlignVCenter|Qt::AlignCenter);
    ui->m_Password->setAlignment(Qt::AlignVCenter|Qt::AlignCenter);
    this->ui->Teclado->setText(this->ui->m_User->text());
    LastFocus=FOCUS_USER;
}

/** Ultimo focus en el password*/
void Status::on_m_Password_selectionChanged(int old,int inew)
{
    ui->m_User->setAlignment(Qt::AlignVCenter|Qt::AlignCenter);
    ui->m_Password->setAlignment(Qt::AlignVCenter|Qt::AlignCenter);
    this->ui->Teclado->setText(this->ui->m_Password->text());
    LastFocus=FOCUS_PASSWORD;
}


/** Testea si hemos metido enter*/
void Status::TestEnter(){
    QString bufferUser=this->ui->m_User->text();
    //QString bufferPassword=this->ui->m_Password->toPlainText();
    QString bufferPassword=this->ui->m_Password->text();
    if (bufferUser.endsWith("\n")){
        bufferUser.remove("\n");
        ui->m_User->setText(bufferUser);
        //on_m_User_selectionChanged();
        this->Salir();
    }
    else if (bufferPassword.endsWith("\n")){
        bufferPassword.remove("\n");
        //this->ui->m_Password->setText(bufferPassword);
        this->ui->m_Password->setText(bufferPassword);
        //on_m_Password_selectionChanged();
        this->Salir();
    }

}

