/** @addtogroup InputData
 *  @{*/
/** @file InputData.h
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright In2 Printing Solutions
 *
 *    @brief Subpantalla de edicion de numero de serie
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: InputData.cpp
 *    @li Versión: x.0
 *    @li Fecha: 28/07/13
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */

#ifndef INPUTDATA_H
#define INPUTDATA_H

#include <QObject>
#include <QTextEdit>
#include <QLabel>
#include <QGroupBox>
#include <QString>
#include "teclado.h"
#include "ui_InputDataUI.h"

class InputData : public QWidget
{
    Q_OBJECT

public:
    static InputData *request(QWidget *parent, const QString& title);
    static InputData *request(QWidget *parent, const QString& title, const QString& text);
    static InputData *requestOnce(QWidget *parent, const QString& title);
    static bool exist();
private:
    InputData(QWidget *parent=0,const QString& title =QString());
    InputData(QWidget *parent,const QString& title,const QString& text);
    ~InputData();
    static InputData *mInstance;

private slots:
//Actualiza el campo seleccionado con el teclado
    void ActualizaCampo(QString item);
//Selecciona el data
    void SelectData();
//Destruye
    void DeleteTeclado();
//Acepta la seleccion
    void AceptaSeleccion();
public:
   //Actualizo el foco
   void setFocus();
private:
    Ui::InputDataUI *ui;
    //Puntero a teclado
    Teclado *SCRTeclado;
public slots:
    void hide();
signals:
//Señal de que se actualicen parametros
    void Update(QString value);
};

#endif // INPUTDATA_H
