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
#include "in2licenser.h"
#include <QProcess>
#include <QTimer>
#include <QFile>
#include <QDebug>
#include "in2crypt.h"
#include "In2GestorDB.h"
#include <QApplication>
#include <QDate>
#include <QMessageBox>

#define SEMILLA 0x0c2ad4a4acb9f023

#define  STORE_DB "Database\\log.sqlite"


#define NUMFIELDS 5


In2Licenser::In2Licenser(QObject *parent) : QObject(parent)
{    
    connect(this,SIGNAL(sgnLicenseError(int)),this,SLOT(sltError(int)));
}

/** Gestor de errores*/
void In2Licenser::sltError(int error){
    m_error=error;
}

/** Chequea licencia*/
QVector<int> In2Licenser::CheckLicense(struct In2License & license){

    //struct In2License license=readLicense();
    //No hay licencia
    QVector<int> fails;

    if (license.m_mother.isEmpty()){
        emit sgnLicenseError(ERROR_NO_LICENSE);
        fails.append(ERROR_NO_LICENSE);
    }
        //No se puede leer la licencia
    QString mother=_readMother();
    if (mother.isEmpty()) {
        emit sgnLicenseError(ERROR_NO_MOTHERBOARD);
        fails.append(ERROR_NO_MOTHERBOARD);
    }
    //Chequeamos los campos
    if (mother.compare(license.m_mother)) {
        emit sgnLicenseError(ERROR_HARDWARE_DIFFERENT);
        fails.append(ERROR_HARDWARE_DIFFERENT);
    }
    if (!license.m_temp.compare("1")){
        QDate date=QDate::fromString(license.m_end,"dd-MM-yyyy");
        if (!date.isValid()) {
            emit sgnLicenseError(ERROR_LICENSE_DATE_ERROR);
            fails.append(ERROR_LICENSE_DATE_ERROR);
        }
        else{
            if (QDate::currentDate()>date) {
                emit sgnLicenseError(ERROR_LICENSE_EXPIRED);
                fails.append(ERROR_LICENSE_EXPIRED);
            }
        }
    }
    int endCounter=license.m_endCounter.toInt();
    int counter=license.m_counter.toInt();
    if (endCounter>0){
        if (counter>endCounter){
            emit sgnLicenseError(ERROR_LICENSE_COUNTER_EXPIRED);
            fails.append(ERROR_LICENSE_COUNTER_EXPIRED);
        }
    }

    return fails;
}
/** Lee la placa base*/
QString In2Licenser::_readMother(){
    QProcess foo;
    QString motherStr;
    foo.start("cmd.exe",QStringList()<<"/c"<<"wmic"<<"baseboard"<<"get"<<"product,Manufacturer,version,serialnumber");
    if (foo.waitForStarted()){
        if (foo.waitForFinished()){
            if (foo.exitCode()==0){
                motherStr=QString(foo.readAllStandardOutput());
            }
        }
    }
    return motherStr;
}

/** Genera licencia*/
QString In2Licenser::encryptLicense(struct In2License &license){    
    license.m_mother=_readMother();
    In2Crypt crypto(SEMILLA); //some random number
    QString testString=QString("%1<@>%2<@>%3<@>%4<@>%5").arg(license.m_mother).arg(license.m_temp).arg(license.m_end).arg(license.m_endCounter).arg(license.m_counter);
    QString result = crypto.encryptToString(testString);
    return result;
}

/** desencripta  licencia*/
struct In2License In2Licenser::decryptLicense(QString str){
    struct In2License license;
    In2Crypt crypto(SEMILLA); //some random number
    QString result = crypto.decryptToString(str);
    QStringList lst=result.split("<@>");
    if (lst.count()==NUMFIELDS){
        license.m_mother=lst.at(0);
        license.m_temp=lst.at(1);
        license.m_end=lst.at(2);
        license.m_endCounter=lst.at(3);
        license.m_counter=lst.at(4);
    }
    return license;
}
/** Lee la licencia*/
struct In2License In2Licenser::readLicense(){
    bool  rtn;
    struct In2License license;
    In2GestorDB db;
    QString file=QString("%1\\%2").arg(QApplication::applicationDirPath()).arg(STORE_DB);
    if (db.open(QString("QSQLITE"),QString("Logs"),file)){
        QString foo=QString("select Key from 'Control'");
        rtn=db.exec(foo);
        if ((rtn)&&(db.m_Tabla.count())){
            QString lic=db.m_Tabla.at(0);
            license=decryptLicense(lic);
        }
        db.close();
    }
    return license;
}

/** Lee la licencia*/
struct In2License In2Licenser::readLicenseFromBBDD(QString bbdd){
    bool  rtn;
    struct In2License license;
    In2GestorDB db;
    QString file=bbdd;
    if (db.open(QString("QSQLITE"),QString("Logs"),file)){
        QString foo=QString("select Key from 'Control'");
        rtn=db.exec(foo);
        if ((rtn)&&(db.m_Tabla.count())){
            QString lic=db.m_Tabla.at(0);
            license=decryptLicense(lic);            
            //Ojo, cada vez que la lea, incremento el contador y la sobreescribo
            struct In2License newLicense=license;
            bool ok;
            int counter=newLicense.m_counter.toInt(&ok,10);
            if (ok){
                counter++;
                newLicense.m_counter=QString("%1").arg(counter);
                foo=encryptLicense(newLicense);
                saveLicenseToBBDD(bbdd,foo);
            }
        }
        db.close();
    }
    return license;
}

/** Devuelve la licencia de la BBDD*/
QString In2Licenser::checkLicenseFromBBDD(QString bbdd){
    bool  rtn;
    QString lic;
    In2GestorDB db;
    QString file=bbdd;
    if (db.open(QString("QSQLITE"),QString("Logs"),file)){
        QString foo=QString("select Key from 'Control'");
        rtn=db.exec(foo);
        if ((rtn)&&(db.m_Tabla.count())){
            lic=db.m_Tabla.at(0);
        }
        db.close();
    }
    return lic;
}

/** Guarda la licencia en una BBDD*/
bool In2Licenser::saveLicenseToBBDD(QString bbdd,QString license){
    bool  rtn=false;
    In2GestorDB db;
    QString file=bbdd;
    if (db.open(QString("QSQLITE"),QString("Logs"),file)){
        QString foo=QString("select Key from Control");
        db.exec(foo);
        if (db.m_Tabla.isEmpty())
            foo=QString("insert into Control (Key) values ('%1')").arg(license);
        else
            foo=QString("update Control set Key='%1'").arg(license);
        rtn=db.exec(foo);
    }
    return rtn;
}

/** Elimina licencia de la BBDD*/
bool In2Licenser::deleteLicenseFromBBDD(QString bbdd){
    bool  rtn=false;
    In2GestorDB db;
    QString file=bbdd;
    if (db.open(QString("QSQLITE"),QString("Logs"),file)){
        QString foo=QString("delete from Control");
        rtn=db.exec(foo);
        db.close();
    }
    return rtn;
}

