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
#include "in2managererp.h"
#include "in2configuratorerp.h"
#include "in2esteveerp.h"

//Una sola instancia
In2ManagerERP *In2ManagerERP::mInstance  = NULL;


In2ManagerERP::In2ManagerERP(QObject *parent) :
    QObject(parent)
{
    config.readConfig();
    m_Cliente=config.getClient();
    m_In2erp=NULL;
    m_GramarErp=NULL;
    m_In2erp=NULL;
    if (!m_Cliente.compare(ERP_ESTEVE)){
        m_SGL=In2EsteveERP::request(this);
        connect(m_SGL,SIGNAL(sgnERPWorkLoaded(int)),this,SIGNAL(sgnERPWorkLoaded(int)));
        connect(m_SGL,SIGNAL(SendDebugInfo(unsigned char,QString)),this,SIGNAL(SendDebugInfo(unsigned char,QString)));
        connect(m_SGL,SIGNAL(sgnERPNewMetrosRef(int)),this,SIGNAL(sgnERPNewMetrosRef(int)));
        connect(m_SGL,SIGNAL(sgnERPTrabajoNuevo()),this,SIGNAL(sgnERPTrabajoNuevo()));
        connect(m_SGL,SIGNAL(sgnERPShowLog()),this,SIGNAL(sgnERPShowLog()));
        connect(m_SGL,SIGNAL(sgnERPLaunchEmpalme()),this,SIGNAL(sgnERPLaunchEmpalme()));
        connect(m_SGL,SIGNAL(sgnMessage(QString)),this,SIGNAL(sgnMessage(QString)));

    }
    else if (!m_Cliente.compare(ERP_GRAMAR)){ //Gramar e In2 usan la misma estructura
        m_In2erp=In2ERP::request(this);
        connect(m_In2erp,SIGNAL(SendDebugInfo(unsigned char,QString)),this,SIGNAL(SendDebugInfo(unsigned char,QString)));
        connect(m_In2erp,SIGNAL(sgnERPNewMetrosRef(int)),this,SIGNAL(sgnERPNewMetrosRef(int)));
    }
    else if (!m_Cliente.compare(ERP_IN2)){
        m_In2erp=In2ERP::request(this);
        connect(m_In2erp,SIGNAL(SendDebugInfo(unsigned char,QString)),this,SIGNAL(SendDebugInfo(unsigned char,QString)));
    }
}

/** Lllamda a instancia unica*/
In2ManagerERP *In2ManagerERP::request(QObject *parent)
{
    if(mInstance == NULL){
        mInstance = new In2ManagerERP(parent);
    }
    return mInstance;
}

/** Devuelve un item de configuracion*/
QString In2ManagerERP::getItem(QString item){
    if (!item.compare(ERP_CONNECTION)) return config.getConnection();
    else if (!item.compare(ERP_USER)) return config.getUser();
    else if (!item.compare(ERP_PASSWORD)) return config.getPassword();
    else if (!item.compare(ERP_CLIENTE))    return config.getClient();
    else if (!item.compare(ERP_ENABLED))    return QString("%1").arg(config.getEnabled());
    else if (!item.compare(ERP_DRIVER))     return config.getDriver();
    else if (!item.compare(ERP_ODBC_ENTRY)) return config.getODBCEntry();
    else if (!item.compare(ERP_BBDD))       return config.getBBDD();
    else if (!item.compare(ERP_WRITE_BBDD)) return config.getWriteBBDD();
    else if (!item.compare(ERP_FIELDS_BBDD_IN)) return config.getFieldsIn();
    else if (!item.compare(ERP_FIELDS_BBDD_OUT))    return config.getFieldsOut();
    else if (!item.compare(ERP_TABLE_IN))   return config.getInTable();
    else if (!item.compare(ERP_TABLE_OUT))  return config.getOutTable();
    else if (!item.compare(ERP_MULTIPLE_ITEMS)) return QString("%1").arg(config.getMultipleItems());
    else if (!item.compare(ERP_FIELD_COMPO))    return config.getFieldCompo();
    else if (!item.compare(ERP_FIELD_OT))       return config.getFieldOT();
    else if (!item.compare(ERP_FIELD_METROS))   return config.getFieldMetros();
    else if (!item.compare(ERP_FIELD_ESTADO))   return config.getFieldEstado();
    else if (!item.compare(ERP_FIELD_ERROR))    return config.getFieldError();
    else if (!item.compare(ERP_FIELD_TIPO_TAREA))   return config.getFieldTipoTarea();
    else return QString();
}
/** Indica si hay ordenes pendientes*/
bool In2ManagerERP::orderPending(){
    bool  rtn=false;
    if (!m_Cliente.compare(ERP_ESTEVE)){
        rtn=m_SGL->orderPending();
    }
    return rtn;
}
/** Indica si hay ordenes pendientes*/
QStringList In2ManagerERP::lstOrderPending(){
    QStringList  lst;
    if (!m_Cliente.compare(ERP_ESTEVE)){
        lst=m_SGL->lstOrderPending();
    }
    return lst;
}

/** Carga una orden pendiente*/
void In2ManagerERP::loadPendingOrder(){
    if (!m_Cliente.compare(ERP_ESTEVE)){
        m_SGL->loadPendingOrder();
    }
}


/** Devuelve el indice de la BBDD de entrada del campo deseado*/
int In2ManagerERP::getItemIndex(QString item){
    int rtn=-1;
    if (!item.compare(ERP_CONNECTION));
    else if (!item.compare(ERP_USER));
    else if (!item.compare(ERP_PASSWORD));
    else if (!item.compare(ERP_CLIENTE));
    else if (!item.compare(ERP_ENABLED));
    else if (!item.compare(ERP_DRIVER));
    else if (!item.compare(ERP_ODBC_ENTRY));
    else if (!item.compare(ERP_BBDD));
    else if (!item.compare(ERP_WRITE_BBDD));
    else if (!item.compare(ERP_FIELDS_BBDD_IN));
    else if (!item.compare(ERP_FIELDS_BBDD_OUT));
    else if (!item.compare(ERP_TABLE_IN));
    else if (!item.compare(ERP_TABLE_OUT));
    else if (!item.compare(ERP_MULTIPLE_ITEMS));
    else if (!item.compare(ERP_FIELD_COMPO))    rtn=config.getCompoFieldIndex();
    else if (!item.compare(ERP_FIELD_OT))       rtn=config.getOTFieldIndex();
    else if (!item.compare(ERP_FIELD_METROS))   rtn=config.getMetrosFieldIndex();
    else if (!item.compare(ERP_FIELD_ESTADO));
    else if (!item.compare(ERP_FIELD_ERROR));
    else if (!item.compare(ERP_FIELD_TIPO_TAREA));
    return rtn;
}

/** hace el check del erp*/
QStringList In2ManagerERP::checkERP(){
    QStringList data;
    if (!m_Cliente.compare(ERP_ESTEVE)){
        data=m_SGL->checkERP();
    }
    else if (!m_Cliente.compare(ERP_IN2)){
        data=m_In2erp->checkERP();
    }
    else if (!m_Cliente.compare(ERP_GRAMAR)){
        data=m_In2erp->checkERP();
    }
    return data;
}
/** Ejecuta un sql*/
QStringList In2ManagerERP::execSQL(QString select,bool save){
    QStringList data;
    if (!m_Cliente.compare(ERP_ESTEVE)){
        data=m_SGL->execSQL(select,save);
    }
    else if (!m_Cliente.compare(ERP_IN2)){
        data=m_In2erp->execSQL(select,save);
    }
    else if (!m_Cliente.compare(ERP_GRAMAR)){
        data=m_In2erp->execSQL(select,save);
    }
    return data;

}
/** Procesa los datos que tenga guardados*/
void In2ManagerERP::processDataIn(){
    if (!m_Cliente.compare(ERP_ESTEVE)){
        m_SGL->processDataIn();
    }
}
/** Devuelve el cliente configurado*/
QString In2ManagerERP::getClient(){
    return m_Cliente;
}

/** Procesa los datos que tenga guardados
void In2ManagerERP::processDataOut(){
    if (!m_Cliente.compare(ERP_ESTEVE)){
        m_SGL->processDataOut();
    }
}*/
/** escribe en la BBDD de salida*/
void In2ManagerERP::sltWriteAnswerToBBDD(QString state,QString error){
    if (!m_Cliente.compare(ERP_ESTEVE)){
        m_SGL->sltWriteAnswerToBBDD(state,error);
    }
}

/** Incluye el trabajo actual en la cola de impresión*/
bool In2ManagerERP::insertCurrentWork(){
    bool rtn=true;
    if (!m_Cliente.compare(ERP_ESTEVE)){

    }
    else if (!m_Cliente.compare(ERP_IN2)){
        rtn=m_In2erp->insertCurrentWork();
    }
    return rtn;
}
/** Indica si se permite un empalme con el dato actual*/
bool In2ManagerERP::empalmePermitido(){
    bool brtn=true;
    if (!m_Cliente.compare(ERP_ESTEVE)){
        QString rtn=m_SGL->getItemIn(in_IND_PERMITIR_EMPALME);
        if (!rtn.compare(SGL_SI)) brtn=true;
        else brtn=false;
    }
    else if (!m_Cliente.compare(ERP_IN2)) brtn=true;
    return brtn;
}


/** Fin de impresion*/
void In2ManagerERP::sltFinImpresion(){
    if (!m_Cliente.compare(ERP_ESTEVE)){
        m_SGL->sltFinImpresion();
    }
}
/** Fin de empalme de entrada*/
void In2ManagerERP::sltFinEmpalmeEntrada(){
    if (!m_Cliente.compare(ERP_ESTEVE)){
        m_SGL->sltFinEmpalmeEntrada();
    }
}
/** Fin de empalme de Salida*/
void In2ManagerERP::sltFinEmpalmeSalida(){
    if (!m_Cliente.compare(ERP_ESTEVE)){
        m_SGL->sltFinEmpalmeSalida();
    }
}


/** Guarda un valor del ERP*/
void In2ManagerERP::setValueERPOut(int index,QString value){
    if (!m_Cliente.compare(ERP_ESTEVE)){
        m_SGL->setItemOut(index,value);
    }
}


/** Escribe la BBDD de salida*/
void In2ManagerERP::writeIn2BBDD(){
    if (!m_Cliente.compare(ERP_ESTEVE)){
        m_SGL->writeIn2BBDD();
    }
}

/** Simula Escribe la BBDD de salida*/
void In2ManagerERP::SimulatewriteIn2BBDD(){
    if (!m_Cliente.compare(ERP_ESTEVE)){
        m_SGL->SimulatewriteIn2BBDD();
    }
}

/** Devuelve el ultimo error*/
QString In2ManagerERP::getLastError(){
    QString rtn;
    if (!m_Cliente.compare(ERP_ESTEVE)){
        rtn=m_SGL->getLastError();
    }
    return rtn;
}
/** Devuelve las condiciones fisicas de entrada*/
bool In2ManagerERP::checkCondicionesFisicasEntrada(){
    bool rtn=true;
    if (!m_Cliente.compare(ERP_ESTEVE)){
        rtn=m_SGL->checkCondicionesFisicasEntrada();
    }
    return rtn;
}
/** Decvuelve un valor del ERP*/
QString In2ManagerERP::getValueFromERP(int index){
    QString rtn;
    if (!m_Cliente.compare(ERP_ESTEVE)){
        rtn=m_SGL->getItemIn(index);
    }
    return rtn;
}

/** Indica si se puede cargar un trabajo*/
bool In2ManagerERP::canLoadWork(){
    bool rtn=false;
    if (!m_Cliente.compare(ERP_ESTEVE)){
        QString txt=getValueFromERP(in_ESTADO);
        if (!txt.compare(SGL_STATE_PND)) rtn=true;
    }
    return rtn;
}

/** Elimina una orden*/
void In2ManagerERP::deleteOrden(QString campo,QString valor){
    if (!m_Cliente.compare(ERP_GRAMAR)){
        In2ConfiguratorERP config;
        config.readConfig();
        QString foo=config.getFieldCompo();
        QString sql=QString("delete from ColaImpresion where %1='%2'").arg(foo).arg(valor);
        execSQL(sql,true);
    }
}
