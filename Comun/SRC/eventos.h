/** @addtogroup Eventos
 *  @{*/
/** @file Eventos.h
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright In2 Printing Solutions. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Pantalla de configuracion de tintas
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: Eventos.h
 *    @li Versión: x.0
 *    @li Fecha: 05/05/08
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */
#ifndef EVENTOS_H
#define EVENTOS_H

#include <QObject>
#include "inkjetform.h"
#include "inputdata.h"
#include <QTableWidget>
#include <QTextEdit>
#include <QLabel>
#include <QComboBox>
#include <QGroupBox>
#include <QPushButton>
#include "ui_GestionEventosUI.h"
#include "inkjetcommon.h"

class Eventos : /*public InkjetForm*/public QWidget
{
	Q_OBJECT

public:
    Eventos(QWidget *parent);
    ~Eventos();

private:
	//Actualiza tamaño de la tabla
	void AjustaTabla();
	//Actualiza un campo de la tabla
	void ActualizaCampo(QString tmp);
    //Carga los ficheros de log
    void LoadLogFiles();

private slots:

    void ResetEventos();
	void Atras();

	//Se ha seleccionado un campo de la tabla
	void SeleccionCampo(int x, int y);
	//Actualiza la informacion de pantalla
	void UpdateInfo();
    void UpdateInfoFecha(QString dia, QString mes, QString ano);
	//Actualiza la info
    //void ActualizaDataSlot();
	//Test root
	void TestRoot();

	void AceptarValores();
    void MostrarTodos();

    void UpdateDiaMesAnoEvento();

    void UpdateDiaEvento();
    void UpdateDia(QString value);

    void UpdateMesEvento();
    void UpdateMes(QString value);

    void UpdateAnoEvento();
    void UpdateAno(QString value);

    void UpdateCalendario(void);

    //void SetDataCommon();
    //Testea si hay que hacer algo por cambiar de pestaña
    void sltCkeckTab(int x);
    //Caputura la pulsación de la tabla de losg
    void sltGetLogTableItem(int x, int y);
    //Muestra el fichero de log que se le diga
    void sltShowLogFile(const QString & file,int x);


public slots:	
	void InputTeclado(QString Buffer);
	
private:
    Ui::GestionEventosUI *ui;
    //InkjetCommon *DataCommon;
    int m_Fila;
    int m_Columna;
    MemoryMap *m_memoryMap;


};

#endif // EVENTOS_H
