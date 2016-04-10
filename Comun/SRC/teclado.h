/** @addtogroup Teclado
 *  @{*/
/** @file Teclado.h
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright In2 Printing Solutions
 *
 *    @brief Pantalla de teclado
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: Teclado.cpp
 *    @li Versión: x.0
 *    @li Fecha: 01/04/13
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */

#ifndef TECLADO_H
#define TECLADO_H

#include <QWidget>
#include <QDialog>
//#include "ui_teclado.h"

#include <QPushButton>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include "ui_tecladoui.h"

#define NUMERICO	0
#define ALFANUMERICO 1


class Teclado : public QWidget
{
    Q_OBJECT
public:
    Teclado(QWidget *parent=0,int mode=0);
    ~Teclado();
    //Carga un texto en buffer de salida
    void setText(QString buffer);
    void setParent(QWidget *parent);
    void show();
    void hide();

public slots:
    void Pulsacion(int i);
    //Sale sin actualizar informacion teclado
    void Salir();
    //Sale actualizando informacion de teclado
    void Aceptar();
    void Mayusculas();
    //Nueva pulsacion
    void TeclaPulsada(QAbstractButton *but);
    //Tecla soltada
    void TeclaSoltada(QAbstractButton *but);
signals:
    //Señal con lo que lleva escrito el teclado
    void bufferTeclado(QString Buffer);
    //Señal de que me pueden eliminar
    void Delete();
    //Señal de que acepten la edicion que hemos hecho
    void Accept();
private:
    Ui::TecladoUI *ui;
    QString Texto;
    QGroupBox *Bloque;
    unsigned char flag_mayusculas;
//Layout de la zona de Numeros
    QGridLayout *m_NumerosLayout;
    QButtonGroup *m_Group;
    unsigned char m_FlagTilde;
private:
//	Ui::TecladoClass ui;
};


#endif // TECLADO_H
