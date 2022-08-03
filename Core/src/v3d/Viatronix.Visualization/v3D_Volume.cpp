// $Id: v3D_Volume.cpp,v 1.12 2006/10/04 18:29:06 gdavidson Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)


// includes
#include "StdAfx.h"
#include "v3D_Volume.h"
#include "vxManagedObserver.h"
#include "v3D_CropBox.h"
#include "v3D_Events.h"
#include "vxInputAdapter.h"
#include "vxUtils.h"
#include "vxMouseCursor.h"
#include "Resampler.h"
#include "vxProgressBar.h"
#include "BaseHistogram.h"
#include "vxBlockVolumeFileReader.h"
#include "vxHistogramUpdaterIntensity.h"
#include "vxHistogramUpdaterLabel.h"
#include "Timer.h"

// defines
#define FILE_REVISION "$Revision: 1.12 $"


// namepsaces
USING_VISUALIZATION_NS


/**
 * Initializer from datatype
 *
 * @param eType volume's type
 */
Volume::Volume( VolumeType eType )
{
  CommonInit( eType );
} // Volume( eType )


/**
 * Initializer from datatype and volfile.
 *
 * @param eType volume's type
 * @param mpFilename volume file to load
 */
Volume::Volume( VolumeType eType, System::String ^ mpFilename )
{
  CommonInit( eType );
  
  Read( mpFilename );
} // Volume( eType, mpFilename )


/**
 *
 */
Volume::Volume( VolumeType eType, System::String ^ mpFilename, bool bProgress )
{
  CommonInit( eType );
  
  m_bProgress = bProgress;

  Read( mpFilename );

} // Volume( VolumeType eType, System::String ^ mpFilename, bool bProgress );


/**
 * Initializer from datatype and dimensions.
 *
 * @param eDatatpe volume's type
 * @param mpDimensions volume dimensions
 */
Volume::Volume( VolumeType eType, Triplet ^ mpDimensions )
{
  CommonInit( eType );
  
  Create( mpDimensions );
} // Volume( eType, mpDimensions )


/**
 * Initializer from type and vxBlockVolume pointer.
 *
 * @param eType volume's type
 * @param volume vxBlockVolume pointer
 */
Volume::Volume( VolumeType eType, System::IntPtr volume )
{   
  this->ThreadSafety = true;
  m_bProgress = true;
  
  vxBlockVolume<uint2> * pBlockVolume = reinterpret_cast< vxBlockVolume<uint2> * >( volume.ToPointer() );
  
  // make a copy of the volume
  m_pBlockVolume = new vxShareableObject<vxBlockVolume<uint2> >( * pBlockVolume );
  
  UpdateHistograms();

  // managed types
  m_mpHeader = gcnew VolumeHeader( System::IntPtr( &m_pBlockVolume->GetHeader() ) );

  m_pObserver = new vxManagedObserver( this );
  m_pBlockVolume->Connect( m_pObserver );
  m_bValid = true;
  m_eType = eType;

  if ( m_eType == VolumeType::Intensity )
    m_pBlockVolume->Modified( vxModInfo(  vxBlockVolume<uint2>, vxUtils::VOLUME_INTENSITY_INITIALIZED ) );
  else
    m_pBlockVolume->Modified( vxModInfo(  vxBlockVolume<uint2>, vxUtils::VOLUME_LABEL_INITIALIZED ) );

  // managed types
  Triple<uint1> color = m_colors->GetColor();
  m_color = System::Drawing::Color::FromArgb( color.X(), color.Y(), color.Z() );
} // constructor


/**
 * destructor.
 */
Volume::~Volume()
{
  this->!Volume();
} // ~Volume()


/**
 * Finalizer.
 */
Volume::!Volume()
{
  m_pBlockVolume->Disconnect( m_pObserver );

  delete m_pBlockVolume;     m_pBlockVolume = NULL;
  delete m_pObserver;       m_pObserver = NULL;
} // !Volume()



/**
 * Common initialization routine for volume.
 *
 * @param eType volume datatype
 */
void Volume::CommonInit( VolumeType eType )
{
  this->ThreadSafety = true;
  
  m_pBlockVolume = new vxShareableObject< vxBlockVolume< uint2 > >();
  
  m_pBlockVolume->GetHeader().SetVolDim( 2, 2, 2 ); // set default size (empty volume)

  UpdateHistograms();
  
  // create the and connect observer
  m_pObserver = new vxManagedObserver( this );
  m_pBlockVolume->Connect( m_pObserver );
  
  m_bValid = false;
  m_eType = eType;
  m_mpFilename = System::String::Empty;

  // managed types
  m_mpHeader = gcnew VolumeHeader(System::IntPtr(&m_pBlockVolume->GetHeader()));

  Triple<uint1> color = m_colors->GetColor();
  m_color = System::Drawing::Color::FromArgb( color.X(), color.Y(), color.Z() );
  m_bProgress = true;
} // CommonInit()


/**
 * Gets the volume dimensions of the volume.
 *
 * @return volume dimensions
 */
Triplet ^ Volume::Dimensions::get()
{
  try
  {
    Triplei dim(m_pBlockVolume->GetHeader().GetVolDim());
    return gcnew Triplet( dim.X(), dim.Y(), dim.Z() );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // get_Dimensions()


/**
 * Gets the world dimensions of the volume.
 *
 * @return world dimensions
 */
Triplet ^ Volume::WorldDimensions::get()
{
  try
  {
    Triplef dim(m_pBlockVolume->GetHeader().GetWorldDim());
    return gcnew Triplet( dim.X(), dim.Y(), dim.Z() );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // get_WorldDimensions()


/**
 * Gets the units of the volume
 *
 * @return units
 */
Triplet ^ Volume::Units::get()
{
  try
  {
    Triplef tmp(m_pBlockVolume->GetHeader().GetVoxelUnits());
    return gcnew Triplet( tmp.X(), tmp.Y(), tmp.Z() );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // get_Units()


/**
 * Sets the units of the volume.
 *
 * @param mpUnit New units for volume.
 */
void Volume::Units::set( Triplet ^ mpUnits )
{
  try
  {
    m_pBlockVolume->GetHeader().SetVoxelUnits( Triplef( mpUnits->x, mpUnits->y, mpUnits->z ) );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // set_Units()


/**
 * Reads the specified volfile.
 *
 * @param mpFilepath Volfile to load.
 */
void Volume::Read( System::String ^ mpFilepath )
{
  Read( mpFilepath, 0.0F, 100.0F );
} // Read( mpFilepath )


void ClearBuffer( const std::string & s )
{
  DWORD dwFlags( FILE_FLAG_NO_BUFFERING | FILE_FLAG_SEQUENTIAL_SCAN );
  HANDLE hFile = CreateFile( s.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, dwFlags, NULL );
  if( hFile )
    CloseHandle( hFile );
}

/**
 * Reads a volume from disk and choose to update the progress meter.
 *
 * @param mpFilepath Volfile to load.
 * @param fPercentBegin Percentage when read operations starts.
 * @param fPercentEnd Percentage when read operations ends.
 */
void Volume::Read( System::String ^ mpFilepath, float4 fPercentBegin, float4 fPercentEnd )
{
  try
  {   
    if( !m_bProgress )
      fPercentBegin = -1.0F;

    //ClearBuffer( ss( mpFilepath ) );

    Timer t;

    if( m_eType == VolumeType::Intensity )
    {
       vxBlockVolumeFileReader< uint2, vxHistogramUpdaterIntensity >::Read( ss( mpFilepath ), * m_pBlockVolume, std::make_pair( fPercentBegin, fPercentEnd ) );
    }
    else
    {
      vxBlockVolumeFileReader< uint2, vxHistogramUpdaterLabel >::Read( ss( mpFilepath ), * m_pBlockVolume, std::make_pair( fPercentBegin, fPercentEnd ) );
    }

    LogDbg( "Load volume took... " + ToAscii( t.ElapsedTime() ), "", "" );

    m_pBlockVolume->GetHeader().SetFileName( ss( mpFilepath ) );
   
    if( m_eType == VolumeType::Intensity )
      m_pBlockVolume->Modified( vxModInfo( vxBlockVolume<uint2>, vxUtils::VOLUME_INTENSITY_INITIALIZED ) );
    else
      m_pBlockVolume->Modified( vxModInfo( vxBlockVolume<uint2>, vxUtils::VOLUME_LABEL_INITIALIZED ) );

    m_bValid = true;
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // Read( mpFilepath, fPercentBegin, fPercentEnd )


/**
 * Writes a Volume to disk.
 * 
 * @param filepath  File path to Volume file.
 */
void Volume::Write( System::String ^ mpFilepath )
{
  try
  {
    m_pBlockVolume->Write( ss( mpFilepath ) );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // Write( )


/**
 * Creates a vxBlockVolume.
 * 
 * @param mpDimensions dimensions of volume
 */
void Volume::Create( Triplet ^ mpDimensions )
{
  try
  {
    vxShareableObject< Triple< float4 > > * pDim = ToNativeTriplePtr( mpDimensions->GetElementPtr() );
    
    m_pBlockVolume->GetHeader().SetVolDim( Triplei( pDim->m_x,pDim->m_y, pDim->m_z ) );

    if ( m_eType == VolumeType::Intensity )
      m_pBlockVolume->Modified( vxModInfo( vxBlockVolume<uint2> , vxUtils::VOLUME_INTENSITY_INITIALIZED ) );
    else
      m_pBlockVolume->Modified( vxModInfo( vxBlockVolume<uint2>, vxUtils::VOLUME_LABEL_INITIALIZED ) );

    m_bValid = true;
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // Create()


/**
 * Gets the specified volume header variable.
 *
 * @param eVariable Name of variable to retrieve
 * @return header variable value
 */
System::String ^ Volume::GetHeaderVariable( HeaderVariables eVariable )
{
  try
  {
    std::string sValue;
    m_pBlockVolume->GetHeader().GetHeaderEntry( static_cast< vxVolumeHeader::VarVolHeaderEntryEnum>( eVariable ), sValue );
    return gcnew System::String( sValue.c_str() );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // GetHeaderVariable()


/**
 * Gets the specified volume header variable.
 *
 * @param mpsGroup DICOM group
 * @param mpsField DICOM field
 * @return header variable value
 */
System::String ^ Volume::GetHeaderDicomElementValue(System::String ^ mpsGroup, System::String ^ mpsField)
{
  try
  {
    std::string sGroup = ss(mpsGroup);
    std::string sField = ss(mpsField);
    std::string sValue;

    sValue = m_pBlockVolume->GetHeader().GetDicomFieldValue(sGroup, sField, sValue);

    return gcnew System::String( sValue.c_str() );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // GetHeaderDicomElementValue()


/** 
 * Gets the current thread safety policy.
 *
 * @return thread saftey flag
 */
bool Volume::ThreadSafety::get()
{
  return vxBlockStatic::IsThreadSafe();
} // get_ThreadSafety()


/** 
 * Sets the current thread safety policy.
 *
 * @param bIsThreadSafe thread safety flag
 */
void Volume::ThreadSafety::set( bool bIsThreadSafe )
{
  vxBlockStatic::SetThreadSafety( bIsThreadSafe );
} // set_ThreadSafety()


/** 
 * Creates the required histograms
 */
void Volume::UpdateHistograms() 
{ 
  try
  {
    vxMouseCursor cursor( "CURSOR_WAIT" );
    
    if ( m_eType == VolumeType::Label )
    {
      vxHistogramUpdaterLabel< uint2 >::Update( *m_pBlockVolume );
    }      
    else
    {
      vxHistogramUpdaterIntensity< uint2 >::Update( *m_pBlockVolume );
    }
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // set_CreateHistogram()



/** 
 * Connects an observer to the object.
 *
 * @param observer vxManagedObserver object interested in events.
 */
void Volume::Connect( System::IntPtr observer )
{
  m_pBlockVolume->Connect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Connect()


/** 
 * Connects an observer to the object.
 *
 * @param observer vxManagedObserver object interested in events.
 */
void Volume::Disconnect( System::IntPtr  observer )
{
  m_pBlockVolume->Disconnect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Disconnect()


/**
 * Processes modified queue events.
 * 
 * @param modifiedList Modified info list.
 */
void Volume::OnModified( System::IntPtr modifiedList )
{
  vxModifiedInfoList * pInfoList = reinterpret_cast< vxModifiedInfoList * >( modifiedList.ToPointer() );
  
  bool bUpdateHistograms( false );
  for( const vxModifiedInfo * pInfo( pInfoList->Begin() ); pInfo != NULL; pInfo = pInfoList->Next() )
  {
    // retrieve modified flag
    uint4 uFlag( pInfo->GetFlag() );

    if( uFlag & vxUtils::VOLUME_LABEL_MODIFIED )
    {      
      Changed( this, System::EventArgs::Empty );
      bUpdateHistograms |= true;
    }
      
    // do we need to update the base histogram?
    if( uFlag & vxUtils::VOLUME_INTENSITY_MODIFIED )
      bUpdateHistograms |= true;
  }

  if( bUpdateHistograms )
    UpdateHistograms();
} // OnModified()


/**
 * Create a new volume by intersection with a crop box.
 *
 * @param mpCropBox      Crop box to intersect with.
 * @param mpModification the modification string (aim to be unique)
 * @return New Volume instance.
 */
Viatronix::Visualization::Volume ^ Volume::Crop( CropBox ^ mpCropBox, System::String ^ mpModification )
{

  vxProgressBar::SetProgress( 20.0F, "cropping data" );
  vxMouseCursor waitCursor( "CURSOR_WAIT" );

  Volume ^ mpDestinationVolume = gcnew Volume( VolumeType::Intensity );
  
  vxShareableObject< vxCropBox > * pCropBox(ToNativeCropBoxPtr( mpCropBox->GetCropBoxPtr() ));

  vxBlockVolume<uint2> * pIntensityVol(ToNativeVolumeUint2Ptr( GetVolumePtr() ));
  vxBlockVolume<uint2> * pDestinationVolume(ToNativeVolumeUint2Ptr( mpDestinationVolume->GetVolumePtr() ));

  // get the bounds of the cropping
  Point<float4> startPoint( pCropBox->GetBox().GetMinPoint().GetDividedAnIso( pIntensityVol->GetHeader().GetVoxelUnits() ) );
  Point<float4> endPoint  ( pCropBox->GetBox().GetMaxPoint().GetDividedAnIso( pIntensityVol->GetHeader().GetVoxelUnits() ) );

  // set the header
  pDestinationVolume->SetHeader( pIntensityVol->GetHeader() );
  pDestinationVolume->GetHeader().SetVolDim( endPoint.m_x - startPoint.m_x + 1, endPoint.m_y - startPoint.m_y + 1, endPoint.m_z - startPoint.m_z + 1 );

  // copy the voxels
  vxBlockVolumeIterator<uint2> destIter( * pDestinationVolume );
  vxBlockVolumeIterator<uint2> srcIter( * pIntensityVol );
  for ( ; destIter.IsNotAtEnd(); destIter.NextBlockZYX() )
  {
    for ( ; destIter.IsNotAtEndOfBlock(); destIter.NextZYXinsideBlock() )
    {
      Vector3Df resamplePos( Vector3Df( destIter.GetPos() ) + Vector3Df( startPoint ) );
      srcIter.SetPos( Vector3Di(Vector3Df(resamplePos)) );
      destIter.SetVoxel( srcIter.GetVoxel() );
    }
  }

  UpdateModificationString( mpDestinationVolume, ss( mpModification ) );
  
  vxProgressBar::SetProgress( 80.0F, "analyzing data" );

  mpDestinationVolume->UpdateHistograms();

  vxProgressBar::SetProgress( 100.0F, "done cropping" );

  vxInputAdapter::CheckMemory();

  return mpDestinationVolume;

} // Crop()


/**
 * Create a new isotropic volume by intersection with a crop box.
 *
 * @param mpCropBox      the crop box to intersect with
 * @param mpModification the modification string (aim to be unique)
 * @return new Volume instance.
 */
Viatronix::Visualization::Volume ^ Volume::CropIsotropic( CropBox ^ mpCropBox, System::String ^ mpModification )
{
  vxProgressBar::SetProgress( 20.0F, "cropping data" );
  vxMouseCursor waitCursor( "CURSOR_WAIT" );

  Volume ^ mpDestinationVolume = gcnew Volume( VolumeType::Intensity );

  vxShareableObject< vxCropBox > * pCropBox(ToNativeCropBoxPtr( mpCropBox->GetCropBoxPtr() ));

  vxBlockVolume<uint2> * pIntensityVol(ToNativeVolumeUint2Ptr( GetVolumePtr() ));
  vxBlockVolume<uint2> * pDestinationVolume(ToNativeVolumeUint2Ptr( mpDestinationVolume->GetVolumePtr() ));

  // get the bounds of the cropping
  Point<float4> startPoint( pCropBox->GetBox().GetMinPoint().GetDividedAnIso( pIntensityVol->GetHeader().GetVoxelUnits() ) );
  Point<float4> endPoint  ( pCropBox->GetBox().GetMaxPoint().GetDividedAnIso( pIntensityVol->GetHeader().GetVoxelUnits() ) );

  float4 fScalingFactor( 0.55F );
  Resampler::BoxCutRun( pIntensityVol, pDestinationVolume, startPoint, endPoint, fScalingFactor );

  UpdateModificationString( mpDestinationVolume, ss( mpModification ) );

  vxProgressBar::SetProgress( 80.0F, "analyzing data" );

  mpDestinationVolume->UpdateHistograms();

  vxProgressBar::SetProgress( 100.0F, "done cropping" );

  vxInputAdapter::CheckMemory();

  return mpDestinationVolume;

} // CropIsotropic()


/**
 * Create a new volume based on the visible components only
 *
 * @param mpLabelVolume  the label volume
 * @param mpComponents   the components that describe the label volume
 * @param mpModification the modification string (aim to be unique)
 * @return new Volume instance
 */
Viatronix::Visualization::Volume ^ Volume::GetVolumeFromVisible( Volume ^ mpLabelVolume, Components ^ mpComponents,
                                                                      System::String ^ mpModification )
{

  vxProgressBar::SetProgress( 20.0F, "generating data" );
  vxMouseCursor waitCursor( "CURSOR_WAIT" );

  Volume ^ mpDestinationVolume = gcnew Volume( Viatronix::Visualization::VolumeType::Intensity );

  vxBlockVolume<uint2>  * pIntensityVol(ToNativeVolumeUint2Ptr ( GetVolumePtr() ));
  vxBlockVolume<uint2> * pLabelVol(ToNativeVolumeUint2Ptr( mpLabelVolume->GetVolumePtr() ));
  vxBlockVolume<uint2>  * pDestinationVol(ToNativeVolumeUint2Ptr ( mpDestinationVolume->GetVolumePtr()  ));
  vxComponentArray     * pComponents(ToNativeComponentsPtr ( mpComponents->GetElementPtr() ));

  if ( pIntensityVol == NULL || pDestinationVol == NULL || pLabelVol == NULL || pComponents == NULL )
  {
    LogFyi(" No Original Volume or Destination Volume ", "Volume", "GetVolumeFromVisible" );
    return mpDestinationVolume;
  }

  // set the header
  pDestinationVol->SetHeader( pIntensityVol->GetHeader() );

  // setup the iterators
  vxBlockVolumeIterator<uint2> destIter( * pDestinationVol );
  vxBlockVolumeIterator<uint2> srcIter( * pIntensityVol );
  vxBlockVolumeIterator<uint2> labelIter( * pLabelVol );
  const std::vector<vxComponent *> * pvComponents(& pComponents->ArrayByLabel());

  // copy the voxels
  for ( ; destIter.IsNotAtEnd(); destIter.NextBlockZYX(), srcIter.NextBlockZYX(), labelIter.NextBlockZYX() )
  {
    for ( ; destIter.IsNotAtEndOfBlock(); destIter.NextZYXinsideBlock(), srcIter.NextZYXinsideBlock(), labelIter.NextZYXinsideBlock() )
    {
      if ( (* pvComponents)[ labelIter.GetVoxel() ]->IsIntensityVisible() )
        destIter.SetVoxel( srcIter.GetVoxel() );
    }
  }

  UpdateModificationString( mpDestinationVolume, ss( mpModification ) );

  vxProgressBar::SetProgress( 80.0F, "analyzing data" );

  mpDestinationVolume->UpdateHistograms();

  vxProgressBar::SetProgress( 100.0F, "done analyzing" );

  vxInputAdapter::CheckMemory();

  return mpDestinationVolume;

} // GetVolumeFromVisible()


/**
* Update the modification string to include the given edit operation
 *
 * @param mpVolume the volume to modify
 * @param sModification the string to add to the end of the modification string
 */
void Volume::UpdateModificationString( Volume ^ mpVolume, const std::string & sModification )
{
  vxVolumeHeader & volumeHeader(ToNativeVolumeInt2Ptr( mpVolume->GetVolumePtr() )->GetHeader());

  VarMap varMap(volumeHeader.GetVariableVolumeHeader());
  std::string sModificationString;
  varMap.GetHdrVar( "ModificationString", sModificationString );

  if ( sModificationString.length() != 0 )
  {
    sModificationString += ",";
  }
  sModificationString += sModification;
  varMap.SetHdrVar( "ModificationString", sModificationString, "modifications to the data" );
  volumeHeader.SetVariableVolumeHeader( varMap );
} // UpdateModificationString()


// Revision history:
// $Log: v3D_Volume.cpp,v $
// Revision 1.12  2006/10/04 18:29:06  gdavidson
// Ported to C++/CLI
//
// Revision 1.11  2006/09/27 17:23:49  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.10  2006/09/27 17:06:28  geconomos
// updated for class name changes to InutAdapter and ViewerImpl
//
// Revision 1.9  2006/04/11 13:38:49  geconomos
// added call to static read function for block volume read
//
// Revision 1.8  2006/04/03 16:48:08  geconomos
// + added  histogram updater as template argument
//
// Revision 1.7  2006/03/29 20:12:43  geconomos
// + removed vxBlockVolumeBinaryHistogram related code
// + added new file reading mechanism
//
// Revision 1.6  2006/01/27 15:47:47  geconomos
// still migrating to visual studio 2005
//
// Revision 1.5  2006/01/20 18:09:00  geconomos
// continued migration to visual studio 2005
//
// Revision 1.4  2005/11/18 21:12:28  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.3.6.1  2005/07/05 17:53:14  frank
// spelling
//
// Revision 1.3  2004/11/03 17:16:00  geconomos
// cleanup
//
// Revision 1.2  2004/04/26 19:09:15  michael
// Adjusted code to reflect the changes in zomm factor, roi not being part of
// camera anymore, etc. and moved serialization of preset from V3D Preset to
// use the one in vxSerializer (removed duplicate code and fixed errors)
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.69  2004/03/02 01:10:42  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.68.2.1  2004/02/24 20:12:53  michael
// took out code that was commented out
//
// Revision 1.68  2004/02/05 13:37:30  frank
// checked the free memory after memory-intensive operations
//
// Revision 1.67  2003/12/16 13:27:52  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.66  2003/09/09 19:16:34  dongqing
// Stopped caching the filename and retrieve from header instead - remove m_mpFilename when this is proven acceptable
//
// Revision 1.65  2003/09/05 14:59:03  michael
// merged the branch "XP1_1" back into the trunc
//
// Revision 1.64  2003/08/20 14:28:56  michael
// added "mailto:"
//
// Revision 1.63  2003/08/12 18:37:53  frank
// Issue #3447: Fixed problem with creating new datasets then visualizing them
//
// Revision 1.62  2003/08/08 19:51:05  wenli
// Add EventHandler Change();
//
// Revision 1.61.2.1  2003/08/12 18:42:04  frank
// Issue #3447: Fixed problem with creating new datasets then visualizing them
//
// Revision 1.61  2003/08/06 15:09:13  michael
// took out log messages and added flush of modified queue when histogram
// thread is done and the histogram is modified.
//
// Revision 1.60  2003/07/31 11:07:53  michael
// added log message when event of histogram completed is received
//
// Revision 1.59  2003/07/30 18:41:23  michael
// cosmetics
//
// Revision 1.58  2003/07/30 15:41:46  michael
// fixed typo hisogram -> histogram
//
// Revision 1.57  2003/07/30 13:16:32  geconomos
// Reworked threading of base histogram.
//
// Revision 1.56  2003/06/18 15:19:46  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.55  2003/06/09 18:35:36  geconomos
// Fixed typos when setting the volumes as modified.
//
// Revision 1.54  2003/06/04 18:30:21  michael
// added VolumeHeader
//
// Revision 1.53  2003/06/03 20:28:14  michael
// added default volume size
//
// Revision 1.52  2003/06/03 12:25:07  michael
// added Angio (XA) presets for Shimadzu that are sensitive to a specific DICOM entry
//
// Revision 1.51  2003/06/02 13:18:53  geconomos
// Made the histogram thread a background thread.
//
// Revision 1.50  2003/05/20 14:46:57  frank
// Made the edit and crop labels be more specific on the GUI
//
// Revision 1.49  2003/05/16 13:13:57  frank
// formatting
//
// Revision 1.48  2003/05/14 13:24:54  frank
// Simplified code
//
// Revision 1.47  2003/05/14 13:08:25  frank
// code review
//
// Revision 1.46  2003/05/14 12:34:01  michael
// code review
//
// Revision 1.45  2003/05/13 19:50:22  frank
// Refactored code
//
// Revision 1.44  2003/05/13 17:10:52  geconomos
// Coding conventions.
//
// Revision 1.43  2003/05/13 14:25:08  frank
// Added progress bar to crop volume
//
// Revision 1.42  2003/05/13 13:28:47  michael
// code review
//
// Revision 1.41  2003/05/05 17:04:29  frank
// Issue #3163: Added wait cursor to new volume operations
//
// Revision 1.40  2003/04/14 19:42:42  frank
// Exposed progress metering to managed classes
//
// Revision 1.39  2003/04/14 16:44:56  frank
// Separated progress percent and progress message interfaces
//
// Revision 1.38  2003/04/09 20:17:28  geconomos
// Updated progress bar to have status text.
//
// Revision 1.37  2003/04/09 15:06:24  michael
// further step in cleaning up the semgentation/component handler code. In most
// locations there now is a IndexHistory and IndexLabel to better differentiate what
// is used. E.g. SegmentationVolumes and renderers use IndexLabel but internally
// the segmentation/component handler mostly works on HistoryIndex (component pane).
//
// Revision 1.36  2003/04/07 17:12:33  frank
// Added a modification string to the volume.  This captures the sequence of modifications done to the volume since creation from original DICOM.
//
// Revision 1.35  2003/04/01 16:01:43  geconomos
// Code cleanup and threaded base histogram creation.
//
// Revision 1.34  2003/03/31 19:42:04  michael
// vxBinaryHistogram is not computed for the XZPoint volume of Vascular
//
// Revision 1.33  2003/03/31 18:57:40  michael
// Completed first step of cleaning up the event mechanism. The events are now
// mostly in vxUtils, named vxEvent???Enum and some day we might be able to
// move to event classes that are more flexible ...
//
// Revision 1.32  2003/03/14 14:01:38  geconomos
// Changed the return type for the BinaryHistogram to IntPtr
//
// Revision 1.31  2003/03/13 20:35:27  frank
// Synchronized m_filename when it changes
//
// Revision 1.30  2003/03/13 16:30:46  frank
// Added VisibleComponentExport
//
// Revision 1.29  2003/03/07 20:34:55  geconomos
// Changed Create method to use managed method GetElementPtr for class
// Triplet
//
// Revision 1.28  2003/02/13 16:56:46  jmeade
// Merge-to-Main_BeforeBranch_VC_1-3_to_UpdateAfterBranch_VC_1-3.
//
// Revision 1.27  2003/02/06 20:07:23  frank
// Added new read function that doesn't automatically show the progress events
//
// Revision 1.26.2.1  2003/02/12 23:01:35  jmeade
// Merged changes between main-branch tags "BeforeBranch_VC_1-3" and "UpdateAfterBranch_VC_1-3" that should've been in VC_1-3 branch.
//
// Revision 1.26  2003/01/22 22:10:01  ingmar
// dimension and units are now Triples
//
// Revision 1.25  2003/01/09 19:04:35  geconomos
// Coding conventions.
//
// Revision 1.24  2002/12/19 21:05:20  geconomos
// Fixed bug with updating the binary histogram on constructor
// from vxBlockVolume.
//
// Revision 1.23  2002/11/27 21:52:12  frank
// Added Block Destroy Failed warning for after RSNA fix
//
// Revision 1.22  2002/11/27 18:50:36  geconomos
// Added progress for volume loading.
//
// Revision 1.21  2002/11/22 03:32:01  frank
// Updated name of cropped volume
//
// Revision 1.20  2002/11/20 20:26:15  frank
// Turned thread safety back on until the crashes + log are fixed.
//
// Revision 1.19  2002/11/20 13:47:51  geconomos
// Defaulted "ThreadSafety" to false ( as per Michael )
//
// Revision 1.18  2002/11/19 20:39:15  geconomos
// Added creation of histogram nad binary historgam in the Crop and CropIsotropic functions.
//
// Revision 1.17  2002/11/15 18:05:06  geconomos
// Added assigned color.
//
// Revision 1.16  2002/11/14 20:27:12  geconomos
// Set filename property in write()
//
// Revision 1.15  2002/11/12 22:38:40  geconomos
// initializtionm of base histogram.
//
// Revision 1.14  2002/11/12 21:32:22  geconomos
// Initializtion of base histogram.
//
// Revision 1.13  2002/11/08 14:04:15  geconomos
// Added WorldDimensions property.
//
// Revision 1.12  2002/11/06 19:54:55  frank
// test
//
// Revision 1.11  2002/11/06 19:35:42  frank
// Added cropping without resampling.
//
// Revision 1.10  2002/11/05 19:51:27  frank
// Implemented cropping
//
// Revision 1.9  2002/11/05 18:56:40  frank
// Added Crop function
//
// Revision 1.8  2002/10/31 12:48:51  geconomos
// Added property for volume filepath.
//
// Revision 1.7  2002/10/18 21:39:52  wenli
// fix Uint2 volume read and write
//
// Revision 1.6  2002/10/17 22:26:43  ingmar
// added ThreadSafety property
//
// Revision 1.5  2002/10/08 21:52:05  jaddonisio
// Fixes per George:
//  i. Call modified based on type
// ii. Fix logical error in if statements of datatype
//
// Revision 1.4  2002/10/07 15:20:34  geconomos
// Merged Volume_Int2 and Volume_uint2 into Volume.
//
// Revision 1.15  2002/09/24 15:16:26  geconomos
// Exception handling and formatting.
//
// Revision 1.14  2002/09/24 14:45:04  geconomos
// Exception handling and formatting.
//
// Revision 1.13  2002/08/26 19:31:17  geconomos
// Added dicom header support.
//
// Revision 1.12  2002/08/16 18:16:06  dmitry
// set units added
//
// Revision 1.11  2002/08/15 19:14:43  dmitry
// Units exposed in the volume.
//
// Revision 1.10  2002/08/14 19:28:17  dmitry
// Histogram creation disabling added.
//
// Revision 1.9  2002/08/14 16:09:28  wenli
// initial version - Vascular
//
// Revision 1.8  2002/08/09 16:20:08  dmitry
// Added Connect and Disconnect methods to IElement.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_Volume.cpp,v 1.12 2006/10/04 18:29:06 gdavidson Exp $
// $Id: v3D_Volume.cpp,v 1.12 2006/10/04 18:29:06 gdavidson Exp $
