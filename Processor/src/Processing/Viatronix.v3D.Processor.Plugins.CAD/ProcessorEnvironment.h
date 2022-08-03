// $Id: ProcessorEnvironment.h,v 1.5.6.1 2009/07/16 19:05:34 dongqing Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos( mailto:george@viatronix.com )

// pragma declarations
#pragma once

// include declarations
#include "vxComCAD.h"

// class definition
class ATL_NO_VTABLE ProcessorEnvironment : 
  public CComObjectRootEx< CComSingleThreadModel >,
  public CComCoClass< ProcessorEnvironment >,
  public IDispatchImpl< IProcessorEnvironment, &__uuidof( IProcessorEnvironment ) >
{
/// ATL macros and defines
public:
	
// protects your object from being deleted if the reference count decrements the count to 0 during constructions
DECLARE_PROTECT_FINAL_CONSTRUCT()

// indicates coclass is not self-registering
DECLARE_NO_REGISTRY()

// indicates object cannot be aggregated
DECLARE_NOT_AGGREGATABLE(ProcessorEnvironment)

// interface map
BEGIN_COM_MAP( ProcessorEnvironment )
  COM_INTERFACE_ENTRY( IProcessorEnvironment )
  COM_INTERFACE_ENTRY( IDispatch )
  COM_INTERFACE_ENTRY_THIS()
END_COM_MAP()

// IProcessorEnvironment implementation
public:

  /// gets the intensity volume
  STDMETHOD( get_IntensityVolume )( IVolume ** ppiIntensityVolume );

  /// gets the segment volume
  STDMETHOD( get_SegmentVolume )( IVolume ** ppiSegmentVolume );

  /// gets the dicom directory
  STDMETHOD( get_DicomDirectory )( BSTR * psDicomDirectory );

  /// sets the dicom directory
  STDMETHOD( put_DicomDirectory )( BSTR  sDicomDirectory );

  /// gets the cad destination directory
  STDMETHOD( get_DestDirectory )( BSTR * psDestDirectory );

  /// sets the cad destination directory
  STDMETHOD( put_DestDirectory )( BSTR  sDestDirectory );

// member functions
public:
  
  /// default constructor
  ProcessorEnvironment();

  /// called after the object is fully instantiated
  HRESULT FinalConstruct();
	
  /// called after the final reference to the instance has been released
	void FinalRelease();

  /// initializes the environment object
  void Initialize( const std::string & spreProcessDirectory, const std::string & sDicomDirectory, 
    const std::string & sVolumeDirectory, const std::string & sDestDirectory, const bool & bDicomOnly );


// member variables
private:

  /// intensity volume
  CComPtr< IVolume > m_spIntensityVolume;

  /// segment volume
  CComPtr< IVolume > m_spSegmentVolume;

  /// dicom directory
  CComBSTR m_sDicomDirectory;

  /// dicom directory
  CComBSTR m_sDestFilePath;

}; // coclass ProcessorEnvironment 

// $Log: ProcessorEnvironment.h,v $
// Revision 1.5.6.1  2009/07/16 19:05:34  dongqing
// add cad destination path into the processorEnvironment
//
// Revision 1.5  2006/02/09 19:33:51  romy
// moved commandline processing to ColonEnvironmentArgs class
//
// Revision 1.4  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.3  2005/11/08 14:42:17  geconomos
// corrected error related to "-dicomonly" flag
//
// Revision 1.2.2.1  2005/12/19 15:50:36  geconomos
// add flag to Initialize() to indicate if only the dicom directory is valid
//
// Revision 1.2  2005/09/20 18:58:00  geconomos
// changed all interfaces to be dual instead of cutom only
//
// Revision 1.1  2005/09/16 14:48:15  geconomos
// moved from vxCAD
//
// Revision 1.1  2005/09/15 16:38:27  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD/ProcessorEnvironment.h,v 1.5.6.1 2009/07/16 19:05:34 dongqing Exp $
// $Id: ProcessorEnvironment.h,v 1.5.6.1 2009/07/16 19:05:34 dongqing Exp $
