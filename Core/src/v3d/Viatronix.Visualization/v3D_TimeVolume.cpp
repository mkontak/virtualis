// $Id: v3D_TimeVolume.cpp,v 1.8 2006/10/04 18:29:06 gdavidson Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Frank Dachille (frank@viatronix.com)


// Includes
#include "StdAfx.h"
#include "v3D_TimeVolume.h"
#include "vxManagedObserver.h"
#include "vxUtils.h"
#include "vxMouseCursor.h"


// Namespaces
USING_VISUALIZATION_NS


// Defines
#define FILE_REVISION "$Revision: 1.8 $"


/**
 * Default constructor.
 */
TimeVolume::TimeVolume()
{
  m_pTimeVolume = new vxShareableObject< vxTimeVolume >();
  m_pObserver = new vxManagedObserver( this );
  m_pTimeVolume->Connect( m_pObserver );
  m_mpFilepath = System::String::Empty;
} // TimeVolume()


/**
 * Constructor from filename.
 *
 * @param filename the file to load
 */
TimeVolume::TimeVolume( System::String ^ mpFilename )
{
  m_pTimeVolume = new vxShareableObject< vxTimeVolume >();
  m_pObserver = new vxManagedObserver( this );
  m_pTimeVolume->Connect( m_pObserver );

  Read( mpFilename );
} // TimeVolume( filename )


/**
 * Constructor from dimensions.
 *
 * @param mpDimensions the desired dimensions
 */
TimeVolume::TimeVolume( Viatronix::Visualization::Triplet ^ mpDimensions )
{
  m_pTimeVolume = new vxShareableObject< vxTimeVolume >();
  m_pObserver = new vxManagedObserver( this );
  m_pTimeVolume->Connect( m_pObserver );
  Create( mpDimensions );
} // TimeVolume( filename )


/**
 * destructor.
 */
TimeVolume::~TimeVolume()
{
  this->!TimeVolume();
} // Finalizer()


/**
 * Finalizer.
 */
TimeVolume::!TimeVolume()
{
  m_pTimeVolume->Disconnect( m_pObserver );
  
  delete m_pTimeVolume; m_pTimeVolume = NULL;
  delete m_pObserver;  m_pObserver = NULL;
} // Finalizer()


/**
 * Returns the dimensions of the volume
 *
 * @return the dimensions
 */
Viatronix::Visualization::Triplet ^ TimeVolume::Dimension::get()
{
  vxPhaseVolume & phaseVol = m_pTimeVolume->GetPhaseVolumeContainer(0);
  vxBlockVolume<uint2> & vol = phaseVol.GetVolume();
  Triplei dim = vol.GetHeader().GetVolDim();
  return gcnew Viatronix::Visualization::Triplet( dim.X(), dim.Y(), dim.Z() );
} // get_Dimensions()


/**
 * Read from a vol file
 *
 * @param mpFilepath the file path
 */
void TimeVolume::Read( System::String ^ mpFilepath )
{
  try
  {
    m_mpFilepath = mpFilepath;

    m_pTimeVolume->Read( ss( mpFilepath ) );

    m_pTimeVolume->Modified( vxModInfo(vxTimeVolume, vxUtils::VOLUME_INTENSITY_INITIALIZED ) );

    m_mpSelectedVolume = gcnew Volume( VolumeType::Intensity, this->Dimension );
    vxBlockVolume<uint2> * pSelectedVolume = _native_ptr_t< vxBlockVolume<uint2> >( m_mpSelectedVolume->GetVolumePtr() );
    pSelectedVolume->SetHeader( m_pTimeVolume->GetPhaseVolumeContainer(0).GetVolume().GetHeader() );
    m_mpSelectedVolume->UpdateHistograms();

    m_pTimeVolume->SetSelectedVolume( pSelectedVolume );
    _native_ptr_t< vxBlockVolume< uint2 > >( m_mpSelectedVolume->GetVolumePtr() )->Modified(  vxModInfo( vxBlockVolume< uint2 >, vxUtils::VOLUME_INTENSITY_MODIFIED ) );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }

} // Read()


/**
 * Returns the number of phases in time
 *
 * @return the number of phases in time
 */
int TimeVolume::NumPhases::get()
{
  return m_pTimeVolume->GetNumOfPhases();
} // get_NumPhases()


/**
 * Returns the number of detectors in time
 *
 * @return the number of detectors in time
 */
int TimeVolume::NumDetectors::get()
{
  int iNumOfDectors = -1;
  
  try
  {
    iNumOfDectors = m_pTimeVolume->GetNumOfDetectors();
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }

  return iNumOfDectors;

} // get_NumDetectors()


/**
 * Writes a TimeVolume to disk.
 * 
 * @param mpFilepath  file path to TimeVolume file.
 */
void TimeVolume::Write( System::String ^ mpFilepath )
{
  try
  {
    m_pTimeVolume->Write( ss( mpFilepath ) );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // Write()


/**
 * Writes the selected phase volume to disk.
 *
 * @param mpFilepath file path to the selected volume
 */
void TimeVolume::WriteSelectedVolume( System::String ^ mpFilepath )
{
  try
  {
    m_pTimeVolume->WriteSelectedPhaseVolume( ss( mpFilepath ) );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // WriteSelectedVolume()


/**
 * Read in an ECG file from a GE scanner (normally a ".iri" file).
 *
 * @param mpFilepath the path to the ECG file
 */
void TimeVolume::ReadIRI( System::String ^ mpFilepath )
{
  try
  {
    m_pTimeVolume->ReadECGFile( ss( mpFilepath ) );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // ReadIRI()


/**
 * Creates a vxBlockVolume
 * 
 * @param mpDimensions the desired dimensions
 */
void TimeVolume::Create( Viatronix::Visualization::Triplet ^ dimensions )
{
  throw ex::AbortOperationException( LogRec( "unimplemented", "TimeVolume", "Create" ) );
} // Create()


///**
// * Called when the state of the volume is modified.
// *
// * @param modifiedInfoList the list of modified messages
// */
//void TimeVolume::Modified( System::IntPtr modifiedInfoList )
//{
//  vxModifiedInfoList * pInfoList = reinterpret_cast< vxModifiedInfoList * >( modifiedInfoList.ToPointer() );
//
//  // iterate through information
//  for (const vxModifiedInfo * pInfo = pInfoList->Begin(); pInfo != NULL; pInfo = pInfoList->Next())
//  {
//    // retrieve modified flag
//    uint4 uFlag(pInfo->GetFlag());
//
//    // update intensity histogram
//    if (uFlag & vxUtils::VOLUME_INTENSITY_INITIALIZED || uFlag & vxUtils::VOLUME_INTENSITY_MODIFIED)
//    {
//      //vxMouseCursor cursor("CURSOR_WAIT");
//      //m_pVolumeHistogram->Update( *m_pBlockVolume ); 
//    }
//  }
//} // Modified()



/**
 * copy all the signal data out into an externally supplied buffer
 *
 * @param externalBuffer the externally supplied buffer
 *
 * @return 
 */
void TimeVolume::CopySignalData( System::IntPtr externalBuffer )
{
  try
  {
    int4 * pSignalData = reinterpret_cast< int4 * >( externalBuffer.ToPointer() );
    m_pTimeVolume->GetECGSignal().CopySignalData( pSignalData );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // CopySignalData()


/**
 * get a pointer to the managed volume
 *
 * @param iIndex the desired volume
 */
Volume ^ TimeVolume::GetVolume( int iIndex )
{
  Volume ^ mpVolume = gcnew Volume( VolumeType::Intensity, System::IntPtr( &( m_pTimeVolume->GetPhaseVolume( iIndex ))));
  mpVolume->UpdateHistograms();
  return mpVolume;
} // GetVolume()


/**
 * set the selected phase at a slice
 *
 * @param iSliceZ the selected slice
 *
 * @return 
 */
int TimeVolume::GetSliceSelection( int iSliceZ ) 
{ 
  int iSliceSelection = -1;
  try
  {
    iSliceSelection =  m_pTimeVolume->GetSelection( iSliceZ ); 
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
  return iSliceSelection;
} // GetSliceSelection()


/**
 * set the selected phase at a slice
 *
 * @param iSliceZ the selected slice
 * @param iPhase  the selected phase
 */
void TimeVolume::SetSliceSelection( int iSliceZ, int iPhase )
{
  try
  {
    m_pTimeVolume->SetSelection( iSliceZ, iPhase );

    _native_ptr_t< vxBlockVolume< uint2 > >( m_mpSelectedVolume->GetVolumePtr() )->Modified(  vxModInfo( vxBlockVolume< uint2 >, vxUtils::VOLUME_INTENSITY_MODIFIED ) );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // SetSliceSelection()


/**
 * get the start time of a slice
 *
 * @param iSliceZ the selected slice
 * @param iPhase  the selected phase
 *
 * @return the start time
 */
int TimeVolume::GetSliceStartTime( int iSliceZ, int iPhase )
{
  int iSliceStartTime = -1;
  try
  {
    iSliceStartTime = m_pTimeVolume->GetECGSignal().GetReconPeriodStart( GetCycle( iSliceZ ) );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
  return iSliceStartTime;
} // GetSliceStartTime()


/**
 * get the end time of a slice
 *
 * @param iSliceZ the selected slice
 * @param iPhase  the selected phase
 *
 * @return the end time
 */
int TimeVolume::GetSliceEndTime( int iSliceZ, int iPhase )
{
  int iSliceEndTime = -1;
  try
  {
    iSliceEndTime = m_pTimeVolume->GetECGSignal().GetReconPeriodEnd( GetCycle( iSliceZ ) );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
  return iSliceEndTime;
} // GetSliceEndTime()


/**
 * get the cycle in which the slice resides
 *
 * @param iSliceZ the selected slice
 *
 * @return the cycle in which the slice resides
 */
int TimeVolume::GetCycle( int iSliceZ )
{
  return iSliceZ / m_pTimeVolume->GetNumOfDetectors();  
} // GetCycle()


/**
 * Set all the slices to the given phase ( could be -1 to clear all )
 *
 * @param iPhase the desired phase
 */
void TimeVolume::ResetSelection( int iPhase )
{
  try
  {
    m_pTimeVolume->ResetSelections( iPhase );
    _native_ptr_t< vxBlockVolume< uint2 > >( m_mpSelectedVolume->GetVolumePtr() )->Modified(  vxModInfo( vxBlockVolume< uint2 >, vxUtils::VOLUME_INTENSITY_MODIFIED ) );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // ResetSelection()

#pragma managed

#undef FILE_REVISION

// $Log: v3D_TimeVolume.cpp,v $
// Revision 1.8  2006/10/04 18:29:06  gdavidson
// Ported to C++/CLI
//
// Revision 1.7  2006/09/27 17:23:49  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.6  2006/06/07 15:52:28  geconomos
// corrected compilation errors in debug mode
//
// Revision 1.5  2006/04/03 16:48:08  geconomos
// + added  histogram updater as template argument
//
// Revision 1.4  2006/01/20 18:09:00  geconomos
// continued migration to visual studio 2005
//
// Revision 1.3  2004/10/29 20:18:46  frank
// formatting
//
// Revision 1.2  2004/10/04 15:13:04  frank
// improved the extended elements modified interface
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.20  2004/03/02 01:10:43  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.19.4.1  2004/02/24 20:12:14  michael
// coding standard: assign NULL after deletion and m_pOberserver --> m_pObserver
//
// Revision 1.19  2003/06/18 15:19:46  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.18  2003/06/09 18:35:36  geconomos
// Fixed typos when setting the volumes as modified.
//
// Revision 1.17  2003/05/07 13:51:41  frank
// code review
//
// Revision 1.16  2003/04/01 16:02:08  geconomos
// Updated to public interface of managed volume class.
//
// Revision 1.15  2003/03/31 18:57:40  michael
// Completed first step of cleaning up the event mechanism. The events are now
// mostly in vxUtils, named vxEvent???Enum and some day we might be able to
// move to event classes that are more flexible ...
//
// Revision 1.14  2003/01/22 22:09:26  ingmar
// dimension and units are now Triples
//
// Revision 1.13  2002/10/10 14:27:34  frank
// Working on updating the 3D view when you go to retrospective preview.
//
// Revision 1.12  2002/10/08 15:31:53  frank
// Added $Log
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_TimeVolume.cpp,v 1.8 2006/10/04 18:29:06 gdavidson Exp $
// $Id: v3D_TimeVolume.cpp,v 1.8 2006/10/04 18:29:06 gdavidson Exp $
