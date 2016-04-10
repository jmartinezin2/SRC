#ifndef INKJETXUSBCTRL_H
#define INKJETXUSBCTRL_H

#include <QObject>
#include "ScorpionDLL-ext.h"
#include "printparameters.h"
//#include "globals.h"

class InkjetXusbCtrl : public QObject
{
    Q_OBJECT
public:
    explicit InkjetXusbCtrl(QObject *parent = 0,int Card=0,const QString & qstrImage="");
    void SetImage(const QImage & image);
    BOOL LoadActiveBitmapToCard(DWORD Head, DWORD Row);
    bool SetupParameters();
    BOOL SetupPrintMode();
    BOOL SendImage();
    int returnNumCabezales();
    void Reset();
    void setCard(int card);
    void setNewPalette(char maxVol);
    void setNewFullPalette(char *listVol);
    DWORD GetCurrentPrintSwathe();
    void GetCurrentPrintMemory(int minHead,int maxHead,DWORD *p);
    void SetSwatheControl();
    void SetPrintOnce(bool value);
    void getSerial(char *buffer);
    void getSerialHead(int head, char *buffer);
    void softStop();
    bool readSwatheCount(DWORD *SwatheCountArray);
    void writeSwatheCount(DWORD *SwatheCountArray);
    DWORD getSwathePrintCount();
    void IgnorePd(DWORD count);
    void EnablePrintMode(bool mode);
    void EnableMaskPrintMode(DWORD mask,bool mode);
    void EnableInternalPD(int value);
    bool printComplete();
    void GenerateProductDetect();
    void setNewConfigFile(QString filename);
    bool getHeadConfig(int head);
    DWORD getHeadStatus(int head);
    DWORD getHeadType(int head);
    bool bootHead(int head);
    bool powerControlHeads(char mask, bool enable);
    void setControlHead(int head);
    void SelectHeadToPrint(int head);
    void setVOffset(int head,int row,DWORD value);
    DWORD getVOffset(int head,int row);
    void setInternalEncoder(BYTE val);
    void setInternalEncoderValue(DWORD val);
    //void ExitContinuousPrintMode();

private:
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
    char AuxPalette[8];
    DWORD m_PosMemSwathe;
    DWORD m_SwatheToPrint;
    int m_ControlHead;

    int m_selectedHead;
    int m_numHeads;



signals:
    void finished();
    void error(unsigned char id,QString err);

public slots:
    void process();

};

#endif
