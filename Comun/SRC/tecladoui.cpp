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
#include "tecladoui.h"
#include "ui_tecladoui.h"

TecladoUI::TecladoUI(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TecladoUI)
{
    ui->setupUi(this);
    Texto.clear();
    flag_mayusculas=0;
    m_FlagTilde=0;
    m_Group=new QButtonGroup();
#if QT_VERSION>=0x050000
    QList<QPushButton *> widgets = this->findChildren<QPushButton *>();
#else
    QList<QPushButton *> widgets = qFindChildren<QPushButton *>(this);
#endif

    foreach (QPushButton *w, widgets){
            m_Group->addButton(w);
     }
    connect(m_Group,SIGNAL(buttonClicked(QAbstractButton*)),this, SLOT(TeclaPulsada(QAbstractButton*)));

}

TecladoUI::~TecladoUI()
{
    delete ui;
}

void TecladoUI::TeclaPulsada(QAbstractButton *but){
    QString tmp;
    tmp=Texto;
    if (!but->text().compare("Back",Qt::CaseInsensitive)){
        Texto=tmp.mid(0,tmp.size()-1);
        m_FlagTilde=0;
    }
    else if (!but->text().compare("Esc",Qt::CaseInsensitive)){
        m_FlagTilde=0;
        this->Salir();
        this->deleteLater();
        return;
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
    else if (!but->text().compare("",Qt::CaseInsensitive)){
        m_FlagTilde=0;
        tmp.append(QString(" "));
        Texto=tmp;
    }
    else if (!but->text().compare("´",Qt::CaseInsensitive)){
        //this->Aceptar();
        m_FlagTilde=1;
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

void TecladoUI::setParent(QWidget *parent)
{
    this->setParent(parent);
    //Bloque->setParent(parent);
}

void TecladoUI::show()
{
    this->showNormal();
    //Bloque->show();
}

void TecladoUI::hide()
{
    Texto = "";
    //Bloque->hide();
    this->hide();
}

//Sale del teclado
void TecladoUI::Salir(){
    emit Delete();
}
void TecladoUI::Aceptar(){
    emit Accept();
}

void TecladoUI::Mayusculas(){
    if (flag_mayusculas==0) flag_mayusculas=1;
    else flag_mayusculas=0;
}
//Carga un texto en buffer de salida
void TecladoUI::setText(QString Buffer){
    Texto=Buffer;
}
