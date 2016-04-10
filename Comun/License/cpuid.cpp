/////////////////////////////////////////////////////////////////////
// cpuid.cpp:	Implementation for the cpuid() function.
//
// Copyright 2005 by Chris Just.  All Rights Reserved.
/////////////////////////////////////////////////////////////////////
#include "cpuid.h"

#ifdef _DEBUG
#	include <iostream>
	using std::cout;
	using std::endl;
#endif	// _DEBUG


//////////////////////////////////////
// Initialize static member variables.
bool CCPUIDInfo::m_IsInitialized				= false;
char CCPUIDInfo::m_ProcessorName[ 13 ]			= "\0\0\0\0\0\0\0\0\0\0\0\0";
std::string  CCPUIDInfo::m_ProcessorType;
std::string  CCPUIDInfo::m_Brand;
unsigned int CCPUIDInfo::m_HighestInputValue	= 0;
unsigned int CCPUIDInfo::m_SteppingID			= 0;
unsigned int CCPUIDInfo::m_DisplayModel			= 0;
unsigned int CCPUIDInfo::m_DisplayFamilyID		= 0;
unsigned int CCPUIDInfo::m_Model				= 0;
unsigned int CCPUIDInfo::m_FamilyID				= 0;
unsigned int CCPUIDInfo::m_ExtendedModel		= 0;
unsigned int CCPUIDInfo::m_ExtendedFamilyID		= 0;
unsigned int CCPUIDInfo::m_CLFlush				= 0;
unsigned int CCPUIDInfo::m_NumberOfCPUs			= 0;
unsigned int CCPUIDInfo::m_APIC_ID				= 0;
bool CCPUIDInfo::m_CPUIDSupported				= false;
bool CCPUIDInfo::m_CMPXCHG16B					= false;
bool CCPUIDInfo::m_L1ContextID					= false;
bool CCPUIDInfo::m_TM2							= false;
bool CCPUIDInfo::m_EST							= false;
bool CCPUIDInfo::m_VMX							= false;
bool CCPUIDInfo::m_CPL							= false;
bool CCPUIDInfo::m_Monitor						= false;
bool CCPUIDInfo::m_SSE3							= false;
bool CCPUIDInfo::m_FPU							= false;
bool CCPUIDInfo::m_VME							= false;
bool CCPUIDInfo::m_DE							= false;
bool CCPUIDInfo::m_PSE							= false;
bool CCPUIDInfo::m_TSC							= false;
bool CCPUIDInfo::m_MSR							= false;
bool CCPUIDInfo::m_PAE							= false;
bool CCPUIDInfo::m_MCE							= false;
bool CCPUIDInfo::m_CX8							= false;
bool CCPUIDInfo::m_APIC							= false;
bool CCPUIDInfo::m_SEP							= false;
bool CCPUIDInfo::m_MTRR							= false;
bool CCPUIDInfo::m_PGE							= false;
bool CCPUIDInfo::m_MCA							= false;
bool CCPUIDInfo::m_CMOV							= false;
bool CCPUIDInfo::m_PAT							= false;
bool CCPUIDInfo::m_PSE36						= false;
bool CCPUIDInfo::m_PSN							= false;
bool CCPUIDInfo::m_CLFSH						= false;
bool CCPUIDInfo::m_DS							= false;
bool CCPUIDInfo::m_ACPI							= false;
bool CCPUIDInfo::m_MMX							= false;
bool CCPUIDInfo::m_FXSR							= false;
bool CCPUIDInfo::m_SSE							= false;
bool CCPUIDInfo::m_SSE2							= false;
bool CCPUIDInfo::m_SS							= false;
bool CCPUIDInfo::m_HTT							= false;
bool CCPUIDInfo::m_TM							= false;
bool CCPUIDInfo::m_PBE							= false;



/////////////////////////////////////////////////////////////////////
// isCPUIDSupported()
/////////////////////////////////////////////////////////////////////
bool
CCPUIDInfo::isCPUIDSupported()
{
	/////////////////////////////////////
	// First see if cpuid is supported.
	// If not, return false.
	__asm
	{
		pushf
		pop		eax						// Copy EFLAGS into EAX.
		mov		ebx,	eax				// Then make a backup copy in EBX.
		xor		eax,	0x00200000		// Flip bit 21.
		push	eax
		popf							// Copy the modified EAX back to EFLAGS.

		pushf
		pop		eax						// Copy EFLAGS into EAX again.

		push	ebx
		popf							// Reset EFLAGS to what it was before all this pushing & popping...

		cmp		eax,	ebx				// Compare EAX & EBX to see if bit 21 is still changed.
		jz		cpuid_not_supported		// If the flag wasn't changed, jump to end of this function.
	}

	m_CPUIDSupported = true;


cpuid_not_supported:

	m_CPUIDSupported = false;
#ifdef _DEBUG
	cout << "*****  The CPUID instruction is not supported!!  *****" << endl;
#endif	// _DEBUG

	return false;
}


/////////////////////////////////////////////////////////////////////
// init()
/////////////////////////////////////////////////////////////////////
bool
CCPUIDInfo::init()
{
	// Only initialize once.
	if ( m_IsInitialized == true )
	{
		return true;
	}


	/////////////////////////////////////
	// First see if cpuid is supported.
	// If not, return false.
	if ( IsCPUIDSupported() == false )
	{
		return false;
	}


	if ( cpuid_00h() == false )
	{
		return false;
	}

	if ( m_HighestInputValue == 0 )
	{
		m_IsInitialized = true;
		return true;
	}


	if ( cpuid_01h() == false )
	{
		return false;
	}

	if ( m_HighestInputValue == 1 )
	{
		m_IsInitialized = true;
		return true;
	}




	m_IsInitialized = true;
	return true;
}


/////////////////////////////////////////////////////////////////////
// cpuid_00h()
/////////////////////////////////////////////////////////////////////
bool
CCPUIDInfo::cpuid_00h()
{
	////////////////////////////////////
	// Pass 0x00 to cpuid.
	// Get the Processor Vendor ID.
	char* pszName		= m_ProcessorName;	// Get a pointer to ProcessorName.
	unsigned int EAXBuf	= 0;

	__asm
	{
		mov		eax,		0			// Get Processor Vendor ID.
		cpuid

		mov		EAXBuf,		eax			// This holds the highest value we can pass to cpuid.
		mov		edi,		pszName		// Copy Processor Vendor ID into buf.
		mov		[edi],		ebx
		mov		[edi+4],	edx
		mov		[edi+8],	ecx
	}

	m_HighestInputValue = EAXBuf;

#ifdef _DEBUG
	cout << "The highest input value recognized is: " << m_HighestInputValue << endl;
#endif	// _DEBUG

	pszName[ 12 ] = '\0';	// Terminate the string.
	return true;
}



/////////////////////////////////////////////////////////////////////
// cpuid_01h()
/////////////////////////////////////////////////////////////////////
bool
CCPUIDInfo::cpuid_01h()
{
	////////////////////////////////////
	// Pass 0x01 to cpuid.
	// Get the Additional Processor Info.

	unsigned int EAXBuf	= 0;
	unsigned int EBXBuf	= 0;
	unsigned int ECXBuf = 0;
	unsigned int EDXBuf = 0;

	__asm
	{
		mov		eax,			1		// Get Version & Feature Info.
		cpuid

		mov		EAXBuf,		eax			// Version Info.
		mov		EBXBuf,		ebx			// Brand index, CLFLUSH line size, Max # of CPUs, APIC ID.
		mov		ECXBuf,		ecx			// Extended Feature Info.
		mov		EDXBuf,		edx			// Feature Info.
	}

	unsigned int processorType = 0;

	// EAX
	// Parse the individual bit groups out.
	// First mask the relevent bits.
	m_SteppingID		= EAXBuf & 0x0000000F;
	m_Model				= EAXBuf & 0x000000F0;
	m_FamilyID			= EAXBuf & 0x00000F00;
	processorType		= EAXBuf & 0x00003000;
	m_ExtendedModel		= EAXBuf & 0x000F0000;
	m_ExtendedFamilyID	= EAXBuf & 0x0FF00000;

	// Then move the bits to the front.
	m_Model				>>= 4;
	m_FamilyID			>>= 8;
	processorType		>>= 12;
	m_ExtendedModel		>>= 16;
	m_ExtendedFamilyID	>>= 20;

	if ( (m_FamilyID == 0x6) || (m_FamilyID == 0xF) )
	{
		m_DisplayModel = (m_ExtendedModel << 4) + m_Model;
	}

	if ( m_FamilyID == 0xF )
	{
		m_DisplayFamilyID = m_ExtendedFamilyID + m_FamilyID;
	}

	switch ( processorType )
	{
	case 0:
		m_ProcessorType = "Original OEM Processor";
		break;
	case 1:
		m_ProcessorType = "Intel OverDrive Processor";
		break;
	case 2:
		m_ProcessorType = "Dual Processor";
		break;
	case 3:
		m_ProcessorType = "Intel Reserved";
		break;
	}

	// EBX
	// Then separate the individual bytes.

	// Use a union because it's easier than pulling bytes out of an int.
	union CPU_Union
	{
		unsigned int	ProcessorSignature;
		unsigned char	EBXArray[ 4 ];
	} EBXInfo;

	EBXInfo.ProcessorSignature	= EBXBuf;
	unsigned int brandIndex		= 0;

	brandIndex		=	EBXInfo.EBXArray[ 0 ];
	m_CLFlush		=	EBXInfo.EBXArray[ 1 ] * 8;	// CLFLUSH is in 8-byte increments.
	m_NumberOfCPUs	=	EBXInfo.EBXArray[ 2 ];
	m_APIC_ID		=	EBXInfo.EBXArray[ 3 ];

	if ( (m_FamilyID == 0xF) && (m_Model >= 0x3) )
	{
		// XXX: Use 'Brand String method'.
	}
	else
	{
		// These are all the Brand strings used in the 'Brand Index'.
		static const char szCeleron[]			= "Intel(R) Celeron(R) processor";
		static const char szCeleronM[]			= "Intel(R) Celeron(R) M processor";
		static const char szPentium3[]			= "Intel(R) Pentium(R) III processor";
		static const char szPentium4[]			= "Intel(R) Pentium(R) 4 processor";
		static const char szPentiumM[]			= "Intel(R) Pentium(R) M processor";
		static const char szXeon[]				= "Intel(R) Xeon(TM) processor";
		static const char szXeonMP[]			= "Intel(R) Xeon(TM) processor MP";
		static const char szPentium3Xeon[]		= "Intel(R) Pentium(R) III Xeon(TM) processor";
		static const char szMobileGenuine[]		= "Mobile Genuine Intel(R) processor";
		static const char szMobilePentium3[]	= "Mobile Intel(R) Pentium(R) III processor-M";
		static const char szMobilePentium4[]	= "Mobile Intel(R) Pentium(R) 4 processor-M";
		static const char szMobileCeleron[]		= "Mobile Intel(R) Celeron(R) processor";

		// Use 'Brand Index' method.
		switch ( brandIndex )
		{
		case 0x00:
			m_Brand = "";	// Not supported.
			break;
		case 0x01:
			m_Brand = szCeleron;
			break;
		case 0x02:
			m_Brand = szPentium3;
			break;
		case 0x03:
			if ( EBXInfo.ProcessorSignature == 0x000006B1 )
			{
				m_Brand = szCeleron;
			}
			else
			{
				m_Brand = szPentium3Xeon;
			}
			break;
		case 0x04:
			m_Brand = szPentium3;
			break;
		case 0x06:
			m_Brand = szMobilePentium3;
			break;
		case 0x07:
			m_Brand = szMobileCeleron;
			break;
		case 0x08:
			m_Brand = szPentium4;
			break;
		case 0x09:
			m_Brand = szPentium4;
			break;
		case 0x0A:
			m_Brand = szCeleron;
			break;
		case 0x0B:
			if ( EBXInfo.ProcessorSignature == 0x00000F13 )
			{
				m_Brand = szXeonMP;
			}
			else
			{
				m_Brand = szXeon;
			}
			break;
		case 0x0C:
			m_Brand = szXeonMP;
			break;
		case 0x0E:
			if ( EBXInfo.ProcessorSignature == 0x00000F13 )
			{
				m_Brand = szXeon;
			}
			else
			{
				m_Brand = szMobilePentium4;
			}
			break;
		case 0x0F:
			m_Brand = szMobileCeleron;
			break;
		case 0x11:
			m_Brand = szMobileGenuine;
			break;
		case 0x12:
			m_Brand = szCeleronM;
			break;
		case 0x13:
			m_Brand = szMobileCeleron;
			break;
		case 0x14:
			m_Brand = szCeleron;
			break;
		case 0x15:
			m_Brand = szMobileGenuine;
			break;
		case 0x16:
			m_Brand = szPentiumM;
			break;
		case 0x17:
			m_Brand = szMobileCeleron;
			break;
		default:
			m_Brand = "Reserved!!";
			break;
		}
	}

	// ECX
	// Parse Extended Feature Info from ECX.
	m_SSE3			= ((ECXBuf & 0x00000001) != 0);
	// Reserved					 0x00000002
	// Reserved					 0x00000004
	m_Monitor		= ((ECXBuf & 0x00000008) != 0);
	m_CPL			= ((ECXBuf & 0x00000010) != 0);
	m_VMX			= ((ECXBuf & 0x00000020) != 0);
	// Reserved					 0x00000040
	m_EST			= ((ECXBuf & 0x00000080) != 0);
	m_TM2			= ((ECXBuf & 0x00000100) != 0);
	// Reserved					 0x00000200
	m_L1ContextID	= ((ECXBuf & 0x00000400) != 0);
	// Reserved					 0x00000800
	// Reserved					 0x00001000
	m_CMPXCHG16B	= ((ECXBuf & 0x00002000) != 0);


	// EDX
	// Parse the Feature Info.
	m_FPU	= ((EDXBuf & 0x00000001) != 0);
	m_VME	= ((EDXBuf & 0x00000002) != 0);
	m_DE	= ((EDXBuf & 0x00000004) != 0);
	m_PSE	= ((EDXBuf & 0x00000008) != 0);
	m_TSC	= ((EDXBuf & 0x00000010) != 0);
	m_MSR	= ((EDXBuf & 0x00000020) != 0);
	m_PAE	= ((EDXBuf & 0x00000040) != 0);
	m_MCE	= ((EDXBuf & 0x00000080) != 0);
	m_CX8	= ((EDXBuf & 0x00000100) != 0);
	m_APIC	= ((EDXBuf & 0x00000200) != 0);
	// Reserved			 0x00000400
	m_SEP	= ((EDXBuf & 0x00000800) != 0);
	m_MTRR	= ((EDXBuf & 0x00001000) != 0);
	m_PGE	= ((EDXBuf & 0x00002000) != 0);
	m_MCA	= ((EDXBuf & 0x00004000) != 0);
	m_CMOV	= ((EDXBuf & 0x00008000) != 0);
	m_PAT	= ((EDXBuf & 0x00010000) != 0);
	m_PSE36	= ((EDXBuf & 0x00020000) != 0);
	m_PSN	= ((EDXBuf & 0x00040000) != 0);
	m_CLFSH	= ((EDXBuf & 0x00080000) != 0);
	// Reserved			 0x00100000
	m_DS	= ((EDXBuf & 0x00200000) != 0);
	m_ACPI	= ((EDXBuf & 0x00400000) != 0);
	m_MMX	= ((EDXBuf & 0x00800000) != 0);
	m_FXSR	= ((EDXBuf & 0x01000000) != 0);
	m_SSE	= ((EDXBuf & 0x02000000) != 0);
	m_SSE2	= ((EDXBuf & 0x04000000) != 0);
	m_SS	= ((EDXBuf & 0x08000000) != 0);
	m_HTT	= ((EDXBuf & 0x10000000) != 0);
	m_TM	= ((EDXBuf & 0x20000000) != 0);
	// Reserved			 0x40000000
	m_PBE	= ((EDXBuf & 0x80000000) != 0);

	return true;
}

