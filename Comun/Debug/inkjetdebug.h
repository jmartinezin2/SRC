#ifndef INKJETDEBUG_H
#define INKJETDEBUG_H

#include <QObject>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QPixmap>
#include <QTextEdit>
//#include "globals.h"
#include "in2defines.h"
#include "ui_InkjetDebugUI.h"



struct stInfoDebug{
    QString TablaDebug[N_ITEMS_TABLA_DEBUG];
};

class InkjetDebug : public QWidget
{
	Q_OBJECT

public:
    static InkjetDebug *request(QWidget *parent);
    void ForceFlush();
    QStringList getErrorList();
    void setUser(QString user);
    //InkjetDebug(QWidget *parent);
    //~InkjetDebug();

private:
    InkjetDebug(QWidget *parent);
    ~InkjetDebug();

private:
    QWidget *Parent;
    Ui::InkjetDebugUI *ui;
    static InkjetDebug *mInstance;
    int m_pendingToSave;
    QString m_user;

signals:
    void sgnInfoError(const QString &);
    void InkjetDbgMsg(const QString &);
private:
    void ClosePreviousFile();
    void SignFile();


private slots:
	void Salir();
    void UpdateShowLevel();
    void SaveLogToFile();
    void MinimizarVentanaDebug();
    void LimpiarVentanaDebug();
    void AutomatedSave();
    void sltEnsureVisible();
    void sltUpdateDB(QString txt);
private:
    unsigned char NivelDebug;
    unsigned int contador_signal;
    stInfoDebug m_stInfo;


public slots:
    void showLog();
    void forcedShowLog();
    void ProcessMsg(unsigned char codigo,const QString  rcv);

    //void ReceiveDebugInfo(unsigned char codigo, QString mensaje);

};

#endif // INKJETDEBUG_H
