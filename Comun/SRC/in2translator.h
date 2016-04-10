#ifndef IN2TRANSLATOR_H
#define IN2TRANSLATOR_H

#include <QObject>

class In2Translator : public QObject
{
    Q_OBJECT
public:
    explicit In2Translator(QObject *parent = 0);
    bool changeLanguaje(QString languaje);
    
signals:
    
public slots:
    
};

#endif // IN2TRANSLATOR_H
