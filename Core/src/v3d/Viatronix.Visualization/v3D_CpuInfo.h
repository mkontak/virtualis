// $Id: v3D_CpuInfo.h,v 1.1 2007/03/22 18:24:35 gdavidson Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson (mailto:gdavidson@viatronix.com)


#pragma once


// includes
#include "CpuInfo.h"


// namespaces
OPEN_VISUALIZATION_NS 


public ref class CpuInfo
{
public:

  /// default constructor
  CpuInfo();

  /// gets the cpu vendor string
  property System::String ^ Vendor { System::String ^ get() { return gcnew System::String( m_cpuInfo.GetVendor().c_str() ); }}

  /// get ths number of cores per processor
  property int4 CoresPerProcessor { int4 get() { return m_cpuInfo.GetCoresPerProcessor(); }}

  /// get ths number of threads per core
  property int4 ThreadsPerCore { int4 get() { return m_cpuInfo.GetThreadsPerCore(); }}

  /// gets the total number of physical processors
  property int4 TotalPhysicalProcessors { int4 get() { return m_cpuInfo.GetTotalPhysicalProcessors(); }}

  /// gets the total number of cores
  property int4 TotalCores { int4 get() { return m_cpuInfo.GetTotalCores(); }}

  /// gets the total number that can concurrently run
  property int4 TotalThreads { int4 get() { return m_cpuInfo.GetTotalThreads(); }}

  /// gets if the hyper-threading is supported
  property bool HyperThreadingSupported { bool get() { return m_cpuInfo.GetHyperThreadingSupported(); }}

  /// gets if the hyper-threading is enabled
  property bool HyperThreadingEnabled { bool get() { return m_cpuInfo.GetHyperThreadingEnabled(); }}

private:

  ::CpuInfo & m_cpuInfo; 

}; // class CpuInfo


// namespaces
CLOSE_VISUALIZATION_NS


// $Log: v3D_CpuInfo.h,v $
// Revision 1.1  2007/03/22 18:24:35  gdavidson
// Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_CpuInfo.h,v 1.1 2007/03/22 18:24:35 gdavidson Exp $
// $Id: v3D_CpuInfo.h,v 1.1 2007/03/22 18:24:35 gdavidson Exp $

