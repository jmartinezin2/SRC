/** @addtogroup GestorDB_SQLITE
 *  @{*/
/** @file GestorDB_SQLITE.h
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright In2 Printing Solutions. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Gestor de la base de datos
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: GestorDB_SQLITE.h
 *    @li Versión: x.0
 *    @li Fecha: 30/05/08
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */

#ifndef GESTORDBSQLITE_H
#define GESTORDBSQLITE_H


#pragma once

//#include <sqlite3.h>
#include <QStringList>
#include <QSqlDatabase>
#include <QSqlError>

//Comandos aceptados
#define	SELECT 0
//#define DELETE 1

class GestorDBSQLITE
{
public:
    GestorDBSQLITE(void);
    bool open(QString cDataBase);	//Abre la base de datos
    void close();				//Cierra la base de datos
    bool open(QString cDataBase,QString conexion);	//Abre la base de datos
    void close(QString conexion);				//Cierra la base de datos
    bool exec(QString sql);		//Ejecuta el comando que le digamos;
    bool execNoSave(char *sql);
    bool execNoSave(const QString & sql);
    QSqlError getLastError();

private:
	//Hace un control de los errores en elacceso de la base de datos
	void ControlErrores();
	//Callback del select
	//int GuardaResultado(void* paque,int n, char** re,char** nomcol);
private:
    QSqlDatabase m_db;	//Nuestra base de datos
    int CamposDatabase;
    QSqlError m_error;
    QString m_name;
public:	
	QStringList m_Tabla;
public:
    ~GestorDBSQLITE(void);
};


#endif
