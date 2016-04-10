#ifndef IN2EVENTFILTER_H
#define IN2EVENTFILTER_H

#include <QObject>
#include <QEvent>
#include <QTimer>

class In2EventFilter : public QObject
{
    Q_OBJECT
public:
    static In2EventFilter *request(QObject *parent);
    void reset();

private:
    explicit In2EventFilter(QObject *parent = 0);
    
protected:
    virtual bool eventFilter(QObject* object,QEvent* event);
signals:
    void timeout();

private:
    static In2EventFilter *mInstance;
    QTimer m_timer;
public slots:
    
};

#endif // IN2EVENTFILTER_H
