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

//#include "principal.h"
#include <QApplication>
#include <QTextCodec>
#include "MemoryMap.h"
#include "In2ConfiguratorPrinter.h"
#include "Composicion.h"
#include "in2gsmonitor.h"
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    /*QTextCodec *linuxCodec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(linuxCodec);*/

    //Principal *w=new Principal(0);

    MemoryMap *m_memoryMap=MemoryMap::request(0);
    m_memoryMap->Init();
    //AbrirFicheroIniImpresora();
    In2ConfiguratorPrinter configPrinter;
    configPrinter.readConfig();
    QString path;
    path.append(QApplication::applicationDirPath());
    path.append(QString("\\Database\\log.sqlite"));

    m_memoryMap->setUser("Admin");
    m_memoryMap->setUserRights(ACCESS_GOD);
    //GlobalSetMainWindow(this);
    //this->setVisible(false);
    In2GsMonitor gsMonitor;
    if (!gsMonitor.isLoaded()){
        QMessageBox::warning(0,"Error","No existe gsdll64.dll. No se puede continuar",QMessageBox::Ok);
        QApplication::exit();
        return 0;
    }

    Composicion *ScrComposicion;
    ScrComposicion=new Composicion(0);
    ScrComposicion->show();
    //QObject::connect(ScrComposicion,SIGNAL(finishDesign()),this,SLOT(deleteLater()));

    //w.show();

    return a.exec();
}
