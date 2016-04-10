#ifndef INKJETXUSBVARCTRL_H
#define INKJETXUSBVARCTRL_H

#include <QObject>
#include "ScorpionDLL-ext.h"
#include "printparameters.h"
#include "MemoryMap.h"


class InkjetXusbVarCtrl : public QObject
{
    Q_OBJECT
public:
    explicit InkjetXusbVarCtrl(QObject *parent = 0,int Card=0,const QString & qstrImage="",int numImages=0);
    void Reset();
    void SetImage(const QImage & image);
    BOOL LoadActiveBitmapToCard(DWORD Head, DWORD Row);
    bool SetupParameters();
    BOOL SetupPrintMode();
    BOOL SendFirstImage();
    BOOL SendImage();
    int returnNumCabezales();
    DWORD GetCurrentPrintSwathe();
    void SetLastSwathe();
    void ReplaceSwathe(DWORD head,DWORD row, int swathe);
    //void ExitContinuousPrintMode();
    void setNumImages(int n);
    //Nueva paleta
    void setNewPalette(char maxVol);
    //Numero de copias
    void setCopyCount(DWORD copy);
    void setNewFullPalette(char *listVol);
    void setNewConfigFile(QString filename);

private:
    BOOL SetupPrintDataRegisterRows2(DWORD Head, DWORD Row, BYTE *abSourceStroke, bool SpitSwatheData);

    //static unsigned long XUSBDownloadDataThread(void* lParam);
    BOOL SendSwathes(DWORD Head, DWORD Row, bool ForceImageLoad );
    BOOL SendSwathes(DWORD Head, DWORD Row, DWORD SwathesToStore);
    BOOL SetupPrintDataRegisterRows(DWORD Head, DWORD Row, BYTE *abSourceStroke, bool SpitSwatheData);
    BOOL SetupCommonDataParameters(DWORD Head, DWORD Row, CPrintParameters sPrintParams,int cyDIB, LPSTR lpDIBBits, bool SwatheFile);
    BOOL GenerateBinaryPrintData(DWORD Head, DWORD Row);
    BOOL PrintBinaryBMP(DWORD Head, DWORD Row, CPrintParameters sPrintParams,int cyDIB, LPSTR lpDIBBits);
    BOOL GenerateGSPrintData(DWORD Head, DWORD Row);
    BOOL Generate2BitPrintData(DWORD Head, DWORD Row);
    BOOL Print256GreyScaleBMP(DWORD Head, DWORD Row, CPrintParameters sPrintParams,int cyDIB, LPSTR lpDIBBits);
    BOOL GenerateSpitSwathe(DWORD Head, DWORD Row);
    BOOL ProcessSwatheFile(DWORD Head, DWORD Row, CPrintParameters sPrintParams,int cyDIB, LPSTR lpDIBBits, int SwatheCount);
    BOOL GenerateSwathePrintData(DWORD Head, DWORD Row);
    BOOL GenerateSwathePrintDataUpdated(DWORD Head, DWORD Row);
    BOOL WriteSwatheFile(DWORD Head, BYTE *abSourceStroke, DWORD NumSwathe);
    BOOL OnTheFlyDownload(DWORD Row);
    /*Este solo de momento para ver si imprime*/
    void XUSBDownloadData();
    void XUSBDownloadDataToHead(int head);



    DWORD m_thisXusb;
    HANDLE m_threads, m_ShareSemaphore, m_ShareSemaphoreMIn;
    int m_ActualXUSBsFound;
    QString m_Key;
    UINT m_uiMaxJets;
    DWORD m_NumberSwathes, m_HeadsRequired, m_InterGap, m_PrintMask ;
    UINT m_Select;
    BYTE m_LUTValue[256], m_BackgroundValue;

    //class CXUSB_MultiApp *m_pApp;
    //class CMainFrame *m_pMainFrame;
    UINT m_nTimer, m_SwatheTimer;
    DWORD m_TimerCount, m_SWTestCounts;
    DWORD m_ActiveCards;
    struct UpdatedPrintDataParameters m_DataParams[MAXHEADS];
    bool m_bSelectCard;
    bool m_WantTestPD;
    bool m_AllSwathesFit, m_WantSubDelay;
    bool m_SeparateRows, m_ErrorDiffusionEnabled;
    bool m_SwatheFile, m_WriteSwatheFile;
    bool InTimerTick;
    DWORD m_PreviousCurrentSwathe[MAXHEADS][MAXROWS];
    BYTE *m_ImagePtr;
    bool m_SwatheShiftWanted;
    bool m_PrintOnce;
    QString m_inkjetImage;
    QImage m_ImageToPrint;
    CPrintParameters m_sPrintParams;
    int m_BufferSize;
    char AuxPalette[8];
    DWORD m_CopyCount;
    int m_NumCabezales;
    MemoryMap *m_memoryMap;


signals:
    void finished();
    void error(QString err,int xusb);

public slots:
    void process();

};

#endif
