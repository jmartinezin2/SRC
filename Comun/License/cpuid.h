/////////////////////////////////////////////////////////////////////
// cpuid.h:	Interface for the cpuid() function.
//
// Copyright 2005 by Chris Just.  All Rights Reserved.
/////////////////////////////////////////////////////////////////////
#include <string>


class CCPUIDInfo
{
public:
	static bool
	IsCPUIDSupported()
	{
		return isCPUIDSupported();
	}

	static unsigned int
	HighestInputValue()
	{
		init();
		return m_HighestInputValue;
	}

	static const char*
	ProcessorName()
	{
		init();
		return m_ProcessorName;
	}

	static unsigned int
	SteppingID()
	{
		init();
		return m_SteppingID;
	}

	static unsigned int
	Model()
	{
		init();
		return m_DisplayModel;
	}

	static unsigned int
	FamilyID()
	{
		init();
		return m_DisplayFamilyID;
	}

	static const char*
	ProcessorType()
	{
		init();
		return m_ProcessorType.c_str();
	}

	static const char*
	Brand()
	{
		init();
		return m_Brand.c_str();
	}

	static unsigned int
	CLFlush()
	{
		init();
		return m_CLFlush;
	}

	static unsigned int
	NumberOfCPUs()
	{
		init();
		return m_NumberOfCPUs;
	}

	static unsigned int
	APIC_ID()
	{
		init();
		return m_APIC_ID;
	}

	static bool
	CMPXCHG16B()
	{
		init();
		return m_CMPXCHG16B;
	}

	static bool
	L1ContextID()
	{
		init();
		return m_L1ContextID;
	}

	static bool
	TM2()
	{
		init();
		return m_TM2;
	}

	static bool
	EST()
	{
		init();
		return m_EST;
	}

	static bool
	VMX()
	{
		init();
		return m_VMX;
	}

	static bool
	CPL()
	{
		init();
		return m_CPL;
	}

	static bool
	Monitor()
	{
		init();
		return m_Monitor;
	}

	static bool
	SSE3()
	{
		init();
		return m_SSE3;
	}
///////////////////////////////////
	static bool
	FPU()
	{
		init();
		return m_FPU;
	}
	
	static bool
	VME()
	{
		init();
		return m_VME;
	}
	
	static bool
	DE()
	{
		init();
		return m_DE;
	}
	
	static bool
	PSE()
	{
		init();
		return m_PSE;
	}
	
	static bool
	TSC()
	{
		init();
		return m_TSC;
	}
	
	static bool
	MSR()
	{
		init();
		return m_MSR;
	}
	
	static bool
	PAE()
	{
		init();
		return m_PAE;
	}
	
	static bool
	MCE()
	{
		init();
		return m_MCE;
	}
	
	static bool
	CX8()
	{
		init();
		return m_CX8;
	}
	
	static bool
	APIC()
	{
		init();
		return m_APIC;
	}
	
	static bool
	SEP()
	{
		init();
		return m_SEP;
	}
	
	static bool
	MTRR()
	{
		init();
		return m_MTRR;
	}
	
	static bool
	PGE()
	{
		init();
		return m_PGE;
	}
	
	static bool
	MCA()
	{
		init();
		return m_MCA;
	}
	
	static bool
	CMOV()
	{
		init();
		return m_CMOV;
	}
	
	static bool
	PAT()
	{
		init();
		return m_PAT;
	}
	
	static bool
	PSE36()
	{
		init();
		return m_PSE36;
	}
	
	static bool
	PSN()
	{
		init();
		return m_PSN;
	}
	
	static bool
	CLFSH()
	{
		init();
		return m_CLFSH;
	}
	
	static bool
	DS()
	{
		init();
		return m_DS;
	}
	
	static bool
	ACPI()
	{
		init();
		return m_ACPI;
	}
	
	static bool
	MMX()
	{
		init();
		return m_MMX;
	}
	
	static bool
	FXSR()
	{
		init();
		return m_FXSR;
	}
	
	static bool
	SSE()
	{
		init();
		return m_SSE;
	}
	
	static bool
	SSE2()
	{
		init();
		return m_SSE2;
	}
	
	static bool
	SS()
	{
		init();
		return m_SS;
	}
	
	static bool
	HTT()
	{
		init();
		return m_HTT;
	}
	
	static bool
	TM()
	{
		init();
		return m_TM;
	}
	
	static bool
	PBE()
	{
		init();
		return m_PBE;
	}



private:
	// Disable all these default functions.
	CCPUIDInfo();
	CCPUIDInfo( const CCPUIDInfo& );
	~CCPUIDInfo();
	CCPUIDInfo& operator=( const CCPUIDInfo& );


	////////////////////////
	// Private functions. //
	////////////////////////

	static bool
	isCPUIDSupported();

	static bool
	init();

	static bool
	cpuid_00h();

	static bool
	cpuid_01h();


	///////////////////////
	// Member Variables. //
	///////////////////////

	static bool			m_CPUIDSupported;
	static bool			m_IsInitialized;

	// EAX = 0.
	static unsigned int	m_HighestInputValue;
	static char			m_ProcessorName[ 13 ];

	// EAX = 1.
	static unsigned int	m_SteppingID;		// EAX
	static unsigned int	m_DisplayModel;
	static unsigned int	m_DisplayFamilyID;
	static unsigned int	m_Model;
	static unsigned int	m_FamilyID;
	static std::string	m_ProcessorType;
	static unsigned int	m_ExtendedModel;
	static unsigned int	m_ExtendedFamilyID;

	static std::string	m_Brand;			// EBX
	static unsigned int	m_CLFlush;
	static unsigned int	m_NumberOfCPUs;
	static unsigned int	m_APIC_ID;

	static bool			m_CMPXCHG16B;		// ECX
	static bool			m_L1ContextID;
	static bool			m_TM2;
	static bool			m_EST;
	static bool			m_VMX;
	static bool			m_CPL;
	static bool			m_Monitor;
	static bool			m_SSE3;

	static bool			m_FPU;				// EDX
	static bool			m_VME;
	static bool			m_DE;
	static bool			m_PSE;
	static bool			m_TSC;
	static bool			m_MSR;
	static bool			m_PAE;
	static bool			m_MCE;
	static bool			m_CX8;
	static bool			m_APIC;
	static bool			m_SEP;
	static bool			m_MTRR;
	static bool			m_PGE;
	static bool			m_MCA;
	static bool			m_CMOV;
	static bool			m_PAT;
	static bool			m_PSE36;
	static bool			m_PSN;
	static bool			m_CLFSH;
	static bool			m_DS;
	static bool			m_ACPI;
	static bool			m_MMX;
	static bool			m_FXSR;
	static bool			m_SSE;
	static bool			m_SSE2;
	static bool			m_SS;
	static bool			m_HTT;
	static bool			m_TM;
	static bool			m_PBE;
};

