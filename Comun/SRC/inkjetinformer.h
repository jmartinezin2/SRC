#ifndef INKJETINFORMER_H
#define INKJETINFORMER_H

#include <QObject>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QPixmap>
#include <QTextEdit>
#include "globals.h"
#include "ui_InkjetDebugUI.h"


struct stInfoInformer{
    QString TablaInformer[N_ITEMS_TABLA_INFORMER];
    unsigned char codigo[N_ITEMS_TABLA_INFORMER];
};

class InkjetInformer : public QObject
{
	Q_OBJECT

public:
    InkjetInformer(QObject *parent);
    ~InkjetInformer();
    unsigned char estado;

private:
    QObject *Parent;
	QString ruta;
	QString nombreBase;
    QString NombreRutaArchivoActual;
	int numeroArchivos;
	int archivoActual;
    int diaActual;
    unsigned char NivelInformer;
    int contador_signal;
    stInfoInformer m_stInformer;
    unsigned char guardarPorNumeroEventos;
    unsigned char guardarPorTiempo;
    int numeroEventosGuardado;
    int tiempoGuardado;
    int contador_tiempo;
    //Timer de eventos
    QTimer *m_TimerInformer;

signals:

private slots:
    void Init();
    void SaveLogToFile();
	void LeerConfiguracionInformer();
    void LeerArchivoActual();
    void ActualizaDataSlot();

public slots:
    void ProcessMsg(unsigned char codigo,const QString & rcv);
};

#endif // INKJETINFORMER_H
