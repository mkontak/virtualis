// $Id: CpuInfo.C,v 1.2 2007/03/01 18:47:05 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos (mailto:george@viatronix.com)

#include "StdAfx.h"
#include "CpuInfo.h"
#include "CpuId.h"


#define CPUID_HYPERTHREADING_CAPABLE(edx)      ( edx &  0x10000000 )          // EDX[ 28 ]
#define CPUID_LOGICAL_PROCESSOR_COUNT(ebx)     ( ( ebx >> 16 ) & 0xFF )       // EBX[ 16:23 ] 
#define CPUID_APIC(ebx)                        ( ( ebx >> 24 ) & 0xFF )       // EBX[ 24:31 ]
#define CPUID_AMD_CORE_COUNT(edx)              ( edx & 0xFF ) + 1             // ECX[ 0:7 ] 
#define CPUID_AMD_LEGACY(ecx)                  ( ecx & 0x2 )                  // ECX[ 1 ] 
#define CPUID_INTEL_CORE_COUNT(eax)            ( ( eax >> 26 ) & 0x3F ) + 1   // EAX[ 26:31 ] 

const char * PROCESSOR_VENDOR_AMD   = "AuthenticAMD";
const char * PROCESSOR_VENDOR_INTEL = "GenuineIntel";


/**
 * Consturctor
 */
CpuInfo::CpuInfo() : 
  m_iCoresPerProcessor( 1 ),
  m_iThreadsPerCore( 1 ),
  m_iTotalPhysicalProcessors( 1 ),
  m_iTotalCores( 1 ),
  m_iTotalThreads( 0 ),
  m_bHyperThreadingSupported( false ),
  m_bHyperThreadingEnabled( false )
{
  CommonInit();
} // CpuInfo()


/**
 * Destructor
 */
CpuInfo::~CpuInfo()
{
} // ~CpuInfo()


/**
 * Performs commoin initialization
 */
void CpuInfo::CommonInit()
{
  CpuIdRegisters & registers = CpuId::GetRegisters();

  /// get cpu string
  char vendor[ 0x40 ];
  memset( vendor, 0, sizeof( vendor ) );
  CpuId::Execute( 0x0 );
  *( reinterpret_cast<int4*>( vendor + 0 ) ) = registers.m_ebx;
  *( (int4*)( vendor + 4 ) ) = registers.m_edx;
  *( (int4*)( vendor + 8 ) ) = registers.m_ecx;
  m_sVendor = vendor;

  // amd processor
  if( !_stricmp( PROCESSOR_VENDOR_AMD, m_sVendor.c_str() ) )
    InitializeAuthenticAMD();
   
  // intel processor
  else if( !_stricmp( PROCESSOR_VENDOR_INTEL, m_sVendor.c_str() ) )
    InitializeGenuineIntel();

  SYSTEM_INFO systemInfo;
  GetSystemInfo( &systemInfo );

  m_iTotalPhysicalProcessors = systemInfo.dwNumberOfProcessors / m_iCoresPerProcessor;
  m_iTotalCores = m_iTotalPhysicalProcessors * m_iCoresPerProcessor;
  m_iTotalThreads = m_iTotalCores * m_iThreadsPerCore;

} // CommonInit()


/**
 * Amd specific initialization
 */
void CpuInfo::InitializeAuthenticAMD()
{
  m_bHyperThreadingSupported = false;
  m_bHyperThreadingEnabled = false;

  CpuIdRegisters & registers = CpuId::GetRegisters();

  // get the number of extended ids
  CpuId::Execute( 0x80000000 );
  DWORD dwExtendedCount( registers.m_eax );

  // legacy processor?
  if( dwExtendedCount < 0x80000008 )
  {
    CpuId::Execute( 0x80000001 );

    if( CPUID_HYPERTHREADING_CAPABLE( registers.m_edx ) )
    {
      int4 iLogicalProcessorCount( CPUID_LOGICAL_PROCESSOR_COUNT( registers.m_ebx ) );
      
      CpuId::Execute( 0x80000001 );
      if( !CPUID_AMD_LEGACY( registers.m_ecx ) )
      {
        // log unhandled processor type
      }
    }
  }
  else
  {
    CpuId::Execute( 0x80000008 );
    m_iCoresPerProcessor = CPUID_AMD_CORE_COUNT( registers.m_ecx );
  }
} // InitializeAuthenticAMD()


/**
 * Intel specific initialization
 */
void CpuInfo::InitializeGenuineIntel()
{  
  CpuIdRegisters & registers = CpuId::GetRegisters();

	CpuId::Execute( 0 );

  // determine the number of cores
  DWORD dwFunctionCount( registers.m_eax );	
  if( ( dwFunctionCount > 3 ) && ( dwFunctionCount < 0x80000000 ) ) 
  { 
	  int4 iCaches( 0 );
		for( int4 iCacheIndex(0 ); ; iCacheIndex++ ) 
    {
      CpuId::Execute( 4, 0, iCacheIndex );
			if( ( registers.m_eax & 0x1F ) == 0 )
				break;
			iCaches++;
		}

    if( iCaches > 0 ) 
    {
      CpuId::Execute( 4 );
      m_iCoresPerProcessor = CPUID_INTEL_CORE_COUNT( registers.m_eax );
	  }
	} // end [ determine the number of cores ]

  CpuId::Execute( 0x00000001 );

  if( CPUID_HYPERTHREADING_CAPABLE( registers.m_edx ) )
  {
    m_bHyperThreadingSupported = true;

    if( m_iCoresPerProcessor == 1 )
    {
      int4 iLogicalProcessorCount( CPUID_LOGICAL_PROCESSOR_COUNT( registers.m_ebx ) );

		  if( iLogicalProcessorCount >= 1 )    // >1 Doesn't mean HT is enabled in the BIOS
		  {
			  DWORD_PTR  dwProcessAffinity( 0 );
			  DWORD_PTR  dwSystemAffinity( 0 );

			  // calculate the appropriate mask based on the  number of logical processors
			  int4 i( 1 );
        DWORD dwPhysicalIdMask( 0xFF );
			  while( i < iLogicalProcessorCount )
			  {
				  i *= 2;
 	        dwPhysicalIdMask  <<= 1;
			  }
			
			  HANDLE hCurrentProcessHandle = GetCurrentProcess();
			  GetProcessAffinityMask( hCurrentProcessHandle, &dwProcessAffinity,&dwSystemAffinity );

        DWORD dwAffinityMask( 1 );
        while( dwAffinityMask != 0 && dwAffinityMask <= dwProcessAffinity )
        {
          // check if this CPU is available
          if( dwAffinityMask & dwProcessAffinity )
          {
            if( SetProcessAffinityMask( hCurrentProcessHandle, dwAffinityMask ) )
            {
              // give OS time to switch CPU
              Sleep( 0 );
              CpuId::Execute( 0x00000001 );
              if( ( CPUID_APIC( registers.m_ebx ) & ~dwPhysicalIdMask ) != 0 ) 
              {
                m_bHyperThreadingEnabled = true;
                m_iCoresPerProcessor = 2;
              }
            }
          }
          dwAffinityMask = dwAffinityMask << 1;
        }             
		  
        // reset the processor affinity
		    SetProcessAffinityMask( hCurrentProcessHandle, dwProcessAffinity );
      }
    }
  }
  else
  {
    m_bHyperThreadingSupported = false;
    m_bHyperThreadingEnabled = false;
  }

} // InitializeGenuineIntel()


// $Log: CpuInfo.C,v $
// Revision 1.2  2007/03/01 18:47:05  geconomos
// code review preparation
//
// Revision 1.1  2006/02/02 20:00:11  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/CpuInfo.C,v 1.2 2007/03/01 18:47:05 geconomos Exp $
// $Id: CpuInfo.C,v 1.2 2007/03/01 18:47:05 geconomos Exp $
