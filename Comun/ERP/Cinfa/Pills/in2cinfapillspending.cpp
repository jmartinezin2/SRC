#include "in2cinfapillspending.h"


#include <QFile>
#include <QApplication>
#include <QSettings>

#define IN2CINFAPILLS_PENDING "In2CinfaPillsPending.ini"
#define SEPARATOR "<@>"
In2CinfaPillsPending::In2CinfaPillsPending(QObject *parent) :
    QObject(parent)
{

}
/** dice si hay una orden pendiente*/
bool In2CinfaPillsPending::orderPending(){
    QString txt=QString("%1\\config\\%2").arg(QApplication::applicationDirPath()).arg(IN2CINFAPILLS_PENDING);
    return QFile::exists(txt);
}

/** Elimina una orden pendiente*/
void In2CinfaPillsPending::deletePending(){
    QString txt=QString("%1\\config\\%2").arg(QApplication::applicationDirPath()).arg(IN2CINFAPILLS_PENDING);
    QFile::remove(txt);
}

/** Guarda una orden pendiente*/
void In2CinfaPillsPending::saveOrder(QStringList &msg){
    QString txt=QString("%1\\config\\%2").arg(QApplication::applicationDirPath()).arg(IN2CINFAPILLS_PENDING);
    QFile::remove(txt);
    QFile file(txt);
    if (file.open(QIODevice::WriteOnly)){
        QString toSave=msg.join(SEPARATOR);
#if QT_VERSION>=0x050000
        file.write(toSave.toLatin1());
#else
        file.write(toSave.toAscii());
#endif

    }
    file.close();
}

/** Devuelve una orden*/
QStringList In2CinfaPillsPending::getOrder(){
    QStringList rtn;
    QString txt=QString("%1\\config\\%2").arg(QApplication::applicationDirPath()).arg(IN2CINFAPILLS_PENDING);
    QFile file(txt);
    if (file.open(QIODevice::ReadOnly)){
        QByteArray byteArray=file.readAll();
        QString txt=QString(byteArray);
        rtn=txt.split(SEPARATOR);
    }
    return rtn;
}
