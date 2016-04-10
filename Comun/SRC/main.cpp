
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
//#include "principal.h"
#include <QTranslator>
#include <QFile>


using namespace std;

//extern QString InkjetPath;
extern long __stdcall In2Crashfilter(EXCEPTION_POINTERS *p);

static Principal *ScrInicial;


int main(int argc, char **argv)
{

        #ifdef __WINDOWS__
    //Q_INIT_RESOURCE(shared);
#endif

    ChangeStyle(THEME_NORMAL);
    QApplication app(argc, argv);

    //Filtramos las excepciones y hacemos un minidump
    //::SetUnhandledExceptionFilter(In2Crashfilter);

    #ifdef MADEIN_IN2
    //app.setWindowIcon(QIcon(":res/images/in2ps_128x128.bmp"));
    app.setWindowIcon(QIcon(":res/images/In2HR.png"));
	#endif

	#ifdef MADEIN_CEMITEC
	app.setWindowIcon(QIcon(":res/images/cemitec_128x128.bmp"));
	#endif

    #ifdef DEPURACION_IN2
    QMessageBox::warning(0,"WARNING","VERSION DE DEPURACION",QMessageBox::Ok);
    #endif
    //QLabel *dummy=ShowMensaje("Iniciando...");

    //Hacemos esto para permitir solo una instancia de la interfaz
    QSharedMemory sharedMemory;
    sharedMemory.setKey("In2 Printing Solutions Inkjet Module UniqueID");

   if (!sharedMemory.create(1)) {
        QMessageBox::warning(0,"Atención","Hay mas de una instancia",QMessageBox::Ok); // Exit already a process running
     /*   app.quit();
        return 0;*/
   }
   //else{
        //Guardamos el path
        SetApplicationDirPath(app.applicationDirPath());
        AbsolutePathDbLog.clear();
        AbsolutePathDbLog.append(QApplication::applicationDirPath());
        AbsolutePathDbLog.append(QString("\\Database\\log.sqlite"));




        ScrInicial=new Principal(0);
        //ScrInicial->show();
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
