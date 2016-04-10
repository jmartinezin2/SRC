//****************************************************************************
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
// KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
// PURPOSE.
//
// Copyright (C) 2004  XaarJet Ltd.  All Rights Reserved.
//
//****************************************************************************
//---------------------------------------------------------------------------------------------
//
// Printparameters.cpp : Defines the class behaviors for the stored parameters.
//
//  This file contains the code for loading and storing the parameters from/to the.
//  registry
//
//	Author :	Yvette Reynolds
//	Edited		Who		When		What
//
//--------------------------------------------------------------------------------------------


#include "globals.h"
#include "printparameters.h"
#include "inkjetwidgets.h"
#include <QXmlReader>
#include <QDomDocument>
#include <QObject>
#include <QDebug>

//***********************************************************************
// Function: CPrintParameters - constructor
//
// Purpose: Loads the data from the registry when an instance is created
//
// Parameters: none
//
//
//***********************************************************************
CPrintParameters::CPrintParameters(QObject *parent,DWORD SelectedCard, QString Key)
 : QObject(parent)
{
    loadConfigFile(SelectedCard,Key);

}
/** Carga el fichero*/
void CPrintParameters::loadConfigFile(DWORD SelectedCard, QString Key){
    int dummy_int;

    QDomDocument doc("mydocument");
    QString name=Key;
    if (name.isEmpty()) name=QString("c:/xaar/XUSB.xml");
    QFile file(name);

    if (!file.open(QIODevice::ReadOnly))
             return;

    if (!doc.setContent(&file))
    {
    file.close();
             return;
    }
    file.close();
    QDomElement docElem = doc.documentElement();
    QDomNodeList nodeList;
    bool bOk = false;

    nodeList = docElem.elementsByTagName("Common").at(0).toElement().elementsByTagName("EncoderDivide");
    m_uiEncoderDivide = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    qDebug() << "EncoderDivide: " << m_uiEncoderDivide;

    nodeList = docElem.elementsByTagName("Common").at(0).toElement().elementsByTagName("SubPixelDivide");
    m_uiSubPixelDivide = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    qDebug() << "SubPixelDivide: " << m_uiSubPixelDivide;

    nodeList = docElem.elementsByTagName("Common").at(0).toElement().elementsByTagName("GreyLevels");
    m_uiGreyLevels = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    qDebug()<< "Grey levels: " << m_uiGreyLevels;

    nodeList = docElem.elementsByTagName("Common").at(0).toElement().elementsByTagName("RemapPalette");
    dummy_int = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    if (dummy_int==0) m_bPaletteRemap=false; else m_bPaletteRemap=true;
    qDebug()<< "RemapPalette: " << m_bPaletteRemap;

    nodeList = docElem.elementsByTagName("Common").at(0).toElement().elementsByTagName("GreyScaleBackgroundLevel");
    m_GSBackgroundValue = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    qDebug()<< "GreyScaleBackgroundLevel: " << m_GSBackgroundValue;

    nodeList = docElem.elementsByTagName("Common").at(0).toElement().elementsByTagName("CopyCount");
    m_CopyCount[0] = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    m_CopyCount[1] = m_CopyCount[0];
    qDebug()<< "CopyCount: " << m_CopyCount[0];

    nodeList = docElem.elementsByTagName("Common").at(0).toElement().elementsByTagName("BinaryRemapPalette");
    dummy_int = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    if (dummy_int==0) m_bBinaryPaletteRemap=false; else m_bBinaryPaletteRemap=true;
    qDebug()<< "BinaryRemapPalette: " << m_bBinaryPaletteRemap;

    nodeList = docElem.elementsByTagName("Common").at(0).toElement().elementsByTagName("BinaryBackgroundRemap");
    dummy_int = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    if (dummy_int==0) m_bBinaryPaletteBackground=false; else m_bBinaryPaletteBackground=true;
    qDebug()<< "BinaryBackgroundRemap: " << m_bBinaryPaletteBackground;

    nodeList = docElem.elementsByTagName("Common").at(0).toElement().elementsByTagName("ReverseSwathe");
    dummy_int = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    if (dummy_int==0) m_bReverseSwatheOrder=false; else m_bReverseSwatheOrder=true;
    qDebug()<< "ReverseSwathe: " << m_bReverseSwatheOrder;

    nodeList = docElem.elementsByTagName("Common").at(0).toElement().elementsByTagName("ReversePrint");
    dummy_int = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    if (dummy_int==0) m_ReverseImageOrder=false; else m_ReverseImageOrder=true;
    qDebug()<< "ReversePrint: " << m_ReverseImageOrder;

    nodeList = docElem.elementsByTagName("Common").at(0).toElement().elementsByTagName("ForceBinary1GreyScale2");
    m_ForceMode = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    qDebug()<< "ForceBinary1GreyScale2: " << m_ForceMode;

    nodeList = docElem.elementsByTagName("Common").at(0).toElement().elementsByTagName("GuardValue");
    m_GuardValue = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    qDebug()<< "GuardValue: " << m_GuardValue;

    nodeList = docElem.elementsByTagName("Common").at(0).toElement().elementsByTagName("Enable2bit");
    dummy_int = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    if (dummy_int==0) m_bEnable2Bit=false; else m_bEnable2Bit=true;
    qDebug()<< "Enable2bit: " << m_bEnable2Bit;

    nodeList = docElem.elementsByTagName("Common").at(0).toElement().elementsByTagName("EncoderPulseMultiply");
    m_uiEncoderMultiply = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    qDebug()<< "EncoderPulseMultiply: " << m_uiEncoderMultiply;

    nodeList = docElem.elementsByTagName("Common").at(0).toElement().elementsByTagName("RequiredDPI");
    m_RequiredDPI = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    qDebug()<< "RequiredDPI: " << m_RequiredDPI;

    nodeList = docElem.elementsByTagName("Common").at(0).toElement().elementsByTagName("QuadRequired");
    m_QuadRequired = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    qDebug()<< "QuadRequired: " << m_QuadRequired;

    nodeList = docElem.elementsByTagName("Common").at(0).toElement().elementsByTagName("PreScalarMultplyRequired");
    m_PreScalarMultplyRequired = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    qDebug()<< "PreScalarMultplyRequired: " << m_PreScalarMultplyRequired;

    nodeList = docElem.elementsByTagName("Common").at(0).toElement().elementsByTagName("EncoderResolution");
    m_EncoderResolution = nodeList.at(0).attributes().namedItem("value").nodeValue().toFloat(&bOk);
    qDebug()<< "EncoderResolution: " << m_EncoderResolution;

    nodeList = docElem.elementsByTagName("Common").at(0).toElement().elementsByTagName("TransportSpeed");
    m_TransportSpeed = nodeList.at(0).attributes().namedItem("value").nodeValue().toFloat(&bOk);
    qDebug()<< "TransportSpeed: " << m_TransportSpeed;

    nodeList = docElem.elementsByTagName("Common").at(0).toElement().elementsByTagName("SelectedCard");
    m_SelectedCard = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    qDebug()<< "SelectedCard: " << m_SelectedCard;



    nodeList = docElem.elementsByTagName("Configuration").at(0).toElement().elementsByTagName("DDFSValue");
    m_DDFSValue = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    qDebug()<< "DDFSValue: " << m_DDFSValue;

    nodeList = docElem.elementsByTagName("Configuration").at(0).toElement().elementsByTagName("PrintType");
    m_PrintType = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    qDebug()<< "PrintType: " << m_PrintType;

    nodeList = docElem.elementsByTagName("Configuration").at(0).toElement().elementsByTagName("PrintOnce");
    m_PrintOnce = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    qDebug()<< "PrintOnce: " << m_PrintOnce;

    nodeList = docElem.elementsByTagName("Configuration").at(0).toElement().elementsByTagName("ClockType");
    m_ClockType = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    qDebug()<< "ClockType: " << m_ClockType;

    nodeList = docElem.elementsByTagName("Configuration").at(0).toElement().elementsByTagName("PrintMode");
    m_PrintMode = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    qDebug()<< "PrintMode: " << m_PrintMode;

    nodeList = docElem.elementsByTagName("Configuration").at(0).toElement().elementsByTagName("PrintTransportMode");
    m_PrintTransportMode = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    qDebug()<< "PrintTransportMode: " << m_PrintTransportMode;

    nodeList = docElem.elementsByTagName("Configuration").at(0).toElement().elementsByTagName("SequenceStart");
    m_SequenceStart = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    qDebug()<< "SequenceStart: " << m_SequenceStart;

    nodeList = docElem.elementsByTagName("Configuration").at(0).toElement().elementsByTagName("SysClock");
    m_SysClock = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    qDebug()<< "SysClock: " << m_SysClock;

    nodeList = docElem.elementsByTagName("Configuration").at(0).toElement().elementsByTagName("SeparateRows");
    m_SeparateRows = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    qDebug()<< "SeparateRows: " << m_SeparateRows;

    nodeList = docElem.elementsByTagName("Configuration").at(0).toElement().elementsByTagName("PDPrintCount");
    m_SWTestCounts = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    qDebug()<< "PDPrintCount: " << m_SWTestCounts;

    nodeList = docElem.elementsByTagName("Configuration").at(0).toElement().elementsByTagName("PDSWDelay");
    m_Delay = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    qDebug()<< "PDSWDelay: " << m_Delay;

    nodeList = docElem.elementsByTagName("Configuration").at(0).toElement().elementsByTagName("IgnoreReversePDforPrint");
    m_IgnoreReversePD = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    qDebug()<< "IgnoreReversePDforPrint: " << m_IgnoreReversePD;

    nodeList = docElem.elementsByTagName("Configuration").at(0).toElement().elementsByTagName("BiDirectionKeepOdd");
    m_BiPrintKeepOdd = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    qDebug()<< "BiDirectionKeepOdd: " << m_BiPrintKeepOdd;

    nodeList = docElem.elementsByTagName("Configuration").at(0).toElement().elementsByTagName("SEPDState");
    m_SEPDSetup = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    qDebug()<< "SEPDState: " << m_SEPDSetup;

    nodeList = docElem.elementsByTagName("Configuration").at(0).toElement().elementsByTagName("EncoderControl");
    m_EncoderControl = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    qDebug()<< "EncoderControl: " << m_EncoderControl;

    nodeList = docElem.elementsByTagName("Configuration").at(0).toElement().elementsByTagName("PDDiscardCount");
    m_PDDiscardCount = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    qDebug()<< "PDDiscardCount: " << m_PDDiscardCount;

    nodeList = docElem.elementsByTagName("Configuration").at(0).toElement().elementsByTagName(QString("PowerAll%1").arg( m_SelectedCard + 1));
    m_AllPowerSelected = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    qDebug()<< QString("PowerAll%1").arg( m_SelectedCard + 1) << m_AllPowerSelected;


    for (UINT uLoop = 0; uLoop < m_uiGreyLevels; uLoop++)
    {
        nodeList = docElem.elementsByTagName("HeadOffsetRegisters").at(0).toElement().elementsByTagName(QString("PaletteRemap%1").arg( uLoop + 1));
        m_PaletteValue[uLoop] = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
        qDebug() << QString("HeadOffsetRegisters PaletteRemap%1: ").arg( uLoop + 1) << m_PaletteValue[uLoop];
    }


    if (SelectedCard == XaarScorpionGetXUSBCount() + 1) SelectedCard = 0;

    nodeList = docElem.elementsByTagName(QString("HeadOffsetRegistersCard%1").arg(SelectedCard + 1)).at(0).toElement().elementsByTagName("CardSelected");
    dummy_int = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    if (dummy_int==0) m_bSelectedCard=false; else m_bSelectedCard=true;
    qDebug()<< QString("HeadOffsetRegistersCard%1 CardSelected: ").arg(SelectedCard + 1) << m_bSelectedCard;

    nodeList = docElem.elementsByTagName(QString("HeadOffsetRegistersCard%1").arg(SelectedCard + 1)).at(0).toElement().elementsByTagName("GlobalOffset");
    m_GlobalOffset = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    qDebug()<< QString("HeadOffsetRegistersCard%1 GlobalOffset: ").arg(SelectedCard + 1) << m_GlobalOffset;

    nodeList = docElem.elementsByTagName(QString("HeadOffsetRegistersCard%1").arg(SelectedCard + 1)).at(0).toElement().elementsByTagName("SwatheSplitWanted");
    dummy_int = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    if (dummy_int==0) m_SwatheSplitWanted=false; else m_SwatheSplitWanted=true;
    qDebug()<< QString("HeadOffsetRegistersCard%1 SwatheSplitWanted: ").arg(SelectedCard + 1) << m_SwatheSplitWanted;

    for (UINT uLoop = 0; uLoop<MAXHEADS; uLoop++)
    {
        for (DWORD OffsetDir = 0; OffsetDir < 2; OffsetDir++)
        {
            for (DWORD uRow = 0; uRow<2; uRow++)
            {
                QString dummy;

                if (OffsetDir == 0) dummy=QString("Forward");
                else dummy=QString("Reverse");

                nodeList = docElem.elementsByTagName(QString("HeadOffsetRegistersCard%1").arg(SelectedCard + 1)).at(0).toElement().elementsByTagName(QString("ProductDetect%1Head%2Row%3").arg(dummy).arg(uLoop+1).arg(uRow+1));
                m_uiProductOffset[OffsetDir][uLoop][uRow] = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
                qDebug()<< QString("HeadOffsetRegistersCard%1 ").arg(SelectedCard + 1) << QString("ProductDetect%1Head%2Row%3: ").arg(dummy).arg(uLoop+1).arg(uRow+1) << m_uiProductOffset[OffsetDir][uLoop][uRow];

                // only want to add global offset to forward offset
                if (OffsetDir == 0) m_uiProductOffset[OffsetDir][uLoop][uRow] += m_GlobalOffset;

                nodeList = docElem.elementsByTagName(QString("HeadOffsetRegistersCard%1").arg(SelectedCard + 1)).at(0).toElement().elementsByTagName(QString("SubPixelDelay%1Head%2Row%3").arg(dummy).arg(uLoop+1).arg(uRow+1));
                m_uiSubPixelDelay[OffsetDir][uLoop][uRow] = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
                qDebug()<< QString("HeadOffsetRegistersCard%1 ").arg(SelectedCard + 1) << QString("SubPixelDelay%1Head%2Row%3: ").arg(dummy).arg(uLoop+1).arg(uRow+1) << m_uiSubPixelDelay[OffsetDir][uLoop][uRow];
            }
        }
        for (DWORD Row = 0; Row < MAXROWS; Row++)
        {
            nodeList = docElem.elementsByTagName(QString("HeadOffsetRegistersCard%1").arg(SelectedCard + 1)).at(0).toElement().elementsByTagName(QString("DirTrueHead%1Row%2").arg(uLoop + 1).arg(Row+1));
            dummy_int = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
            if (dummy_int==0) m_bDirTrue[uLoop][Row]=false; else m_bDirTrue[uLoop][Row]=true;
            qDebug()<< QString("HeadOffsetRegistersCard%1 ").arg(SelectedCard + 1) << QString("DirTrueHead%1Row%2: ").arg(uLoop+1).arg(Row+1) << m_bDirTrue[uLoop][Row];

            nodeList = docElem.elementsByTagName(QString("HeadOffsetRegistersCard%1").arg(SelectedCard + 1)).at(0).toElement().elementsByTagName(QString("MirrorHead%1Row%2").arg(uLoop + 1).arg(Row+1));
            dummy_int = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
            if (dummy_int==0) m_bMirror[uLoop][Row]=false; else m_bMirror[uLoop][Row]=true;
            qDebug()<< QString("HeadOffsetRegistersCard%1 ").arg(SelectedCard + 1) << QString("MirrorHead%1Row%2: ").arg(uLoop+1).arg(Row+1) << m_bMirror[uLoop][Row];

            nodeList = docElem.elementsByTagName(QString("HeadOffsetRegistersCard%1").arg(SelectedCard + 1)).at(0).toElement().elementsByTagName(QString("Head%1Row%2SelectedforPrint").arg(uLoop + 1).arg(Row+1));
            dummy_int = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
            if (dummy_int==0) m_bSelectHead[uLoop][Row]=false; else m_bSelectHead[uLoop][Row]=true;
            qDebug()<< QString("HeadOffsetRegistersCard%1 ").arg(SelectedCard + 1) << QString("Head%1Row%2SelectedforPrint: ").arg(uLoop+1).arg(Row+1) << m_bSelectHead[uLoop][Row];

            nodeList = docElem.elementsByTagName(QString("HeadOffsetRegistersCard%1").arg(SelectedCard + 1)).at(0).toElement().elementsByTagName(QString("ChannelOffsetStartHead%1Row%2").arg(uLoop + 1).arg(Row+1));
            m_RowLeadChannels[uLoop][Row] = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
            qDebug()<< QString("HeadOffsetRegistersCard%1 ").arg(SelectedCard + 1) << QString("ChannelOffsetStartHead%1Row%2: ").arg(uLoop+1).arg(Row+1) << m_RowLeadChannels[uLoop][Row];

            nodeList = docElem.elementsByTagName(QString("HeadOffsetRegistersCard%1").arg(SelectedCard + 1)).at(0).toElement().elementsByTagName(QString("ChannelOffsetStartHead%1Row%2").arg(uLoop + 1).arg(Row+1));
            m_RowTrailChannels[uLoop][Row] = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
            qDebug()<< QString("HeadOffsetRegistersCard%1 ").arg(SelectedCard + 1) << QString("ChannelOffsetStartHead%1Row%2: ").arg(uLoop+1).arg(Row+1) << m_RowTrailChannels[uLoop][Row];

            nodeList = docElem.elementsByTagName(QString("HeadOffsetRegistersCard%1").arg(SelectedCard + 1)).at(0).toElement().elementsByTagName(QString("Head%1Row%2SelectedforPrint").arg(uLoop + 1).arg(Row+1));
            m_HeadSelected[uLoop][Row] = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
            qDebug()<< QString("HeadOffsetRegistersCard%1 ").arg(SelectedCard + 1) << QString("Head%1Row%2SelectedforPrint: ").arg(uLoop+1).arg(Row+1) << m_HeadSelected[uLoop][Row];

            if (uLoop == 0)				// ie 1st head
            {
                nodeList = docElem.elementsByTagName(QString("HeadOffsetRegistersCard%1").arg(SelectedCard + 1)).at(0).toElement().elementsByTagName(QString("HeadAllRow%1SelectedforPrint").arg(Row+1));
                m_AllHeadSelected[Row] = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
                qDebug()<< QString("HeadOffsetRegistersCard%1 ").arg(SelectedCard + 1) << QString("HeadAllRow%1SelectedforPrint: ").arg(Row+1) << m_AllHeadSelected[Row];

            }
        }

        nodeList = docElem.elementsByTagName(QString("HeadOffsetRegistersCard%1").arg(SelectedCard + 1)).at(0).toElement().elementsByTagName(QString("SwatheStartHead%1").arg(uLoop+1));
        m_SwatheStartIndex[uLoop] = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
        qDebug()<< QString("HeadOffsetRegistersCard%1 ").arg(SelectedCard + 1) << QString("SwatheStartHead%1: ").arg(uLoop+1) << m_SwatheStartIndex[uLoop];

        nodeList = docElem.elementsByTagName(QString("HeadOffsetRegistersCard%1").arg(SelectedCard + 1)).at(0).toElement().elementsByTagName(QString("SwatheAddHead%1").arg(uLoop+1));
        m_SwatheIncrement[uLoop] = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
        qDebug()<< QString("HeadOffsetRegistersCard%1 ").arg(SelectedCard + 1) << QString("SwatheAddHead%1: ").arg(uLoop+1) << m_SwatheIncrement[uLoop];

        nodeList = docElem.elementsByTagName(QString("HeadOffsetRegistersCard%1").arg(SelectedCard + 1)).at(0).toElement().elementsByTagName(QString("ChannelOffsetStartHead%1").arg(uLoop+1));
        m_StartExtra[uLoop] = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
        qDebug()<< QString("HeadOffsetRegistersCard%1 ").arg(SelectedCard + 1) << QString("ChannelOffsetStartHead%1: ").arg(uLoop+1) << m_StartExtra[uLoop];

        nodeList = docElem.elementsByTagName(QString("HeadOffsetRegistersCard%1").arg(SelectedCard + 1)).at(0).toElement().elementsByTagName(QString("ChannelOffsetEndHead%1").arg(uLoop+1));
        m_EndExtra[uLoop] = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
        qDebug()<< QString("HeadOffsetRegistersCard%1 ").arg(SelectedCard + 1) << QString("ChannelOffsetEndHead%1: ").arg(uLoop+1) << m_EndExtra[uLoop];

    }
    nodeList = docElem.elementsByTagName(QString("HeadOffsetRegistersCard%1").arg(SelectedCard + 1)).at(0).toElement().elementsByTagName("SwatheSplitWanted");
    dummy_int = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    if (dummy_int==0) m_SwatheSplitWanted=false; else m_SwatheSplitWanted=true;
    qDebug()<< QString("HeadOffsetRegistersCard%1 ").arg(SelectedCard + 1) << QString("SwatheSplitWanted: ") << m_SwatheSplitWanted;


    DWORD Cycles = (m_ClockType == CYCLEMODE ? 3:1);
    qDebug()<< "Cycles: "<< Cycles;

    nodeList = docElem.elementsByTagName("AbsoluteEncoder").at(0).toElement().elementsByTagName("ForwardProductDetectTrigger");
    m_ForwardPDTrigger = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    m_ForwardPDTrigger *= Cycles;
    qDebug()<< "ForwardProductDetectTrigger: " << m_ForwardPDTrigger;

    nodeList = docElem.elementsByTagName("AbsoluteEncoder").at(0).toElement().elementsByTagName("ReverseProductDetectTrigger");
    m_ReversePDTrigger = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    m_ReversePDTrigger *= Cycles;
    qDebug()<< "ReverseProductDetectTrigger: " << m_ReversePDTrigger;

    nodeList = docElem.elementsByTagName("AbsoluteEncoder").at(0).toElement().elementsByTagName("PreloadValue");
    m_PreloadValue = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    m_PreloadValue *= Cycles;
    qDebug()<< "PreloadValue: " << m_PreloadValue;

    nodeList = docElem.elementsByTagName("AbsoluteEncoder").at(0).toElement().elementsByTagName("ReloadOnExternalProductDetect");
    m_ReloadOnExternalPD = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    qDebug()<< "ReloadOnExternalProductDetect: " << m_ReloadOnExternalPD;

    nodeList = docElem.elementsByTagName("AbsoluteEncoder").at(0).toElement().elementsByTagName("Invert");
    m_Invert = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    qDebug()<< "Invert: " << m_Invert;

    nodeList = docElem.elementsByTagName("AbsoluteEncoder").at(0).toElement().elementsByTagName("PDGeneration");
    m_PDGeneration = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    qDebug()<< "PDGeneration: " << m_PDGeneration;


    nodeList = docElem.elementsByTagName("SpitData").at(0).toElement().elementsByTagName("IdleSpitFrequency");
    m_IdleSpitFrequency = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    qDebug()<< "IdleSpitFrequency: " << m_IdleSpitFrequency;

    nodeList = docElem.elementsByTagName("SpitData").at(0).toElement().elementsByTagName("IdleDPD");
    m_IdleGreyLevel = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    qDebug()<< "IdleDPD: " << m_IdleGreyLevel;

    nodeList = docElem.elementsByTagName("SpitData").at(0).toElement().elementsByTagName("IdleSpitActive");
    m_Active = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    qDebug()<< "IdleSpitActive: " << m_Active;

    nodeList = docElem.elementsByTagName("SpitData").at(0).toElement().elementsByTagName("IdleSpitLength");
    m_IdleSpitLength = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    qDebug()<< "IdleSpitLength: " << m_IdleSpitLength;

    nodeList = docElem.elementsByTagName("SpitData").at(0).toElement().elementsByTagName("UniDirectionSpitLength");
    m_UniSpitLength = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    qDebug()<< "UniDirectionSpitLength: " << m_UniSpitLength;


    nodeList = docElem.elementsByTagName("ErrorDiffusion").at(0).toElement().elementsByTagName("ScreeningNoise");
    m_NoiseValue = nodeList.at(0).attributes().namedItem("value").nodeValue().toFloat(&bOk);
    qDebug()<< "ScreeningNoise: " << m_NoiseValue;

    nodeList = docElem.elementsByTagName("ErrorDiffusion").at(0).toElement().elementsByTagName("RandomInitialise");
    m_Random = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    qDebug()<< "RandomInitialise: " << m_Random;

    nodeList = docElem.elementsByTagName("ErrorDiffusion").at(0).toElement().elementsByTagName("Bustrophedon");
    m_Bust = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    qDebug()<< "Bustrophedon: " << m_Bust;

    nodeList = docElem.elementsByTagName("ErrorDiffusion").at(0).toElement().elementsByTagName("RatioA");
    m_RatioValue[0] = nodeList.at(0).attributes().namedItem("value").nodeValue().toFloat(&bOk);
    qDebug()<< "RatioA: " << m_RatioValue[0];

    nodeList = docElem.elementsByTagName("ErrorDiffusion").at(0).toElement().elementsByTagName("RatioB");
    m_RatioValue[1] = nodeList.at(0).attributes().namedItem("value").nodeValue().toFloat(&bOk);
    qDebug()<< "RatioB: " << m_RatioValue[1];

    nodeList = docElem.elementsByTagName("ErrorDiffusion").at(0).toElement().elementsByTagName("RatioC");
    m_RatioValue[2] = nodeList.at(0).attributes().namedItem("value").nodeValue().toFloat(&bOk);
    qDebug()<< "RatioC: " << m_RatioValue[2];

    nodeList = docElem.elementsByTagName("ErrorDiffusion").at(0).toElement().elementsByTagName("RatioD");
    m_RatioValue[3] = nodeList.at(0).attributes().namedItem("value").nodeValue().toFloat(&bOk);
    qDebug()<< "RatioD: " << m_RatioValue[3];

    nodeList = docElem.elementsByTagName("ErrorDiffusion").at(0).toElement().elementsByTagName("Enabled");
    m_Enabled = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    qDebug()<< "Enabled: " << m_Enabled;

    nodeList = docElem.elementsByTagName("ErrorDiffusion").at(0).toElement().elementsByTagName("ActiveLevelMap");
    m_LevelMap = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
    qDebug()<< "ActiveLevelMap: " << m_LevelMap;

    m_Levels = m_uiGreyLevels - 1;

    for (UINT uLoop = 0; uLoop < m_Levels + 1; uLoop++)
    {
        nodeList = docElem.elementsByTagName("ErrorDiffusion").at(0).toElement().elementsByTagName(QString("ActiveLevel%1").arg(uLoop));
        m_ActiveLevel[uLoop] = nodeList.at(0).attributes().namedItem("value").nodeValue().toInt(&bOk,10);
        qDebug()<< QString("ActiveLevel%1: ").arg(uLoop) << m_ActiveLevel[uLoop];
    }
}


/** Carga el fichero*/
void CPrintParameters::saveConfigFile(DWORD SelectedCard,QString Key){
    int dummy_int;
    QDomDocument doc("mydocument");
    QString name=Key;
    if (name.isEmpty()) name=QString("c:/xaar/XUSB.xml");
    QFile file(name);

    if (!file.open(QIODevice::ReadOnly))
             return;

    if (!doc.setContent(&file))
    {
    file.close();
             return;
    }
    file.close();
    QDomElement docElem = doc.documentElement();
    QDomNodeList nodeList;


    nodeList = docElem.elementsByTagName("Common").at(0).toElement().elementsByTagName("EncoderDivide");
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_uiEncoderDivide));

    nodeList = docElem.elementsByTagName("Common").at(0).toElement().elementsByTagName("SubPixelDivide");
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_uiSubPixelDivide));

    nodeList = docElem.elementsByTagName("Common").at(0).toElement().elementsByTagName("GreyLevels");
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_uiGreyLevels));

    nodeList = docElem.elementsByTagName("Common").at(0).toElement().elementsByTagName("RemapPalette");
    dummy_int=(m_bPaletteRemap)?1:0;
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(dummy_int));


    nodeList = docElem.elementsByTagName("Common").at(0).toElement().elementsByTagName("GreyScaleBackgroundLevel");
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_GSBackgroundValue));

    nodeList = docElem.elementsByTagName("Common").at(0).toElement().elementsByTagName("CopyCount");
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_CopyCount[0]));

    nodeList = docElem.elementsByTagName("Common").at(0).toElement().elementsByTagName("BinaryRemapPalette");
    dummy_int=(m_bBinaryPaletteRemap)?1:0;
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(dummy_int));

    nodeList = docElem.elementsByTagName("Common").at(0).toElement().elementsByTagName("BinaryBackgroundRemap");
    dummy_int=(m_bBinaryPaletteBackground)?1:0;
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(dummy_int));

    nodeList = docElem.elementsByTagName("Common").at(0).toElement().elementsByTagName("ReverseSwathe");
    dummy_int=(m_bReverseSwatheOrder)?1:0;
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(dummy_int));

    nodeList = docElem.elementsByTagName("Common").at(0).toElement().elementsByTagName("ReversePrint");
    dummy_int=(m_ReverseImageOrder)?1:0;
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(dummy_int));


    nodeList = docElem.elementsByTagName("Common").at(0).toElement().elementsByTagName("ForceBinary1GreyScale2");
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_ForceMode));


    nodeList = docElem.elementsByTagName("Common").at(0).toElement().elementsByTagName("GuardValue");
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_GuardValue));


    nodeList = docElem.elementsByTagName("Common").at(0).toElement().elementsByTagName("Enable2bit");
    dummy_int=(m_bEnable2Bit)?1:0;
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(dummy_int));

    nodeList = docElem.elementsByTagName("Common").at(0).toElement().elementsByTagName("EncoderPulseMultiply");
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_uiEncoderMultiply));

    nodeList = docElem.elementsByTagName("Common").at(0).toElement().elementsByTagName("RequiredDPI");
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_RequiredDPI));

    nodeList = docElem.elementsByTagName("Common").at(0).toElement().elementsByTagName("QuadRequired");
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_QuadRequired));

    nodeList = docElem.elementsByTagName("Common").at(0).toElement().elementsByTagName("PreScalarMultplyRequired");
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_PreScalarMultplyRequired));

    nodeList = docElem.elementsByTagName("Common").at(0).toElement().elementsByTagName("EncoderResolution");
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_EncoderResolution));

    nodeList = docElem.elementsByTagName("Common").at(0).toElement().elementsByTagName("TransportSpeed");
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_TransportSpeed));

    nodeList = docElem.elementsByTagName("Common").at(0).toElement().elementsByTagName("SelectedCard");
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_SelectedCard));

    nodeList = docElem.elementsByTagName("Configuration").at(0).toElement().elementsByTagName("DDFSValue");
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_DDFSValue));

    nodeList = docElem.elementsByTagName("Configuration").at(0).toElement().elementsByTagName("PrintType");
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_PrintType));

    nodeList = docElem.elementsByTagName("Configuration").at(0).toElement().elementsByTagName("PrintOnce");
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_PrintOnce));

    nodeList = docElem.elementsByTagName("Configuration").at(0).toElement().elementsByTagName("ClockType");
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_ClockType));

    nodeList = docElem.elementsByTagName("Configuration").at(0).toElement().elementsByTagName("PrintMode");
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_PrintMode));

    nodeList = docElem.elementsByTagName("Configuration").at(0).toElement().elementsByTagName("PrintTransportMode");
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_PrintTransportMode));

    nodeList = docElem.elementsByTagName("Configuration").at(0).toElement().elementsByTagName("SequenceStart");
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_SequenceStart));

    nodeList = docElem.elementsByTagName("Configuration").at(0).toElement().elementsByTagName("SysClock");
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_SysClock));

    nodeList = docElem.elementsByTagName("Configuration").at(0).toElement().elementsByTagName("SeparateRows");
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_SeparateRows));

    nodeList = docElem.elementsByTagName("Configuration").at(0).toElement().elementsByTagName("PDPrintCount");
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_SWTestCounts));

    nodeList = docElem.elementsByTagName("Configuration").at(0).toElement().elementsByTagName("PDSWDelay");
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_Delay));

    nodeList = docElem.elementsByTagName("Configuration").at(0).toElement().elementsByTagName("IgnoreReversePDforPrint");
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_IgnoreReversePD));

    nodeList = docElem.elementsByTagName("Configuration").at(0).toElement().elementsByTagName("BiDirectionKeepOdd");
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_BiPrintKeepOdd));

    nodeList = docElem.elementsByTagName("Configuration").at(0).toElement().elementsByTagName("SEPDState");
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_SEPDSetup));

    nodeList = docElem.elementsByTagName("Configuration").at(0).toElement().elementsByTagName("EncoderControl");
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_EncoderControl));

    nodeList = docElem.elementsByTagName("Configuration").at(0).toElement().elementsByTagName("PDDiscardCount");
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_PDDiscardCount));

    nodeList = docElem.elementsByTagName("Configuration").at(0).toElement().elementsByTagName(QString("PowerAll%1").arg( m_SelectedCard + 1));
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_AllPowerSelected));


    for (UINT uLoop = 0; uLoop < m_uiGreyLevels; uLoop++)
    {
        nodeList = docElem.elementsByTagName("HeadOffsetRegisters").at(0).toElement().elementsByTagName(QString("PaletteRemap%1").arg( uLoop + 1));
        nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_PaletteValue[uLoop]));
    }


    if (SelectedCard == XaarScorpionGetXUSBCount() + 1) SelectedCard = 0;

    nodeList = docElem.elementsByTagName(QString("HeadOffsetRegistersCard%1").arg(SelectedCard + 1)).at(0).toElement().elementsByTagName("CardSelected");
    dummy_int=(m_bSelectedCard)?1:0;
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(dummy_int));


    nodeList = docElem.elementsByTagName(QString("HeadOffsetRegistersCard%1").arg(SelectedCard + 1)).at(0).toElement().elementsByTagName("GlobalOffset");
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_GlobalOffset));


    nodeList = docElem.elementsByTagName(QString("HeadOffsetRegistersCard%1").arg(SelectedCard + 1)).at(0).toElement().elementsByTagName("SwatheSplitWanted");
    dummy_int=(m_SwatheSplitWanted)?1:0;
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(dummy_int));

    for (UINT uLoop = 0; uLoop<MAXHEADS; uLoop++)
    {
        for (DWORD OffsetDir = 0; OffsetDir < 2; OffsetDir++)
        {
            for (DWORD uRow = 0; uRow<2; uRow++)
            {
                QString dummy;

                if (OffsetDir == 0) dummy=QString("Forward");
                else dummy=QString("Reverse");

                nodeList = docElem.elementsByTagName(QString("HeadOffsetRegistersCard%1").arg(SelectedCard + 1)).at(0).toElement().elementsByTagName(QString("ProductDetect%1Head%2Row%3").arg(dummy).arg(uLoop+1).arg(uRow+1));
                nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_uiProductOffset[OffsetDir][uLoop][uRow]));


                // only want to add global offset to forward offset
                if (OffsetDir == 0) m_uiProductOffset[OffsetDir][uLoop][uRow] += m_GlobalOffset;

                nodeList = docElem.elementsByTagName(QString("HeadOffsetRegistersCard%1").arg(SelectedCard + 1)).at(0).toElement().elementsByTagName(QString("SubPixelDelay%1Head%2Row%3").arg(dummy).arg(uLoop+1).arg(uRow+1));
                nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_uiSubPixelDelay[OffsetDir][uLoop][uRow]));

            }
        }
        for (DWORD Row = 0; Row < MAXROWS; Row++)
        {

            nodeList = docElem.elementsByTagName(QString("HeadOffsetRegistersCard%1").arg(SelectedCard + 1)).at(0).toElement().elementsByTagName(QString("DirTrueHead%1Row%2").arg(uLoop + 1).arg(Row+1));
            dummy_int=(m_bDirTrue[uLoop][Row])?1:0;
            nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(dummy_int));


            nodeList = docElem.elementsByTagName(QString("HeadOffsetRegistersCard%1").arg(SelectedCard + 1)).at(0).toElement().elementsByTagName(QString("MirrorHead%1Row%2").arg(uLoop + 1).arg(Row+1));
            dummy_int=(m_bMirror[uLoop][Row])?1:0;
            nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(dummy_int));


            nodeList = docElem.elementsByTagName(QString("HeadOffsetRegistersCard%1").arg(SelectedCard + 1)).at(0).toElement().elementsByTagName(QString("Head%1Row%2SelectedforPrint").arg(uLoop + 1).arg(Row+1));
            dummy_int=(m_bSelectHead[uLoop][Row])?1:0;
            nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(dummy_int));





            nodeList = docElem.elementsByTagName(QString("HeadOffsetRegistersCard%1").arg(SelectedCard + 1)).at(0).toElement().elementsByTagName(QString("ChannelOffsetStartHead%1Row%2").arg(uLoop + 1).arg(Row+1));
            nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_RowLeadChannels[uLoop][Row]));

            nodeList = docElem.elementsByTagName(QString("HeadOffsetRegistersCard%1").arg(SelectedCard + 1)).at(0).toElement().elementsByTagName(QString("ChannelOffsetStartHead%1Row%2").arg(uLoop + 1).arg(Row+1));
            nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_RowTrailChannels[uLoop][Row]));


            nodeList = docElem.elementsByTagName(QString("HeadOffsetRegistersCard%1").arg(SelectedCard + 1)).at(0).toElement().elementsByTagName(QString("Head%1Row%2SelectedforPrint").arg(uLoop + 1).arg(Row+1));
            nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_HeadSelected[uLoop][Row]));


            if (uLoop == 0)				// ie 1st head
            {
                nodeList = docElem.elementsByTagName(QString("HeadOffsetRegistersCard%1").arg(SelectedCard + 1)).at(0).toElement().elementsByTagName(QString("HeadAllRow%1SelectedforPrint").arg(Row+1));
                nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_AllHeadSelected[Row]));
            }
        }

        nodeList = docElem.elementsByTagName(QString("HeadOffsetRegistersCard%1").arg(SelectedCard + 1)).at(0).toElement().elementsByTagName(QString("SwatheStartHead%1").arg(uLoop+1));
        nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_SwatheStartIndex[uLoop]));


        nodeList = docElem.elementsByTagName(QString("HeadOffsetRegistersCard%1").arg(SelectedCard + 1)).at(0).toElement().elementsByTagName(QString("SwatheAddHead%1").arg(uLoop+1));
        nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_SwatheIncrement[uLoop]));

        nodeList = docElem.elementsByTagName(QString("HeadOffsetRegistersCard%1").arg(SelectedCard + 1)).at(0).toElement().elementsByTagName(QString("ChannelOffsetStartHead%1").arg(uLoop+1));
        nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_StartExtra[uLoop]));

        nodeList = docElem.elementsByTagName(QString("HeadOffsetRegistersCard%1").arg(SelectedCard + 1)).at(0).toElement().elementsByTagName(QString("ChannelOffsetEndHead%1").arg(uLoop+1));
        nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_EndExtra[uLoop]));

    }


    nodeList = docElem.elementsByTagName(QString("HeadOffsetRegistersCard%1").arg(SelectedCard + 1)).at(0).toElement().elementsByTagName("SwatheSplitWanted");
    dummy_int=(m_SwatheSplitWanted)?1:0;
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(dummy_int));


    DWORD Cycles = (m_ClockType == CYCLEMODE ? 3:1);
    qDebug()<< "Cycles: "<< Cycles;


    nodeList = docElem.elementsByTagName("AbsoluteEncoder").at(0).toElement().elementsByTagName("ForwardProductDetectTrigger");
    dummy_int=m_ForwardPDTrigger/Cycles;
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(dummy_int));


    nodeList = docElem.elementsByTagName("AbsoluteEncoder").at(0).toElement().elementsByTagName("ReverseProductDetectTrigger");
    dummy_int=m_ReversePDTrigger/Cycles;
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(dummy_int));


    nodeList = docElem.elementsByTagName("AbsoluteEncoder").at(0).toElement().elementsByTagName("PreloadValue");
    dummy_int=m_PreloadValue/Cycles;
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(dummy_int));

    nodeList = docElem.elementsByTagName("AbsoluteEncoder").at(0).toElement().elementsByTagName("ReloadOnExternalProductDetect");
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_ReloadOnExternalPD));

    nodeList = docElem.elementsByTagName("AbsoluteEncoder").at(0).toElement().elementsByTagName("Invert");
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_Invert));

    nodeList = docElem.elementsByTagName("AbsoluteEncoder").at(0).toElement().elementsByTagName("PDGeneration");
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_PDGeneration));

    nodeList = docElem.elementsByTagName("SpitData").at(0).toElement().elementsByTagName("IdleSpitFrequency");
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_IdleSpitFrequency));

    nodeList = docElem.elementsByTagName("SpitData").at(0).toElement().elementsByTagName("IdleDPD");
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_IdleGreyLevel));

    nodeList = docElem.elementsByTagName("SpitData").at(0).toElement().elementsByTagName("IdleSpitActive");
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_Active));

    nodeList = docElem.elementsByTagName("SpitData").at(0).toElement().elementsByTagName("IdleSpitLength");
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_IdleSpitLength));

    nodeList = docElem.elementsByTagName("SpitData").at(0).toElement().elementsByTagName("UniDirectionSpitLength");
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_UniSpitLength));


    nodeList = docElem.elementsByTagName("ErrorDiffusion").at(0).toElement().elementsByTagName("ScreeningNoise");
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_NoiseValue));

    nodeList = docElem.elementsByTagName("ErrorDiffusion").at(0).toElement().elementsByTagName("RandomInitialise");
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_Random));

    nodeList = docElem.elementsByTagName("ErrorDiffusion").at(0).toElement().elementsByTagName("Bustrophedon");
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_Bust));

    nodeList = docElem.elementsByTagName("ErrorDiffusion").at(0).toElement().elementsByTagName("RatioA");
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_RatioValue[0]));

    nodeList = docElem.elementsByTagName("ErrorDiffusion").at(0).toElement().elementsByTagName("RatioB");
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_RatioValue[1]));

    nodeList = docElem.elementsByTagName("ErrorDiffusion").at(0).toElement().elementsByTagName("RatioC");
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_RatioValue[2]));

    nodeList = docElem.elementsByTagName("ErrorDiffusion").at(0).toElement().elementsByTagName("RatioD");
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_RatioValue[3]));

    nodeList = docElem.elementsByTagName("ErrorDiffusion").at(0).toElement().elementsByTagName("Enabled");
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_Enabled));

    nodeList = docElem.elementsByTagName("ErrorDiffusion").at(0).toElement().elementsByTagName("ActiveLevelMap");
    nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_LevelMap));

    m_Levels = m_uiGreyLevels - 1;

    for (UINT uLoop = 0; uLoop < m_Levels + 1; uLoop++)
    {
        nodeList = docElem.elementsByTagName("ErrorDiffusion").at(0).toElement().elementsByTagName(QString("ActiveLevel%1").arg(uLoop));
        nodeList.at(0).attributes().namedItem("value").setNodeValue(QString("%1").arg(m_ActiveLevel[uLoop]));
    }
    QString xml=doc.toString();
    file.setFileName("c:/xaar/XUSB1.xml");
    if (!file.open(QIODevice::WriteOnly| QIODevice::Text))
             return;
    file.write(xml.toUtf8().data());
    file.close();

}




// ----------------------------------------------------------------------
//
// copy ctor
// ----------------------------------------------------------------------
CPrintParameters::CPrintParameters( const CPrintParameters &s )
{

    m_uiEncoderDivide = s.m_uiEncoderDivide;
	m_uiSubPixelDivide = s.m_uiSubPixelDivide;
	m_uiGreyLevels = s.m_uiGreyLevels;
	m_bPaletteRemap = s.m_bPaletteRemap;
	m_GSBackgroundValue = s.m_GSBackgroundValue;
	m_CopyCount[0] = s.m_CopyCount[0];
	m_CopyCount[1] = s.m_CopyCount[1];
	m_bBinaryPaletteRemap = s.m_bBinaryPaletteRemap;
	m_bBinaryPaletteBackground = s.m_bBinaryPaletteBackground;
	m_bEnable2Bit = s.m_bEnable2Bit;
	m_bReverseSwatheOrder = s.m_bReverseSwatheOrder;
	m_ReverseImageOrder = s.m_ReverseImageOrder;
	m_uiEncoderMultiply = s.m_uiEncoderMultiply;
	m_RequiredDPI = s.m_RequiredDPI;
	m_QuadRequired = s.m_QuadRequired;
	m_EncoderResolution = s.m_EncoderResolution;
	m_TransportSpeed = s.m_TransportSpeed;
	m_PreScalarMultplyRequired = s.m_PreScalarMultplyRequired;

	m_DDFSValue = s.m_DDFSValue;
	m_PrintType = s.m_PrintType;
	m_PrintOnce = s.m_PrintOnce;
	m_ClockType = s.m_ClockType;
	m_PrintMode = s.m_PrintMode;
	m_PrintTransportMode = s.m_PrintTransportMode;
	m_SequenceStart = s.m_SequenceStart;
	m_SysClock = s.m_SysClock;
	m_IgnoreReversePD = s.m_IgnoreReversePD;
	m_GuardValue = s.m_GuardValue;
	m_BiPrintKeepOdd = s.m_BiPrintKeepOdd;

	for (DWORD uLoop = 0; uLoop < m_uiGreyLevels; uLoop++)
			m_PaletteValue[uLoop] = s.m_PaletteValue[uLoop];

	m_bSelectedCard = s.m_bSelectedCard;
	m_GlobalOffset = s.m_GlobalOffset;
	m_SwatheSplitWanted = s.m_SwatheSplitWanted;
	m_SeparateRows = s.m_SeparateRows;

	for (DWORD Head = 0; Head < MAXHEADS; Head++)
	{
		for (DWORD OffsetDir = 0; OffsetDir < 2; OffsetDir++)
			for (DWORD uRow = 0; uRow<2; uRow++)
			{
				m_uiProductOffset[OffsetDir][Head][uRow] = s.m_uiProductOffset[OffsetDir][Head][uRow];
				m_uiSubPixelDelay[OffsetDir][Head][uRow] = s.m_uiSubPixelDelay[OffsetDir][Head][uRow];
			}
		for (DWORD Row = 0; Row < 2; Row++)
		{
			m_bDirTrue[Head][Row] = s.m_bDirTrue[Head][Row];
			m_bMirror[Head][Row] = s.m_bMirror[Head][Row];
			m_bSelectHead[Head][Row] = s.m_bSelectHead[Head][Row];
			m_RowTrailChannels[Head][Row] = s.m_RowTrailChannels[Head][Row];		// number of unused channels at end of print head per row
			m_RowLeadChannels[Head][Row] = s.m_RowLeadChannels[Head][Row];			// number of unused channels at start of print head per row
		}
		m_SwatheStartIndex[Head] = s.m_SwatheStartIndex[Head];
		m_SwatheIncrement[Head] = s.m_SwatheIncrement[Head];
		m_StartExtra[Head] = s.m_StartExtra[Head];
		m_EndExtra[Head] = s.m_EndExtra[Head];
	}
	m_SWTestCounts = s.m_SWTestCounts;
	m_Delay = s.m_Delay;
	m_SwatheSplitWanted = s.m_SwatheSplitWanted;

	m_ForwardPDTrigger = s.m_ForwardPDTrigger;
	m_ReversePDTrigger = s.m_ReversePDTrigger;
	m_PreloadValue = s.m_PreloadValue;
	m_ReloadOnExternalPD =  s.m_ReverseImageOrder;
	m_Invert = s.m_Invert;
	m_PDGeneration = s.m_PDGeneration;
	m_SEPDSetup = s.m_SEPDSetup;
	m_EncoderControl = s.m_EncoderControl;
	m_PDDiscardCount = s.m_PDDiscardCount;

	m_SelectedCard = s.m_SelectedCard;
	for (DWORD Head = 0; Head < MAXHEADS; Head++)
		for (DWORD Row = 0; Row < MAXROWS; Row++)
		{
			m_HeadSelected[Head][Row] = s.m_HeadSelected[Head][Row];
			if (Head == 0)
				m_AllHeadSelected[Row] = s.m_AllHeadSelected[Row];
		}

	m_AllPowerSelected = s.m_AllPowerSelected;
	m_Active = s.m_Active;
	m_IdleSpitFrequency = s.m_IdleSpitFrequency;
	m_IdleSpitLength = s.m_IdleSpitLength;
	m_UniSpitLength = s.m_UniSpitLength;
	m_IdleGreyLevel = s.m_IdleGreyLevel;
	m_SpitActive = s.m_SpitActive;
	m_Enabled = s.m_Enabled;
	m_Random = s.m_Random;
	m_Bust = s.m_Bust;
	m_LevelMap = s.m_LevelMap;
	for (DWORD Loop = 0; Loop < 4; Loop++)
		m_RatioValue[Loop] = s.m_RatioValue[Loop];
	m_NoiseValue = s.m_NoiseValue;
	for (DWORD Loop = 0; Loop < NUMBERGREYSCALES; Loop++) 
		m_ActiveLevel[Loop] = s.m_ActiveLevel[Loop];
 }


CPrintParameters::~CPrintParameters(){

}

// ----------------------------------------------------------------------
//
// Serialize
// ----------------------------------------------------------------------
/*void CPrintParameters::Serialize( CArchive& ar )
{
	CObject::Serialize( ar );
	m_bSerializeOk = true;
    if (ar.IsStoring())
	{
		ar << m_Marker;
		ar << m_CreatedVersion;
		ar << m_uiEncoderDivide;
		ar << m_uiSubPixelDivide;
		ar << m_uiGreyLevels;
		ar << (int)m_bPaletteRemap;
		ar << m_GSBackgroundValue;
		ar << (int)m_bEnable2Bit;
		ar << (int)m_bBinaryPaletteRemap;
		ar << (int)m_bBinaryPaletteBackground;
		ar << (int)m_bReverseSwatheOrder;
		ar << (int)m_ReverseImageOrder;
		ar << m_uiEncoderMultiply;

		ar << m_DDFSValue;
		ar << m_PrintType;
		ar << m_PrintOnce;
		ar << m_ClockType;
		ar << m_PrintMode;
		ar << m_PrintTransportMode;
		ar << m_SequenceStart;
		ar << m_SysClock;
		ar << m_IgnoreReversePD;
		ar << m_GuardValue;
		ar << m_BiPrintKeepOdd;

		for (DWORD uLoop = 0; uLoop < m_uiGreyLevels; uLoop++)
			ar << m_PaletteValue[uLoop];

		ar << m_bSelectedCard;
		ar << m_GlobalOffset;
		ar << m_SwatheSplitWanted;

		for (DWORD uLoop = 0; uLoop<MAXHEADS; uLoop++)
		{
			for (DWORD OffsetDir = 0; OffsetDir < 2; OffsetDir++)
				for (DWORD uRow = 0; uRow<2; uRow++)
				{
					if (OffsetDir == 0)
						ar << m_uiProductOffset[OffsetDir][uLoop][uRow] - m_GlobalOffset;
					else
						ar << m_uiProductOffset[OffsetDir][uLoop][uRow];
					ar << m_uiSubPixelDelay[OffsetDir][uLoop][uRow];
				}
			for (DWORD Row = 0; Row < 2; Row++)
			{
				ar << m_bDirTrue[uLoop][Row];
				ar << m_bMirror[uLoop][Row];
				ar << m_bSelectHead[uLoop][Row];
			}
			ar << m_SwatheStartIndex[uLoop];
			ar << m_SwatheIncrement[uLoop];
			ar << m_StartExtra[uLoop];
			ar << m_EndExtra[uLoop];
		}
		ar << m_SWTestCounts;
		ar << m_Delay;
		ar << m_SwatheSplitWanted;
		ar << m_SeparateRows;

		ar << m_ForwardPDTrigger;
		ar << m_ReversePDTrigger;
		ar << m_PreloadValue;
		ar << m_ReloadOnExternalPD;
		ar << m_Invert;
		ar << m_PDGeneration;
		ar << m_SEPDSetup;
		ar << m_EncoderControl;

		ar << m_RequiredDPI;
		ar << m_QuadRequired;
		ar << m_EncoderResolution;
		ar << m_TransportSpeed;
		ar << m_PDDiscardCount;
		ar << m_PreScalarMultplyRequired;

		for (DWORD Head = 0; Head < MAXHEADS; Head++)
			for (DWORD Row = 0; Row < MAXROWS; Row++)
			{
				ar << m_RowTrailChannels[Head][Row];
				ar << m_RowLeadChannels[Head][Row];
			}

		ar << m_SelectedCard;
		for (DWORD Head = 0; Head < MAXHEADS; Head++)
			for (DWORD Row = 0; Row < MAXROWS; Row++)
			{
				ar << m_HeadSelected[Head][Row];
				if (Head == 0)
					ar << m_AllHeadSelected[Row];
			}
		ar << m_AllPowerSelected;

		ar << m_Active;
		ar << m_IdleSpitFrequency;
		ar << m_IdleSpitLength;
		ar << m_UniSpitLength;
		ar << m_IdleGreyLevel;
		ar << m_SpitActive;
		ar << m_Enabled;
		ar << m_Random;
		ar << m_Bust;
		ar << m_LevelMap;
		for (DWORD Loop = 0; Loop < 4; Loop++)
            ar << m_RatioValue[Loop];
		ar << m_NoiseValue;
		for (DWORD Loop = 0; Loop < NUMBERGREYSCALES; Loop++) 
			ar << m_ActiveLevel[Loop];
		for (DWORD Row = 0; Row < MAXROWS; Row++)
			ar << m_CopyCount[Row];
	}
    else
	{
		ar >> m_Marker;
		ar >> m_CreatedVersion;
		if (m_Marker == MARKER && m_CreatedVersion >= PRINTPARAMETERVERSION)
		{
			DWORD Value;
			ar >> m_uiEncoderDivide;
			ar >> m_uiSubPixelDivide;
			ar >> m_uiGreyLevels;
			ar >> Value;
			m_bPaletteRemap = Value == 0 ? false:true;
			ar >> m_GSBackgroundValue;
			ar >> Value;
			m_bEnable2Bit = Value == 0 ? false:true;
			ar >> Value;
			m_bBinaryPaletteRemap = Value == 0 ? false:true;
			ar >> Value;
			m_bBinaryPaletteBackground = Value == 0 ? false:true;
			ar >> Value;
			m_bReverseSwatheOrder = Value == 0 ? false:true;
			ar >> Value;
			m_ReverseImageOrder = Value == 0 ? false:true;
			ar >> m_uiEncoderMultiply;

			ar >> m_DDFSValue;
			ar >> m_PrintType;
			ar >> m_PrintOnce;
			ar >> m_ClockType;
			ar >> m_PrintMode;
			ar >> m_PrintTransportMode;
			ar >> m_SequenceStart;
			ar >> m_SysClock;
			ar >> m_IgnoreReversePD;
			ar >> m_GuardValue;
			ar >> m_BiPrintKeepOdd;

			for (DWORD uLoop = 0; uLoop < m_uiGreyLevels; uLoop++)
				ar >> m_PaletteValue[uLoop];

			ar >> m_bSelectedCard;
			ar >> m_GlobalOffset;
			ar >> m_SwatheSplitWanted;

			for (DWORD uLoop = 0; uLoop<MAXHEADS; uLoop++)
			{
				for (DWORD OffsetDir = 0; OffsetDir < 2; OffsetDir++)
					for (DWORD uRow = 0; uRow<2; uRow++)
					{
						ar >> m_uiProductOffset[OffsetDir][uLoop][uRow];
						ar >> m_uiSubPixelDelay[OffsetDir][uLoop][uRow];
					}
				for (DWORD Row = 0; Row < 2; Row++)
				{
					ar >> m_bDirTrue[uLoop][Row];
					ar >> m_bMirror[uLoop][Row];
					ar >> m_bSelectHead[uLoop][Row];
				}
				ar >> m_SwatheStartIndex[uLoop];
				ar >> m_SwatheIncrement[uLoop];
				ar >> m_StartExtra[uLoop];
				ar >> m_EndExtra[uLoop];
			}
			ar >> m_SWTestCounts;
			ar >> m_Delay;
			ar >> m_SwatheSplitWanted;
			ar >> m_SeparateRows;

			ar >> m_ForwardPDTrigger;
			ar >> m_ReversePDTrigger;
			ar >> m_PreloadValue;
			ar >> m_ReloadOnExternalPD;
			ar >> m_Invert;
			ar >> m_PDGeneration;
			ar >> m_SEPDSetup;
			ar >> m_EncoderControl;

			ar >> m_RequiredDPI;
			ar >> m_QuadRequired;
			ar >> m_EncoderResolution;
			ar >> m_TransportSpeed;
			ar >> m_PDDiscardCount;
			ar >> m_PreScalarMultplyRequired;
			if (m_CreatedVersion >= PRINTPARAMETERVERSION1STUPDATE41)
			{
				for (DWORD Head = 0; Head < MAXHEADS; Head++)
					for (DWORD Row = 0; Row < MAXROWS; Row++)
					{
						ar >> m_RowTrailChannels[Head][Row];
						ar >> m_RowLeadChannels[Head][Row];
					}
			}
		
			if (m_CreatedVersion >= PRINTPARAMETERVERSION41)
			{
				ar >> m_SelectedCard;
				for (DWORD Head = 0; Head < MAXHEADS; Head++)
					for (DWORD Row = 0; Row < MAXROWS; Row++)
					{
						ar >> m_HeadSelected[Head][Row];
						if (Head == 0)
							ar >> m_AllHeadSelected[Row];
					}

				ar >> m_AllPowerSelected;
	
				ar >> m_Active;
				ar >> m_IdleSpitFrequency;
				ar >> m_IdleSpitLength;
				ar >> m_UniSpitLength;
				ar >> m_IdleGreyLevel;
				ar >> m_SpitActive;
				ar >> m_Enabled;
				ar >> m_Random;
				ar >> m_Bust;
				ar >> m_LevelMap;
				for (DWORD Loop = 0; Loop < 4; Loop++)
					ar >> m_RatioValue[Loop];
				ar >> m_NoiseValue;
				for (DWORD Loop = 0; Loop < NUMBERGREYSCALES; Loop++) 
					ar >> m_ActiveLevel[Loop];
			}
			if (m_CreatedVersion >= PRINTPARAMETERVERSION42)
			{
				for (DWORD Row = 0; Row < MAXROWS; Row++)
					ar >> m_CopyCount[Row];
			}
		}
#ifdef APPLICATION
		else
		{
			DisplayErrorMessage(IDS_PARAMETERVERSIONERROR, MB_OK);
			m_bSerializeOk = false;
		}
#endif
	}
}
*/
