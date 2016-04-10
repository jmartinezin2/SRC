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
#include "in2selectpalette.h"
#include <QLineEdit>
#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>

#define MAX_S 16
#define ROW_ITEMS 7


In2SelectPalette::In2SelectPalette(QWidget *parent, QString id) : ui(new Ui::In2SelectPalette)
{
    ui->setupUi(this);
    connect(ui->m_pbCancel,SIGNAL(clicked(bool)),this,SLOT(sltCancel()));
    connect(ui->m_pbSelect,SIGNAL(clicked(bool)),this,SLOT(sltAcceptAndExit()));
    connect(ui->m_pbSelTodos,SIGNAL(clicked(bool)),this,SLOT(sltSelectAll()));
    connect(ui->m_pbSelNinguno,SIGNAL(clicked(bool)),this,SLOT(sltDeSelectAll()));
    ui->m_leTotal->setText("0");
    ui->m_leSeleccionados->setText("0");
    m_id=id;
}
/** Cancela*/
void In2SelectPalette::sltSelectAll(){
    for (int x=0;x<m_cb.count();x++){
        QCheckBox *p=m_cb.at(x);
        if (p) p->setChecked(true);
    }
    int count=ui->m_leTotal->text().toInt();
    ui->m_leSeleccionados->setText(QString("%1").arg(count));
}
/** Cancela*/
void In2SelectPalette::sltDeSelectAll(){
    for (int x=0;x<m_cb.count();x++){
        QCheckBox *p=m_cb.at(x);
        if (p) p->setChecked(false);
    }
    ui->m_leSeleccionados->setText("0");

}
/** Cancela*/
void In2SelectPalette::sltAcceptAndExit(){
    QString colors;
    int count=m_cb.count();
    ui->m_progressBar->setMaximum(count);
    for (int x=0;x<m_cb.count();x++){

        QCheckBox *p=m_cb.at(x);
        if (p) {
            if (p->isChecked()){
                colors.append(QString("%1@").arg(m_pal.at(x)));
            }
        }
        ui->m_progressBar->setValue(x);
        if ((x%100)==0) QCoreApplication::processEvents();
    }
    if (!colors.isEmpty()) {
        colors.chop(1); //Quitamos la ultima arroba

        emit sgnNewColor(colors,m_id);
    }
    sltExit();
}
/** Cancela*/
void In2SelectPalette::sltCancel(){
    sltExit();
}
/** Salimos*/
void In2SelectPalette::sltExit(){
    this->deleteLater();
}

/** Compara una paleta*/
void In2SelectPalette::comparePalette(QVector<QRgb> &pal){
    int count=0;
    for (int x=0;x<pal.count();x++){
        QRgb color=pal.at(x);
        for (int y=0;y<m_pal.count();y++){
            if (color==m_pal.at(y)){
                QCheckBox *p=m_cb.at(y);
                if (p) {
                    p->setChecked(true);
                    count++;
                }
                break;
            }
        }
    }
    ui->m_leSeleccionados->setText(QString("%1").arg(count));
}

/** Carga una paleta*/
void In2SelectPalette::setPalette(QVector<QRgb> &pal){
    int count=pal.count();
    m_pal=pal;
    ui->m_leTotal->setText(QString("%1").arg(pal.count()));

    //Cojo 3 subpaletas (rojo, verde y azul), las ordeno y las concateno
    QVector<QRgb> vectRed,vectGreen,vectBlue;
    ui->m_progressBar->setMaximum(count*2);
    for (int x=0;x<m_pal.count();x++){
        ui->m_progressBar->setValue(x);
        //ui->m_progressBar->update();
        //this->update();
        QCoreApplication::processEvents();
        QRgb foo=m_pal.at(x);
        QColor color(foo);
        int red=color.red();
        int green=color.green();
        int blue=color.blue();
        if ((red>green)&&(red>blue)){
            vectRed.append(foo);
        }
        else if ((green>red)&&(green>blue)){
           vectGreen.append(foo);
        }
        else{
            vectBlue.append(foo);
        }

    }

    qSort(vectRed);
    qSort(vectGreen);
    qSort(vectBlue);


    //qSort(m_pal);
    m_pal.clear();
    m_pal=vectRed+vectGreen+vectBlue;
    QFont fnt;
    fnt.setPointSize(5);
    QWidget *scrollwidget=new QWidget();
    scrollwidget->setLayout(new QGridLayout());
    ui->m_scrollArea->setWidget(scrollwidget);

    //ui->m_progressBar->setMaximum(count);
    for (int x=0;x<count;x++){
        QPalette color;
        color.setColor(QPalette::Base,QColor(m_pal.at(x)));
        QLineEdit *label=new QLineEdit(this);
        label->setReadOnly(true);
        QLabel *number=new QLabel(this);
        QCheckBox *cb=new QCheckBox(this);
        connect(cb,SIGNAL(clicked(bool)),this,SLOT(sltUpdateCounter(bool)));
        //cb->setText(QString("%1").arg(m_pal.at(x)));
        connect(this,SIGNAL(destroyed(QObject*)),label,SLOT(deleteLater()));
        connect(this,SIGNAL(destroyed(QObject*)),cb,SLOT(deleteLater()));
        connect(this,SIGNAL(destroyed(QObject*)),number,SLOT(deleteLater()));
        label->setMinimumHeight(MAX_S);
        label->setMinimumWidth(MAX_S);
        cb->setMaximumHeight(MAX_S);
        cb->setMaximumWidth(MAX_S);
        cb->setText(QString("%1").arg(m_pal.at(x)));
        number->setText(QString("0x%1").arg(QString::number(m_pal.at(x),16)));
        label->setPalette(color);

        QWidget *widget=new QWidget(this);
        widget->setLayout(new QGridLayout());
        /*widget->setMaximumWidth(35);
        widget->setMaximumHeight(35);*/

        static_cast<QGridLayout*>(widget->layout())->addWidget(label,0,0);
        static_cast<QGridLayout*>(widget->layout())->addWidget(cb,0,1);
        static_cast<QGridLayout*>(widget->layout())->addWidget(number,1,0,1,1);

        /*QGridLayout *foo=new QGridLayout();
        foo->addWidget(label,0,0);
        foo->addWidget(cb,0,1);
        foo->addWidget(number,1,0,1,2);*/



        m_cb.append(cb);
        static_cast<QGridLayout*>(scrollwidget->layout())->addWidget(widget,x/ROW_ITEMS,x%ROW_ITEMS,0);
        ui->m_progressBar->setValue(x+count);
        QCoreApplication::processEvents();
    }
    ui->m_progressBar->setValue(0);
}
/** Actualiza el contador*/
void In2SelectPalette::sltUpdateCounter(bool checked){
    int count=ui->m_leSeleccionados->text().toInt();
    if (checked) count++;
    else {
        if (count) count--;
    }

    ui->m_leSeleccionados->setText(QString("%1").arg(count));
}
