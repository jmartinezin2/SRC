////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR 
// FITNESS FOR A PARTICULAR PURPOSE.
//
// Copyright (C) 2005-2014  XaarJet Ltd.  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Definition of the interface between the Scorpion driver and the application
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if !defined(DRIVERDEFINES)
#define DRIVERDEFINES

#ifdef __cplusplus
#include <ioS>

extern "C" {
#endif

// List of all functions that are available from the DLL
#define DllExport __declspec( dllexport )

const DWORD MAXHEADS = 8;				// Maximum number of printheads per card
const DWORD MAXXPMHEADS = 6;			// Maximum number of printheads per XUSB in XPM mode
const DWORD MAXSCORPIONS = 24;			// Maximum number of XUSBs supported in non-XPM system. This value should match MAXUSB in UsbExternalInterface.h in UsbInterface project.
const DWORD BLOCKSIZE = 64;
const DWORD MAXROWS = 2;
const DWORD SWATHEBLOCKSPERSWATHE = 7;
const DWORD MULTIPLESEPD = 4;

#ifndef MAXDEFINED
#define MAXDEFINED 1
const DWORD MAXXUSBXPM = 2;		// Maximum number of virtual XUSBs per XPM
const DWORD MAXGIGA    = 24;	// Maximum number of XPMs
const DWORD MAXTOTALXUSBS = max(MAXSCORPIONS, MAXGIGA * MAXXUSBXPM); // The maximum value of either (maximum number of XUSBs in XUSB system) or (maximum number of virtual XUSBs in XPM system)
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Bit Identifiers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const BYTE BIT0 = 0x001;
const BYTE BIT1 = 0x002;
const BYTE BIT2 = 0x004;
const BYTE BIT3 = 0x008;
const BYTE BIT4 = 0x010;
const BYTE BIT5 = 0x020;
const BYTE BIT6 = 0x040;
const BYTE BIT7 = 0x080;
const DWORD BIT8 = 0x0100;
const DWORD BIT9 = 0x0200;
const DWORD BIT10 = 0x0400;
const DWORD BIT11 = 0x0800;
const DWORD BIT12 = 0x01000;
const DWORD BIT13 = 0x02000;
const DWORD BIT14 = 0x04000;
const DWORD BIT15 = 0x08000;
const DWORD BIT16 = 0x010000;
const DWORD BIT21 = 0x200000;
const DWORD BIT22 = 0x400000;
const DWORD BIT23 = 0x800000;

const UINT UPHCHIPS = 6 * 2;
const UINT XAAR1001CHIPS = 8 * 2;

const UINT MONO = 2;					// number of colours for mono mode
const UINT BIT4COLOUR = 16;				// number of colours for 4 bit mode
const UINT BIT8COLOUR = 256;			// number of colours for 8 bit mode

const BYTE START_MASK = 0x80;			// mask to start reading bitmap from - stored in dib-bitmap msb first
const BYTE START_NIBBLE_MASK = 0xf0;	// mask to start reading bitmap from - stored in dib-bitmap msb first
const BYTE BLACK = 0x01;				// set to this for printing at the print-head
const BYTE WHITE = 0x00;				// set to this for no printing at the print-head

const UINT PRODUCTMIN = 0;				// number that the h/w subtracts to position the stroke engine in image memory
const UINT NUMBERGREYSCALES = 16;		// number of grey scales

const UINT VLDPHExtraWhiteSpace = 330;	// extra white space needed for VLDPH heads

const DWORD MAXHEADTYPES = 15;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// The total number of nozzles on the printhead
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const UINT MAXJETS[MAXHEADTYPES] = 
{
	0, 
	384, 
	0, 
	324, 
	512, 
	128, 
	128, 
	768, 
	512, 
	1024, 
	512, 
	224, 
	1024, 
	1024, 
	384
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// The number of printing nozzles on the printhead
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const UINT PrintJets[MAXHEADTYPES] = 
{
	0, 
	382, 
	0, 
	318, 
	500, 
	126, 
	128, 
	764, 
	500, 
	1000, 
	500, 
	224, 
	1000, 
	1000, 
	382
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// The number of non-printing leading nozzles on the printhead
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const DWORD LEADCHANNELS[MAXHEADTYPES] =  
{
	0, 
	1, 
	0, 
	3, 
	6, 
	1, 
	0, 
	1, 
	6, 
	6, 
	6, 
	0, 
	6, 
	6, 
	1
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// The number of non-printing trailing nozzles on the printhead
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const DWORD TRAILCHANNELS[MAXHEADTYPES] = 
{
	0, 
	1, 
	0, 
	3, 
	6, 
	1, 
	0, 
	1, 
	6, 
	6, 
	6, 
	0, 
	6, 
	6, 
	1
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Indicates whether the printhead is a binary printhead or not
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const bool BINARYHEAD[MAXHEADTYPES] =  
{
	false, 
	true, 
	false, 
	false, 
	true, 
	true, 
	true, 
	false, 
	false, 
	false, 
	false, 
	false, 
	false, 
	false, 
	true
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// The number of grey levels supported by the printhead
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const DWORD GREYLEVELS[MAXHEADTYPES] = 
{
	0, 
	2, 
	0, 
	6, 
	2, 
	2, 
	2, 
	6, 
	8, 
	8, 
	8, 
	8, 
	8, 
	8, 
	2
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Indicates whether or not the printhead type is a valid type
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const bool HEADTYPEACTIVE[MAXHEADTYPES] = 
{
	true, 
	true, 
	true, 
	true, 
	false, 
	false, 
	false, 
	true, 
	true, 
	true, 
	true, 
	true, 
	true, 
	true, 
	true
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// The number of rows on the printhead
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const DWORD NUMBEROFROWS[MAXHEADTYPES] = 
{
	0, 
	1, 
	0, 
	1, 
	1, 
	1, 
	1, 
	2, 
	1, 
	2, 
	1, 
	1, 
	2, 
	2, 
	1
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// The number of physical rows on the printhead
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const DWORD NUMBEROFSIDES[MAXHEADTYPES] = 
{
	0, 
	1, 
	0, 
	2, 
	2, 
	2, 
	2, 
	2, 
	1, 
	2, 
	1, 
	1, 
	2, 
	2, 
	2
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// The number of rows that waveforms are allowed to be downloaded to
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const DWORD NUMBEROFCONFIGROWS[MAXHEADTYPES] = 
{
	0, 
	1, 
	0, 
	2, 
	2, 
	2, 
	2, 
	2, 
	1, 
	2, 
	1, 
	1, 
	2, 
	2, 
	2
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// The number of chips on each printhead
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const DWORD CHIPCOUNT[MAXHEADTYPES] = 
{
	0, 
	6, 
	0, 
	0, 
	8, 
	2, 
	2, 
	6, 
	8, 
	8, 
	8, 
	16, 
	8, 
	8, 
	6
};
//const DWORD PRELOADROWCOUNT[MAXHEADTYPES] = {0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 1};	// single buffer support
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// The number of strokes required to be preloaded to allow the printhead to print straight away
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const DWORD PRELOADROWCOUNT[MAXHEADTYPES] = 
{
	0, 
	2, 
	0, 
	1, 
	2, 
	2, 
	2, 
	2, 
	2, 
	2, 
	2, 
	15, 
	2, 
	2, 
	2
};  // double buffer support
//const DWORD PRELOADROWCOUNT[MAXHEADTYPES] = {0, 2, 1, 1, 2, 2, 2, 2, 2, 0, 0, 0, 1, 0, 2};	// 1001 off, double buffer support
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The IDs of supported printheads
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
enum HeadTypes 
{
	NOTSET, 
	OMNIDOT380, 
	SPARE1, 
	XJ318, 
	XJ500, 
	XJ126, 
	XJ128, 
	OMNIDOT760, 
	DARWIN, 
	XAAR1001MK3, 
	XAAR50XHPC, 
	VLDPH, 
	XAAR1001, 
	XAAR1001MKII, 
	XAAR382 
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The clock type
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
enum ClockType 
{
	PIXELMODE, 
	CYCLEMODE, 
	VLDPHMODE
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The transport mode options
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
enum 
{
	WEB = 0, 
	UNIPRINT, 
	BIPRINT, 
	UNIPRINTWITHSPIT, 
	UNIBIPRINT
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The direction options
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
enum 
{
	FORWARD = 0, 
	REVERSE
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The possible errors
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
enum ErrorValues 
{
	ERRORFALSE,										// No error value to retrieve 
	ERROR_BITFILENOTFOUND,							// The FPGA configuration file could not be found 
	ERROR_USBNOTFOUND,								// The XUSB/XPM device could not be found
	ERROR_HEADNOTSETUP,								// The printhead is not setup
	ERROR_HEADONLYONEROW,							// Attempt to set or read a setting for the second row of a printhead with only one row
	ERROR_CONFIGFILENOTFOUND,						// The configuration file could not be read
	ERROR_IMAGEFILENOTOPENED,						// The image file could not be opened
	ERROR_IMAGEFILEREAD,							// The image file could not be read
	ERROR_WRONGNUMBERCOLOURS,						// The image bit depth does not match that of the printhead
	ERROR_IMAGETOBIGFORMEMORY,						// The image is too big to fit into the XUSB/XPM memory
	ERROR_FAILEDTOLOADIMAGETOHEAD,					// Image could not be downloaded to the printhead
	ERROR_BINARYHEADGREYSCALEIMAGE,					// Attempt to specify a greyscale image for a binary printhead
	ERROR_GREYSCALEHEADBINARYIMAGE,					// Attempt to specigy a binary image for a greyscale printhead
	ERROR_NOT4BITIMAGE,								// Image is not a valid 2 or 16 colour image
	ERROR_NODRIVER,									// The XUSB/SPM driver could not be loaded
	ERROR_NOTENOUGHPRINTLINES,						// The image does not have enough lines for the printhead
	ERROR_WRONGCONFIGFILELENGTH,					// The configuration file is not the length expected
	ERROR_FPGAINCORRECTVERSION,						// This version requires a newer version of the FPGA code
	ERROR_COMMANDFAILED,							// The command failed
	ERROR_WRONGCHECKSUM,							// A call was made to a Xaar reserved function with an incorrect checksum
	ERROR_FAILEDTOGETSYNC,							// Failed to enter GetSyncControl. The command was not performed.
	ERROR_USBWRITE,									// Error writing to the XUSB/XPM
	ERROR_USBREAD,									// Error reading from the XUSB/XPM
	ERROR_FAILEDTOENTERCRITICAL,					// Failed to enter the critical section. The command was not performed.
	ERRORREADNOTVALID,								// The values returned should be treated with caution as the read was not successful.
	ERROR_USBNOWAITWRITE,							// Error writing to the XUSB/XPM using USBWriteNoWait
	ERROR_FIRMWAREVERSIONERROR,						// The firmware version number is not correct 
	ERROR_FAILEDTOPERFORMERRORDIFFUSION,			// Error diffusion has not be performed successfully
	ERROR_CANTDOSCREENINGONBINARYIMAGE_ERROR,		// Error diffusion cant be performed on a binary image 
	ERROR_CANTDO2BITONBINARYIMAGE_ERROR,			// A binary image should not be used in 2 bit mode
	ERROR_DATAOUTOFRANGE,							// The values passed to the DLL for downloading to the printheads are out of range 
	ERROR382HEADOLDFIRMWARE,						// The Xaar Proton printhead has an old version of firmware 
	ERRORDRIVERNOTRUNNING,							// The XUSB/XPM driver is not running 
	ERRORHEADPOWERUPNOTSUPPORTED,					// The XUSB/XPM is not capable of supporting printhead power
	ERROR_REGISTRYACCESS,							// Cannot access registry 
	ERROR_DLL3264BITMISMATCH,						// 32 bit/64 bit DLL mismatch
	ERROR_NOTSUPPORTED,								// The function is not supported
	ERROR_HEADNOTBOOTED,							// The printhead has not booted 
	ERROR_BOOTINPROGRESS,							// The printhead is currently booting
	ERROR_WRONGBOXTYPE,								// This box type does not support this function 
	ERROR_FAILEDTOALLOCATEMEMORY,					// Failed to allocate memory 
	ERROR_LINKBUSY,									// Data command cannot be actioned
	ERROR_NOEEPROMACCESSALLOWED,					// Cannot access EEPROM
	ERROR_FAILEDTOSWITCHPAGE						// Failed to switch page
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The print mode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
enum PrintMode 
{
	SINGLESHOT = 0,		// Single shot - print once per product detect
	FIXEDGAPCONT,		// Fixed gap continuous
	FIXEDGAPB2B,		// Fixed gap back-to-back
	LABELWAIT			// Special mode
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
enum PackedMode 
{
	NOCOMPRESSION = 0, 
	PACKBITS8
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Shaft Encoder Mode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
enum SEMode 
{
	EXTERNALSE = 0, 
	DDFS, 
	ABSOLUTESE
};
	
// WIDTHBYTES performs DWORD-aligning of DIB scanlines.  The "bits" parameter is the bit count for the scanline (biWidth * biBitCount), and this macro returns the number of DWORD-aligned bytes needed to hold those bits.
#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The minimum sample clock for a printhead
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const DWORD SampleClockMin[MAXHEADTYPES] = 
{
	0, 
	100, 
	0, 
	0, 
	7, 
	100, 
	500, 
	85, 
	100, 
	100, 
	100, 
	0, 
	100, 
	100, 
	7
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The maximum sample clock for a printhead
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const DWORD SampleClockMax[MAXHEADTYPES] = 
{
	0, 
	990, 
	0, 
	0, 
	511, 
	2000, 
	2000, 
	990, 
	990, 
	990, 
	990, 
	0, 
	990, 
	990, 
	223
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The minimum efficiency for a printhead
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const float EfficiencyMin[MAXHEADTYPES] = 
{
	0, 
	0, 
	0, 
	0, 
	0, 
	0.5, 
	0, 
	0, 
	0, 
	0, 
	0, 
	0, 
	0, 
	0, 
	0
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The maximum efficiency for a printhead
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const float EfficiencyMax[MAXHEADTYPES] = 
{
	0, 
	0, 
	0, 
	0, 
	0, 
	1.5, 
	0, 
	0, 
	0, 
	0, 
	0, 
	0, 
	0, 
	0, 
	0
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The minimum V0 for a printhead
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const float V0Min[MAXHEADTYPES] = 
{
	0, 
	0, 
	0, 
	10, 
	0, 
	0, 
	0, 
	0, 
	0, 
	0, 
	0, 
	0, 
	0, 
	0, 
	0
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The maximum V0 for a printhead
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const float V0Max[MAXHEADTYPES] = 
{
	0, 
	100, 
	0, 
	60, 
	0, 
	40, 
	0, 
	100, 
	100, 
	100, 
	100, 
	0, 
	100, 
	100, 
	30
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The minimum Alpha for a printhead
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const float AlphaMin[MAXHEADTYPES] = 
{
	0, 
	(float)-0.2, 
	0, 
	-1, 
	(float)-0.2, 
	5, 
	0, 
	(float)-0.2, 
	(float)-0.2, 
	(float)-0.2, 
	(float)-0.2, 
	0, 
	(float)-0.2, 
	(float)-0.2, 
	(float)-0.2
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The maximum Alpha for a printhead
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const float AlphaMax[MAXHEADTYPES] = 
{
	0, 
	0, 
	0, 
	0, 
	0, 
	6000, 
	6000, 
	0, 
	0, 
	0, 
	0, 
	0, 
	0, 
	0, 
	0 
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The minimum Vi for a printhead
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const float ViMin[MAXHEADTYPES] = 
{
	0, 
	-5, 
	0, 
	14, 
	0, 
	3, 
	0, 
	-5, 
	-5, 
	-5, 
	-5, 
	0, 
	-5, 
	-5, 
	-50
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The maximum Vi for a printhead
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const float ViMax[MAXHEADTYPES] = 
{
	0, 
	50, 
	0, 
	28, 
	0, 
	400, 
	0, 
	50, 
	50, 
	50, 
	50, 
	0, 
	50, 
	50, 
	50
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The minimum K44 for a printhead
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const float K44Min[MAXHEADTYPES] = 
{
	0, 
	0, 
	0, 
	0, 
	0, 
	1, 
	0, 
	0, 
	0, 
	0, 
	0, 
	0, 
	0, 
	0, 
	0
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The maximum K44 for a printhead
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const float K44Max[MAXHEADTYPES] = 
{
	0, 
	0, 
	0, 
	0, 
	0, 
	23, 
	0, 
	0, 
	0, 
	0, 
	0, 
	0, 
	0, 
	0, 
	0
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The maximum trim for a printhead
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const DWORD TrimMax[MAXHEADTYPES] = 
{
	0, 
	511, 
	0, 
	0, 
	255, 
	511, 
	0, 
	511, 
	511, 
	511, 
	511, 
	0, 
	511, 
	511, 
	255
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The hardware box type
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
enum BOXTYPE 
{
	XUSBB = 0, 
	XPMB
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The type of printhead
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const PCTSTR PrintHeadType[] = 
{	
	_T("Not Connected"), 
	_T("Xaar 380"), 
	_T("Not Used"), 
	_T("Xaar 318"), 
	_T("Xaar 500"), 
	_T("Xaar 126"), 
	_T("Xaar 128_Electron"), 
	_T("Xaar 760"), 
	_T("Xaar 501"), 
	_T("Xaar 1002"), 
	_T("Xaar 50x HPC"), 
	_T("VLDPH"), 
	_T("Xaar 1001"), 
	_T("Xaar 1001"), 
	_T("Xaar Proton"), 
	_T("Xaar 1002"), 
	_T("Xaar 1003")
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const PCTSTR ExtendedPrintHeadType[] = 
{
	_T("Xaar 1002"),
	_T("Xaar 1003") 
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The printhead type filename
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const PCTSTR PrintHeadTypeFilename[] = 
{
	_T("Not Connected"), 
	_T("Xaar 380"), 
	_T("Not Used"), 
	_T("Xaar 318"), 
	_T("Xaar 500"), 
	_T("Xaar 126"), 
	_T("Xaar 128_Electron"), 
	_T("Xaar 760"), 
	_T("Xaar 501"), 
	_T("Xaar HPC3"), 
	_T("Xaar 50x HPC"), 
	_T("VLDPH"), 
	_T("Xaar 1001"), 
	_T("Xaar 1001"), 
	_T("Xaar Proton"), 
	_T("Xaar 1002"), 
	_T("Xaar 1003")
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The ink product code
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const PCTSTR ProductCodeInk[] =
{ 
	_T("UV"), 
	_T("Solvent"), 
	_T("Water"), 
	_T("Oil"), 
	_T("Generic"), 
	_T("Reserved")
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The drop size product code
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const PCTSTR ProductCodeDropSize[] =
{ 
	_T("3"), 
	_T("8"), 
	_T("40"), 
	_T("80"), 
	_T("35"), 
	_T("6"), 
	_T("12"), 
	_T("Reserved")
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The extended dropsixe product code
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const PCTSTR ExtendedProductCodeDropSize[2][10] = 
{
	{
		_T("3"), 
		_T("8"), 
		_T("40"), 
		_T("80"), 
		_T("35"), 
		_T("6"), 
		_T("12"), 
		_T("8"), 
		_T("9"), 
		_T("10")
	},
	{
		_T("21"), 
		_T("22"), 
		_T("23"), 
		_T("24"), 
		_T("25"), 
		_T("26"), 
		_T("27"), 
		_T("28"), 
		_T("29"), 
		_T("30")
		}
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The stagger product code
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const PCTSTR ProductCodeStagger[] =
{ 
	_T("47um"), 
	_T("23.5um"), 
	_T("0"), 
	_T("Reserved")
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The site product code
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const PCTSTR ProductCodeSite[] =
{ 
	_T("UK"), 
	_T("AB"), 
	_T("Agfa"), 
	_T("Reserved")
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The class product code
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const PCTSTR ProductCodeClass[] =
{ 
	_T("Class 1"), 
	_T("Class 2"), 
	_T("Reserved")
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Months of the year string
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const PCTSTR MonthString[] = 
{
	_T("NotSet"), 
	_T("January"), 
	_T("February"), 
	_T("March"), 
	_T("April"), 
	_T("May"), 
	_T("June"), 
	_T("July"),
	_T("August"), 
	_T("September"), 
	_T("October"), 
	_T("November"), 
	_T("December")
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// XJ500 printhead types
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const PCTSTR XJ500HeadType[] = 
{
	_T("Xaar 500-180"), 
	_T("Xaar 500-180UV"), 
	_T("Xaar 500-180CM"), 
	_T("Reserved"), 
	_T("Xaar 500-360"), 
	_T("Xaar 500-360UV"),
	_T("Xaar 500-360 Solvent"), 
	_T("Xaar 500-360MAX"), 
	_T("Xaar 500"), 
	_T("Reserved"), 
	_T("Reserved"), 
	_T("Reserved"), 
	_T("Reserved"), 
	_T("Reserved"), 
	_T("Reserved"), 
	_T("Reserved")
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// XJ500 ink types
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const PCTSTR XJ500InkType[] = 
{
	_T("UV"), 
	_T("Oil"), 
	_T("Solvent"), 
	_T("Oil/Solvent"), 
	_T("Reserved")
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// XJ500 drop volumes
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const PCTSTR XJ500DropVolume[] = 
{
	_T("40"), 
	_T("48"), 
	_T("75"), 
	_T("80"), 
	_T("Reserved")
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// XJ500 frequencies
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const PCTSTR XJ500Frequency[] = 
{
	_T("4kHz"), 
	_T("7.2kHz"), 
	_T("7.3kHz"), 
	_T("8kHz"), 
	_T("Reserved")
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Logging levels
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const PCTSTR LogLevel[] = 
{
	_T("None"), 
	_T("XUSBDll Debug only"), 
	_T("XUSB XTSDll Debug Only"), 
	_T("All")
};

typedef void (CALLBACK *BLOCK_CALLBACK)(DWORD Card, DWORD blockId);

#pragma pack (4)
//   The * indicates that these values are modified by the DLL. The parameters that have a [MAXROWS] are for each row of the printhead
struct PrintDataParameters
{
	// Head Setup parameters
	DWORD Head;								// This printhead number
	DWORD HeadType;							// Code indicating type of printhead connected
	DWORD HeadIndex[MAXROWS];				// The index of the printhead in the array of actually connected printhead
	DWORD NumberOfRows;						// The number of rows on the printhead
	DWORD SeparateRows;						// If true, treat each row as an individual head
	DWORD ImageLength[MAXROWS];				// The number of strokes in an image
	DWORD ImageSize[MAXROWS];				// The number of bytes in an image
	DWORD ProductOffset;					// Number of strokes of Offset after the product and before the print starts
	DWORD InterGap;							// Gap used between continuous prints
	DWORD FirstSwatheBlock;					// The memory address where 1st swathe control block is stored
	DWORD SwatheBlock;						// The memory address to store this particular swathe control block
	DWORD ThisSwathe;						// The number of the active swathe
	DWORD NextSwatheBlock;					// The memory address where the next swathe block will be stored
	DWORD MemoryBlock[MAXROWS];				// The memory address to store this image block
	DWORD FirstMemoryBlock[MAXROWS];		// The memory address where 1st image swathe is stored
	DWORD MemoryBlocksNeeded[MAXROWS];		// The number of memory blocks needed to store the image swathe
	DWORD PreLoadSwatheBlock;				// The number of memory blocks that the pre-load strokes requires
	DWORD PrintMode;						// The print mode e.g. single shot etc.
	bool  PrintOnce;						// If true only one complete print is required
	DWORD CycleMode;						// Cycle Mode (e.g. set to PIXELMODE, CYCLEMODE)
	bool ForwardBuffer;						// Print direction i.e. forward or reverse
	DWORD StartDir[MAXROWS];				// The starting head direction bit for each row 
	DWORD DirBlock;							// The direction to use for this swathe

	// System setup parameters
	DWORD SubPixelDivide;					// The subpixel divide value
	DWORD SaveSubPixelOffset[2][MAXROWS];	// The subpixeloffsets to use, 1st index is for forward or reverse offsets, 2nd index = row
	DWORD SubPixelOffset;					// The sub pixel offset to use for this swathe
	DWORD EncoderDivide;					// A copy of the encoder divide

	// Image control parameters
	DWORD TrailChannels;					// The number of unused channels at end of print head - same value currently used for both rows, max 31
	DWORD LeadChannels;						// The number of unused channels at start of print head - same value currently used for both rows, max 31
	DWORD DataChannels;						// The total number of printing channels
	DWORD HeadChannels;						// The number of printing channels per side
	bool BufferReverse[MAXROWS];			// The direction to read the data from the image buffer eg for 760, [0] = true, [1] = false
	DWORD NibbleControl[MAXROWS];			// For each row defines if the even/odd/both nibbles of image data is used for printing
	DWORD NibbleIndex;						// Used to defines if we are using row 1 or row 2
	DWORD LoopCount;						// Set this to 1

	LPSTR lpDIBBits;						// Pointer to the bitmap in (screen) memory
	DWORD TotalImageWidth[MAXROWS];			// The total width of the image
	DWORD BitDifference;					// The number of bits to store .... this needs to be set to 4

	// Swathe control parameters
	DWORD NumberSwathes[MAXROWS];			// The number of swathes to print entire image
	DWORD SwatheMemoryCount[MAXROWS];		// The total number of swathes that will fit into memory for this head
	DWORD StoredSwathes[MAXROWS];			// The total number of swathes that have been stored to the XUSB box
	DWORD PreviousPrintSwathe[MAXROWS];		// The number of the previous swathe that was stored
	bool AllSwathesFit[MAXROWS];			// True if all the swathes fit in memory at once
	bool Binary;							// True if binary or false if greyscale head?
	DWORD GreyLevel;						// The number of grey levels
	bool FirstSwathe[MAXROWS];				// This should be set to true for each row for the 1st swathe of a print (doesn't need to be set again for repeat print swathes)
	bool LastSwathe[MAXROWS];				// This should be set for last swathe - specifies if the image is only required to be printed once
	bool LastSwatheInMemory[MAXROWS];		// This indicates that this swathe is at the end of the swathe memory
	DWORD SendID[MAXROWS];					// Id of the swathe that has been setup for sending to xusb box
	bool BiPrintKeepOdd;					// Defines if, when in bi-directional printing the number of swathes are rounded up

	// These 2 values are used when a print head is only required to print part of an image
	DWORD SwatheStartIndex;					// The offset into the swathe to start printing from
	DWORD SwatheIncrement;					// The amount to add to locate the next swathe

	DWORD SourceStrokeWidth;				// The number of blocks required for each image stroke

	// print parameters
	DWORD PrintTransportMode;				// Used to determine if bi-directional printing is required
	bool bReverseSwatheOrder;				// Define if the 1st or last swathe should be printed first
	bool bReverseImageOrder;				// Specify if the 1st or last stroke of the image is printed first
	bool bPaletteRemap;						// True if palette remap required
	bool bBinaryBackgroundInvert;			// Invert the background for a binary image
	DWORD SaveProductOffset[2][MAXROWS];	// 1st index if to forward or reverse offsets
	bool bSelectHead[MAXROWS];				// This printhead is selected for print

	DWORD GuardValue;						// Set guard channels to this value

	DWORD SEPDSetup;						// Bits to control SE and PD configuration. Effectively the ID of the shaftencoder/product detect pair
	bool Enable2Bit;						// True if 2 bit mode is enabled
	BYTE SysClock;							// Encoder mode i.e. Internal, external or absolute
	BYTE VLDPHCount;						// The number of 16 nozzle VLDPH print units
	BYTE Spare;								// Spare
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Additional parameters used in addition to the print data parameters
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct UpdatedPrintDataParameters
{
	struct PrintDataParameters OriginalParameters;
	DWORD RowTrailChannels[MAXROWS];		// The number of unused channels at end of print head per row
	DWORD RowLeadChannels[MAXROWS];			// The number of unused channels at start of print head per row
	DWORD CopyCount[MAXROWS];				// The number of repeated copies
	DWORD XPMSEPDSetup[4];					// XPM - bits to control shaft encoder and product detect configuration 
	DWORD PDFilter;							// XPM - Product Detect Filter, pass value through
	DWORD Spare[13];						// Spare
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Absolute encoder values
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct AbsoluteEncoderValues
{
	DWORD ForwardPDTriggerValue;
	DWORD ReversePDTriggerValue;
	DWORD EncoderCountPreloadValue;			// Value to be loaded on event below
	BYTE  PreloadType;						// 0 for register or 1 for ext PD reset
	BYTE  InvertDirection;					// 1 for invert direction
	BYTE  PDGeneration;						// 0 =both, 1 = forward only, 2 = reverse only
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Error diffusion values
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct ErrorDiffusionValues
{
	bool Enabled;
	DWORD MaxDPD;
	float Noise;
	bool Random;
	bool Bustrophedon;
	float Ratio[4];
	bool LevelActive[NUMBERGREYSCALES];
	bool SingleCycleEnabled;
	bool EncodeEnabled;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// XJ500 information block
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct XJ500Values
{
	DWORD FirmwareVersion;
	BYTE HeadType;
	BYTE HeadDescription;
	BYTE InkType;
	BYTE DropSize;
	BYTE Frequency;
	BYTE MTCount;
	char WaveformID[4][5];
	BYTE WireBondOk;
	BYTE DualBankMode;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Xaar 501 information block
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct Xaar501Info
{
	DWORD Temperatures[4];
	DWORD ASICCurrent;
	DWORD Voltage[5];
	DWORD HeaterSetpoint;
	DWORD PSOCMajor;
	DWORD PSOCMinor;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct PrintParametersRegistry
{
	// Common
	DWORD EncoderDivide;
	DWORD GreyLevels;
	BYTE BinaryRemapValue;
	bool BinaryPaletteRemap;
	bool BinaryPaletteBackground;
	bool Enable2Bit;
	DWORD GuardValue;
	DWORD BiPrintKeepOdd;
	bool ReverseSwatheOrder;
	bool ReverseImageOrder;

	// Config
	DWORD PrintMode;
	DWORD ClockType;
	DWORD PrintOnce;
	DWORD PrintTransportMode;
	DWORD SeparateRows;

	// Head specific
	DWORD ProductOffset[2][MAXROWS];
	DWORD SubPixelDelay[2][MAXROWS];
	bool DirTrue[MAXROWS];
	bool Mirror[MAXROWS];
	bool SelectHead[MAXROWS];

	// other
	DWORD StartExtra;
	DWORD EndExtra;
	bool SwatheSplitWanted;
	DWORD SwatheStartIndex, SwatheIncrement;
	DWORD RowTrailChannels[MAXROWS];				// number of unused channels at end of print head per row
	DWORD RowLeadChannels[MAXROWS];					// number of unused channels at start of print head per row

	BYTE ForceMode;

	DWORD SEPDSetup;
	DWORD EncoderControl;

	BYTE SysClock;

	DWORD CopyCount[MAXROWS];
	BYTE VLDPHCount;
	BYTE bSpare[3];
	DWORD Spare[7];
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Swathe data mask
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct MaskTable
{
	bool Setup;					// is this data setup
	bool UseTable;				// should this mask be used
	BYTE Bits;					// = 1 for binary mask or 4 for GS mask
	DWORD MaskWidth;			// number of channels that should be acted on
	DWORD TableWidth;			// dwords needed to hold the data for each row
	DWORD TableHeight;			// number of rows in the table
	DWORD *MaskData;			// mask of the data stored for each row in table
};

#if !defined(RS232STRUCTURE)
#define RS232STRUCTURE

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RS232 status data
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct RS232StatusData
{
	BYTE Temperature;
	DWORD PWM;
	DWORD Voltage;
	BYTE Errors[4];
	BYTE VTrim;
	BYTE Reserved;
	BYTE Checksum;
};
#endif RS232STRUCTURE

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Encoder values
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct EncoderValuesStruct
{
	DWORD RequiredDPI;
	DWORD QuadRequired;
	DWORD ClockType;
	DWORD EncoderDivide;
	DWORD EncoderPulseMultiply;
	DWORD Prescaler;
	DWORD FractionalRegValue;
	float Resolution;
	float TransportSpeed;
	float EffectiveDivide;
	float FractionalMultiple;		
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Waveform information block
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct WaveformInformation
{
	BYTE GreyLevels;
	BYTE ClockType;
	DWORD SampleClock;
	DWORD MaxPrintFrequency;
	char WaveformID[5];
	double V0;
	double Alpha;
	double Vi;
};

#pragma pack()

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Exported Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Top level functions
DllExport BOOL	_stdcall	bXaarScorpionDriverLoaded();
DllExport BOOL	_stdcall	bXaarScorpionFullSpeedInterface();
DllExport BOOL	_stdcall	bXaarScorpionXSPIDataBufferReset(DWORD Card, DWORD Head, DWORD Row);
DllExport DWORD	_stdcall	XaarScorpionGetErrorValue();
DllExport void	_stdcall	XaarScorpionAllowGetTemperatures(bool Allow);
DllExport bool	_stdcall	XaarScorpionReadAllowGetTemperatures();
DllExport void	_stdcall	XaarScorpionAllowLogging(bool Allow);
DllExport void	_stdcall	XaarScorpionLogPrintRegister(DWORD Card);
DllExport void	_stdcall	XaarScorpionBootScorpionFPGA(DWORD Card);
DllExport void	_stdcall	XaarScorpionLogInformation(char *Info);
DllExport BOOL	_stdcall	bXaarScorpionGetUsbStatus(DWORD Card, DWORD &StatusFlags);
DllExport DWORD	_stdcall	XaarScorpionGetXUSBCount();
void ScorpionReInitialiseSystem(bool SocketRefresh);

DllExport void	_stdcall	XaarScorpionReInitialiseSystem();
DllExport void	_stdcall	XaarScorpionReInitialiseSystemRefresh();
DllExport BOOL	_stdcall	bXaarScorpionReassignXUSBPorts();
DllExport void	_stdcall	XaarScorpionAllowTimerTick(bool Allow);
DllExport DWORD  _stdcall	XaarScorpionGetMaxXUSB();
DllExport BOOL	_stdcall	bXaarScorpionIs64Bit(BOOL &Is64Bit);

// Image loading functions
DllExport BOOL	_stdcall	bXaarScorpionLoadImage(DWORD Card, DWORD Head, DWORD Row, char *ImageFile);
DllExport BOOL	_stdcall	bXaarScorpionEnableIdleSpit(bool Value);
DllExport BOOL	_stdcall	bXaarScorpionXUSBBusy(DWORD Card);
DllExport DWORD	_stdcall	XaarScorpionXUSBBusyCount(DWORD Card);
DllExport void	_stdcall	XaarScorpionXUSBLogToDisc(char *LogString);
DllExport void	_stdcall	XaarScorpionXUSBCallbackForSendComplete(DWORD Card, BLOCK_CALLBACK callback);
DllExport BOOL	_stdcall	bXaarScorpionSetupErrorDiffusion(struct ErrorDiffusionValues &DiffusionParams);

// System setup functions
DllExport BOOL	_stdcall	bXaarScorpionSetEncoderDivide(DWORD Card, DWORD Value);
DllExport BOOL	_stdcall	bXaarScorpionSetDDFSValue(DWORD Card, DWORD Value);
DllExport BOOL	_stdcall	bXaarScorpionSetDDFSEnable(DWORD Card, BYTE Enable);
DllExport BOOL	_stdcall	bXaarScorpionSetDDFSHeadSelect(DWORD Card, DWORD EnableMask);
DllExport BOOL	_stdcall	bXaarScorpionSetupEncoderDirection(DWORD Card, BYTE Value);
DllExport BOOL	_stdcall	bXaarScorpionSetPDInternal(DWORD Card, BYTE Enable);
DllExport BOOL	_stdcall	bXaarScorpionSetPDHeadSelect(DWORD Card, DWORD EnableMask);
DllExport BOOL	_stdcall	bXaarScorpionPDNoReverse(DWORD Card, bool Value);
DllExport BOOL	_stdcall	bXaarScorpionEncoderPulseMultiply(DWORD Card, DWORD Value);
DllExport BOOL	_stdcall	bXaarScorpionSetupEncoderValues();

// Print head functions
DllExport BOOL	_stdcall	bXaarScorpionEnableHeadPrintMode(DWORD Card, DWORD Head, DWORD Row, bool Mode);
DllExport BOOL	_stdcall	bXaarScorpionEnablePrintMode(DWORD Card, DWORD PrintMask, bool Mode);
DllExport BOOL	_stdcall	bXaarScorpionLoadWaveformAndDownload(DWORD Card, DWORD Head, DWORD Row, bool NewWaveform, char *WaveformFile);
DllExport BOOL	_stdcall	bXaarScorpionPrintComplete(DWORD Card);
DllExport BOOL	_stdcall	bXaarScorpionLoadParallelWaveformAndDownload(DWORD Card, bool NewWaveform, char *WaveformFile);
DllExport BOOL  _stdcall	bXaarScorpionGetWaveformTriggerMode(DWORD Card, DWORD Head, DWORD Row, BYTE &CycleTrigger);
DllExport BOOL	_stdcall	bXaarScorpionPrintStopEndSequence(DWORD Card);
DllExport BOOL	_stdcall	bXaarScorpionFastWaveformDownload(DWORD Card, DWORD Head, DWORD Row, char *WaveformFile);
DllExport BOOL _stdcall		bXaarScorpionExtractWaveformInformation(DWORD HeadType, char *WaveformFile, struct WaveformInformation &WaveformInfo);

// Print head parameter functions
DllExport BOOL	_stdcall	bXaarScorpionWriteHeadSampleClock(DWORD Card, DWORD Head, DWORD Row, DWORD Value);
DllExport BOOL	_stdcall	bXaarScorpionGetHeadSampleClock(DWORD Card, DWORD Head, DWORD Row, DWORD &SampleClock);
DllExport BOOL	_stdcall	bXaarScorpionGetHeadWaveformID(DWORD Card, DWORD Head, DWORD Row, char *Value);
DllExport BOOL	_stdcall	bXaarScorpionGetHeadVBase(DWORD Card, DWORD Head, DWORD Row, DWORD &Value);
DllExport BOOL	_stdcall	bXaarScorpionWriteHeadVBase(DWORD Card, DWORD Head, DWORD Row, DWORD Value);
DllExport BOOL	_stdcall	bXaarScorpionGetHeadVOffset(DWORD Card, DWORD Head, DWORD Row, DWORD &Value);
DllExport BOOL	_stdcall	bXaarScorpionWriteHeadVOffset(DWORD Card, DWORD Head, DWORD Row, DWORD Value);
DllExport BOOL	_stdcall	bXaarScorpionGetHeadPTrim(DWORD Card, DWORD Head, DWORD Row, DWORD *Value, DWORD ChipCount);
DllExport BOOL	_stdcall	bXaarScorpionWriteHeadPTrim(DWORD Card, DWORD Head, DWORD Row, DWORD *Value, DWORD ChipCount);
DllExport BOOL	_stdcall	bXaarScorpionGetHeadTCParameters(DWORD Card, DWORD Head, DWORD Row, float &ZeroOffset, float &Alpha, float &VI);
DllExport BOOL	_stdcall	bXaarScorpionWriteHeadTCParameters(DWORD Card, DWORD Head, DWORD Row, float &ZeroOffset, float &Alpha, float &VI);
DllExport BOOL	_stdcall	bXaarScorpionGetHeadEfficiency(DWORD Card, DWORD Head, DWORD Row, DWORD &Factor);
DllExport BOOL	_stdcall	bXaarScorpionWriteHeadEfficiency(DWORD Card, DWORD Head, DWORD Row, DWORD Factor);
DllExport BOOL	_stdcall	bXaarScorpionWriteRemapTable(DWORD Card, DWORD Head, BYTE DPD, BYTE *Values);
DllExport BOOL	_stdcall	bXaarScorpionGetHeadTCParametersK44(DWORD Card, DWORD Head, DWORD Row, float &K44);
DllExport BOOL	_stdcall	bXaarScorpionWriteHeadTCParametersK44(DWORD Card, DWORD Head, DWORD Row, float &K44);
DllExport BOOL	_stdcall	bXaarScorpionGetWaveformChanged(DWORD Card, DWORD Head, DWORD Row, BYTE &Changed);
DllExport BOOL _stdcall		bXaarScorpionWriteAllVOffset(DWORD Card, DWORD *Value);

// Printing/ Print Data functions
DllExport BOOL	_stdcall	bXaarScorpionGenerateProductDetect(DWORD Card);
DllExport BOOL	_stdcall	bXaarScorpionGenerateProductDetectHead(DWORD Card, DWORD Head);
DllExport BOOL	_stdcall	bXaarScorpionGenerateProductDetectRow(DWORD Card, DWORD Head, DWORD Row);
DllExport BOOL	_stdcall	bXaarScorpionSetPrintData(DWORD Card, BYTE *Data, struct PrintDataParameters &DataParams);
DllExport BOOL	_stdcall	bXaarScorpionGetPrintData(DWORD Card, BYTE *Data, DWORD DataLength, DWORD &MemoryBlock);
DllExport BOOL	_stdcall	bXaarScorpionOnTheFlyOffsets(DWORD Card, DWORD *Values, DWORD SubDivide);
DllExport BOOL	_stdcall	bXaarScorpionIgnorePD(DWORD Card, DWORD IgnoreCount);
DllExport BOOL	_stdcall	bXaarScorpionGetPrintFrequency(DWORD Card, DWORD *Values);

// Read head/XUSB box parameter functions
DllExport BOOL	_stdcall	bXaarScorpionFPGAConfigured(DWORD Card);
DllExport BOOL	_stdcall	bXaarScorpionHeadPowerUpSupported(DWORD Card);
DllExport BOOL	_stdcall	bXaarScorpionGetUsbOk(DWORD Card);
DllExport BOOL	_stdcall	bXaarScorpionGetHeadConfig(DWORD Card, DWORD Head);
DllExport BOOL	_stdcall	bXaarScorpionGetStatusData(DWORD Card);
DllExport BOOL	_stdcall	bXaarScorpionGetTemperature(DWORD Card, DWORD Head, DWORD &Temp1, DWORD &Temp2);
DllExport BOOL	_stdcall	bXaarScorpionGetVersion(DWORD Card, BYTE &Major, BYTE &Minor);
DllExport BOOL	_stdcall	bXaarScorpionGetFPGAVersion(DWORD Card, BYTE &Major, BYTE &Minor);
DllExport BOOL	_stdcall	bXaarScorpionGetHeadVersion(DWORD Card, DWORD Head, BYTE &Major, BYTE &Minor);
DllExport BYTE	_stdcall	bXaarScorpionGetHeadType(DWORD Card, DWORD Head);
DllExport BOOL	_stdcall	bXaarScorpionBootHead(DWORD Card, DWORD Head);
DllExport BOOL	_stdcall	bXaarScorpionGetSerialNoMapped(DWORD Card, char *SerialStr);
DllExport BOOL	_stdcall	bXaarScorpionGetHeadSerial(DWORD Card, DWORD Head, char *Value);
DllExport BOOL	_stdcall	bXaarScorpionGetXaarProductCode(DWORD Card, DWORD Head, char *Value);
DllExport BOOL	_stdcall	bXaarScorpionGetXaarDateCode(DWORD Card, DWORD Head, char *Value);
DllExport BOOL	_stdcall	bXaarScorpionGetWaveformLoaded(DWORD Card, DWORD Head, BYTE &Wave1);
DllExport BOOL	_stdcall	bXaarScorpionGetXaarHardwareDescriptor(DWORD Card, DWORD Head, char *Value);
DllExport BOOL	_stdcall	bXaarScorpionGetPCBTemperature(DWORD Card, DWORD *Temp);
DllExport BOOL	_stdcall	bXaarScorpionGetHeadOk(DWORD Card, DWORD Head, BYTE &HeadOk);
DllExport BYTE	_stdcall	bXaarScorpionGetHeadStatus(DWORD Card, DWORD Head);
DllExport BOOL	_stdcall	bXaarScorpionHeadPowerControl(DWORD Card, BYTE HeadMask, bool Enable);
DllExport BOOL	_stdcall	bXaarScorpionSetXUSBID(DWORD Card, BYTE Data);
DllExport BOOL	_stdcall	bXaarScorpionDivertImage(DWORD Card, BYTE Data);

// IP Address functions for use with XPM Only. Please note that since there are two virtual XUSBs in an XPM these functions should in general be used with odd numbered XUSBs
DllExport BOOL  _stdcall    bXaarScorpionSetPreferredIPAddresses(DWORD Card, char *preferredServerIPAddress, char *preferredXpmIPAddress);  // Set the IP addresses preferred by the box
DllExport BOOL  _stdcall    bXaarScorpionGetPreferredIPAddresses(DWORD Card, char *preferredServerIPAddress, char *preferredXpmIPAddress);	// Get the IP addresses preferred by the box
DllExport BOOL  _stdcall    bXaarScorpionGetActualIPAddresses(DWORD Card, char *actualServerIPAddress, char *actualXpmIPAddress);			// Get the IP addresses actually being used currently by the box

// Memory and swathe control functions
DllExport BOOL	_stdcall	bXaarScorpionGetMemoryDataCard(DWORD Card);
DllExport BOOL	_stdcall	bXaarScorpionGetMemoryData(DWORD Card , DWORD Head, DWORD NumberOfRows, DWORD &MemoryLocation);
DllExport BOOL	_stdcall	bXaarScorpionGetMemoryDataLocation(DWORD Card, DWORD Head, DWORD NumberOfRows, DWORD Row, DWORD &MemoryLocation);
DllExport BOOL	_stdcall	bXaarScorpionGetRegisterSDRAMData(DWORD Card, DWORD Page, bool SDRam, DWORD *MemPtr, DWORD &Amount);
DllExport BOOL	_stdcall	bXaarScorpionSetRegisterSDRAMData(DWORD Card, DWORD Page, DWORD Offset, bool SDRam, DWORD Value);
DllExport BOOL	_stdcall	bXaarScorpionGetMemoryBlocks(DWORD Card, DWORD Page, DWORD *MemPtr, DWORD &Amount);
DllExport BOOL	_stdcall	bXaarScorpionReadSwathePrintCount(DWORD Card, DWORD *SwatheCountArray);
DllExport BOOL	_stdcall	bXaarScorpionGetCarriageInformation(DWORD Card, BYTE &ForwardDirection, DWORD &EncoderPosition, DWORD *ErrorValues);
DllExport BOOL	_stdcall	bXaarScorpionWriteSwathePrintCount(DWORD Card, DWORD *SwatheCountArray);

// Setup parameters
DllExport DWORD	_stdcall	XaarScorpionGetMemoryStartBlock(DWORD HeadIndex, DWORD HeadsRequired);
DllExport DWORD _stdcall	XaarScorpionGetMemoryStartBlockXPM(DWORD Card, DWORD Head, DWORD HeadsRequired);
DllExport DWORD	_stdcall	XaarScorpionGetSwatheStartBlock(DWORD Head);
DllExport DWORD	_stdcall	XaarScorpionGetMaxHeadBlocks(DWORD HeadsRequired);
DllExport DWORD	_stdcall	XaarScorpionGetMaxHeadBlocksXPM(DWORD Card, DWORD HeadsRequired);
DllExport DWORD	_stdcall	XaarScorpionGetMaxSwatheBlocks();

DllExport BOOL	_stdcall	bXaarScorpionSendRS232Message(DWORD Card, DWORD Head, DWORD Row, char * SendCommand, DWORD SendLength);
DllExport BOOL	_stdcall	bXaarScorpionReadRS232Message(DWORD Card, DWORD Head, DWORD Row, char * ReadBuffer, DWORD RequiredLength);

// Absolute encoder functions
DllExport BOOL _stdcall		bXaarScorpionAbsoluteEncoderEnable(DWORD Card, BOOL Enable);
DllExport BOOL _stdcall		bXaarScorpionAbsoluteEncoderSetup(DWORD Card, struct AbsoluteEncoderValues &AbsoluteValues);
DllExport BOOL _stdcall		bXaarScorpionAbsoluteEncoderLoadNow(DWORD Card);

// XJ500 Functions
DllExport BOOL _stdcall		bXaarScorpionGet500Information(DWORD Card, DWORD Head, DWORD Row, struct XJ500Values &Values);
DllExport BOOL _stdcall		bXaarScorpionGet500EEPROM(DWORD Card, DWORD Head, DWORD Row, char * ReadBuffer);
DllExport BOOL _stdcall		bXaarScorpionGet500WaveformInformation(DWORD Card, DWORD Head, DWORD Row, char *Mode, DWORD &PWM);
DllExport BOOL _stdcall		bXaarScorpionSelectActiveWaveform(DWORD Card, DWORD Head, DWORD Row, DWORD WaveformNumber);
DllExport BOOL _stdcall		bXaarScorpionGetXJ500StatusInformation(DWORD Card, DWORD Head, DWORD Row, struct RS232StatusData &Data);
DllExport BOOL _stdcall		bXaarScorpionGet382TimeStampedData(DWORD Card, DWORD Head, DWORD Row, char *Values);

// Functions with the extended PrintDataStucture
DllExport BOOL  _stdcall	bXaarScorpionSetPrintDataParametersUpdated(DWORD Card, struct UpdatedPrintDataParameters &UDataParams);
DllExport BOOL	_stdcall	bXaarScorpionSetupImageDataParametersUpdated(DWORD Card, DWORD Head, DWORD Row, struct UpdatedPrintDataParameters &DataParams, DWORD HeadsRequired);
DllExport BOOL	_stdcall	bXaarScorpionSetupImageDataParametersNoRegistry(DWORD Card, DWORD Head, DWORD Row, struct UpdatedPrintDataParameters &DataParams, DWORD HeadsRequired, struct PrintParametersRegistry &RegistryParams);
DllExport void	_stdcall	XaarScorpionLogPrintStructInfoUpdated(struct UpdatedPrintDataParameters &DataParams);
DllExport BOOL	_stdcall	bXaarScorpionSwathesToStoreUpdated(DWORD Card, DWORD Head, struct UpdatedPrintDataParameters &DataParams, DWORD &Swathes);
DllExport BOOL	_stdcall	bXaarScorpionSetupSwatheBlockParametersUpdated(struct UpdatedPrintDataParameters &DataParams);
DllExport BOOL	_stdcall	bXaarScorpionSetPrintDataUpdated(DWORD Card, BYTE *Data, struct UpdatedPrintDataParameters &DataParams);
DllExport BOOL	_stdcall	bXaarScorpionPerformErrorDiffusionUpdated(struct UpdatedPrintDataParameters &DataParams, struct ErrorDiffusionValues &DiffusionParams, BYTE *TargetData);
DllExport BOOL	_stdcall	bXaarScorpionPerformAndSaveErrorDiffusionUpdated(struct UpdatedPrintDataParameters &DataParams, char *SaveFile);
DllExport BOOL	_stdcall	bXaarScorpionGetCurrentPrintSwatheUpdated(DWORD Card, DWORD Head, struct UpdatedPrintDataParameters DataParams, DWORD &MemoryLocation);
DllExport BOOL	_stdcall	bXaarScorpionSetupSwatheSpitBlockParametersUpdated(struct UpdatedPrintDataParameters &DataParams, bool SpitSwatheData);
DllExport BOOL	_stdcall	bXaarScorpionGetPrintDataParametersUpdated(struct UpdatedPrintDataParameters &DataParams);
DllExport BOOL	_stdcall	bXaarScorpionSetupEncoderValuesNoReadRegistryUpdated(struct EncoderValuesStruct &Values);

DllExport BOOL	_stdcall	bXaarScorpionSetPrintSwatheDataMasked(DWORD Card, BYTE *Data, struct PrintDataParameters &UDataParams, struct MaskTable Left, struct MaskTable Right);
DllExport BOOL	_stdcall	bXaarScorpionSetPrintSwatheDataMaskedUpdated(DWORD Card, BYTE *Data, struct UpdatedPrintDataParameters &UDataParams, struct MaskTable Left, struct MaskTable Right);

// Proton functions
DllExport BOOL	_stdcall	bXaarScorpionReadProtonExtendedStatus(DWORD Card, DWORD Head, DWORD Row, char *Values);

DllExport BOOL	_stdcall	bXaarScorpionReadProtonExtendedStatusExtra(DWORD Card, DWORD Head, DWORD Row, char *Values, const int);
DllExport BOOL	_stdcall	bXaarScorpionReadProtonExtendedExtra(DWORD Card, DWORD Head, DWORD Row, int command, char *Values, const int);

// Xaar501 Functions
DllExport BOOL _stdcall		bXaarScorpionSet501TCControlMode(DWORD Card, DWORD Head, DWORD Values);
DllExport BOOL _stdcall		bXaarScorpionGet501Information(DWORD Card, DWORD Head, struct Xaar501Info &Xaar501Data);
DllExport BOOL _stdcall		bXaarScorpionGet501Version(DWORD Card, DWORD Head, BYTE &Major, BYTE &Minor);

// Support functions
DllExport BOOL	_stdcall	bXaarScorpionSetSetTestWedgePrint(DWORD Card, DWORD Head, DWORD Row);
DllExport BOOL	_stdcall	bXaarScorpionGetEEpromAll(DWORD Card, DWORD Head, char *Value, bool LogValues, bool ForceNoAccess);
DllExport BOOL	_stdcall	bXaarScorpionSetDirection(DWORD Card, DWORD Head, DWORD Row, BYTE Value);

//	Xaar reserved encrypted commands
DllExport BOOL	_stdcall	bXaarScorpionReservedWriteHeadWaveformID(DWORD Card, DWORD Head, DWORD Row, char * WaveID, DWORD Code);
DllExport BOOL	_stdcall	bXaarScorpionReservedWriteHeadSerialNo(DWORD Card, DWORD Head, DWORD &SerialNum, DWORD Code);
DllExport BOOL	_stdcall	bXaarScorpionReservedWriteHeadProductCode(DWORD Card, DWORD Head, char * Values, DWORD Code);
DllExport BOOL	_stdcall	bXaarScorpionReservedWriteXaarDateCode(DWORD Card, DWORD Head, char * Date, DWORD Code);
DllExport BOOL	_stdcall	bXaarScorpionReservedWriteXaarHardwareDescriptor(DWORD Card, DWORD Head, char * Values, DWORD Code);
DllExport BOOL	_stdcall	bXaarScorpionReservedWriteEEprom(DWORD Card, DWORD Head, DWORD Index, char *Values, DWORD Count, DWORD Code, bool ForceNoAccess);
DllExport BOOL	_stdcall	bXaarScorpionReservedWriteWaveformChanged(DWORD Card, DWORD Head, DWORD Row, char *Values, DWORD Code);
DllExport BOOL  _stdcall    bXaarScorpionReservedLoadWaveformAndDownloadFixTC(DWORD Card, DWORD Head, DWORD Row, char * Waveform, DWORD TCValue);
DllExport BOOL	_stdcall	bXaarScorpionReservedSetProtonHeaterModesEnable(DWORD Card, DWORD Head, DWORD Row, char *Values, DWORD Code);
DllExport BOOL	_stdcall	bXaarScorpionReservedReadProtonInternalParameters(DWORD Card, DWORD Head, DWORD Row, char *Values, DWORD Code);

// Any new functions needed for XPM
DllExport BYTE	_stdcall	bXaarScorpionGetBoxType(DWORD Card);
DllExport BOOL	_stdcall	bXaarScorpionEncoderPulseMultiplySEPD(DWORD Card, DWORD SEPD, DWORD Value);
DllExport BOOL	_stdcall	bXaarScorpionAbsoluteEncoderEnableSEPD(DWORD Card, DWORD SEPD, BOOL Enable);
DllExport BOOL	_stdcall	bXaarScorpionAbsoluteEncoderSetupSEPD(DWORD Card, DWORD SEPD, struct AbsoluteEncoderValues &AbsoluteValues);
DllExport BOOL	_stdcall	bXaarScorpionAbsoluteEncoderLoadNowSEPD(DWORD Card, DWORD SEPD);
DllExport BOOL	_stdcall	bXaarScorpionSetupEncoderDirectionSEPD(DWORD Card, DWORD Head, BYTE Value);
DllExport BOOL	_stdcall	bXaarScorpionSetPDInternalSEPD(DWORD Card, DWORD Mask);
DllExport BOOL	_stdcall	bXaarScorpionIgnorePDSEPD(DWORD Card, DWORD SEPD, DWORD IgnoreCount);
DllExport BOOL	_stdcall	bXaarScorpionSelectSEPD(DWORD Card, BYTE *SEPD);
//DllExport BOOL	_stdcall	bXaarScorpionSetupPDEncoderSelectSEPD(DWORD Card, BYTE *SEPD);
DllExport BOOL	_stdcall	bXaarScorpionGetCarriageInformationSEPD(DWORD Card, DWORD SEPD, BYTE &ForwardDirection, DWORD &EncoderPosition, DWORD *ErrorValues);
DllExport BOOL	_stdcall	bXaarScorpionSetEncoderDivideHead(DWORD Card, DWORD Head, DWORD Value);
DllExport BOOL	_stdcall	bXaarScorpionSetDDFSValueSEPD(DWORD Card, DWORD SEPD, DWORD Value);
DllExport BOOL	_stdcall	bXaarScorpionPDNoReverseSEPD(DWORD Card, DWORD SEPD, bool Value);
DllExport BOOL	_stdcall	bXaarScorpionReloadAppXMLFileParameters();
DllExport BOOL	_stdcall	bXaarScorpionSaveAppXMLFileParameters();
DllExport BYTE	_stdcall	bXaarScorpionGetBoxHeadCount(DWORD Card);
DllExport BOOL	_stdcall	bXaarScorpionEncoderPulseMultiplySEPDExtended(DWORD Card, DWORD SEPD, DWORD Value, DWORD FractionalRegvalue, DWORD Prescalar);

DllExport BOOL	_stdcall	bXaarScorpionGetXPMTotalBoxMemorySize(DWORD Card, LONGLONG * Size);
DllExport BOOL	_stdcall	bXaarScorpionSetXPMBoxMemoryAccessSize(DWORD Card, LONGLONG Size);

DllExport BOOL	_stdcall	bXaarScorpionGetExtendedHeadType(DWORD Card, DWORD Head, BYTE &ExtendedHeadType, BYTE &DropSize, char * HeadTypeInformation);
DllExport BOOL _stdcall		bXaarScorpionXPMsConnected(DWORD &PortsConnected, DWORD &PortsWanted);

// Test Functions
DllExport BOOL	_stdcall	bXaarScorpionTestFunction1(DWORD Card, DWORD Head);

DllExport BOOL	_stdcall    bXaarScorpionSetAppXmlStringCached(LPCTSTR keyName, PCTSTR stringToSet);
DllExport BOOL  _stdcall    bXaarScorpionSetAppXmlIntegerCached(LPCTSTR keyName, DWORD dwordToSet);
DllExport BOOL  _stdcall    bXaarScorpionSetAppXmlSectionCached(LPCTSTR section);

#ifdef __cplusplus
 }
 #endif

#endif