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


#include "viewini.h"
#include "globals.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QButtonGroup>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QApplication>
#include <Windows.h>
#include <QFontComboBox>
#include <assert.h>
#include <QPixmap>
#include <QDebug>


viewIni::viewIni(QWidget *parent)
    : ui(new Ui::ViewIniUI)
{
    ui->setupUi(this);

    MostrarIni();

    connect(ui->m_Back,SIGNAL(clicked()),this,SLOT(Salir()));

}

viewIni::~viewIni(){

}


//Acepta la selección realizada y vuelve a la pantalla de Configuracion
void viewIni::Salir(){
    this->deleteLater();
}



void viewIni::MostrarIni(){
    MemoryMap *memoryMap=MemoryMap::request(0);
    QString nombreFicheroIni=memoryMap->m_mapa.m_variables.NombreIni;
    QString ficheroIni=memoryMap->m_mapa.m_variables.NombreRutaIni;

    ui->m_IniContents->setTitle(nombreFicheroIni);
    ui->m_Content->clear();

    QFile inpuFile(ficheroIni);
    if (inpuFile.open(QIODevice::ReadOnly)){
        QTextStream in(&inpuFile);
        while (!in.atEnd()){
            QString line=in.readLine();
            QString formattedLine=formateaLinea(line);
            ui->m_Content->append(QString("%1").arg(formattedLine));
        }
        inpuFile.close();
    }

    QPoint punto(0,0);
    QTextCursor cursor=ui->m_Content->cursorForPosition(punto);
    ui->m_Content->setTextCursor(cursor);

}


QString viewIni::formateaLinea(QString entrada){

    QString stringRet;
    QString stringRetLeft,stringRetRight;

    stringRet.clear();
    stringRetLeft.clear();
    stringRetRight.clear();

    if (entrada.isEmpty()){
        stringRet = QString("\r\n");
    }
    else{
        QChar primerCaracter;
        primerCaracter=entrada.at(0);
        if (primerCaracter=='['){
            stringRet = QString("<font color=\"red\">%1</font>\r\n").arg(entrada);
        }
        else if (primerCaracter.isLetterOrNumber()){
            int indice=entrada.indexOf("=",Qt::CaseInsensitive);
            stringRetLeft=entrada.left(indice);
            if (indice!=-1) stringRetRight=entrada.right(entrada.size()-indice-1);
            else stringRetRight=QString("");
            stringRet=QString("<font color=\"blue\">%1</font> = <font color=\"green\">%2</font>\r\n").arg(stringRetLeft).arg(stringRetRight);
        }
        else{
            stringRet = QString("??\r\n");
        }
    }

    return stringRet;

}

