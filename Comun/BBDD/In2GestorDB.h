/** @addtogroup In2GestorDB
 *  @{*/
/** @file In2GestorDB.h
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright In2GestorDB. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Gestor de la base de datos
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: In2GestorDB.h
 *    @li Versión: x.0
 *    @li Fecha: 01/05/14
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */

#ifndef IN2GESTORDB_H
#define IN2GESTORDB_H


#pragma once

#include <QStringList>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariantList>


class In2GestorDB
{
public:
	In2GestorDB(void);
    bool open(QString driver,QString conexion,QString name);
    bool open(QString driver,QString conexion,QString name,QString user, QString password);
	void close();				//Cierra la base de datos
	bool execNoParse(char *sql);		//Ejecuta el comando que le digamos
	bool exec(char *sql); //Ejecuta el comando que le digamos y parsea resultados en una tabla
    bool exec(const QString & sql);
	bool execNoSave(char *sql);
    bool execNoSave(const QString & sql);
    bool execClob(const QString & sql, QVariantList &lst);
	void ValoresDB(QString sql);
	int SizeLastQuery();
	void setNumCamposDataBase(int num);
    bool testBlob();
    QString getLastError();
private:
public:	
	QStringList m_Tabla;
	unsigned int CamposDatabase;
    QString NombreCampo[10];
    QString m_name;
    QString m_lastError;
public:
	~In2GestorDB(void);
};


#endif
