// $Id: CpuId.h,v 1.3 2007/03/01 18:47:05 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos (mailto:george@viatronix.com)

#pragma once


/**
 * cpuid related registers - for some reason the 32-bit 
 * inline assembler chokes when I make this a nested class
 */
struct CpuIdRegisters
{
  DWORD m_eax;
  DWORD m_ebx;
  DWORD m_ecx;
  DWORD m_edx;
}; // struct CpuIdRegisters


// class declaration
class VX_BASE_DLL CpuId
{
// member functions
public:
  
  /// sets the specified function arguments and executes cpuid
  static CpuIdRegisters & Execute( DWORD eax, DWORD ebx = 0, DWORD ecx = 0, DWORD edx = 0 );

  /// gets the cpuid related registers
  static CpuIdRegisters & GetRegisters();

private:

  /// disallow instances
  CpuId() {}

}; // class CpuId

// $Log: CpuId.h,v $
// Revision 1.3  2007/03/01 18:47:05  geconomos
// code review preparation
//
// Revision 1.2  2006/08/01 17:54:16  jmeade
// dll export calls.
//
// Revision 1.1  2006/02/02 20:00:11  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/CpuId.h,v 1.3 2007/03/01 18:47:05 geconomos Exp $
// $Id: CpuId.h,v 1.3 2007/03/01 18:47:05 geconomos Exp $
