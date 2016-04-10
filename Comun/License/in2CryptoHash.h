
#ifndef IN2CRYPTOHASH_H
#define IN2CRYPTOHASH_H

#include <QWidget>
#include <QString>
#include <QStringList>
#include <QCryptographicHash>
#include <QByteArray>


#define CRYPTO_ALGORITMO_MD4    0
#define CRYPTO_ALGORITMO_MD5    1
#define CRYPTO_ALGORITMO_SHA1   2


class in2CryptoHash : public QObject
{
    Q_OBJECT

public:
    explicit in2CryptoHash(QWidget *parent=0);
    ~in2CryptoHash();

signals:


public slots:
    void resetHash();
    QString getHash();
    QString getHash(unsigned char algoritmo);
    void putIntoHash(QByteArray inputData);

private slots:

private:
    QString hexHash;
    QByteArray numeroHash;
    QByteArray tmp;

protected:
};


#endif // IN2CRYPTOHASH_H

