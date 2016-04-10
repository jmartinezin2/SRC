/*  Copyright (C) 2013 In2 Printing Solutions <info@in2ps.com>
    Author: Jose Maria Martinez

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
#include "inkjetmultiview.h"
#include <QtGlobal>
#include <QFile>
#include <QString>
#include <QMessageBox>
#include <QImage>
#include <QThread>

InkjetMultiview::InkjetMultiview(QObject *parent) :
    QObject(parent)
{
    memset(m_DataParams,0x00,sizeof(m_DataParams));
    m_WriteSwatheFile=false;
    //Testeamos el numero de XUSB que existen
    m_ActualXUSBsFound = qMax(1, (int)XaarScorpionGetXUSBCount());

    bXaarScorpionEnablePrintMode(0,0xffff,false);
}


BOOL InkjetMultiview::SendSwathes(DWORD Card, DWORD Head, DWORD Row, bool ForceImageLoad)
{
    BOOL DownLoadOk = true;
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
        }
        else
        {
            // We have already downloaded some swathe data to the XUSB, we need to see if there is
            // room for any more
            m_DataParams[Card][Head].OriginalParameters.NibbleIndex = Row;	// set the nibble index to the row of interest
            // Call XUSB function, this function will read the memory location of the XUSB and from
            // that information determine which swathe is currently being printed
            bXaarScorpionSwathesToStoreUpdated(Card, Head, m_DataParams[Card][Head], SwathesToStore);
        }

        // for all swathes that there is space to store
        for (DWORD Loop = 0; Loop < SwathesToStore && DownLoadOk; Loop++)
        {
            if (m_DataParams[Card][Head].OriginalParameters.Spare[0] == 1)
                DownLoadOk = GenerateSwathePrintDataUpdated(Card, Head, Row);
            else
            {
                if (m_DataParams[Card][Head].OriginalParameters.Binary)		// binary swathe?
                    DownLoadOk = GenerateBinaryPrintData(Card, Head, Row);	// generate binary swathe data
                else
                {
                    if (m_DataParams[Card][Head].OriginalParameters.Enable2Bit) // 2 bit swathe
                        DownLoadOk = Generate2BitPrintData(Card, Head, Row);	// generate 2 bit swathe data
                    else
                        DownLoadOk = GenerateGSPrintData(Card, Head, Row);		// generate grey scale swathe data
                }
            }
            m_DataParams[Card][Head].OriginalParameters.StoredSwathes[Row]++;		// increase the number of swathes stored to the XUSB
            if (Row == 0 && !m_SeparateRows)
                m_DataParams[Card][Head].OriginalParameters.StoredSwathes[1]++;		// if first row or dual rows head increase swathes on 2nd row
            //Txemari...no necesitamos?
            //m_pApp->SaveDownloadedSwatheCount(Card, Head, Row, m_DataParams[Card][Head].OriginalParameters.StoredSwathes[Row]);
            if (Row == 0 && !m_SeparateRows){};
                //Txemari...No lo necesitamos?
                //m_pApp->SaveDownloadedSwatheCount(Card, Head, 1, m_DataParams[Card][Head].OriginalParameters.StoredSwathes[1]);
        }
    }
    return DownLoadOk;
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
BOOL InkjetMultiview::SendSwathes(DWORD Card, DWORD Head, DWORD Row, DWORD SwathesToStore)
{
    BOOL DownLoadOk = true;

    // for all swathes that there is space to store
    for (DWORD Loop = 0; Loop < SwathesToStore && DownLoadOk; Loop++)
    {
        if (m_DataParams[Card][Head].OriginalParameters.Spare[0] == 1)
            DownLoadOk = GenerateSwathePrintDataUpdated(Card, Head, Row);
        else
        {
            if (m_DataParams[Card][Head].OriginalParameters.Binary)		// binary swathe?
                DownLoadOk = GenerateBinaryPrintData(Card, Head, Row);	// generate binary swathe data
            else
            {
                if (m_DataParams[Card][Head].OriginalParameters.Enable2Bit) // 2 bit swathe
                    DownLoadOk = Generate2BitPrintData(Card, Head, Row);	// generate 2 bit swathe data
                else
                    DownLoadOk = GenerateGSPrintData(Card, Head, Row);		// generate grey scale swathe data
            }
        }
        m_DataParams[Card][Head].OriginalParameters.StoredSwathes[Row]++;		// increase the number of swathes stored to the XUSB
        if (Row == 0 && !m_SeparateRows)
            m_DataParams[Card][Head].OriginalParameters.StoredSwathes[1]++;		// if first row or dual rows head increase swathes on 2nd row
    }
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
BOOL InkjetMultiview::SetupPrintDataRegisterRows(DWORD Card, DWORD Head, DWORD InRow, BYTE *abSourceStroke, bool SpitSwatheData)
{
    // MemoryBlock will be modified by the function that sends the swathe data so we need
    // to save the location that this swathe will be stored
    DWORD StoreMemory = m_DataParams[Card][Head].OriginalParameters.MemoryBlock[InRow];	// save pointer to where this image block is stored in memory


    BOOL DownLoadOk = true;

    m_DataParams[Card][Head].OriginalParameters.NibbleIndex = InRow;					// set the index to the row required
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

// ----------------------------------------------------------------------
//
// SetupCommonDataParameters
//
// Calls the function in the DLL to setup the parameters in the structure based
// on the registry settings
//
// This function sets up the parameters in the structure that are common for GS and Binary heads
//
// --------------------------------------------------------------------
BOOL InkjetMultiview::SetupCommonDataParameters(DWORD Card, DWORD Head, DWORD Row, CPrintParameters sPrintParams,int cyDIB, LPSTR lpDIBBits, bool SwatheFile)
{
    BOOL Success = true;

    // Setup the pointer to the image data .... this is only for reference and not used by the dll
    m_DataParams[Card][Head].OriginalParameters.lpDIBBits = lpDIBBits;

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
// PrintBinaryBMP
// This sets up the parameters in the structure, the parameters are
// used when the data and the registers are sent to the scorpion board
// ----------------------------------------------------------------------
BOOL InkjetMultiview::PrintBinaryBMP(DWORD Card, DWORD Head, DWORD Row, CPrintParameters sPrintParams, int cyDIB, LPSTR lpDIBBits)
{
    SetupCommonDataParameters(Card, Head, Row, sPrintParams, cyDIB, lpDIBBits, false);
    if (m_DataParams[Card][Head].OriginalParameters.NumberSwathes[Row] == 0)
        return false;

    return SendSwathes(Card, Head, Row, true);
}

// ----------------------------------------------------------------------
//
// GenerateBinaryPrintData
// The image is packed in memory with 2 channels per byte for binary format
// This code is used when a Scorpion 380 binary print head is connected, and one row of print data is setup
// The data is set into D3 for the printhead
// The data is setup for all printing channels of the head and the data and parameters
// sent to the scorpion board.
// ----------------------------------------------------------------------
BOOL InkjetMultiview::GenerateBinaryPrintData(DWORD Card, DWORD Head, DWORD Row)
{

    DWORD SourceLine;
    BYTE *abSourceStroke = new BYTE [m_DataParams[Card][Head].OriginalParameters.ImageSize[Row]];
    UINT uiSourceDifference = 1;				// number of bits to store source data

    // Now work out which swathe we should be printing
    DWORD NumSwathe = m_DataParams[Card][Head].OriginalParameters.StoredSwathes[Row] % m_DataParams[Card][Head].OriginalParameters.NumberSwathes[Row];
    DWORD Swathe = m_DataParams[Card][Head].OriginalParameters.bReverseSwatheOrder ? (m_DataParams[Card][Head].OriginalParameters.NumberSwathes[Row] - NumSwathe - 1):NumSwathe;

    if ((m_DataParams[Card][Head].OriginalParameters.PrintTransportMode == BIPRINT || m_DataParams[Card][Head].OriginalParameters.PrintTransportMode == UNIBIPRINT) && (NumSwathe % 2 == 1))	// bi direction scanning and on reverse pass
        m_DataParams[Card][Head].OriginalParameters.ForwardBuffer = false;
    else
        m_DataParams[Card][Head].OriginalParameters.ForwardBuffer = true;
    bool ForwardBuffer = m_DataParams[Card][Head].OriginalParameters.ForwardBuffer;		// need to adjust order of filling image buffer
    if (m_DataParams[Card][Head].OriginalParameters.bReverseImageOrder)
        ForwardBuffer = !ForwardBuffer;


    DWORD BaseSourceByteOffset = (m_DataParams[Card][Head].OriginalParameters.SwatheStartIndex + Swathe * m_DataParams[Card][Head].OriginalParameters.SwatheIncrement) / (8 / uiSourceDifference); // point to correct swathe start

    DWORD ImageWidth = qMin(m_DataParams[Card][Head].OriginalParameters.TotalImageWidth[Row] - (m_DataParams[Card][Head].OriginalParameters.SwatheStartIndex + Swathe * m_DataParams[Card][Head].OriginalParameters.SwatheIncrement), m_DataParams[Card][Head].OriginalParameters.HeadChannels);	// number of jets in chip bank
    UINT uiLineOffset = WIDTHBYTES(m_DataParams[Card][Head].OriginalParameters.TotalImageWidth[Row] * uiSourceDifference);

    DWORD ShiftCount = (uiSourceDifference * Swathe * m_DataParams[Card][Head].OriginalParameters.SwatheIncrement) % (8 / uiSourceDifference);
    BYTE bSourceMaskStart = START_MASK >> ShiftCount;						// binary 1 (bit per pixel)

    BYTE bBaseTargetMask, bBaseStartMask;
    DWORD TargetBaseStart = 0;

    if (m_DataParams[Card][Head].OriginalParameters.BitDifference == 1)			// if  8 bits per byte we have to deal with leading + trailing
    {
        bBaseTargetMask = 1;		// << (m_DataParams[Card][Head].LeadChannels % 8);
        bBaseStartMask = 1;
        TargetBaseStart = 0;		//m_DataParams[Card][Head].LeadChannels / 8;
    }
    else
    {
        bBaseStartMask = bBaseTargetMask = START_NIBBLE_MASK >> 4;
    }

    memset(abSourceStroke, 0, m_DataParams[Card][Head].OriginalParameters.ImageSize[Row]);

    for (DWORD iOuterLoop = 0; iOuterLoop < m_DataParams[Card][Head].OriginalParameters.ImageLength[Row] ; iOuterLoop++)
    {
        if (ForwardBuffer)
            SourceLine = m_DataParams[Card][Head].OriginalParameters.ImageLength[Row] - iOuterLoop - 1;
        else
            SourceLine = iOuterLoop;

        BYTE bSourceMask = bSourceMaskStart;
        BYTE bTargetMask = bBaseTargetMask;
        DWORD TargetRowOffset = m_DataParams[Card][Head].OriginalParameters.SourceStrokeWidth * iOuterLoop;
        DWORD TargetByte = TargetBaseStart;			//0;
        DWORD SourceByteOffset = BaseSourceByteOffset;
        for (DWORD uiInnerLoop = 0; uiInnerLoop < ImageWidth; uiInnerLoop++)
        {
            BYTE Data = m_DataParams[Card][Head].OriginalParameters.lpDIBBits[uiLineOffset * SourceLine + SourceByteOffset];
            if (m_DataParams[Card][Head].OriginalParameters.bPaletteRemap)			// palette remap required
            {
                if ((Data & bSourceMask) == 0)	// this bit need to be set
                    abSourceStroke[TargetRowOffset + TargetByte] |= bTargetMask;	// then set bit in output data
            }
            else
            {
                if (Data & bSourceMask)
                    abSourceStroke[TargetRowOffset + TargetByte] |= bTargetMask;
            }

            bSourceMask >>= uiSourceDifference;
            if (bSourceMask == 0)
            {
                bSourceMask = START_MASK;
                SourceByteOffset++;
            }
            bTargetMask <<= m_DataParams[Card][Head].OriginalParameters.BitDifference;			// for nibble storage
            if (bTargetMask == 0)
            {
                bTargetMask = bBaseStartMask;
                TargetByte++;
            }
        }
        if (m_DataParams[Card][Head].OriginalParameters.bBinaryBackgroundInvert)			// now clear the space to background if invert required
            for (DWORD uiInnerLoop = ImageWidth; uiInnerLoop < m_DataParams[Card][Head].OriginalParameters.HeadChannels; uiInnerLoop++)
            {
                abSourceStroke[TargetRowOffset + TargetByte] |= bTargetMask;
                bTargetMask <<= m_DataParams[Card][Head].OriginalParameters.BitDifference;
                if (bTargetMask == 0)
                {
                    bTargetMask = bBaseStartMask;
                    TargetByte++;
                }
            }
    }
    BOOL DownLoadOk = true;
    DownLoadOk = SetupPrintDataRegisterRows(Card, Head, Row, abSourceStroke, false);
    delete [] abSourceStroke;
    return DownLoadOk;
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
BOOL InkjetMultiview::Print256GreyScaleBMP(DWORD Card, DWORD Head, DWORD Row, CPrintParameters sPrintParams, int cyDIB, LPSTR lpDIBBits)
{

    SetupCommonDataParameters(Card, Head, Row, sPrintParams, cyDIB, lpDIBBits, false);

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
// GenerateGSPrintData
// The BMP data is stored in scan line format
// The image is packed in memory with 2 channels per byte for greyscale format
// Palette remap may be setup and a software palette remap applied to the print data
// This code is used when a Scorpion 760 GS print head is connected, and two rows of print data is setup
// The data is set into D0..D2 normally for the printhead
// The data is setup for all printing channels of the head and the data and parameters
// sent to the scorpion board.
// ----------------------------------------------------------------------
BOOL InkjetMultiview::GenerateGSPrintData(DWORD Card, DWORD Head, DWORD Row)
{

    DWORD SourceLine;
    BYTE *abSourceStroke = new BYTE [m_DataParams[Card][Head].OriginalParameters.ImageSize[Row]];

    DWORD NumSwathe = m_DataParams[Card][Head].OriginalParameters.StoredSwathes[Row] % m_DataParams[Card][Head].OriginalParameters.NumberSwathes[Row];
    DWORD Swathe = m_DataParams[Card][Head].OriginalParameters.bReverseSwatheOrder ? (m_DataParams[Card][Head].OriginalParameters.NumberSwathes[Row] - NumSwathe - 1):NumSwathe;

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
            ImageWidth = qMin(m_DataParams[Card][Head].OriginalParameters.TotalImageWidth[Row] - BaseSourceByteOffset, m_DataParams[Card][Head].OriginalParameters.HeadChannels);	// number of jets in chip bank
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
    //Txemari...no lo usamos?
    //gThisValue++;
    BOOL DownLoadOk = true;
    DownLoadOk = SetupPrintDataRegisterRows(Card, Head, Row, abSourceStroke, false);
    delete [] abSourceStroke;

    return DownLoadOk;
}

// ----------------------------------------------------------------------
//
// Generate2BitPrintData
// The BMP data is stored in scan line format
// The image is packed in memory with 2 channels per byte for greyscale format
// Palette remap may be setup and a software palette remap applied to the print data
// This code is used when a Scorpion 760 GS print head is connected, and two rows of print data is setup
// The data is set into D0..D2 normally for the printhead
// The data is setup for all printing channels of the head and the data and parameters
// sent to the scorpion board.
// ----------------------------------------------------------------------
BOOL InkjetMultiview::Generate2BitPrintData(DWORD Card, DWORD Head, DWORD Row)
{

    DWORD SourceLine;
    BYTE *abSourceStroke = new BYTE [m_DataParams[Card][Head].OriginalParameters.ImageSize[Row]];

    DWORD NumSwathe = m_DataParams[Card][Head].OriginalParameters.StoredSwathes[Row] % m_DataParams[Card][Head].OriginalParameters.NumberSwathes[Row];
    DWORD Swathe = m_DataParams[Card][Head].OriginalParameters.bReverseSwatheOrder ? (m_DataParams[Card][Head].OriginalParameters.NumberSwathes[Row] - NumSwathe - 1):NumSwathe;

    if ((m_DataParams[Card][Head].OriginalParameters.PrintTransportMode == BIPRINT || m_DataParams[Card][Head].OriginalParameters.PrintTransportMode == UNIBIPRINT) && (NumSwathe % 2 == 1))	// bi direction scanning and on reverse pass
        m_DataParams[Card][Head].OriginalParameters.ForwardBuffer = false;
    else
        m_DataParams[Card][Head].OriginalParameters.ForwardBuffer = true;

    bool ForwardBuffer = m_DataParams[Card][Head].OriginalParameters.ForwardBuffer;		// need to adjust order of filling image buffer
    if (m_DataParams[Card][Head].OriginalParameters.bReverseImageOrder)
        ForwardBuffer = !ForwardBuffer;

    DWORD BaseSourceByteOffset = m_DataParams[Card][Head].OriginalParameters.SwatheStartIndex + Swathe * m_DataParams[Card][Head].OriginalParameters.SwatheIncrement; // point to correct swathe start
    DWORD ImageWidth;
    if (m_DataParams[Card][Head].OriginalParameters.TotalImageWidth[Row] > m_DataParams[Card][Head].OriginalParameters.SwatheStartIndex + Swathe * m_DataParams[Card][Head].OriginalParameters.SwatheIncrement)
        ImageWidth = qMin(m_DataParams[Card][Head].OriginalParameters.TotalImageWidth[Row] - (m_DataParams[Card][Head].OriginalParameters.SwatheStartIndex + Swathe * m_DataParams[Card][Head].OriginalParameters.SwatheIncrement), m_DataParams[Card][Head].OriginalParameters.HeadChannels);	// number of jets in chip bank
    else
        ImageWidth = 0;							// 0 jets to print
    UINT uiLineOffset = WIDTHBYTES(m_DataParams[Card][Head].OriginalParameters.TotalImageWidth[Row] * 8);

    BYTE bTargetMask[4];
    if (m_SeparateRows || (!m_SeparateRows && m_DataParams[Card][Head].OriginalParameters.BufferReverse[Row]))
    {
        bTargetMask[0] = 0x03;
        bTargetMask[1] = 0x0C;
        bTargetMask[2] = 0x30;
        bTargetMask[3] = 0xC0;
    }
    else
    {
        bTargetMask[0] = 0x0C;
        bTargetMask[1] = 0x03;
        bTargetMask[2] = 0xC0;
        bTargetMask[3] = 0x30;
    }

    memset(abSourceStroke, 0, m_DataParams[Card][Head].OriginalParameters.ImageSize[Row]);

    for (DWORD iOuterLoop = 0; iOuterLoop < m_DataParams[Card][Head].OriginalParameters.ImageLength[Row] ; iOuterLoop++)
    {
        if (ForwardBuffer)
            SourceLine = m_DataParams[Card][Head].OriginalParameters.ImageLength[Row] - iOuterLoop - 1;
        else
            SourceLine = iOuterLoop;
        BYTE MaskNibble = 0;
        DWORD TargetByte = m_DataParams[Card][Head].OriginalParameters.SourceStrokeWidth * iOuterLoop;
        DWORD SourceByteOffset = BaseSourceByteOffset;

        for (DWORD uiInnerLoop = 0; uiInnerLoop < ImageWidth;uiInnerLoop++)
        {
            abSourceStroke[TargetByte] |= (bTargetMask[MaskNibble] & m_LUTValue[(BYTE)m_DataParams[Card][Head].OriginalParameters.lpDIBBits[uiLineOffset * SourceLine + uiInnerLoop + SourceByteOffset]]);
            MaskNibble = (MaskNibble + 1) % 4;
            if (MaskNibble == 0)
                TargetByte++;
        }

        // now clear the space to background
        for (DWORD uiInnerLoop = ImageWidth; uiInnerLoop < m_DataParams[Card][Head].OriginalParameters.HeadChannels; uiInnerLoop++)
        {
            abSourceStroke[TargetByte] |= bTargetMask[MaskNibble] & m_BackgroundValue;
            MaskNibble = (MaskNibble + 1) % 2;
            if (MaskNibble == 0)
                TargetByte++;
        }
    }
    //Txemri...no lo usamos?
    //gThisValue++;
    BOOL DownLoadOk = true;
    DownLoadOk = SetupPrintDataRegisterRows(Card, Head, Row, abSourceStroke, false);
    delete [] abSourceStroke;
    return DownLoadOk;
}

 BOOL InkjetMultiview::ProcessSwatheFile(DWORD Card, DWORD Head, DWORD Row, CPrintParameters sPrintParams, int cyDIB, LPSTR lpDIBBits, int SwatheCount)
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
// GenerateSwathePrintData
// The Data is read in from the file in the required format
// At the start of the file are 3 DWORDS, these contain the
// SwatheWidth, SwatheLength and SwatheCount
// The file that contains the swathedata end on end
// ----------------------------------------------------------------------
BOOL InkjetMultiview::GenerateSwathePrintData(DWORD Card, DWORD Head, DWORD Row)
{
    BOOL DownLoadOk = false;
    //BYTE *abSourceStroke = new BYTE [m_DataParams[Card][Head].OriginalParameters.ImageSize[Row]];

    DWORD NumSwathe = m_DataParams[Card][Head].OriginalParameters.StoredSwathes[Row] % m_DataParams[Card][Head].OriginalParameters.NumberSwathes[Row];
    DWORD Swathe = m_DataParams[Card][Head].OriginalParameters.bReverseSwatheOrder ? (m_DataParams[Card][Head].OriginalParameters.NumberSwathes[Row] - NumSwathe - 1):NumSwathe;

    if ((m_DataParams[Card][Head].OriginalParameters.PrintTransportMode == BIPRINT || m_DataParams[Card][Head].OriginalParameters.PrintTransportMode == UNIBIPRINT) && (NumSwathe % 2 == 1))	// bi direction scanning and on reverse pass
        m_DataParams[Card][Head].OriginalParameters.ForwardBuffer = false;
    else
        m_DataParams[Card][Head].OriginalParameters.ForwardBuffer = true;

    long SeekPosition  = 3 * sizeof(DWORD) + Swathe * m_DataParams[Card][Head].OriginalParameters.ImageSize[Row];

    /*Txemari...modificado a QFile
    FILE *FileStream;
    if (fopen_s(&FileStream, m_DataParams[Card][Head].OriginalParameters.lpDIBBits, "r" ) == 0)
    {
        fseek(FileStream, SeekPosition, SEEK_SET);
        DWORD ReadCount = (DWORD)fread(abSourceStroke, 1, m_DataParams[Card][Head].OriginalParameters.ImageSize[Row], FileStream);
        fclose(FileStream);
        if (ReadCount == m_DataParams[Card][Head].OriginalParameters.ImageSize[Row])
             DownLoadOk = SetupPrintDataRegisterRows(Card, Head, Row, abSourceStroke, false);
    }*/

    QFile FileStream(m_DataParams[Card][Head].OriginalParameters.lpDIBBits);
    unsigned char abSourceStroke[m_DataParams[Card][Head].OriginalParameters.ImageSize[Row]];
    if (FileStream.open(QIODevice::ReadOnly)){
        FileStream.seek(SeekPosition);
        DWORD ReadCount=(DWORD)FileStream.read((char*)abSourceStroke,m_DataParams[Card][Head].OriginalParameters.ImageSize[Row]);
        FileStream.close();
        if (ReadCount == m_DataParams[Card][Head].OriginalParameters.ImageSize[Row])
             DownLoadOk = SetupPrintDataRegisterRows(Card, Head, Row, abSourceStroke, false);
    }



    return DownLoadOk;
}

// ----------------------------------------------------------------------
//
// GenerateSwathePrintDataUpdated
// The Data is read in from the file in the required format
// At the start of the file are 4 DWORDS, these contain the
// SwatheWidth, SwatheLength, SwatheCount and BitCount
// The file that contains the swathedata end on end
// ----------------------------------------------------------------------
BOOL InkjetMultiview::GenerateSwathePrintDataUpdated(DWORD Card, DWORD Head, DWORD Row)
{
    BOOL DownLoadOk = false;
    //BYTE *abSourceStroke = new BYTE [m_DataParams[Card][Head].OriginalParameters.ImageSize[Row]];

    DWORD NumSwathe = m_DataParams[Card][Head].OriginalParameters.StoredSwathes[Row] % m_DataParams[Card][Head].OriginalParameters.NumberSwathes[Row];
    DWORD Swathe = m_DataParams[Card][Head].OriginalParameters.bReverseSwatheOrder ? (m_DataParams[Card][Head].OriginalParameters.NumberSwathes[Row] - NumSwathe - 1):NumSwathe;

    if ((m_DataParams[Card][Head].OriginalParameters.PrintTransportMode == BIPRINT || m_DataParams[Card][Head].OriginalParameters.PrintTransportMode == UNIBIPRINT) && (NumSwathe % 2 == 1))	// bi direction scanning and on reverse pass
        m_DataParams[Card][Head].OriginalParameters.ForwardBuffer = false;
    else
        m_DataParams[Card][Head].OriginalParameters.ForwardBuffer = true;

    long SeekPosition  = 4 * sizeof(DWORD) + Swathe * m_DataParams[Card][Head].OriginalParameters.ImageSize[Row];

    /*FILE *FileStream;
    if ( fopen_s(&FileStream, m_DataParams[Card][Head].OriginalParameters.lpDIBBits, "rb" ) == 0)
    {
        fseek(FileStream, SeekPosition, SEEK_SET);
        DWORD ReadCount = (DWORD)fread(abSourceStroke, 1, m_DataParams[Card][Head].OriginalParameters.ImageSize[Row], FileStream);
        fclose(FileStream);
        if (ReadCount == m_DataParams[Card][Head].OriginalParameters.ImageSize[Row])
             DownLoadOk = SetupPrintDataRegisterRows(Card, Head, Row, abSourceStroke, false);
    }
    delete [] abSourceStroke;*/

    QFile FileStream(m_DataParams[Card][Head].OriginalParameters.lpDIBBits);
    unsigned char abSourceStroke[m_DataParams[Card][Head].OriginalParameters.ImageSize[Row]];
    if (FileStream.open(QIODevice::ReadOnly)){
        FileStream.seek(SeekPosition);
        DWORD ReadCount=(DWORD)FileStream.read((char*)abSourceStroke,m_DataParams[Card][Head].OriginalParameters.ImageSize[Row]);
        FileStream.close();
        if (ReadCount == m_DataParams[Card][Head].OriginalParameters.ImageSize[Row])
             DownLoadOk = SetupPrintDataRegisterRows(Card, Head, Row, abSourceStroke, false);
    }
    return DownLoadOk;
}

// ----------------------------------------------------------------------
//
// WriteSwatheFile
// Writes a swathedatafile to disc
// The first time this file is created it write the SwatheWidth, SwatheLength and SwatheCount
// ----------------------------------------------------------------------
BOOL InkjetMultiview::WriteSwatheFile(DWORD Card, DWORD Head, BYTE *abSourceStroke, DWORD NumSwathe)
{
    BOOL SetupOk = false;
    DWORD Row = 0;
    QString Filename=QString("c:\\XaarLog\\TestSwathe_%1%2.SWA").arg(Card+1).arg(Head+1);
    QFile FileStream(Filename);
    if (NumSwathe==0) FileStream.open(QIODevice::WriteOnly);
    else FileStream.open(QIODevice::Append);
    if (FileStream.isOpen()){
        if (NumSwathe == 0){
            DWORD BytesPerStroke = PrintJets[m_DataParams[Card][Head].OriginalParameters.HeadType] * m_DataParams[Card][Head].OriginalParameters.BitDifference/ 8;
            FileStream.write((char*)&BytesPerStroke, sizeof(DWORD));
            FileStream.write((char*)&m_DataParams[Card][Head].OriginalParameters.ImageLength[Row], sizeof(DWORD));
            FileStream.write((char*)&m_DataParams[Card][Head].OriginalParameters.NumberSwathes[Row],  sizeof(DWORD));
            FileStream.write((char*)&m_DataParams[Card][Head].OriginalParameters.BitDifference, sizeof(DWORD));
         }
         FileStream.write((char*)abSourceStroke,m_DataParams[Card][Head].OriginalParameters.ImageSize[Row]);
         //fwrite(abSourceStroke, m_DataParams[Card][Head].OriginalParameters.ImageSize[Row], 1, FileStream);
         FileStream.close();
         SetupOk = true;
    }
  /*
    if ( fopen_s( &FileStream, Filename.GetBuffer(), NumSwathe == 0?"w":"a" ) == 0)
    {
        if (NumSwathe == 0)
        {
            DWORD BytesPerStroke = PrintJets[m_DataParams[Card][Head].OriginalParameters.HeadType] * m_DataParams[Card][Head].OriginalParameters.BitDifference/ 8;
            fwrite(&BytesPerStroke, sizeof(DWORD), 1, FileStream);
            fwrite(&m_DataParams[Card][Head].OriginalParameters.ImageLength[Row], sizeof(DWORD), 1, FileStream);
            fwrite(&m_DataParams[Card][Head].OriginalParameters.NumberSwathes[Row],  sizeof(DWORD), 1, FileStream);
            fwrite(&m_DataParams[Card][Head].OriginalParameters.BitDifference, sizeof(DWORD), 1, FileStream);
        }
        fwrite(abSourceStroke, m_DataParams[Card][Head].OriginalParameters.ImageSize[Row], 1, FileStream);
        fclose(FileStream);
        SetupOk = true;
    }*/

    return SetupOk;
}


// ----------------------------------------------------------------------
//
//	LoadActiveBitmapToCard
//  Determines which is the active window and then loads the bitmap
//  Checks that the bitmap loaded is correct for the head type connected
//
// ----------------------------------------------------------------------
BOOL InkjetMultiview::LoadActiveBitmapToCard(DWORD Card, DWORD Head, DWORD Row)
{
    BOOL bReturnOk = FALSE;
    //HDIB  hDIB = NULL;
    CPrintParameters sPrintParams(this,Card, m_Key);
    if (Row == 0)
    {
        m_DataParams[Card][Head].OriginalParameters.bSelectHead[0] = sPrintParams.m_bSelectHead[Head][0];
        m_DataParams[Card][Head].OriginalParameters.bSelectHead[1] = sPrintParams.m_bSelectHead[Head][1];
        m_DataParams[Card][Head].OriginalParameters.AllSwathesFit[0] = true;			// this image will fit entirely in memory
        m_DataParams[Card][Head].OriginalParameters.AllSwathesFit[1] = true;			// this image will fit entirely in memory
    }

    m_DataParams[Card][Head].OriginalParameters.HeadType = bXaarScorpionGetHeadType(Card, Head);
    if (m_DataParams[Card][Head].OriginalParameters.HeadType == NOTSET)
    {
        m_DataParams[Card][Head].OriginalParameters.bSelectHead[0] = false;					// if head is not connected .... don't print to it
        m_DataParams[Card][Head].OriginalParameters.bSelectHead[1] = false;					// if head is not connected .... don't print to it
    }

    if (Row == 0 || (Row == 1 && sPrintParams.m_SeparateRows))
        m_DataParams[Card][Head].OriginalParameters.NumberSwathes[Row] = 0;


    if (!((sPrintParams.m_SeparateRows && m_DataParams[Card][Head].OriginalParameters.bSelectHead[Row]) ||
        (Row == 0 && !sPrintParams.m_SeparateRows && (m_DataParams[Card][Head].OriginalParameters.bSelectHead[0] || m_DataParams[Card][Head].OriginalParameters.bSelectHead[1]))))

        return true;

    // find out which is the active bitmap window
    /*CXUSB_MultiDoc* pDoc = GetDocument();
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
            DisplayErrorMessage(IDS_MULTIPLEFILEUNLOADED_ERROR,  MB_OK | MB_APPLMODAL | MB_ICONSTOP);
            return FALSE;
        }
    }
    else
        hDIB = pDoc->GetHDIB();
    */

    QImage inkjetImagedummy(m_inkjetImage);
    QImage inkjetImage=inkjetImagedummy.mirrored(false,true);

    if (m_DataParams[Card][Head].OriginalParameters.HeadType == NOTSET)
        m_DataParams[Card][Head].OriginalParameters.HeadType = sPrintParams.m_PrintType;		// default to head type entered
    m_DataParams[Card][Head].OriginalParameters.Binary = BINARYHEAD[m_DataParams[Card][Head].OriginalParameters.HeadType];
    m_DataParams[Card][Head].OriginalParameters.NumberOfRows = NUMBEROFROWS[m_DataParams[Card][Head].OriginalParameters.HeadType];

    //if (hDIB != NULL)
    if (!m_inkjetImage.isEmpty())
    {
        bReturnOk = TRUE;

        /*LPSTR lpDIB = (LPSTR) ::GlobalLock((HGLOBAL) hDIB);
        LPSTR lpDIBBits = ::FindDIBBits(lpDIB);	// Pointer to DIB bits

        int cyDIB = (int) ::DIBWidth(lpDIB);				// Size of DIB - y, width of image (ie channels)
        m_DataParams[Card][Head].OriginalParameters.ImageLength[Row] = (int) ::DIBHeight(lpDIB);			// Size of DIB - x, height of image (ie number of strokes)
        WORD wNumColours = (int)::DIBNumColors(lpDIB);*/

        LPSTR lpDIBBits=(LPSTR)inkjetImage.bits();
        int cyDIB = inkjetImage.width();
        m_DataParams[Card][Head].OriginalParameters.ImageLength[Row]=inkjetImage.height();
        WORD wNumColours=inkjetImage.colorCount();

        DWORD MaxJets = MAXJETS[m_DataParams[Card][Head].OriginalParameters.HeadType] / m_DataParams[Card][Head].OriginalParameters.NumberOfRows;

        bool Binary = m_DataParams[Card][Head].OriginalParameters.Binary;
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
            QMessageBox::warning(0,"Error","Imagen muy grande",QMessageBox::Ok);
            //DisplayErrorMessage(IDS_IMAGE_TO_BIG_FOR_MEMORY, MB_ICONINFORMATION | MB_OK);
            bReturnOk = FALSE;
        }

        if ((unsigned int)cyDIB > MAXJETS[m_DataParams[Card][Head].OriginalParameters.HeadType] && // contains at least 2 swathes
            2 * m_DataParams[Card][Head].OriginalParameters.ImageLength[Row] * SourceStrokeWidth > XaarScorpionGetMaxHeadBlocks(m_HeadsRequired[Card]))
        {
            QMessageBox::warning(0,"Error","Imagen muy grande para 2 swathes",QMessageBox::Ok);
            //DisplayErrorMessage(IDS_IMAGE_TO_BIG_TO_FIT_2_SWATHES_IN_MEMORY, MB_ICONINFORMATION | MB_OK);
            bReturnOk = FALSE;
        }

        /*if (wNumColours == MONO && sPrintParams.m_bEnable2Bit && !pDoc->bSwatheFile())
        {
            DisplayErrorMessage(IDS_CANTDO2BITONBINARYIMAGE_ERROR, MB_OK | MB_APPLMODAL | MB_ICONSTOP);
            bReturnOk = FALSE;
        }*/
        if (bReturnOk)
        {
            switch (wNumColours)
            {
                        case MONO:
                            QMessageBox::warning(0,"Error","Error greyscale",QMessageBox::Ok);
                            //DisplayErrorMessage(IDS_GREYSCALEHEADBINARYIMAGE_ERROR, MB_OK | MB_APPLMODAL | MB_ICONSTOP);
                            bReturnOk = FALSE;
                            break;
                        case BIT8COLOUR:
                            bReturnOk = Print256GreyScaleBMP(Card, Head, Row, sPrintParams, cyDIB, lpDIBBits);
                            if (!bReturnOk)
                                QMessageBox::warning(0,"Error","Error la enviar al cabezal",QMessageBox::Ok);
                                //DisplayErrorMessage(IDS_FAILEDTOLOADIMAGETOHEAD_ERROR, MB_OK | MB_APPLMODAL | MB_ICONSTOP);
                            break;
                        default:
                            QMessageBox::warning(0,"Error","No es imagen de 4 bit",QMessageBox::Ok);
                            /*DisplayErrorMessage(IDS_NOT_4BIT_IMAGE, MB_OK | MB_APPLMODAL | MB_ICONSTOP);*/
                            bReturnOk = FALSE;
            }
         }
    }
    return bReturnOk;
}

// ----------------------------------------------------------------------
//
//	SetupParameters
//  Sets up the card related common parameters, prior to setting
//  specific head parameters
//
// ----------------------------------------------------------------------
bool InkjetMultiview::SetupParameters()
{
    m_WantSubDelay = false;
    bool HeadSelected = false;

    for (DWORD Card = 0; Card < (unsigned int)m_ActualXUSBsFound; Card++)
    {
        m_HeadsRequired[Card] = 0;
        CPrintParameters sPrintParams(this,Card, m_Key);
        m_bSelectCard[Card] = sPrintParams.m_bSelectedCard;
        if (Card == 0)
        {
            m_SeparateRows = sPrintParams.m_SeparateRows == 0 ? false:true;
            m_PrintOnce = sPrintParams.m_PrintOnce == 0 ? false:true;
            if (sPrintParams.m_PrintMode == FIXEDGAPCONT)
                m_InterGap = 0xffffff;
            else
                m_InterGap = 0;
        }
        if (m_bSelectCard[Card])
        {
            for (DWORD Head = 0; Head < MAXHEADS; Head++)
                for (DWORD Row = 0; Row < MAXROWS; Row++)
                {
                    if (sPrintParams.m_bSelectHead[Head][Row])
                    {
                        if (bXaarScorpionGetHeadConfig(Card, Head))
                            m_DataParams[Card][Head].OriginalParameters.HeadType = bXaarScorpionGetHeadType(Card, Head);
                        if (m_DataParams[Card][Head].OriginalParameters.HeadType == OMNIDOT380)
                            m_SeparateRows = true;		// if 380 force single row select to true
                    }
                }
            for (DWORD Head = 0; Head < MAXHEADS; Head++)
            {
                for (DWORD Row = 0; Row < MAXROWS; Row++)
                {
                    if (sPrintParams.m_bSelectHead[Head][Row])
                    {
                        HeadSelected = true;
                        m_DataParams[Card][Head].OriginalParameters.HeadIndex[Row] = m_HeadsRequired[Card];		//set the head index for this head
                        if (sPrintParams.m_SeparateRows || (!sPrintParams.m_SeparateRows && (Row == 0 || (Row == 1 && !sPrintParams.m_bSelectHead[Head][0]))))
                            m_HeadsRequired[Card]++;
                        if (sPrintParams.m_PrintMode == FIXEDGAPCONT && m_DataParams[Card][Head].OriginalParameters.HeadType != NOTSET)
                            m_InterGap = qMin(m_InterGap, sPrintParams.m_uiProductOffset[0][Head][Row]);
                    }

                    m_DataParams[Card][Head].OriginalParameters.SubPixelDivide = sPrintParams.m_uiSubPixelDivide - 1;
                    if (sPrintParams.m_uiSubPixelDivide > 1)
                        m_WantSubDelay = true;
                }
            }
            if (!m_WantSubDelay)				// if we dont want sub pixel
                for (DWORD Head = 0; Head < MAXHEADS; Head++)
                    m_DataParams[Card][Head].OriginalParameters.SubPixelDivide = 0;	// clear the value so it's not used
        }
    }
    if (!HeadSelected)
        QMessageBox::warning(0,"Error","No head selected",QMessageBox::Ok);
        //DisplayErrorMessage(IDS_ERROR_NOHEADSSELECTED, MB_OK);
    return HeadSelected;
}

// ----------------------------------------------------------------------
//
// Sets up the USB controller
// Takes all the data entered and sends this down to the USB board.
// The image will have already been sent down as separate button.
// As the image is sent down as a separate button, we need to modifiy anything that is print mode
// specific here
//  m_IgnoreReversePD should be false if printing needs to ignore
//	direction when looking for reverse PD's
// ----------------------------------------------------------------------
BOOL InkjetMultiview::SetupPrintMode()
{

    BOOL SendOk = true;
    DWORD SequenceStart=0;

    //BeginWaitCursor();
    m_ActiveCards = 0;
    bXaarScorpionEnableIdleSpit(false);					// take out of idle spit mode as image loaded ready to print

    for (DWORD Card = 0; Card < (unsigned int)m_ActualXUSBsFound && SendOk; Card++)
    {
        CPrintParameters sPrintParams(this,Card, m_Key);
        if (Card == 0)
            SequenceStart = sPrintParams.m_SequenceStart;
        if (m_bSelectCard[Card])
        {
            while (bXaarScorpionXUSBBusy(Card))
                SleepEx(25, true);

            m_ActiveCards++;
            m_PrintMask[Card] = 0;

            if (((sPrintParams.m_PrintTransportMode == UNIPRINTWITHSPIT) ||
                ((sPrintParams.m_PrintTransportMode == UNIPRINT || sPrintParams.m_PrintTransportMode == BIPRINT)
                            && sPrintParams.m_IgnoreReversePD)) && // for bi-dir printing ignore PD on reverse pass
                            (sPrintParams.m_PrintMode == SINGLESHOT || sPrintParams.m_PrintMode == LABELWAIT))
                SendOk = bXaarScorpionPDNoReverse(Card, true);
            else
                SendOk = bXaarScorpionPDNoReverse(Card, false);

            if (SendOk)
                SendOk = bXaarScorpionSetDDFSValue(Card, sPrintParams.m_DDFSValue * (sPrintParams.m_ClockType == 1 ? 3:1) * sPrintParams.m_uiEncoderDivide);
            if (SendOk)
                SendOk = bXaarScorpionSetDDFSEnable(Card, sPrintParams.m_SysClock == DDFS ? 1:0 );	// select source of encoder
            if (SendOk)
                SendOk = bXaarScorpionSetPDInternal(Card, SequenceStart == 1 ?true:false);	// set PD source internal or external
            if (SendOk)
                SendOk = bXaarScorpionSetEncoderDivide(Card, sPrintParams.m_uiEncoderDivide - 1);
            if (SendOk)
                SendOk = bXaarScorpionEncoderPulseMultiply(Card, sPrintParams.m_uiEncoderMultiply > 0 ?sPrintParams.m_uiEncoderMultiply - 1:0);
            if (SendOk)
                SendOk = bXaarScorpionIgnorePD(Card, sPrintParams.m_PDDiscardCount);
            if (sPrintParams.m_SysClock == ABSOLUTESE)
            {
                AbsoluteEncoderValues AbsoluteValues;
                AbsoluteValues.EncoderCountPreloadValue = sPrintParams.m_PreloadValue;
                AbsoluteValues.ForwardPDTriggerValue = sPrintParams.m_ForwardPDTrigger;
                AbsoluteValues.InvertDirection = (BYTE)sPrintParams.m_Invert;
                AbsoluteValues.PreloadType = (BYTE)sPrintParams.m_ReloadOnExternalPD;
                AbsoluteValues.ReversePDTriggerValue = sPrintParams.m_ReversePDTrigger;
                AbsoluteValues.PDGeneration = (BYTE)sPrintParams.m_PDGeneration;

                if (SendOk)
                    SendOk = bXaarScorpionAbsoluteEncoderSetup(Card, AbsoluteValues);
            }
            if (SendOk)
                SendOk = bXaarScorpionAbsoluteEncoderEnable(Card, sPrintParams.m_SysClock == ABSOLUTESE ? true:false);
            for (DWORD Head = 0; Head < MAXHEADS; Head++)
            {
                if (m_DataParams[Card][Head].OriginalParameters.bSelectHead[0])				// if this head is required set up print mask
                    m_PrintMask[Card] |= (BIT0 << (Head * 2));					// set bits one row in print head
                if (m_DataParams[Card][Head].OriginalParameters.bSelectHead[1])
                    m_PrintMask[Card] |= (BIT1 << (Head * 2));				// if 2nd rows is selected print with row 2 as well
            }
            if (SendOk)
                SendOk = bXaarScorpionEnablePrintMode(Card, m_PrintMask[Card], true);		// now enter print mode for heads selected
        }
    }

    if (!SendOk)
        QMessageBox::warning(0,"Error","Error al imprimir",QMessageBox::Ok);
        //DisplayErrorMessage(IDS_ERROR_FAILEDTOSTARTPRINT, MB_OK);
    /*if (m_nTimer == 0 && SequenceStart == 1)							// test PD required
    {
        m_TimerCount = 0;
        m_Key->Setup(APPWRITE,_T("Configuration"));
        m_Key->QueryValueWithDefault(APPWRITE,_T("PDPrintCount"), m_SWTestCounts, 65535);
        m_SWTestCounts = m_SWTestCounts * m_ActiveCards * m_NumberSwathes;
        m_Key->QueryValueWithDefault(APPWRITE,_T("PDSWDelay"), Delay, 2000); // 2 second = 2000
        m_WantTestPD = true;
    }
    else
        m_WantTestPD = false;*/

    InTimerTick = false;
    //m_nTimer = (UINT)SetTimer(1, Delay, NULL);
    Sleep(1);
    //EndWaitCursor();

    return SendOk;
}

BOOL InkjetMultiview::SendImage(QString image)
{
    BOOL bLoadOk = true;

    m_inkjetImage=image;
    m_NumberSwathes = 0;
    m_AllSwathesFit = true;										// all images will fit entirely in memory
    m_SwatheFile = false;

    for (DWORD Card = 0; Card < (unsigned int)m_ActualXUSBsFound; Card++)
    {
        CPrintParameters sPrintParams(this,Card, m_Key);
        m_bSelectCard[Card] = sPrintParams.m_bSelectedCard;
        bXaarScorpionEnablePrintMode(Card,0xffff,false); //Cada vez qwue se envia una imagen, stop primero
    }


    //XUSBDownloadData();
    //CPrintParameters sPrintParams(this,0, m_Key);
    if (SetupParameters()){
        XUSBDownloadData();
        /*for (DWORD Card = 0; Card < (unsigned int)m_ActualXUSBsFound ; Card++){
            QThread* thread = new QThread;
            XaarWorker* worker = new XaarWorker();
            worker->moveToThread(thread);
            connect(worker, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
            connect(thread, SIGNAL(started()), worker, SLOT(process()));
            connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
            connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
            connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
            thread->start();
        }*/
    }
    if(SetupPrintMode()) QMessageBox::warning(0,"OK","Envío de imagen finalizado",QMessageBox::Ok);
    return bLoadOk;
}

/*Este solo de momento para ver si imprime...para aplicacion monohilo*/
void InkjetMultiview::XUSBDownloadData()
{
    DWORD XUSBId = 0;

    BOOL bLoadOk = true;
    DWORD NumberSwathes = 0;
    for (DWORD Head = 0; Head < MAXHEADS && bLoadOk; Head++)
    {
        for (DWORD Row = 0; Row < MAXROWS && bLoadOk; Row++)
        {
            bLoadOk = LoadActiveBitmapToCard(XUSBId, Head, Row);
            if (bLoadOk)
            {
                if (NumberSwathes == 0)				// only want to add swathe count once
                    NumberSwathes += m_DataParams[XUSBId][Head].OriginalParameters.NumberSwathes[Row];
                if (m_DataParams[XUSBId][Head].OriginalParameters.AllSwathesFit[Row] == false)
                    m_AllSwathesFit = false;
            }
        }
    }

    m_NumberSwathes = qMax(m_NumberSwathes, NumberSwathes);		// may need lock on this variable??


    if (m_AllSwathesFit)			// need to download more swathes when there is space
    {
        bool Exit = false;
//		XaarScorpionAllowLogging(true);

        //	need to see if there is room to download more data
            BOOL ReadOk = bXaarScorpionGetMemoryDataCard(XUSBId);			// read swathe buffer space
            for (DWORD Head = 0; Head < MAXHEADS && ReadOk && !Exit; Head++)
            {
                for (DWORD Row = 0; Row < MAXROWS && !Exit; Row++)
                {
                    if (m_DataParams[XUSBId][Head].OriginalParameters.bSelectHead[Row])		// if we want this head
                        SendSwathes(XUSBId, Head, Row, false);								// try to put data into the memory
                }
            }

//		XaarScorpionAllowLogging(false);
    }
}


/****************Implemento la clase que envia la info al xaar aqui mismo*******************************************/

XaarWorker::XaarWorker() {
}

XaarWorker::~XaarWorker() {
}

void XaarWorker::process() {

    emit finished();
}

