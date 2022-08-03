// $Id: CpuInfo.h,v 1.3 2007/03/01 18:47:05 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos (mailto:george@viatronix.com)

#pragma once

// class declaration
class VX_BASE_DLL CpuInfo
{
// member functions
public:
  
  /// constructor
  CpuInfo();
  
  /// destructor
  virtual ~CpuInfo();

  /// gets the cpu vendor string
  const std::string & GetVendor() const { return m_sVendor; }

  /// get ths number of cores per processor
  const int4 GetCoresPerProcessor() const { return m_iCoresPerProcessor; }

  /// get ths number of threads per core
  const int4 GetThreadsPerCore() const { return m_iThreadsPerCore; }

  /// gets the total number of physical processors
  const int4 GetTotalPhysicalProcessors() const { return m_iTotalPhysicalProcessors; }

  /// gets the total number of cores
  const int4 GetTotalCores() const { return m_iTotalCores; }

  /// gets the total number that can concurrently run
  const int4 GetTotalThreads() const { return m_iTotalThreads; }

  /// gets if the hyper-threading is supported
  const bool GetHyperThreadingSupported() const { return m_bHyperThreadingSupported; }

  /// gets if the hyper-threading is enabled
  const bool GetHyperThreadingEnabled() const { return m_bHyperThreadingEnabled; }

private:

  /// common initialization routine
  void CommonInit();

  /// amd specific initialization
  void InitializeAuthenticAMD();

  /// intel specific initialization
  void InitializeGenuineIntel();

// member variables
private:

  /// vendor string
  std::string m_sVendor;

  /// number of cores
  int4 m_iCoresPerProcessor;

  /// number of threads per core
  int4 m_iThreadsPerCore;

  /// total number of physical processors
  int4 m_iTotalPhysicalProcessors;
  
  /// total number of cores
  int4 m_iTotalCores;

  /// total concurrent threads
  int4 m_iTotalThreads;

  /// indicates if hyperthreading supported
  bool m_bHyperThreadingSupported;

  /// indicates if hyperthreading enabled
  bool m_bHyperThreadingEnabled;

}; // class CpuInfo


// $Log: CpuInfo.h,v $
// Revision 1.3  2007/03/01 18:47:05  geconomos
// code review preparation
//
// Revision 1.2  2006/08/01 17:54:16  jmeade
// dll export calls.
//
// Revision 1.1  2006/02/02 20:00:11  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/CpuInfo.h,v 1.3 2007/03/01 18:47:05 geconomos Exp $
// $Id: CpuInfo.h,v 1.3 2007/03/01 18:47:05 geconomos Exp $
