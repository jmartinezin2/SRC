#ifndef IN2AISCONFIGURATOR_H
#define IN2AISCONFIGURATOR_H

#include <QObject>
#include <QVector>
#include "in2aisvisionobject.h"

class In2AISConfigurator : public QObject
{
    Q_OBJECT
public:
    explicit In2AISConfigurator(QObject *parent = 0);
    bool  readConfig(QString file);
    QVector<In2AISVisionObject> getAISVisionObjects();
    bool saveConfig(QString file,QVector<In2AISVisionObject> & lista);
    QString getStringAIS();
    QString getStringAISFromList(QVector<In2AISVisionObject> & lista);
    QString getStringLoad(QString txt);
    QString getStringConfig(QString txt);
    QString getStringSave(QString txt);
    QString getStringStop();
    
private:
    int m_count;
    QVector<In2AISVisionObject> m_lista;

signals:
    
public slots:
    
};

#endif // IN2AISCONFIGURATOR_H
