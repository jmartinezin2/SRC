/** @addtogroup ConfUsers
 *  @{*/
/** @file ConfUsers.h
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright In2 Printing Solutions
 *
 *    @brief Pantalla de configuracion de tintas
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: ConfUsers.h
 *    @li Versión: x.0
 *    @li Fecha: 05/05/13
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */
#ifndef COLAIMPRESION_H
#define COLAIMPRESION_H

#include <QObject>
#include "inkjetform.h"
#include "inputdata.h"
#include <QTableWidget>
#include <QTextEdit>
#include <QLabel>
#include <QComboBox>
#include <QGroupBox>
#include <QPushButton>
#include "ui_ColaImpresionUI.h"
#include "inkjetcommon.h"

//Chequea la OF en una BBDD
struct OFcheckODBC{
    bool m_enabled;
    QString m_user;
    QString m_password;
    QString m_select;
    QString m_connODBC;
};

class ColaTrabajo : public QWidget
{
	Q_OBJECT

public:
    ColaTrabajo(QWidget *parent, bool bLoad=true);
    ~ColaTrabajo();
    void setAutoload(bool value);

private:
	//Actualiza un campo de la tabla
	void ActualizaCampo(QString tmp);

private slots:

	void Atras();
    //Cancela la seleccion actual
    void sltCancelaScan();
    // Selecciona automaticamente el testo segun se escribe
    void sltSelectText(QString txt);
    //Seleccionar trabajo
    void TrabajoSeleccionar();
    //Eliminar trabajo
    void TrabajoEliminar();
	//Se ha seleccionado un campo de la tabla
	void SeleccionCampo(int x, int y);
	//Actualiza la informacion de pantalla
	void UpdateInfo();
	//Actualiza la info
	void ActualizaDataSlot();
	//Test root
	void TestRoot();
    //Seleccion por scan
    void sltScanSelection(QString p);
    //Seleccion por Scan
    void sltCheckScan();
    //Actualiza la tabla de historico
    void updateHistoryTable(QStringList data,int campos);

	
public slots:	
	void InputTeclado(QString Buffer);
signals:
    void ActualizaMaquina();
    void SendInformerInfo(unsigned char code, QString msg);
    void FileSelected();
    void KillIn2Msg();
private:
    Ui::ColaImpresionUI *ui;
    //InkjetCommon *DataCommon;
    bool m_autoload;
    bool m_ScanEnabled;
    //Celda seleccionada
    int iFila;
    int iColumna;
    struct OFcheckODBC m_ConnODBC;
    //GestorDBODBC *m_gestorDB;
    QWidget *m_prev;
    MemoryMap *m_memoryMap;
};

#endif // COLAIMPRESION_H
