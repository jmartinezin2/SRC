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
#include "in2esteveerp.h"
#include "in2defines.h"
#include "in2managerreport.h"
#include <QThread>
#include <QTimer>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include "in2store.h"
#include "in2gestorerpdaemon.h"
#include "MemoryMap.h"
//#include "in2gestormaquina.h"
#include "in2compofile.h"
#include "in2configuratorerp.h"
#include "in2estevepending.h"
#include <QVariantList>
#include <QVariant>
#ifndef IN2DESIGNER
    #include "in2managerdiametros.h"
#endif

//Una sola instancia
In2EsteveERP *In2EsteveERP::mInstance  = NULL;

//Tiempo de actualizacion
//#define UPDATE_TIME 10000

/** Lllamda a instancia unica*/
In2EsteveERP *In2EsteveERP::request(QObject *parent)
{
    if(mInstance == NULL){
        mInstance = new In2EsteveERP(parent);
    }
    return mInstance;
}

In2EsteveERP::In2EsteveERP(QObject *parent) :
    QObject(parent)
{
    //Le asigno el gestor de ERP
    for (int x=0;x<MAX_OUT_ELEMENTS;x++) m_dataOut.append(QString());
    for (int x=0;x<MAX_IN_ELEMENTS;x++) m_dataIn.append(QString());
    m_gestorErp=In2GestorERP::request(this);
    m_pendingOrder=false;
    In2ConfiguratorERP config;
    config.readConfig();
    QString ID=config.getID();
    m_dataOut.replace(out_IMPRESORA_ID,ID);
}

bool In2EsteveERP::orderPending(){
    In2EstevePending sglPending;
    return sglPending.orderPending();
}
QStringList In2EsteveERP::lstOrderPending(){
    In2EstevePending sglPending;
    return sglPending.getOrder();
}

/** Procesa los datos de entrada*/
void In2EsteveERP::processDataIn(){
    QStringList data=m_gestorErp->getLastRequest();
    setDataIn(data);
}
/** Carga una orden pendiente*/
void In2EsteveERP::loadPendingOrder(){
    //Testeo si hay algo pending
    In2EstevePending sglPending;
    if (sglPending.orderPending()){
        QStringList data=sglPending.getOrder();
        if (data.count()==MAX_IN_ELEMENTS) _loadDatum(data);
        m_pendingOrder=true;

    }    
    _loadDataToMemoryMap();
}

/** Carga datos al mapa de memoria*/
void In2EsteveERP::_loadDataToMemoryMap(){
    QString error;
    MemoryMap *memoryMap=MemoryMap::request(0);
    //Se chequea metros
    int workLoaded=SGL_WORKLOADED_NOK;
    memoryMap->m_mapa.m_variables.Proceso=m_dataIn.at(in_COD_ORDEN);
    memoryMap->m_mapa.m_variables.OrdenFabricacion=m_dataIn.at(in_TAREA_ID);
    QString fileIni=QString("%1/%2.ini").arg(m_dataIn.at(in_PATH_FICHERO_INI)).arg(m_dataIn.at(in_COD_PRODUCTO));
    if (/*QFile::exists(fileIni)*/1){
        memoryMap->m_mapa.m_variables.NombreRutaIni=fileIni;
        memoryMap->m_FilePath=memoryMap->m_mapa.m_variables.NombreRutaIni;
        memoryMap->m_FilePath=memoryMap->m_FilePath.replace(QString(".ini"),QString("_OF%1_C1_comp.bmp").arg(memoryMap->m_mapa.m_variables.OrdenFabricacion));
        //No necesito testear que existe
        if (/*QFile::exists(memoryMap->m_FilePath*/1){
            QFileInfo info(memoryMap->m_mapa.m_variables.NombreRutaIni);
            memoryMap->m_mapa.m_variables.NombreIni=info.fileName();
            memoryMap->m_mapa.m_variables.RutaIni=info.path();
            memoryMap->m_mapa.m_variables.RutaBmp=memoryMap->m_mapa.m_variables.RutaIni;
            memoryMap->m_mapa.m_variables.NombreRutaBmp=memoryMap->m_mapa.m_variables.NombreRutaIni;
            memoryMap->m_mapa.m_variables.NombreRutaBmp.replace(".ini",".bmp");

            In2CompoFile compoFile;
            compoFile.AbrirFicheroIni();

            In2ConfiguratorERP config;
            config.readConfig();
            int index=memoryMap->getIndexFixedData(config.getFieldLote());
            if (index!=-1){
                memoryMap->m_mapa.m_campos[index].ValorActualCampo=m_dataIn.at(in_LOTE_LDE);
            }
            index=memoryMap->getIndexFixedData(config.getFieldFchCad());
            if (index!=-1){                                                
                QString foo=m_dataIn.at(in_FCH_CADUCIDAD);
                QDateTime fooDateTime=formatDateTime(foo);
                if (!fooDateTime.isNull()){
                    QString format=memoryMap->m_mapa.m_campos[index].m_dateTimeParameters.formato;
                    if (!format.isEmpty()){
                        memoryMap->m_mapa.m_campos[index].ValorActualCampo=fooDateTime.toString(format);
                        if(memoryMap->m_mapa.m_campos[index].ValorActualCampo.isEmpty()){
                        }
                    }
                    else{
                        /*error=tr("ERROR-Formato de fecha de caducidad desconocido");
                        m_dataIn.replace(in_TEXTO_ERROR,error);
                        emit SendDebugInfo(LOGGER_ERROR,error);
                        emit sgnERPShowLog();
                        workLoaded=SGL_WORKLOADED_NOK;
                        emit sgnERPWorkLoaded(workLoaded);
                        return;*/
                    }

                }
            }
            index=memoryMap->getIndexFixedData(config.getFieldFchFab());
            if (index!=-1){
                QString foo=m_dataIn.at(in_FCH_FABRICACION);
                QDateTime fooDateTime=formatDateTime(foo);
                if (!fooDateTime.isNull()){
                    QString format=memoryMap->m_mapa.m_campos[index].m_dateTimeParameters.formato;
                    if (!format.isEmpty()){
                        memoryMap->m_mapa.m_campos[index].ValorActualCampo=fooDateTime.toString(format);
                        if(memoryMap->m_mapa.m_campos[index].ValorActualCampo.isEmpty()){
                        }
                    }
                    else{
                    }
                }
            }
            if (!m_dataIn.at(in_TIPO_TAREA).compare(SGL_TAREA_IMPRIMIR,Qt::CaseInsensitive)){
                if (!QFile::exists(fileIni)){
                    error=tr("SGL:Imprimir->No existe %1").arg(memoryMap->m_FilePath);
                    emit SendDebugInfo(LOGGER_ERROR,error);
                    emit sgnERPShowLog();
                    workLoaded=SGL_WORKLOADED_NOK;
                }

                else{
                    bool validate=false;
                    int foo=m_dataIn.at(in_METROS_IMPRIMIR).toInt(&validate);
                    if (validate){
                        emit sgnERPNewMetrosRef(foo);
                        if (!m_pendingOrder)
                            emit sgnERPTrabajoNuevo();
                    }
                }

                if (!m_dataIn.at(in_COD_BULTO_PADRE).isEmpty())
                        workLoaded=SGL_WORKLOADED_SCAN;
                    else{
                        workLoaded=SGL_WORKLOADED_OK;
                     }

            }
            else if (!m_dataIn.at(in_TIPO_TAREA).compare(SGL_TAREA_DOCUMENTAR,Qt::CaseInsensitive)) workLoaded=SGL_WORKLOADED_DOC;
            else workLoaded=SGL_WORKLOADED_NOK;
            //Creamos una entrada en la BBDD

        }
        else{
            error=tr("SGL:File+OT->No existe %1").arg(memoryMap->m_FilePath);
            emit SendDebugInfo(LOGGER_ERROR,error);
            emit sgnERPShowLog();
            //QMessageBox::warning(this,"ERROR",error,QMessageBox::Ok);
        }
    }
    else{
        error=tr("SGL:File->No existe %1").arg(fileIni);
        emit SendDebugInfo(LOGGER_ERROR,error);
        emit sgnERPShowLog();
        //QMessageBox::warning(this,"ERROR",error,QMessageBox::Ok);
    }

    emit sgnERPWorkLoaded(workLoaded);

}
/** Formatea el DateTime que viene de SGL*/
QDateTime In2EsteveERP::formatDateTime(QString foo){
    QDateTime fooDateTime;
    QStringList lst=foo.split("-");
    if (lst.count()==3){
        int year=lst.at(0).toInt();
        int month=lst.at(1).toInt();
        QStringList lstext=lst.at(2).split("T");
        if (lstext.count()==2){
            int day=lstext.at(0).toInt();
            QStringList lstTime=lstext.at(1).split(":");
            if (lstTime.count()==3){
                int hour=lstTime.at(0).toInt();
                int min=lstTime.at(1).toInt();
                int sg=lstTime.at(2).toInt();
                //Si llego aqui, tengo de todo
                fooDateTime=QDateTime(QDate(year,month,day));
                fooDateTime.addSecs(hour*3600+min*60+sg);
            }
        }
    }
    return fooDateTime;
}


/** Guarda los datos de entrada*/
bool In2EsteveERP::setDataIn(const QStringList & data){
    bool rtn=true;
    if (data.count()==MAX_IN_ELEMENTS){
        QString state=data.at(in_ESTADO);
        if (!state.compare(SGL_STATE_PND)){
            readSGLPending(data);
        }
        else if (!state.compare(SGL_STATE_FIN)){
            readSGLFin(data);
            emit sgnERPWorkLoaded(SGL_WORKLOADED_NOK);
        }
        else if (!state.compare(SGL_STATE_ERROR)){
            readSGLError(data);
        }
        else{
            QString error=tr("ERP:Estado ESTADO %1").arg(m_dataIn.at(in_ESTADO));
            emit SendDebugInfo(LOGGER_ERROR,error);
            emit sgnERPShowLog();
        }
    }
    else{
        rtn=false;
        QString error=tr("ERP:Expected %1 items, got %2").arg(MAX_IN_ELEMENTS).arg(data.count());
        emit SendDebugInfo(LOGGER_ERROR,error);
        emit sgnERPShowLog();
    }
    return rtn;
}
/** Guarda los datos de salida*/
bool In2EsteveERP::setDataOut(QStringList & data){
    bool rtn=true;
    if (data.count()==MAX_OUT_ELEMENTS){
        m_dataOut=data;
    }
    else rtn=false;
    return rtn;
}

/** Devuelve un item de la entrada*/
QString In2EsteveERP::getItemIn(int item){
    QString rtn;
    if (item<m_dataIn.count()){
        rtn=m_dataIn.at(item);
    }
    return rtn;
}
/** Devuelve un item de la salida*/
QString In2EsteveERP::getItemOut(int item){
    QString rtn;
    if (item<m_dataOut.count()){
        rtn=m_dataOut.at(item);
    }
    return rtn;
}
/** Cambia un item de la entrada*/
bool In2EsteveERP::setItemIn(int index,QString item){
    bool rtn=false;
    if (index<m_dataIn.count()){
        m_dataIn.replace(index,item);
        rtn=true;
    }
    return rtn;

}
/** Cambia un item de la salida*/
bool In2EsteveERP::setItemOut(int index,QString item){
    bool rtn=false;
    if (index<m_dataOut.count()){
        m_dataOut.replace(index,item);
        rtn=true;
    }
    return rtn;
}

/** Chequea el ERP*/
QStringList In2EsteveERP::checkERP(){
    QStringList data=m_gestorErp->checkERP();
    return data;
}



/** Carga datos*/
void In2EsteveERP::_loadDatum(QStringList lst){
    m_dataIn=lst;
    m_dataOut.replace(out_MSG_ID,m_dataIn.at(in_MSG_ID));
    m_dataOut.replace(out_TAREA_ID,m_dataIn.at(in_TAREA_ID));
    m_dataOut.replace(out_FCH_CREACION,m_dataIn.at(in_FCH_CREACION));
    m_dataOut.replace(out_FCH_PROCESO,m_dataIn.at(in_FCH_PROCESO));
    m_dataOut.replace(out_METROS_IMPRESOS,SGL_ZERO);
    m_dataOut.replace(out_METROS_GASTADOS,SGL_ZERO);
    m_dataOut.replace(out_IND_FIN_BULTO_PADRE,SGL_NO);
    m_dataOut.replace(out_IND_FIN_BULTO_HIJO,SGL_NO);
    m_dataOut.replace(out_IND_RECHAZO,SGL_NO);
    m_dataOut.replace(out_IND_FORMATO_PERSONALIZADO,SGL_NO);
    m_dataOut.replace(out_ESTADO,SGL_STATE_PND);
    //m_dataOut.replace(out_IMPRESORA_ID,m_dataIn.at(in_IMPRESORA_ID));
}
/** Devuelve el ultimo error*/
QString In2EsteveERP::getLastError(){
    return m_lastError;
}

/** Ha leido que el SGL está pending*/
void In2EsteveERP::readSGLPending(const QStringList &lst){
    QString join=lst.join("@");
    emit SendDebugInfo(LOGGER_ERROR,QString("ERP Pending: %1").arg(join));
    _loadDatum(lst);
    m_pendingOrder=false;
    //Si todo está OK, cambiamos el estado
    QString error="";
    //Compruebo que existen ficheros
    QString fileIni=QString("%1/%2.ini").arg(m_dataIn.at(in_PATH_FICHERO_INI)).arg(m_dataIn.at(in_COD_PRODUCTO));
    MemoryMap *memoryMap=MemoryMap::request(0);
    QFileInfo infoFile(fileIni);
    QString ruta=infoFile.absolutePath();
    QString nombre=infoFile.fileName();
    QString nombreruta=fileIni;
    QString state=SGL_STATE_FIN;
    QString alu;
    alu.clear();
    memoryMap->m_mapa.m_variables.NombreIni=nombre;
    memoryMap->m_mapa.m_variables.RutaIni=ruta;
    memoryMap->m_mapa.m_variables.NombreRutaIni=nombreruta;
    memoryMap->m_mapa.m_variables.RutaBmp=ruta;
    memoryMap->m_mapa.m_variables.NombreRutaBmp=memoryMap->m_mapa.m_variables.NombreRutaIni;
    memoryMap->m_mapa.m_variables.NombreRutaBmp.replace(".ini",".bmp");

        if (!m_dataIn.at(in_TIPO_TAREA).compare(SGL_TAREA_DOCUMENTAR,Qt::CaseInsensitive)){
            saveOrder();            
            _sltGenerateReport();

        }
        else if (!m_dataIn.at(in_TIPO_TAREA).compare(SGL_TAREA_IMPRIMIR,Qt::CaseInsensitive)){
            if (!QFile::exists(fileIni)) error.append(tr("[No existe fichero %1]\n").arg(fileIni));
            //Compruebo que hay campos en esa composicion
            QString lote=m_dataIn.at(in_LOTE_LDE);
            QString cad=m_dataIn.at(in_FCH_CADUCIDAD);
            QString fab=m_dataIn.at(in_FCH_FABRICACION);
            alu=m_dataIn.at(in_COD_BULTO_PADRE);
            In2ConfiguratorERP config;
            config.readConfig();
            In2CompoFile compoFile;
            compoFile.AbrirFicheroIni();
            if (lote.isNull()){
                int index=memoryMap->getIndexFixedData(config.getFieldLote());
                if (index!=-1){
                    if(memoryMap->m_mapa.m_campos[index].OnOffCampo) error.append(tr("SGL ERROR: No hay Lote\n"));
                }
            }
            else{
                int index=memoryMap->getIndexFixedData(config.getFieldLote());
                if (index!=-1){
                    if(!memoryMap->m_mapa.m_campos[index].OnOffCampo) error.append(tr("SGL ERROR: Campo Lote no activado en composicion\n"));
                }
                else
                    error.append(tr("SGL ERROR: Campo Lote no definido en composicion\n"));
            }
            if (cad.isNull()){
                int index=memoryMap->getIndexFixedData(config.getFieldFchCad());
                if (index!=-1){
                    if(memoryMap->m_mapa.m_campos[index].OnOffCampo) error.append(tr("SGL ERROR: No hay Fecha de caducidad\n"));
                }
            }
            else{
                int index=memoryMap->getIndexFixedData(config.getFieldFchCad());
                if (index!=-1){
                    if(!memoryMap->m_mapa.m_campos[index].OnOffCampo) error.append(tr("SGL ERROR: Campo Fecha de caducidad no activada en Composicion\n"));
                }
                else
                    error.append(tr("SGL ERROR: Campo Caducidad no definido en composicion\n"));

            }
            if (fab.isNull()){
                int index=memoryMap->getIndexFixedData(config.getFieldFchFab());
                if (index!=-1){
                    if(memoryMap->m_mapa.m_campos[index].OnOffCampo) error.append(tr("SGL ERROR: No hay Fecha de fabricacion\n"));
                }
            }
            else{
                int index=memoryMap->getIndexFixedData(config.getFieldFchFab());
                if (index!=-1){
                    if(!memoryMap->m_mapa.m_campos[index].OnOffCampo) error.append(tr("SGL ERROR: Campo Fabricacion no activado en composicion\n"));
                }
                else
                    error.append(tr("SGL ERROR: Campo Fabricacion no definido en composicion\n"));
            }
            //Testeamos si se puede hacer empalme
            if (error.isEmpty()){
                state=SGL_STATE_FIN;
                saveOrder();
               _loadDataToMemoryMap();
            }
       }
       else error=tr("ERP:Tarea desconocida %1\n").arg(m_dataIn.at(in_TIPO_TAREA));

       if (!error.isEmpty()){
           /*QString state=SGL_STATE_ERROR;
           QString foo=QString("update %1 set %2='%3',%4='%5' where %6='%7'")
                   .arg(m_gestorErp->getTableIn())
                   .arg(m_gestorErp->getFieldEstado())
                   .arg(state)
                   .arg(m_gestorErp->getFieldError())
                   .arg(error)
                   .arg(m_gestorErp->getFieldMsg())
                   .arg(m_dataIn.at(in_MSG_ID));
           emit SendDebugInfo(LOGGER_STORE,foo);
           m_gestorErp->execSQL(foo,false);
           emit SendDebugInfo(LOGGER_ERROR,error);
           emit sgnERPShowLog();
           emit sgnERPWorkLoaded(SGL_WORKLOADED_NOK);*/
           sltWriteAnswerToBBDD(SGL_STATE_ERROR, error);
           //emit SendDebugInfo(LOGGER_ERROR,error);
           emit sgnERPShowLog();
           emit sgnERPWorkLoaded(SGL_WORKLOADED_NOK);
           emit sgnMessage(error);

       }
    else {
           if (alu.isEmpty()){ //Solo escribo fin si leo aluminio
            /*QString foo=QString("update %1 set %2='%3',%4='%5' where %6='%7'")
                        .arg(m_gestorErp->getTableIn())
                        .arg(m_gestorErp->getFieldEstado())
                        .arg(state)
                        .arg(m_gestorErp->getFieldError())
                        .arg(error)
                        .arg(m_gestorErp->getFieldMsg())
                        .arg(m_dataIn.at(in_MSG_ID));
                emit SendDebugInfo(LOGGER_STORE,foo);
                m_gestorErp->execSQL(foo,false);*/
               sltWriteAnswerToBBDD(SGL_STATE_FIN,"");
           }
    }
}
/** Comprueba las condiciones fisicas de maquina*/
bool In2EsteveERP::checkCondicionesFisicasEntrada(){
#ifndef IN2DESIGNER
    bool rtn=true;
    In2ManagerDiametros *managerDiam=In2ManagerDiametros::request(0);
    double diamSalida=0;
    //double diamEntrada=0;
    MemoryMap *memoryMap=MemoryMap::request(0);
    MonitorDiametro *p=managerDiam->getMonitor(IN2MANAGER_DOUT);
    if (p)
        diamSalida=p->getCurrentDiameter(0);
    /*p=managerDiam->getMonitor(IN2MANAGER_DIN);
    if (p)
        diamEntrada=p->getCurrentDiameter(0);*/
    QString foo=m_dataIn.at(in_IND_PERMITIR_EMPALME);
    m_lastError.clear();
    if ((!foo.compare(SGL_NO,Qt::CaseInsensitive))&&(diamSalida)){
        if (diamSalida>memoryMap->m_mapa.m_machinedefaults.DiametroMinimoBultoSalida){
            rtn=false;
            m_lastError.append(tr("SGL ERROR: No se permite empalme (D.Salida=%1,Max=%2)\n").arg(diamSalida).arg(memoryMap->m_mapa.m_machinedefaults.DiametroMinimoBultoSalida));
        }
    }
    else{
        if (diamSalida<memoryMap->m_mapa.m_machinedefaults.DiametroMinimoBultoSalida){
            rtn=false;
            m_lastError.append(tr("SGL ERROR: No hay bulto hijo (D.Salida=%1,Min=%2)\n").arg(diamSalida).arg(memoryMap->m_mapa.m_machinedefaults.DiametroMinimoBultoSalida));
        }
    }
    return rtn;
    #endif
}



/** Escribe fin en la BBDD*/
void In2EsteveERP::sltWriteAnswerToBBDD(QString state, QString Error){
    QString foo=QString("update %1 set %2='%3',%4='%5' where %6='%7'")
                .arg(m_gestorErp->getTableIn())
                .arg(m_gestorErp->getFieldEstado())
                .arg(state)
                .arg(m_gestorErp->getFieldError())
                .arg(Error)
                .arg(m_gestorErp->getFieldMsg())
                .arg(m_dataIn.at(in_MSG_ID));
        emit SendDebugInfo(LOGGER_STORE,foo);
        m_gestorErp->execSQL(foo,false);
}

/** Guarda una orden pendiente*/
void In2EsteveERP::saveOrder(){
    In2EstevePending sglPending;
    sglPending.saveOrder(m_dataIn);
}
/** Elimina una orden pendiente*/
void In2EsteveERP::deleteOrder(){
    In2EstevePending sglPending;
    sglPending.deletePending();
}


/** Ejecuta un sql*/
QStringList In2EsteveERP::execSQL(QString select, bool save){
    return m_gestorErp->execSQL(select,save);
}

/** Genera un report*/
void In2EsteveERP::_sltGenerateReport(){
    _loadDataToMemoryMap();
    emit SendDebugInfo(LOGGER_STORE,tr("REPORT REQUEST"));
    MemoryMap *memoryMap=MemoryMap::request(0);
    QString foo=QString("select Ini from OrdenVsIni where Orden='%1'").arg(m_dataIn.at(in_COD_ORDEN));
    In2Store db;
    QStringList rslt=db.execSql(foo);
    if (rslt.count()) memoryMap->m_mapa.m_variables.NombreRutaIni=rslt.at(0);
    emit SendDebugInfo(LOGGER_WARNING,QString("_sltGenerateReport%: 1: resultados %2 [%3]").arg(foo).arg(rslt.count()).arg(memoryMap->m_mapa.m_variables.NombreRutaIni));
    //Cogermos el bmp general
    memoryMap->m_FilePath=memoryMap->m_mapa.m_variables.NombreRutaIni;
    memoryMap->m_FilePath=memoryMap->m_FilePath.replace(QString(".ini"),QString("_OF_C1_comp.bmp").arg(memoryMap->m_mapa.m_variables.OrdenFabricacion));


    In2ManagerReport *report=new In2ManagerReport(0);
    In2Trabajo p;
    p.bActive=false;
    report->setTrabajo(p);
    QThread *t=new QThread;
    t->start();
    report->sltSetOutputName(QString("%1.pdf").arg(m_dataIn.at(in_COD_ORDEN)));
    connect(report,SIGNAL(In2ReportFinished()),report,SLOT(deleteLater()));
    connect(report,SIGNAL(In2ReportFinished()),t,SLOT(deleteLater()));
    connect(report,SIGNAL(In2NewReportGenerated(QString)),this,SLOT(sltFinDocumentacion(QString)));
    connect(report,SIGNAL(In2ReportDataBaseData(QString)),this,SLOT(sltWriteDatabaseData(QString)));
    /*connect(report,SIGNAL(In2SignedReportFinished()),this,SLOT(sltFinDocumentacion()));
    connect(report,SIGNAL(In2ReportFailed(QString)),this,SLOT(sltFinFailDocumentacion(QString)));*/

    report->moveToThread(t);
    QTimer::singleShot(0,report,SLOT(sltGenerateReport()));
}
/** Escribe en la BBDD de reports*/
void In2EsteveERP::sltWriteDatabaseData(QString  lst){
    In2GestorERP *p=In2GestorERP::request(0);
    QString foo=QString("update %1 ").arg(p->getTableOut());
    QString data=lst;    
    //foo=QString("insert into IN2_TO_SGL_LOG (COD_ORDEN,DATOS) values ('%1',to_clob('%2'))").arg(m_dataIn.at(in_COD_ORDEN)).arg(data);
    foo=QString("insert into IN2_TO_SGL_LOG (COD_ORDEN,DATOS) values ('%1',?)").arg(m_dataIn.at(in_COD_ORDEN));
    QString txt=tr("ERP:%1").arg(foo);
    emit SendDebugInfo(LOGGER_WARNING,txt);
    QVariant variant=QVariant(data);
    QVariantList args;
    args.append(variant);

    p->execClob(foo,args);
    if (!p->getLastQueryResult()){
        txt=QString("Error ERP: sltWriteDataBaseData():%1").arg(foo);
        emit SendDebugInfo(LOGGER_STORE,txt);
        emit sgnERPWorkLoaded(SGL_WORKLOADED_NODOC);
    }
}


/** Ha leido que el SGL está en fin*/
void In2EsteveERP::readSGLFin(const QStringList &lst){
    _loadDatum(lst);
    emit SendDebugInfo(LOGGER_ERROR,QString("chekERP:State FIN"));
    emit sgnERPShowLog();
}

/** Ha leido que el SGL está en error*/
void In2EsteveERP::readSGLError(const QStringList &lst){
    _loadDatum(lst);
    emit SendDebugInfo(LOGGER_ERROR,QString("chekERP:State ERROR %1").arg(lst.at(in_TEXTO_ERROR)));
    emit sgnERPShowLog();

}

/** Ha terminado la documentacion*/
void In2EsteveERP::sltFinDocumentacion(QString name){
    m_dataOut.replace(out_ESTADO,SGL_STATE_FIN);
    writeIn2BBDD();
    QMessageBox::warning(0,"INFO",tr("Se ha generado report %1").arg(name),QMessageBox::Ok);
}

/** Ha terminado la documentacion con fallo*/
void In2EsteveERP::sltFinFailDocumentacion(QString & txt){

}

/** Ha terminado un empalme*/
void In2EsteveERP::sltFinEmpalmeEntrada(){
    MemoryMap *memoryMap=MemoryMap::request(0);
    QString personalizado=SGL_NO;
    QString txt=tr("Empalme bulto PADRE");
    for (int x=0; x<NUM_CAMPOS;x++){
        if (memoryMap->m_mapa.m_campos[x].OnOffCampo) personalizado=SGL_SI;
    }

    int iPrinted=(memoryMap->m_mapa.m_variables.WebHeight*memoryMap->m_mapa.m_maquina.iCounter)/1000;
    int iUsed=memoryMap->m_mapa.m_maquina.metros_trabajo;
    if (iPrinted<iUsed*CORRECION_METROS) iPrinted=iUsed*CORRECION_METROS;

    m_dataOut.replace(out_IND_FIN_BULTO_HIJO,SGL_NO);
    m_dataOut.replace(out_IND_FIN_BULTO_PADRE,SGL_SI);
    m_dataOut.replace(out_ESTADO,SGL_STATE_PND);
    m_dataOut.replace(out_METROS_GASTADOS,QString("%1").arg(iUsed));
    m_dataOut.replace(out_METROS_IMPRESOS,QString("%1").arg(iPrinted));
    m_dataOut.replace(out_IND_FORMATO_PERSONALIZADO,personalizado);
    m_dataOut.replace(out_TEXTO_OBSERVACIONES,txt);
    writeIn2BBDD();

}

/** Ha terminado un empalme*/
void In2EsteveERP::sltFinEmpalmeSalida(){
    MemoryMap *memoryMap=MemoryMap::request(0);
    QString personalizado=SGL_NO;
    QString txt=tr("Empalme bulto HIJO");
    for (int x=0; x<NUM_CAMPOS;x++){
        if (memoryMap->m_mapa.m_campos[x].OnOffCampo) personalizado=SGL_SI;
    }

    int iPrinted=(memoryMap->m_mapa.m_variables.WebHeight*memoryMap->m_mapa.m_maquina.iCounter)/1000;
    int iUsed=memoryMap->m_mapa.m_maquina.metros_trabajo;
    if (iPrinted<iUsed*CORRECION_METROS) iPrinted=iUsed*CORRECION_METROS;


    m_dataOut.replace(out_IND_FIN_BULTO_HIJO,SGL_SI);
    m_dataOut.replace(out_IND_FIN_BULTO_PADRE,SGL_NO);
    m_dataOut.replace(out_ESTADO,SGL_STATE_PND);
    m_dataOut.replace(out_METROS_GASTADOS,QString("%1").arg(iUsed));
    m_dataOut.replace(out_METROS_IMPRESOS,QString("%1").arg(iPrinted));
    m_dataOut.replace(out_IND_FORMATO_PERSONALIZADO,personalizado);
    m_dataOut.replace(out_TEXTO_OBSERVACIONES,txt);
    writeIn2BBDD();

}

/** Ha terminado la documentacion*/
void In2EsteveERP::sltFinImpresion(){
    MemoryMap *memoryMap=MemoryMap::request(0);
    bool bWriteSGL=false;

    //In2GestorMaquina *maquina=In2GestorMaquina::request(0);
    QString txt;
    int mode=memoryMap->m_mapa.m_maquina.modoParo;
    unsigned char cCodigoParada=(unsigned char)((memoryMap->m_mapa.m_maquina.codigo_parada&0xFF00)>>8);
    unsigned char cCodigoParada2=(unsigned char)(((char)memoryMap->m_mapa.m_maquina.codigo_parada&0x00FF));
    unsigned int code=cCodigoParada+(cCodigoParada2*256);
    switch (mode){
    case FIN_EMERGENCIA:
        txt=QString("Parada:Emergencia [%1]").arg(code);
        if (code&EMERGENCIA_SETA) txt.append(tr("[SETA]"));
        if (code&EMERGENCIA_VARSALIDA) txt.append(tr("[VARSALIDA]"));
        if (code&EMERGENCIA_VARENTRADA) txt.append(tr("[VARENTRADA]"));
        if (code&EMERGENCIA_ARRASTRE) txt.append(tr("[ARRASTRE]"));
        if (code&EMERGENCIA_BANDEJA) txt.append(tr("[BANDEJA]"));
        if (code&EMERGENCIA_MESA) txt.append(tr("[MESA]"));
        if (code&EMERGENCIA_CAPOTA) txt.append(tr("[CAPOTA]"));
        if (code&EMERGENCIA_PRESION) txt.append(tr("[PRESION AIRE]"));
        if (code&EMERGENCIA_LAMPARA_ELECTRICO) txt.append(tr("[LAMPARA.FALLO ELECTRICO]"));
        if (code&EMERGENCIA_LAMPARA_APAGADA) txt.append(tr("[LAMPARA APAGADA]"));
        if (code&EMERGENCIA_DESPLAZADOR_LAMPARA) txt.append(tr("[DESPLAZADOR LAMPARA]"));

        break;
    case FIN_LAMPARA:
        txt=QString("Parada:Temperatura Lámpara");
        break;
    case FIN_SAI:
        txt=QString("Parada: SAI");
        break;
    case FIN_PULSADOR:
        txt=QString("Parada: Pulsador");
        break;
    case FIN_METROS:
        txt=QString("Parada: Metros");
        m_dataOut.replace(out_IND_FIN_BULTO_HIJO,SGL_SI);
        m_dataOut.replace(out_IND_FIN_BULTO_PADRE,SGL_NO);
        m_dataOut.replace(out_IND_RECHAZO,SGL_NO);
        bWriteSGL=true;
        break;
    case FIN_DIAMETRO:
        txt=QString("Parada: Diametro");
        m_dataOut.replace(out_IND_FIN_BULTO_PADRE,SGL_SI);
        m_dataOut.replace(out_IND_FIN_BULTO_HIJO,SGL_NO);
        m_dataOut.replace(out_IND_RECHAZO,SGL_NO);

        /*if (!m_dataIn.at(in_IND_PERMITIR_EMPALME).compare(SGL_SI))
            emit sgnERPLaunchEmpalme();*/
        bWriteSGL=true;
        break;
     case FIN_ELECTRONICA:
        txt=QString("Parada: Electrónica");
        break;
     case FIN_CABEZALES:
        txt=QString("Parada: Cabezales");
        break;
    }
    QString personalizado=SGL_NO;
    for (int x=0; x<NUM_CAMPOS;x++){
        if (memoryMap->m_mapa.m_campos[x].OnOffCampo) personalizado=SGL_SI;
    }


    if (bWriteSGL){
        int iPrinted=(memoryMap->m_mapa.m_variables.WebHeight*memoryMap->m_mapa.m_maquina.iCounter)/1000;
        int iUsed=memoryMap->m_mapa.m_maquina.metros_trabajo;
        if (iPrinted<iUsed*CORRECION_METROS) iPrinted=iUsed*CORRECION_METROS;


        m_dataOut.replace(out_ESTADO,SGL_STATE_PND);
        m_dataOut.replace(out_METROS_GASTADOS,QString("%1").arg(iUsed));
        m_dataOut.replace(out_METROS_IMPRESOS,QString("%1").arg(iPrinted));
    //emit SendDebugInfo(LOGGER_WARNING,QString("wh%1 - c%2").arg(memoryMap->m_mapa.m_variables.WebHeight).arg(memoryMap->m_mapa.m_maquina.iCounter));
        m_dataOut.replace(out_IND_FORMATO_PERSONALIZADO,personalizado);
        m_dataOut.replace(out_TEXTO_OBSERVACIONES,txt);
        writeIn2BBDD();
    }

}

/** Escribe resultado en la BBDD de salida*/
void In2EsteveERP::writeIn2BBDD(){
    In2GestorERP *p=In2GestorERP::request(0);
    QStringList fields=m_gestorErp->getFieldsOut().split("@");
    //Solo cuando he terminado la orden, la elimino
    deleteOrder();
    if (fields.count()==m_dataOut.count()){
        //QString foo=QString("update %1 ").arg(p->getTableOut());
        QString foo=QString("insert into %1 (%2,%3,%4,%5,%6,%7,%8,%9,%10,%11) values ('%12','%13','%14','%15','%16','%17','%18','%19','%20','%21') ")
               .arg(p->getTableOut())
               .arg(fields.at(1))
               .arg(fields.at(2))
               .arg(fields.at(3))
               .arg(fields.at(4))
               .arg(fields.at(5))
               .arg(fields.at(6))
               .arg(fields.at(7))
               .arg(fields.at(8))
               .arg(fields.at(9))
               .arg(fields.at(12))
               .arg(m_dataOut.at(out_TAREA_ID))
               .arg(m_dataOut.at(out_METROS_IMPRESOS))               
               .arg(m_dataOut.at(out_METROS_GASTADOS))               
               .arg(m_dataOut.at(out_IND_FIN_BULTO_PADRE))               
               .arg(m_dataOut.at(out_IND_FIN_BULTO_HIJO))               
               .arg(m_dataOut.at(out_IND_RECHAZO))               
               .arg(m_dataOut.at(out_IND_FORMATO_PERSONALIZADO))               
               .arg(m_dataOut.at(out_TEXTO_OBSERVACIONES))               
               .arg(m_dataOut.at(out_ESTADO))               
               .arg(m_dataOut.at(out_IMPRESORA_ID));

        p->execSQL(foo,false);
        emit SendDebugInfo(LOGGER_STORE,foo);
        if (!p->getLastQueryResult()){
            emit SendDebugInfo(LOGGER_ERROR,QString("writeIn2BBDD:Error"));
        }
    }
    else{
        emit SendDebugInfo(LOGGER_ERROR,QString("writeIn2BBDD:Error Fields %1, Data %2")
                           .arg(fields.count()).arg(m_dataOut.count()));
        emit sgnERPShowLog();

    }
}

/** Escribe resultado en la BBDD de salida*/
void In2EsteveERP::SimulatewriteIn2BBDD(){
    In2GestorERP *p=In2GestorERP::request(0);
    QStringList fields=m_gestorErp->getFieldsOut().split("@");
    //Solo cuando he terminado la orden, la elimino
    deleteOrder();
    if (fields.count()==m_dataOut.count()){
        //QString foo=QString("update %1 ").arg(p->getTableOut());
        QString foo=QString("[SIMULATE] insert into %1 (%2,%3,%4,%5,%6,%7,%8,%9,%10,%11) values ('%12','%13','%14','%15','%16','%17','%18','%19','%20','%21') ")
               .arg(p->getTableOut())
               .arg(fields.at(1))
               .arg(fields.at(2))
               .arg(fields.at(3))
               .arg(fields.at(4))
               .arg(fields.at(5))
               .arg(fields.at(6))
               .arg(fields.at(7))
               .arg(fields.at(8))
               .arg(fields.at(9))
               .arg(fields.at(12))
               .arg(m_dataOut.at(out_TAREA_ID))
               .arg(m_dataOut.at(out_METROS_IMPRESOS))
               .arg(m_dataOut.at(out_METROS_GASTADOS))
               .arg(m_dataOut.at(out_IND_FIN_BULTO_PADRE))
               .arg(m_dataOut.at(out_IND_FIN_BULTO_HIJO))
               .arg(m_dataOut.at(out_IND_RECHAZO))
               .arg(m_dataOut.at(out_IND_FORMATO_PERSONALIZADO))
               .arg(m_dataOut.at(out_TEXTO_OBSERVACIONES))
               .arg(m_dataOut.at(out_ESTADO))
               .arg(m_dataOut.at(out_IMPRESORA_ID));

        //p->execSQL(foo,false);
        emit SendDebugInfo(LOGGER_STORE,foo);
       /* if (!p->getLastQueryResult()){
            emit SendDebugInfo(LOGGER_ERROR,QString("writeIn2BBDD:Error"));
        }*/
    }
    else{
        emit SendDebugInfo(LOGGER_ERROR,QString("writeIn2BBDD:Error Fields %1, Data %2")
                           .arg(fields.count()).arg(m_dataOut.count()));
        emit sgnERPShowLog();

    }
}


