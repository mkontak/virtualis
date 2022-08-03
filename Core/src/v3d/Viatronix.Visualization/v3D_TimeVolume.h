// $Id: v3D_TimeVolume.h,v 1.4 2006/10/04 18:29:06 gdavidson Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Frank Dachille (frank@viatronix.com)


// Pragmas
#pragma once


// Includes
#include "v3D_Volume.h"
#include "v3D_Triplet.h"
#include "vxTimeVolume.h"


// Fowards
class vxManagedObserver;


// Namespaces
OPEN_VISUALIZATION_NS


/**
* Volume class.
*/

public ref class  TimeVolume : 
  public Viatronix::Visualization::IObserver
{

// constructors and finalizers
public:
  
  /// default constructor
  TimeVolume( );

  /// constructor from filename
  TimeVolume( System::String ^ mpFilename );

  /// constructor from triplet
  TimeVolume( Viatronix::Visualization::Triplet ^ mpDimensions );
  
  /// destructor.
  virtual ~TimeVolume();

  /// Finalizer.
  !TimeVolume();

  // IElement implementation
public:

  /// processes modified events
  virtual void OnModified( System::IntPtr modifiedList ) { }

  /// raised when element is modified
  virtual event System::EventHandler ^ Modified;

// managed methods
public:
  
  /// reads a volume from disk.
  void Read( System::String ^ mpFilepath );
  
  /// writes a volume to disk.
  void Write( System::String ^ mpFilepath );

  /// Read in an ECG file from a GE scanner (normally a ".iri" file).
  void ReadIRI( System::String ^ mpFilepath );

  /// writes the selected phase volume to disk.
  void WriteSelectedVolume( System::String ^ mpFilepath );

  /// Set all the slices to the given phase ( could be -1 to clear all )
  void ResetSelection( int iPhase );

  /// create a new volume given the dimensions
  void Create( Viatronix::Visualization::Triplet ^ mpDimensions );

  /// get a pointer to the volume
  System::IntPtr GetVolumePtr( int iIndex ) { return System::IntPtr( &(m_pTimeVolume->GetPhaseVolume(iIndex)) ); }

  /// get a pointer to the managed volume
  Volume ^ GetVolume( int iIndex );

  /// called when the object is modified
  //void Modified( System::IntPtr modifiedInfoList );

  /// retrieve the selected phase at a slice
  int GetSliceSelection( int iSliceZ );

  /// set the selected phase at a slice
  void SetSliceSelection( int iSliceZ, int iPhase );

  /// get the start time of a slice
  int GetSliceStartTime( int iSliceZ, int iPhase );

  /// get the end time of a slice
  int GetSliceEndTime( int iSliceZ, int iPhase );

  /// get the cycle in which the slice resides
  int GetCycle( int iSliceZ );

  /// copy all the signal data out into an externally supplied buffer
  void CopySignalData( System::IntPtr externalBuffer );

  /// get a single ECG sample
  int GetEcgSample( int iIndex ) { return m_pTimeVolume->GetECGSignal().GetSample( iIndex ); }

  /// get the selection at a slice (deprecated: should use GetSliceSelection instead)
  ///int Selection( int zSlice ) { return m_pTimeVolume->GetSelection( zSlice ); }

  /// set the selected phase at a slice (deprecated: should use SetSliceSelection)
  ///void set_Selection( int zSlice, int phase ) { m_pTimeVolume->SetSelection( zSlice, phase ); }

// properties
public:

  /// Selected volume property
  property Viatronix::Visualization::Volume ^ SelectedVolume
  {
    Viatronix::Visualization::Volume ^ get() { return m_mpSelectedVolume; }
  }

  /// Dimension property (read only).
  property Viatronix::Visualization::Triplet ^ Dimension
  {
    Viatronix::Visualization::Triplet ^ get();
  }

  /// Returns the number of phases in time.
  property int NumPhases { int get(); }

  /// Returns the number of Detectors,.
  property int NumDetectors { int get(); }

  /// Gets the number of ECG samples
  property int NumEcgSamples { int get() { return m_pTimeVolume->GetECGSignal().GetNumSamples(); }}

  /// Gets the sampling rate of the ECG in seconds (e.g., a new sample comes every 0.02 seconds)
  property float EcgSampleRateSeconds { float get() { return 1.0F / m_pTimeVolume->GetECGSignal().GetSampleRate(); }}

  /// The start time of the ECG data
  property float EcgStartTime { float get() { return 0.0F; }}

  /// Get the file path
  property System::String ^ FilePath { System::String ^ get() { return m_mpFilepath; }}

// fields
private:

  /// the path from which the file was read
  System::String ^ m_mpFilepath;

  /// the shared time volume
  vxShareableObject< vxTimeVolume > * m_pTimeVolume;

  /// selected volume ( managed class )
  Viatronix::Visualization::Volume ^ m_mpSelectedVolume;

  /// the observer
  vxManagedObserver * m_pObserver;

}; // TimeVolume


CLOSE_VISUALIZATION_NS


// $Log: v3D_TimeVolume.h,v $
// Revision 1.4  2006/10/04 18:29:06  gdavidson
// Ported to C++/CLI
//
// Revision 1.3  2006/09/27 17:23:49  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.2  2004/10/04 15:11:24  frank
// improved the extended elements modified interface
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.12  2003/12/22 12:41:27  mkontak
// Fix to allow compiling of DICOM
//
// Revision 1.11  2003/05/13 13:28:47  michael
// code review
//
// Revision 1.10  2003/05/07 13:51:41  frank
// code review
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_TimeVolume.h,v 1.4 2006/10/04 18:29:06 gdavidson Exp $
// $Id: v3D_TimeVolume.h,v 1.4 2006/10/04 18:29:06 gdavidson Exp $
