/** @addtogroup InkjetForm
 *  @{*/
/** @file in2Licenser.h
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright In2 Printing Solutions. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Dialogo principal para la aplicación inkjet
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: in2Licenser.h
 *    @li Versión: x.0
 *    @li Fecha: 01/04/08
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */

#ifndef IN2LICENSER_H
#define IN2LICENSER_H


#include <QString>
#include <QStringList>

#include "in2SystemInformation.h"
#include "in2RegistryInformation.h"
//#include "in2BIOSInformation.h"
#include "in2CryptoHash.h"

#define LICENSE_PATH "HKEY_LOCAL_MACHINE\\SOFTWARE\\IN2"
#define LICENSE_KEY "key"

class in2Licenser : public QObject
{
	Q_OBJECT

public:
    explicit in2Licenser(QWidget *parent=0);
    ~in2Licenser();
    QString getInfo();

signals:
    void sglReadInstallationHashRtn(QString outputData);
    void sglReadInstallationInfoRtn(QString outputData);

public slots:	
    void init();
    void sltSystemInfo(QString infoString);
    void sltRegistryInfo(QString infoString);
    QString  sltGenerateLicenseType1();
    void sltWriteLicenseType1();
    bool sltCheckLicense();


signals:
    void sgnNoIn2License();

private slots:

private:
    QString m_info;

protected:
};


#endif // IN2LICENSER_H
