// $Id: vxRendererOverlay.C,v 1.11.8.1 2009/03/23 15:42:52 gdavidson Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)


// includes
#include "stdafx.h"
#include "vxRendererOverlay.h"
#include "vxUtils.h"
#include "vxEnvironment.h"
#include "GLFontGeorge.h"
#include "vxIntensityConverter.h"
#include "vxMathUtils.h"
#include "vxViewerTypes.h"
#include "vxOverlayStrings.h"
#include "vxIDs.h"
#include "vxRendererUtils.h"


// namespaces
using namespace std;


// defines
#define FILE_REVISION "$Revision: 1.11.8.1 $"


// static member initialization
const float4 vxRendererOverlay::m_fFontHeightFactor( 0.8F );
const int4 vxRendererOverlay::m_iLogoHeight( 15 );


/**
 * Constructor
 *
 * @param environment the environment
 */
vxRendererOverlay::vxRendererOverlay( vxEnvironment & environment )
: vxRenderer( environment )
{
  m_fInterlineSpacing = m_fFontHeightFactor * environment.GetFont()->GetPixelHeight();
  m_fSideSpace = 6.0F;
  m_textColorNormal = Triple<float4>( 1.0F, 1.0F, 1.0F );
  m_textColorHighlighted = Triple<float4>( 1.0F, 1.0F, 0.0F );
} // vxRendererOverlay()


/**
 * Has something changed that makes rendering necessary?
 * @return whether rendering is necessary
 */
bool vxRendererOverlay::IsRedrawNecessary()
//******************************************************************* 
{

  bool bChangedFont( GetEnvironment().IsModified( vxEnvironment::FONT ) );
  bool bChangedOverlay( GetEnvironment().IsModified( vxIDs::OverlayStrings ) );
  bool bChangedWinRect( GetEnvironment().IsModified( vxEnvironment::WIN_RECT ) );

  bool bRedrawNecessary( false );
  bRedrawNecessary |= bChangedFont;
  bRedrawNecessary |= bChangedOverlay;
  bRedrawNecessary |= bChangedWinRect;

  return bRedrawNecessary;

} // IsRedrawNecessary()


/**
 * Setup the OpenGL environment for overlay drawing
 */
void vxRendererOverlay::SetupRenderState()
{

  // save the OpenGL state
  glPushAttrib( GL_ALL_ATTRIB_BITS );
  glMatrixMode( GL_PROJECTION );
  glPushMatrix();
  glMatrixMode( GL_MODELVIEW );
  glPushMatrix();
  
  // new settings
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  Rect<float4> viewPortRect( 0.0F, 0.0F, GetEnvironment().GetWinRect().GetViewportSize().first, GetEnvironment().GetWinRect().GetViewportSize().second );
  glOrtho( 0.0F, viewPortRect.Width(), 0.0F, viewPortRect.Height(), -50, 50 );
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
  glDisable( GL_DEPTH_TEST );
  glColor3f( 1.0F, 1.0F, 1.0F );

} // SetupRenderState()


/**
 * Restore the OpenGL environment after overlay drawing
 */
void vxRendererOverlay::RestoreRenderState()
{
  // restore the OpenGL state
  glPopAttrib();
  glMatrixMode( GL_PROJECTION );
  glPopMatrix();
  glMatrixMode( GL_MODELVIEW );
  glPopMatrix();  

} // RestoreRenderState()


/**
 * Render all the overlay strings
 */
void vxRendererOverlay::Render()
{
  // exit if there is no volume or it is empty
  if ( GetEnvironment().GetIntensityVolume() == NULL ) return;
  if ( GetEnvironment().GetIntensityVolume()->GetHeader().GetVolDim().GetMaxComponent() == 0 ) return;

  // exit if the overlay strings are non-existent
  vxOverlayStrings * pOverlayStrings = dynamic_cast< vxOverlayStrings * >( GetEnvironment().GetElement( vxIDs::OverlayStrings ) );
  if ( pOverlayStrings == NULL ) return;

  SetupRenderState();

  // recompute in case the font size has changed
  m_fInterlineSpacing = m_fFontHeightFactor * GetEnvironment().GetFont()->GetPixelHeight();

  Rect<float4> viewPortRect(0, 0, GetEnvironment().GetWinRect().GetViewportSize().first, GetEnvironment().GetWinRect().GetViewportSize().second);

  // display all the strings in the upper right corner
  uint4 uCount(1);
  vxOverlayStrings::OverlayElementList upperRight = pOverlayStrings->GetFilteredList( vxOverlayStrings::vxOverlayElement::UpperRight );
  vxOverlayStrings::OverlayElementListIterator it( upperRight.begin() );
  for( ; it != upperRight.end(); it++, uCount++ )
  {
    const int4 xPosition( viewPortRect.m_right - m_fSideSpace );
    const int4 yPosition( viewPortRect.m_bottom - uCount * m_fInterlineSpacing );
    const Point2D< int4 > position( xPosition, yPosition );
    GetEnvironment().GetFont()->PutFormattedString( it->GetText().c_str(), GLFontGeorge::ALIGN_RIGHT, position, it->GetColor() );
  }

  // display all the strings in the upper left corner
  uCount = 1;
  vxOverlayStrings::OverlayElementList upperLeft = pOverlayStrings->GetFilteredList( vxOverlayStrings::vxOverlayElement::UpperLeft );
  for( it = upperLeft.begin(); it != upperLeft.end(); ++it, uCount++ )
  {
    const Point2D<int4> position( m_fSideSpace, viewPortRect.Height() - uCount * m_fInterlineSpacing );
    GetEnvironment().GetFont()->PutFormattedString( it->GetText().c_str(), GLFontGeorge::ALIGN_LEFT, position, it->GetColor() );
  }

  // display all the strings in the lower right corner
  uCount = 1;
  vxOverlayStrings::OverlayElementList lowerRight = pOverlayStrings->GetFilteredList( vxOverlayStrings::vxOverlayElement::LowerRight );
  for( it = lowerRight.begin(); it != lowerRight.end(); ++it, uCount++ )
  {
    const int4 xPosition( viewPortRect.m_right - m_fSideSpace );
    const int4 yPosition( viewPortRect.m_top + uCount * m_fInterlineSpacing );
    const Point2D< int4 > position( xPosition, yPosition );
    GetEnvironment().GetFont()->PutFormattedString( it->GetText().c_str(), GLFontGeorge::ALIGN_RIGHT, position, it->GetColor() );
  }

  // display all the strings in the lower left corner
  uCount = 0;
  vxOverlayStrings::OverlayElementList lowerLeft = pOverlayStrings->GetFilteredList( vxOverlayStrings::vxOverlayElement::LowerLeft );
  for( it = lowerLeft.begin(); it != lowerLeft.end(); ++it, uCount++ )
  {
    const float4 fToolBarOffset( 43.0F );
    const int4 xPosition( m_fSideSpace );
    const int4 yPosition( viewPortRect.m_top + uCount * m_fInterlineSpacing + fToolBarOffset );
    const Point2D< int4 > position( xPosition, yPosition );
    GetEnvironment().GetFont()->PutFormattedString( it->GetText().c_str(), GLFontGeorge::ALIGN_LEFT, position, it->GetColor() );
  }

  // display all the strings on the middle left side
  uCount = 0;
  vxOverlayStrings::OverlayElementList middleLeft = pOverlayStrings->GetFilteredList( vxOverlayStrings::vxOverlayElement::MiddleLeft );
  for( it = middleLeft.begin(); it != middleLeft.end(); ++it, uCount++ )
  {
    const int4 iTextHeightY( middleLeft.size() * m_fInterlineSpacing );
    const int4 iTextTopY( viewPortRect.Height()/2 - iTextHeightY / 2 );
    const int4 yPosition( iTextTopY + uCount * m_fInterlineSpacing );
    const Point2D< int4 > position( m_fSideSpace, yPosition );
    GetEnvironment().GetFont()->PutFormattedString( it->GetText().c_str(), GLFontGeorge::ALIGN_LEFT, position, it->GetColor() );
  }

  // display all the strings on the middle right side
  uCount = 0;
  vxOverlayStrings::OverlayElementList middleRight = pOverlayStrings->GetFilteredList( vxOverlayStrings::vxOverlayElement::MiddleRight );
  for( it = middleRight.begin(); it != middleRight.end(); ++it, uCount++ )
  {
    const int4 xPosition( viewPortRect.Width() - m_fSideSpace );
    const int4 iMiddleY( ( viewPortRect.m_top + viewPortRect.m_bottom ) / 2 );
    const int4 iTextHeightY( middleLeft.size() * m_fInterlineSpacing );
    const int4 iTextTopY( iMiddleY - iTextHeightY / 2 );
    const int4 yPosition( iTextTopY + uCount * m_fInterlineSpacing );
    const Point2D< int4 > position( xPosition, yPosition );
    GetEnvironment().GetFont()->PutFormattedString( it->GetText().c_str(), GLFontGeorge::ALIGN_RIGHT, position, it->GetColor() );
  }

  // display all the strings at the upper middle
  uCount = 1;
  vxOverlayStrings::OverlayElementList upperMiddle = pOverlayStrings->GetFilteredList( vxOverlayStrings::vxOverlayElement::UpperMiddle );
  for( it = upperMiddle.begin(); it != upperMiddle.end(); ++it, uCount++ )
  {
    const Point2D< int4 > position( viewPortRect.Width() / 2, viewPortRect.m_bottom - uCount * m_fInterlineSpacing );
    GetEnvironment().GetFont()->PutFormattedString( it->GetText().c_str(), GLFontGeorge::ALIGN_CENTER, position, it->GetColor() );
  }

  // display all the strings at the lower middle (adjusted for the viatronix logo)
  uCount = 0;
  vxOverlayStrings::OverlayElementList lowerMiddle = pOverlayStrings->GetFilteredList( vxOverlayStrings::vxOverlayElement::LowerMiddle );
  for( it = lowerMiddle.begin(); it != lowerMiddle.end(); ++it, uCount++ )
  {
    const Point2D< int4 > position( viewPortRect.Width() / 2, viewPortRect.m_top + m_iLogoHeight + uCount * m_fInterlineSpacing + m_fSideSpace );
    GetEnvironment().GetFont()->PutFormattedString( it->GetText().c_str(), GLFontGeorge::ALIGN_CENTER, position, it->GetColor() );
  }

  // display all the strings in the center
  uCount = 1;
  vxOverlayStrings::OverlayElementList center = pOverlayStrings->GetFilteredList( vxOverlayStrings::vxOverlayElement::MiddleMiddle );
  for( it = center.begin(); it != center.end(); ++it, uCount++ )
  {
    const int4 iTextHeight( center.size() * m_fInterlineSpacing );
    const int4 xPosition( ( viewPortRect.m_left + viewPortRect.m_right) / 2 );
    const int4 yPosition( ( viewPortRect.m_top + viewPortRect.m_bottom ) / 2 + center.size() * m_fInterlineSpacing / 2 - uCount * m_fInterlineSpacing + iTextHeight / 2 );
    const Point2D< int4 > position( xPosition, yPosition );
    GetEnvironment().GetFont()->PutFormattedString( it->GetText().c_str(), GLFontGeorge::ALIGN_CENTER, position, it->GetColor() );
  }

  // display all the strings that are randomly placed
  vxOverlayStrings::OverlayElementList random = pOverlayStrings->GetFilteredList( vxOverlayStrings::vxOverlayElement::Random );
  for( it = random.begin(); it != random.end(); ++it )
  {
    const Point2D< int4 > position( it->GetRandomScreenPosition().m_x, it->GetRandomScreenPosition().m_y );
    switch ( it->GetAlignment() )
    {
    case vxOverlayStrings::vxOverlayElement::AlignLeft:
      GetEnvironment().GetFont()->PutFormattedString( it->GetText().c_str(), GLFontGeorge::ALIGN_LEFT, position, it->GetColor() );
    	break;
    case vxOverlayStrings::vxOverlayElement::AlignCenter:
      GetEnvironment().GetFont()->PutFormattedString( it->GetText().c_str(), GLFontGeorge::ALIGN_CENTER, position, it->GetColor() );
    	break;
    case vxOverlayStrings::vxOverlayElement::AlignRight:
      GetEnvironment().GetFont()->PutFormattedString( it->GetText().c_str(), GLFontGeorge::ALIGN_RIGHT, position, it->GetColor() );
      break;
    default:
      throw ex::IllegalArgumentException( LogRec( "unknown alignment: " + ToAscii( it->GetAlignment() ), "vxRendererOverlay", "Render" ) );
    	break;
    }
  }

  // draw the logo
  vxRendererUtils::DrawLogo( GetEnvironment().GetWinRect() );

  RestoreRenderState();

  pOverlayStrings->Clear();

} // Render()


/**
 * Render the patient information in the upper left
 */
void vxRendererOverlay::RenderPatientInformation()
{
  // see if we need to render patient information
  vxOverlayStrings * pOverlayStrings = dynamic_cast< vxOverlayStrings * >( GetEnvironment().GetElement( vxIDs::OverlayStrings ) );

  pOverlayStrings->AddElement( vxOverlayStrings::vxOverlayElement( GetPatientNameString(), vxOverlayStrings::vxOverlayElement::UpperLeft ) );
  pOverlayStrings->AddElement( vxOverlayStrings::vxOverlayElement( GetPatientIdString  (), vxOverlayStrings::vxOverlayElement::UpperLeft ) );
  pOverlayStrings->AddElement( vxOverlayStrings::vxOverlayElement( GetStudyDateString  (), vxOverlayStrings::vxOverlayElement::UpperLeft ) );
  pOverlayStrings->AddElement( vxOverlayStrings::vxOverlayElement( GetPatientAgeString (), vxOverlayStrings::vxOverlayElement::UpperLeft ) );
  pOverlayStrings->AddElement( vxOverlayStrings::vxOverlayElement( GetPatientSexString (), vxOverlayStrings::vxOverlayElement::UpperLeft ) );
} // RenderPatientInformation()


/**
 * Render modality-specific information in the lower left
 */
void vxRendererOverlay::RenderModalityInformation()
{
  // see if we need to render overlay information
  std::string sDataModality;
  GetEnvironment().GetIntensityVolume()->GetHeader().GetHeaderEntry(vxVolumeHeader::DataModality, sDataModality);
  if ( sDataModality == "CT" )
  {
    vxOverlayStrings * pOverlayStrings = dynamic_cast< vxOverlayStrings * >( GetEnvironment().GetElement( vxIDs::OverlayStrings ) );
    pOverlayStrings->AddElement( vxOverlayStrings::vxOverlayElement( GetCurrentAsString()  , vxOverlayStrings::vxOverlayElement::LowerLeft ) );
    pOverlayStrings->AddElement( vxOverlayStrings::vxOverlayElement( GetPeakVoltageString(), vxOverlayStrings::vxOverlayElement::LowerLeft ) );
  }
} // RenderModalityInformation()


/**
 * Return the window/level as a string
 *
 * @return the window/level as a string
 */
std::string vxRendererOverlay::GetWindowLevelString()
{
  char sTmpStr[201];
  vxBlockVolume<uint2> * pVolume = static_cast<vxBlockVolume<uint2> *>(GetEnvironment().GetIntensityVolume());
  vxVolumeHeader & header(pVolume->GetHeader());

  const int4 iWindow(vxIntensityConverter::ToShort(GetEnvironment().GetColormap()->GetWindowLevel().GetWindow(),
                     header, false));
  const int4 iLevel(vxIntensityConverter::ToShort(GetEnvironment().GetColormap()->GetWindowLevel().GetLevel(),
                    header, true));
  sprintf (sTmpStr, "W/L: %d/%d", iWindow, iLevel);

  return sTmpStr;
} // GetWindowLevelString()

/**
 * Return the volume coordinate as a string
 * @return the volume coordinate as a string
 */
std::string vxRendererOverlay::GetVolCoordinateAsString()
{
	Vector3Df posVolume( * GetEnvironment().GetCurrentVoxelPos() );
 	std::string strVolCord( "V" );
	std::string strTemp = posVolume.ToString();
	if ( strTemp.length() > 0 )
		strVolCord.append ( strTemp.substr( 8, strTemp.length() - 8 ));

	return strVolCord;
} //GetVolCoordinateAsString()

/**
 * Return the world cordinate as a string
 * @return the world cordinate as a string
 */
std::string vxRendererOverlay::GetWorldCoordinateAsString()
{
	Vector3Df posVolume( * GetEnvironment().GetCurrentVoxelPos() );
  Point3Df  posWorld( posVolume.ScaleAnIso( GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits() ) );

	std::string strWorldCord( "W" );
	std::string strTemp = posWorld.ToString();
	if ( strTemp.length() > 0 )
		strWorldCord.append ( strTemp.substr( 7, strTemp.length() - 7 ));

	return strWorldCord;
} //GetWorldCoordinateAsString()



/**
 * Return the value of the current voxel as a string
 *
 * @return the value of the current voxel as a string.
 */
std::string vxRendererOverlay::GetCurrentVoxelString()
{
  ///////////////////////////////////////////////////////////////////////////
  //
  // we need to check if the position of current voxel is inside the volume
  //
  ///////////////////////////////////////////////////////////////////////////
  vxBlockVolumeIterator<uint2> iter( * static_cast< vxBlockVolume<uint2> * >( GetEnvironment().GetIntensityVolume() ) );
  iter.SetPos( * static_cast< Vector3D<int> * >( GetEnvironment().GetCurrentVoxelPos() ) );
  
  int1 sTmpStr[201];
  vxVolumeHeader & header(GetEnvironment().GetIntensityVolume()->GetHeader());  
  string sUnits(vxIntensityConverter::GetUnit(header));
  
  if ( iter.IsInsideVolume() )
  {
    int4 iModalityVal( vxIntensityConverter::RawToModality( iter.GetVoxel(), header ) );

		if( iter.GetVoxel() >= USHRT_MAX )
		{
      sprintf(sTmpStr, "%s: ?? ", sUnits.c_str(), iModalityVal);
		}
		else
		{
      sprintf(sTmpStr, "%s: %5d", sUnits.c_str(), iModalityVal);
		}

  } // GetCurrentVoxelString()
  else
  {
    sprintf( sTmpStr, "%s: --- ", sUnits.c_str() );
  }
      
  return sTmpStr;
}

/**
 * Return the HU value of the current voxel as a string
 * @return the value of the current voxel as a string.
 */
std::string vxRendererOverlay::GetVoxelString( Vector3D<int> pos )
{
  // we need to check if the position of current voxel is inside the volume
  vxBlockVolumeIterator<uint2> iter( * static_cast< vxBlockVolume<uint2> * >( GetEnvironment().GetIntensityVolume() ) );
  iter.SetPos( pos );//* static_cast< Vector3D<int> * >( GetEnvironment().GetCurrentVoxelPos() ) );
  
  int1 sTmpStr[201];
  vxVolumeHeader & header(GetEnvironment().GetIntensityVolume()->GetHeader());  
  string sUnits(vxIntensityConverter::GetUnit(header));
  if ( iter.IsInsideVolume() )
  {
    int4 iModalityVal( vxIntensityConverter::RawToModality( iter.GetVoxel(), header ) );

		if( iter.GetVoxel() >= USHRT_MAX )
		{
      sprintf(sTmpStr, " ?? ", iModalityVal);
		}
		else
		{
      sprintf(sTmpStr, "%5d  ", iModalityVal);
		}
  } // GetCurrentVoxelString()
  else
  {
    sprintf( sTmpStr, "%s: -- ", sUnits.c_str() );
  }
  return sTmpStr;
}


/**
 * Renders the orientation letters for a 3D view
 */
void vxRendererOverlay::Render3DOrientationLettering()
{
  // get the overlay strings
  vxOverlayStrings * pOverlayStrings = dynamic_cast< vxOverlayStrings * >( GetEnvironment().GetElement( vxIDs::OverlayStrings ) );

  // choose the maximum component of both the right and up directions
  std::string sLeft;
  std::string sRight;
  std::string sUp;
  std::string sDown;
  const Normal3Df vRight( GetEnvironment().GetCamera()->GetViewRight() );
  const Normal3Df vUp   ( GetEnvironment().GetCamera()->GetViewUp   () );
  const int4 iRightCoord( vRight.GetMaxAbsComponentCoord() );
  const int4 iUpCoord   ( vUp   .GetMaxAbsComponentCoord() );
  if ( fabs( vRight[ iRightCoord ] ) > fabs( vUp[ iUpCoord ] ) )
  {
    // the right direction is dominant
    bool bMajor( vRight[ iRightCoord ] >= 0 );
    switch ( iRightCoord )
  {
    case 0:
      sLeft  = bMajor ? "R" : "L";
      sRight = bMajor ? "L" : "R";
    	break;
    case 1:
      sLeft  = bMajor ? "A" : "P";
      sRight = bMajor ? "P" : "A";
      break;
    case 2:
      sLeft  = bMajor ? "I" : "S";
      sRight = bMajor ? "S" : "I";
      break;
    default:
    	break;
    }
  
    // In the up vector, zero out the component that is dominant in the right direction.
    // Then we can find the secondary dominant component
    Vector3Df vUpModified( vUp );
    vUpModified[ iRightCoord ] = 0;
    const int4 iUpModifiedCoord( vUpModified.GetMaxAbsComponentCoord() );
    bMajor = vUpModified[ iUpModifiedCoord ] >= 0;
    switch ( iUpModifiedCoord )
    {
  case 0:
      sUp   = bMajor ? "L" : "R";
      sDown = bMajor ? "R" : "L";
    	break;
    case 1:
      sUp   = bMajor ? "P" : "A";
      sDown = bMajor ? "A" : "P";
      break;
    case 2:
      sUp   = bMajor ? "S" : "I";
      sDown = bMajor ? "I" : "S";
      break;
    default:
    	break;
    }
  }
  else
  {
    // the up direction is dominant
    bool bMajor( vUp[ iUpCoord ] >= 0 );
    switch ( iUpCoord )
    {    
    case 0:
      sUp   = bMajor ? "L" : "R";
      sDown = bMajor ? "R" : "L";
      break;
    case 1:
      sUp   = bMajor ? "P" : "A";
      sDown = bMajor ? "A" : "P";
      break;
    case 2:
      sUp   = bMajor ? "S" : "I";
      sDown = bMajor ? "I" : "S";
      break;
    default:
      break;
    }

    // In the right vector, zero out the component that is dominant in the up direction.
    // Then we can find the secondary dominant component
    Vector3Df vRightModified( vRight );
    vRightModified[ iUpCoord ] = 0;
    const int4 iRightModifiedCoord( vRightModified.GetMaxAbsComponentCoord() );
    bMajor = vRightModified[ iRightModifiedCoord ] >= 0;
    switch ( iRightModifiedCoord )
    {
    case 0:
      sLeft  = bMajor ? "R" : "L";
      sRight = bMajor ? "L" : "R";
    break;
  case 1:
      sLeft  = bMajor ? "A" : "P";
      sRight = bMajor ? "P" : "A";
    break;
  case 2:
      sLeft  = bMajor ? "I" : "S";
      sRight = bMajor ? "S" : "I";
		break;
    default:
      break;
    }
  }

  // draw the Orientation Markers on the top, right, bottom, and left center of the window
  pOverlayStrings->AddElement( vxOverlayStrings::vxOverlayElement( sLeft , vxOverlayStrings::vxOverlayElement::MiddleLeft  ) );
  pOverlayStrings->AddElement( vxOverlayStrings::vxOverlayElement( sRight, vxOverlayStrings::vxOverlayElement::MiddleRight ) );
  pOverlayStrings->AddElement( vxOverlayStrings::vxOverlayElement( sUp   , vxOverlayStrings::vxOverlayElement::UpperMiddle ) );
  pOverlayStrings->AddElement( vxOverlayStrings::vxOverlayElement( sDown , vxOverlayStrings::vxOverlayElement::LowerMiddle ) );

} // Render3DOrientationLettering()


/**
 * Return the patient name as a string
 *
 * @return the patient name as a string
 */
std::string vxRendererOverlay::GetPatientNameString()
{
  std::string sTmp;
  GetEnvironment().GetIntensityVolume()->GetHeader().GetHeaderEntry(vxVolumeHeader::PatientName, sTmp);
  
  if (sTmp == "")
    return "Name: --";
  else
    return "Name: " + sTmp;

} // GetPatientNameString()


/**
 * Return the patient id as a string
 *
 * @return the patient id as a string
 */
std::string vxRendererOverlay::GetPatientIdString()
{
  std::string sTmp;
  GetEnvironment().GetIntensityVolume()->GetHeader().GetHeaderEntry(vxVolumeHeader::PatientID, sTmp);

  if (sTmp == "")
    return "ID: --";
  else
    return "ID: " + sTmp;

} // GetPatientIdString()


/**
 * Return the patient birth date as a string
 *
 * @return the patient birth date as a string
 */
std::string vxRendererOverlay::GetPatientBirthDateString()
{
  std::string sTmp;
  GetEnvironment().GetIntensityVolume()->GetHeader().GetHeaderEntry(vxVolumeHeader::PatientBirthdate, sTmp);

  if (sTmp == "")
    return "DOB: --";
  else
    return "DOB: " + sTmp;

} // GetPatientBirthDateString()


/**
 * display the rendering modes for debugging purposes
 */
void vxRendererOverlay::DisplayRenderingModes()
{

  vxOverlayStrings * pOverlayStrings = dynamic_cast< vxOverlayStrings * >(  GetEnvironment().GetElement( vxIDs::OverlayStrings ) );

  // todo fix hard coded values
  const int4 iNumModes( 21 );
  std::string modeNames [ iNumModes ] =
  {
    "SHOW_PATIENT_INFORMATION",
    "SHOW_IMAGE_INFORMATION",
    "SHOW_VESSEL_CENTERLINE",
    "SHOW_COMPONENT_IN_MPR",
    "SHOW_COLORMAP_IN_MPR",
    "SHOW_CURSOR_3D_IN_MPR",
    "SHOW_INTENSITY_RANGE",
    "SHOW_SLICE_SHADOW",
    "SHOW_LABEL_ON_TOP",
    "SHOW_CROP_BOX",
    "SHOW_BOX_OF_INTEREST",
    "SHOW_SHADOW_CLIPPED",
    "AUTO_FLY_MODE",
    "SHOW_JITTERING",
    "SHOW_PRE_INTEGRATION",
    "SHOW_SHADING",
    "SHOW_SHADOWS",
    "SHOW_EVERY_PIXEL",
    "SHOW_CUTS_SHADED",
    "SHOW_CUTS_GRAYSCALE",
  };
  int4 modeValues [ iNumModes ] =
  {
    0x00000001,
    0x00000002,
    0x00000004,
    0x00000010,
    0x00000020,
    0x00000040,
    0x00000100,
    0x00000200,
    0x00000400,
    0x00000800,
    0x00001000,
    0x00002000,
    0x00008000,
    0x00010000,
    0x00020000,
    0x00040000,
    0x00080000,
    0x00100000,
    0x00200000,
    0x00400000,
  };
  for ( int i=0; i<iNumModes; ++i )
  {
    if ( GetEnvironment().GetRenderingMode()->IsEnabled( static_cast< vxRenderingMode::vxOptionBitEnum >( modeValues[i] ) ) )
    {
      pOverlayStrings->AddElement( vxOverlayStrings::vxOverlayElement( modeNames[i], vxOverlayStrings::vxOverlayElement::MiddleLeft ) );
    }
  }

} // DisplayRenderingModes()


/**
 * Return the patient age as a string
 *
 * @return the patient age as a string
 */
std::string vxRendererOverlay::GetPatientAgeString()
{
  std::string sTmp;
  GetEnvironment().GetIntensityVolume()->GetHeader().GetHeaderEntry(vxVolumeHeader::PatientAge, sTmp);
  
  if (sTmp == "")
  {
    return "Age: --";
  }
  else
  {
    if (sTmp[sTmp.size()-1] == 'Y')
      return "Age: " + sTmp + "ears";
    else
      return "Age: " + sTmp;
  }

} // GetPatientAgeString()


/**
 * Return the patient sex as a string
 *
 * @return the patient sex as a string
 */
std::string vxRendererOverlay::GetPatientSexString()
{
  std::string sBuf, sTmp;
  GetEnvironment().GetIntensityVolume()->GetHeader().GetHeaderEntry(vxVolumeHeader::PatientSex, sTmp);

  if (sTmp.size() > 0)
  {
    if (sTmp[0] == 'M' || sTmp[0] == 'm')
    {
      sTmp = "Male";
    }
    else if (sTmp[0] == 'F' || sTmp[0] == 'f')
    {
      sTmp = "Female";
    }
    else
    {
      sTmp = "unspecified"; // do not laugh, some DICOM files come like this!
    }
  }
  else
  {
    sTmp = "--";
  }
  sBuf = "Sex: " + sTmp;

  return sBuf;  
} // GetPatientSexString()


/**
 * Return the peak voltage as a string
 *
 * @return the peak voltage as a string
 */
std::string vxRendererOverlay::GetPeakVoltageString()
{
  std::string sTmp;
  GetEnvironment().GetIntensityVolume()->GetHeader().GetHeaderEntry(vxVolumeHeader::Voltage, sTmp);

  if (sTmp == "")
    return "kVp: --";
  else
    return "kVp: " + sTmp;

} // GetPeakVoltageString()


/**
 * Return the current as a string
 *
 * @return the current as a string
 */
std::string vxRendererOverlay::GetCurrentAsString()
{
  std::string sTmp;
  GetEnvironment().GetIntensityVolume()->GetHeader().GetHeaderEntry(vxVolumeHeader::Current, sTmp);

  if (sTmp == "")
    return "mA: --";
  else
    return "mA: " + sTmp;

} // GetCurrentAsString()


/**
 * Return the slice thickness as a string
 *
 * @return the slice thickness as a string
 */
std::string vxRendererOverlay::GetSliceThicknessString()
{
  std::string sTmp;
  GetEnvironment().GetIntensityVolume()->GetHeader().GetHeaderEntry(vxVolumeHeader::SliceThickness, sTmp);

  if (sTmp== "")
    return "Thick: --";
  else
    return "Thick: " + sTmp + "mm";

} // GetSliceThicknessString()


/**
 * Return the field of view as a string.
 *
 * @return the field of view as a string
 */
std::string vxRendererOverlay::GetFieldOfViewString()
{
  int1 sTmpStr[201];

  if ( dynamic_cast< vxViewerTypeMPROrtho * >( GetEnvironment().GetViewerType() ) )
  {

    uint2 uAxisX( GetEnvironment().GetCamera()->GetViewRight().GetMaxAbsComponentCoord() );
    uint2 uAxisY( GetEnvironment().GetCamera()->GetViewUp().GetMaxAbsComponentCoord() );
    float4 fVolumeX( GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim()[ uAxisX ] );
    float4 fVolumeY( GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim()[ uAxisY ] );
    float4 fWindowX( GetEnvironment().GetWinRect().GetViewportWidth() );
    float4 fWindowY( GetEnvironment().GetWinRect().GetViewportHeight() );
    float4 fAspectRatioVolume( fVolumeY / fVolumeX );
    float4 fAspectRatioWindow( fWindowY / fWindowX );
    float4 fVolumeMax( max( fVolumeX, fVolumeY ) );
    float4 fWidth( 0.0F );
    float4 fHeight( 0.0F );
    if ( fAspectRatioVolume > fAspectRatioWindow )
    {
      fHeight = GetEnvironment().GetZoomFactor()->GetSize();
      if ( fHeight == fVolumeY )
      {
        fWidth = fVolumeX;
      }
      else
      {
        const float4 fWindowWidth = GetEnvironment().GetZoomFactor()->GetSize() * fAspectRatioWindow;
        fWidth = min( fVolumeX, fWindowWidth );
      }
    }
    else
    {
      fWidth = GetEnvironment().GetZoomFactor()->GetSize();
      if ( fWidth == fVolumeX )
      {
        fHeight = fVolumeY;
      }
      else
      {
        const float4 fWindowHeight = GetEnvironment().GetZoomFactor()->GetSize() / fAspectRatioWindow;
        fHeight = min( fVolumeY, fWindowHeight );
      }
    }

    // only display one number of width and height are the same
    if ( fWidth == fHeight )
    {
      sprintf( sTmpStr, "FOV: %.1fcm^2", fWidth );
    }
    else
    {
      sprintf( sTmpStr, "FOV: %.1fx%.1fcm", fWidth, fHeight );
    }
  }
  else if ( dynamic_cast< vxViewerTypeMPROblique * >( GetEnvironment().GetViewerType() ) )
  {
  }

  return sTmpStr;

} // GetFieldOfViewString()


/**
 * Return the study date as a string.
 *
 * @return the study date as a string
 */
std::string vxRendererOverlay::GetStudyDateString()
{
  std::string sTmp;
  GetEnvironment().GetIntensityVolume()->GetHeader().GetHeaderEntry( vxVolumeHeader::StudyDate, sTmp );

  if (sTmp== "")
    return "Date: --";
  else
    return "Date: " + sTmp;

} // GetStudyDateString()


/**
 * Get a string that represents the RAO or LAO angulation as if it was a C-arm
 *
 * @param enviroment the environment
 *
 * @return the resultant string
 */
std::string vxRendererOverlay::GetRaoLaoString()
{

  const float4 fDegreesRao( vxUtils::GetRaoLaoDegrees( Normal3Df( GetEnvironment().GetCamera()->GetViewDirection() ) ) );
  std::string sText;

  if ( fDegreesRao == 0 )
  {
    sText = "AP";
  }
  else if ( fDegreesRao == 180 )
  {
    sText = "PA";
  }
  else if ( fDegreesRao > 90 )
  {
    sText = "RPO: " + ToAscii( int( 180 - fDegreesRao ) ) + "deg";
  }
  else if ( fDegreesRao > 0 )
  {
    sText = "RAO: " + ToAscii( int( fDegreesRao ) ) + "deg";
  }
  else if ( fDegreesRao < -90 )
  {
    sText = "LPO: " + ToAscii( int( 180 + fDegreesRao ) ) + "deg";
  }
  else if ( fDegreesRao < 0 )
  {
    sText = "LAO: " + ToAscii( int( -fDegreesRao ) ) + "deg";
  }

  return sText;

} // GetRaoLaoString()


/**
 * Get a string that represents the cranial or caudal angulation as if it was a C-arm
 *
 * @param enviroment the environment
 *
 * @return the resultant string
 */
std::string vxRendererOverlay::GetCranCaudString()
{

  const float4 fDegreesCran( vxUtils::GetCranCaudDegrees( Normal3Df( GetEnvironment().GetCamera()->GetViewDirection() ) ) );
  std::string sText;

  if ( fDegreesCran >= 0 )
  {
    sText = "CRAN: " + ToAscii( int( fDegreesCran ) ) + "deg";
  }
  else
  {
    sText = "CAUD: " + ToAscii( int( -fDegreesCran ) ) + "deg";
  }

  return sText;

} // GetCranCaudString()


#undef FILE_REVISION


// $Log: vxRendererOverlay.C,v $
// Revision 1.11.8.1  2009/03/23 15:42:52  gdavidson
// Replaced the Viatronix text overlay with an image
//
// Revision 1.11  2006/10/30 17:35:58  romy
// space adjusted for HU display
//
// Revision 1.10  2006/10/27 07:44:54  romy
// HU display for neighbours added for Zoom > 250
//
// Revision 1.9  2006/10/26 17:40:02  romy
// added Methods to return volume and world cordinates as strings
//
// Revision 1.8  2006/04/20 12:31:54  geconomos
// corrected assignement of reference to vxVolumeHeader
//
// Revision 1.7  2005/11/18 21:15:22  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.6.2.2  2005/08/04 18:57:09  frank
// removed unused cursor 3D tracking
//
// Revision 1.6.2.1  2005/08/04 18:33:32  frank
// debugging code
//
// Revision 1.6  2005/03/08 19:09:13  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.5.2.1  2005/02/08 16:16:46  frank
// fixed field of view display
//
// Revision 1.5  2004/10/23 10:23:34  frank
// made some functions not static
//
// Revision 1.4  2004/09/17 12:48:09  frank
// made random screen drawing more flexible
//
// Revision 1.3  2004/08/31 15:17:07  frank
// fixed zoom and field of view
//
// Revision 1.2  2004/04/06 16:26:41  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.102  2004/01/31 01:59:38  michael
// reference to vxVolumeHeader is sufficient
//
// Revision 1.101  2004/01/30 01:21:34  geconomos
// added a check for WIN_RECT when deciding if rendering is necessary.
//
// Revision 1.100  2004/01/22 16:20:04  wenli
// Using RawToModality()
//
// Revision 1.99  2004/01/14 14:18:19  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.98  2004/01/12 14:18:03  frank
// Made the overlay sensitive to changes in the overlay elements
//
// Revision 1.97  2004/01/07 18:30:17  frank
// Removed dead code
//
// Revision 1.96  2004/01/05 15:41:12  michael
// fixed missing redraw sensitivity of the overlay renderers. Added base class
// to be returning true in RedrawNecessary() if font has changed and made
// derived classes call base class plus their additional stuff.
//
// Revision 1.95  2003/12/18 14:14:01  frank
// Renamed the element ID filename to match class name
//
// Revision 1.94  2003/11/24 13:04:06  frank
// Added RPO and LPO projections
//
// Revision 1.93  2003/11/19 06:15:56  frank
// cleaned up conditionals
//
// Revision 1.92  2003/11/19 01:38:07  frank
// fixed typo
//
// Revision 1.91  2003/11/19 01:36:54  frank
// Centralized a rendering constant to avoid duplication
//
// Revision 1.90  2003/11/04 15:47:40  michael
// accounting for the changes in the overlay renderers (inheritance and naming)
//
// Revision 1.89  2003/10/28 20:37:30  frank
// fixed problem with degree symbol in font
//
// Revision 1.88  2003/10/02 12:29:23  frank
// fixed lowercase orientation letter
//
// Revision 1.87  2003/09/04 17:29:45  michael
// merged the branch "XP1_1" back into the trunc
//
// Revision 1.86  2003/09/02 17:55:52  michael
// made distance between lines (m_fInterlineSpacing) font size dependent
//
// Revision 1.85  2003/08/28 16:50:57  frank
// Fixed orientation lettering in 3D for some cases.
//
// Revision 1.84  2003/08/22 19:24:01  frank
// Issue #3460: Revised orientation lettering method - unified between 2D and 3D
//
// Revision 1.83  2003/08/13 12:55:43  michael
// accounted for change from OverlayStrings to vxOverlayStrings
//
// Revision 1.82.2.1  2003/08/29 15:20:11  frank
// Fixed orientation lettering in 3D
//
// Revision 1.82  2003/08/04 19:30:21  frank
// RAO/LAO now takes a view normal
//
// Revision 1.81  2003/08/04 14:42:14  frank
// Moved rao/lao and cran/caud degree calculations to utility functions
//
// Revision 1.80  2003/07/09 19:08:51  frank
// Added string for cran/caud and rao/lao angulation
// we do not yet have a definitive definition of the angles so it is still subject to change
//
// Revision 1.79  2003/07/07 13:22:47  frank
// Removed xyz position
//
// Revision 1.78  2003/06/18 15:19:58  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.77  2003/06/12 20:08:18  wenli
// change the depth of the glOrth() to -50 to  50
//
// Revision 1.76  2003/06/09 12:30:23  michael
// Implemented a true colormap object that returns discretized colortables based
// on either volume header (taking modality and bits stored) or on the maximum
// index needed -> size = max index + 1
//
// Revision 1.75  2003/06/05 13:27:58  michael
// fixed bug in current voxel display
//
// Revision 1.74  2003/06/04 18:33:54  michael
// accounting for new vxIntensityConverter parameters (header, not modality)
//
// Revision 1.73  2003/06/03 20:31:53  michael
// added check to see if volume is valid (each dimension > 0)
//
// Revision 1.72  2003/05/23 20:13:20  michael
// added image information display flag
//
// Revision 1.71  2003/05/23 18:56:41  frank
// Moved "show patient information" logic out of render function
//
// Revision 1.70  2003/05/23 18:54:32  frank
// Showed modality information even if patient information is turned off
//
// Revision 1.69  2003/05/22 18:42:20  frank
// showed window/level regardless of modality
//
// Revision 1.68  2003/05/22 14:56:57  dongqing
// code review
//
// Revision 1.67  2003/05/22 13:04:29  frank
// code review
//
// Revision 1.66  2003/05/19 14:40:21  frank
// Removed unused return parameter
//
// Revision 1.65  2003/05/17 15:44:50  michael
// reformatting
//
// Revision 1.64  2003/05/05 18:04:55  frank
// Issue #3167: Removed "mm: --" from overlay
//
// Revision 1.63  2003/05/05 16:04:35  frank
// Refactored overlay rendering code so it can be easily re-used
//
// Revision 1.62  2003/05/02 18:27:05  frank
// Moved 2D viewer to more flexible overlay strings
//
// Revision 1.61  2003/05/01 19:53:20  frank
// Added the rest of the string locations
//
// Revision 1.60  2003/05/01 15:46:43  frank
// not yet working
//
// Revision 1.58  2003/05/01 13:35:37  frank
// Made two utility functions static so others can use them as well.
//
// Revision 1.57  2003/04/29 15:10:59  frank
// Added random screen locations
//
// Revision 1.56  2003/04/18 15:20:27  frank
// Removed HUconverter
//
// Revision 1.55  2003/04/11 18:33:50  frank
// Got overlay strings working
//
// Revision 1.54  2003/04/10 12:04:18  frank
// Added Render method to render all overlay strings (eventually)
//
// Revision 1.53  2003/03/22 16:03:47  michael
// fixed boundary check (not 0 to n-1) but (1 to n)
//
// Revision 1.52  2003/03/21 21:14:38  michael
// fixed out of bounce slice values for fly view (issue #2955)
//
// Revision 1.51  2003/03/21 15:40:40  geconomos
// Updated code for the removal of the viewer type enumeration.
//
// Revision 1.50  2003/02/28 21:43:00  michael
// removed vxWinRect from vxRenderer. Is now only in vxViewer and is set in
// vxEnvironment when needed (during rendering or manipulation).
//
// Revision 1.49  2003/01/22 22:02:16  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.48  2003/01/13 23:25:46  frank
// Removed old parameters, fixed text layout
//
// Revision 1.47  2003/01/08 21:48:27  frank
// Added additional study info to overlays.
//
// Revision 1.46  2003/01/07 15:23:58  frank
// Fixed slice display in x-ray mode
//
// Revision 1.45  2002/12/20 16:14:45  frank
// Added x-ray projection
//
// Revision 1.44  2002/11/27 20:58:41  frank
// Changed interline spacing
//
// Revision 1.43  2002/11/22 13:14:03  michael
// chnaged overlay information for oblique (image number)
//
// Revision 1.42  2002/11/22 03:52:43  frank
// No need for view-type logic inside of the window/level string generator.  These are not related and prevent extended views from using the function.
//
// Revision 1.41  2002/11/22 01:46:51  frank
// Added current HU/Raw display as string function to remove tons of redundant code.
//
// Revision 1.40  2002/11/21 23:30:08  michael
// added overlay information for oblique
//
// Revision 1.39  2002/11/21 21:02:45  geconomos
// now also displaying W/L in 3D View (ingmar)
//
// Revision 1.38  2002/11/20 17:46:31  michael
// added view dependent stuff to handle VIEW_MPR_OBLIQUE, too
//
// Revision 1.37  2002/11/18 16:36:55  michael
// completed plane object renaming
//
// Revision 1.36  2002/11/12 00:11:54  michael
// accounted for rounding error when converting world plane distance to volume index coordinates
//
// Revision 1.35  2002/11/07 00:05:28  ingmar
// adjusted displayed number to policy that plane distance represents world coordinates
//
// Revision 1.34  2002/10/16 15:28:00  michael
// replace houndsfeld convertion by call to vxIntensityConverter
//
// Revision 1.33  2002/10/05 01:47:35  frank
// Added Viatronix to bottom of screen.
//
// Revision 1.32  2002/09/30 17:07:46  frank
// Issue #2695: Cleaned up phase number and patient information opn phase renderer overlay display.
//
// Revision 1.31  2002/09/24 19:41:14  frank
// fixed issue #2675
//
// Revision 1.30  2002/09/24 15:32:49  frank
// Added explanation for missing images when the slab or slice is outside of the current crop box.
//
// Revision 1.29  2002/09/19 14:26:02  michael
// completed transition from vxTransferFunction to vxColormap
//
// Revision 1.28  2002/09/09 20:40:20  frank
// Fixed upper left string display.
//
// Revision 1.27  2002/08/22 15:02:57  frank
// update
//
// Revision 1.26  2002/08/17 12:31:56  frank
// Centralized display of strings in the upper two corners. Now you just add strings to the two lists and they are rendered for you by RenderCornerStrings.  No more hand formatting.
//
// Revision 1.25  2002/07/22 23:17:22  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-22-2002"
// into the main trunc
//
// Revision 1.24  2002/07/22 18:24:11  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-15-2002"
// into the main trunc
//
// Revision 1.23  2002/06/27 18:06:23  uday
// Slice Numbers in MIP mode.
//
// Revision 1.22.2.3  2002/07/09 18:21:22  michael
// Changed display of slice number to show 1 at top (head of patient) and also
// adjusted vxManipulatorPlaneOrtho to have HOME is slice 1 and END is the
// last one.
//
// Revision 1.22.2.2  2002/07/08 14:38:18  frank
// Fixed MIP/VR caption.
//
// Revision 1.22.2.1  2002/07/01 18:53:55  uday
// Slice Numbers in mip mode
//
// Revision 1.22  2002/06/26 21:56:06  uday
// Using GetSlabPair() to get thin slab indices.
//
// Revision 1.21  2002/06/26 17:35:02  uday
// Added thin slab numbering to GetSliceNumberString().
//
// Revision 1.20  2002/06/19 17:31:40  frank
// Fixed study date format.
//
// Revision 1.19  2002/06/12 23:19:45  frank
// Moved DICOM string functions back to RendererOverlay.
//
// Revision 1.18  2002/05/22 19:59:04  jenny
// fixed orientation lettering and removed a bunch of functions to process the age, patient id, sex and so on, put them in VolumeHeader
//
// Revision 1.17  2002/05/21 15:31:32  michael
// corrected misspelled "saggital" to "sagittal"
//
// Revision 1.16  2002/05/20 14:36:17  michael
// Added "ears" in case the age ends with a Y which stands for years.
//
// Revision 1.15  2002/05/20 14:30:24  michael
// fixed display of information. "Name: ", "ID: ", "Age: ", and "Sex:" are always
// used as prefix for the information
//
// Revision 1.14  2002/05/17 21:28:20  jenny
// switched the letters used by 2DOverlay
//
// Revision 1.13  2002/05/17 00:47:49  jenny
// worked on VolumeHeader Version 4.0
//
// Revision 1.12  2002/05/08 22:11:11  jenny
// worked on orientation lettering
//
// Revision 1.11  2002/05/07 19:57:21  michael
// fixed patient age and sex display in the viewers
//
// Revision 1.10  2002/05/01 13:49:01  michael
// Started code review related stuff ...
//
// Revision 1.9  2002/04/29 18:10:50  frank
// Added patient age.
//
// Revision 1.8  2002/04/29 15:26:06  frank
// Removed excess information on screen.
//
// Revision 1.7  2002/04/27 19:47:02  frank
// Added some scan information.
//
// Revision 1.6  2002/04/27 05:00:08  frank
// Cleaning up code.
//
// Revision 1.5  2002/04/27 04:20:54  frank
// Began centralizing the patient information block.
//
// Revision 1.4  2002/04/26 13:39:55  michael
// coding guidelines: adjusted some parameter and variable naming
//
// Revision 1.3  2002/04/26 01:38:36  frank
// Removed dead code.
//
// Revision 1.2  2002/04/25 19:39:58  frank
// Centralized orientation lettering in vxRendererOverlay class.
//
// Revision 1.1  2002/04/24 19:20:57  frank
// Initial version.
//
// Revision 1.20  2002/04/21 22:57:40  michael
// Changed HU converter to return int2 instead of requiring it to be a second parameter
//
// Revision 1.19  2002/04/21 21:58:28  michael
// removed HU conversion from WindowLevel class since it is not a property of this object
//
// Revision 1.18  2002/04/21 20:07:47  frank
// Lined up the text.
//
// Revision 1.17  2002/04/16 19:05:21  michael
// introduced vxWindowLevel class and use it throughout the code
//
// Revision 1.16  2002/04/14 00:18:00  michael
// adjusted method name
//
// Revision 1.15  2002/04/09 21:20:48  dmitry
// Offset calculated for the plane.
//
// Revision 1.14  2002/04/01 23:29:51  michael
// Fixed offsets to match 3D overlay offsets and corrected wrong labels
//
// Revision 1.13  2002/04/01 16:37:09  michael
// push and pop of attributes happens once in Render()
//
// Revision 1.12  2002/03/29 16:06:33  michael
// cosmetics
//
// Revision 1.11  2002/03/28 21:44:35  geconomos
// Disabled depth test  in Render().
//
// Revision 1.10  2002/03/27 16:10:05  michael
// cosmetics
//
// Revision 1.9  2002/03/27 15:07:56  michael
// adjusted Get.SetSize() to Get/SetViewportSize()
//
// Revision 1.8  2002/03/25 16:33:46  michael
// adjusted to VolumeData::ModalityEnum
//
// Revision 1.7  2002/03/21 22:00:35  uday
// push-pop edit
//
// Revision 1.6  2002/03/21 20:08:39  jenny
// put comments
//
// Revision 1.5  2002/03/21 19:16:41  michael
// added log history lines
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererOverlay.C,v 1.11.8.1 2009/03/23 15:42:52 gdavidson Exp $
// $Id: vxRendererOverlay.C,v 1.11.8.1 2009/03/23 15:42:52 gdavidson Exp $
