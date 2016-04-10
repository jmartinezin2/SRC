#ifndef IN2XMLPARSER_H
#define IN2XMLPARSER_H

#include <QObject>
#include <QFile>
#include <QDomDocument>
#include <QStringList>

class In2XmlParser : public QObject
{
    Q_OBJECT
public:
    explicit In2XmlParser(QObject *parent = 0);
    In2XmlParser(QObject *parent = 0,QString file="");
    bool setValue(QStringList PathKey,QString attribute, QString value);
    QString getValue(QStringList PathKey, QString attribute);
    bool saveContentsToFile(QString fileName);

private:
    QDomDocument m_qdomDocument;
    bool m_enabled;

signals:
    
public slots:
    
};

#endif // IN2XMLPARSER_H
