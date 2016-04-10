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
#include <QBitmap>
#include <QPainter>
#include <QDebug>
#include <QSignalMapper>
#include "teclado.h"
#include "MemoryMap.h"
#include <QDesktopWidget>
#include "globals.h"


static QStringList TextoBotonMayusculas=(QStringList()<<"1"<<"2"<<"3"<<"4"<<"5"<<"6"<<"7"<<"8"<<"9"<<"0"<<"Q"<<"W"<<"E"<<"R"<<"T"<<"Y"<<"U"<<"I"<<"O"<<"P"<<"A"<<"S"<<"D"<<"F"<<
    "G"<<"H"<<"J"<<"K"<<"L"<<"Ñ"<<"Z"<<"X"<<"C"<<"V"<<"B"<<"N"<<"M"<<","<<"."<<"_"<<"-"<<"?"<<"/"<<"!"
    <<"SHIFT"<<"SPACE"<<"BACK"<<"ESC"<<"ENTER");

static QStringList TextoBotonMinusculas=(QStringList()<<"1"<<"2"<<"3"<<"4"<<"5"<<"6"<<"7"<<"8"<<"9"<<"0"<<"q"<<"w"<<"e"<<"r"<<"t"<<"y"<<"u"<<"i"<<"o"<<"p"<<"a"<<"s"<<"d"<<"f"<<
    "g"<<"h"<<"j"<<"k"<<"l"<<"ñ"<<"z"<<"x"<<"c"<<"v"<<"b"<<"n"<<"m"<<","<<"."<<"_"<<"-"<<"?"<<"/"<<"!"
    <<"SHIFT"<<"SPACE"<<"BACK"<<"ESC"<<"ENTER");

Teclado::Teclado(QWidget *parent,int mode):ui(new Ui::TecladoUI)
{

    QDesktopWidget p;
    ui->setupUi(this);
    Qt::WindowFlags flags=Qt::Window;
    this->setWindowFlags(flags|Qt::CustomizeWindowHint|Qt::WindowStaysOnTopHint);
    this->move((p.width()-this->width())/2,p.height()-this->height());
    this->show();
    flag_mayusculas=0;
    m_FlagTilde=0;
    m_Group=new QButtonGroup();
#if QT_VERSION>=0x050000
    QList<QPushButton *> widgets = this->findChildren<QPushButton *>();
#else
    QList<QPushButton *> widgets = qFindChildren<QPushButton *>(this);
#endif

    //bool bdummy;
    foreach (QPushButton *w, widgets){
            m_Group->addButton(w);
            /*if (mode==NUMERICO){
                w->text().toInt(&bdummy,10);
                if (!bdummy) w->setEnabled(false);
            }*/

     }
    connect(m_Group,SIGNAL(buttonClicked(QAbstractButton*)),this, SLOT(TeclaPulsada(QAbstractButton*)));
    connect(m_Group,SIGNAL(buttonReleased(QAbstractButton*)),this, SLOT(TeclaSoltada(QAbstractButton*)));
    this->update();
}

Teclado::~Teclado()
{
 /*   delete (m_NumerosLayout);
    delete (Bloque);*/
}
void Teclado::TeclaSoltada(QAbstractButton *but){
    /*but->setDown(false);
    QPalette p = but->palette();
    p.setColor(QPalette::Button, QColor(Qt::lightGray));
    but->setPalette(p);*/

}

void Teclado::TeclaPulsada(QAbstractButton *but){
    QString tmp;
/*    QPalette p = but->palette();
    p.setColor(QPalette::Button, QColor(Qt::darkGray));
    but->setPalette(p);

    but->setDown(true);*/
    tmp=Texto;
    if (!but->text().compare("Back",Qt::CaseInsensitive)){
        Texto=tmp.mid(0,tmp.size()-1);
        m_FlagTilde=0;
    }
    else if (!but->text().compare("Esc",Qt::CaseInsensitive)){
        m_FlagTilde=0;
        this->Salir();
    }
    else if (!but->text().compare("Shift",Qt::CaseInsensitive)){
#if QT_VERSION>=0x050000
    QList<QPushButton *> widgets = this->findChildren<QPushButton *>();
#else
    QList<QPushButton *> widgets = qFindChildren<QPushButton *>(this);
#endif
        foreach (QPushButton *w, widgets){
            if (flag_mayusculas)
                w->setText(w->text().toLower());
            else
                w->setText(w->text().toUpper());
         }
        flag_mayusculas^=1;
        m_FlagTilde=0;
    }
    else if (!but->text().compare("ENTER",Qt::CaseInsensitive)){
        m_FlagTilde=0;
        this->Aceptar();
    }
    else if (!but->text().compare("´",Qt::CaseInsensitive)){
        //this->Aceptar();
        m_FlagTilde=1;
    }
    else if (!but->text().compare("",Qt::CaseInsensitive)){
        m_FlagTilde=0;
        tmp.append(QString(" "));
        Texto=tmp;
    }
    else {
        if (m_FlagTilde){
            if (but->text()=="a") Texto.append("á");
            else if (but->text()=="e") Texto.append("é");
            else if (but->text()=="i") Texto.append("í");
            else if (but->text()=="o") Texto.append("ó");
            else if (but->text()=="u") Texto.append("ú");
            else Texto.append(but->text());
        }
        else
        Texto.append(but->text());
        m_FlagTilde=0;
    }
    emit bufferTeclado(Texto);
}

void Teclado::setParent(QWidget *parent)
{
    this->setParent(parent);
    //Bloque->setParent(parent);
}

void Teclado::show()
{
    this->showNormal();
    //Bloque->show();
}

void Teclado::hide()
{
    Texto = "";
    //Bloque->hide();
    this->hide();
}

//Vuelca las pulsaciones de teclado
void Teclado::Pulsacion(int i){
    QString tmp;
    tmp=Texto;

    //Boton de mayúsculas
    if (i==TextoBotonMayusculas.size()-5) this->Mayusculas();
    //Boton de espacio
    else if (i==TextoBotonMayusculas.size()-4) Texto.append(" ");
    //Boton de back
    else if (i==TextoBotonMayusculas.size()-3) Texto=tmp.mid(0,tmp.size()-1);
    //Boton de escape
    else if (i==TextoBotonMayusculas.size()-2){
        this->Salir();
    }
    //Boton de enter
    else if (i==TextoBotonMayusculas.size()-1)this->Aceptar();
    //Resto de letras
    else{
        if (flag_mayusculas==0) Texto.append(TextoBotonMinusculas.at(i));
        else Texto.append(TextoBotonMayusculas.at(i));
    }
    emit bufferTeclado(Texto); //Manda una señal con lo que lleva escrito
}
//Sale del teclado
void Teclado::Salir(){
    emit Delete();
}
void Teclado::Aceptar(){
    emit Accept();
}

void Teclado::Mayusculas(){
    if (flag_mayusculas==0) flag_mayusculas=1;
    else flag_mayusculas=0;
}
//Carga un texto en buffer de salida
void Teclado::setText(QString Buffer){
    Texto=Buffer;
}
