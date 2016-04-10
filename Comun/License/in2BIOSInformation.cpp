// DumpSMBIOS.cpp : Defines the entry point for the console application.
//

#include "in2BIOSInformation.h"

#include <QObject>
#include <QWidget>

//Constructor

in2BIOSInformation::in2BIOSInformation(QWidget *parent)
{

}

in2BIOSInformation::~in2BIOSInformation(){

}



const char* in2BIOSInformation::getHeaderStringA(const UINT type)
{
    static const char* PRT_HEADER[] =
    {
        "-=======================================================-",
        "========== BIOS information ==========",
        "========== System information ==========",
        "========== Base Board information ==========",
        "========== System Enclosure information ==========",
        "========== Processor information ==========",
        "========== Memory Controller information ==========",
        "========== Memory Module information ==========",
        "========== Cache information ==========",
        "========== Port Connector Information ==========",
        "========== System Slots ==========",
        "========== On Board Devices Information ==========",
        "========== OEM String ==========",
        "========== System Configuration Options ==========",
        "========== BIOS Language Information ==========",
        "========== Group Associations ==========",
        "========== System Event Log ==========",
        "========== Physical Memory Array ==========",
        "========== Memory Device ==========",
        "========== Memory Error Information ==========",
        "========== Memory Array Mapped Address ==========",
    };
    if (cstrHEADER == type)
    return PRT_HEADER[0];
    return PRT_HEADER[type + 1];
}


const wchar_t* in2BIOSInformation::getHeaderStringW(const UINT type)
{
    static wchar_t buff[2048];
    const char* pStr = getHeaderStringA(type);
    SecureZeroMemory(buff, sizeof(buff));
    MultiByteToWideChar(CP_OEMCP, 0, pStr, strlen(pStr), buff, sizeof(buff));
    return buff;
}

const char* in2BIOSInformation::LocateStringA(const char* str, UINT i)
{
    static const char strNull[] = "Null String";
    if (0 == i || 0 == *str) return strNull;
    while (--i)
    {
    str += strlen((char*)str) + 1;
    }
    return str;
}


const wchar_t* in2BIOSInformation::LocateStringW(const char* str, UINT i)
{
    static wchar_t buff[2048];
    const char *pStr = LocateStringA(str, i);
    SecureZeroMemory(buff, sizeof(buff));
    MultiByteToWideChar(CP_OEMCP, 0, pStr, strlen(pStr), buff, sizeof(buff));
    return buff;
}


const char* in2BIOSInformation::toPointString(void* p)
{
    return (char*)p + ((PSMBIOSHEADER)p)->Length;
}


bool in2BIOSInformation::ProcBIOSInfo(void* p)
{
    PBIOSInfo pBIOS = (PBIOSInfo)p;
    const char *str = toPointString(p);
    //_tprintf(TEXT("%s\n"), getHeaderString(0));
    //_tprintf(TEXT("Vendor: %s\n"), LocateString(str, pBIOS->Vendor));
    //_tprintf(TEXT("Version: %s\n"), LocateString(str, pBIOS->Version));
    //_tprintf(TEXT("BIOS Starting Segment: 0x%X\n"), pBIOS->StartingAddrSeg);
    //_tprintf(TEXT("Release Date: %s\n"), LocateString(str, pBIOS->ReleaseDate));
    //_tprintf(TEXT("Image Size: %dK\n"), (pBIOS->ROMSize + 1) * 64);

    if (pBIOS->Header.Length > 0x14)
    { // for spec v2.4 and later
        //_tprintf(TEXT("System BIOS version: %d.%d\n"), pBIOS->MajorRelease, pBIOS->MinorRelease);
        //_tprintf(TEXT("EC Firmware version: %d.%d\n"), pBIOS->ECFirmwareMajor, pBIOS->ECFirmwareMinor);
    }
    return true;
}



bool in2BIOSInformation::ProcSysInfo(void* p)
{
    PSystemInfo pSystem = (PSystemInfo)p;
    const char *str = toPointString(p);
    //_tprintf(TEXT("%s\n"), getHeaderString(1));
    //_tprintf(TEXT("Manufacturer: %s\n"), LocateString(str, pSystem->Manufacturer));
    //_tprintf(TEXT("Product Name: %s\n"), LocateString(str, pSystem->ProductName));
    //_tprintf(TEXT("Version: %s\n"), LocateString(str, pSystem->Version));
    //_tprintf(TEXT("Serial Number: %s\n"), LocateString(str, pSystem->SN));
    // for v2.1 and later
    if (pSystem->Header.Length > 0x08)
    {
        //_tprintf(TEXT("UUID: %02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X\n"),
        //pSystem->UUID[0], pSystem->UUID[1], pSystem->UUID[2], pSystem->UUID[3],
        //pSystem->UUID[4], pSystem->UUID[5], pSystem->UUID[6], pSystem->UUID[7],
        //pSystem->UUID[8], pSystem->UUID[9], pSystem->UUID[10], pSystem->UUID[11],
        //pSystem->UUID[12], pSystem->UUID[13], pSystem->UUID[14], pSystem->UUID[15]);
    }
    if (pSystem->Header.Length > 0x19)
    {
        // fileds for spec. 2.4
        //_tprintf(TEXT("SKU Number: %s\n"), LocateString(str, pSystem->SKUNumber));
        //_tprintf(TEXT("Family: %s\n"), LocateString(str, pSystem->Family));
    }
    return true;
}


bool in2BIOSInformation::ProcBoardInfo(void* p)
{
    PBoardInfo pBoard = (PBoardInfo)p;
    const char *str = toPointString(p);
    //_tprintf(TEXT("%s\n"), getHeaderString(2));
    //_tprintf(TEXT("Length: 0x%X\n"), pBoard->Header.Length);
    //_tprintf(TEXT("Manufacturer: %s\n"), LocateString(str, pBoard->Manufacturer));
    //_tprintf(TEXT("Product Name: %s\n"), LocateString(str, pBoard->Product));
    //_tprintf(TEXT("Version: %s\n"), LocateString(str, pBoard->Version));
    //_tprintf(TEXT("Serial Number: %s\n"), LocateString(str, pBoard->SN));
    //_tprintf(TEXT("Asset Tag Number: %s\n"), LocateString(str, pBoard->AssetTag));
    if (pBoard->Header.Length > 0x08)
    {
        //_tprintf(TEXT("Location in Chassis: %s\n"), LocateString(str, pBoard->LocationInChassis));
    }
    return true;
}



bool in2BIOSInformation::ProcSystemEnclosure(void* p)
{
    PSystemEnclosure pSysEnclosure = (PSystemEnclosure)p;
    const char *str = toPointString(p);
    //_tprintf(TEXT("%s\n"), getHeaderString(3));
    //_tprintf(TEXT("Length: 0x%X\n"), pSysEnclosure->Header.Length);
    //_tprintf(TEXT("Manufacturer: %s\n"), LocateString(str, pSysEnclosure->Manufacturer));
    //_tprintf(TEXT("Version: %s\n"), LocateString(str, pSysEnclosure->Version));
    //_tprintf(TEXT("Serial Number: %s\n"), LocateString(str, pSysEnclosure->SN));
    //_tprintf(TEXT("Asset Tag Number: %s\n"), LocateString(str, pSysEnclosure->AssetTag));
    return true;
}


bool in2BIOSInformation::ProcProcessorInfo(void* p)
{
    PProcessorInfo pProcessor = (PProcessorInfo)p;
    const char *str = toPointString(p);
    //_tprintf(TEXT("%s\n"), getHeaderString(4));
    //_tprintf(TEXT("Length: 0x%X\n"), pProcessor->Header.Length);
    //_tprintf(TEXT("Socket Designation: %s\n"), LocateString(str, pProcessor->SocketDesignation));
    //_tprintf(TEXT("Processor Manufacturer: %s\n"), LocateString(str, pProcessor->Manufacturer));
    //_tprintf(TEXT("Processor Version: %s\n"), LocateString(str, pProcessor->Version));
    //_tprintf(TEXT("External Clock: %dMHz, 0MHz is unknown clock\n"), pProcessor->ExtClock);
    //_tprintf(TEXT("Max Speed: %dMHz\n"), pProcessor->MaxSpeed);
    //_tprintf(TEXT("Current Speed: %dMHz\n"), pProcessor->CurrentSpeed);
    return true;
}


bool in2BIOSInformation::ProcMemModuleInfo(void* p)
{
    PMemModuleInfo pMemModule = (PMemModuleInfo)p;
    const char *str = toPointString(p);
    //_tprintf(TEXT("%s\n"), getHeaderString(6));
    //_tprintf(TEXT("Length: 0x%X\n"), pMemModule->Header.Length);
    //_tprintf(TEXT("Socket Designation: %s\n"), LocateString(str, pMemModule->SocketDesignation));
    //_tprintf(TEXT("Current Speed: %dns\n"), pMemModule->CurrentSpeed);
    return true;
}


bool in2BIOSInformation::ProcCacheInfo(void *p)
{
    PCacheInfo pCache = (PCacheInfo)p;
    const char *str = toPointString(p);
    //_tprintf(TEXT("%s\n"), getHeaderString(7));
    //_tprintf(TEXT("Length: 0x%X\n"), pCache->Header.Length);
    //_tprintf(TEXT("Socket Designation: %s\n"), LocateString(str, pCache->SocketDesignation));
    return true;
}


bool in2BIOSInformation::ProcOEMString(void* p)
{
    PSMBIOSHEADER pHdr = (PSMBIOSHEADER)p;
    const char *str = toPointString(p);
    //_tprintf(TEXT("%s\n"), getHeaderString(11));
    //_tprintf(TEXT("OEM String: %s\n"), LocateString(str, *(((char*)p) + 4)));
    return true;
}


bool in2BIOSInformation::ProcMemoryDevice(void* p)
{
    PMemoryDevice pMD = (PMemoryDevice)p;
    const char *str = toPointString(p);
    //_tprintf(TEXT("%s\n"), getHeaderString(17));
    //_tprintf(TEXT("Length: 0x%X\n"), pMD->Header.Length);
    //_tprintf(TEXT("Total Width: %dbits\n"), pMD->TotalWidth);
    //_tprintf(TEXT("Data Width: %dbits\n"), pMD->DataWidth);
    //_tprintf(TEXT("Device Locator: %s\n"), LocateString(str, pMD->DeviceLocator));
    //_tprintf(TEXT("Bank Locator: %s\n"), LocateString(str, pMD->BankLocator));
    if (pMD->Header.Length > 0x15)
    {
        //_tprintf(TEXT("Speed: %d\n"), pMD->Speed);
        //_tprintf(TEXT("Manufacturer: %s\n"), LocateString(str,pMD->Manufacturer));
        //_tprintf(TEXT("Serial Number: %s\n"), LocateString(str, pMD->SN));
        //_tprintf(TEXT("Asset Tag Number: %s\n"), LocateString(str, pMD->AssetTag));
        //_tprintf(TEXT("Part Number: %s\n"), LocateString(str, pMD->PN));
    }
    return true;
}


bool in2BIOSInformation::ProcMemoryArrayMappedAddress(void* p)
{
    PMemoryArrayMappedAddress pMAMA = (PMemoryArrayMappedAddress)p;
    const char *str = toPointString(p);
    //_tprintf(TEXT("%s\n"), getHeaderString(19));
    //_tprintf(TEXT("Length: 0x%X\n"), pMAMA->Header.Length);
    //_tprintf(TEXT("Starting Address: 0x%08X\n"), pMAMA->Starting);
    //_tprintf(TEXT("Ending Address: 0x%08X\n"), pMAMA->Ending);
    //_tprintf(TEXT("Memory Array Handle: 0x%X\n"), pMAMA->Handle);
    //_tprintf(TEXT("Partition Width: 0x%X\n"), pMAMA->PartitionWidth);
    return true;
}


bool in2BIOSInformation::ProcPortableBattery(void* p)
{
    PPortableBattery pPB = (PPortableBattery)p;
    const char *str = toPointString(p);
    //_tprintf(TEXT("============= Portable Battery =============\n"));
    //_tprintf(TEXT("Length: 0x%X\n"), pPB->Header.Length);
    //_tprintf(TEXT("Location: %s\n"), LocateString(str, pPB->Location));
    //_tprintf(TEXT("Manufacturer: %s\n"), LocateString(str, pPB->Manufacturer));
    //_tprintf(TEXT("Manufacturer Date: %s\n"), LocateString(str, pPB->Date));
    //_tprintf(TEXT("Serial Number: %s\n"), LocateString(str, pPB->SN));
    return true;
}


bool in2BIOSInformation::DispatchStructType(PSMBIOSHEADER hdr)
{
    typedef struct {
        BYTE t;
        bool(*Proc)(void* p);
    } TPFUNC;

    const TPFUNC tpfunc[] = {
        { 0, in2BIOSInformation::ProcBIOSInfo },
        { 1, in2BIOSInformation::ProcSysInfo },
        { 2, in2BIOSInformation::ProcBoardInfo },
        { 3, in2BIOSInformation::ProcSystemEnclosure },
        { 4, in2BIOSInformation::ProcProcessorInfo },
        { 6, in2BIOSInformation::ProcMemModuleInfo },
        { 7, in2BIOSInformation::ProcCacheInfo },
        { 11, in2BIOSInformation::ProcOEMString },
        { 17, in2BIOSInformation::ProcMemoryDevice },
        { 19, in2BIOSInformation::ProcMemoryArrayMappedAddress },
        { 22, in2BIOSInformation::ProcPortableBattery }
    };

    for (UINT i = 0; i < sizeof(tpfunc) / sizeof(TPFUNC); i++)
    {
        if (tpfunc[i].t == hdr->Type)
        {
            //_tprintf(TEXT("%s\n"), getHeaderString(cstrHEADER));
            tpfunc[i].Proc((void*)hdr);
            return true;
        }
    }

    return false;
}

void in2BIOSInformation::DumpSMBIOSStruct(void *Addr, UINT Len)
{
    LPBYTE p = (LPBYTE)(Addr);
    //const DWORD lastAddress = ((DWORD)p) + Len;
    PSMBIOSHEADER pHeader;
    for (;;) {
        pHeader = (PSMBIOSHEADER)p;
        DispatchStructType(pHeader);
        PBYTE nt = p + pHeader->Length; // point to struct end
        while (0 != (*nt | *(nt + 1))) nt++; // skip string area
        nt += 2;
        //if ((DWORD)nt > lastAddress)
        //break;
        p = nt;
    }
}

