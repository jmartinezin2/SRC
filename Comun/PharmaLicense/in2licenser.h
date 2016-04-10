#ifndef IN2LICENSER_H
#define IN2LICENSER_H

#include <QObject>


#define LICENSE  "wmic baseboard get product,Manufacturer,version,serialnumber"

#define ERROR_NO_MOTHERBOARD 1300
#define ERROR_NO_LICENSE    1301
#define ERROR_HARDWARE_DIFFERENT 1302
#define ERROR_LICENSE_EXPIRED   1303
#define ERROR_LICENSE_DATE_ERROR   1304
#define ERROR_LICENSE_COUNTER_EXPIRED 1305


typedef struct In2License{
    QString m_mother;
    QString m_temp;
    QString m_end;
    QString m_endCounter;
    QString m_counter;
}IN2LICENSE;

class In2Licenser : public QObject
{
    Q_OBJECT
public:
    explicit In2Licenser(QObject *parent = 0);
    struct In2License readLicense();
    struct In2License readLicenseFromBBDD(QString file);
    QString checkLicenseFromBBDD(QString bbdd);
    QString encryptLicense(struct In2License & license);
    struct In2License decryptLicense(QString str);
    bool saveLicenseToBBDD(QString bbdd,QString license);
    QVector<int> CheckLicense(struct In2License &license);
    bool deleteLicenseFromBBDD(QString bbdd);

private:
    QString _readMother();

signals:
    void sgnLicenseError(int error);


private slots:
    void sltError(int error);


private:
    int m_error;

public slots:
};

#endif // IN2LICENSER_H
