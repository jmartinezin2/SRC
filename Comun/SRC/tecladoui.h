#ifndef TECLADOUI_H
#define TECLADOUI_H

#include <QWidget>
#include <QButtonGroup>

namespace Ui {
class TecladoUI;
}

class TecladoUI : public QWidget
{
    Q_OBJECT
    
public:
    explicit TecladoUI(QWidget *parent = 0);
    ~TecladoUI();
    //Carga un texto en buffer de salida
    void setText(QString buffer);
    void setParent(QWidget *parent);
    void show();
    void hide();

public slots:
    //Sale sin actualizar informacion teclado
    void Salir();
    //Sale actualizando informacion de teclado
    void Aceptar();
    void Mayusculas();
    //Nueva pulsacion
    void TeclaPulsada(QAbstractButton *but);
signals:
    //Señal con lo que lleva escrito el teclado
    void bufferTeclado(QString Buffer);
    //Señal de que me pueden eliminar
    void Delete();
    //Señal de que acepten la edicion que hemos hecho
    void Accept();

private:
    Ui::TecladoUI *ui;
    QButtonGroup *m_Group;
    QString Texto;
    unsigned char flag_mayusculas;
    unsigned char m_FlagTilde;

};

#endif // TECLADOUI_H
