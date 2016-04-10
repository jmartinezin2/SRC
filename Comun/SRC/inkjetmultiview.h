#ifndef INKJETMULTIVIEW_H
#define INKJETMULTIVIEW_H

#include <QObject>

//Defino tipos para compatibilizar con la lib de de windows de xaar
/*typedef  unsigned short DWORD;
typedef bool BOOL;
typedef unsigned char BYTE;
typedef char* LPSTR;
typedef unsigned int UINT;
typedef void* PVOID;
typedef PVOID HANDLE;*/

#include "ScorpionDLL-ext.h"
#include "printparameters.h"

//Sustituye a m_pApp->GetXUSBsFound()
//#define NUM_XUSB 1

/** Miniclase para que haga envie a los xaar las imagenes*/
class XaarWorker : public QObject {
        Q_OBJECT
    public:
        XaarWorker();
        ~XaarWorker();

    public slots:
        void process();

    signals:
        void finished();
        void error(QString err);

    private:
    };

class InkjetMultiview : public QObject
{
    Q_OBJECT
public:
    explicit InkjetMultiview(QObject *parent = 0);
    BOOL LoadActiveBitmapToCard(DWORD Card, DWORD Head, DWORD Row);
    bool SetupParameters();
    BOOL SetupPrintMode();
    BOOL SendImage(QString image);
    //void ExitContinuousPrintMode();

private:
    //static unsigned long XUSBDownloadDataThread(void* lParam);
    BOOL SendSwathes(DWORD Card, DWORD Head, DWORD Row, bool ForceImageLoad );
    BOOL SendSwathes(DWORD Card, DWORD Head, DWORD Row, DWORD SwathesToStore);
    BOOL SetupPrintDataRegisterRows(DWORD Card, DWORD Head, DWORD Row, BYTE *abSourceStroke, bool SpitSwatheData);
    BOOL SetupCommonDataParameters(DWORD Card, DWORD Head, DWORD Row, CPrintParameters sPrintParams,int cyDIB, LPSTR lpDIBBits, bool SwatheFile);
    BOOL GenerateBinaryPrintData(DWORD Card, DWORD Head, DWORD Row);
    BOOL PrintBinaryBMP(DWORD Card, DWORD Head, DWORD Row, CPrintParameters sPrintParams,int cyDIB, LPSTR lpDIBBits);
    BOOL GenerateGSPrintData(DWORD Card, DWORD Head, DWORD Row);
    BOOL Generate2BitPrintData(DWORD Card, DWORD Head, DWORD Row);
    BOOL Print256GreyScaleBMP(DWORD Card, DWORD Head, DWORD Row, CPrintParameters sPrintParams,int cyDIB, LPSTR lpDIBBits);
    BOOL GenerateSpitSwathe(DWORD Card, DWORD Head, DWORD Row);
    BOOL ProcessSwatheFile(DWORD Card, DWORD Head, DWORD Row, CPrintParameters sPrintParams,int cyDIB, LPSTR lpDIBBits, int SwatheCount);
    BOOL GenerateSwathePrintData(DWORD Card, DWORD Head, DWORD Row);
    BOOL GenerateSwathePrintDataUpdated(DWORD Card, DWORD Head, DWORD Row);
    BOOL WriteSwatheFile(DWORD Card, DWORD Head, BYTE *abSourceStroke, DWORD NumSwathe);
    BOOL OnTheFlyDownload(DWORD Row);
    /*Este solo de momento para ver si imprime*/
    void XUSBDownloadData();


    HANDLE m_threads[MAXSCORPIONS], m_ShareSemaphore[MAXSCORPIONS], m_ShareSemaphoreMIn[MAXSCORPIONS];
    int m_ActualXUSBsFound;
    QString m_Key;
    UINT m_uiMaxJets;
    DWORD m_NumberSwathes, m_HeadsRequired[MAXSCORPIONS], m_InterGap, m_PrintMask[MAXSCORPIONS] ;
    UINT m_Select;
    BYTE m_LUTValue[256], m_BackgroundValue;

    //class CXUSB_MultiApp *m_pApp;
    //class CMainFrame *m_pMainFrame;
    UINT m_nTimer, m_SwatheTimer;
    DWORD m_TimerCount, m_SWTestCounts;
    DWORD m_ActiveCards;
    struct UpdatedPrintDataParameters m_DataParams[MAXSCORPIONS][MAXHEADS];
    bool m_bSelectCard[MAXSCORPIONS];
    bool m_WantTestPD;
    bool m_AllSwathesFit, m_WantSubDelay;
    bool m_SeparateRows, m_ErrorDiffusionEnabled;
    bool m_SwatheFile, m_WriteSwatheFile;
    bool InTimerTick;
    DWORD m_PreviousCurrentSwathe[MAXSCORPIONS][MAXHEADS][MAXROWS];

    BYTE *m_ImagePtr;
    bool m_SwatheShiftWanted;
    bool m_PrintOnce;
    QString m_inkjetImage;
    
signals:
    
public slots:
    
};

#endif // INKJETMULTIVIEW_H
