
#ifndef IN2SYSTEMINFORMATION_H
#define IN2SYSTEMINFORMATION_H

#include <QWidget>
#include <QString>
#include <QStringList>
#include <QCryptographicHash>
#include <QByteArray>

class in2SystemInformation : public QWidget
{
    Q_OBJECT

public:
    explicit in2SystemInformation(QWidget *parent=0);
    ~in2SystemInformation();

signals:
    void sglSystemInfo(QString info);

public slots:
    QString getVolumeInformation();
    QString getProcessorInfo();
    QString getSystemInfo();
    QString getComputerName();
    QString getProductInfo();
    QString getIPAddresses();
    QString getMACs();
    QString getBIOSInfo();

private slots:

private:

protected:
};


#endif // IN2SYSTEMINFORMATION_H

