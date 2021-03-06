﻿/*  Copyright (C) 2013 In2 Printing Solutions <info@in2ps.com>
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
#include "in2reportgenerator.h"
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



#define OUTPUT_FOLDER "PDF"
#define DEFAULT_REPORT_FONT_SIZE 12
#define LINE_HEIGHT DEFAULT_REPORT_FONT_SIZE+4
#define TITLE_FONT_SIZE 15

#define XMARGIN 25
#define YMARGIN 130
#define YHEIGHT 18
#define DEFAULT_OFFSET 35

In2ReportGenerator::In2ReportGenerator(QObject *parent) :
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

    /*InkjetDebug *dbg=InkjetDebug::request(0);
    connect(this,SIGNAL(SendDebugInfo(unsigned char,QString)),dbg,SLOT(ProcessMsg(unsigned char,QString)));*/

}
/** Destructor */
In2ReportGenerator::~In2ReportGenerator(){

}

/** Devueleve si es automatico*/
bool In2ReportGenerator::getAuto(){
    return m_auto;
}

/** Carga un trabajo*/
void In2ReportGenerator::setTrabajo(struct In2Trabajo &t){
    m_Trabajo=t;
}

/** Carga el nombre de salida*/
void In2ReportGenerator::sltSetOutputName(QString name){
    m_name=QString("%1\\%2").arg(m_outputPath).arg(name);
}

/** Genera un report*/
void In2ReportGenerator::sltGenerateReport(){
    QPrinter printer;
    printer.setOutputFormat(QPrinter::PdfFormat);
    QString name=m_name;
    //QDateTime time=QDateTime::currentDateTime();
    //QString markTime=time.toString("yyMMdd_hhmmss");
    if (m_name.isEmpty()){
        if (m_memoryMap->m_mapa.m_variables.Proceso.isEmpty()) name=QString("%1\\OFFLINE.pdf").arg(m_outputPath);
        else name=QString("%1\\%2.pdf").arg(m_outputPath).arg(m_memoryMap->m_mapa.m_variables.Proceso);
        //name.replace(".ini",QString("%1.pdf").arg(markTime));
        //name.replace(".ini",QString(".pdf"));
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
    painter.restore();
    yOffset+=5;

    QRect rect;
    QString txt;
    painter.save();
    In2Store db;


    QString bmpImage=m_memoryMap->m_mapa.m_variables.NombreRutaIni;
    bmpImage.replace(".ini",".bmp");
    QImage img=QImage(bmpImage);

    if (!img.isNull()){

        QImage toDraw;
        if (img.width()>img.height())
            toDraw=img.transformed(QTransform().rotate(90)).scaled(m_width-20,printer.height()-2*yOffset,Qt::KeepAspectRatio,Qt::SmoothTransformation);
        else toDraw=img.scaled(m_width-20,printer.height()-2*yOffset,Qt::KeepAspectRatio,Qt::SmoothTransformation);
        painter.drawImage(10,yOffset,toDraw);
        painter.setPen( QPen( Qt::red, 1) );
        painter.drawRect(10,yOffset,toDraw.width(),yOffset+toDraw.height());
        printer.newPage();
    }
    painter.restore();

    rect=QRect(0,yOffset,m_width,YHEIGHT);
    painter.drawText(rect,Qt::TextWrapAnywhere,"REGISTRO PARADAS");
    painter.drawLine(0,yOffset+YHEIGHT,m_width,yOffset+YHEIGHT);
    painter.restore();
    yOffset+=DEFAULT_OFFSET;

    if (m_memoryMap->m_mapa.m_variables.Proceso.compare("OFFLINE",Qt::CaseInsensitive)){
        QStringList lst=db.getAllDataFromCode(m_memoryMap->m_mapa.m_variables.Proceso,QString("%1").arg(LOGGER_PARADA));
        if (lst.count()){
            for (int x=0;x<lst.count();x++){
                txt=lst.at(x);
                QStringList foo=txt.split("<@>");
                for (int z=0;z<foo.count();z++){
                    rect=QRect(0,yOffset,m_width,YHEIGHT);
                    painter.drawText(rect,Qt::TextWrapAnywhere,foo.at(z));
                    yOffset+=YHEIGHT;
                    if (yOffset>printer.height()){
                        yOffset=DEFAULT_OFFSET;
                        printer.newPage();
                        txt=QString("REGISTRO PARADAS: %1").arg(m_memoryMap->m_mapa.m_variables.Proceso);
                        _setEncabezado(&painter,txt);
                    }
                }
                painter.drawLine(0,yOffset,m_width,yOffset);
                painter.drawLine(0,yOffset+2,m_width,yOffset+2);
                yOffset+=15;
            }
        }
    }

    if (m_memoryMap->m_mapa.m_variables.Proceso.compare("OFFLINE",Qt::CaseInsensitive)){
        QStringList lst=db.getAllData(m_memoryMap->m_mapa.m_variables.Proceso);
        yOffset=DEFAULT_OFFSET;
        QString snd;
        if (lst.count()){
            printer.newPage();
            _setEncabezado(&painter,QString("HISTORICO %1").arg(m_memoryMap->m_mapa.m_variables.Proceso));
            int fields=db.fieldsLastQuery();
            if (fields==0) fields=1; //por si hay interbloqueo
            for (int x=0;x<lst.count();x+=fields){
                QString txt;
                for (int y=x;y<x+fields;y++){
                    txt.append(QString("<%1>").arg(lst.at(y)));
                }
                int numLineas=txt.length()/125+1;
                rect=QRect(0,yOffset,m_width,numLineas*YHEIGHT);
                painter.drawText(rect,Qt::TextWrapAnywhere,txt);
                yOffset+=numLineas*YHEIGHT;
                if (yOffset>printer.height()){
                    yOffset=DEFAULT_OFFSET;
                    printer.newPage();
                }
                snd.append(txt);
            }
            emit In2ReportDataBaseData(snd);
        }
    }

    QFile iniFile(m_memoryMap->m_mapa.m_variables.NombreRutaIni);
    if (iniFile.open(QFile::ReadOnly | QFile::Text)){
        QTextStream in(&iniFile);
        QStringList lst=in.readAll().split("\n");
        painter.drawText(0,0,in.readAll());
        printer.newPage();
        yOffset=DEFAULT_OFFSET;
        txt=QString("Ini file: %1").arg(m_memoryMap->m_mapa.m_variables.NombreRutaIni);
        _setEncabezado(&painter,txt);
        for (int x=0;x<lst.count();x++){
            rect=QRect(0,yOffset,m_width,YHEIGHT);
            txt=lst.at(x);
            painter.drawText(rect,Qt::TextWrapAnywhere,txt);
            yOffset+=YHEIGHT;
            if (yOffset>printer.height()){
                yOffset=DEFAULT_OFFSET;
                printer.newPage();
                txt=QString("Ini file: %1").arg(m_memoryMap->m_mapa.m_variables.NombreRutaIni);
                _setEncabezado(&painter,txt);
            }
        }
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
void In2ReportGenerator::_setEncabezado(QPainter *painter,QString txt){
    painter->save();
    painter->drawText(0,0,txt);
    painter->setPen( QPen( Qt::blue, 1 ) );
    painter->drawLine(0,18,m_width,18);
    painter->drawLine(0,20,m_width,20);
    painter->restore();
}

/** Firma el pdf*/
bool In2ReportGenerator::SignFile(){
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
                command=QString("%1 /INPUT \"%2\" /OUTPUT \"%3\" /SIGNCERTIFICATE %4 /SIGNPAGE 1 /DISPLAYSIGNATURE /SIGN").arg(m_signCommand).arg(m_name).arg(outputFile).arg(m_signFile);
            else
                //command=QString("\"%1\" -i %2 -o %3 -p %4 -c %5").arg(m_Config.m_signCommand).arg(m_Config.m_inputPdfFile).arg(m_Config.m_outputPdfFile).arg(m_Config.m_signPassword).arg(m_Config.m_signFile);
                command=QString("%1 /INPUT \"%2\" /OUTPUT \"%3\" /SIGNCERTIFICATE %4 /SIGNPASSWORD %5 /SIGNPAGE 1  /DISPLAYSIGNATURE /SIGN").arg(m_signCommand).arg(m_name).arg(outputFile).arg(m_signFile).arg(m_signPassword);
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
            m_signProcess.start("cmd.exe", QStringList() << "/c" << "tmp.bat");
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


