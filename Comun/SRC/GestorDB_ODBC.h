/** @addtogroup GestorDB_ODBC
 *  @{*/
/** @file GestorDB_ODBC.h
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright In2 Printing Solutions. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Gestor de la base de datos
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: GestorDB_ODBC.h
 *    @li Versión: x.0
 *    @li Fecha: 30/05/08
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */

#ifndef GESTORDBODBC_H
#define GESTORDBODBC_H


#pragma once

#include <QStringList>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include "MemoryMap.h"

class GestorDBODBC
{
public:
	GestorDBODBC(void);
	bool open(QString cDataBase);	//Abre la base de datos
    bool open(QString driver,QString conexion,QString user, QString password);
    bool isEnabled();
    void setEnabled(bool value);
    void close();				//Cierra la base de datos
	bool execNoParse(char *sql);		//Ejecuta el comando que le digamos
	bool exec(char *sql); //Ejecuta el comando que le digamos y parsea resultados en una tabla
    bool exec(const QString & sql);
	bool execNoSave(char *sql);
	void ValoresDB(QString sql);
	int SizeLastQuery();
	void setNumCamposDataBase(int num);
	void NumeroCamposDB();
private:
    bool m_enabled;

public:	
	QSqlDatabase dbODBC;						//Nuestra base de datos
	QStringList m_Tabla;
	unsigned int CamposDatabase;
    QString NombreCampo[10];
    QString m_name;

public:
	~GestorDBODBC(void);
};


#endif
