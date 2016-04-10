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

#ifndef PRINTPARAMETERS_H
#define PRINTPARAMETERS_H

#include <QString>
#include <QWidget>
#include "inkjetwidgets.h"
#include "ScorpionDLL-ext.h"
// PrintParameters.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// PrintParameters class

class CPrintParameters : public QObject
{
    Q_OBJECT// Construction
private:
    QString* m_Key;
public:
    CPrintParameters(QObject *parent = 0,DWORD SelectedCard = 0, QString Key="");	// standard constructor
	CPrintParameters( const CPrintParameters &s );		// copy constructor
    ///Txemari, quito esto que no lo usamos y da mil warnings
    CPrintParameters& operator=( const CPrintParameters &s ) {return *this;} // assignment operator
    void loadConfigFile(DWORD SelectedCard, QString Key);
    void saveConfigFile(DWORD SelectedCard,QString Key);
    ~CPrintParameters();

    //void Serialize( CArchive& archive );
    //bool SerializeOk() {return m_bSerializeOk;}

	DWORD m_CreatedVersion, m_Marker;
	// Common
	DWORD m_uiEncoderDivide, m_uiSubPixelDivide, m_uiEncoderMultiply;
	DWORD m_uiGreyLevels, m_GSBackgroundValue, m_CopyCount[MAXROWS];
	bool m_bPaletteRemap, m_bBinaryPaletteRemap, m_bBinaryPaletteBackground, m_bEnable2Bit;
	bool m_bReverseSwatheOrder, m_ReverseImageOrder;
	BYTE m_PaletteValue[NUMBERGREYSCALES];
	DWORD m_GuardValue, m_BiPrintKeepOdd;
	DWORD m_RequiredDPI, m_QuadRequired, m_PreScalarMultplyRequired;
	float m_EncoderResolution, m_TransportSpeed;

	// Config
	DWORD m_PrintMode;
	DWORD m_DDFSValue;
	DWORD m_PrintType;
	DWORD m_PrintTransportMode;
	DWORD m_SequenceStart;
	DWORD m_SysClock;
	DWORD m_ClockType, m_PrintOnce, m_IgnoreReversePD, m_PDDiscardCount;
	DWORD m_SeparateRows;

	// Head specific
	DWORD m_uiProductOffset[2][MAXHEADS][2], m_uiSubPixelDelay[2][MAXHEADS][2];		// 1st index if to forward or reverse offsets
	DWORD m_GlobalOffset;
	bool m_bDirTrue[MAXHEADS][MAXROWS], m_bMirror[MAXHEADS][MAXROWS];
	bool m_bSelectHead[MAXHEADS][MAXROWS];
	bool m_bSelectedCard;

	// other
	DWORD m_SWTestCounts, m_Delay;
	DWORD m_StartExtra[MAXHEADS], m_EndExtra[MAXHEADS];
	bool m_SwatheSplitWanted;
	DWORD m_SwatheStartIndex[MAXHEADS], m_SwatheIncrement[MAXHEADS];
	DWORD m_RowTrailChannels[MAXHEADS][MAXROWS];				// number of unused channels at end of print head per row
	DWORD m_RowLeadChannels[MAXHEADS][MAXROWS];					// number of unused channels at start of print head per row

	BYTE m_ForceMode;

	// Absolute encoder
	DWORD m_ForwardPDTrigger, m_ReversePDTrigger, m_PreloadValue;
	DWORD m_ReloadOnExternalPD, m_Invert, m_PDGeneration;

	DWORD m_SEPDSetup, m_EncoderControl;

	// Extra parameters for completeness
	DWORD m_SelectedCard;
	DWORD m_HeadSelected[MAXHEADS][MAXROWS], m_AllHeadSelected[MAXROWS];
	DWORD m_AllPowerSelected;

	DWORD m_Active, m_IdleSpitFrequency, m_IdleSpitLength, m_UniSpitLength, m_IdleGreyLevel, m_SpitActive;

	DWORD m_Enabled, m_Random, m_Bust, m_LevelMap, m_Levels;
	float m_RatioValue[4], m_NoiseValue;
	int m_ActiveLevel[NUMBERGREYSCALES];


private:
// Implementation
	bool m_bSerializeOk;
protected:

//	DECLARE_MESSAGE_MAP()
};

#endif
