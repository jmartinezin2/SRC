#ifndef IN2HASH_H
#define IN2HASH_H

#include <QObject>

class In2Hash : public QObject
{
    Q_OBJECT
public:
    explicit In2Hash(QObject *parent = 0);
    ~In2Hash();
    QByteArray CalculateHash(QByteArray DatosEncriptar);
    QByteArray GenerateHashFromFile(const QString &strFile);
    bool TestHash(QByteArray DatosEncriptar,QByteArray ClaveEncriptada);
    bool HashOk(QByteArray HashLeido,QByteArray DatosEncriptar);


signals:

public slots:
};

#endif // IN2HASH_H
