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
#include "in2configprint.h"
#include "in2xmlparser.h"
#include "GestorImpresion.h"
#include "clienteopc.h"
#include "inkjetDebug.h"
#include "in2defines.h"

In2ConfigPrint::In2ConfigPrint(QWidget *parent) :
    ui(new Ui::In2ConfigPrintUI)
{

    m_memoryMap=MemoryMap::request(0);
    ui->setupUi(this);
    connect(ui->m_pbOK,SIGNAL(clicked()),this,SLOT(sltSaveAndExit()));
    connect(ui->m_cbReverse,SIGNAL(clicked()),this,SLOT(sltCheckConfig()));
    //Para que siempre esté por encima
    Qt::WindowFlags flags=this->windowFlags();
    flags|=Qt::WindowStaysOnTopHint;
    this->setWindowFlags(flags);

    GestorImpresion *printEngine=GestorImpresion::request(this);
    QString configFile=printEngine->getConfigFile();
    In2XmlParser parser(0,configFile);
    QStringList params=QStringList()<<"XUSB"<<"Configuration"<<"PrintMode";
    QString val=parser.getValue(params,"value");
    ui->m_cbModoImpresion->setCurrentIndex(val.toInt());
    params=QStringList()<<"XUSB"<<"Configuration"<<"SequenceStart";
    val=parser.getValue(params,"value");
    ui->m_cbTipoPD->setCurrentIndex(val.toInt());


#ifdef HIGH_SPEED
    ui->m_cbHighSpeed->setEnabled(true);
    ui->m_cbStandard->setEnabled(true);
#endif
    if (m_memoryMap->m_mapa.m_impresion.bEscalaGrises){
        ui->m_cbGrayScale->setChecked(true);
        ui->m_cbMono->setChecked(false);
    }
    else{
        ui->m_cbGrayScale->setChecked(false);
        ui->m_cbMono->setChecked(true);
    }

    if (m_memoryMap->m_mapa.m_impresion.bHighSpeed){
        ui->m_cbHighSpeed->setChecked(true);
        ui->m_cbStandard->setChecked(false);
    }
    else{
        ui->m_cbHighSpeed->setChecked(false);
        ui->m_cbStandard->setChecked(true);
    }
    if (m_memoryMap->m_mapa.m_impresion.bReverse){
        ui->m_cbReverse->setChecked(true);
        ui->m_cbNormal->setChecked(false);
    }
    else{
        ui->m_cbReverse->setChecked(false);
        ui->m_cbNormal->setChecked(true);
    }
    if (m_memoryMap->m_mapa.m_impresion.bMonitorHead) ui->m_cbMonitorHeads->setChecked(true);
    else ui->m_cbMonitorHeads->setChecked(false);

    if (m_memoryMap->m_mapa.m_impresion.m_fltrMacula.bMonitorMacula) ui->m_cbMonitorMacula->setChecked(true);
    else ui->m_cbMonitorMacula->setChecked(false);

    if (m_memoryMap->m_mapa.m_impresion.bMonitorVision) ui->m_cbMonitorVision->setChecked(true);
    else ui->m_cbMonitorVision->setChecked(false);


    params=QStringList()<<"XUSB"<<"HeadOffsetRegistersCard1"<<"GlobalOffset";
    val=parser.getValue(params,"value");
    float foo=val.toFloat()/14.18;
    ui->m_leOffsetPoints->setText(val);
    ui->m_leOffsetMm->setText(QString("%1").arg(foo));

    ClienteOPC *opc=ClienteOPC::request(0);
    m_Maculas=opc->LeerDato(ITEM_NUMERO_MACULAS).toInt();
    ui->m_leNMaculas->setText(QString("%1").arg(m_Maculas));

    InkjetDebug *dbg=InkjetDebug::request(0);
    connect(this,SIGNAL(SendDebugInfo(unsigned char,QString)),dbg,SLOT(ProcessMsg(unsigned char,QString)));
    connect(ui->m_leOffsetMm,SIGNAL(editingFinished()),this,SLOT(sltUpdateOffsetPoints()));
    connect(ui->m_leOffsetPoints,SIGNAL(editingFinished()),this,SLOT(sltUpdateOffsetMm()));
}

/** Cambia el offset en mm*/
void In2ConfigPrint::sltUpdateOffsetMm(){
    int foo=ui->m_leOffsetPoints->text().toInt();
    float val=foo*14.18;
    ui->m_leOffsetMm->setText(QString("%1").arg(val));
}
/** Cambia el offset en puntos*/
void In2ConfigPrint::sltUpdateOffsetPoints(){
    float foo=ui->m_leOffsetMm->text().toFloat();
    int val=(int)(foo/14.18);
    ui->m_leOffsetPoints->setText(QString("%1").arg(val));
}

/** Chequea la condfiguracion*/
void In2ConfigPrint::sltCheckConfig(){
    if (ui->m_cbReverse->isChecked()){
        ui->m_cbTipoPD->setCurrentIndex(0);
        ui->m_cbModoImpresion->setCurrentIndex(0);
    }
}

/** Guarda las cosas y sale*/
void In2ConfigPrint::sltSaveAndExit(){
    if (ui->m_cbGrayScale->isChecked()){
        m_memoryMap->m_mapa.m_impresion.bEscalaGrises=true;
    }
    else{
        m_memoryMap->m_mapa.m_impresion.bEscalaGrises=false;
    }
    if (ui->m_cbHighSpeed->isChecked()){
        m_memoryMap->m_mapa.m_impresion.bHighSpeed=true;
    }
    else{
        m_memoryMap->m_mapa.m_impresion.bHighSpeed=false;
    }
    if (ui->m_cbReverse->isChecked()){
        m_memoryMap->m_mapa.m_impresion.bReverse=true;

    }
    else{
        m_memoryMap->m_mapa.m_impresion.bReverse=false;
    }
    if (ui->m_cbMonitorHeads->isChecked()) m_memoryMap->m_mapa.m_impresion.bMonitorHead=true;
    else m_memoryMap->m_mapa.m_impresion.bMonitorHead=false;


    if (ui->m_cbMonitorMacula->isChecked()) m_memoryMap->m_mapa.m_impresion.m_fltrMacula.bMonitorMacula=true;
    else m_memoryMap->m_mapa.m_impresion.m_fltrMacula.bMonitorMacula=false;

    if (ui->m_cbMonitorVision->isChecked()) m_memoryMap->m_mapa.m_impresion.bMonitorVision=true;
    else  m_memoryMap->m_mapa.m_impresion.bMonitorVision=false;


    GestorImpresion *printEngine=GestorImpresion::request(this);
    QString configFile=printEngine->getConfigFile();
    In2XmlParser parser(0,configFile);
    QStringList params=QStringList()<<"XUSB"<<"Configuration"<<"PrintMode";
    QString val=QString("%1").arg(ui->m_cbModoImpresion->currentIndex());
    parser.setValue(params,"value",val);
    params=QStringList()<<"XUSB"<<"Configuration"<<"SequenceStart";
    val=QString("%1").arg(ui->m_cbTipoPD->currentIndex());
    parser.setValue(params,"value",val);    
    params=QStringList()<<"XUSB"<<"HeadOffsetRegistersCard1"<<"GlobalOffset";
    val=QString("%1").arg(ui->m_leOffsetPoints->text());
    parser.setValue(params,"value",val);

    parser.saveContentsToFile(configFile);


    int fooMaculas=ui->m_leNMaculas->text().toInt();
    m_memoryMap->m_mapa.m_maquina.iContadorMaculas=fooMaculas;
    double height=m_memoryMap->m_mapa.m_variables.WebHeight;
    if (fooMaculas!=m_Maculas){
        ClienteOPC *opc=ClienteOPC::request(0);
        opc->EscribirDato(ITEM_NUMERO_MACULAS,QVariant(fooMaculas));
        m_memoryMap->m_mapa.m_impresion.m_fltrMacula.fDistancia=height*fooMaculas;
        m_memoryMap->m_mapa.m_impresion.m_fltrMacula.fMax=m_memoryMap->m_mapa.m_impresion.m_fltrMacula.fDistancia+m_memoryMap->m_mapa.m_impresion.m_fltrMacula.fError;
        m_memoryMap->m_mapa.m_impresion.m_fltrMacula.fMin=m_memoryMap->m_mapa.m_impresion.m_fltrMacula.fDistancia-m_memoryMap->m_mapa.m_impresion.m_fltrMacula.fError;
        QString txt=QString("In2ConfigPrint:MaculaCount=%1;Dist=%2;Max=%3,Min=%4").arg(fooMaculas)
                .arg(m_memoryMap->m_mapa.m_impresion.m_fltrMacula.fDistancia)
                .arg(m_memoryMap->m_mapa.m_impresion.m_fltrMacula.fMax)
                .arg(m_memoryMap->m_mapa.m_impresion.m_fltrMacula.fMin);
        emit SendDebugInfo(LOGGER_STORE,txt);

    }
    this->deleteLater();
}

