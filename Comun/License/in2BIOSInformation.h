#ifndef IN2BIOSINFORMATION
#define IN2BIOSINFORMATION

#include <windows.h>

//#include "stdafx.h"
#pragma pack(push)
#pragma pack(1)

typedef struct _RawSMBIOSData
{
BYTE Used20CallingMethod;
BYTE SMBIOSMajorVersion;
BYTE SMBIOSMinorVersion;
BYTE DmiRevision;
DWORD Length;
PBYTE SMBIOSTableData;
} RawSMBIOSData, *PRawSMBIOSData;

typedef struct _SMBIOSHEADER_
{
BYTE Type;
BYTE Length;
WORD Handle;
} SMBIOSHEADER, *PSMBIOSHEADER;

typedef struct _TYPE_0_ {
SMBIOSHEADER Header;
UCHAR Vendor;
UCHAR Version;
UINT16 StartingAddrSeg;
UCHAR ReleaseDate;
UCHAR ROMSize;
ULONG64 Characteristics;
UCHAR Extension[2]; // spec. 2.3
UCHAR MajorRelease;
UCHAR MinorRelease;
UCHAR ECFirmwareMajor;
UCHAR ECFirmwareMinor;
} BIOSInfo, *PBIOSInfo;

typedef struct _TYPE_1_ {
SMBIOSHEADER Header;
UCHAR Manufacturer;
UCHAR ProductName;
UCHAR Version;
UCHAR SN;
UCHAR UUID[16];
UCHAR WakeUpType;
UCHAR SKUNumber;
UCHAR Family;
} SystemInfo, *PSystemInfo;

typedef struct _TYPE_2_ {
SMBIOSHEADER Header;
UCHAR Manufacturer;
UCHAR Product;
UCHAR Version;
UCHAR SN;
UCHAR AssetTag;
UCHAR FeatureFlags;
UCHAR LocationInChassis;
UINT16 ChassisHandle;
UCHAR Type;
UCHAR NumObjHandle;
UINT16 *pObjHandle;
} BoardInfo, *PBoardInfo;

typedef struct _TYPE_3_ {
SMBIOSHEADER Header;
UCHAR Manufacturer;
UCHAR Type;
UCHAR Version;
UCHAR SN;
UCHAR AssetTag;
UCHAR BootupState;
UCHAR PowerSupplyState;
UCHAR ThermalState;
UCHAR SecurityStatus;
ULONG32 OEMDefine;
UCHAR Height;
UCHAR NumPowerCord;
UCHAR ElementCount;
UCHAR ElementRecordLength;
UCHAR pElements;
} SystemEnclosure, *PSystemEnclosure;

typedef struct _TYPE_4_ {
SMBIOSHEADER Header;
UCHAR SocketDesignation;
UCHAR Type;
UCHAR Family;
UCHAR Manufacturer;
ULONG64 ID;
UCHAR Version;
UCHAR Voltage;
UINT16 ExtClock;
UINT16 MaxSpeed;
UINT16 CurrentSpeed;
// Todo, Here
} ProcessorInfo, *PProcessorInfo;

typedef struct _TYPE_5_ {
SMBIOSHEADER Header;
// Todo, Here
} MemCtrlInfo, *PMemCtrlInfo;

typedef struct _TYPE_6_ {
SMBIOSHEADER Header;
UCHAR SocketDesignation;
UCHAR BankConnections;
UCHAR CurrentSpeed;
// Todo, Here
} MemModuleInfo, *PMemModuleInfo;

typedef struct _TYPE_7_ {
SMBIOSHEADER Header;
UCHAR SocketDesignation;
UINT16 Configuration;
UINT16 MaxSize;
UINT16 InstalledSize;
UINT16 SupportSRAMType;
UINT16 CurrentSRAMType;
UCHAR Speed;
UCHAR ErrorCorrectionType;
UCHAR SystemCacheType;
UCHAR Associativity;
} CacheInfo, *PCacheInfo;

typedef struct _TYPE_17_ {
SMBIOSHEADER Header;
UINT16 PhysicalArrayHandle;
UINT16 ErrorInformationHandle;
UINT16 TotalWidth;
UINT16 DataWidth;
UINT16 Size;
UCHAR FormFactor;
UCHAR DeviceSet;
UCHAR DeviceLocator;
UCHAR BankLocator;
UCHAR MemoryType;
UINT16 TypeDetail;
UINT16 Speed;
UCHAR Manufacturer;
UCHAR SN;
UCHAR AssetTag;
UCHAR PN;
UCHAR Attributes;
} MemoryDevice, *PMemoryDevice;

typedef struct _TYPE_19_ {
SMBIOSHEADER Header;
ULONG32 Starting;
ULONG32 Ending;
UINT16 Handle;
UCHAR PartitionWidth;
} MemoryArrayMappedAddress, *PMemoryArrayMappedAddress;

typedef struct _TYPE_22_ {
SMBIOSHEADER Header;
UCHAR Location;
UCHAR Manufacturer;
UCHAR Date;
UCHAR SN;
UCHAR DeviceName;
} PortableBattery, *PPortableBattery;

#pragma pack(push)
#ifdef UNICODE
#define getHeaderString getHeaderStringW
#define LocateString	LocateStringW
#else
#define getHeaderString getHeaderStringA
#define LocateString	LocateStringA
#endif

#include <QObject>

class in2BIOSInformation : public QObject
{
    Q_OBJECT

public:
    explicit in2BIOSInformation(QWidget *parent=0);
    ~in2BIOSInformation();
    const UINT cstrHEADER = 0xFFFF;

signals:



public slots:
    const char* getHeaderStringA(const UINT type);
    const wchar_t* getHeaderStringW(const UINT type);
    const char* LocateStringA(const char* str, UINT i);
    const wchar_t* LocateStringW(const char* str, UINT i);
    const char* toPointString(void* p);
    bool ProcBIOSInfo(void* p);
    bool ProcSysInfo(void* p);
    bool ProcBoardInfo(void* p);
    bool ProcSystemEnclosure(void* p);
    bool ProcProcessorInfo(void* p);
    bool ProcMemModuleInfo(void* p);
    bool ProcCacheInfo(void *p);
    bool ProcOEMString(void* p);
    bool ProcMemoryDevice(void* p);
    bool ProcMemoryArrayMappedAddress(void* p);
    bool ProcPortableBattery(void* p);
    bool DispatchStructType(PSMBIOSHEADER hdr);
    void DumpSMBIOSStruct(void *Addr, UINT Len);
};




#endif // IN2BIOSINFORMATION

