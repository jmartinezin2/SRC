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

#include "in2logon.h"
#include "inkjetdebug.h"
#include <QFile>
#include <QSettings>
#include <QApplication>
#include "in2log.h"
#include "in2defines.h"

#define DEFAULT_CONFIG_FILE "In2Logon.ini"
#define DEFAULT_HOST "basalde.cemitec.local"
#define DEFAULT_DN "CN=users,DC=cemitec,DC=local"
#define DEFAULT_FILTER_ROOT ""
/*"(memberOf=CN=in2root,CN=Users,DC=cemitec,DC=local)"*/
#define DEFAULT_FILTER_PRINTER ""
/*"(memberOf=CN=in2printer,,CN=Users,DC=cemitec,DC=local)"*/
#define DEFAULT_LOCALHOST "cemitec.local"

#define TXT_CONFIG "CONFIG"
#define TXT_HOST "ConfigHost"
#define TXT_DN "ConfigDn"
#define TXT_FILTER_ROOT  "ConfigFilterRoot"
#define TXT_FILTER_PRINTER  "ConfigFilterPrinter"
#define TXT_LOCALHOST "ConfigLocalHost"


In2Logon::In2Logon(QObject *parent) :
    QObject(parent)
{
    m_memoryMap=MemoryMap::request(0);
    QString strPath=QString("%1\\Config\\%2").arg(QApplication::applicationDirPath()).arg(DEFAULT_CONFIG_FILE);
    QFile dummyFile(strPath);
    if (dummyFile.exists()){
        QSettings FicheroIni(strPath,QSettings::IniFormat,0);
        FicheroIni.beginGroup(TXT_CONFIG);
        m_Config.m_host=FicheroIni.value(TXT_HOST,DEFAULT_HOST).toString();
        m_Config.m_Dn=FicheroIni.value(TXT_DN,DEFAULT_DN).toString();
        m_Config.m_filterRoot=FicheroIni.value(TXT_FILTER_ROOT,DEFAULT_FILTER_ROOT).toString();
        m_Config.m_filterPrinter=FicheroIni.value(TXT_FILTER_PRINTER,DEFAULT_FILTER_PRINTER).toString();
        m_Config.m_LocalHost=FicheroIni.value(TXT_LOCALHOST,DEFAULT_LOCALHOST).toInt();
        m_Config.m_host.remove("\"");
        m_Config.m_Dn.remove("\"");
        m_Config.m_filterRoot.remove("\"");
        m_Config.m_filterPrinter.remove("\"");
        m_Config.m_LocalHost.remove("\"");
        FicheroIni.endGroup();
    }
    else{
        emit SendDebugInfo(LOGGER_WARNING,"In2Logon:No existe In2Logon.ini");
        m_Config.m_host=DEFAULT_HOST;
        m_Config.m_Dn=DEFAULT_DN;
        m_Config.m_filterRoot=DEFAULT_FILTER_ROOT;
        m_Config.m_filterPrinter=DEFAULT_FILTER_PRINTER;
        m_Config.m_LocalHost=DEFAULT_LOCALHOST;
    }
}
/** Chequea contra el directorio activo por defecto*/
int In2Logon::CheckDA(){
    Logger *m_logger=new Logger();
    m_logger->setDN(m_Config.m_Dn);
    m_logger->setFilter(m_Config.m_filterRoot);
    m_logger->setHostName(m_Config.m_host);
    m_logger->setLocalHost(m_Config.m_LocalHost);
    QString user=m_memoryMap->getUser();
    QString password=m_memoryMap->getPassword();
    int rtn=m_logger->TryConnection(user,password);
    return rtn;
}
/** Chequea contra el directorio activo y contra un grupo especifico*/
int In2Logon::CheckDA(const QString & group){
    /*Logger *m_logger=new Logger();
    m_logger->setDN(m_Config.m_Dn);
    m_logger->setFilter(m_Config.m_filter);
    m_logger->setHostName(m_Config.m_host);
    m_logger->setLocalHost(m_Config.m_LocalHost);
    QString user=m_memoryMap->getUser();
    QString password=m_memoryMap->getPassword();
    int rtn=m_logger->TryConnection(user,password);*/
    if (group!="")
        m_Config.m_filterRoot=QString("(memberOf=%1)").arg(group);
    else
        m_Config.m_filterRoot.clear();
    int rtn=CheckDA();
    return rtn;
}
/** Chequea contra el directorio activo y contra un grupo especifico*/
int In2Logon::CheckDARoot(){
    Logger *m_logger=new Logger();
    m_logger->setDN(m_Config.m_Dn);
    m_logger->setFilter(m_Config.m_filterRoot);
    m_logger->setHostName(m_Config.m_host);
    m_logger->setLocalHost(m_Config.m_LocalHost);
    QString user=m_memoryMap->getUser();
    QString password=m_memoryMap->getPassword();
    int rtn=m_logger->TryConnection(user,password);
    return rtn;

}

/** Chequea contra el directorio activo y contra un grupo especifico*/
int In2Logon::CheckDAPrinter(){
    Logger *m_logger=new Logger();
    m_logger->setDN(m_Config.m_Dn);
    m_logger->setFilter(m_Config.m_filterPrinter);
    m_logger->setHostName(m_Config.m_host);
    m_logger->setLocalHost(m_Config.m_LocalHost);
    QString user=m_memoryMap->getUser();
    QString password=m_memoryMap->getPassword();
    int rtn=m_logger->TryConnection(user,password);
    return rtn;
}
/** Teste los derechos de usuario a nivel local*/
int In2Logon::TestLocalUser(){
   int ret=1;
   QString derechos;
   MemoryMap *memoryMap=MemoryMap::request(0);
   QString tmp=QString("select Tipo from Usuarios where Nombre='%1'and Password='%2'")
            .arg(m_memoryMap->getUser())
            .arg(m_memoryMap->getPassword());
   In2Log db;
   QStringList rslt=db.execSQL(tmp);
   if (!rslt.count()){
        InkjetDebug *dbg=InkjetDebug::request(0);
        dbg->ProcessMsg(LOGGER_ERROR,tr("No existen usuarios en la BBDD"));
        memoryMap->setUserRights(ACCESS_STRANGE);
        ret=0;
    }
    else{
        if (rslt.count()>0){
            derechos=rslt.at(0);
            if (derechos==STR_ROOT) {
                memoryMap->setUserRights(ACCESS_ROOT);
                ret=1;
            }
            else if (derechos==STR_PRINTER){
                memoryMap->setUserRights(ACCESS_PRINTER);
                ret=1;
            }
            else if (derechos==STR_MAINTENANCE){
                memoryMap->setUserRights(ACCESS_MAINTENANCE);
                ret=1;
            }
            else{
                memoryMap->setUserRights(ACCESS_STRANGE);
                ret=0;
            }
        }
        else {
            memoryMap->setUserRights(ACCESS_STRANGE);
            ret=0;
        }
    }
    return ret;
}



