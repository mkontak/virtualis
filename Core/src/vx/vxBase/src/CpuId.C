// $Id: CpuId.C,v 1.2 2007/03/01 18:47:05 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos (mailto:george@viatronix.com)

#include "StdAfx.h"
#include "CpuId.h"

#ifdef _WIN64

  // prototype for function defined in CpuId.asm
  extern "C" void cpuid64( CpuIdRegisters * pRegisters );

#endif


/**
 * Sets the specified function arguments and executes cpuid
 *
 * @param   eax   register eax value to set
 * @param   ebx   register ebx value to set
 * @param   ecx   register ecx value to set
 * @param   edx   register edx value to set
 */
CpuIdRegisters & CpuId::Execute( DWORD eax, DWORD ebx, DWORD ecx, DWORD edx )
{
  CpuIdRegisters & registers = GetRegisters();
  
  // set the function arguments
  registers.m_eax = eax;
  registers.m_ebx = ebx;
  registers.m_ecx = ecx;
  registers.m_edx = edx;

  #ifdef _WIN64 
  {
    // code in CpuId.asm
    cpuid64( &registers );
  }
  #else
  { 
    CpuIdRegisters * p = &registers;
    __asm 
    {
      mov	      edi, p
      mov eax, [edi].m_eax
      mov ecx, [edi].m_ecx
      cpuid
      mov [edi].m_eax, eax
      mov [edi].m_ebx, ebx
      mov [edi].m_ecx, ecx
      mov [edi].m_edx, edx
    }
  }
  #endif
  
  return registers;
} // Execute( DWORD eax, DWORD ebx, DWORD ecx, DWORD edx )


/**
 * Gets the cpuid related registers
 *
 * @return   CpuIdRegisters singleton instance
 */
CpuIdRegisters & CpuId::GetRegisters()
{
  // singleton instance
  static CpuIdRegisters registers;

  // initialize the first time around
  static bool bInitialized( false );
  if( !bInitialized )
  {
    memset( &registers, 0, sizeof( CpuIdRegisters ) );
    bInitialized = true;
  }
  return registers;
} // GetRegisters()

// $Log: CpuId.C,v $
// Revision 1.2  2007/03/01 18:47:05  geconomos
// code review preparation
//
// Revision 1.1  2006/02/02 20:00:11  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/CpuId.C,v 1.2 2007/03/01 18:47:05 geconomos Exp $
// $Id: CpuId.C,v 1.2 2007/03/01 18:47:05 geconomos Exp $
