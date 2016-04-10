#ifndef IN2APPLICATION_H
#define IN2APPLICATION_H

#include <QApplication>


class In2Application : public QApplication
{
    Q_OBJECT
public:
    explicit In2Application(int argc ,char** argv);


protected:
    virtual bool notify(QObject* object,QEvent* event);

signals:
    
public slots:
    
};

#endif // IN2APPLICATION_H
