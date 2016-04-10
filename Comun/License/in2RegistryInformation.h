/** @addtogroup InkjetForm
 *  @{*/
/** @file in2RegistryInformation.h
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright In2 Printing Solutions. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Dialogo principal para la aplicación inkjet
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: in2RegistryInformation.h
 *    @li Versión: x.0
 *    @li Fecha: 01/04/08
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */

#ifndef IN2REGISTRYINFORMATION_H
#define IN2REGISTRYINFORMATION_H

#include <QWidget>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QCryptographicHash>
#include <QByteArray>

#define ID_HKEY_CLASSES_ROOT    0
#define ID_HKEY_CURRENT_USER    1
#define ID_HKEY_LOCAL_MACHINE   2
#define ID_HKEY_USERS           3
#define ID_HKEY_CURRENT_CONFIG  4

#define STR_HKEY_CLASSES_ROOT    0
#define STR_HKEY_CURRENT_USER    1
#define STR_HKEY_LOCAL_MACHINE   2
#define STR_HKEY_USERS           3
#define STR_HKEY_CURRENT_CONFIG  4



class in2RegistryInformation : public QWidget
{
    Q_OBJECT

public:
    explicit in2RegistryInformation(QWidget *parent=0);
    ~in2RegistryInformation();
    QString getValue(int idHKEY, QString key);
    QString getValue(QString strHKEY, QString key);
    QString getRegistryInfo();

signals:
    void sglRegistryInfo(QString info);

public slots:

private slots:

private:

protected:
};


#endif // IN2REGISTRYINFORMATION_H
