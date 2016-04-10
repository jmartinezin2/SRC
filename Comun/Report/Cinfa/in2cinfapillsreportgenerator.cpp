#include "in2cinfapillsreportgenerator.h"
#include <QFile>
#include <QSettings>
#include <QPrinter>
#include <QPainter>
#include <QRect>
#include <QDateTime>
#include <QProcess>
#include <QApplication>
#include <QTextStream>
#include "In2GestorDB.h"
#include "in2store.h"
#include "in2reportconfigurator.h"
#include "inkjetdebug.h"
#include "in2gestorvision.h"
#include <QDir>




#define OUTPUT_FOLDER "PDF"
#define DEFAULT_REPORT_FONT_SIZE 12
#define LINE_HEIGHT DEFAULT_REPORT_FONT_SIZE+4
#define TITLE_FONT_SIZE 15

#define XMARGIN 25
#define YMARGIN 130
#define YHEIGHT 18

In2CinfaPillsReportGenerator::In2CinfaPillsReportGenerator(QObject *parent) :
    QObject(parent)
{

    In2ReportConfigurator p;
    p.readConfig();
    m_signCommand=p.getSignCommand();
    m_signFile=p.getSignFile();
    m_signPassword=p.getSignPassword();
    m_signActive=p.getSignActive();
    m_Logo=p.getLogo();
    m_auto=p.getAuto();
    m_outputPath=p.getOutputPath();
    m_wselect=p.getWselect();
    m_memoryMap=MemoryMap::request(0);

    m_Trabajo.elapsedTime=0;
    m_Trabajo.iPrinted=0;
    m_Trabajo.iTotalToPrint=0;
    m_Trabajo.Offset=0;

}
/** Destructor */
In2CinfaPillsReportGenerator::~In2CinfaPillsReportGenerator(){

}

/** Devueleve si es automatico*/
bool In2CinfaPillsReportGenerator::getAuto(){
    return m_auto;
}

/** Carga un trabajo*/
void In2CinfaPillsReportGenerator::setTrabajo(struct In2Trabajo &t){
    m_Trabajo=t;
}

/** Carga el nombre de salida*/
void In2CinfaPillsReportGenerator::sltSetOutputName(QString name){
    m_name=QString("%1\\%2").arg(m_outputPath).arg(name);
}

/** Genera un report*/
void In2CinfaPillsReportGenerator::sltGenerateReport(){
    QPrinter printer;
    printer.setOutputFormat(QPrinter::PdfFormat);
    //printer.setResolution(QPrinter::HighResolution);
    QString name=m_name;
    QDateTime time=QDateTime::currentDateTime();
    QString markTime=time.toString("yyMMdd_hhmmss");
    if (m_name.isEmpty()){        
        if (m_memoryMap->m_mapa.m_variables.NombreIni.isEmpty()) name=QString("%1\\OFFLINE.ini").arg(m_outputPath);
        else name=QString("%1\\%2").arg(m_outputPath).arg(m_memoryMap->m_mapa.m_variables.NombreIni);
        name.replace(".ini",QString("%1.pdf").arg(markTime));
    }
    printer.setOutputFileName(name);
    m_name=name;

    QPainter painter; //The QPainter class performs painting on widgets and other paint devices (such as QPrinter)
    QFont font=QFont( "Arial", 9, QFont::Black );
    //QFontMetrics fontMetrics(font);
    painter.setFont(font);
    //here we start painting to the printer
    if (!painter.begin(&printer)){
        QString err=tr("ReportGenerator: No se pudo generar PDF");
        emit SendDebugInfo(LOGGER_WARNING,err);
        emit In2ReportFailed(err);
        return;
    }

    int yOffset=72;
    m_width=printer.width();

    QImage logoIn2(":res/images/In2HR.png");
    painter.drawImage(0,0,logoIn2.scaled(64,64,Qt::KeepAspectRatio,Qt::SmoothTransformation));
    painter.save();
    painter.setPen( QPen( Qt::blue, 1 ) );
    painter.drawText(128,30,"In2 Printing Solutions. Printing report");
    painter.drawLine(0,yOffset,m_width,yOffset);
    painter.drawLine(0,yOffset+2,m_width,yOffset+2);
    //painter.restore();
    yOffset+=50;
    painter.drawText(128,yOffset,"Resumen Impresión");
    painter.drawLine(0,yOffset+2,m_width,yOffset+2);
    painter.drawLine(0,yOffset+4,m_width,yOffset+4);

    painter.restore();
    yOffset+=25;

    QString txt;
    In2Store db;
    QRect rect;
    if (m_memoryMap->m_mapa.m_variables.Proceso.compare("OFFLINE",Qt::CaseInsensitive)){
        QStringList lst=db.getAllDataFromCode(m_memoryMap->m_mapa.m_variables.Proceso,QString("%1").arg(LOGGER_PARADA));
        if (lst.count()){
            QStringList first=lst.first().split("@");
            QStringList last=lst.last().split("@");

            txt=QString("Lote=%1").arg(m_memoryMap->m_mapa.m_variables.Proceso);
            rect=QRect(0,yOffset,m_width,YHEIGHT);
            painter.drawText(rect,txt);
            yOffset+=YHEIGHT;

            QStringList data=first.at(1).split("=");
            QString fecha;
            QString hora;
            if (data.count()==2)
                fecha=data.at(1);
            data=first.at(2).split("=");
            if (data.count()==2)
                hora=data.at(1);

            txt=QString("Inicio: %1 - %2").arg(fecha).arg(hora); //Inicio impresion
            rect=QRect(0,yOffset,m_width,YHEIGHT);
            painter.drawText(rect,txt);
            yOffset+=YHEIGHT;

            data=last.at(1).split("=");
            if (data.count()==2)
                fecha=data.at(1);
            data=last.at(3).split("=");
            if (data.count()==2)
                hora=data.at(1);

            txt=QString("Final: %1 - %2").arg(fecha).arg(hora); //Fin impresion
            rect=QRect(0,yOffset,m_width,YHEIGHT);
            painter.drawText(rect,txt);
            yOffset+=YHEIGHT;

            //Hay que hacer un calculo
            int iBuenos=0;
            int iRechazados=0;

            for (int x=0;x<lst.count();x++){
                QStringList actual=lst.at(x).split("@");
                QStringList data;
                bool ok=true;
                if (actual.count()>5){
                    QString foo=actual.at(4); //Compriomirdos buenos
                    data=foo.split("=");
                    if (data.count()==2){
                        int num=data.at(1).toInt(&ok);
                        if (ok) iBuenos+=num;
                    }
                    foo=actual.at(5); //Comprimidos malos
                    data=foo.split("=");
                    if (data.count()==2){
                        int num=data.at(1).toInt(&ok);
                        if (ok) iRechazados+=num;
                    }
                }
            }
            txt=QString("Comprimidos buenos=%1").arg(iBuenos);
            rect=QRect(0,yOffset,m_width,YHEIGHT);
            painter.drawText(rect,txt);
            yOffset+=YHEIGHT;

            txt=QString("Comprimidos rechazados=%1").arg(iRechazados);
            rect=QRect(0,yOffset,m_width,YHEIGHT);
            painter.drawText(rect,txt);
            yOffset+=YHEIGHT;

            txt=QString("Nº.Paradas=%1").arg(lst.count());
            rect=QRect(0,yOffset,m_width,YHEIGHT);
            painter.drawText(rect,txt);
            yOffset+=YHEIGHT;

            yOffset=25;
            printer.newPage();

            painter.drawText(25,yOffset,"Resumen Paradas");
            painter.drawLine(0,yOffset+2,m_width,yOffset+2);
            painter.drawLine(0,yOffset+4,m_width,yOffset+4);
            painter.restore();
            yOffset+=25;

            painter.setPen( QPen( Qt::blue, 1 ) );
            painter.drawLine(0,yOffset,m_width,yOffset);
            painter.restore();
            painter.setPen( QPen( Qt::black, 1 ) );
            yOffset+=5;
        }



        //Cada unza de las paradas
        for (int x=0;x<lst.count();x++){
            QStringList data=lst.at(x).split("@");
            for (int y=0;y<data.count();y++){
                rect=QRect(0,yOffset,m_width,YHEIGHT);
                txt=QString(data.at(y));
                painter.drawText(rect,txt);
                yOffset+=YHEIGHT;
                if (yOffset>printer.height()){
                    yOffset=25;
                    printer.newPage();
                }

            }
            painter.setPen( QPen( Qt::blue, 1 ) );
            painter.drawLine(0,yOffset,m_width,yOffset);
            painter.restore();
            painter.setPen( QPen( Qt::black, 1 ) );

        }

        yOffset+=5;

    }

    painter.restore();
    painter.end();

    if (!SignFile()){
        emit In2ReportFinished();
    }
    else emit In2SignedReportFinished();
    emit In2NewReportGenerated(m_name);
    emit SendDebugInfo(LOGGER_STORE,tr("GENERADO REPORT %1").arg(m_name));


}
/** Genera un encabezado*/
void In2CinfaPillsReportGenerator::_setEncabezado(QPainter *painter,QString txt){
    painter->save();
    painter->drawText(0,0,txt);
    painter->setPen( QPen( Qt::blue, 1 ) );
    painter->drawLine(0,18,m_width,18);
    painter->drawLine(0,20,m_width,20);
    painter->restore();
}

/** Firma el pdf*/
bool In2CinfaPillsReportGenerator::SignFile(){
    bool rtn=true;
    QString outputFile=m_name;
    outputFile.replace(".pdf","_In2.pdf");
    if (m_signActive){
        //Testeamos que esté todo
        if ((!QFile::exists(m_signCommand)) ||(!QFile::exists(m_signFile))){
            rtn=false;
        }

        else{
            QString command;
            if (m_signPassword.isEmpty())
                //command=QString("\"%1\" -i %2 -o %3 -p \"\" -c %4").arg(m_Config.m_signCommand).arg(m_Config.m_inputPdfFile).arg(m_Config.m_outputPdfFile).arg(m_Config.m_signFile);
                command=QString("\"%1\" /INPUT \"%2\" /OUTPUT \"%3\" /SIGNCERTIFICATE \"%4\" /SIGNPAGE 1 /DISPLAYSIGNATURE /SIGN").arg(m_signCommand).arg(m_name).arg(outputFile).arg(m_signFile);
            else
                //command=QString("\"%1\" -i %2 -o %3 -p %4 -c %5").arg(m_Config.m_signCommand).arg(m_Config.m_inputPdfFile).arg(m_Config.m_outputPdfFile).arg(m_Config.m_signPassword).arg(m_Config.m_signFile);
                command=QString("\"%1\" /INPUT \"%2\" /OUTPUT \"%3\" /SIGNCERTIFICATE %4 /SIGNPASSWORD %5 /SIGNPAGE 1  /DISPLAYSIGNATURE /SIGN").arg(m_signCommand).arg(m_name).arg(outputFile).arg(m_signFile).arg(m_signPassword);

            emit SendDebugInfo(LOGGER_WARNING, command);
            QFile myBat("tmp.bat");
            if (myBat.open(QIODevice::WriteOnly)){
#if QT_VERSION>=0x050000
          myBat.write(command.toLatin1().data());
#else
          myBat.write(command.toAscii().data());
#endif

                myBat.close();
            }
            QProcess m_signProcess;
            //QStringList foo=QStringList()<< "/c" << "tmp.bat";
            m_signProcess.start("c:\\windows\\system32\\cmd.exe", QStringList()<<"/C"<<"tmp.bat");
            //m_signProcess.start("cmd.exe",foo);
            if (m_signProcess.waitForStarted())
            {
                //Lo bloqueo o no?
                if (m_signProcess.waitForFinished()) {
                    if (QFile::exists(outputFile)) {
                        QFile::remove(m_name);
                        m_name=outputFile;
                    }
                }
            }
            else{
                QString err=tr("ReportGenerator: Error de aplicacion de firma");
                emit SendDebugInfo(LOGGER_WARNING,(LOGGER_WARNING,err));
                emit In2ReportFailed(err);
                rtn=false;
            }
        }
    }
    return rtn;
}


