#ifndef GESTORIMPRESION_H
#define GESTORIMPRESION_H

#include <QObject>
#include <QThread>
#include "InkjetXusbCtrl.h"
#include "imagevarxaarcreator.h"
#include "imagevarxaarsender.h"

class GestorImpresion : public QObject
{
    Q_OBJECT
public:
    static GestorImpresion *request(QObject *parent);
    int getNumCards();
    int EnablePrintMode(int card);
    int EnableMaskPrintMode(DWORD mask,int card);
    int DisablePrintMode(int card);
    void FullDisablePrintMode();
    void sendImageToPrint(int card,const QImage & image);
    void resetCard(int card);
    void changePalette(int card,char *pal);
    void storePalette(char *val);
    //void preLoadVarImages();
    void CleanVariableBuffer();
    void appendVariableImage(In2VarImage p);
    void getSerial(int x,char *buffer);
    void getSerialHead(int card, int head, char *buffer);
    unsigned int checkSerialHead(int card,int head);
    void ExecFullSoftStop();
    void WriteSwathePrintCount(int card,DWORD *swathePrintCount);
    void IgnorePd(int card,DWORD count);
    void EnableInternalPD(int card,int value);
    void setVariableGenerationLimit(int x);
    void setVariableBufferSize(int x);
    void setVariableReps(int reps);
    struct In2VarImage getVariableImage(int card,int index);
    void setNewConfigFile(QString filename);
    void setHighSpeedActive(QString filename, bool active);
    void GeneratePD(int card);
    DWORD getHeadStatus(int card,int head);
    QString getHeadStatusDescription(int card,int head);
    bool bootHead(int card,int head);
    DWORD getNumConnectedHeads(int card);
    bool PowerControlHead(int card,char mask, bool enable);
    void setControlHead(int card,int head);
    void deleteModule();
    void sendImageToHead(int card, int head, const QImage & image);
    QString getConfigFile();
    bool readSwathePrintCount(int card,DWORD *count);
    bool isDriverLoaded();
    void setVariableDataOrigin(int origin);
    int getActiveSenderThreads();
    int getActiveCreatorThreads();
    QStringList getCurrentVariableList(int card);
    bool isImagenEnviada();
    void setHeadVOffset(int card,int head,int row,DWORD value);
    DWORD getHeadVOffset(int card,int head,int row);
    void EnableInternalEncoder(int card,BYTE val);
    void setInternalEncoderValue(int card,DWORD val);
    void sltForceReinit();

private:
    GestorImpresion(QObject *parent = 0);

signals:
    void SendDebugInfo(unsigned char level,QString data);
    void errorImpresion(int);
    void currentPrintNumber(int card,DWORD count);
    void lastSwathePrinted(int card);
    //Señales de impresion variable
    void StartPrecreate(int Origin);
    void StartGenerating();
    void PauseGenerating();
    void StopGenerating();
    void StartPreload();
    void StartLoading();
    void LastVarSwatheReplaced(int);
    void newImagePreCreated(int);
    void newImagePreLoaded(int);
    void sgnReinicioSistema(int step);
    void sgnKillIn2Msg();
    void newExternalImage(QString  txt);
    void sgnTest();



private slots:
    void sltSendDebugInfo(unsigned char level,QString data);
    void sltAskPrintCount();
    //Impresion variablrr
    void sltLoadVarImages();
    void sltInitImpresionVariable();
    void sltCreateVarImages();
    void sltLastVarSwatheReplaced(int);
    void sltImagePreCreated(int);
    void sltImagePreLoaded(int);
    //void sltwaitLastSwathe();
    void sltTestCards();
    void sltLaunchCreatorAndSenderThreads();
    void _init();



public slots:
    void sltRefreshInfo();
    void sltMonitorPrintCount(bool activate);
    void sltLaunchImpresionVariable();
    void sltLaunchImpresionVariableNoPrecreate();
    void sltPauseVariablePrinting();
    bool InitGestor();
    void sltPowerDownHeads();
    void sltPowerUpHeads();
    void sltReiniciarSistema();
    void sltReiniciarSistema1();
    void sltReiniciarSistema2();
    void sltReiniciarSistema3();
    void sltReiniciarSistema4();
    void sltMonitorHeads(bool monitor);
    void _sltMonitorHeads();
    void sltLoadExternalVarImage(QString txt);
    void sltStopVariable();

private:
     int m_inumCards;
     QVector<InkjetXusbCtrl*> m_cards;
     QVector<ImageVarXaarCreator*> m_ImageCreator;
     QVector<ImageVarXaarSender*> m_ImageSender;
     static GestorImpresion *mInstance;
     In2BufferImages m_VariableBufferData;
     bool m_MonitorPrintCount;
     int m_VariableGenLimit;
     int m_Repeticiones;
     QThread *threadCreator;
     QThread *threadSender;
     char m_AuxPalette[8];
     bool m_bHighSpeedActive;
     QString m_ConfigFile;
     bool bDriverLoaded;
     bool bInit;
     int m_VariableDataOrigin;
     bool m_monitorHeads;
     MemoryMap *memoryMap;
     bool m_ImagenEnviada;

};



#endif // GESTORIMPRESION_H
