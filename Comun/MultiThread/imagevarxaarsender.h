#ifndef IMAGEVARXAARSENDER_H
#define IMAGEVARXAARSENDER_H

#include <QObject>
#include "inkjetxusbvarctrl.h"
#include "globals.h"
#include "in2bufferimages.h"


enum{
    SENDER_PAUSED=0,
    SENDER_LAUNCHED,
    SENDER_STOP
};

class ImageVarXaarSender : public QObject
{
    Q_OBJECT
public:
    explicit ImageVarXaarSender(QObject *parent = 0,int Card=0,/*QList<struct In2VarImage>*/In2BufferImages *data=NULL);
    void Reset();
    void changePalette(char *palette);
    void setNewConfigFile(QString filename);
    void setDataOrigin(int origin);
private:
    bool TestBufferVacio();
signals:
    void FinishedSending();
    void error(QString err);
    void LastSwatheReplaced(int);
    void CompleteBufferReplaced();
    void FinishedPreLoading();
    void SendDebugInfo(unsigned char code,const QString &);
    void ImagePreLoaded(int);

private slots:
    void sltUpdateSignals();

public slots:
    void XaarLastSwathePrinted(int);
    void PreloadImages();
    void PreloadSingleImage(int x);
    void sltSendingLoop();
    void sltNoPreloadSendingLoop();
    void sltSendingCMYKLoop();
    void sltStartSending();
    void sltNoPreloadStartSending();

    void sltStopSending();
    void sltPauseSending();
private:
    int m_lastSentSwathe;
    int m_Card;
    In2BufferImages *m_data;
    InkjetXusbVarCtrl* newCard;
    int m_BufferSize;
    int m_state;
    int m_TotalSwatheSent;
    int m_DataOrigin;
    bool m_memoryEmpty;


};

#endif // IMAGEVARXAARSENDER_H
