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
#include "in2previsualizator.h"
#include <QFile>
#include <QPixmap>
#include <QPrinter>
#include <QPrintDialog>
#include <QTimer>

In2Previsualizator::In2Previsualizator(QWidget *parent,QString file) : ui(new Ui::In2Visualizator)
{
    ui->setupUi(this);
    this->setAttribute( Qt::WA_DeleteOnClose );
    connect(ui->m_pbOK,SIGNAL(clicked()),this,SLOT(deleteLater()));
    connect(ui->m_pbPrint,SIGNAL(clicked()),this,SLOT(sltPrint()));
    connect(ui->m_pbMuestra,SIGNAL(clicked()),this,SLOT(sltGeneraMuestra()));
    connect( this, SIGNAL(destroyed(QObject*)), this, SLOT(widgetDestroyed(QObject*)) );
    ui->m_pbMuestra->setVisible(false);
    m_view=NULL;
    m_scene=NULL;
    m_fileName=file;
    this->setWindowTitle(file);
    QTimer::singleShot(0,this,SLOT(sltShowImage()));
    //connect(this,SIGNAL(destroyed())
}

/** Genera una muestra para enviar a calidad*/
void In2Previsualizator::sltGeneraMuestra(){

}

/** Muestra la imagen*/
void In2Previsualizator::sltShowImage(){
    if (QFile::exists(m_fileName)){
        QPixmap pixmap(m_fileName);
        if (!pixmap.isNull()){
            m_view=new In2GraphicsView(ui->m_graphicsView);
            m_scene=new In2GraphicsScene(0,0,pixmap.width(),pixmap.height());
            In2GraphicsPixmapItem *p=new In2GraphicsPixmapItem(0,0,false);
            p->setPixmap(pixmap);
            m_scene->addItem(p);
            ui->m_graphicsView->setScene(m_scene);
            ui->m_graphicsView->fitInView(p,Qt::KeepAspectRatio);
            ui->m_graphicsView->show();

        }
    }
}

/** Imprime el documento*/
void In2Previsualizator::sltPrint(){
    if (QFile::exists(m_fileName)){
        QPixmap pixmap(m_fileName);
        if (!pixmap.isNull()){
            QPrinter printer(QPrinter::HighResolution);
            QPrintDialog*dlg = new QPrintDialog(&printer,this);
            dlg->setWindowTitle(QObject::tr("In2 Printing Solutions"));
            if(dlg->exec() == QDialog::Accepted) {
                //parent.print(&printer);
                QPainter painter(&printer);
                painter.drawPixmap(0,0,pixmap);
            }
            delete dlg;
        }
    }
}

In2Previsualizator::~In2Previsualizator()
{
}

