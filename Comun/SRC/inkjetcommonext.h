#ifndef INKJETCOMMONEXT_H
#define INKJETCOMMONEXT_H

#include <QObject>
#include "globals.h"
#include "inkjetdebug.h"
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>

//La estructura con punteros a pasar
typedef struct stCommonExt{
    //Imagen a mostrar de fondo
    QLabel *m_Imagen;
    //Texto de informacion de status
    QLabel *m_TextoStatus;
    QLabel *m_User;
    QLabel *fecha;
    QTextEdit *info;
    QLabel *usuario_actual;
    QLabel *led_EstadoMaquina;
    QLabel *led_EstadoTemperatura;
    QLabel *led_EstadoLampara;
    QPushButton *m_MostrarInicio;
    QPushButton *m_MostrarAyuda;
    QLabel *lbl_EstadoMaquina;
    QLabel *lbl_EstadoTemperatura;
    QLabel *lbl_EstadoLampara;
    QLabel *lbl_IP;
    QLabel *lblLamp;

}STCOMMONEXT;


class InkjetCommonExt : public QObject
{
    Q_OBJECT
public:
    explicit InkjetCommonExt(QObject *parent = 0);
    explicit InkjetCommonExt(QWidget *parent,struct stCommonExt & dataCommon);
    //Muestra el estado de las comunicaciones
    void ShowComms();
    //Carga un LED especifico
    void ShowLED(int index);
    //Muestra estado de impresora
    void ShowEstadoImpresora();
    //Refresca el modo de funcionamiento
    void RefreshUserMode(void);
    //Refresca informacion superior
    void ShowInfo();

private:
    void LoadErrorList();
    void LoadIPInfo();
public slots:
    //Muestra la barra de tareas de windows
    void MostrarAyuda(void);
    void MostrarMantenimiento(void);
    void sltEstadoFluidica(char state);
    void sltEstadoLampara(char state);

private slots:
    void sltAddInfo(const QString &);

private:
 //Esta estructura contiene los punteros a los leds comunes a todas las pantallas.
  struct stCommonExt stDataCommon;
  QWidget *m_parent;
  InkjetDebug *debug;

signals:
  void showLog();

public slots:

};

#endif // INKJETCOMMONEXT_H
