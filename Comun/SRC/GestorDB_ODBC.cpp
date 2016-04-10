/** @addtogroup GestorDB_ODBC
/** @addtogroup GestorDB_ODBC
 *  @{*/
/** @file GestorDB_ODBC.cpp
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright Cemitec. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Gestor de la base de datos
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: GestorDB_ODBC.cpp
 *    @li Versión: x.0
 *    @li Fecha: 30/05/08
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */

#include "GestorDB_ODBC.h"
#include "MemoryMap.h"
#include "globals.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <qsqlresult.h> 
#include <QSqlRecord>
#include <QSqlResult>
#include <QSqlError>
#include <QStringList>
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>

GestorDBODBC::GestorDBODBC(void)
{
}

GestorDBODBC::~GestorDBODBC(void)
{
}

//Abre la base de datos
bool GestorDBODBC::open(QString cDataBase){

    QString conexion_sql=QString("ConexionODBC");
    //QString tmp=QString("DRIVER={Microsoft Access Driver (*.mdb, *.accdb)};Uid=Admin;Pwd=;FIL={MS Access};DBQ=");
    QString tmp=QString("DRIVER={Microsoft Access Driver (*.mdb)};FIL={MS Access};DBQ=");
    tmp.append(cDataBase);
    //tmp=cDataBase;
    dbODBC = QSqlDatabase::addDatabase("QODBC",conexion_sql);
    m_name=conexion_sql;
    dbODBC.setDatabaseName(tmp);
    bool ok = dbODBC.open();
    m_enabled=ok;
    return ok;

}

/** Abre la base de datos.
 *@conexion: Damos el origen de datos ODBC configurado en el PC
 **/
bool GestorDBODBC::open(QString driver,QString conexion,QString user, QString password){

    //QString conexion_sql=QString("ConexionODBC");
    dbODBC = QSqlDatabase::addDatabase(driver);
    dbODBC.setDatabaseName(conexion);
    m_name=conexion;
    dbODBC.setUserName(user);
    dbODBC.setPassword(password);    
    bool ok = dbODBC.open();
    m_enabled=ok;
    qDebug()<<dbODBC.lastError();
    return ok;

}
/** Devuelve si la conexion está habilitada*/
bool GestorDBODBC::isEnabled(){
    return m_enabled;
}
/** Establece si la conexion está habiliatada*/
void GestorDBODBC::setEnabled(bool value){
    m_enabled=value;
}

//Abre la base de datos
void GestorDBODBC::ValoresDB(QString sql){

    unsigned char i;
    QSqlQuery *q=new QSqlQuery(dbODBC);
    bool okp=q->prepare(sql);
    if (okp){
        okp=q->exec();
        if (okp==true){
            QSqlRecord rec = q->record();
            int CDatabase=rec.count();
            for(i=0;i<CDatabase;i++) NombreCampo[i]=rec.fieldName(i);
        }
    }
    q->finish();
    delete (q);
}

//Cierra la base de datos
void GestorDBODBC::close(){
	dbODBC.close();
    QSqlDatabase::removeDatabase(m_name);

}

//Ejecuta comandos SQL
bool GestorDBODBC::exec(char *sql){    
    QString dummy=QString("%1").arg(sql);
    return exec(dummy);
}

//Ejecuta comandos SQL
bool GestorDBODBC::exec(const QString & sql){

    QSqlQuery *q=new QSqlQuery(dbODBC);
    QString dummy;
    dummy=sql;
    bool okp=q->prepare(dummy);
    if (okp){
        okp=q->exec();
        if (okp){
            m_Tabla.clear();
            QSqlRecord rec = q->record();
            CamposDatabase=rec.count();
            while (q->next()){
                for(int i=0;i<CamposDatabase;i++){
                    dummy=q->value(i).toString();
                    m_Tabla.append(dummy);
                }
            }
        }
    }
    q->finish();
    delete (q);
    return okp;

}


int GestorDBODBC::SizeLastQuery(){
	return CamposDatabase;
}

void GestorDBODBC::setNumCamposDataBase(int num){
	CamposDatabase=num;
}

//Ejecuta comandos SQL
bool GestorDBODBC::execNoParse(char *sql){
    QSqlQuery *q=new QSqlQuery(dbODBC);
    QString dummy=QString("%1").arg(sql);
    bool okp=q->prepare(dummy);
    if (okp){
        okp=q->exec();
        if (okp){
            m_Tabla.clear();
            QStringList fieldNames;
            QSqlRecord rec = q->record();
            for( int i=0; i<rec.count(); ++i )
                fieldNames << rec.fieldName( i );
            foreach( QString str, fieldNames ){
                while( q->next() ){
                    m_Tabla << q->value(rec.indexOf( str )).toString();
                }
                q->seek(-1);
            }
        }
    }
    q->finish();
    delete (q);
	return okp;
}
//Ejecuta comandos SQL sin alterar m_Tabla
bool GestorDBODBC::execNoSave(char *sql){
    QSqlQuery *q=new QSqlQuery(dbODBC);
    QString dummy=QString("%1").arg(sql);
    bool okp=q->prepare(dummy);
    if (okp){
        okp=q->exec();
        q->finish();
        delete (q);
    }
    return okp;
}

//Calcula el numero de campos
void GestorDBODBC::NumeroCamposDB(){

    QSqlQuery *q=new QSqlQuery(dbODBC);
	QString dummy;
    QString NombreTablaDatos="Historico"/*"DatosImprimir"*/;
	dummy=QString("select * from %1").arg(NombreTablaDatos);
    q->prepare(dummy);
    bool okp=q->exec();
	if (okp==true){
        QSqlRecord rec = q->record();
		CamposDatabase=rec.count();
	}
    q->finish();
    delete (q);
    MemoryMap *memoryMap=MemoryMap::request(0);
    memoryMap->m_mapa.m_database.NumCamposTotalesDB=CamposDatabase;
}

