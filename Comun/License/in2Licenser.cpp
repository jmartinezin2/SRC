#include <QStringList>
#include <QDebug>

#include "in2Licenser.h"
#include "in2SystemInformation.h"
#include "in2RegistryInformation.h"
//#include "in2BIOSInformation.h"
#include "in2CryptoHash.h"
#include <QSettings>




//Constructor 
in2Licenser::in2Licenser(QWidget *parent)
{

    /*ScrIn2SystemInformation = new in2SystemInformation(0);
    connect(ScrIn2SystemInformation,SIGNAL(sglSystemInfo(QString)),this,SLOT(sltSystemInfo(QString)));
    ScrIn2RegistryInformation = new in2RegistryInformation(0);
    connect(ScrIn2RegistryInformation,SIGNAL(sglRegistryInfo(QString)),this,SLOT(sltRegistryInfo(QString)));
    ScrIn2CryptoHash = new in2CryptoHash(0);*/

}

in2Licenser::~in2Licenser(){

}


void in2Licenser::init(){


}

/** Calcula un hash tipo 1*/
QString in2Licenser::sltGenerateLicenseType1(){

    QString tmp;
    QString result;
    in2SystemInformation *In2SystemInformation = new in2SystemInformation(0);
    in2RegistryInformation *In2RegistryInformation = new in2RegistryInformation(0);
    in2CryptoHash *In2CryptoHash = new in2CryptoHash(0);
    tmp.clear();
    result.clear();
    In2CryptoHash->resetHash();
    tmp="001\n";
    m_info.append(tmp);
    In2CryptoHash->putIntoHash(tmp.toUtf8());
    tmp=In2SystemInformation->getVolumeInformation();
    m_info.append(tmp);
    In2CryptoHash->putIntoHash(tmp.toUtf8());
    tmp=In2SystemInformation->getProductInfo();
    m_info.append(tmp);
    In2CryptoHash->putIntoHash(tmp.toUtf8());
    tmp=In2SystemInformation->getBIOSInfo();
    m_info.append(tmp);
    In2CryptoHash->putIntoHash(tmp.toUtf8());
    /*tmp=In2SystemInformation->getMACs();
    m_info.append(tmp);
    In2CryptoHash->putIntoHash(tmp.toUtf8());*/
    //tmp=In2SystemInformation->getIPAddresses();
    tmp=In2RegistryInformation->getRegistryInfo();
    m_info.append(tmp);
    In2CryptoHash->putIntoHash(tmp.toUtf8());
    result=In2CryptoHash->getHash(CRYPTO_ALGORITMO_MD5);

    delete (In2SystemInformation);
    delete (In2RegistryInformation);
    delete (In2CryptoHash);
    return result;
}
/** Guarda la licencia tipo 1*/
void in2Licenser::sltWriteLicenseType1(){
    QString foo=sltGenerateLicenseType1();
    QSettings settings(LICENSE_PATH,QSettings::NativeFormat);
    QString keyName = LICENSE_KEY;
    settings.setValue(keyName,foo);
    settings.sync();

}

bool in2Licenser::sltCheckLicense(){
    bool rtn=false;
    QSettings settings(LICENSE_PATH,QSettings::NativeFormat);
    QString keyName = LICENSE_KEY;
    QString lic=settings.value(keyName,"").toString();
    if (lic.isEmpty()){
        emit sgnNoIn2License();
    }
    else{
       QString val=sltGenerateLicenseType1();
       if (!lic.compare(val)){
            rtn=true;
       }
    }
    return rtn;

}

/** Devuelve la info de la licencia*/
QString in2Licenser::getInfo(){
    return m_info;
}


void in2Licenser::sltSystemInfo(QString infoString){
    emit sglReadInstallationInfoRtn(infoString);

}

void in2Licenser::sltRegistryInfo(QString infoString){
    emit sglReadInstallationInfoRtn(infoString);
}


