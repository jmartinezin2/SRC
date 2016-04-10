#ifndef INKJETCOMMON_H
#define INKJETCOMMON_H

#include <QObject>
#include "globals.h"
#include <QLabel>
#include <QPushButton>
#include "MemoryMap.h"

//La estructura con punteros a pasar
typedef struct stCommon{
    //Imagen a mostrar de fondo
    QLabel *m_Imagen;
    //Texto de informacion de status
    QLabel *m_TextoStatus;
    //Labels del fichero
    QLabel *m_NombreFicheroConfig;
    QLabel *m_RutaFicheroConfig;
    QLabel *m_User;
    QLabel *fecha;
    //QLabel *hora;
    QLabel *usuario_actual;
    QLabel *led_EstadoMaquina;
    QLabel *led_EstadoTemperatura;
    QLabel *led_EstadoLampara;
    QPushButton *m_MostrarInicio;
    QPushButton *m_MostrarAyuda;
    QPushButton *m_MostrarMantenimiento;
    QLabel *lbl_EstadoMaquina;
    QLabel *lbl_EstadoTemperatura;
    QLabel *lbl_EstadoLampara;

}STCOMMON;

class InkjetCommon : public QObject
{
    Q_OBJECT
public:
    explicit InkjetCommon(QObject *parent = 0);
    explicit InkjetCommon(QWidget *parent,struct stCommon & dataCommon);
    //Muestra informacion relativa al estado del sistema en la ventana inferior
    void ShowStatus(int iEstado);
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

public slots:
    //Muestra la barra de tareas de windows
    void MostrarInicio(void);
    void MostrarAyuda(void);
    void MostrarMantenimiento(void);


private:
 //Esta estructura contiene los punteros a los leds comunes a todas las pantallas.
  struct stCommon stDataCommon;
  QWidget *m_parent;
  MemoryMap *m_memoryMap;

signals:

public slots:

};

#endif // INKJETCOMMON_H
