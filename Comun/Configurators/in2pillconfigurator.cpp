#include "in2pillconfigurator.h"
#include <QApplication>
#include <QFile>
#include <QStringList>
#include <QSettings>

#define DEFAULT_CONFIG_FILE "In2Pills.ini"

#define PRINTABLE "PRINTABLE"
#define DIAMETER   "Diameter"
#define X0         "X0"
#define Y0         "Y0"
#define WIDTH      "Width"
#define HEIGHT     "Height"
#define ITEMS       "Items"
#define ITEM        "ITEM"
#define OFFSET      "Offset"
#define FILE        "File"
#define NAME        "Name"
#define VPROGRAM    "VisionProgram"

In2PillConfigurator::In2PillConfigurator(QObject *parent) :
    QObject(parent)
{
}
/** Lee la configuracion*/
void In2PillConfigurator::readConfig(){
    QString strPath=QString("%1\\Config\\%2").arg(QApplication::applicationDirPath()).arg(DEFAULT_CONFIG_FILE);

    m_pillDiameter.clear();
    m_x0.clear();
    m_y0.clear();
    m_width.clear();
    m_height.clear();
    m_pill.clear();

    if (QFile::exists(strPath)){
        QSettings FicheroIni(strPath,QSettings::IniFormat,0);
        FicheroIni.beginGroup(PRINTABLE);
        m_pillDiameter=FicheroIni.value(DIAMETER,"").toString();
        m_x0=FicheroIni.value(X0,"").toString();
        m_y0=FicheroIni.value(Y0,"").toString();
        m_width=FicheroIni.value(WIDTH,"").toString();
        m_height=FicheroIni.value(HEIGHT,"").toString();
        m_items=FicheroIni.value(ITEMS,0).toInt();
        FicheroIni.endGroup();
        QStringList lst=FicheroIni.childGroups();
        for (int x=0;x<m_items;x++){
            QString foo=QString("%1%2").arg(ITEM).arg(x);
            if (lst.contains(foo)){
                FicheroIni.beginGroup(foo);
                struct strPill fooPill;
                fooPill.m_file=FicheroIni.value(FILE,"").toString();
                fooPill.m_Offset=FicheroIni.value(OFFSET,0).toInt();
                fooPill.m_name=FicheroIni.value(NAME,"").toString();
                fooPill.m_VProgram=FicheroIni.value(VPROGRAM,0).toInt();
                m_pill.append(fooPill);
                FicheroIni.endGroup();
            }
        }
    }
}
/** Devueñve el vector de pildoras*/
QVector<struct strPill> In2PillConfigurator::getPillVector(){
    return m_pill;
}

/** Guarda los valores*/
void In2PillConfigurator::saveConfig(){
    QString strPath=QString("%1\\Config\\%2").arg(QApplication::applicationDirPath()).arg(DEFAULT_CONFIG_FILE);

    if (QFile::exists(strPath)){
        QSettings FicheroIni(strPath,QSettings::IniFormat,0);
        FicheroIni.beginGroup(PRINTABLE);
        FicheroIni.setValue(DIAMETER,m_pillDiameter);
        FicheroIni.setValue(X0,m_x0);
        FicheroIni.setValue(Y0,m_y0);
        FicheroIni.setValue(WIDTH,m_width);
        FicheroIni.setValue(HEIGHT,m_height);
        FicheroIni.sync();
    }
}

/** Devuelve el rectangulo*/
QRect In2PillConfigurator::getPrintableRect(){
    QRect rect;
    //if ((m_x0.isNull())||(m_y0.isNull())||(m_width.isNull())||(m_height.isNull())){
        int x0=m_x0.toInt();
        int y0=m_y0.toInt();
        int width=m_width.toInt();
        int height=m_height.toInt();
        rect=QRect(x0,y0,width,height);
    //}
    return rect;
}
/** Devuelve el diametro*/
QString In2PillConfigurator::getPillDiameter(){
    return m_pillDiameter;
}
/**  Devuelve el X0*/
QString In2PillConfigurator::getX0(){
    return m_x0;
}
/** devuelve el Y0*/
QString In2PillConfigurator::getY0(){
    return m_y0;
}
/** Devuelve el width*/
QString In2PillConfigurator::getWidth(){
    return m_width;
}
/** Devuelve el height*/
QString In2PillConfigurator::getHeight(){
    return m_height;
}

/** carga el diametro*/
void In2PillConfigurator::setPillDiameter(QString val){
    m_pillDiameter=val;
}
/**  carga el X0*/
void In2PillConfigurator::setX0(QString val){
    m_x0=val;
}
/** carga el Y0*/
void In2PillConfigurator::setY0(QString val){
    m_y0=val;
}
/** carga el width*/
void In2PillConfigurator::setWidth(QString val){
    m_width=val;
}
/** Carga el height*/
void In2PillConfigurator::setHeight(QString val){
    m_height=val;
}



