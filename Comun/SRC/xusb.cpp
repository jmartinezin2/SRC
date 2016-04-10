/** @addtogroup XUSBData
 *  @{*/
/** @file XUSBData.cpp
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright Cemitec. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Pantalla de parametros de impresion
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: XUSBData.cpp
 *    @li Versión: x.0
 *    @li Fecha: 05/05/08
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */

#include "xusb.h"
#include "globals.h"
#include "inputdata.h"
#include <QMessageBox>
#include <QFileDialog>
#include "principal.h"
#include "dbgmem.h"
#include <QDebug>

//Fichero de cabecera para Xaar
#include "ScorpionDLL-ext.h"
#include "Scorpionapi.h"
//#include "XMLWrapper.h"
//#include "TinyXML/tinyxml.h"

//Puntero a datos
unsigned char *Data;


XUSBData::XUSBData(QWidget *parent)
	: QWidget(parent)
{
	
	
}


XUSBData::~XUSBData()
{

}

void XUSBData::ConectarConXUSB(){

    bool dummyError;
    DWORD ErrorLoading;

	//Inicializamos XUSB
    dummyError=bXaarScorpionDriverLoaded();
    if (dummyError==0){
        ErrorLoading=XaarScorpionGetErrorValue();
        QMessageBox::warning(0,"Atención!:",QString("Error de conexión con XUSB.\nCódigo de error:%1").arg(ErrorLoading),QMessageBox::Ok);
    }

}


// ----------------------------------------------------------------------
//
//  When SendImage is selected.
//
//  This is called to simply load the image into the scorpion box
//	This now cycles round head, card, row - so that if multiple xusbs
//  are found then the data will be sent to head 1 usb1, head 1 usb2 ...
//  this used to be xusb 1 head1, xusb 1 head2...
//  this modified way should allow the swathe data to finish for 1 box before
//  sending the data for the next head
//
// ----------------------------------------------------------------------
bool XUSBData::SendImage()
{
    bool bLoadOk = true;

	m_NumberSwathes = 0;
    m_AllSwathesFit = true;     // all images will fit entirely in memory
    m_SwatheFile = false;

	//No sé que hace esto: BeginWaitCursor();
    //JORGE for (DWORD Card = 0; Card < m_pApp->GetXUSBsFound(); Card++)
    for (DWORD Card = 0; Card < 1; Card++)
	{
        //UpdatedPrintDataParameters sPrintParams(Card, m_Key);
        CPrintParameters sPrintParams;
		m_bSelectCard[Card] = sPrintParams.m_bSelectedCard;
	}

    //JORGE m_Key->Setup(APPWRITE,_T("Configuration"));
    //JORGE m_Key->QueryValueWithDefault(APPWRITE, _T("MaskEnable"), m_MaskEnable, false);

	// call function to grab the image from the screen an send it to the drive board
	for (int Head = 0; Head < MAXHEADS && bLoadOk; Head++)
	{
        //JORGE for (DWORD Card = 0; Card < m_pApp->GetXUSBsFound() && bLoadOk; Card++)
        for (DWORD Card = 0; Card < 1 && bLoadOk; Card++)
		{
			if (m_bSelectCard[Card])
			{
				for (DWORD Row = 0; Row < MAXROWS && bLoadOk; Row++)
				{
					bLoadOk = LoadActiveBitmapToCard(Card, Head, Row);
					if (bLoadOk)
                    {
						if (m_NumberSwathes == 0)				// only want to add swathe count once
							m_NumberSwathes += m_DataParams[Card][Head].OriginalParameters.NumberSwathes[Row];
                        if (m_DataParams[Card][Head].OriginalParameters.AllSwathesFit[Row] == false)
                            m_AllSwathesFit = false;
					}
				}
			}
		}
	}
	
	//Nomuestro los errores: if (!m_AllSwathesFit && !m_SwatheFile) DisplayErrorMessage(IDS_IMAGETOOBIG, MB_OK);
	
	//No sé que hace esto: EndWaitCursor();
	
	return bLoadOk;
}

// ----------------------------------------------------------------------
//
//	LoadActiveBitmapToCard
//  Determines which is the active window and then loads the bitmap
//  Checks that the bitmap loaded is correct for the head type connected
//
// ----------------------------------------------------------------------
bool XUSBData::LoadActiveBitmapToCard(DWORD Card, DWORD Head, DWORD Row)
{
    bool bReturnOk = false;
	HDIB  hDIB = NULL;
    //CPrintParameters sPrintParams(Card, m_Key);
    CPrintParameters sPrintParams;
	if (Row == 0)
	{
		m_DataParams[Card][Head].OriginalParameters.bSelectHead[0] = sPrintParams.m_bSelectHead[Head][0];
		m_DataParams[Card][Head].OriginalParameters.bSelectHead[1] = sPrintParams.m_bSelectHead[Head][1];
        m_DataParams[Card][Head].OriginalParameters.AllSwathesFit[0] = true; // this image will fit entirely in memory
        m_DataParams[Card][Head].OriginalParameters.AllSwathesFit[1] = true; // this image will fit entirely in memory
	}

	m_DataParams[Card][Head].OriginalParameters.HeadType = bXaarScorpionGetHeadType(Card, Head);
	if (m_DataParams[Card][Head].OriginalParameters.HeadType == NOTSET)
	{
        m_DataParams[Card][Head].OriginalParameters.bSelectHead[0] = false;	// if head is not connected, don't print to it
        m_DataParams[Card][Head].OriginalParameters.bSelectHead[1] = false;	// if head is not connected, don't print to it
	}

	if (Row == 0 || (Row == 1 && sPrintParams.m_SeparateRows))
		m_DataParams[Card][Head].OriginalParameters.NumberSwathes[Row] = 0;

	if (!((sPrintParams.m_SeparateRows && m_DataParams[Card][Head].OriginalParameters.bSelectHead[Row]) ||
		(Row == 0 && !sPrintParams.m_SeparateRows && (m_DataParams[Card][Head].OriginalParameters.bSelectHead[0] || m_DataParams[Card][Head].OriginalParameters.bSelectHead[1]))))
		return true;

	// find out which is the active bitmap window
    // Esto no lo aplicamos:sabemos qué imagen es la activa
    /*
    CScorpionDoc* pDoc = GetDocument();
	if (pDoc != NULL && pDoc->bMultiDocument())
	{
		CString strPath = pDoc->GetPathName();	// find the name of the document
		if (CheckDocumentLoaded(strPath, Card, Head, Row))
		{
			pDoc = GetDocument(strPath, Card, Head, Row);		// get the document relating to this head
			hDIB = pDoc->GetHDIB();
		}
		else
		{
			//No muestro los errores: DisplayErrorMessage(IDS_MULTIPLEFILEUNLOADED_ERROR,  MB_OK | MB_APPLMODAL | MB_ICONSTOP);
            return false;
		}
	}
	else
		hDIB = pDoc->GetHDIB();
    */

	if (m_DataParams[Card][Head].OriginalParameters.HeadType == NOTSET)
		m_DataParams[Card][Head].OriginalParameters.HeadType = sPrintParams.m_PrintType;		// default to head type entered
	m_DataParams[Card][Head].OriginalParameters.Binary = BINARYHEAD[m_DataParams[Card][Head].OriginalParameters.HeadType];
	m_DataParams[Card][Head].OriginalParameters.NumberOfRows = NUMBEROFROWS[m_DataParams[Card][Head].OriginalParameters.HeadType];

    //if (hDIB != NULL)
    //{
		bReturnOk = TRUE;

        //No sé que hace: LPSTR lpDIB = (LPSTR) ::GlobalLock((HGLOBAL) hDIB);
		LPSTR lpDIBBits = ::FindDIBBits(lpDIB);	// Pointer to DIB bits

		int cyDIB = (int) ::DIBWidth(lpDIB);				// Size of DIB - y, width of image (ie channels)
		m_DataParams[Card][Head].OriginalParameters.ImageLength[Row] = (int) ::DIBHeight(lpDIB);			// Size of DIB - x, height of image (ie number of strokes)
		WORD wNumColours = (int)::DIBNumColors(lpDIB);

		DWORD MaxJets = MAXJETS[m_DataParams[Card][Head].OriginalParameters.HeadType] / m_DataParams[Card][Head].OriginalParameters.NumberOfRows;
        bool Binary = m_DataParams[Card][Head].OriginalParameters.Binary;
        //La imagen siempre va a ser greyscale
        switch (sPrintParams.m_ForceMode)
		{
			case 1:	Binary = true;
				break;
            case 2:	Binary = false;
				break;
		}
		DWORD BitDifference = 1;
		if (!Binary)
		{
			if (m_DataParams[Card][Head].OriginalParameters.Enable2Bit)
				BitDifference = 2;				// number of bits to store
			else
				BitDifference = 4;				// number of bits to store
		}

		DWORD SourceStrokeWidth = (BitDifference * MaxJets / 8 + BLOCKSIZE - 1)/ BLOCKSIZE;// rounds to 64 byte boundary

		if (m_DataParams[Card][Head].OriginalParameters.ImageLength[Row] * SourceStrokeWidth > XaarScorpionGetMaxHeadBlocks(m_HeadsRequired[Card]))
		{
            //No muestro los errores: DisplayErrorMessage(IDS_IMAGE_TO_BIG_FOR_MEMORY, MB_ICONINFORMATION | MB_OK);
            bReturnOk = false;
		}

		if (cyDIB > MAXJETS[m_DataParams[Card][Head].OriginalParameters.HeadType] && // contains at least 2 swathes
			2 * m_DataParams[Card][Head].OriginalParameters.ImageLength[Row] * SourceStrokeWidth > XaarScorpionGetMaxHeadBlocks(m_HeadsRequired[Card]))
		{
			//No muestro los errores: DisplayErrorMessage(IDS_IMAGE_TO_BIG_TO_FIT_2_SWATHES_IN_MEMORY, MB_ICONINFORMATION | MB_OK);
            bReturnOk = false;
		}

        //JORGE: Lo quito
        //if (wNumColours == MONO && m_DiffusionParams.Enabled)
        //{
			//No muestro los errores: DisplayErrorMessage(IDS_CANTDOSCREENINGONBINARYIMAGE_ERROR, MB_OK | MB_APPLMODAL | MB_ICONSTOP);
            //bReturnOk = false;
        //}

		if (wNumColours == MONO && sPrintParams.m_bEnable2Bit && !pDoc->bSwatheFile())
		{
			//No muestro los errores: DisplayErrorMessage(IDS_CANTDO2BITONBINARYIMAGE_ERROR, MB_OK | MB_APPLMODAL | MB_ICONSTOP);
            bReturnOk = false;
		}

        if (bReturnOk)
		{
			if ((m_DataParams[Card][Head].OriginalParameters.Binary && sPrintParams.m_ForceMode != 2) || (sPrintParams.m_ForceMode == 1))
			{
                if (pDoc->bSwatheFile())
				{
					if (wNumColours == 1)
  					{
  						cyDIB *= 2;
						int SwatheCount = (int) ::DIBSize(lpDIB);	
  						bReturnOk = ProcessSwatheFile(Card, Head, Row, sPrintParams, cyDIB, lpDIBBits, SwatheCount);
						m_SwatheFile = true;
                        //if (!bReturnOk)
 							//No muestro los errores: DisplayErrorMessage(IDS_FAILEDTOLOADIMAGETOHEAD_ERROR, MB_OK | MB_APPLMODAL | MB_ICONSTOP);
					}
					else
					{
						//No muestro los errores: DisplayErrorMessage(IDS_FAILEDTOLOADIMAGETOHEAD_ERROR, MB_OK | MB_APPLMODAL | MB_ICONSTOP);
                        bReturnOk = false;
					}
 
 				}
				else
				{
                    /*
					if (wNumColours == MONO || m_DiffusionParams.Enabled)
					{
						bReturnOk = PrintBinaryBMP(Card, Head, Row, sPrintParams, cyDIB, lpDIBBits);
                        //if (!bReturnOk)
							//No muestro los errores: DisplayErrorMessage(IDS_FAILEDTOLOADIMAGETOHEAD_ERROR, MB_OK | MB_APPLMODAL | MB_ICONSTOP);
					}
					else
					{
						//No muestro los errores: DisplayErrorMessage(IDS_BINARYHEADGREYSCALEIMAGE_ERROR, MB_OK | MB_APPLMODAL | MB_ICONSTOP);
                        bReturnOk = false;
					}
                    */
				}
			}
			else
			{										// Greyscale
				if (pDoc->bSwatheFile())
				{
					if ((sPrintParams.m_bEnable2Bit && wNumColours == MONO) || (!sPrintParams.m_bEnable2Bit && wNumColours == 4))
  					{
  						cyDIB *= 2;
						int SwatheCount = (int) ::DIBSize(lpDIB);	
  						bReturnOk = ProcessSwatheFile(Card, Head, Row, sPrintParams, cyDIB, lpDIBBits, SwatheCount);
                        m_SwatheFile = true;
                        //if (!bReturnOk)
 							//No muestro los errores: DisplayErrorMessage(IDS_FAILEDTOLOADIMAGETOHEAD_ERROR, MB_OK | MB_APPLMODAL | MB_ICONSTOP);
					}
					else
					{
                        //No muestro los errores: isplayErrorMessage(IDS_FAILEDTOLOADIMAGETOHEAD_ERROR, MB_OK | MB_APPLMODAL | MB_ICONSTOP);
                        bReturnOk = false;
					}
 				}
  				else
  				{
					switch (wNumColours)
					{
						case MONO:
							//No muestro los errores: DisplayErrorMessage(IDS_GREYSCALEHEADBINARYIMAGE_ERROR, MB_OK | MB_APPLMODAL | MB_ICONSTOP);
                            bReturnOk = false;
							break;
						case BIT8COLOUR:
							bReturnOk = Print256GreyScaleBMP(Card, Head, Row, sPrintParams, cyDIB, lpDIBBits);
							if (!bReturnOk)
								//No muestro los errores: DisplayErrorMessage(IDS_FAILEDTOLOADIMAGETOHEAD_ERROR, MB_OK | MB_APPLMODAL | MB_ICONSTOP);
							break;
						default:
							//No muestro los errores: DisplayErrorMessage(IDS_NOT_4BIT_IMAGE, MB_OK | MB_APPLMODAL | MB_ICONSTOP);
                            bReturnOk = false;
					}
				}
			}
		}
        //No sé que hace: ::GlobalUnlock((HGLOBAL) hDIB);
	}
	return bReturnOk;
}


bool XUSBData::ProcessSwatheFile(DWORD Card, DWORD Head, DWORD Row, UpdatedPrintDataParameters sPrintParams, int cyDIB, LPSTR lpDIBBits, int SwatheCount)
 {
	m_DataParams[Card][Head].OriginalParameters.TotalImageWidth[0] = SwatheCount * (PrintJets[m_DataParams[Card][Head].OriginalParameters.HeadType] / 
		(sPrintParams.m_SeparateRows ? NUMBEROFROWS[m_DataParams[Card][Head].OriginalParameters.HeadType]:1));
	m_DataParams[Card][Head].OriginalParameters.TotalImageWidth[1] = m_DataParams[Card][Head].OriginalParameters.TotalImageWidth[0];
  	SetupCommonDataParameters(Card, Head, Row, sPrintParams, cyDIB, lpDIBBits, true);
  	if (m_DataParams[Card][Head].OriginalParameters.NumberSwathes[Row] == 0)
        return false;
  
  	m_DataParams[Card][Head].OriginalParameters.Spare[0] = 1;
  	return SendSwathes(Card, Head, Row, true);
 }

 
 // ----------------------------------------------------------------------
//
// SetupCommonDataParameters
//
// Calls the function in the DLL to setup the parameters in the structure based on the registry settings
//
// This function sets up the parameters in the structure that are common for GS and Binary heads
// 
// --------------------------------------------------------------------
bool XUSBData::SetupCommonDataParameters(DWORD Card, DWORD Head, DWORD Row, UpdatedPrintDataParameters sPrintParams,int cyDIB, LPSTR lpDIBBits, bool SwatheFile)
{
    bool Success = true;

	// Setup the pointer to the image data .... this is only for reference and not used by the dll
	m_DataParams[Card][Head].OriginalParameters.lpDIBBits = lpDIBBits;

	if (m_MaskEnable)							// if masking is required
		Success = m_pMaskClass->LoadInMaskArray(Card, Head, Row);

	if (!SwatheFile)							// if data is read in from swathefile
		m_DataParams[Card][Head].OriginalParameters.TotalImageWidth[Row] = cyDIB;

	if (Success)								// call function in dll to setup structure based on registry
		Success = bXaarScorpionSetupImageDataParametersUpdated(Card, Head, Row, m_DataParams[Card][Head], m_HeadsRequired[Card]);
	else
		m_DataParams[Card][Head].OriginalParameters.NumberSwathes[0] = m_DataParams[Card][Head].OriginalParameters.NumberSwathes[1] = 0;

	m_DataParams[Card][Head].OriginalParameters.InterGap = m_InterGap;	// set the gap
	return Success;
}

// ----------------------------------------------------------------------
//
// Print256GreyScaleBMP
// This sets up the parameters in the structure, the parameters are
// used when the data and the registers are sent to the scorpion board
// The LUT for image data has a range of values centered round each source point
// ie 6 dpd would require 51 @ values
// The interval (ie number of values to divide total range by) is centered so the
// 1st and last values share the interval
// 26 51 51 51 51 26	How many
// 0  1  2  3  4  5		LUT value
// ----------------------------------------------------------------------
bool XUSBData::Print256GreyScaleBMP(DWORD Card, DWORD Head, DWORD Row, UpdatedPrintDataParameters sPrintParams, int cyDIB, LPSTR lpDIBBits)
{

    SetupCommonDataParameters(Card, Head, Row, sPrintParams, cyDIB, lpDIBBits, false);

//	BYTE Palette[NUMBERGREYSCALES] = {3,5,7,9,11, 0,0,0,0,0,0,0,0,0,0,0};
//	bXaarScorpionWriteRemapTable(Card, Head, 4, Palette);

	if (m_DataParams[Card][Head].OriginalParameters.NumberSwathes[Row] == 0)
        return false;

	DWORD Interval = 255 / (sPrintParams.m_uiGreyLevels - 1);		// determine number of values at in each interval
	DWORD Index = 0;
	BYTE Value;
	DWORD LoopCount = Interval / 2;									// the interval has 1/2 the values at the start and the rest at the end
	if (sPrintParams.m_bEnable2Bit)
	{
		m_BackgroundValue = (BYTE)(sPrintParams.m_GSBackgroundValue + (sPrintParams.m_GSBackgroundValue << 2) +
					    			(sPrintParams.m_GSBackgroundValue << 4) + (sPrintParams.m_GSBackgroundValue << 6));
		for (DWORD Grey = 0; Grey < sPrintParams.m_uiGreyLevels; Grey++)
		{
			if (sPrintParams.m_bPaletteRemap)
				Value = (BYTE)(sPrintParams.m_PaletteValue[Grey] + (sPrintParams.m_PaletteValue[Grey] << 2) +
								(sPrintParams.m_PaletteValue[Grey] << 4) + (sPrintParams.m_PaletteValue[Grey] << 6));
			else
				Value = (BYTE)(Grey + (Grey << 2) + (Grey << 4) + (Grey << 6));		// we now setup the values as defined by user
			for (DWORD Loop = 0; Loop < LoopCount && Index < sizeof(m_LUTValue); Loop++)
				m_LUTValue[Index++] = Value;
			LoopCount = Interval;
		}
	}
	else
	{
		for (DWORD Grey = 0; Grey < sPrintParams.m_uiGreyLevels; Grey++)
		{
			if (sPrintParams.m_bPaletteRemap)
				Value = (BYTE)(sPrintParams.m_PaletteValue[Grey] + (sPrintParams.m_PaletteValue[Grey] << 4));
			else
				Value = (BYTE)(Grey + (Grey << 4));		// we now setup the values as defined by user
			for (DWORD Loop = 0; Loop < LoopCount && Index < sizeof(m_LUTValue); Loop++)
				m_LUTValue[Index++] = Value;
			LoopCount = Interval;
		}
		m_BackgroundValue = (BYTE)(sPrintParams.m_GSBackgroundValue + (sPrintParams.m_GSBackgroundValue << 4));
	}
	return SendSwathes(Card, Head, Row, true);
}

// ----------------------------------------------------------------------
//
// SendSwathes
//
// This function is responsible for determining how many swathes can be
// downloaded to the XUSB and then calling the correct function.
// The function called will generate the swathe data and then it will
// call the function to download the swathe data and swathe control
// blocks to the XUSB.
//
// It looks to see if the first image has been loaded - ForceImageLoad, if 
// first image then load swathes.
// If all swathes dont fit into memory, then we need to see if some has been 
// printed and made room for another swathe to be downloaded
// --------------------------------------------------------------------
bool XUSBData::SendSwathes(DWORD Card, DWORD Head, DWORD Row, bool ForceImageLoad)
{
    bool DownLoadOk = true;
	// if this is the first time and we want to force swathe download or
	// it isnt the first time but all swathes didnt fit into the XUSB
	if (ForceImageLoad || !m_DataParams[Card][Head].OriginalParameters.AllSwathesFit[Row])	// image wont all fit in one go
	{
		DWORD SwathesToStore = 0;		// clear the number of swathes to store
		// if first time then set the number of swathes to store to be the number
		// of swathes that will fit into the XUSB
		if (ForceImageLoad)				
		{
			SwathesToStore = m_DataParams[Card][Head].OriginalParameters.SwatheMemoryCount[Row];
			// clear PreviousPrintSwathe as there are none when we are on the first swathe			
			m_DataParams[Card][Head].OriginalParameters.PreviousPrintSwathe[Row] = 0;	
			if (m_FixedVariableMode)				// might want to force fixed
			{
				// if all swathes dont fit and we want to force to fit then modify parameters
				if (!m_DataParams[Card][Head].OriginalParameters.AllSwathesFit[Row] && m_ForceFixed[Card][Head])
				{
					for (DWORD Loop = 0; Loop < MAXROWS; Loop++)
					{
						m_DataParams[Card][Head].OriginalParameters.AllSwathesFit[Loop] = true;
						m_DataParams[Card][Head].OriginalParameters.NumberSwathes[Loop] = m_DataParams[Card][Head].OriginalParameters.SwatheMemoryCount[Loop];
					}
				}
			}
		}
		else
		{
			// We have already downloaded some swathe data to the XUSB, we need to see if there is
			// room for any more
//			DWORD SavePreviousPrintSwathe = m_DataParams[Card][Head].OriginalParameters.PreviousPrintSwathe[m_DataParams[Card][Head].OriginalParameters.NibbleIndex];
			m_DataParams[Card][Head].OriginalParameters.NibbleIndex = Row;	// set the nibble index to the row of interest
			// Call XUSB function, this function will read the memory location of the XUSB and from
			// that information determine which swathe is currently being printed
			bXaarScorpionSwathesToStoreUpdated(Card, Head, m_DataParams[Card][Head], SwathesToStore);
			if (m_ForceImageLoadTest)
				SwathesToStore = 1;
		}

		// for all swathes that there is space to store
		for (DWORD Loop = 0; Loop < SwathesToStore && DownLoadOk; Loop++)
		{
            DownLoadOk = GenerateGSPrintData(Card, Head, Row);		// generate grey scale swathe data
			m_DataParams[Card][Head].OriginalParameters.StoredSwathes[Row]++;		// increase the number of swathes stored to the XUSB
			if (Row == 0 && !m_SeparateRows)
				m_DataParams[Card][Head].OriginalParameters.StoredSwathes[1]++;		// if first row or dual rows head increase swathes on 2nd row
		}
	}
	return DownLoadOk;
}

// ----------------------------------------------------------------------
//
// GenerateGSPrintData
// The BMP data is stored in scan line format
// The image is packed in memory with 2 channels per byte for greyscale format
// Palette remap may be setup and a software palette remap applied to the print data
// This code is used when a Scorpion 760 GS print head is connected, and two rows of print data is setup
// The data is set into D0..D2 normally for the printhead
// The data is setup for all printing channels of the head and the data and parameters
// sent to the scorpion board.
// ----------------------------------------------------------------------
bool XUSBData::GenerateGSPrintData(DWORD Card, DWORD Head, DWORD Row)
{
    bool PointerSetup = false;
	DWORD SourceLine;
	BYTE *abSourceStroke = new BYTE [m_DataParams[Card][Head].OriginalParameters.ImageSize[Row]];

	DWORD NumSwathe = m_DataParams[Card][Head].OriginalParameters.StoredSwathes[Row] % m_DataParams[Card][Head].OriginalParameters.NumberSwathes[Row];
	DWORD Swathe = m_DataParams[Card][Head].OriginalParameters.bReverseSwatheOrder ? (m_DataParams[Card][Head].OriginalParameters.NumberSwathes[Row] - NumSwathe - 1):NumSwathe;
	if (NumSwathe == 0 && m_ImagePtr == NULL && m_KeepImageSwathe)	// use this for testing speed of download
	{
		PointerSetup = true;
		m_ImagePtr = abSourceStroke;
	}

	if ((m_DataParams[Card][Head].OriginalParameters.PrintTransportMode == BIPRINT || m_DataParams[Card][Head].OriginalParameters.PrintTransportMode == UNIBIPRINT) && (NumSwathe % 2 == 1))	// bi direction scanning and on reverse pass
        m_DataParams[Card][Head].OriginalParameters.ForwardBuffer = false;
	else
		m_DataParams[Card][Head].OriginalParameters.ForwardBuffer = true;

    bool ForwardBuffer = m_DataParams[Card][Head].OriginalParameters.ForwardBuffer;		// need to adjust order of filling image buffer
	if (m_DataParams[Card][Head].OriginalParameters.bReverseImageOrder)
		ForwardBuffer = !ForwardBuffer;

	DWORD BaseSourceByteOffset = m_DataParams[Card][Head].OriginalParameters.SwatheStartIndex + Swathe * m_DataParams[Card][Head].OriginalParameters.SwatheIncrement; // point to correct swathe start
	if (m_SwatheShiftWanted && m_DataParams[Card][Head].OriginalParameters.bReverseSwatheOrder && BaseSourceByteOffset > 0 && 
		(m_DataParams[Card][Head].OriginalParameters.TotalImageWidth[Row] % m_DataParams[Card][Head].OriginalParameters.HeadChannels) != 0)
		BaseSourceByteOffset -= (m_DataParams[Card][Head].OriginalParameters.HeadChannels - (m_DataParams[Card][Head].OriginalParameters.TotalImageWidth[Row] % m_DataParams[Card][Head].OriginalParameters.HeadChannels)); // offset for reverse
	DWORD ImageWidth;
	if (m_SwatheShiftWanted && m_DataParams[Card][Head].OriginalParameters.bReverseSwatheOrder && NumSwathe + 1 == m_DataParams[Card][Head].OriginalParameters.NumberSwathes[Row] &&
		(m_DataParams[Card][Head].OriginalParameters.TotalImageWidth[Row] % m_DataParams[Card][Head].OriginalParameters.HeadChannels) != 0)
		ImageWidth = m_DataParams[Card][Head].OriginalParameters.TotalImageWidth[Row] % m_DataParams[Card][Head].OriginalParameters.HeadChannels;
	else
	{
		if (m_DataParams[Card][Head].OriginalParameters.TotalImageWidth[Row] > BaseSourceByteOffset)
			ImageWidth = min(m_DataParams[Card][Head].OriginalParameters.TotalImageWidth[Row] - BaseSourceByteOffset, m_DataParams[Card][Head].OriginalParameters.HeadChannels);	// number of jets in chip bank
		else
			ImageWidth = 0;							// 0 jets to print
	}
	UINT uiLineOffset = WIDTHBYTES(m_DataParams[Card][Head].OriginalParameters.TotalImageWidth[Row] * 8);

	DWORD StartClear = ImageWidth;
	DWORD EndClear = m_DataParams[Card][Head].OriginalParameters.HeadChannels;
	BYTE bTargetMask[2];
	DWORD ShiftValue = 0;
	if (m_SwatheShiftWanted)
		ShiftValue = PrintJets[m_DataParams[Card][Head].OriginalParameters.HeadType] - ImageWidth;

	DWORD TargetByteOffset = ShiftValue / 2;

	if (m_SwatheShiftWanted)
	{
		StartClear = 0;
		EndClear = TargetByteOffset;
	}

	if (m_SeparateRows || (!m_SeparateRows && m_DataParams[Card][Head].OriginalParameters.BufferReverse[Row]))
	{
		bTargetMask[0] = START_NIBBLE_MASK >> (m_DataParams[Card][Head].OriginalParameters.BitDifference * ((1 + ShiftValue) % 2));
		bTargetMask[1] = START_NIBBLE_MASK >> (m_DataParams[Card][Head].OriginalParameters.BitDifference * (ShiftValue % 2));
	}
	else
	{
		bTargetMask[0] = START_NIBBLE_MASK >> (m_DataParams[Card][Head].OriginalParameters.BitDifference * (ShiftValue % 2));
		bTargetMask[1] = START_NIBBLE_MASK >> (m_DataParams[Card][Head].OriginalParameters.BitDifference * ((1 + ShiftValue) % 2));
	}

	memset(abSourceStroke, 0, m_DataParams[Card][Head].OriginalParameters.ImageSize[Row]);

	for (DWORD iOuterLoop = 0; iOuterLoop < m_DataParams[Card][Head].OriginalParameters.ImageLength[Row] ; iOuterLoop++)
	{
		if (ForwardBuffer)
			SourceLine = m_DataParams[Card][Head].OriginalParameters.ImageLength[Row] - iOuterLoop - 1;
		else
			SourceLine = iOuterLoop;
		BYTE MaskNibble = 0;
		DWORD TargetByte = m_DataParams[Card][Head].OriginalParameters.SourceStrokeWidth * iOuterLoop + TargetByteOffset;
		DWORD SourceByteOffset = BaseSourceByteOffset;

		for (DWORD uiInnerLoop = 0; uiInnerLoop < ImageWidth;uiInnerLoop++)
		{
			abSourceStroke[TargetByte] |= (bTargetMask[MaskNibble] & m_LUTValue[(BYTE)m_DataParams[Card][Head].OriginalParameters.lpDIBBits[uiLineOffset * SourceLine + uiInnerLoop + SourceByteOffset]]);
			MaskNibble = (MaskNibble + 1) % 2;
			if (MaskNibble == 0)
				TargetByte++;
		}

		// now clear the space to background
		for (DWORD uiInnerLoop = StartClear; uiInnerLoop < EndClear; uiInnerLoop++)
		{
			abSourceStroke[TargetByte] |= bTargetMask[MaskNibble] & m_BackgroundValue;
			MaskNibble = (MaskNibble + 1) % 2;
			if (MaskNibble == 0)
				TargetByte++;
		}
	}
	gThisValue++;
    bool DownLoadOk = true;
	if (!m_TestSwatheDataCreateOnly)
        DownLoadOk = SetupPrintDataRegisterRows(Card, Head, Row, abSourceStroke, false);
	if (m_ImageToDisc)
        GSImageToDisc(Card, Head, Row, abSourceStroke, false);
	if (m_WriteSwatheFile)
		WriteSwatheFile(Card, Head, abSourceStroke, NumSwathe);
	if (!PointerSetup)						// use this for testing speed of download
		delete [] abSourceStroke;
	if (m_DataParams[Card][Head].OriginalParameters.PrintTransportMode == UNIPRINTWITHSPIT)
		GenerateSpitSwathe(Card, Head, Row);

	return DownLoadOk;
}

// ----------------------------------------------------------------------
//
// SetupPrintDataRegisterRows
//
// This function sends the swathe data and swathe control blocks to the 
// XUSB.  It is called for each swathe on each head on each XUSB.
//
// It sends the swathe data that is passed in the abSourceStroke pointer
// to the XUSB.  If masking is selected then a different function is called
// that performs the masking before sending the swathe data.
//
// It also sets up parameters for the swathe control blocks before calling
// the DLL function which generates the swathe control blocks.
//
// --------------------------------------------------------------------
bool XUSBData::SetupPrintDataRegisterRows(DWORD Card, DWORD Head, DWORD InRow, BYTE *abSourceStroke, bool SpitSwatheData)
{
	// MemoryBlock will be modified by the function that sends the swathe data so we need 
	// to save the location that this swathe will be stored
	DWORD StoreMemory = m_DataParams[Card][Head].OriginalParameters.MemoryBlock[InRow];	// save pointer to where this image block is stored inmemory

    bool DownLoadOk = true;

	m_DataParams[Card][Head].OriginalParameters.NibbleIndex = InRow;					// set the index to the row required
	if (m_MaskEnable)																// if masking is required
		DownLoadOk = bXaarScorpionSetPrintSwatheDataMaskedUpdated(Card, abSourceStroke, m_DataParams[Card][Head], m_pMaskClass->m_Mask[Card][Head][InRow][0], m_pMaskClass->m_Mask[Card][Head][InRow][1]);
	else																			// else download image swathe without masking
		DownLoadOk = bXaarScorpionSetPrintDataUpdated(Card, abSourceStroke, m_DataParams[Card][Head]);

	// if this is a dual row head then set the memory block for the 2nd row to be the same as the 1st row
	if (!m_SeparateRows)															
		m_DataParams[Card][Head].OriginalParameters.MemoryBlock[1] =  m_DataParams[Card][Head].OriginalParameters.MemoryBlock[0];

	// This is the location in the XUSB were the first swathe is stored - it is also useful for when the
	// image is larger than the space in the box and is re-used when the swathes "wrap around" 
	DWORD SaveMemory = m_DataParams[Card][Head].OriginalParameters.FirstMemoryBlock[InRow];

	// This is were the next swathe block would be stored in memory
	DWORD CurrentMemory = m_DataParams[Card][Head].OriginalParameters.MemoryBlock[InRow];

	// for all rows on this head
	for (DWORD Row = 0; Row < m_DataParams[Card][Head].OriginalParameters.NumberOfRows && DownLoadOk; Row++)
	{
		DWORD ActiveRow = Row;			// set the active row to the current row
		if (m_SeparateRows)				// if individual rows
			ActiveRow = InRow;			// set the active row to the row that was passed into the function

		m_DataParams[Card][Head].OriginalParameters.NibbleIndex = ActiveRow;	// Select the row of interest

		// If uni-directional printing with spitting - this will not normally be true
		if (m_DataParams[Card][Head].OriginalParameters.PrintTransportMode == UNIPRINTWITHSPIT)
			bXaarScorpionSetupSwatheSpitBlockParametersUpdated(m_DataParams[Card][Head], SpitSwatheData);
		// normal function that is called - this sets up where the swathe control block is stored
		else			
			bXaarScorpionSetupSwatheBlockParametersUpdated(m_DataParams[Card][Head]);	

		// If this is the last swathe in memory ie the next swathe wraps around
		// then we need to set savememory back to the start of the XUSB swathe memory
		if (m_DataParams[Card][Head].OriginalParameters.LastSwatheInMemory[ActiveRow])
			SaveMemory = m_DataParams[Card][Head].OriginalParameters.FirstMemoryBlock[ActiveRow];		// 1st memory block
		// otherwise savememory is where the next swathe data is stored
		else
			SaveMemory = m_DataParams[Card][Head].OriginalParameters.MemoryBlock[ActiveRow];		// save next memory block

		// now set memory block to where this current swathe is stored so that the swathe control
		// block points to the swathe data that has just been stored
		m_DataParams[Card][Head].OriginalParameters.MemoryBlock[ActiveRow] = StoreMemory;		// point to where this image is stored

		// if this row is selected for print
		if (m_DataParams[Card][Head].OriginalParameters.bSelectHead[ActiveRow])
		{
			// setup the offset for this swathe depending on print direction
			m_DataParams[Card][Head].OriginalParameters.ProductOffset = m_DataParams[Card][Head].OriginalParameters.SaveProductOffset[m_DataParams[Card][Head].OriginalParameters.ForwardBuffer == true ? 0:1][ActiveRow];

			// if sub pixel offset required set up the values - otherwise set it to 0
			if (m_WantSubDelay)
				m_DataParams[Card][Head].OriginalParameters.SubPixelOffset = m_DataParams[Card][Head].OriginalParameters.SaveSubPixelOffset[m_DataParams[Card][Head].OriginalParameters.ForwardBuffer == true ? 0:1][ActiveRow];
			else
				m_DataParams[Card][Head].OriginalParameters.SubPixelOffset = 0;		// disable subpixel

			// set up the dir block depending on if this is a forward pass or not
			if (m_DataParams[Card][Head].OriginalParameters.ForwardBuffer)
				m_DataParams[Card][Head].OriginalParameters.DirBlock = m_DataParams[Card][Head].OriginalParameters.StartDir[ActiveRow];
			else		// if reverse ... we need to swap every other dir for bi directional printing
				m_DataParams[Card][Head].OriginalParameters.DirBlock = (m_DataParams[Card][Head].OriginalParameters.StartDir[ActiveRow] + m_DataParams[Card][Head].OriginalParameters.ThisSwathe) % 2;

			// Log the data structure to disc
			XaarScorpionLogPrintStructInfoUpdated(m_DataParams[Card][Head]);
			// Call the XUSB DLL function to setup and send the XUSB swathe control blocks
			DownLoadOk = bXaarScorpionSetPrintDataParametersUpdated(Card, m_DataParams[Card][Head]);
		}
		m_DataParams[Card][Head].OriginalParameters.MemoryBlock[ActiveRow] = CurrentMemory;	// restore this value
	}
	m_DataParams[Card][Head].OriginalParameters.MemoryBlock[InRow] = SaveMemory;	// now point to next block

	return DownLoadOk;
}





