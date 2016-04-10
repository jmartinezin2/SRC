#ifndef IN2AISSTRINGGEN_H
#define IN2AISSTRINGGEN_H

#include <QObject>
#include <QVector>
#include "in2aisvisionobject.h"

class In2AISStringGen : public QObject
{
    Q_OBJECT
public:
    explicit In2AISStringGen(QObject *parent = 0);
    void setItems(QVector<In2AISVisionObject> & lst);
    QString getString(QVector<In2AISVisionObject> &lst);
    QString getStringLoadFile(QString file);
    QString getStringSaveFile(QString file);
    QString getStringStop();
    QString getStringConfig(QString file);
    QString completeOCRString(QString txt);

    In2AISVisionObject getTemplateGeneral(QString txt);
    In2AISVisionObject getTemplateCabezal(QString txt);
    QVector<In2AISVisionObject> getOCRList(QString txt);

private:
    void _getTypes();

private:
    int m_totalOCR;
    int m_totalTemplate1;
    int m_totalTemplate2;
    QVector<In2AISVisionObject> m_lista;
signals:
    
public slots:
    
};

#endif // IN2AISSTRINGGEN_H
