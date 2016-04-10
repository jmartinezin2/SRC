#include <QStringList>
#include <string.h>
#include <QSettings>
#include <QPalette>
#include <QProcess>
#include <QProcess>
#include <windows.h>
#include <QDebug>
#include <QCryptographicHash>
#include <QSettings>
#include <QNetworkInterface>
#include <QCryptographicHash>
#include <winnt.h>
#include "windows.h"
#include "winbase.h"

#include "in2SystemInformation.h"
//#include "in2BIOSInformation.h"


//Constructor 
in2SystemInformation::in2SystemInformation(QWidget *parent)
{

    // extract Windows Registry settings and save the data we're interested in..Qt method
    QSettings settings( "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft", QSettings::NativeFormat );

    // windows system identifiers..notcase sensitive..fyi
    QStringList wids;

    wids.append( "1:" + settings.value( "Windows NT/CurrentVersion/ProductId" ).toString().trimmed() );
    wids.append( "2:" + settings.value( "Internet Explorer/Registration/ProductID" ).toString().trimmed() );
    wids.append( "3:" + settings.value( "Cryptography/MachineGuid" ).toString().trimmed() );

}

in2SystemInformation::~in2SystemInformation(){

}



QString in2SystemInformation::getVolumeInformation(){

    QString rtn;
    QString dummy;
    TCHAR volumeName[MAX_PATH + 1] = { 0 };
    //TCHAR volumeName;
    TCHAR fileSystemName[MAX_PATH + 1] = { 0 };

    DWORD serialNumber = 0;
    DWORD maxComponentLen = 0;
    DWORD fileSystemFlags = 0;
    if (GetVolumeInformation(
            TEXT("C:\\"),
            volumeName,
                /*ARRAYSIZE(volumeName)*/MAX_PATH+1,
            &serialNumber,
            &maxComponentLen,
            &fileSystemFlags,
            fileSystemName,
            /*ARRAYSIZE(fileSystemName)*/MAX_PATH+1))
        {

            QString foo=QString().fromUtf16((const ushort*)volumeName);
            dummy.append(QString("Volume Name: %1\n").arg(foo));
            dummy.append(QString("Serial Number: %1\n").arg(serialNumber));
            foo=QString().fromUtf16((const ushort*)fileSystemName);
            dummy.append(QString("File System Name: %1\n").arg(foo));
            dummy.append(QString("Max Component Length: %1\n").arg(maxComponentLen));
        }

        rtn.append(QString("%1").arg(dummy));
        return (rtn);

    /*
    Volume Name:  0x23f3c0
    Serial Number:  2397160479
    File System Name:  0x23f5d0
    Max Component Length:  255
     */
}

QString in2SystemInformation::getProcessorInfo(){

    QString rtn;

    QString processorInfo = QString("Architecture:%1 \nNumber of Processors:%2 \nProcessor Identifier:%3\n")
            .arg(getenv("PROCESSOR_ARCHITECTURE"))
            .arg(getenv("NUMBER_OF_PROCESSORS"))
            .arg(getenv("PROCESSOR_IDENTIFIER"));

    rtn.append(processorInfo);
    return (rtn);

    /*
    "Architecture:AMD64
    Number of Processors:4
    Processor Identifier:Intel64 Family 6 Model 58 Stepping 9, GenuineIntel"
    */
}


QString in2SystemInformation::getSystemInfo(){

    QString rtn;

    SYSTEM_INFO siSysInfo;
    // Copy the hardware information to the SYSTEM_INFO structure.
    GetSystemInfo(&siSysInfo); //Funciona ok
    /*
      wProcessorArquitecture = 9
        PROCESSOR_ARCHITECTURE_AMD64    9           x64 (AMD or Intel)
        PROCESSOR_ARCHITECTURE_ARM      Reserved    ARM
        PROCESSOR_ARCHITECTURE_IA64     6           Intel Itanium-based
        PROCESSOR_ARCHITECTURE_INTEL    0           x86
        PROCESSOR_ARCHITECTURE_UNKNOWN  0xffff      Unknown architecture

      dwNumberOfProcessors = 4

      dwProcessorType = 8664
        PROCESSOR_INTEL_386 (386)
        PROCESSOR_INTEL_486 (486)
        PROCESSOR_INTEL_PENTIUM (586)
        PROCESSOR_INTEL_IA64 (2200)
        PROCESSOR_AMD_X8664 (8664)
        PROCESSOR_ARM (Reserved)

      dwProcessorLevel = 6
        If wProcessorArchitecture is PROCESSOR_ARCHITECTURE_INTEL, wProcessorLevel is defined by the CPU vendor.
        If wProcessorArchitecture is PROCESSOR_ARCHITECTURE_IA64, wProcessorLevel is set to 1.

      dwProcessorRevision = 14857
     */



    rtn.append(QString("NumberOfProcessors = %1\n").arg(siSysInfo.dwNumberOfProcessors));

    return (rtn);

}


QString in2SystemInformation::getComputerName(){

    QString rtn;

    //TCHAR buffer[256] = TEXT("");
    char buffer[256];
    QStringList zDescription;
    zDescription << "NetBIOS" << "DNS hostname" << "DNS domain" << "DNS fully-qualified" << "Physical NetBIOS" << "Physical DNS hostname" << "Physical DNS domain" << "Physical DNS fully-qualified";
    int cnf = 0;
    DWORD dwSize = sizeof(buffer);
    //cnf=ComputerNameNetBIOS;
    if (!GetComputerNameA(&buffer[0],&dwSize)){

    /*if (!GetComputerNameEx((COMPUTER_NAME_FORMAT)cnf, buffer, &dwSize)) {*/
    }
    else {
        //QString s=QString::fromWCharArray(buffer,32);
        QString s(buffer);
        rtn.append(QString("%1 %2\n").arg(zDescription.at(cnf)).arg(s));
     }
     /*dwSize = _countof(buffer);
     ZeroMemory(buffer, dwSize);*/
     return (rtn);
}


QString in2SystemInformation::getProductInfo(){

    QString rtn;

    OSVERSIONINFO osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&osvi);

    rtn.append(QString("ProductInfo MajorVersion: %1\n").arg(osvi.dwMajorVersion));
    rtn.append(QString("ProductInfo MinorVersion: %1\n").arg(osvi.dwMinorVersion));

    return (rtn);

}


QString in2SystemInformation::getBIOSInfo(){

    QString rtn;
/*
    DWORD needBufferSize = 0;
    // the seqence just for x86, but don't worry we know SMBIOS/DMI only exist on x86 platform
    const BYTE byteSignature[] = { 'B', 'M', 'S', 'R' };
    const DWORD Signature = *((DWORD*)byteSignature);
    LPBYTE pBuff = NULL;
    needBufferSize = GetSystemFirmwareTable(Signature, 0, NULL, 0);
    //_tprintf(TEXT("We need prepare %d bytes for recevie SMBIOS/DMI Table\n"), needBufferSize);
    pBuff = (LPBYTE) malloc(needBufferSize);
    if (pBuff)
    {
        GetSystemFirmwareTable(Signature, 0, pBuff, needBufferSize);
        const PRawSMBIOSData pDMIData = (PRawSMBIOSData)pBuff;
        BYTE majorVer = pDMIData->SMBIOSMajorVersion;
        BYTE minorVer = pDMIData->SMBIOSMinorVersion;
        BYTE dmiVer = pDMIData->DmiRevision;
        DWORD len = pDMIData->Length;

        emit sglSystemInfo(QString("SMBIOS Major version: %1").arg(majorVer));
        emit sglSystemInfo(QString("SMBIOS Minor version: %1").arg(minorVer));
        emit sglSystemInfo(QString("DMI Revision: %1").arg(dmiVer));
        emit sglSystemInfo(QString("Total length: %1").arg(len));

        rtn.append(majorVer);
        rtn.append(minorVer);
        rtn.append(dmiVer);
        rtn.append(len);

        //_tprintf(TEXT("SMBIOS version:%d.%d\n"), pDMIData->SMBIOSMajorVersion, pDMIData->SMBIOSMinorVersion);
        //_tprintf(TEXT("DMI Revision:%x\n"), pDMIData->DmiRevision);
        //_tprintf(TEXT("Total length: %d\n"), pDMIData->Length);
        //_tprintf(TEXT("DMI at address %x\n"), (DWORD)((PBYTE)&pDMIData->SMBIOSTableData));
        //DumpSMBIOSStruct(&(pDMIData->SMBIOSTableData), pDMIData->Length);
    }
    //else
        //_tprintf(TEXT("Can not allocate memory for recevice SMBIOS/DMI table\n"));

    if (pBuff)  free(pBuff);
*/
    return (rtn);

}


QString in2SystemInformation::getIPAddresses(){
    
    QString rtn;

	QList<QHostAddress> list = QNetworkInterface::allAddresses();
    for (int i=0 ; i < list.size() ; i++){
        rtn.append(QString("IPAddress %1 = %2\n").arg(i+1).arg(list.at(i).toString()));
    }

    return (rtn);

}


QString in2SystemInformation::getMACs(){

    QString rtn;

    QList<QNetworkInterface> listInterfaces = QNetworkInterface::allInterfaces();
    for (int i=0 ; i < listInterfaces.size() ; i++){
        rtn.append(QString("MAC %1 = %2 %3 %4\n")
                   .arg(i+1)
                   .arg(listInterfaces.at(i).hardwareAddress())
                   .arg(listInterfaces.at(i).humanReadableName())
                   .arg(listInterfaces.at(i).name()));
        /*int flags=listInterfaces.at(i).flags();
        if (flags&QNetworkInterface::IsRunning)
            rtn.append(QString("MAC %1 = %2\n")
                           .arg(i+1)
                           .arg(listInterfaces.at(i).hardwareAddress()));*/

    }

    return (rtn);

}

