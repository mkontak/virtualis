// $Id: v3D_Segmentation.cpp,v 1.17 2006/10/04 18:29:05 gdavidson Exp $
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
#include "v3D_Segmentation.h"
#include "vxManagedObserver.h"
#include "v3D_Environment.h"
#include "vxInputAdapter.h"
#include "vxUtils.h"
#include "vxComponentUtils.h"
#include "vxMouseCursor.h"
#include "vxUndoActionSegmentation.h"


// defines
#ifdef MessageBox
#undef MessageBox
#endif
// defines
#define FILE_REVISION "$Revision: 1.17 $"


// namespaces
USING_VISUALIZATION_NS
using namespace System;
using namespace System::Xml;


/**
 * Default constructor
 */
Segmentation::Segmentation() :
m_mpLabelVolume( nullptr ),
m_mpIntensityVolume( nullptr ),
m_mpComponents( nullptr )
{
  m_pObserver = new vxManagedObserver ( this );
  m_pSegmentation = new vxShareableObject< InteractiveSegmentation >();
  m_pSegmentation->Connect( m_pObserver );
} // Segmentation()


/**
 * destructor
 */
Segmentation::~Segmentation()
{
  this->!Segmentation();
} // ~Segmentation()


/**
 * Finalizer()
 */
Segmentation::!Segmentation()
{
  delete m_pSegmentation; m_pSegmentation = NULL;
  delete m_pObserver;    m_pObserver = NULL;
} // !Segmentation()


/**
 * Returns the intensity volume
 * @return intensity volume
 */
Volume ^ Segmentation::IntensityVolume::get()
{
  return m_mpIntensityVolume;
} // get_IntensityVolume()


/**
 * Sets the intensity volume
 * @param mpVolume intensity volume
 */
void Segmentation::IntensityVolume::set( Volume ^ mpVolume )
{
  try
  {
    m_mpIntensityVolume = mpVolume;
    m_pSegmentation->SetScanVolume( *_native_ptr_t< vxBlockVolume< uint2 > >( m_mpIntensityVolume->GetVolumePtr() ) );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // set_IntensityVolume()


/**
 * Returns the label volume
 * @return label volume
 */
Volume ^ Segmentation::LabelVolume::get()
{
  return m_mpLabelVolume;
} // get_LabelVolume()


/**
 * Sets the label volume
 * @param mpLabelVolume label volume
 */
void Segmentation::LabelVolume::set( Volume ^ mpLabelVolume )
{
  try
  {
    m_mpLabelVolume = mpLabelVolume;

    m_pSegmentation->SetSegmentVolume( _native_ptr_t< vxBlockVolume< uint2 > >( m_mpLabelVolume->GetVolumePtr() ) );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // set_LabelVolume()


/**
 * Returns the components
 * @return components
 */
Components ^ Segmentation::Components::get()
{
  return m_mpComponents;
} // get_Components()


/**
 * Sets the components
 * @param mpComponents components
 */
void Segmentation::Components::set( Viatronix::Visualization::Components ^ mpComponents )
{
  try
  {
    m_mpComponents = mpComponents;
    m_pSegmentation->SetCompHdlrPtr( _native_ptr_t< ComponentHandler >( Components->GetCompHdlrPtr() ) );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // set_Components()


/**
 * Returns the value of low threshold for region grow
 * @return low threshold value
 */
long Segmentation::RegionThresholdLow::get()
{
  try
  {
    return m_pSegmentation->GetRegionIntensityLow();
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // get_RegionThresholdLow()


/**
 * Sets the value of low threshold for region grow.
 * @param thresholdlow value(long) of low threshold.
 */
void Segmentation::RegionThresholdLow::set( long iThresholdLow )
{
  try
  {
    m_pSegmentation->SetRegionIntensityLow( iThresholdLow );
    m_pSegmentation->Modified( vxModInfo( InteractiveSegmentation, InteractiveSegmentation::EVENT_INTENSITY_RANGE ) );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // set_RegionThresholdLow()


/**
 * Returns the value of high threshold for region grow.
 * @return high threshold value
 */
long Segmentation::RegionThresholdHigh::get()
{
  try
  {
    return m_pSegmentation->GetRegionIntensityHigh();
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // get_RegionThresholdHigh()


/**
 * Sets the value of high threshold for region grow.
 * @param thresholdHigh value(long) of high threshold.
 */
void Segmentation::RegionThresholdHigh::set( long iThresholdHigh )
{
  try
  {
    m_pSegmentation->SetRegionIntensityHigh( iThresholdHigh );
    m_pSegmentation->Modified( vxModInfo( InteractiveSegmentation, InteractiveSegmentation::EVENT_INTENSITY_RANGE ) );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // set_RegionThresholdHigh()


/**
 * Returns the value of method used for region grow.
 * @return region grow method
 */
Viatronix::Visualization::RegionGrowMethods Segmentation::RegionGrowMethod::get()
{
  try
  {
    return static_cast< Viatronix::Visualization::RegionGrowMethods >( m_pSegmentation->GetRegionGrowMethod() );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // get_RegionGrowMethod()


/**
 * Sets the value of method used for region grow.
 * @param eMethod value of region grow method.
 */
void Segmentation::RegionGrowMethod::set(Viatronix::Visualization::RegionGrowMethods eMethod )
{
  try
  {
    m_pSegmentation->SetRegionGrowMethod(static_cast< SegParams::RegionGrowMethodEnum >( eMethod ) );
    m_pSegmentation->Modified( vxModInfo( InteractiveSegmentation, InteractiveSegmentation::EVENT_SEGPARAMS_UPDATE ) );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // set_RegionGrowMethod()


/**
 * Gets the value of add option for region grow.
 * @return value(long) of add option.
 */
Viatronix::Visualization::RegionGrowAddOptions Segmentation::RegionGrowAddOpt::get()
{
  try
  {
   return static_cast< RegionGrowAddOptions > ( m_pSegmentation->GetRegionGrowAddOpt() );
  }  
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // get_RegionGrowAddOpt


/**
 * Sets the value of add option for region grow.
 * @param eOptions value(long) of add option.
 */
void Segmentation::RegionGrowAddOpt::set( Viatronix::Visualization::RegionGrowAddOptions eOptions )
{
  try
  {
    m_pSegmentation->SetRegionGrowAddOpt(static_cast< SegParams::RegionAddOptionEnum >( eOptions ) );
    m_pSegmentation->Modified( vxModInfo( InteractiveSegmentation, InteractiveSegmentation::EVENT_SEGPARAMS_UPDATE ) );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // set_RegionGrowAddOpt()


/**
 * Run method for Interactive Segmentation
 *
 * @param mode              run mode
 * @param option            option for run
 * @param bRestrictedSubset only operate on the visible subset of the volume
 * @param mpEnvironment     the viewer's environment
 */
void Segmentation::Run( Viatronix::Visualization::SegmentationModes eMode, long iOption, bool bRestrictedSubset,
                        Viatronix::Visualization::Environment ^ mpEnvironment )
{
  try
  {
    vxMouseCursor cursor( "CURSOR_WAIT" );

    if ( m_mpLabelVolume != nullptr && m_mpComponents != nullptr )
    {
      InteractiveSegmentation::ModeIDsEnum eSegMode = static_cast<InteractiveSegmentation::ModeIDsEnum>( eMode );

	    bool bAppendingToNone( m_pSegmentation->GetCompHdlrPtr()->GetSelectedComponentIndex() == 0 &&
                             m_pSegmentation->GetRegionGrowAddOpt() == SegParams::APPEND_REGIONS );
      if ( bAppendingToNone )
      {
        std::string sComponentName( m_pSegmentation->GetCompHdlrPtr()->GetComponentsPtr()->ArrayByLabel()[ 0 ]->GetName() );
        std::string sMessage( "Appending to " + sComponentName + " is not possible. A new component will be created instead" );
        Viatronix::UI::MessageBox::ShowInfo( gcnew System::String( sMessage.c_str() ));
      }

      // get the valid subset of the volume
      vxEnvironment * pEnvironment = ToNativeEnvironmentPtr( mpEnvironment->GetEnvironmentPtr() );
      Box<uint4> validSubsetUint4( vxUtils::GetActiveBoundingBox( bRestrictedSubset, pEnvironment->GetIntensityVolume(),
        pEnvironment->GetViewerType(), pEnvironment->GetPlaneOrtho(), pEnvironment->GetRenderingMode(),
        pEnvironment->GetOffset(), pEnvironment->GetMaximumOffset(), pEnvironment->GetCropBox() ) );
      Box<int4> validSubsetInt4( Point3Di( validSubsetUint4.GetMinPoint() ), Point3Di( validSubsetUint4.GetMaxPoint() ) );
      static_cast< InteractiveSegmentation * >( m_pSegmentation )->SetValidSubset( validSubsetUint4 );

      if ( iOption == 2 )
      {
        m_pSegmentation->GrowComponent( m_pSegmentation->GetCompHdlrPtr()->GetSelectedComponentIndex() );
      }
      else
      {
        m_pSegmentation->Run( eSegMode, iOption );
      }
      
      if ( m_pSegmentation->IsSuccessful() || iOption == 2 ) // Segmentation OK...
      {
        _native_ptr_t< vxBlockVolume< uint2 > >( m_mpLabelVolume->GetVolumePtr() )->Modified(vxModInfo(vxBlockVolume<uint2>, vxUtils::VOLUME_LABEL_MODIFIED));
  
        vxShareableObject< vxComponentArray > * pComponents( ToNativeComponentsPtr( m_mpComponents->GetElementPtr() ) );
        switch( eSegMode )
        {
          case InteractiveSegmentation::REGION_GROW_MODE:
          case InteractiveSegmentation::REGION_THRESHOLD_SEGMENT_MODE:
          {
            if ( m_pSegmentation->GetRegionGrowAddOpt() == SegParams::NEW_REGIONS || bAppendingToNone == true )
            {
              if ( eSegMode == InteractiveSegmentation::REGION_THRESHOLD_SEGMENT_MODE)
              {
                uint4 uCompIndex(m_pSegmentation->GetCompHdlrPtr()->GetComponentCount()-1);
                vxComponent * pComponent = ( * pComponents )[ uCompIndex ];
                pComponent->SetName(vxComponentUtils::GetUniqueComponentName( * pComponents, m_pSegmentation->GetNextComponentName() ) );
                pComponent->SetColor( m_pSegmentation->GetNextComponentColor() );
              }
              pComponents->Modified( vxModIndInfo( vxComponentArray, 0, vxUtils::COMPONENTARRAY_ADDED ) );
            }
            else // appending to existing region...
            {
              pComponents->Modified( vxModIndInfo( vxComponentArray, 0, vxUtils::COMPONENTARRAY_MODIFIED ) );
            }
          }
          break;
          case InteractiveSegmentation::MORPH_OP_MODE:
          {
            pComponents->Modified( vxModIndInfo( vxComponentArray, 0, vxUtils::COMPONENTARRAY_MODIFIED ) );
          }
          break;
          default:
          {
            pComponents->Modified( vxModIndInfo( vxComponentArray, 0, vxUtils::COMPONENTARRAY_MODIFIED ) );
          }
          break;
        }
      }
      else // segmentation failed
      {
        std::string sMsg("");
        if ( m_pSegmentation->GetStatus() == InteractiveSegmentation::FAILED_TO_SEGMENT )
        {
          sMsg.append("Failed to segment region.\nThe region may already be segmented or not within the selected intensity range.\nPlease review your settings and try again.");
        }
        else if ( m_pSegmentation->GetStatus() == InteractiveSegmentation::NO_SELECTED_REGION )
        {
          if (m_pSegmentation->GetCompHdlrPtr()->GetSelectedComponentIndex() == 0)
          {
            sMsg.append("Grow or shrink not available for the background component");
          }
          else
          {
            sMsg.append("Not able to perform operation. A component must be selected.");
          }
        }
        Viatronix::UI::MessageBox::ShowInfo( gcnew System::String( sMsg.c_str() ));
      }    
    }

    vxInputAdapter::CheckMemory();

  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // Run()


/**
 * Run method for Interactive Segmentation
 *
 * @param mode              run mode
 * @param option            option for run
 * @param bRestrictedSubset only operate on the visible subset of the volume
 * @param mpEnvironment     the viewer's environment
 */
void Segmentation::ThresholdSegment( Viatronix::Visualization::Environment ^ mpEnvironment, bool bRestrictedSubset )
{
  try
  {
    vxMouseCursor cursor( "CURSOR_WAIT" );

    if ( m_mpLabelVolume == nullptr || m_mpComponents == nullptr )
    {
      throw gcnew ApplicationException( "Label volume or components is NULL" );
    }

    bool bAppendingToNone(m_pSegmentation->GetCompHdlrPtr()->GetSelectedComponentIndex() == 0 && m_pSegmentation->GetRegionGrowAddOpt() == SegParams::APPEND_REGIONS);
    if (bAppendingToNone == true)
    {
        std::string sComponentName( m_pSegmentation->GetCompHdlrPtr()->GetComponentsPtr()->ArrayByLabel()[ 0 ]->GetName() );
        std::string sMessage( "Appending to " + sComponentName + " is not possible. A new component will be created instead" );
        Viatronix::UI::MessageBox::ShowInfo( gcnew System::String( sMessage.c_str() ));
    }

    // get the valid subset of the volume
    vxEnvironment * pEnvironment = ToNativeEnvironmentPtr( mpEnvironment->GetEnvironmentPtr() );
    Box<uint4> validSubsetUint4( vxUtils::GetActiveBoundingBox( bRestrictedSubset, pEnvironment->GetIntensityVolume(),
            pEnvironment->GetViewerType(), pEnvironment->GetPlaneOrtho(), pEnvironment->GetRenderingMode(),
            pEnvironment->GetOffset(), pEnvironment->GetMaximumOffset(), pEnvironment->GetCropBox() ) );
    Box<int4> validSubsetInt4( Point3Di( validSubsetUint4.GetMinPoint() ), Point3Di( validSubsetUint4.GetMaxPoint() ) );
    static_cast< InteractiveSegmentation * >( m_pSegmentation )->SetValidSubset( validSubsetUint4 );

    bool bResult = m_pSegmentation->ThresholdSegment();
    
    if ( bResult ) // Segmentation OK...
    {

      vxShareableObject< vxComponentArray > * pComponents( ToNativeComponentsPtr( m_mpComponents->GetElementPtr() ) );
      if ( m_pSegmentation->GetRegionGrowAddOpt() == SegParams::NEW_REGIONS || bAppendingToNone == true )
      {

      }

      // appending to existing region...
      else
      {
        pComponents->Modified( vxModIndInfo( vxComponentArray, 0, vxUtils::COMPONENTARRAY_MODIFIED ) );
      }
    }

    // segmentation failed
    else
    {
      std::string sMsg = "Unable to find any voxels that meet the threshold criteria and are unlocked.\nPlease review your settings and try again.";
      Viatronix::UI::MessageBox::ShowInfo( gcnew System::String( sMsg.c_str() ));
    }    

    vxInputAdapter::CheckMemory();

  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // ThresholdSegment()


/**
 * Grow a component
 *
 * @param mpEnvironment     the viewer's environment
 * @param bRestrictedSubset only operate on the visible subset of the volume
 *
 * @return true if anything has grown (false if there is no region to expand into)
 */
bool Segmentation::GrowComponent( Viatronix::Visualization::Environment ^ mpEnvironment, bool bRestrictedSubset )
{

  try
  {
    if ( m_mpLabelVolume == nullptr && m_mpComponents == nullptr )
    {
      LogWrn( "Necessary data is missing!", "Segmentation", "GrowComponent" );
      return false;
    }

    if ( m_pSegmentation->GetCompHdlrPtr()->GetSelectedComponentIndex() == 0 )
    {
      Viatronix::UI::MessageBox::ShowInfo( "Grow not available for the background component" );
      return false;
    }

    vxMouseCursor cursor( "CURSOR_WAIT" );

    // get the valid subset of the volume
    vxEnvironment * pEnvironment = ToNativeEnvironmentPtr( mpEnvironment->GetEnvironmentPtr() );
    Box<uint4> validSubsetUint4( vxUtils::GetActiveBoundingBox( bRestrictedSubset, pEnvironment->GetIntensityVolume(),
            pEnvironment->GetViewerType(), pEnvironment->GetPlaneOrtho(), pEnvironment->GetRenderingMode(),
            pEnvironment->GetOffset(), pEnvironment->GetMaximumOffset(), pEnvironment->GetCropBox() ) );
    static_cast< InteractiveSegmentation * >( m_pSegmentation )->SetValidSubset( validSubsetUint4 );

    const bool bAnyGrown = m_pSegmentation->GrowComponent( m_pSegmentation->GetCompHdlrPtr()->GetSelectedComponentIndex() );
    
    vxInputAdapter::CheckMemory();
    return bAnyGrown;

  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
  catch ( ... )
  {
    const std::string sMsg( "Encountered a problem during grow operation." );
    LogErr( sMsg.c_str(), "Segmentation", "GrowComponent" );
    throw gcnew v3DException( gcnew System::String( sMsg.c_str() ));
  }

  return false;

} // GrowComponent()


/**
 * Shrink a component
 *
 * @param mpEnvironment     the viewer's environment
 * @param bRestrictedSubset only operate on the visible subset of the volume
 *
 * @return if the component is still available (it will be automatically removed if shrunk to nothing)
 */
bool Segmentation::ShrinkComponent( Viatronix::Visualization::Environment ^ mpEnvironment, bool bRestrictedSubset )
{

  bool bComponentStillAvailable = true;
  try
  {
    if ( m_mpLabelVolume == nullptr && m_mpComponents == nullptr )
    {
      LogWrn( "Necessary data is missing!", "Segmentation", "GrowComponent" );
      return false;
    }

    if ( m_pSegmentation->GetCompHdlrPtr()->GetSelectedComponentIndex() == 0 )
    {
      Viatronix::UI::MessageBox::ShowInfo( "Shrink not available for the background component" );
      return false;
    }

    vxMouseCursor cursor( "CURSOR_WAIT" );

    // get the valid subset of the volume
    vxEnvironment * pEnvironment = ToNativeEnvironmentPtr( mpEnvironment->GetEnvironmentPtr() );
    Box<uint4> validSubsetUint4( vxUtils::GetActiveBoundingBox( bRestrictedSubset, pEnvironment->GetIntensityVolume(),
            pEnvironment->GetViewerType(), pEnvironment->GetPlaneOrtho(), pEnvironment->GetRenderingMode(),
            pEnvironment->GetOffset(), pEnvironment->GetMaximumOffset(), pEnvironment->GetCropBox() ) );
    Box<int4> validSubsetInt4( Point3Di( validSubsetUint4.GetMinPoint() ), Point3Di( validSubsetUint4.GetMaxPoint() ) );
    static_cast< InteractiveSegmentation * >( m_pSegmentation )->SetValidSubset( validSubsetUint4 );

    bComponentStillAvailable = m_pSegmentation->ShrinkComponent( m_pSegmentation->GetCompHdlrPtr()->GetSelectedComponentIndex() );
    
    vxInputAdapter::CheckMemory();

  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
  catch ( ... )
  {
    const std::string sMsg( "Encountered a problem during shrink operation." );
    LogErr( sMsg.c_str(), "Segmentation", "ShrinkComponent" );
    throw gcnew v3DException( gcnew System::String( sMsg.c_str() ));
  }

  return bComponentStillAvailable;

} // ShrinkComponent()


/**
 * sets the component color for the next segmented object
 *
 * @param color the next component color
 */
void Segmentation::NextComponentColor::set( System::Drawing::Color color )
{
  try
  {
    m_pSegmentation->SetNextComponentColor( PixelRGBA<uint1>( color.R, color.G, color.B, 255 ) );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( & e );
  }
} // set_NextComponentColor()


/**
 * sets the component name for the next segmented object
 *
 * @param mpName the next component name
 */
void Segmentation::NextComponentName::set( System::String ^ mpName )
{
  try
  {
    m_pSegmentation->SetNextComponentName( ss( mpName ) );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( & e );
  }
} // set_NextComponentName()


/** 
 * Connects an observer from the object.
 * @param observer vxManagedObserver interested in events.
 */
void Segmentation::Connect( System::IntPtr observer )
{
  m_pSegmentation->Connect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Connect()


/** 
 * Disconnects an observer from the object.
 * @param observer vxManagedObserver interested in events.
 */
void Segmentation::Disconnect( System::IntPtr observer )
{
  m_pSegmentation->Disconnect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Disconnect()


/**
 * Processes modified queue events.
 * @param modifiedList Modified info list.
 */
void Segmentation::OnModified( System::IntPtr modifiedList )
{
  vxModifiedInfoList * pInfoList = reinterpret_cast< vxModifiedInfoList * >( modifiedList.ToPointer() );

  int4 uFlags(0);
  for ( const vxModifiedInfo * pInfo = pInfoList->Begin(); pInfo != NULL; pInfo = pInfoList->Next())
  {
    uFlags |= pInfo->GetFlag();
  }

  if (uFlags & InteractiveSegmentation::EVENT_INTENSITY_RANGE )
    ThresholdChanged( this, System::EventArgs::Empty );

  if (uFlags & InteractiveSegmentation::EVENT_SEGPARAMS_UPDATE )
    Changed( this, System::EventArgs::Empty );

} // OnModified()


/**
 * Serializes the segmentation object to xml.
 * @return xml string representing object.
 */
System::String ^ Segmentation::ToXml()
{
  try
  {
    return System::String::Empty;
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // ToXml()


/**
 * Creates a new segmentation object from the specified xml string.
 * @param xml xml string
 * @return segmentation object
 */
Segmentation ^ Segmentation::FromXml( System::String ^ mpXml )
{
  try
  {
    Segmentation ^ segmentation = gcnew Segmentation();

    return segmentation;
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // FromXml()


/**
 * Creates a new segmentation from the specified xml resource.
 * @param xml xml string
 * @return segmentation object
 */
Segmentation ^ Segmentation::FromResource( System::String ^ mpXmlResource )
{
  try
  {
    XmlTextReader xmlReader( System::Reflection::Assembly::GetCallingAssembly()->GetManifestResourceStream( mpXmlResource ) );
    XmlDocument xmlDocument;
    xmlDocument.Load( %xmlReader );

    return Segmentation::FromXml( xmlDocument.InnerXml );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }

} // FromResource()


/**
 * Creates a new segmentation from the specified xml file string.
 * @param xml xml string
 * @return segmentation object
 */
Segmentation ^ Segmentation::FromFile( System::String ^ mpXmlFile )
{
  try
  {
    XmlDocument xmlDocument;
    xmlDocument.Load( mpXmlFile );

    return Segmentation::FromXml( xmlDocument.InnerXml );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // FromFile()


/**
 * Sets a restore point
 */
void Segmentation::SetRestorePoint()
{

  vxEnvironment::GetUndoActions().PushAction( new vxUndoActionSegmentation(
    ToNativeVolumeUint2Ptr( m_mpLabelVolume->GetElementPtr() ),
    ToNativeComponentsPtr( m_mpComponents->GetElementPtr() ) ) );
    
} // SetRestorePoint()


// undefines
#undef FILE_REVISION


// $Log: v3D_Segmentation.cpp,v $
// Revision 1.17  2006/10/04 18:29:05  gdavidson
// Ported to C++/CLI
//
// Revision 1.16  2006/09/27 17:23:49  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.15  2006/09/27 17:06:28  geconomos
// updated for class name changes to InutAdapter and ViewerImpl
//
// Revision 1.14  2006/07/06 15:05:17  frank
// Issue #4840: Refactored component creation and naming method
//
// Revision 1.13  2006/04/20 20:01:41  romy
// Removed the handling of Grow/Shrink results here.Moved to the pane component.
//
// Revision 1.12  2006/04/20 18:37:58  romy
// Removed the handling of Grow/Shrink results here.Moved to the pane component.
//
// Revision 1.11  2006/04/20 17:18:49  romy
// modified the false message box on a shrink operation
//
// Revision 1.10  2006/04/18 18:43:58  frank
// Issue #4711: Fixed the threshold all functionality - deprecated the old code
//
// Revision 1.9  2006/03/29 20:11:50  geconomos
// removed vxBlockVolumeBinaryHistogram related code
//
// Revision 1.8  2006/03/02 15:51:28  frank
// cleaned up grow and shrink operations
//
// Revision 1.7  2006/02/27 15:23:42  frank
// split out component utilities
// added undo for component operations
//
// Revision 1.6  2006/02/17 15:19:07  frank
// rendering mode is no longer necessary for the segmentation object
//
// Revision 1.5  2006/01/20 18:09:00  geconomos
// continued migration to visual studio 2005
//
// Revision 1.4  2005/11/18 21:12:28  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.3.2.4  2005/08/26 12:30:09  frank
// Issue #4401: Fixed messages referring to the NONE component
//
// Revision 1.3.2.3  2005/08/04 14:49:26  frank
// Issue #4340: Fixed problem with text being obscured in a warning message box.
//
// Revision 1.3.2.2  2005/07/11 18:11:46  frank
// cleaned up naming and color of components
//
// Revision 1.3.2.1  2005/07/05 17:55:37  frank
// added names and colors to the segmentation presets
//
// Revision 1.3  2005/05/13 17:29:17  frank
// updated the segmentation failed message to be more descriptive
//
// Revision 1.2  2004/03/21 23:10:00  geconomos
// Upgrade to Visual Studio .NET 2003
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.32  2004/03/02 01:10:43  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.31.2.1  2004/02/24 20:14:10  michael
// took out code that was commented out
//
// Revision 1.31  2004/02/11 22:44:04  michael
// replaced "NONE" by "Background"
//
// Revision 1.30  2004/02/05 15:11:46  frank
// necessary include file
//
// Revision 1.29  2004/02/05 13:36:19  frank
// checked the free memory after any operation
//
// Revision 1.28  2003/12/22 12:55:39  frank
// Issue #3593: Restricted segmentation to the visible volume on the shift key
//
// Revision 1.27  2003/12/16 13:27:00  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.26  2003/11/20 21:10:02  frank
// Fixed problem with the segmentation range not being displayed
//
// Revision 1.25  2003/08/20 14:31:12  michael
// added "mailto:"
//
// Revision 1.24  2003/07/30 15:41:00  geconomos
// Added RenderingMode property to Segmentation object.
//
// Revision 1.23  2003/06/18 15:19:46  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.22  2003/05/16 13:13:56  frank
// formatting
//
// Revision 1.21  2003/05/13 13:28:46  michael
// code review
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_Segmentation.cpp,v 1.17 2006/10/04 18:29:05 gdavidson Exp $
// $Id: v3D_Segmentation.cpp,v 1.17 2006/10/04 18:29:05 gdavidson Exp $
