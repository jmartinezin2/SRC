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
#include "in2managerreport.h"
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
#include "MemoryMap.h"


In2ManagerReport::In2ManagerReport(QObject *parent) :
    QObject(parent)
{
    MemoryMap *memoryMap=MemoryMap::request(0);
    m_client=memoryMap->getClient();
    m_In2Gen=NULL;
    m_In2EsteveGen=NULL;
#ifndef PHARMA
    m_In2CinfaPillsGen=NULL;
#endif
    if (!m_client.compare(CLIENT_IN2)){
        m_In2Gen=new In2ReportGenerator(this);
        connect(m_In2Gen,SIGNAL(SendDebugInfo(unsigned char,QString)),this,SIGNAL(SendDebugInfo(unsigned char,QString)));
        connect(m_In2Gen,SIGNAL(In2SignedReportFinished()),this,SIGNAL(In2SignedReportFinished()));
        connect(m_In2Gen,SIGNAL(In2ReportFinished()),this,SIGNAL(In2ReportFinished()));
        connect(m_In2Gen,SIGNAL(In2NewReportGenerated(QString)),this,SIGNAL(In2NewReportGenerated(QString)));
        connect(m_In2Gen,SIGNAL(In2ReportFailed(QString)),this,SIGNAL(In2ReportFailed(QString)));
        connect(m_In2Gen,SIGNAL(In2ReportDataBaseData(QString)),this,SIGNAL(In2ReportDataBaseData(QString)));
    }
    else if (!m_client.compare(CLIENT_ESTEVE)){
        m_In2EsteveGen=new In2EsteveReportGenerator(this);
        connect(m_In2EsteveGen,SIGNAL(SendDebugInfo(unsigned char,QString)),this,SIGNAL(SendDebugInfo(unsigned char,QString)));
        connect(m_In2EsteveGen,SIGNAL(In2SignedReportFinished()),this,SIGNAL(In2SignedReportFinished()));
        connect(m_In2EsteveGen,SIGNAL(In2ReportFinished()),this,SIGNAL(In2ReportFinished()));
        connect(m_In2EsteveGen,SIGNAL(In2NewReportGenerated(QString)),this,SIGNAL(In2NewReportGenerated(QString)));
        connect(m_In2EsteveGen,SIGNAL(In2ReportFailed(QString)),this,SIGNAL(In2ReportFailed(QString)));
        connect(m_In2EsteveGen,SIGNAL(In2ReportDataBaseData(QString)),this,SIGNAL(In2ReportDataBaseData(QString)));
    }
#ifndef PHARMA
    else if (!m_client.compare(CLIENT_CINFAPILS)){
        m_In2CinfaPillsGen=new In2CinfaPillsReportGenerator(this);
        connect(m_In2CinfaPillsGen,SIGNAL(SendDebugInfo(unsigned char,QString)),this,SIGNAL(SendDebugInfo(unsigned char,QString)));
        connect(m_In2CinfaPillsGen,SIGNAL(In2SignedReportFinished()),this,SIGNAL(In2SignedReportFinished()));
        connect(m_In2CinfaPillsGen,SIGNAL(In2ReportFinished()),this,SIGNAL(In2ReportFinished()));
        connect(m_In2CinfaPillsGen,SIGNAL(In2NewReportGenerated(QString)),this,SIGNAL(In2NewReportGenerated(QString)));
        connect(m_In2CinfaPillsGen,SIGNAL(In2ReportFailed(QString)),this,SIGNAL(In2ReportFailed(QString)));
        connect(m_In2CinfaPillsGen,SIGNAL(In2ReportDataBaseData(QString)),this,SIGNAL(In2ReportDataBaseData(QString)));
    }
#endif
    else{ //Por defecto el generador de In2
        m_In2Gen=new In2ReportGenerator(this);
        connect(m_In2Gen,SIGNAL(SendDebugInfo(unsigned char,QString)),this,SIGNAL(SendDebugInfo(unsigned char,QString)));
        connect(m_In2Gen,SIGNAL(In2SignedReportFinished()),this,SIGNAL(In2SignedReportFinished()));
        connect(m_In2Gen,SIGNAL(In2ReportFinished()),this,SIGNAL(In2ReportFinished()));
        connect(m_In2Gen,SIGNAL(In2NewReportGenerated(QString)),this,SIGNAL(In2NewReportGenerated(QString)));
        connect(m_In2Gen,SIGNAL(In2ReportFailed(QString)),this,SIGNAL(In2ReportFailed(QString)));
        connect(m_In2Gen,SIGNAL(In2ReportDataBaseData(QString)),this,SIGNAL(In2ReportDataBaseData(QString)));
    }
    InkjetDebug *dbg=InkjetDebug::request(0);
    connect(this,SIGNAL(SendDebugInfo(unsigned char,QString)),dbg,SLOT(ProcessMsg(unsigned char,QString)));
}



/** Destructor */
In2ManagerReport::~In2ManagerReport(){
    if (m_In2Gen) delete (m_In2Gen);
    if (m_In2EsteveGen) delete (m_In2EsteveGen);
#ifndef PHARMA
    if (m_In2CinfaPillsGen) delete (m_In2CinfaPillsGen);
#endif
}

/** Devueleve si es automatico*/
bool In2ManagerReport::getAuto(){
    bool  rtn=false;
    if (!m_client.compare(CLIENT_IN2)){
        if (m_In2Gen) rtn=m_In2Gen->getAuto();
    }
    else if (!m_client.compare(CLIENT_ESTEVE)){
        if (m_In2EsteveGen) rtn=m_In2EsteveGen->getAuto();
    }
#ifndef PHARMA
    else if (!m_client.compare(CLIENT_CINFAPILS)){
        if (m_In2CinfaPillsGen) rtn=m_In2CinfaPillsGen->getAuto();
    }
#endif
    else{
        if (m_In2Gen) rtn=m_In2Gen->getAuto();
    }
    return rtn;
}

/** Carga un trabajo*/
void In2ManagerReport::setTrabajo(struct In2Trabajo &t){
    if (!m_client.compare(CLIENT_IN2)){
        if (m_In2Gen) m_In2Gen->setTrabajo(t);
    }
    else if (!m_client.compare(CLIENT_ESTEVE)){
        if (m_In2EsteveGen) m_In2EsteveGen->setTrabajo(t);
    }
#ifndef PHARMA
    else if (!m_client.compare(CLIENT_CINFAPILS)){
        if (m_In2CinfaPillsGen) m_In2CinfaPillsGen->setTrabajo(t);
    }
#endif
    else{
        if (m_In2Gen) m_In2Gen->setTrabajo(t);
    }
}

/** Carga el nombre de salida*/
void In2ManagerReport::sltSetOutputName(QString name){
    if (!m_client.compare(CLIENT_IN2)){
        if (m_In2Gen) m_In2Gen->sltSetOutputName(name);
    }
    else if (!m_client.compare(CLIENT_ESTEVE)){
        if (m_In2EsteveGen) m_In2EsteveGen->sltSetOutputName(name);
    }
#ifndef PHARMA
    else if (!m_client.compare(CLIENT_CINFAPILS)){
        if (m_In2CinfaPillsGen) m_In2CinfaPillsGen->sltSetOutputName(name);
    }
#endif
    else{
        if (m_In2Gen) m_In2Gen->sltSetOutputName(name);
    }
}

/** Genera un report*/
void In2ManagerReport::sltGenerateReport(){
    if (!m_client.compare(CLIENT_IN2)){
        if (m_In2Gen) m_In2Gen->sltGenerateReport();
    }
    else if (!m_client.compare(CLIENT_ESTEVE)){
        if (m_In2EsteveGen) m_In2EsteveGen->sltGenerateReport();
    }
#ifndef PHARMA
    else if (!m_client.compare(CLIENT_CINFAPILS)){
        if (m_In2CinfaPillsGen) m_In2CinfaPillsGen->sltGenerateReport();
    }
#endif
    else{
        if (m_In2Gen) m_In2Gen->sltGenerateReport();
    }
}
/** Genera un encabezado*/
void In2ManagerReport::_setEncabezado(QPainter *painter,QString txt){
    if (!m_client.compare(CLIENT_IN2)){
        if (m_In2Gen) m_In2Gen->_setEncabezado(painter,txt);
    }
    else if (!m_client.compare(CLIENT_ESTEVE)){
        if (m_In2EsteveGen) m_In2EsteveGen->_setEncabezado(painter,txt);
    }
#ifndef PHARMA
    else if (!m_client.compare(CLIENT_CINFAPILS)){
        if (m_In2CinfaPillsGen) m_In2CinfaPillsGen->_setEncabezado(painter,txt);
    }
#endif
    else{
        if (m_In2Gen) m_In2Gen->_setEncabezado(painter,txt);
    }
}

/** Firma el pdf*/
bool In2ManagerReport::SignFile(){
    bool rtn=false;
    if (!m_client.compare(CLIENT_IN2)){
        if (m_In2Gen) rtn=m_In2Gen->SignFile();
    }
    else if (!m_client.compare(CLIENT_ESTEVE)){
        if (m_In2EsteveGen) rtn=m_In2EsteveGen->SignFile();
    }
#ifndef PHARMA
    else if (!m_client.compare(CLIENT_CINFAPILS)){
        if (m_In2CinfaPillsGen) rtn=m_In2CinfaPillsGen->SignFile();
    }
#endif
    else{
        if (m_In2Gen) rtn=m_In2Gen->SignFile();
    }

    return rtn;
}


