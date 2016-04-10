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

#include "inkjetform.h"
//#include "principal.h"
#include <QApplication>
#include <QSharedMemory>
#include <QMessageBox>
#include "globals.h"
#include "status.h"
#include <QDebug>
#include "winbase.h"
#include <string>
#include <iostream>
#include "principal.h"
#include <QTranslator>
#include <QFile>
#include "in2application.h"
#include "in2eventfilter.h"
#include "In2Licenser.h"


using namespace std;

//extern QString InkjetPath;
extern long __stdcall In2Crashfilter(EXCEPTION_POINTERS *p);

static Principal *ScrInicial;


int main(int argc, char **argv)
{

    ChangeStyle(THEME_NORMAL);
    QApplication app(argc, argv);
    In2EventFilter *f=In2EventFilter::request(0);
    app.installEventFilter(f);


    //Filtramos las excepciones y hacemos un minidump
    ::SetUnhandledExceptionFilter(In2Crashfilter);

    app.setWindowIcon(QIcon(":res/images/In2HR.png"));

    #ifdef DEPURACION_IN2
        QMessageBox::warning(0,"WARNING","VERSION DE DEPURACION",QMessageBox::Ok);
    #endif
    //QLabel *dummy=ShowMensaje("Iniciando...");

    //Hacemos esto para permitir solo una instancia de la interfaz
    QSharedMemory sharedMemory;
    sharedMemory.setKey("In2 Printing Solutions Inkjet Module UniqueID");

   if (!sharedMemory.create(1)) {
        QMessageBox::warning(0,"Atencion","Hay mas de una instancia",QMessageBox::Ok); // Exit already a process running
     /*   app.quit();
        return 0;*/
   }
   //else{
        //Guardamos el path
        SetApplicationDirPath(app.applicationDirPath());

       /* In2Licenser In2GestorLicencia;
        QString licencia=In2GestorLicencia.checkLicenseFromBBDD(QString("%1\\Database\\log.sqlite").arg(QApplication::applicationDirPath()));
        if (licencia.isEmpty()){
            QMessageBox::warning(0,"ERROR",QString("Error irrecuperable 1300 - No es posible iniciar"),QMessageBox::Ok);
            app.quit();
            return 0;
        }
        else{
            struct In2License strcIn2License=In2GestorLicencia.readLicenseFromBBDD(QString("%1\\Database\\log.sqlite").arg(QApplication::applicationDirPath()));
            QVector<int> fails=In2GestorLicencia.CheckLicense(strcIn2License);
            for (int x=0;x<fails.count();x++){
              int error=fails.at(x);
              QMessageBox::warning(0,"ERROR",QString("Error %1 - No es posible iniciar").arg(error),QMessageBox::Ok);
              app.quit();
              return 0;
            }
        }*/
        //Si todo está OK,
        ScrInicial=new Principal(0);
        QTimer::singleShot(0,ScrInicial,SLOT(init()));
   //}

   //delete (dummy);
   return app.exec();//Antes solo estaba esta linea
   //return 0;


 }

/** Filtro para analisis postmortem*/
extern long __stdcall In2Crashfilter(EXCEPTION_POINTERS *p)
{

    QFile file(QString("%1\\Output\\Dumps\\dump%2.txt").arg(QApplication::applicationDirPath()).arg(QTime::currentTime().toString("hhmmss")));
    if (file.open(QIODevice::WriteOnly)){

        QString dumpInf;
        //dumpInf.append(QString("Ini:%1\n").arg(QString::number(*((DWORD*)ini),16)));
        dumpInf.append(QString("Code:%1\n").arg(QString::number(p->ExceptionRecord->ExceptionCode,16)));
        DWORD *add=static_cast<DWORD*>(p->ExceptionRecord->ExceptionAddress);
        dumpInf.append(QString("Address:%1\n").arg(QString::number(*add,16)));
        dumpInf.append(QString("Flags:%1\n").arg(QString::number(p->ExceptionRecord->ExceptionFlags)));
        //dumpInf.append(QString("Info:%1\n").arg(p->ExceptionRecord->ExceptionInformation));
        dumpInf.append(QString("Params:%1\n").arg(p->ExceptionRecord->NumberParameters));
        //dumpInf.append(QString("Record:%1\n").arg(p->ExceptionRecord->ExceptionRecord));
        if (p->ExceptionRecord->NumberParameters){
            for (DWORD i=0;i<p->ExceptionRecord->NumberParameters;i++){
                dumpInf.append(QString("Params(%1):%1\n").arg(p->ExceptionRecord->ExceptionInformation[i]));
            }
        }
        MemoryMap *memoryMap=MemoryMap::request(0);
        dumpInf.append(QString("Fx: %1\n").arg(memoryMap->m_DebugLine));
        file.write(dumpInf.toAscii().data());
        file.close();
    }
  return 0;
}


/*static __inline__ void * get_pc(void) {
    register void * pc asm("pc");
    __asm__("" : "=r"(pc));
    return pc;
}*/
