//****************************************************************************
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
// KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
// PURPOSE.
//
// Copyright (C) 2005  XaarJet Ltd.  All Rights Reserved.
//
//****************************************************************************
// -----------------------------------------------------------------------------------------------------------------------
//
// Header files defines the interface between the Scorpion card driver and the application
//
// -----------------------------------------------------------------------------------------------------------------------
#if !defined(DRIVERDEFINES)
#define DRIVERDEFINES

#ifdef __cplusplus
extern "C" {
#endif

// List of all functions that are available from the DLL
#define DllExport __declspec( dllexport )

#include <windows.h>



const DWORD MAXHEADS = 8;//8;				// maximum number of heads per card
const DWORD MAXSCORPIONS = 12;			// maximum number of boards to connect to
const DWORD BLOCKSIZE = 64;
const DWORD MAXROWS = 2;
const DWORD SWATHEBLOCKSPERSWATHE = 7;

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

const UINT MONO = 2;		// number of colours for mono mode
const UINT BIT4COLOUR = 16;		// number of colours for 4 bit mode
const UINT BIT8COLOUR = 256;	// number of colours for 8 bit mode

const BYTE START_MASK = 0x80;	// mask to start reading bitmap from - stored in dib-bitmap msb first
const BYTE START_NIBBLE_MASK = 0xf0;	// mask to start reading bitmap from - stored in dib-bitmap msb first
const BYTE BLACK = 0x01;		// set to this for printing at the print-head
const BYTE WHITE = 0x00;		// set to this for no printing at the print-head

const UINT PRODUCTMIN = 0;		// number that the h/w subtracts to position the stroke engine in image memory
const UINT NUMBERGREYSCALES = 16;	// number of grey scales

const DWORD MAXHEADTYPES = 15;
const UINT MAXJETS[MAXHEADTYPES] = {0, 384, 511, 324, 512, 128, 128, 768, 1024, 0, 0, 0, 1024, 0, 384};
const UINT PrintJets[MAXHEADTYPES] = {0, 382, 511, 318, 500, 126, 128, 764, 1000, 0, 0, 0, 1000, 0, 382};
const DWORD LEADCHANNELS[MAXHEADTYPES] =  {0, 1, 0, 3, 6, 1, 0, 1, 6, 0, 0, 0, 6, 0, 1};
const DWORD TRAILCHANNELS[MAXHEADTYPES] = {0, 1, 0, 3, 6, 1, 0, 1, 6, 0, 0, 0, 6, 0, 1};
const bool BINARYHEAD[MAXHEADTYPES] =  {false, true, false, false, true, true, true, false, false, false, false, false, false, false, true};
const DWORD GREYLEVELS[MAXHEADTYPES] = {0, 2, 6, 6, 2, 2, 2, 6, 8, 0, 0, 0, 8, 0, 2};
const bool HEADTYPEACTIVE[MAXHEADTYPES] = {true, true, true, true, false, false, false, true, true, false, true, false, true, false, true};
const DWORD NUMBEROFROWS[MAXHEADTYPES] = {0, 1, 1, 1, 1, 1, 1, 2, 2, 0, 0, 0, 2, 0, 1};
const DWORD NUMBEROFSIDES[MAXHEADTYPES] = {0, 1, 1, 2, 2, 2, 2, 2, 2, 0, 0, 0, 2, 0, 2};
const DWORD NUMBEROFCONFIGROWS[MAXHEADTYPES] = {0, 1, 1, 2, 2, 2, 2, 2, 2, 0, 0, 0, 2, 0, 2};
const DWORD CHIPCOUNT[MAXHEADTYPES] = {0, 6, 6, 0, 8, 2, 2, 6, 8, 0, 6, 0, 8, 0, 6};
//const DWORD PRELOADROWCOUNT[MAXHEADTYPES] = {0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 1};	// single buffer support
const DWORD PRELOADROWCOUNT[MAXHEADTYPES] = {0, 2, 1, 1, 2, 2, 2, 2, 2, 0, 0, 0, 2, 0, 2};	// double buffer support
//const DWORD PRELOADROWCOUNT[MAXHEADTYPES] = {0, 2, 1, 1, 2, 2, 2, 2, 2, 0, 0, 0, 1, 0, 2};	// 1001 off, double buffer support


enum HeadTypes {NOTSET, OMNIDOT380, TESTPIM, XJ318, XJ500, XJ126, XJ128, OMNIDOT760, XAAR1001MKII, SPARE2, SPARE3, SPARE4, XAAR1001, SPARE5, XAAR382 };	// Print head types
enum ClockType {PIXELMODE, CYCLEMODE};
enum {WEB = 0, UNIPRINT, BIPRINT, UNIPRINTWITHSPIT, UNIBIPRINT};
enum {FORWARD = 0, REVERSE};
enum ErrorValues {ERRORFALSE, ERROR_BITFILENOTFOUND, ERROR_USBNOTFOUND, ERROR_HEADNOTSETUP, ERROR_HEADONLYONEROW,
				ERROR_CONFIGFILENOTFOUND,
				ERROR_IMAGEFILENOTOPENED, ERROR_IMAGEFILEREAD, ERROR_WRONGNUMBERCOLOURS,
				ERROR_IMAGETOBIGFORMEMORY, ERROR_FAILEDTOLOADIMAGETOHEAD, ERROR_BINARYHEADGREYSCALEIMAGE,
				ERROR_GREYSCALEHEADBINARYIMAGE, ERROR_NOT4BITIMAGE, ERROR_NODRIVER, ERROR_NOTENOUGHPRINTLINES,
				ERROR_WRONGCONFIGFILELENGTH, ERROR_FPGAINCORRECTVERSION, ERROR_COMMANDFAILED,
				ERROR_WRONGCHECKSUM, ERROR_FAILEDTOGETSYNC, ERROR_USBWRITE, ERROR_USBREAD,
				ERROR_FAILEDTOENTERCRITICAL, ERRORREADNOTVALID, ERROR_USBNOWAITWRITE,
				ERROR_FIRMWAREVERSIONERROR, ERROR_FAILEDTOPERFORMERRORDIFFUSION,
				ERROR_CANTDOSCREENINGONBINARYIMAGE_ERROR, ERROR_CANTDO2BITONBINARYIMAGE_ERROR,
				ERROR_DATAOUTOFRANGE, ERROR382HEADOLDFIRMWARE, ERRORDRIVERNOTRUNNING, ERRORHEADPOWERUPNOTSUPPORTED,
				ERROR_NOTSUPPORTED
	};
enum PrintMode {SINGLESHOT = 0, FIXEDGAPCONT, FIXEDGAPB2B, LABELWAIT};
enum PackedMode {NOCOMPRESSION = 0, PACKBITS8};
enum SEMode {EXTERNALSE = 0, DDFS, ABSOLUTESE};


// WIDTHBYTES performs DWORD-aligning of DIB scanlines.  The "bits"
// parameter is the bit count for the scanline (biWidth * biBitCount),
// and this macro returns the number of DWORD-aligned bytes needed
// to hold those bits.

#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)

const DWORD SampleClockMin[] = {0, 100, 0, 0, 7, 100, 500, 85, 100, 0, 0, 0, 100, 0, 7};
const DWORD SampleClockMax[] = {0, 990, 0, 0, 511, 2000, 2000, 990, 990, 0, 0, 0, 990, 0, 511};
const float EfficiencyMin[] = {0, 0, 0, 0, 0, 0.5, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const float EfficiencyMax[] = {0, 0, 0, 0, 0, 1.5, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const float V0Min[] = {0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const float V0Max[] = {0, 100, 0, 60, 0, 40, 0, 100, 100, 0, 0, 0, 100, 0, 0};
const float AlphaMin[] = {0, (float)-0.2, 0, -1, 0, 5, 0, (float)-0.2, (float)-0.2, 0, 0, 0, (float)-0.2, 0, 0};
const float AlphaMax[] = {0, 0, 0, 0, 0, 6000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const float ViMin[] = {0, -5, 0, 14, 0, 3, 0, -5, -5, 0, 0, 0, -5, 0, 0};
const float ViMax[] = {0, 50, 0, 28, 0, 400, 0, 50, 50, 0, 0, 0, 50, 0, 0};
const float K44Min[] = {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const float K44Max[] = {0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const DWORD TrimMax[] = {0, 511, 0, 0, 255, 511, 0, 511, 511, 0, 0, 0, 511, 0, 255};

/*
const PCTSTR PrintHeadType[] = {_T("Not Connected"), _T("Xaar 380"), _T("Xaar TestPim"), _T("Xaar 318"), _T("Xaar 500"), _T("Xaar 126"), _T("Xaar 128_Electron"), _T("Xaar 760"), _T("Xaar 1001"), _T("Not Used"), _T("Not Used"), _T("Not Used"), _T("Xaar 1001"), _T("Not Used"), _T("Xaar Proton") };
const PCTSTR ProductCodeInk[] ={ _T("UV"), _T("Solvent"), _T("Water"), _T("Oil"), _T("Generic"), _T("Reserved")};
const PCTSTR ProductCodeDropSize[] ={ _T("3pl"), _T("8pl"), _T("40pl"), _T("80pl"), _T("35pl"), _T("6pl"), _T("Reserved")};
const PCTSTR ProductCodeStagger[] ={ _T("47um"), _T("23.5um"), _T("0"), _T("Reserved")};
const PCTSTR ProductCodeSite[] ={ _T("UK"), _T("AB"), _T("Agfa"), _T("Reserved")};
const PCTSTR ProductCodeClass[] ={ _T("Class 1"), _T("Class 2"), _T("Reserved")};
const PCTSTR MonthString[] = {_T("NotSet"), _T("January"), _T("February"), _T("March"), _T("April"), _T("May"), _T("June"), _T("July"),
			_T("August"), _T("September"), _T("October"), _T("November"), _T("December")};
const PCTSTR XJ500HeadType[] = {_T("Xaar 500-180"), _T("Xaar 500-180UV"), _T("Xaar 500-180CM"), _T("Reserved"), _T("Xaar 500-360"), _T("Xaar 500-360UV"),
			_T("Xaar 500-360 Solvent"), _T("Xaar 500-360MAX"), _T("Xaar 500"), _T("Reserved"), _T("Reserved"), _T("Reserved"), _T("Reserved"), _T("Reserved"), _T("Reserved"), _T("Reserved")};
const PCTSTR XJ500InkType[] = {_T("UV"), _T("Oil"), _T("Solvent"), _T("Oil/Solvent"), _T("Reserved")};
const PCTSTR XJ500DropVolume[] = {_T("40"), _T("48"), _T("75"), _T("80"), _T("Reserved")};
const PCTSTR XJ500Frequency[] = {_T("4kHz"), _T("7.2kHz"), _T("7.3kHz"), _T("8kHz"), _T("Reserved")};
const PCTSTR LogLevel[] = {_T("None"), _T("XUSBDll Debug only"), _T("XUSB XTSDll Debug Only"), _T("All")};
*/

typedef void (CALLBACK *BLOCK_CALLBACK)(DWORD Card, DWORD blockId);

//   The * indicates that these values are modified by the DLL
// The parameters that have a [2] are for both rows of the head
struct PrintDataParameters
{
	// Head Setup parameters
	DWORD Head;								// this head number
	DWORD HeadType;							// Code to indicate type of head connected
	DWORD HeadIndex[MAXROWS];				// used to select the memory area to use
	DWORD NumberOfRows;						// single or 2 row heads
	DWORD SeparateRows;						// Treat each row as an individual head
	DWORD ImageLength[MAXROWS];				// number of strokes in image
	DWORD ImageSize[MAXROWS];				// number of bytes in image
	DWORD ProductOffset;					// Number of strokes of Offset after the PD and before the print starts
	DWORD InterGap;							// Gap used between continuous prints
	DWORD FirstSwatheBlock;					// location in memory where 1st swathe control block is stored
	DWORD SwatheBlock;						// location in memory to store this swathe control block
	DWORD ThisSwathe;						// the number of the active swathe
	DWORD NextSwatheBlock;					// location in memory where the next swathe block will be stored
	DWORD MemoryBlock[MAXROWS];				// location in memory to store this image block
	DWORD FirstMemoryBlock[MAXROWS];		// location in memory where 1st image swathe is stored
	DWORD MemoryBlocksNeeded[MAXROWS];		// number of memory blocks needed to store the image swathe
	DWORD PreLoadSwatheBlock;				// number of memory blocks that the pre-load strokes will require
	DWORD PrintMode;						// print mode ie single shot etc
	bool  PrintOnce;						// set to true if only one complete print is required
	DWORD CycleMode;						// cycle mode ie PIXELMODE, CYCLEMODE
	bool ForwardBuffer;						// direction of print ie forward or reverse
	DWORD StartDir[MAXROWS];				// for each row set the starting head direction bit
	DWORD DirBlock;							// the direction to use for this swathe

	// System setup parameters
	DWORD SubPixelDivide;					// sub pixel divide value
	DWORD SaveSubPixelOffset[2][MAXROWS];	// the subpixeloffsets to use, 1st index is for forward or reverse offsets, 2nd index = row
	DWORD SubPixelOffset;					// the sub pixel offset to use for this swathe
	DWORD EncoderDivide;					// a copy of the encoder divide

	// Image control parameters
	DWORD TrailChannels;					// number of unused channels at end of print head - same value currently used for both rows, max 31
	DWORD LeadChannels;						// number of unused channels at start of print head - same value currently used for both rows, max 31
	DWORD DataChannels;						// total number of printing channels
	DWORD HeadChannels;						// number of printing channels per side
	bool BufferReverse[MAXROWS];			// direction to read the data from the image buffer eg for 760, [0] = true, [1] = false
	DWORD NibbleControl[MAXROWS];			// for each row defines if the even/odd/both nibbles of image data is used for printing
	DWORD NibbleIndex;						// defines if this is row 1 or row 2
	DWORD LoopCount;						// set this to 1
	LPSTR lpDIBBits;						// ptr to the bitmap in (screen) memory
	DWORD TotalImageWidth[MAXROWS];			// total width of the image
	DWORD BitDifference;					// number of bits to store .... this needs to be set to 4

	// Swathe control parameters
	DWORD NumberSwathes[MAXROWS];			// number of swathes to print entire image
	DWORD SwatheMemoryCount[MAXROWS];		// total number of swathes that will fit into memory for this head
	DWORD StoredSwathes[MAXROWS];			// total number of swathes that have been stored to the XUSB box
	DWORD PreviousPrintSwathe[MAXROWS];		// the number of the previous swathe that was stored
	bool AllSwathesFit[MAXROWS];			// Do all the swathes fit in memory at once?
	bool Binary;							// binary or greyscale head?
	DWORD GreyLevel;						// Number of grey levels
	bool FirstSwathe[MAXROWS];				// This should be set to true for each row for the 1st swathe of a print (doesn't need to be set again for repeat print swathes)
	bool LastSwathe[MAXROWS];				// This should be set for last swathe - specifies if the image is only required to be printed once
	bool LastSwatheInMemory[MAXROWS];		// This indicates that this swathe is at the end of the swathe memory
	DWORD SendID[MAXROWS];					// Id of the swathe that has been setup for sending to xusb box
	bool BiPrintKeepOdd;					// this defines if, when in bi-directional printing the number of swathes are rounded up

	// these 2 values are used when a print head is only required to print part of an image
	DWORD SwatheStartIndex;					// the offset into the swathe to start printing from
	DWORD SwatheIncrement;					// the amount to add to locate the next swathe

	UINT SourceStrokeWidth;					// number of blocks required for each image stroke

	// print parameters
	DWORD PrintTransportMode;				// need to know if bi-directional printing is requried
	bool bReverseSwatheOrder;				// this defines if the 1st or last swathe should be printed first
	bool bReverseImageOrder;				// specifies if the 1st or last stroke of the image is printed first
	bool bPaletteRemap;						// is palette remap required
	bool bBinaryBackgroundInvert;			// invert the background for a binary image
	UINT SaveProductOffset[2][MAXROWS];		// 1st index if to forward or reverse offsets
	bool bSelectHead[MAXROWS];				// this head is selected for print

	DWORD GuardValue;						// Set guard channels to this value

	DWORD SEPDSetup;						// bits to control SE and PD configuration
	bool Enable2Bit;						// 2 bit mode enabled?
	BYTE SysClock;							// Encoder mode ie Interal, external or absolute
	BYTE Spare[2];							// spare
};

struct UpdatedPrintDataParameters
{
	struct PrintDataParameters OriginalParameters;
	DWORD RowTrailChannels[MAXROWS];		// number of unused channels at end of print head per row
	DWORD RowLeadChannels[MAXROWS];			// number of unused channels at start of print head per row
	DWORD CopyCount[MAXROWS];				// number of repeated copies
	DWORD Spare[18];
};

struct AbsoluteEncoderValues
{
	DWORD ForwardPDTriggerValue;
	DWORD ReversePDTriggerValue;
	DWORD EncoderCountPreloadValue;			// Value to be loaded on event below
	BYTE  PreloadType;						// 0 for register or 1 for ext PD reset
	BYTE  InvertDirection;					// 1 for invert direction
	BYTE  PDGeneration;						// 0 =both, 1 = forward only, 2 = reverse only
};

struct ErrorDiffusionValues
{
	bool Enabled;
	DWORD MaxDPD;
	float Noise;
	bool Random;
	bool Bustrophedon;
	float Ratio[4];
	bool LevelActive[NUMBERGREYSCALES];
};

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
	DWORD Spare[8];
};

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

struct RS232StatusData
{
	BYTE Temperature;
	int Voltage;
	BYTE Errors[4];
	BYTE VTrim;
	BYTE Reserved;
	BYTE Checksum;
};
#endif //RS232STRUCTURE


// Top level functions
DllExport BOOL	_stdcall	bXaarScorpionDriverLoaded();
DllExport BOOL	_stdcall	bXaarScorpionXSPIDataBufferReset(DWORD Card, DWORD Head, DWORD Row);
DllExport DWORD	_stdcall	XaarScorpionGetErrorValue();
DllExport void	_stdcall	XaarScorpionAllowGetTemperatures(bool Allow);
DllExport bool	_stdcall	XaarScorpionReadAllowGetTemperatures();
DllExport void	_stdcall	XaarScorpionAllowLogging(bool Allow);
DllExport void	_stdcall	XaarScorpionLogPrintRegister(DWORD Card);
DllExport void	_stdcall	XaarScorpionBootScorpionFPGA(DWORD Card);
DllExport void	_stdcall	XaarScorpionLogInformation(char *Info);
DllExport BOOL	_stdcall	bXaarScorpionGetUsbStatus(DWORD Card, DWORD &StatusFlags);
DllExport void	_stdcall	XaarScorpionLogPrintStructInfo(struct PrintDataParameters &DataParams);
DllExport DWORD	_stdcall	XaarScorpionGetXUSBCount();
DllExport void	_stdcall	XaarScorpionReInitialiseSystem();
DllExport BOOL	_stdcall	bXaarScorpionReassignXUSBPorts();
DllExport void	_stdcall	XaarScorpionAllowTimerTick(bool Allow);
DllExport DWORD  _stdcall	XaarScorpionGetMaxXUSB();

// Image loading functions
DllExport BOOL	_stdcall	bXaarScorpionLoadImage(DWORD Card, DWORD Head, DWORD Row, char *ImageFile);
DllExport BOOL	_stdcall	bXaarScorpionEnableIdleSpit(bool Value);
DllExport BOOL	_stdcall	bXaarScorpionXUSBBusy(DWORD Card);
DllExport DWORD	_stdcall	XaarScorpionXUSBBusyCount(DWORD Card);
DllExport void	_stdcall	XaarScorpionXUSBLogToDisc(char *LogString);
DllExport void	_stdcall	XaarScorpionXUSBCallbackForSendComplete(DWORD Card, BLOCK_CALLBACK callback);
DllExport BOOL	_stdcall	bXaarScorpionPerformErrorDiffusion(struct PrintDataParameters &DataParams, struct ErrorDiffusionValues &DiffusionParams, BYTE *TargetData);
DllExport BOOL	_stdcall	bXaarScorpionSetupErrorDiffusion(struct ErrorDiffusionValues &DiffusionParams);
DllExport BOOL	_stdcall	bXaarScorpionPerformAndSaveErrorDiffusion(struct PrintDataParameters &DataParams, char *SaveFile);

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
DllExport BOOL	_stdcall	bXaarScorpionSetPrintDataParameters(DWORD Card, struct PrintDataParameters &DataParams);
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

// Memory and swathe control functions
DllExport BOOL	_stdcall	bXaarScorpionGetMemoryDataCard(DWORD Card);
DllExport BOOL	_stdcall	bXaarScorpionGetMemoryData(DWORD Card , DWORD Head, DWORD NumberOfRows, DWORD &MemoryLocation);
DllExport BOOL	_stdcall	bXaarScorpionGetMemoryDataLocation(DWORD Card, DWORD Head, DWORD NumberOfRows, DWORD Row, DWORD &MemoryLocation);
DllExport BOOL	_stdcall	bXaarScorpionGetCurrentPrintSwathe(DWORD Card, DWORD Head, struct PrintDataParameters DataParams, DWORD &MemoryLocation);
DllExport BOOL	_stdcall	bXaarScorpionSwathesToStore(DWORD Card, DWORD Head, struct PrintDataParameters &DataParams, DWORD &Swathes);
DllExport BOOL	_stdcall	bXaarScorpionGetRegisterSDRAMData(DWORD Card, DWORD Page, bool SDRam, DWORD *MemPtr, DWORD &Amount);
DllExport BOOL	_stdcall	bXaarScorpionSetRegisterSDRAMData(DWORD Card, DWORD Page, DWORD Offset, bool SDRam, DWORD Value);
DllExport BOOL	_stdcall	bXaarScorpionGetMemoryBlocks(DWORD Card, DWORD Page, DWORD *MemPtr, DWORD &Amount);
DllExport BOOL	_stdcall	bXaarScorpionSetupSwatheBlockParameters(struct PrintDataParameters &DataParams);
DllExport BOOL	_stdcall	bXaarScorpionSetupSwatheSpitBlockParameters(struct PrintDataParameters &DataParams, bool SpitSwatheData);
DllExport BOOL	_stdcall	bXaarScorpionReadSwathePrintCount(DWORD Card, DWORD *SwatheCountArray);
DllExport BOOL	_stdcall	bXaarScorpionGetCarriageInformation(DWORD Card, BYTE &ForwardDirection, DWORD &EncoderPosition, DWORD *ErrorValues);
DllExport BOOL	_stdcall	bXaarScorpionWriteSwathePrintCount(DWORD Card, DWORD *SwatheCountArray);

// Setup parameters
DllExport DWORD	_stdcall	XaarScorpionGetMemoryStartBlock(DWORD HeadIndex, DWORD HeadsRequired);
DllExport DWORD	_stdcall	XaarScorpionGetSwatheStartBlock(DWORD Head);
DllExport DWORD	_stdcall	XaarScorpionGetMaxHeadBlocks(DWORD HeadsRequired);
DllExport DWORD	_stdcall	XaarScorpionGetMaxSwatheBlocks();
DllExport BOOL	_stdcall	bXaarScorpionGetPrintDataParameters(struct PrintDataParameters &DataParams);
DllExport BOOL	_stdcall	bXaarScorpionSetupImageDataParameters(DWORD Card, DWORD Head, DWORD Row, struct PrintDataParameters &DataParams, DWORD HeadsRequired);

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

// Functions with the extended PrintDataStucture
DllExport BOOL  _stdcall	bXaarScorpionSetPrintDataParametersUpdated(DWORD Card, struct UpdatedPrintDataParameters &UDataParams);
DllExport BOOL	_stdcall	bXaarScorpionSetupImageDataParametersUpdated(DWORD Card, DWORD Head, DWORD Row, struct UpdatedPrintDataParameters &DataParams, DWORD HeadsRequired);
DllExport BOOL	_stdcall	bXaarScorpionSetupImageDataParametersNoRegistry(DWORD Card, DWORD Head, DWORD Row, struct UpdatedPrintDataParameters &DataParams, DWORD HeadsRequired, struct PrintParametersRegistry RegistryParams);
DllExport void	_stdcall	XaarScorpionLogPrintStructInfoUpdated(struct UpdatedPrintDataParameters &DataParams);
DllExport BOOL	_stdcall	bXaarScorpionSwathesToStoreUpdated(DWORD Card, DWORD Head, struct UpdatedPrintDataParameters &DataParams, DWORD &Swathes);
DllExport BOOL	_stdcall	bXaarScorpionSetupSwatheBlockParametersUpdated(struct UpdatedPrintDataParameters &DataParams);
DllExport BOOL	_stdcall	bXaarScorpionSetPrintDataUpdated(DWORD Card, BYTE *Data, struct UpdatedPrintDataParameters &DataParams);
DllExport BOOL	_stdcall	bXaarScorpionPerformErrorDiffusionUpdated(struct UpdatedPrintDataParameters &DataParams, struct ErrorDiffusionValues &DiffusionParams, BYTE *TargetData);
DllExport BOOL	_stdcall	bXaarScorpionPerformAndSaveErrorDiffusionUpdated(struct UpdatedPrintDataParameters &DataParams, char *SaveFile);
DllExport BOOL	_stdcall	bXaarScorpionGetCurrentPrintSwatheUpdated(DWORD Card, DWORD Head, struct UpdatedPrintDataParameters DataParams, DWORD &MemoryLocation);
DllExport BOOL	_stdcall	bXaarScorpionSetupSwatheSpitBlockParametersUpdated(struct UpdatedPrintDataParameters &DataParams, bool SpitSwatheData);
DllExport BOOL	_stdcall	bXaarScorpionGetPrintDataParametersUpdated(struct UpdatedPrintDataParameters &DataParams);
//DllExport BOOL  _stdcall	bXaarScorpionSetPrintDataParametersStopBlock(DWORD Card, struct UpdatedPrintDataParameters &DataParams);
DllExport BOOL	_stdcall	bXaarScorpionSetupEncoderValuesNoReadRegistry(DWORD RequiredDPI, float Resolution, DWORD QuadRequired, float TransportSpeed, DWORD ClockType);

DllExport BOOL	_stdcall	bXaarScorpionSetPrintSwatheDataMasked(DWORD Card, BYTE *Data, struct PrintDataParameters &UDataParams, struct MaskTable Left, struct MaskTable Right);
DllExport BOOL	_stdcall	bXaarScorpionSetPrintSwatheDataMaskedUpdated(DWORD Card, BYTE *Data, struct UpdatedPrintDataParameters &UDataParams, struct MaskTable Left, struct MaskTable Right);

// Support functions
DllExport BOOL	_stdcall	bXaarScorpionSetSetTestWedgePrint(DWORD Card, DWORD Head, DWORD Row);
DllExport BOOL	_stdcall	bXaarScorpionGetEEpromAll(DWORD Card, DWORD Head, char *Value, bool LogValues, bool ForceNoAccess);

//	Xaar reserved encrypted commands
DllExport BOOL	_stdcall	bXaarScorpionReservedWriteHeadWaveformID(DWORD Card, DWORD Head, DWORD Row, char * WaveID, DWORD Code);
DllExport BOOL	_stdcall	bXaarScorpionReservedWriteHeadSerialNo(DWORD Card, DWORD Head, DWORD &SerialNum, DWORD Code);
DllExport BOOL	_stdcall	bXaarScorpionReservedWriteHeadProductCode(DWORD Card, DWORD Head, char * Values, DWORD Code);
DllExport BOOL	_stdcall	bXaarScorpionReservedWriteXaarDateCode(DWORD Card, DWORD Head, char * Date, DWORD Code);
DllExport BOOL	_stdcall	bXaarScorpionReservedWriteXaarHardwareDescriptor(DWORD Card, DWORD Head, char * Values, DWORD Code);
DllExport BOOL	_stdcall	bXaarScorpionReservedWriteEEprom(DWORD Card, DWORD Head, DWORD Index, char *Values, DWORD Count, DWORD Code, bool ForceNoAccess);
DllExport BOOL	_stdcall	bXaarScorpionReservedWriteWaveformChanged(DWORD Card, DWORD Head, DWORD Row, char *Values, DWORD Code);
DllExport BOOL  _stdcall    bXaarScorpionReservedLoadWaveformAndDownloadFixTC(DWORD Card, DWORD Head, DWORD Row, char * Waveform, DWORD TCValue);
DllExport BOOL	_stdcall	bXaarScorpionReservedReadProtonInternalParameters(DWORD Card, DWORD Head, DWORD Row, char *Values, DWORD Code);
DllExport BOOL	_stdcall	bXaarScorpionReservedWriteProtonInternalParameters(DWORD Card, DWORD Head, DWORD Row, char *Values, DWORD Code);

// Test Functions
DllExport BOOL	_stdcall	bXaarScorpionTestFunction1(DWORD Card, DWORD Head);

#ifdef __cplusplus
 }
 #endif

#endif
