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
#include "InkjetXusbVarCtrl.h"
#include <QtGlobal>
#include <QFile>
#include <QString>
#include <QMessageBox>
#include <QImage>
#include <QThread>
#include "globals.h"

#define MAXHEADS 5
#define MAXROWS 2
//De momento, solo las swathes que dejo
#define MAXSWATHES SIZE_BUFFER_IMAGES

///Posiciones de memoria donde voy escribiendo las swathes...como mucho 1000 imagenes por XUSB
static DWORD PosMemSwathe[MAXHEADS][1000];
static DWORD PosSCB[MAXHEADS][1000];

#define DEFAULT_DROP 3

/** Crea el objeto para 1 tarjeta...le pasamos la referencia del Qstring*/
InkjetXusbVarCtrl::InkjetXusbVarCtrl(QObject *parent,int Card,const QString & qstrImage,int numImages) :
    QObject(parent)
{
    m_memoryMap=MemoryMap::request(this);
    m_threads=0;
    m_ShareSemaphore=0;
    m_ShareSemaphoreMIn=0;
    m_ActualXUSBsFound=0;
    m_Key.clear();
    m_uiMaxJets=0;
    m_NumberSwathes=0;
    m_HeadsRequired=0;
    m_InterGap=0;
    m_PrintMask=0;
    m_Select=0;
    memset(&m_LUTValue,0x00,sizeof(m_LUTValue));

    m_BackgroundValue=0;

    //class CXUSB_MultiApp *m_pApp;
    //class CMainFrame *m_pMainFrame;
    m_nTimer=0;
    m_SwatheTimer=0;
    m_TimerCount=0;
    m_SWTestCounts=0;
    m_ActiveCards=0;
    m_bSelectCard=false;
    m_WantTestPD=0;
    m_AllSwathesFit=0;
    m_WantSubDelay=0;
    m_SeparateRows=false;
    m_ErrorDiffusionEnabled=0;
    m_SwatheFile=0;
    m_WriteSwatheFile=0;
    InTimerTick=0;
    memset(&m_PreviousCurrentSwathe,0x00,sizeof(m_PreviousCurrentSwathe));


    m_ImagePtr=NULL;
    m_SwatheShiftWanted=false;
    m_PrintOnce=false;
    m_inkjetImage.clear();


    memset(m_DataParams,0x00,sizeof(m_DataParams));
    memset(&PosMemSwathe,0xff,sizeof(PosMemSwathe));
    memset(&PosSCB,0x00,sizeof(PosSCB));
    m_WriteSwatheFile=false;
    //Asignamos el USB
    m_thisXusb=Card;
    //Hacemos stop
    while (bXaarScorpionXUSBBusy(m_thisXusb)){
        SleepEx(25, true);
    }
    m_inkjetImage=qstrImage;
    m_InterGap=0;
    m_BufferSize=numImages;
    m_NumCabezales=/*this->returnNumCabezales();*/MAXHEADS;
    setNewPalette(DEFAULT_DROP);
    CPrintParameters sPrintParamsIni(this,m_thisXusb, m_Key);
    m_sPrintParams=sPrintParamsIni;
    m_CopyCount=1;
}
/** Cambia el fichero de configuración*/
void InkjetXusbVarCtrl::setNewConfigFile(QString filename){
    QFile file(filename);
    if (file.exists()){
        m_Key=filename;
        //CPrintParameters sPrintParamsIni(this,m_thisXusb, m_Key);
        m_sPrintParams.loadConfigFile(m_thisXusb, m_Key);

    }
    //else emit error(LOGGER_ERROR,QString("InkjetXusbVarCtrl() No existe:%1").arg(filename));
}


/** Volumen máximo a usar 0-7 (equivalente a 0-42 pl)*/
void InkjetXusbVarCtrl::setNewPalette(char maxVol){
    memset(&AuxPalette,maxVol,sizeof(AuxPalette));
    AuxPalette[6]=0;
    AuxPalette[7]=0;
}

/** Volumen máximo a usar 0-7 (equivalente a 0-42 pl)*/
void InkjetXusbVarCtrl::setNewFullPalette(char *listVol){
    memcpy(AuxPalette,listVol,8);
    //strncpy(AuxPalette,listVol,sizeof(listVol));
}


/** Establece el numero de imagenes*/
void InkjetXusbVarCtrl::setNumImages(int n){
    m_BufferSize=n;
}

/** Resetea el controlador*/
void InkjetXusbVarCtrl::Reset(){
    m_threads=0;
    m_ShareSemaphore=0;
    m_ShareSemaphoreMIn=0;
    m_ActualXUSBsFound=0;
    m_Key.clear();
    m_uiMaxJets=0;
    m_NumberSwathes=0;
    m_HeadsRequired=0;
    m_InterGap=0;
    m_PrintMask=0;
    m_Select=0;
    memset(&m_LUTValue,0x00,sizeof(m_LUTValue));

    m_BackgroundValue=0;

    //class CXUSB_MultiApp *m_pApp;
    //class CMainFrame *m_pMainFrame;
    m_nTimer=0;
    m_SwatheTimer=0;
    m_TimerCount=0;
    m_SWTestCounts=0;
    m_ActiveCards=0;
    m_bSelectCard=false;
    m_WantTestPD=0;
    m_AllSwathesFit=0;
    m_WantSubDelay=0;
    m_SeparateRows=false;
    m_ErrorDiffusionEnabled=0;
    m_SwatheFile=0;
    m_WriteSwatheFile=0;
    InTimerTick=0;
    memset(&m_PreviousCurrentSwathe,0x00,sizeof(m_PreviousCurrentSwathe));

    m_ImagePtr=NULL;
    m_SwatheShiftWanted=false;
    m_PrintOnce=false;
    m_inkjetImage.clear();


    memset(m_DataParams,0x00,sizeof(m_DataParams));
    memset(&PosMemSwathe,0xff,sizeof(PosMemSwathe));
    memset(&PosSCB,0x00,sizeof(PosSCB));
    m_WriteSwatheFile=false;
    //Hacemos stop
    while (bXaarScorpionXUSBBusy(m_thisXusb)){
        SleepEx(25, true);
    }
    m_InterGap=0;
    //m_BufferSize=numImages;
    setNewPalette(DEFAULT_DROP);
    CPrintParameters sPrintParamsIni(this,m_thisXusb, m_Key);
    m_sPrintParams=sPrintParamsIni;
    m_CopyCount=1;
    m_NumCabezales=this->returnNumCabezales();

    /*bXaarScorpionEnablePrintMode(0,0xFFFF,false);
    bXaarScorpionEnablePrintMode(0,0xFFFF,true);*/
}
/** Numero de cpias de cada imagen*/
void InkjetXusbVarCtrl::setCopyCount(DWORD copy){
    m_CopyCount=copy;
}

/** Carga la imagen a imprimir*/
void InkjetXusbVarCtrl::SetImage(const QImage & Image){
    m_inkjetImage.clear();
    m_ImageToPrint=Image;
}
/** Devuelve el numero de cabezales asociados*/
int InkjetXusbVarCtrl::returnNumCabezales(){
    int dummy=0;
    for (int x=0;x</*MAXHEADS*/m_NumCabezales;x++){
        if (bXaarScorpionGetHeadType(m_thisXusb,x)!=0) dummy++;
    }
    //emit error(QString("Width %1").arg(dummy),(int)m_thisXusb);
    return dummy;
}

/** Envia los swathes*/
BOOL InkjetXusbVarCtrl::SendSwathes(DWORD Head, DWORD Row, bool ForceImageLoad)
{
    BOOL DownLoadOk = true;
   // m_DataParams[Head].OriginalParameters.AllSwathesFit[Row]=false;
    // if this is the first time and we want to force swathe download or
    // it isnt the first time but all swathes didnt fit into the XUSB
    if (ForceImageLoad || !m_DataParams[Head].OriginalParameters.AllSwathesFit[Row])	// image wont all fit in one go
    {
        DWORD SwathesToStore = 0;		// clear the number of swathes to store
        // if first time then set the number of swathes to store to be the number
        // of swathes that will fit into the XUSB
        if (ForceImageLoad)
        {
            SwathesToStore = m_DataParams[Head].OriginalParameters.SwatheMemoryCount[Row];
            // clear PreviousPrintSwathe as there are none when we are on the first swathe
            m_DataParams[Head].OriginalParameters.PreviousPrintSwathe[Row] = 0;
        }
        else
        {
            // We have already downloaded some swathe data to the XUSB, we need to see if there is
            // room for any more
            m_DataParams[Head].OriginalParameters.NibbleIndex = Row;	// set the nibble index to the row of interest
            // Call XUSB function, this function will read the memory location of the XUSB and from
            // that information determine which swathe is currently being printed
            bXaarScorpionSwathesToStoreUpdated(m_thisXusb, Head, m_DataParams[Head], SwathesToStore);
        }

        // for all swathes that there is space to store
        for (DWORD Loop = 0; Loop < SwathesToStore && DownLoadOk; Loop++)
        {
            if (m_DataParams[Head].OriginalParameters.Spare[0] == 1)
                DownLoadOk = GenerateSwathePrintDataUpdated(Head, Row);
            else
            {
                if (m_DataParams[Head].OriginalParameters.Binary)		// binary swathe?
                    DownLoadOk = GenerateBinaryPrintData(Head, Row);	// generate binary swathe data
                else
                {
                    if (m_DataParams[Head].OriginalParameters.Enable2Bit) // 2 bit swathe
                        DownLoadOk = Generate2BitPrintData(Head, Row);	// generate 2 bit swathe data
                    else{
                        if (m_sPrintParams.m_ForceMode)DownLoadOk = GenerateBinaryPrintData(Head, Row);	// generate binary swathe data
                        else DownLoadOk = GenerateGSPrintData(Head, Row);		// generate grey scale swathe data
                    }


                }
            }
            m_DataParams[Head].OriginalParameters.StoredSwathes[Row]++;		// increase the number of swathes stored to the XUSB
            if (Row == 0 && !m_SeparateRows)
                m_DataParams[Head].OriginalParameters.StoredSwathes[1]++;		// if first row or dual rows head increase swathes on 2nd row
            if (Row == 0 && !m_SeparateRows){};
        }
    }
    return DownLoadOk;
}
/** Envia swathes*/
BOOL InkjetXusbVarCtrl::SendSwathes(DWORD Head, DWORD Row, DWORD SwathesToStore)
{
    BOOL DownLoadOk = true;

    // for all swathes that there is space to store
    for (DWORD Loop = 0; Loop < SwathesToStore && DownLoadOk; Loop++)
    {
        if (m_DataParams[Head].OriginalParameters.Spare[0] == 1)
            DownLoadOk = GenerateSwathePrintDataUpdated(Head, Row);
        else
        {
            if (m_DataParams[Head].OriginalParameters.Binary)		// binary swathe?
                DownLoadOk = GenerateBinaryPrintData(Head, Row);	// generate binary swathe data
            else
            {
                if (m_DataParams[Head].OriginalParameters.Enable2Bit) // 2 bit swathe
                    DownLoadOk = Generate2BitPrintData(Head, Row);	// generate 2 bit swathe data
                else
                    if (m_sPrintParams.m_ForceMode) DownLoadOk = GenerateBinaryPrintData(Head, Row);		// generate grey scale swathe data
                    else DownLoadOk = GenerateGSPrintData(Head, Row);		// generate grey scale swathe data
                    //DownLoadOk = GenerateBinaryPrintData(Head, Row);	// generate binary swathe data

            }
        }
        m_DataParams[Head].OriginalParameters.StoredSwathes[Row]++;		// increase the number of swathes stored to the XUSB
        if (m_DataParams[Head].OriginalParameters.ThisSwathe<(DWORD)m_BufferSize)
            m_DataParams[Head].OriginalParameters.ThisSwathe++;

        if (Row == 0 && !m_SeparateRows)
            m_DataParams[Head].OriginalParameters.StoredSwathes[1]++;		// if first row or dual rows head increase swathes on 2nd row
    }
    return DownLoadOk;
}


/**Carga los registros de fila*/
BOOL InkjetXusbVarCtrl::SetupPrintDataRegisterRows(DWORD Head, DWORD InRow, BYTE *abSourceStroke, bool SpitSwatheData)
{
    // MemoryBlock will be modified by the function that sends the swathe data so we need
    // to save the location that this swathe will be stored


    if (m_sPrintParams.m_ForceMode){
        m_sPrintParams.m_bBinaryPaletteRemap=true;
        m_sPrintParams.m_bPaletteRemap=true;
        m_DataParams[Head].OriginalParameters.Binary=true;
        m_DataParams[Head].OriginalParameters.BitDifference=1;
        m_DataParams[Head].OriginalParameters.bPaletteRemap=true;
        BYTE PaletteTable[2] = {0};
        PaletteTable[0] = 3;    // eg 3 or 6
        bXaarScorpionWriteRemapTable(m_thisXusb, Head, 1, PaletteTable);
    }


    DWORD StoreMemory = m_DataParams[Head].OriginalParameters.MemoryBlock[InRow];	// save pointer to where this image block is stored in memory


    BOOL DownLoadOk = true;

    m_DataParams[Head].OriginalParameters.NibbleIndex = InRow;					// set the index to the row required
    DownLoadOk = bXaarScorpionSetPrintDataUpdated(m_thisXusb, abSourceStroke, m_DataParams[Head]);

    // if this is a dual row head then set the memory block for the 2nd row to be the same as the 1st row
    if (!m_SeparateRows)
        m_DataParams[Head].OriginalParameters.MemoryBlock[1] =  m_DataParams[Head].OriginalParameters.MemoryBlock[0];

    // This is the location in the XUSB were the first swathe is stored - it is also useful for when the
    // image is larger than the space in the box and is re-used when the swathes "wrap around"
    DWORD SaveMemory = m_DataParams[Head].OriginalParameters.FirstMemoryBlock[InRow];

    // This is were the next swathe block would be stored in memory
    DWORD CurrentMemory = m_DataParams[Head].OriginalParameters.MemoryBlock[InRow];

    // for all rows on this head
    for (DWORD Row = 0; Row < m_DataParams[Head].OriginalParameters.NumberOfRows && DownLoadOk; Row++)
    {
        DWORD ActiveRow = Row;			// set the active row to the current row
        if (m_SeparateRows)				// if individual rows
            ActiveRow = InRow;			// set the active row to the row that was passed into the function

        m_DataParams[Head].OriginalParameters.NibbleIndex = ActiveRow;	// Select the row of interest

        // If uni-directional printing with spitting - this will not normally be true
        if (m_DataParams[Head].OriginalParameters.PrintTransportMode == UNIPRINTWITHSPIT)
            bXaarScorpionSetupSwatheSpitBlockParametersUpdated(m_DataParams[Head], SpitSwatheData);
        // normal function that is called - this sets up where the swathe control block is stored
        else{
            ///Txemari....ooojo!
            m_DataParams[Head].OriginalParameters.SwatheMemoryCount[ActiveRow]=m_BufferSize;
            bXaarScorpionSetupSwatheBlockParametersUpdated(m_DataParams[Head]);
            /*BOOL dummyLastSwathe=m_DataParams[Head].OriginalParameters.LastSwathe[InRow];
            BOOL dummyLastSwatheInMemory=m_DataParams[Head].OriginalParameters.LastSwatheInMemory[InRow];
            bXaarScorpionSetupSwatheBlockParametersUpdated(m_DataParams[Head]);
            m_DataParams[Head].OriginalParameters.LastSwathe[InRow]=dummyLastSwathe;
            m_DataParams[Head].OriginalParameters.LastSwatheInMemory[InRow]=dummyLastSwatheInMemory;*/
         }
        // If this is the last swathe in memory ie the next swathe wraps around
        // then we need to set savememory back to the start of the XUSB swathe memory
        if ((m_DataParams[Head].OriginalParameters.LastSwatheInMemory[ActiveRow])/*||(m_DataParams[Head].OriginalParameters.LastSwathe[ActiveRow])*/){
            SaveMemory = m_DataParams[Head].OriginalParameters.FirstMemoryBlock[ActiveRow];
        }// 1st memory block
        // otherwise savememory is where the next swathe data is stored
        else
            SaveMemory = m_DataParams[Head].OriginalParameters.MemoryBlock[ActiveRow];		// save next memory block

        // now set memory block to where this current swathe is stored so that the swathe control
        // block points to the swathe data that has just been stored
        m_DataParams[Head].OriginalParameters.MemoryBlock[ActiveRow] = StoreMemory;		// point to where this image is stored

        // if this row is selected for print
        if (m_DataParams[Head].OriginalParameters.bSelectHead[ActiveRow])
        {
            // setup the offset for this swathe depending on print direction
            m_DataParams[Head].OriginalParameters.ProductOffset = m_DataParams[Head].OriginalParameters.SaveProductOffset[m_DataParams[Head].OriginalParameters.ForwardBuffer == true ? 0:1][ActiveRow];

            // if sub pixel offset required set up the values - otherwise set it to 0
            if (m_WantSubDelay)
                m_DataParams[Head].OriginalParameters.SubPixelOffset = m_DataParams[Head].OriginalParameters.SaveSubPixelOffset[m_DataParams[Head].OriginalParameters.ForwardBuffer == true ? 0:1][ActiveRow];
            else
                m_DataParams[Head].OriginalParameters.SubPixelOffset = 0;		// disable subpixel

            // set up the dir block depending on if this is a forward pass or not
            if (m_DataParams[Head].OriginalParameters.ForwardBuffer)
                m_DataParams[Head].OriginalParameters.DirBlock = m_DataParams[Head].OriginalParameters.StartDir[ActiveRow];
            else		// if reverse ... we need to swap every other dir for bi directional printing
                m_DataParams[Head].OriginalParameters.DirBlock = (m_DataParams[Head].OriginalParameters.StartDir[ActiveRow] + m_DataParams[Head].OriginalParameters.ThisSwathe) % 2;

            // Log the data structure to disc
            //XaarScorpionAllowLogging(true);

            ///Txemari esto es lo que funciona
            //m_DataParams[Head].OriginalParameters.SwatheBlock=BLOCKSIZE*m_DataParams[Head].OriginalParameters.ThisSwathe+m_DataParams[Head].OriginalParameters.FirstSwatheBlock/*+ActiveRow*/;
            /*if ((m_DataParams[Head].OriginalParameters.LastSwatheInMemory[ActiveRow])||(m_DataParams[Head].OriginalParameters.LastSwathe[ActiveRow])){
                m_DataParams[Head].OriginalParameters.NextSwatheBlock=m_DataParams[Head].OriginalParameters.FirstSwatheBlock;
            }
            else
                m_DataParams[Head].OriginalParameters.NextSwatheBlock=m_DataParams[Head].OriginalParameters.SwatheBlock+BLOCKSIZE;
            */
            ///Hay que actualizar el numero de copias
            /*m_DataParams[Head].OriginalParameters.PrintOnce=false;
            m_DataParams[Head].CopyCount[0]=m_CopyCount;
            m_DataParams[Head].CopyCount[1]=m_CopyCount;*/
            DownLoadOk = bXaarScorpionSetPrintDataParametersUpdated(m_thisXusb,m_DataParams[Head]);
            //Guardamos la posicion de memoria de la swathe en curso
            if (ActiveRow==0){
                PosSCB[Head][m_DataParams[Head].OriginalParameters.ThisSwathe]=m_DataParams[Head].OriginalParameters.SwatheBlock;
                PosMemSwathe[Head][m_DataParams[Head].OriginalParameters.ThisSwathe]=m_DataParams[Head].OriginalParameters.MemoryBlock[ActiveRow];/*StoreMemory;*/
            }
        }
        m_DataParams[Head].OriginalParameters.MemoryBlock[ActiveRow] = CurrentMemory;	// restore this value
    }
     m_DataParams[Head].OriginalParameters.MemoryBlock[InRow] = SaveMemory;
    //Ultimo swathe, apunta otra vez al inicio
    /*if (m_DataParams[Head].OriginalParameters.LastSwathe[InRow]){
        m_DataParams[Head].OriginalParameters.MemoryBlock[InRow] = m_DataParams[Head].OriginalParameters.FirstMemoryBlock[InRow];	// now point to next block
        m_DataParams[Head].OriginalParameters.DirBlock = 0;
    }*/

    return DownLoadOk;
}

/**Carga los registros de fila*/
BOOL InkjetXusbVarCtrl::SetupPrintDataRegisterRows2(DWORD Head, DWORD InRow, BYTE *abSourceStroke, bool SpitSwatheData)
{
    // MemoryBlock will be modified by the function that sends the swathe data so we need
    // to save the location that this swathe will be stored
    DWORD StoreMemory = m_DataParams[Head].OriginalParameters.MemoryBlock[InRow];	// save pointer to where this image block is stored in memory


    BOOL DownLoadOk = true;

    m_DataParams[Head].OriginalParameters.NibbleIndex = InRow;					// set the index to the row required
    DownLoadOk = bXaarScorpionSetPrintDataUpdated(m_thisXusb, abSourceStroke, m_DataParams[Head]);

    // if this is a dual row head then set the memory block for the 2nd row to be the same as the 1st row
    if (!m_SeparateRows)
        m_DataParams[Head].OriginalParameters.MemoryBlock[1] =  m_DataParams[Head].OriginalParameters.MemoryBlock[0];

    // This is the location in the XUSB were the first swathe is stored - it is also useful for when the
    // image is larger than the space in the box and is re-used when the swathes "wrap around"
    DWORD SaveMemory = m_DataParams[Head].OriginalParameters.FirstMemoryBlock[InRow];

    // This is were the next swathe block would be stored in memory
    DWORD CurrentMemory = m_DataParams[Head].OriginalParameters.MemoryBlock[InRow];

    // for all rows on this head
    for (DWORD Row = 0; Row < m_DataParams[Head].OriginalParameters.NumberOfRows && DownLoadOk; Row++)
    {
        DWORD ActiveRow = Row;			// set the active row to the current row
        if (m_SeparateRows)				// if individual rows
            ActiveRow = InRow;			// set the active row to the row that was passed into the function

        m_DataParams[Head].OriginalParameters.NibbleIndex = ActiveRow;	// Select the row of interest

        // If uni-directional printing with spitting - this will not normally be true
        if (m_DataParams[Head].OriginalParameters.PrintTransportMode == UNIPRINTWITHSPIT)
            bXaarScorpionSetupSwatheSpitBlockParametersUpdated(m_DataParams[Head], SpitSwatheData);
        // normal function that is called - this sets up where the swathe control block is stored
        else
            bXaarScorpionSetupSwatheBlockParametersUpdated(m_DataParams[Head]);

        // If this is the last swathe in memory ie the next swathe wraps around
        // then we need to set savememory back to the start of the XUSB swathe memory
        if (m_DataParams[Head].OriginalParameters.LastSwatheInMemory[ActiveRow])
            SaveMemory = m_DataParams[Head].OriginalParameters.FirstMemoryBlock[ActiveRow];		// 1st memory block
        // otherwise savememory is where the next swathe data is stored
        else
            SaveMemory = m_DataParams[Head].OriginalParameters.MemoryBlock[ActiveRow];		// save next memory block

        // now set memory block to where this current swathe is stored so that the swathe control
        // block points to the swathe data that has just been stored
        m_DataParams[Head].OriginalParameters.MemoryBlock[ActiveRow] = StoreMemory;		// point to where this image is stored

        // if this row is selected for print
        if (m_DataParams[Head].OriginalParameters.bSelectHead[ActiveRow])
        {
            // setup the offset for this swathe depending on print direction
            m_DataParams[Head].OriginalParameters.ProductOffset = m_DataParams[Head].OriginalParameters.SaveProductOffset[m_DataParams[Head].OriginalParameters.ForwardBuffer == true ? 0:1][ActiveRow];

            // if sub pixel offset required set up the values - otherwise set it to 0
            if (m_WantSubDelay)
                m_DataParams[Head].OriginalParameters.SubPixelOffset = m_DataParams[Head].OriginalParameters.SaveSubPixelOffset[m_DataParams[Head].OriginalParameters.ForwardBuffer == true ? 0:1][ActiveRow];
            else
                m_DataParams[Head].OriginalParameters.SubPixelOffset = 0;		// disable subpixel

            // set up the dir block depending on if this is a forward pass or not
            if (m_DataParams[Head].OriginalParameters.ForwardBuffer)
                m_DataParams[Head].OriginalParameters.DirBlock = m_DataParams[Head].OriginalParameters.StartDir[ActiveRow];
            else		// if reverse ... we need to swap every other dir for bi directional printing
                m_DataParams[Head].OriginalParameters.DirBlock = (m_DataParams[Head].OriginalParameters.StartDir[ActiveRow] + m_DataParams[Head].OriginalParameters.ThisSwathe) % 2;

            // Log the data structure to disc
            //XaarScorpionAllowLogging(true);
            /*m_DataParams[Head].OriginalParameters.SwatheBlock=BLOCKSIZE*m_DataParams[Head].OriginalParameters.ThisSwathe+m_DataParams[Head].OriginalParameters.FirstSwatheBlock;
            if ((m_DataParams[Head].OriginalParameters.LastSwatheInMemory[ActiveRow])||(m_DataParams[Head].OriginalParameters.LastSwathe[ActiveRow]))
                m_DataParams[Head].OriginalParameters.NextSwatheBlock=m_DataParams[Head].OriginalParameters.FirstSwatheBlock;
            else
                m_DataParams[Head].OriginalParameters.NextSwatheBlock=m_DataParams[Head].OriginalParameters.SwatheBlock+BLOCKSIZE;
            */

            XaarScorpionLogPrintStructInfoUpdated(m_DataParams[Head]);
            // Call the XUSB DLL function to setup and send the XUSB swathe control blocks
           // emit error(LOGGER_WARNING,QString("%1 %2 %3").arg(m_thisXusb).arg(Head).arg(m_DataParams[Head].OriginalParameters.SwatheBlock));

            DownLoadOk = bXaarScorpionSetPrintDataParametersUpdated(m_thisXusb,m_DataParams[Head]);
        }
        m_DataParams[Head].OriginalParameters.MemoryBlock[ActiveRow] = CurrentMemory;	// restore this value
    }
    m_DataParams[Head].OriginalParameters.MemoryBlock[InRow] = SaveMemory;	// now point to next block

    return DownLoadOk;
}

/** Carga parametros comunes para GS y Binary data*/
BOOL InkjetXusbVarCtrl::SetupCommonDataParameters(DWORD Head, DWORD Row, CPrintParameters sPrintParams,int cyDIB, LPSTR lpDIBBits, bool SwatheFile)
{
    BOOL Success = true;
    // Setup the pointer to the image data .... this is only for reference and not used by the dll
    m_DataParams[Head].OriginalParameters.lpDIBBits = lpDIBBits;

    if (!SwatheFile)							// if data is read in from swathefile
        m_DataParams[Head].OriginalParameters.TotalImageWidth[Row] = cyDIB;

    if (Success){								// call function in dll to setup structure based on registry
            Success=bXaarScorpionSetupImageDataParametersUpdated(m_thisXusb, Head, Row, m_DataParams[Head], m_HeadsRequired);
    }
    else
        m_DataParams[Head].OriginalParameters.NumberSwathes[0] = m_DataParams[Head].OriginalParameters.NumberSwathes[1] = 0;

  /*  m_DataParams[Head].OriginalParameters.NumberSwathes[Row]=8;
    m_DataParams[Head].OriginalParameters.LastSwathe[Row]=false;
    m_DataParams[Head].OriginalParameters.LastSwatheInMemory[Row]=false;
    m_DataParams[Head].OriginalParameters.PrintOnce=false;*/
    //m_DataParams[Head].OriginalParameters.NumberSwathes[Row]=2;

    return Success;
}

/** Carga los parametros que se envian al XUSB*/
BOOL InkjetXusbVarCtrl::PrintBinaryBMP(DWORD Head, DWORD Row, CPrintParameters sPrintParams, int cyDIB, LPSTR lpDIBBits)
{
    SetupCommonDataParameters(Head, Row, /*sPrintParams*/m_sPrintParams, cyDIB, lpDIBBits, false);
    if (m_DataParams[Head].OriginalParameters.NumberSwathes[Row] == 0)
        return false;

    return SendSwathes(Head, Row, /*true*/DWORD(1));
}

/** Genera los datos y se envian a los heads*/
BOOL InkjetXusbVarCtrl::GenerateBinaryPrintData(DWORD Head, DWORD Row)
{

    DWORD SourceLine;
    BYTE *abSourceStroke = new BYTE [m_DataParams[Head].OriginalParameters.ImageSize[Row]];
    UINT uiSourceDifference = 1;				// number of bits to store source data

    // Now work out which swathe we should be printing
    DWORD NumSwathe = m_DataParams[Head].OriginalParameters.StoredSwathes[Row] % m_DataParams[Head].OriginalParameters.NumberSwathes[Row];
    DWORD Swathe = m_DataParams[Head].OriginalParameters.bReverseSwatheOrder ? (m_DataParams[Head].OriginalParameters.NumberSwathes[Row] - NumSwathe - 1):NumSwathe;

    if ((m_DataParams[Head].OriginalParameters.PrintTransportMode == BIPRINT || m_DataParams[Head].OriginalParameters.PrintTransportMode == UNIBIPRINT) && (NumSwathe % 2 == 1))	// bi direction scanning and on reverse pass
        m_DataParams[Head].OriginalParameters.ForwardBuffer = false;
    else
        m_DataParams[Head].OriginalParameters.ForwardBuffer = true;
    bool ForwardBuffer = m_DataParams[Head].OriginalParameters.ForwardBuffer;		// need to adjust order of filling image buffer
    if (m_DataParams[Head].OriginalParameters.bReverseImageOrder)
        ForwardBuffer = !ForwardBuffer;


    DWORD BaseSourceByteOffset = (m_DataParams[Head].OriginalParameters.SwatheStartIndex + Swathe * m_DataParams[Head].OriginalParameters.SwatheIncrement) / (8 / uiSourceDifference); // point to correct swathe start

    DWORD ImageWidth = qMin(m_DataParams[Head].OriginalParameters.TotalImageWidth[Row] - (m_DataParams[Head].OriginalParameters.SwatheStartIndex + Swathe * m_DataParams[Head].OriginalParameters.SwatheIncrement), m_DataParams[Head].OriginalParameters.HeadChannels);	// number of jets in chip bank
    UINT uiLineOffset = WIDTHBYTES(m_DataParams[Head].OriginalParameters.TotalImageWidth[Row] * uiSourceDifference);

    DWORD ShiftCount = (uiSourceDifference * Swathe * m_DataParams[Head].OriginalParameters.SwatheIncrement) % (8 / uiSourceDifference);
    BYTE bSourceMaskStart = START_MASK >> ShiftCount;						// binary 1 (bit per pixel)

    BYTE bBaseTargetMask, bBaseStartMask;
    DWORD TargetBaseStart = 0;

    if (m_DataParams[Head].OriginalParameters.BitDifference == 1)			// if  8 bits per byte we have to deal with leading + trailing
    {
        bBaseTargetMask = 1;		// << (m_DataParams[Card][Head].LeadChannels % 8);
        bBaseStartMask = 1;
        TargetBaseStart = 0;		//m_DataParams[Card][Head].LeadChannels / 8;
    }
    else
    {
        bBaseStartMask = bBaseTargetMask = START_NIBBLE_MASK >> 4;
    }

    memset(abSourceStroke, 0, m_DataParams[Head].OriginalParameters.ImageSize[Row]);


    for (DWORD iOuterLoop = 0; iOuterLoop < m_DataParams[Head].OriginalParameters.ImageLength[Row] ; iOuterLoop++)
    {
        if (ForwardBuffer)
            SourceLine = m_DataParams[Head].OriginalParameters.ImageLength[Row] - iOuterLoop - 1;
        else
            SourceLine = iOuterLoop;

        BYTE bSourceMask = bSourceMaskStart;
        BYTE bTargetMask = bBaseTargetMask;
        DWORD TargetRowOffset = m_DataParams[Head].OriginalParameters.SourceStrokeWidth * iOuterLoop;
        DWORD TargetByte = TargetBaseStart;			//0;
        DWORD SourceByteOffset = BaseSourceByteOffset;
        for (DWORD uiInnerLoop = 0; uiInnerLoop < ImageWidth; uiInnerLoop++)
        {
            BYTE Data = m_DataParams[Head].OriginalParameters.lpDIBBits[uiLineOffset * SourceLine + SourceByteOffset];


            if (m_DataParams[Head].OriginalParameters.bPaletteRemap)			// palette remap required
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
            bTargetMask <<= m_DataParams[Head].OriginalParameters.BitDifference;			// for nibble storage
            if (bTargetMask == 0)
            {
                bTargetMask = bBaseStartMask;
                TargetByte++;
            }
        }
        if (m_DataParams[Head].OriginalParameters.bBinaryBackgroundInvert)			// now clear the space to background if invert required
            for (DWORD uiInnerLoop = ImageWidth; uiInnerLoop < m_DataParams[Head].OriginalParameters.HeadChannels; uiInnerLoop++)
            {
                abSourceStroke[TargetRowOffset + TargetByte] |= bTargetMask;
                bTargetMask <<= m_DataParams[Head].OriginalParameters.BitDifference;
                if (bTargetMask == 0)
                {
                    bTargetMask = bBaseStartMask;
                    TargetByte++;
                }
            }
    }
    BOOL DownLoadOk = true;
    DownLoadOk = SetupPrintDataRegisterRows(Head, Row, abSourceStroke, false);
    delete [] abSourceStroke;
    return DownLoadOk;
}

/** Print256GreyScaleBMP
    This sets up the parameters in the structure, the parameters are
    used when the data and the registers are sent to the scorpion board
    The LUT for image data has a range of values centered round each source point
    ie 6 dpd would require 51 @ values
    The interval (ie number of values to divide total range by) is centered so the
    1st and last values share the interval
    26 51 51 51 51 26	How many
    0  1  2  3  4  5		LUT value
**/
BOOL InkjetXusbVarCtrl::Print256GreyScaleBMP(DWORD Head, DWORD Row, CPrintParameters sPrintParams, int cyDIB, LPSTR lpDIBBits)
{
    //Es la primera swathe...
    if (m_DataParams[Head].OriginalParameters.ImageSize[Row]==0){
        SetupCommonDataParameters(Head, 0, sPrintParams, cyDIB, lpDIBBits, false);
        SetupCommonDataParameters(Head, 1, sPrintParams, cyDIB, lpDIBBits, false);
     }
    else (m_DataParams[Head].OriginalParameters.NumberSwathes[Row]=1);
    if (m_DataParams[Head].OriginalParameters.NumberSwathes[Row] == 0){
        emit error("Print256GS error",m_thisXusb);
        return false;
    }
    //m_DataParams[Head].OriginalParameters.BufferReverse[1]=m_DataParams[Head].OriginalParameters.BufferReverse[0];
    DWORD Interval = 255 / (sPrintParams.m_uiGreyLevels - 1);		// determine number of values at in each interval
    DWORD Index = 0;
    BYTE Value;
    DWORD LoopCount = Interval / 2;									// the interval has 1/2 the values at the start and the rest at the end
    if (sPrintParams.m_bEnable2Bit)
    {
        /*m_BackgroundValue = (BYTE)(sPrintParams.m_GSBackgroundValue + (sPrintParams.m_GSBackgroundValue << 2) +
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
        }*/
        m_BackgroundValue = (BYTE)(sPrintParams.m_GSBackgroundValue + (sPrintParams.m_GSBackgroundValue << 2) +
                                    (sPrintParams.m_GSBackgroundValue << 4) + (sPrintParams.m_GSBackgroundValue << 6));
        for (DWORD Grey = 0; Grey < sPrintParams.m_uiGreyLevels; Grey++)
        {
            if (sPrintParams.m_bPaletteRemap){
                //Modificamos valor de gota para todos
                Value =  (BYTE)(AuxPalette[Grey] + (AuxPalette[Grey] << 2) +
                                                            (AuxPalette[Grey] << 4) + (AuxPalette[Grey] << 6));
            }
            else
                Value = (BYTE)(Grey + (Grey << 2) + (Grey << 4) + (Grey << 6));		// we now setup the values as defined by user
            for (DWORD Loop = 0; Loop < LoopCount && Index < sizeof(m_LUTValue); Loop++)
                m_LUTValue[Index++] = Value;
            LoopCount = Interval;
        }

    }
    else
    {
        /*for (DWORD Grey = 0; Grey < sPrintParams.m_uiGreyLevels; Grey++)
        {
            if (sPrintParams.m_bPaletteRemap)
                Value = (BYTE)(sPrintParams.m_PaletteValue[Grey] + (sPrintParams.m_PaletteValue[Grey] << 4));
            else
                Value = (BYTE)(Grey + (Grey << 4));		// we now setup the values as defined by user
            for (DWORD Loop = 0; Loop < LoopCount && Index < sizeof(m_LUTValue); Loop++)
                m_LUTValue[Index++] = Value;
            LoopCount = Interval;
        }
        m_BackgroundValue = (BYTE)(sPrintParams.m_GSBackgroundValue + (sPrintParams.m_GSBackgroundValue << 4));*/

        for (DWORD Grey = 0; Grey < sPrintParams.m_uiGreyLevels; Grey++)
        {
            if (sPrintParams.m_bPaletteRemap){
                /*if (m_thisXusb==0)  Value = (BYTE)(sPrintParams.m_PaletteValue[Grey] + (sPrintParams.m_PaletteValue[Grey] << 4));
                else Value = (BYTE)(AuxPalette[Grey] + (AuxPalette[Grey] << 4));*/
                //Modificamos valor de gota para todos
                Value = (BYTE)(AuxPalette[Grey] + (AuxPalette[Grey] << 4));
                //Value = (BYTE)(sPrintParams.m_PaletteValue[Grey] + (sPrintParams.m_PaletteValue[Grey] << 4));
            }
            else
                Value = (BYTE)(Grey + (Grey << 4));		// we now setup the values as defined by user
            for (DWORD Loop = 0; Loop < LoopCount && Index < sizeof(m_LUTValue); Loop++)
                m_LUTValue[Index++] = Value;
            LoopCount = Interval;
        }
        m_BackgroundValue = (BYTE)(sPrintParams.m_GSBackgroundValue + (sPrintParams.m_GSBackgroundValue << 4));
    }
    return SendSwathes(Head, Row,(DWORD)1);
}

/** GenerateGSPrintData
The BMP data is stored in scan line format
The image is packed in memory with 2 channels per byte for greyscale format
Palette remap may be setup and a software palette remap applied to the print data
This code is used when a Scorpion 760 GS print head is connected, and two rows of print data is setup
The data is set into D0..D2 normally for the printhead
The data is setup for all printing channels of the head and the data and parameters
 sent to the scorpion board.
**/
BOOL InkjetXusbVarCtrl::GenerateGSPrintData(DWORD Head, DWORD Row)
{

    DWORD SourceLine;
    BYTE *abSourceStroke = new BYTE [m_DataParams[Head].OriginalParameters.ImageSize[Row]];

    DWORD NumSwathe = m_DataParams[Head].OriginalParameters.StoredSwathes[Row] % m_DataParams[Head].OriginalParameters.NumberSwathes[Row];
    DWORD Swathe = m_DataParams[Head].OriginalParameters.bReverseSwatheOrder ? (m_DataParams[Head].OriginalParameters.NumberSwathes[Row] - NumSwathe - 1):NumSwathe;

    if ((m_DataParams[Head].OriginalParameters.PrintTransportMode == BIPRINT || m_DataParams[Head].OriginalParameters.PrintTransportMode == UNIBIPRINT) && (NumSwathe % 2 == 1))	// bi direction scanning and on reverse pass
        m_DataParams[Head].OriginalParameters.ForwardBuffer = false;
    else
        m_DataParams[Head].OriginalParameters.ForwardBuffer = true;

    bool ForwardBuffer = m_DataParams[Head].OriginalParameters.ForwardBuffer;		// need to adjust order of filling image buffer
    if (m_DataParams[Head].OriginalParameters.bReverseImageOrder)
        ForwardBuffer = !ForwardBuffer;

    DWORD BaseSourceByteOffset = m_DataParams[Head].OriginalParameters.SwatheStartIndex + Swathe * m_DataParams[Head].OriginalParameters.SwatheIncrement; // point to correct swathe start
    if (m_SwatheShiftWanted && m_DataParams[Head].OriginalParameters.bReverseSwatheOrder && BaseSourceByteOffset > 0 &&
        (m_DataParams[Head].OriginalParameters.TotalImageWidth[Row] % m_DataParams[Head].OriginalParameters.HeadChannels) != 0)
        BaseSourceByteOffset -= (m_DataParams[Head].OriginalParameters.HeadChannels - (m_DataParams[Head].OriginalParameters.TotalImageWidth[Row] % m_DataParams[Head].OriginalParameters.HeadChannels)); // offset for reverse

    //BaseSourceByteOffset=Head*1000;
    //m_DataParams[Head].OriginalParameters.ImageSize[Row];
    DWORD ImageWidth;
    if (m_SwatheShiftWanted && m_DataParams[Head].OriginalParameters.bReverseSwatheOrder && NumSwathe + 1 == m_DataParams[Head].OriginalParameters.NumberSwathes[Row] &&
        (m_DataParams[Head].OriginalParameters.TotalImageWidth[Row] % m_DataParams[Head].OriginalParameters.HeadChannels) != 0)
        ImageWidth = m_DataParams[Head].OriginalParameters.TotalImageWidth[Row] % m_DataParams[Head].OriginalParameters.HeadChannels;
    else
    {
        if (m_DataParams[Head].OriginalParameters.TotalImageWidth[Row] > BaseSourceByteOffset)
            ImageWidth = qMin(m_DataParams[Head].OriginalParameters.TotalImageWidth[Row] - BaseSourceByteOffset, m_DataParams[Head].OriginalParameters.HeadChannels);	// number of jets in chip bank
        else
            ImageWidth = 0;							// 0 jets to print
    }
    UINT uiLineOffset = WIDTHBYTES(m_DataParams[Head].OriginalParameters.TotalImageWidth[Row] * 8);

    DWORD StartClear = ImageWidth;
    DWORD EndClear = m_DataParams[Head].OriginalParameters.HeadChannels;
    BYTE bTargetMask[2];
    DWORD ShiftValue = 0;
    if (m_SwatheShiftWanted)
        ShiftValue = PrintJets[m_DataParams[Head].OriginalParameters.HeadType] - ImageWidth;

    DWORD TargetByteOffset = ShiftValue / 2;

    if (m_SwatheShiftWanted)
    {
        StartClear = 0;
        EndClear = TargetByteOffset;
    }

    if (m_SeparateRows || (!m_SeparateRows && m_DataParams[Head].OriginalParameters.BufferReverse[Row]))
    {
        bTargetMask[0] = START_NIBBLE_MASK >> (m_DataParams[Head].OriginalParameters.BitDifference * ((1 + ShiftValue) % 2));
        bTargetMask[1] = START_NIBBLE_MASK >> (m_DataParams[Head].OriginalParameters.BitDifference * (ShiftValue % 2));
    }
    else
    {
        bTargetMask[0] = START_NIBBLE_MASK >> (m_DataParams[Head].OriginalParameters.BitDifference * (ShiftValue % 2));
        bTargetMask[1] = START_NIBBLE_MASK >> (m_DataParams[Head].OriginalParameters.BitDifference * ((1 + ShiftValue) % 2));
    }

    memset(abSourceStroke, 0, m_DataParams[Head].OriginalParameters.ImageSize[Row]);

/*    QImage imagen("D:\\Data\\INKJET\\Images\\texto150.bmp");
    QImage imagen2=imagen.mirrored(false,true);*/
    //DWORD iOuterLoop2=0;
    for (DWORD iOuterLoop = 0; iOuterLoop < m_DataParams[Head].OriginalParameters.ImageLength[Row] ; iOuterLoop++)
    {
        /*if (iOuterLoop==m_DataParams[Head].OriginalParameters.ImageLength[Row]/2) {
            m_DataParams[Head].OriginalParameters.lpDIBBits=(LPSTR)imagen2.bits();
            iOuterLoop2=0;
        }*/

        if (ForwardBuffer)
            SourceLine = m_DataParams[Head].OriginalParameters.ImageLength[Row] - iOuterLoop - 1;
        else
            SourceLine = iOuterLoop;
        BYTE MaskNibble = 0;
        DWORD TargetByte = m_DataParams[Head].OriginalParameters.SourceStrokeWidth * iOuterLoop + TargetByteOffset;
        DWORD SourceByteOffset = BaseSourceByteOffset;

        for (DWORD uiInnerLoop = 0; uiInnerLoop < ImageWidth;uiInnerLoop++)
        {
            abSourceStroke[TargetByte] |= (bTargetMask[MaskNibble] & m_LUTValue[(BYTE)m_DataParams[Head].OriginalParameters.lpDIBBits[uiLineOffset * SourceLine + uiInnerLoop + SourceByteOffset]]);
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
        //iOuterLoop2++;
    }
    //Txemari...no lo usamos?
    //gThisValue++;
    BOOL DownLoadOk = true;
    DownLoadOk = SetupPrintDataRegisterRows(Head, Row, abSourceStroke, false);


    return DownLoadOk;
}







/** Generate2BitPrintData
The BMP data is stored in scan line format
The image is packed in memory with 2 channels per byte for greyscale format
Palette remap may be setup and a software palette remap applied to the print data
This code is used when a Scorpion 760 GS print head is connected, and two rows of print data is setup
The data is set into D0..D2 normally for the printhead
The data is setup for all printing channels of the head and the data and parameters
sent to the scorpion board.
**/
BOOL InkjetXusbVarCtrl::Generate2BitPrintData(DWORD Head, DWORD Row)
{

    DWORD SourceLine;
    BYTE *abSourceStroke = new BYTE [m_DataParams[Head].OriginalParameters.ImageSize[Row]];

    DWORD NumSwathe = m_DataParams[Head].OriginalParameters.StoredSwathes[Row] % m_DataParams[Head].OriginalParameters.NumberSwathes[Row];
    DWORD Swathe = m_DataParams[Head].OriginalParameters.bReverseSwatheOrder ? (m_DataParams[Head].OriginalParameters.NumberSwathes[Row] - NumSwathe - 1):NumSwathe;

    if ((m_DataParams[Head].OriginalParameters.PrintTransportMode == BIPRINT || m_DataParams[Head].OriginalParameters.PrintTransportMode == UNIBIPRINT) && (NumSwathe % 2 == 1))	// bi direction scanning and on reverse pass
        m_DataParams[Head].OriginalParameters.ForwardBuffer = false;
    else
        m_DataParams[Head].OriginalParameters.ForwardBuffer = true;

    bool ForwardBuffer = m_DataParams[Head].OriginalParameters.ForwardBuffer;		// need to adjust order of filling image buffer
    if (m_DataParams[Head].OriginalParameters.bReverseImageOrder)
        ForwardBuffer = !ForwardBuffer;

    DWORD BaseSourceByteOffset = m_DataParams[Head].OriginalParameters.SwatheStartIndex + Swathe * m_DataParams[Head].OriginalParameters.SwatheIncrement; // point to correct swathe start
    DWORD ImageWidth;
    if (m_DataParams[Head].OriginalParameters.TotalImageWidth[Row] > m_DataParams[Head].OriginalParameters.SwatheStartIndex + Swathe * m_DataParams[Head].OriginalParameters.SwatheIncrement)
        ImageWidth = qMin(m_DataParams[Head].OriginalParameters.TotalImageWidth[Row] - (m_DataParams[Head].OriginalParameters.SwatheStartIndex + Swathe * m_DataParams[Head].OriginalParameters.SwatheIncrement), m_DataParams[Head].OriginalParameters.HeadChannels);	// number of jets in chip bank
    else
        ImageWidth = 0;							// 0 jets to print
    UINT uiLineOffset = WIDTHBYTES(m_DataParams[Head].OriginalParameters.TotalImageWidth[Row] * 8);

    BYTE bTargetMask[4];
    if (m_SeparateRows || (!m_SeparateRows && m_DataParams[Head].OriginalParameters.BufferReverse[Row]))
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

    memset(abSourceStroke, 0, m_DataParams[Head].OriginalParameters.ImageSize[Row]);

    for (DWORD iOuterLoop = 0; iOuterLoop < m_DataParams[Head].OriginalParameters.ImageLength[Row] ; iOuterLoop++)
    {
        if (ForwardBuffer)
            SourceLine = m_DataParams[Head].OriginalParameters.ImageLength[Row] - iOuterLoop - 1;
        else
            SourceLine = iOuterLoop;
        BYTE MaskNibble = 0;
        DWORD TargetByte = m_DataParams[Head].OriginalParameters.SourceStrokeWidth * iOuterLoop;
        DWORD SourceByteOffset = BaseSourceByteOffset;

        for (DWORD uiInnerLoop = 0; uiInnerLoop < ImageWidth;uiInnerLoop++)
        {
            abSourceStroke[TargetByte] |= (bTargetMask[MaskNibble] & m_LUTValue[(BYTE)m_DataParams[Head].OriginalParameters.lpDIBBits[uiLineOffset * SourceLine + uiInnerLoop + SourceByteOffset]]);
            MaskNibble = (MaskNibble + 1) % 4;
            if (MaskNibble == 0)
                TargetByte++;
        }

        // now clear the space to background
        for (DWORD uiInnerLoop = ImageWidth; uiInnerLoop < m_DataParams[Head].OriginalParameters.HeadChannels; uiInnerLoop++)
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
    DownLoadOk = SetupPrintDataRegisterRows(Head, Row, abSourceStroke, false);
    delete [] abSourceStroke;
    return DownLoadOk;
}
/** Procesa el swathe file*/
 BOOL InkjetXusbVarCtrl::ProcessSwatheFile(DWORD Head, DWORD Row, CPrintParameters sPrintParams, int cyDIB, LPSTR lpDIBBits, int SwatheCount)
 {
    m_DataParams[Head].OriginalParameters.TotalImageWidth[0] = SwatheCount * (PrintJets[m_DataParams[Head].OriginalParameters.HeadType] /
        (sPrintParams.m_SeparateRows ? NUMBEROFROWS[m_DataParams[Head].OriginalParameters.HeadType]:1));
    m_DataParams[Head].OriginalParameters.TotalImageWidth[1] = m_DataParams[Head].OriginalParameters.TotalImageWidth[0];
    SetupCommonDataParameters(Head, Row, sPrintParams, cyDIB, lpDIBBits, true);
    if (m_DataParams[Head].OriginalParameters.NumberSwathes[Row] == 0)
        return false;

    m_DataParams[Head].OriginalParameters.Spare[0] = 1;
    return SendSwathes(Head, Row, true);
 }

/** GenerateSwathePrintData
The Data is read in from the file in the required format
At the start of the file are 3 DWORDS, these contain the
SwatheWidth, SwatheLength and SwatheCount
The file that contains the swathedata end on end
**/
BOOL InkjetXusbVarCtrl::GenerateSwathePrintData(DWORD Head, DWORD Row)
{
    BOOL DownLoadOk = false;
    //BYTE *abSourceStroke = new BYTE [m_DataParams[Card][Head].OriginalParameters.ImageSize[Row]];

    DWORD NumSwathe = m_DataParams[Head].OriginalParameters.StoredSwathes[Row] % m_DataParams[Head].OriginalParameters.NumberSwathes[Row];
    DWORD Swathe = m_DataParams[Head].OriginalParameters.bReverseSwatheOrder ? (m_DataParams[Head].OriginalParameters.NumberSwathes[Row] - NumSwathe - 1):NumSwathe;

    if ((m_DataParams[Head].OriginalParameters.PrintTransportMode == BIPRINT || m_DataParams[Head].OriginalParameters.PrintTransportMode == UNIBIPRINT) && (NumSwathe % 2 == 1))	// bi direction scanning and on reverse pass
        m_DataParams[Head].OriginalParameters.ForwardBuffer = false;
    else
        m_DataParams[Head].OriginalParameters.ForwardBuffer = true;

    long SeekPosition  = 3 * sizeof(DWORD) + Swathe * m_DataParams[Head].OriginalParameters.ImageSize[Row];

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

    QFile FileStream(m_DataParams[Head].OriginalParameters.lpDIBBits);
    unsigned char *abSourceStroke= new unsigned char[m_DataParams[Head].OriginalParameters.ImageSize[Row]];
    if (FileStream.open(QIODevice::ReadOnly)){
        FileStream.seek(SeekPosition);
        DWORD ReadCount=(DWORD)FileStream.read((char*)abSourceStroke,m_DataParams[Head].OriginalParameters.ImageSize[Row]);
        FileStream.close();
        if (ReadCount == m_DataParams[Head].OriginalParameters.ImageSize[Row])
             DownLoadOk = SetupPrintDataRegisterRows(Head, Row, abSourceStroke, false);
    }
    delete (abSourceStroke);
    return DownLoadOk;
}

/** GenerateSwathePrintDataUpdated
The Data is read in from the file in the required format
At the start of the file are 4 DWORDS, these contain the
SwatheWidth, SwatheLength, SwatheCount and BitCount
The file that contains the swathedata end on end
**/
BOOL InkjetXusbVarCtrl::GenerateSwathePrintDataUpdated(DWORD Head, DWORD Row)
{
    BOOL DownLoadOk = false;
    //BYTE *abSourceStroke = new BYTE [m_DataParams[Card][Head].OriginalParameters.ImageSize[Row]];

    DWORD NumSwathe = m_DataParams[Head].OriginalParameters.StoredSwathes[Row] % m_DataParams[Head].OriginalParameters.NumberSwathes[Row];
    DWORD Swathe = m_DataParams[Head].OriginalParameters.bReverseSwatheOrder ? (m_DataParams[Head].OriginalParameters.NumberSwathes[Row] - NumSwathe - 1):NumSwathe;

    if ((m_DataParams[Head].OriginalParameters.PrintTransportMode == BIPRINT || m_DataParams[Head].OriginalParameters.PrintTransportMode == UNIBIPRINT) && (NumSwathe % 2 == 1))	// bi direction scanning and on reverse pass
        m_DataParams[Head].OriginalParameters.ForwardBuffer = false;
    else
        m_DataParams[Head].OriginalParameters.ForwardBuffer = true;

    long SeekPosition  = 4 * sizeof(DWORD) + Swathe * m_DataParams[Head].OriginalParameters.ImageSize[Row];

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

    QFile FileStream(m_DataParams[Head].OriginalParameters.lpDIBBits);
    unsigned char *abSourceStroke=new unsigned char[m_DataParams[Head].OriginalParameters.ImageSize[Row]];
    if (FileStream.open(QIODevice::ReadOnly)){
        FileStream.seek(SeekPosition);
        DWORD ReadCount=(DWORD)FileStream.read((char*)abSourceStroke,m_DataParams[Head].OriginalParameters.ImageSize[Row]);
        FileStream.close();
        if (ReadCount == m_DataParams[Head].OriginalParameters.ImageSize[Row])
             DownLoadOk = SetupPrintDataRegisterRows(Head, Row, abSourceStroke, false);
    }
    delete (abSourceStroke);
    return DownLoadOk;
}

/**
WriteSwatheFile
Writes a swathedatafile to disc
The first time this file is created it write the SwatheWidth, SwatheLength and SwatheCount
**/
BOOL InkjetXusbVarCtrl::WriteSwatheFile(DWORD Head, BYTE *abSourceStroke, DWORD NumSwathe)
{
    BOOL SetupOk = false;
    DWORD Row = 0;
    QString Filename=QString("c:\\XaarLog\\TestSwathe_%1%2.SWA").arg(m_thisXusb+1).arg(Head+1);
    QFile FileStream(Filename);
    if (NumSwathe==0) FileStream.open(QIODevice::WriteOnly);
    else FileStream.open(QIODevice::Append);
    if (FileStream.isOpen()){
        if (NumSwathe == 0){
            DWORD BytesPerStroke = PrintJets[m_DataParams[Head].OriginalParameters.HeadType] * m_DataParams[Head].OriginalParameters.BitDifference/ 8;
            FileStream.write((char*)&BytesPerStroke, sizeof(DWORD));
            FileStream.write((char*)&m_DataParams[Head].OriginalParameters.ImageLength[Row], sizeof(DWORD));
            FileStream.write((char*)&m_DataParams[Head].OriginalParameters.NumberSwathes[Row],  sizeof(DWORD));
            FileStream.write((char*)&m_DataParams[Head].OriginalParameters.BitDifference, sizeof(DWORD));
         }
         FileStream.write((char*)abSourceStroke,m_DataParams[Head].OriginalParameters.ImageSize[Row]);
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


/**	LoadActiveBitmapToCard
Determines which is the active window and then loads the bitmap
Checks that the bitmap loaded is correct for the head type connected
**/
BOOL InkjetXusbVarCtrl::LoadActiveBitmapToCard(DWORD Head, DWORD Row)
{
    BOOL bReturnOk = FALSE;
    //HDIB  hDIB = NULL;
    //Original
    /*CPrintParameters sPrintParams(this,m_thisXusb, m_Key);*/
    if (Row == 0)
    {
        m_DataParams[Head].OriginalParameters.bSelectHead[0] = m_sPrintParams.m_bSelectHead[Head][0];
        m_DataParams[Head].OriginalParameters.bSelectHead[1] = m_sPrintParams.m_bSelectHead[Head][1];
        m_DataParams[Head].OriginalParameters.AllSwathesFit[0] = true;			// this image will fit entirely in memory
        m_DataParams[Head].OriginalParameters.AllSwathesFit[1] = true;			// this image will fit entirely in memory
    }

    m_DataParams[Head].OriginalParameters.HeadType = bXaarScorpionGetHeadType(m_thisXusb, Head);
    if (m_DataParams[Head].OriginalParameters.HeadType == NOTSET)
    {
        m_DataParams[Head].OriginalParameters.bSelectHead[0] = false;					// if head is not connected .... don't print to it
        m_DataParams[Head].OriginalParameters.bSelectHead[1] = false;					// if head is not connected .... don't print to it
    }

    if (Row == 0 || (Row == 1 && m_sPrintParams.m_SeparateRows)){
        //emit error("Num SW =0",m_thisXusb);
        m_DataParams[Head].OriginalParameters.NumberSwathes[Row] = 0;
     }

    if (!((m_sPrintParams.m_SeparateRows && m_DataParams[Head].OriginalParameters.bSelectHead[Row]) ||
        (Row == 0 && !m_sPrintParams.m_SeparateRows && (m_DataParams[Head].OriginalParameters.bSelectHead[0] || m_DataParams[Head].OriginalParameters.bSelectHead[1])))){
        return true;
    }
    QImage inkjetImage;    
     if (m_inkjetImage.isEmpty()){
        QImage inkjetImagedummy(m_ImageToPrint);
        inkjetImage=inkjetImagedummy.mirrored(false,true);
        //else inkjetImage=inkjetImagedummy;
        //inkjetImage=QImage(m_ImageToPrint);
     }
    else{
        QImage inkjetImagedummy(m_inkjetImage);
        inkjetImage=inkjetImagedummy.mirrored(false,true);
        //else inkjetImage=inkjetImagedummy;
        //inkjetImage=QImage(m_inkjetImage);
    }


    if (m_DataParams[Head].OriginalParameters.HeadType == NOTSET)
        m_DataParams[Head].OriginalParameters.HeadType = m_sPrintParams.m_PrintType;		// default to head type entered
    m_DataParams[Head].OriginalParameters.Binary = BINARYHEAD[m_DataParams[Head].OriginalParameters.HeadType];
    m_DataParams[Head].OriginalParameters.NumberOfRows = NUMBEROFROWS[m_DataParams[Head].OriginalParameters.HeadType];

    //if (hDIB != NULL)
    //if (!m_inkjetImage.isEmpty())
    if (inkjetImage.byteCount()==0){
        emit error("ERROR No hay datos de imagen",m_thisXusb);
        return false;
    }
    else
    {
        bReturnOk = TRUE;
        //inkjetImage.save(QString("test.bmp"),"BMP");
        LPSTR lpDIBBits=(LPSTR)inkjetImage.bits();
        int cyDIB = inkjetImage.width();
        m_DataParams[Head].OriginalParameters.ImageLength[Row]=inkjetImage.height();
        WORD wNumColours=inkjetImage.colorCount();
        if (wNumColours==2) {
            m_sPrintParams.m_ForceMode=true;
            m_sPrintParams.m_bBinaryPaletteRemap=true;
            m_sPrintParams.m_bPaletteRemap=true;
            m_DataParams[Head].OriginalParameters.Binary=true;
            m_DataParams[Head].OriginalParameters.BitDifference=1;
            m_DataParams[Head].OriginalParameters.bPaletteRemap=true;
            BYTE PaletteTable[2] = {0};
            PaletteTable[0] = 3;    // eg 3 or 6
            bXaarScorpionWriteRemapTable(m_thisXusb, Head, 1, PaletteTable);
        }

        DWORD MaxJets = MAXJETS[m_DataParams[Head].OriginalParameters.HeadType] / m_DataParams[Head].OriginalParameters.NumberOfRows;

        bool Binary = m_DataParams[Head].OriginalParameters.Binary;
        switch (m_sPrintParams.m_ForceMode)
        {
            case 1:	Binary = true;
                break;
            case 2:	Binary = false;
                break;
        }
        DWORD BitDifference = 1;
        if (!Binary)
        {
            if (m_DataParams[Head].OriginalParameters.Enable2Bit)
                BitDifference = 2;				// number of bits to store
            else
                BitDifference = 4;				// number of bits to store
        }

        DWORD SourceStrokeWidth = (BitDifference * MaxJets / 8 + BLOCKSIZE - 1)/ BLOCKSIZE;// rounds to 64 byte boundary

        if (m_DataParams[Head].OriginalParameters.ImageLength[Row] * SourceStrokeWidth > XaarScorpionGetMaxHeadBlocks(m_HeadsRequired))
        {
            emit error("Imagen muy grande",(int)m_thisXusb);
            bReturnOk = FALSE;
        }

        if ((unsigned int)cyDIB > MAXJETS[m_DataParams[Head].OriginalParameters.HeadType] && // contains at least 2 swathes
            2 * m_DataParams[Head].OriginalParameters.ImageLength[Row] * SourceStrokeWidth > XaarScorpionGetMaxHeadBlocks(m_HeadsRequired))
        {
            emit error("Imagen muy grande para 2 swathes",(int)m_thisXusb);
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
                            //emit error("Printing Binary BMP!",(int)m_thisXusb);
                            //DisplayErrorMessage(IDS_GREYSCALEHEADBINARYIMAGE_ERROR, MB_OK | MB_APPLMODAL | MB_ICONSTOP);

                             bReturnOk = PrintBinaryBMP(Head, Row, m_sPrintParams, cyDIB, lpDIBBits);
                            //bReturnOk = FALSE;
                            break;
                        case BIT8COLOUR:
                            bReturnOk = Print256GreyScaleBMP(Head, Row, m_sPrintParams, cyDIB, lpDIBBits);
                            if (!bReturnOk)
                                emit error("Error al enviar al cabezal",(int)m_thisXusb);
                            break;
                        default:
                            emit error("No es imagen de 4 bit",(int)m_thisXusb);
                            bReturnOk = FALSE;
            }
         }
    }
    return bReturnOk;
}

/**	SetupParameters
  Sets up the card related common parameters, prior to setting
  specific head parameters
**/
bool InkjetXusbVarCtrl::SetupParameters()
{
    m_WantSubDelay = false;
    bool HeadSelected = false;


    m_HeadsRequired = 0;
    //Original
    /*CPrintParameters sPrintParams(this,m_thisXusb, m_Key);*/
    m_bSelectCard= m_sPrintParams.m_bSelectedCard;
        ///if (m_thisXusb == 0)
        ///{
            m_SeparateRows = m_sPrintParams.m_SeparateRows == 0 ? false:true;
            m_PrintOnce = m_sPrintParams.m_PrintOnce == 0 ? false:true;
            if (m_sPrintParams.m_PrintMode == FIXEDGAPCONT)
                m_InterGap = 0xffffff;
            else
                m_InterGap = 0;
        ///}
        if (m_bSelectCard)
        {
            for (DWORD Head = 0; Head < /*MAXHEADS*/(DWORD)m_NumCabezales; Head++)
                for (DWORD Row = 0; Row < MAXROWS; Row++)
                {
                    if (m_sPrintParams.m_bSelectHead[Head][Row])
                    {
                        if (bXaarScorpionGetHeadConfig(m_thisXusb, Head))
                            m_DataParams[Head].OriginalParameters.HeadType = bXaarScorpionGetHeadType(m_thisXusb, Head);
                        if (m_DataParams[Head].OriginalParameters.HeadType == OMNIDOT380)
                            m_SeparateRows = true;		// if 380 force single row select to true
                    }
                }
            for (DWORD Head = 0; Head < /*MAXHEADS*/(DWORD)m_NumCabezales; Head++)
            {
                for (DWORD Row = 0; Row < MAXROWS; Row++)
                {
                    if (m_sPrintParams.m_bSelectHead[Head][Row])
                    {
                        HeadSelected = true;
                        m_DataParams[Head].OriginalParameters.HeadIndex[Row] = m_HeadsRequired;		//set the head index for this head
                        if (m_sPrintParams.m_SeparateRows || (!m_sPrintParams.m_SeparateRows && (Row == 0 || (Row == 1 && !m_sPrintParams.m_bSelectHead[Head][0]))))
                            m_HeadsRequired++;
                        if (m_sPrintParams.m_PrintMode == FIXEDGAPCONT && m_DataParams[Head].OriginalParameters.HeadType != NOTSET)
                            m_InterGap = qMin(m_InterGap, m_sPrintParams.m_uiProductOffset[0][Head][Row]);
                    }

                    m_DataParams[Head].OriginalParameters.SubPixelDivide = m_sPrintParams.m_uiSubPixelDivide - 1;
                    if (m_sPrintParams.m_uiSubPixelDivide > 1)
                        m_WantSubDelay = true;
                }
            }
            if (!m_WantSubDelay)				// if we dont want sub pixel
                for (DWORD Head = 0; Head < /*MAXHEADS*/(DWORD)m_NumCabezales; Head++)
                    m_DataParams[Head].OriginalParameters.SubPixelDivide = 0;	// clear the value so it's not used
        }
    if (!HeadSelected)
        emit error("No head selected",(int)m_thisXusb);
    return HeadSelected;
}

/**
    Sets up the USB controller
    Takes all the data entered and sends this down to the USB board.
    The image will have already been sent down as separate button.
    As the image is sent down as a separate button, we need to modifiy anything that is print mode
    specific here
    m_IgnoreReversePD should be false if printing needs to ignore
    direction when looking for reverse PD's
**/
BOOL InkjetXusbVarCtrl::SetupPrintMode()
{

    BOOL SendOk = true;
    DWORD SequenceStart;

    //BeginWaitCursor();
    m_ActiveCards = 0;
    bXaarScorpionEnableIdleSpit(false);					// take out of idle spit mode as image loaded ready to print
    //Original
    /*CPrintParameters sPrintParams(this,m_thisXusb, m_Key);*/
        ///if (m_thisXusb == 0)
            SequenceStart = m_sPrintParams.m_SequenceStart;
        if (m_bSelectCard)
        {
            while (bXaarScorpionXUSBBusy(m_thisXusb))
                SleepEx(25, true);

            m_ActiveCards++;
            m_PrintMask = 0;

            if (((m_sPrintParams.m_PrintTransportMode == UNIPRINTWITHSPIT) ||
                ((m_sPrintParams.m_PrintTransportMode == UNIPRINT || m_sPrintParams.m_PrintTransportMode == BIPRINT)
                            && m_sPrintParams.m_IgnoreReversePD)) && // for bi-dir printing ignore PD on reverse pass
                            (m_sPrintParams.m_PrintMode == SINGLESHOT || m_sPrintParams.m_PrintMode == LABELWAIT))
                SendOk = bXaarScorpionPDNoReverse(m_thisXusb, true);
            else
                SendOk = bXaarScorpionPDNoReverse(m_thisXusb, false);

            if (SendOk)
                SendOk = bXaarScorpionSetDDFSValue(m_thisXusb, m_sPrintParams.m_DDFSValue * (m_sPrintParams.m_ClockType == 1 ? 3:1) * m_sPrintParams.m_uiEncoderDivide);
            if (SendOk)
                SendOk = bXaarScorpionSetDDFSEnable(m_thisXusb, m_sPrintParams.m_SysClock == DDFS ? 1:0 );	// select source of encoder
            if (SendOk)
                SendOk = bXaarScorpionSetPDInternal(m_thisXusb, SequenceStart == 1 ?true:false);	// set PD source internal or external
            if (SendOk)
                SendOk = bXaarScorpionSetEncoderDivide(m_thisXusb, m_sPrintParams.m_uiEncoderDivide - 1);
            if (SendOk)
                SendOk = bXaarScorpionEncoderPulseMultiply(m_thisXusb, m_sPrintParams.m_uiEncoderMultiply > 0 ?m_sPrintParams.m_uiEncoderMultiply - 1:0);
            if (SendOk)
                SendOk = bXaarScorpionIgnorePD(m_thisXusb, m_sPrintParams.m_PDDiscardCount);
            if (m_sPrintParams.m_SysClock == ABSOLUTESE)
            {
                AbsoluteEncoderValues AbsoluteValues;
                AbsoluteValues.EncoderCountPreloadValue = m_sPrintParams.m_PreloadValue;
                AbsoluteValues.ForwardPDTriggerValue = m_sPrintParams.m_ForwardPDTrigger;
                AbsoluteValues.InvertDirection = (BYTE)m_sPrintParams.m_Invert;
                AbsoluteValues.PreloadType = (BYTE)m_sPrintParams.m_ReloadOnExternalPD;
                AbsoluteValues.ReversePDTriggerValue = m_sPrintParams.m_ReversePDTrigger;
                AbsoluteValues.PDGeneration = (BYTE)m_sPrintParams.m_PDGeneration;

                if (SendOk)
                    SendOk = bXaarScorpionAbsoluteEncoderSetup(m_thisXusb, AbsoluteValues);
            }
            if (SendOk)
                SendOk = bXaarScorpionAbsoluteEncoderEnable(m_thisXusb, m_sPrintParams.m_SysClock == ABSOLUTESE ? true:false);
            for (DWORD Head = 0; Head < /*MAXHEADS*/(DWORD)m_NumCabezales; Head++)
            {
                if (m_DataParams[Head].OriginalParameters.bSelectHead[0])				// if this head is required set up print mask
                    m_PrintMask |= (BIT0 << (Head * 2));					// set bits one row in print head
                if (m_DataParams[Head].OriginalParameters.bSelectHead[1])
                    m_PrintMask |= (BIT1 << (Head * 2));				// if 2nd rows is selected print with row 2 as well
            }
            if (SendOk)
                SendOk = bXaarScorpionEnablePrintMode(m_thisXusb, m_PrintMask, true);		// now enter print mode for heads selected
        }

    if (!SendOk)
        emit error("Error al imprimir",(int)m_thisXusb);
    InTimerTick = false;
    //m_nTimer = (UINT)SetTimer(1, Delay, NULL);
    Sleep(1);
    //EndWaitCursor();

    return SendOk;
}

BOOL InkjetXusbVarCtrl::SendImage(){
    XUSBDownloadData();
    return true;
}

/** Envia la imagen*/
BOOL InkjetXusbVarCtrl::SendFirstImage()
{
    BOOL bLoadOk = true;

    m_NumberSwathes = 0;
    m_AllSwathesFit = true;										// all images will fit entirely in memory
    m_SwatheFile = false;
    memset(m_DataParams,0x00,sizeof(m_DataParams));
    //Original
    /*CPrintParameters sPrintParams(this,m_thisXusb, m_Key);*/
    m_bSelectCard = m_sPrintParams.m_bSelectedCard;
    //Hacemos stop
    while (bXaarScorpionXUSBBusy(m_thisXusb)){
        SleepEx(25, true);
    }
    if (SetupParameters()){
        XUSBDownloadData();
        //if (!SetupPrintMode()) bLoadOk=false;
    }
    else bLoadOk=false;

     //emit error("Envio de imagen finalizado",(int)m_thisXusb);
    return bLoadOk;
}



/*El metodo que hace el trabajo*/
void InkjetXusbVarCtrl::XUSBDownloadData()
{
    BOOL bLoadOk = true;
    DWORD NumberSwathes = 0;
    for (int Head = 0; Head < /*MAXHEADS*/m_NumCabezales && bLoadOk; Head++)
    {
        for (DWORD Row = 0; Row < MAXROWS && bLoadOk; Row++)
        {
            bLoadOk = LoadActiveBitmapToCard(Head, Row);
            if (bLoadOk)
            {
                if (NumberSwathes == 0)				// only want to add swathe count once
                    NumberSwathes += m_DataParams[Head].OriginalParameters.NumberSwathes[Row];
                if (m_DataParams[Head].OriginalParameters.AllSwathesFit[Row] == false)
                    m_AllSwathesFit = false;
            }
        }
    }

    m_NumberSwathes = qMax(m_NumberSwathes, NumberSwathes);		// may need lock on this variable??


    if (m_AllSwathesFit)			// need to download more swathes when there is space
    {
        bool Exit = false;
        XaarScorpionAllowLogging(false);

        //	need to see if there is room to download more data
            BOOL ReadOk = bXaarScorpionGetMemoryDataCard(m_thisXusb);			// read swathe buffer space
            for (DWORD Head = 0; Head < /*MAXHEADS*/(DWORD)m_NumCabezales && ReadOk && !Exit; Head++)
            {
                for (DWORD Row = 0; Row < MAXROWS && !Exit; Row++)
                {
                    if (m_DataParams[Head].OriginalParameters.bSelectHead[Row])		// if we want this head
                        SendSwathes(Head, Row, false);								// try to put data into the memory
                }
            }

        //XaarScorpionAllowLogging(false);
    }
}

/*El metodo que hace el trabajo*/
void InkjetXusbVarCtrl::XUSBDownloadDataToHead(int head)
{
    int Head=head;
    BOOL bLoadOk = true;
    DWORD NumberSwathes = 0;
    for (DWORD Row = 0; Row < MAXROWS && bLoadOk; Row++)
    {
        bLoadOk = LoadActiveBitmapToCard(Head, Row);
        if (bLoadOk)
        {
            if (NumberSwathes == 0) NumberSwathes += m_DataParams[Head].OriginalParameters.NumberSwathes[Row];				// only want to add swathe count once
            if (m_DataParams[Head].OriginalParameters.AllSwathesFit[Row] == false) m_AllSwathesFit = false;
        }
    }

    m_NumberSwathes = qMax(m_NumberSwathes, NumberSwathes);		// may need lock on this variable??


    if (m_AllSwathesFit)			// need to download more swathes when there is space
    {
        bool Exit = false;
        XaarScorpionAllowLogging(false);

        //	need to see if there is room to download more data
            bXaarScorpionGetMemoryDataCard(m_thisXusb);			// read swathe buffer space
            for (DWORD Row = 0; Row < MAXROWS && !Exit; Row++)
            {
                if (m_DataParams[Head].OriginalParameters.bSelectHead[Row])		// if we want this head
                    SendSwathes(Head, Row, false);								// try to put data into the memory
            }
    }
}


/** Nuestro slot de recepcion*/
void InkjetXusbVarCtrl::process(){
    SendImage();
    XaarScorpionAllowLogging(false);
    emit finished();
}
/** Devuelve la swathe en la que estoy*/
DWORD InkjetXusbVarCtrl::GetCurrentPrintSwathe(){
    DWORD dummy;
    bXaarScorpionGetMemoryDataCard(m_thisXusb);
    //bXaarScorpionGetCurrentPrintSwatheUpdated(0,0,m_DataParams[0],dummy );
    //bXaarScorpionGetMemoryDataLocation(m_thisXusb,0,0,0,dummy);
    bXaarScorpionGetMemoryDataLocation(m_thisXusb,m_memoryMap->m_mapa.m_machinedefaults.ControlHead,0,0,dummy);

    //En dummy tenemos la posicion actual..no testeo la primera posicion ya que siempre va a ser false
    int x=0;
    for (x=1;x<m_BufferSize-1;x++){
        if (dummy<PosMemSwathe[m_memoryMap->m_mapa.m_machinedefaults.ControlHead][x]) break;
    }
    return x-1;
    /*DWORD dummy=0;
    struct UpdatedPrintDataParameters tmpDataParams=m_DataParams[0];
    bXaarScorpionGetCurrentPrintSwatheUpdated(0,0,tmpDataParams,dummy);
    return dummy;*/

}
/** Establece que esa es la ultima swathe*/
void InkjetXusbVarCtrl::SetLastSwathe(){
    for (int x=0;x</*MAXHEADS*/m_NumCabezales;x++){
        m_DataParams[x].OriginalParameters.LastSwathe[0]=true;
        m_DataParams[x].OriginalParameters.LastSwatheInMemory[0]=true;
        m_DataParams[x].OriginalParameters.LastSwathe[1]=true;
        m_DataParams[x].OriginalParameters.LastSwatheInMemory[1]=true;
        /*m_DataParams[x].OriginalParameters.MemoryBlock[0] = m_DataParams[x].OriginalParameters.FirstMemoryBlock[0];	// now point to next block
        m_DataParams[x].OriginalParameters.MemoryBlock[1] = m_DataParams[x].OriginalParameters.FirstMemoryBlock[1];	// now point to next block
        m_DataParams[x].OriginalParameters.DirBlock = 0;*/
    }
}
/** Cambia la imagen previamente cargada con setImagen en el swathe indicado*/
void InkjetXusbVarCtrl::ReplaceSwathe(DWORD head,DWORD row, int swathe){

    DWORD localPosMem=PosMemSwathe[head][swathe];
    struct UpdatedPrintDataParameters localDataParams;
    memcpy(&localDataParams,&m_DataParams[head],sizeof(struct UpdatedPrintDataParameters));
    localDataParams.OriginalParameters.NibbleIndex=0;
    localDataParams.OriginalParameters.MemoryBlock[row]=localPosMem;
    //QImage inkjetImage=m_ImageToPrint.mirrored(false,true);
    QImage inkjetImage=m_ImageToPrint.mirrored(false,true);

    localDataParams.OriginalParameters.lpDIBBits=(LPSTR)inkjetImage.bits();
    //GenerateGSPrintData(head,row);

    DWORD SourceLine;
    BYTE *abSourceStroke = new BYTE [localDataParams.OriginalParameters.ImageSize[row]];

    DWORD NumSwathe = localDataParams.OriginalParameters.StoredSwathes[row] % localDataParams.OriginalParameters.NumberSwathes[row];
    DWORD Swathe = localDataParams.OriginalParameters.bReverseSwatheOrder ? (localDataParams.OriginalParameters.NumberSwathes[row] - NumSwathe - 1):NumSwathe;

    bool ForwardBuffer = localDataParams.OriginalParameters.ForwardBuffer;		// need to adjust order of filling image buffer
    if (localDataParams.OriginalParameters.bReverseImageOrder)
        ForwardBuffer = !ForwardBuffer;

    DWORD BaseSourceByteOffset = localDataParams.OriginalParameters.SwatheStartIndex + Swathe * localDataParams.OriginalParameters.SwatheIncrement; // point to correct swathe start
    if (m_SwatheShiftWanted && localDataParams.OriginalParameters.bReverseSwatheOrder && BaseSourceByteOffset > 0 &&
        (localDataParams.OriginalParameters.TotalImageWidth[row] % localDataParams.OriginalParameters.HeadChannels) != 0)
        BaseSourceByteOffset -= (localDataParams.OriginalParameters.HeadChannels - (localDataParams.OriginalParameters.TotalImageWidth[row] % localDataParams.OriginalParameters.HeadChannels)); // offset for reverse
    DWORD ImageWidth;
    if (m_SwatheShiftWanted && localDataParams.OriginalParameters.bReverseSwatheOrder && NumSwathe + 1 == localDataParams.OriginalParameters.NumberSwathes[row] &&
        (localDataParams.OriginalParameters.TotalImageWidth[row] % localDataParams.OriginalParameters.HeadChannels) != 0)
        ImageWidth = localDataParams.OriginalParameters.TotalImageWidth[row] % localDataParams.OriginalParameters.HeadChannels;
    else
    {
        if (localDataParams.OriginalParameters.TotalImageWidth[row] > BaseSourceByteOffset)
            ImageWidth = qMin(localDataParams.OriginalParameters.TotalImageWidth[row] - BaseSourceByteOffset, localDataParams.OriginalParameters.HeadChannels);	// number of jets in chip bank
        else
            ImageWidth = 0;							// 0 jets to print
    }
    UINT uiLineOffset = WIDTHBYTES(localDataParams.OriginalParameters.TotalImageWidth[row] * 8);
    DWORD StartClear = ImageWidth;
    DWORD EndClear = localDataParams.OriginalParameters.HeadChannels;
    BYTE bTargetMask[2];
    DWORD ShiftValue = 0;
    if (m_SwatheShiftWanted)
        ShiftValue = PrintJets[localDataParams.OriginalParameters.HeadType] - ImageWidth;

    DWORD TargetByteOffset = ShiftValue / 2;

    if (m_SwatheShiftWanted)
    {
        StartClear = 0;
        EndClear = TargetByteOffset;
    }

    if (m_SeparateRows || (!m_SeparateRows && localDataParams.OriginalParameters.BufferReverse[row]))
    {
        bTargetMask[0] = START_NIBBLE_MASK >> (localDataParams.OriginalParameters.BitDifference * ((1 + ShiftValue) % 2));
        bTargetMask[1] = START_NIBBLE_MASK >> (localDataParams.OriginalParameters.BitDifference * (ShiftValue % 2));
    }
    else
    {
        bTargetMask[0] = START_NIBBLE_MASK >> (localDataParams.OriginalParameters.BitDifference * (ShiftValue % 2));
        bTargetMask[1] = START_NIBBLE_MASK >> (localDataParams.OriginalParameters.BitDifference * ((1 + ShiftValue) % 2));
    }

    memset(abSourceStroke, 0, localDataParams.OriginalParameters.ImageSize[row]);

    for (DWORD iOuterLoop = 0; iOuterLoop < localDataParams.OriginalParameters.ImageLength[row] ; iOuterLoop++)
    {

        if (ForwardBuffer)
            SourceLine = localDataParams.OriginalParameters.ImageLength[row] - iOuterLoop - 1;
        else
            SourceLine = iOuterLoop;
        BYTE MaskNibble = 0;
        DWORD TargetByte = localDataParams.OriginalParameters.SourceStrokeWidth * iOuterLoop + TargetByteOffset;
        DWORD SourceByteOffset = BaseSourceByteOffset;

        for (DWORD uiInnerLoop = 0; uiInnerLoop < ImageWidth;uiInnerLoop++)
        {
            abSourceStroke[TargetByte] |= (bTargetMask[MaskNibble] & m_LUTValue[(BYTE)localDataParams.OriginalParameters.lpDIBBits[uiLineOffset * SourceLine + uiInnerLoop + SourceByteOffset]]);
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
    //Cargamos nueva imagen
    bXaarScorpionSetPrintDataUpdated(m_thisXusb, abSourceStroke, localDataParams);

    delete [] abSourceStroke;
}


