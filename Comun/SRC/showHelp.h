/** @addtogroup showHelp
 *  @{*/
/** @file showHelp.h
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright In2 Printing Solutions. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Pantalla de parametros de impresion
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: showHelp.h
 *    @li Versión: x.0
 *    @li Fecha: 05/05/08
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */

#ifndef SHOWHELP_H
#define SHOWHELP_H

#include "MemoryMap.h"
#include <QComboBox>
#include <QTextEdit>
#include <QCheckBox>
#include <QTimer>
#include <QButtonGroup>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QFontComboBox>
#include "GestorDB_SQLITE.h"
#include "ui_helpUI.h"

class showHelp : public QWidget
{
	Q_OBJECT

public:
    static showHelp *request(QWidget *parent);
    //Constructor
    showHelp(QWidget *parent);

private:

    //Destructor
    ~showHelp();
    static showHelp *mInstance;
    GestorDBSQLITE miDataBaseHelp;
    QString AbsolutePathDbHelp;
    QString AbsolutePathHelpFiles;
    int numCampos;
    QStringList indice;
    QStringList resultados;

private slots:

    void initVariables();
	void Salir();
    void abrirBaseDatos();
    void cargaIndice();
    QString devuelveArchivoMostrar(QString nivel);
    void muestraElementoSeleccionado();
    void busquedaTag(QString nivel);
    void muestraResultadoSeleccionado();
    void realizarBusqueda();
    void avanzarNivel();
    void retrocederNivel();
    void avanzarNivelResultados();
    void retrocederNivelResultados();
    void mostrarPaginaDefault(); 
    void mostrarPaginaNoEncontrada();
    void muestraElemento(QString nivel);

public slots:


private:
    Ui::helpUI *ui;


signals:


};

#endif // SHOWHELP_H
