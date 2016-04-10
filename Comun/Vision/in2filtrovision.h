#ifndef IN2FILTROVISION_H
#define IN2FILTROVISION_H

#include <QObject>
//#include <QTime>
#include <QElapsedTimer>

class In2FiltroVision
{
public:
    In2FiltroVision(/*QObject *parent = 0*/);
    bool isEnabled();
    void setEnabled(bool enabled);
    unsigned short getNumErrores();
    unsigned short getCounter();
    void setNumErrores(unsigned short num);
    quint64 getTime();
    void setTime(quint64 time);
    bool updateCounter(unsigned short counter);
    void startTimer();
    void stopTimer();
    void setItemOPC(int item);
    int getItemOPC();
    quint64 getLastElapsedTime();
    quint64 getCurrentElapsed();
    QString dbgTime();
signals:
    //void SendDebugInfo(unsigned char val,QString txt);

private:
    bool m_bEnabled;
    unsigned short  m_counter;
    unsigned short  m_numErrores;
    quint64  m_time;
    quint64 m_lastElapsed;
    int m_itemOPC;
    QElapsedTimer m_timeElapsed;
public slots:
    
};

//Q_DECLARE_TYPEINFO(In2FiltroVision, Q_MOVABLE_TYPE);

#endif // IN2FILTROVISION_H
