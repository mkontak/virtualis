// $Id: vxRendererOverlayMPRCurved.C,v 1.4 2007/11/20 19:12:59 romy Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Frank Dachille (frank@viatronix.com)

/**
 * This renderer displays graphs of lumen area, diameter, and measurements for the curved MPR views.
 */


// includes
#include "StdAfx.h"
#include "vxRendererOverlayMPRCurved.h"
#include "vxRibbon.h"
#include "vxIDs.h"
#include "vxVesselPreferences.h"
#include "vxVesselUtilities.h"
#include "ValueRange.h"
#include "vxOverlayStrings.h"
#include "vxEnvironment.h"
#include "vxVessel.h"


// Defines
#define FILE_REVISION "$Revision: 1.4 $"


// namespaces
using namespace std;


/**
 * Default constructor
 *
 * @param environment the environment
 */
vxRendererOverlayMPRCurved::vxRendererOverlayMPRCurved( vxEnvironment & environment )
:vxRendererOverlayMPR( environment )
{
} // vxRendererOverlayMPRCurved()


/**
 * Destructor
 */
vxRendererOverlayMPRCurved::~vxRendererOverlayMPRCurved()
{
} // ~vxRendererOverlayMPRCurved()


/**
 * Has something changed that makes redrawing necessary?
 *
 * @return redraw necessary?
 */
bool vxRendererOverlayMPRCurved::IsRedrawNecessary()
{
  bool bIsRedrawNecessary( false );
  try
  {
    const bool bBaseClassNeedsRedraw( vxRendererOverlayMPR::IsRedrawNecessary() );
    const bool bRenderingModeChanged( GetEnvironment().IsModified( vxEnvironment::RENDERING_MODE ) );
    const bool bPreferencesChanged( GetEnvironment().IsModified( vxIDs::VesselPreferences ));
    const bool bRibbonChanged( GetEnvironment().IsModified( vxIDs::Ribbon ) );
    const bool bVesselChanged( GetEnvironment().IsModified( vxIDs::VesselArray ) );
    const bool bCurvedPlaneChanged( GetEnvironment().IsModified( vxEnvironment::PLANE_MPR ) );

    bIsRedrawNecessary = bBaseClassNeedsRedraw ||
                         bRenderingModeChanged ||
                         bPreferencesChanged ||
                         bRibbonChanged ||
                         bVesselChanged ||
                         bCurvedPlaneChanged;
  }
  catch ( ... )
  {
    LogErr( "Unhandled exception", "vxRendererOverlayMPRCurved", "IsRedrawNecessary" );
  }

  return bIsRedrawNecessary;
} // IsRedrawNecessary()


/**
 * this is ONLY for image space parallel algorithms
 */
void vxRendererOverlayMPRCurved::Render()
{
  try
  {

    // call base class for basic information to be displayed
    vxRendererOverlayMPR::Render();

    // recompute the interline spacing in case it has changed
    m_fInterlineSpacing = m_fFontHeightFactor * GetEnvironment().GetFont()->GetPixelHeight();

    // see if the user wishes to display the graphs
    vxVesselPreferences * pPreferences = dynamic_cast< vxVesselPreferences * >(GetEnvironment().GetElement( vxIDs::VesselPreferences ));
    if( pPreferences == NULL )
    {
      LogDbg( "Preferences pointer is NULL", "vxRendererOverlayMPRCurved", "RenderWindow" );
      return;
    }

    // get the ribbon
    vxRibbon * pRibbon = static_cast<vxRibbon *>(GetEnvironment().GetElement( vxIDs::Ribbon ));
    if (pRibbon == NULL)
    {
      LogDbg( "Ribbon pointer is NULL", "vxRendererOverlayMPRCurved", "RenderWindow" );
      return;
    }
    
    // get the viewport size
    uint4 uHeight = GetEnvironment().GetWinRect().GetViewportHeight();
    uint4 uWidth = GetEnvironment().GetWinRect().GetViewportWidth();

    // get the vessel
    vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
    if( pVessel == NULL )
    {
      //LogDbg( "Vessel pointer is NULL", "vxRendererOverlayMPRCurved", "Render" );
      return;
    }

    // only render if there is some data in the vessel
    if ( pVessel->size() < 2 )
      return;

    // setup a simple projection matrix
    SetupRenderState();

    if ( pPreferences->GetDisplayGraphs() )
    {
      // area graph on the window left side
      glPushAttrib( GL_ALL_ATTRIB_BITS );
      glEnable( GL_BLEND );
      glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
      glShadeModel( GL_FLAT );
      glBegin(GL_QUAD_STRIP);

      // get the maximum wall area
      const ValueRange<float4> wallAreaRange( pVessel->GetWallAreaRange( pVessel->GetEntireDomain() ) );
      float4 fMaxWallArea( wallAreaRange.GetMaximum() );
      const bool bShowAreaGraph( fMaxWallArea != 0.0F );
      if ( fMaxWallArea <= 0.0F ) fMaxWallArea = 1.0F;

      // first draw the wall area
      uint4 y=0;
      for ( ; y<uHeight && bShowAreaGraph; ++y )
      {

        // sample the area of the vessel at this scanline
        const float4 fSection( vxVesselUtilities::GetVesselIndex( y, pRibbon, uHeight ) );
        if ( fSection >= 0.0F && fSection <= pVessel->size() - 1 )
        {
          const float4 fLumenArea( pVessel->ResampleLumenAreaSquareMM( fSection ) );
          const float4 fWallArea( pVessel->ResampleWallAreaSquareMM( fSection ) );
          const float4 xLumen( fLumenArea / fMaxWallArea * 100.0F );
          const float4 xWall( fWallArea / fMaxWallArea * 100.0F );

          // draw the vessel wall area
          glColor4f( 0.0F, 1.0F, 0.5F, 1.0F );
          glVertex2f( xLumen, uHeight - 1 - y );
          glColor4f( 1.0F, 0.5F, 0.5F, 1.0F );
          glVertex2f( xWall, uHeight - 1 - y );
        }

      } // for each vessel section
      glEnd();

      // draw the lumen area now
      glShadeModel( GL_SMOOTH );
      glBegin(GL_QUAD_STRIP);
      for ( y=0; y<uHeight && bShowAreaGraph; ++y )
      {

        // sample the area of the vessel at this scanline
        const float4 fSection( vxVesselUtilities::GetVesselIndex( y, pRibbon, uHeight ) );
        if ( fSection >= 0.0F && fSection <= pVessel->size() - 1 )
        {
          const float4 fLumenArea( pVessel->ResampleLumenAreaSquareMM( fSection ) );
          const float4 x( fLumenArea / fMaxWallArea * 100.0F );

          // draw the vessel lumen area
          glColor4f( 0.0F, 1.0F, 0.5F, 0.25F );
          glVertex2f( 0.0F, uHeight - 1 - y );
          glColor4f( 0.0F, 1.0F, 0.5F, 1.0F );
          glVertex2f( x, uHeight - 1 - y );
        }

      } // for each vessel section
      glEnd();

      // get the maximum diameter
      ValueRange<float4> diameterRange( pVessel->GetDiameterRange( pVessel->GetEntireDomain() ) );
      float4 fMaxDiameter( diameterRange.GetMaximum() );
      const bool bShowDiameterGraph( fMaxDiameter != 0.0F );
      if ( fMaxDiameter <= 0.0F ) fMaxDiameter = 1.0F;

      // minimum diameter graph on the window right side
      glEnable( GL_LINE_SMOOTH );
      glLineWidth( 1.5F );
      glColor4f( 1, 0, 0, 1 );
      glBegin( GL_LINE_STRIP );
      for ( y=0; y<uHeight && bShowDiameterGraph; ++y )
      {

        // sample the vessel minimum diameter
        const float4 fSection( vxVesselUtilities::GetVesselIndex( y, pRibbon, uHeight ) );
        if ( fSection >= 0.0F && fSection <= pVessel->size() - 1 )
        {
          const float4 fMinDiameterSection( pVessel->ResampleMinMaxDiameter( fSection ).GetMinimum() );
          const float4 x( fMinDiameterSection / fMaxDiameter * 100.0F );

          // draw it
          glVertex2f( uWidth - x, uHeight - 1 - y );
        }

      } // for each vessel section
      glEnd();
      glDisable( GL_BLEND );
      glPopAttrib();

      // Draw text in the measurement lines
      if ( ( pVessel->GetMeasurementDomain().first  >= 0 ) && ( pVessel->GetMeasurementDomain().first  < pVessel->size() ) &&
          ( pVessel->GetMeasurementDomain().second >= 0 ) && ( pVessel->GetMeasurementDomain().second < pVessel->size() ) &&
          GetEnvironment().GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_IMAGE_INFORMATION ) )
      {
        Rect<float4> viewPortRect(0.0F, 0.0F, GetEnvironment().GetWinRect().GetViewportSize().first, GetEnvironment().GetWinRect().GetViewportSize().second);
        const Triple<float4> textColor(1.0F, 1.0F, 1.0F);
        const float4 fLeft( m_fSideSpace );
        const float4 fRight( viewPortRect.m_right - m_fSideSpace );
        const float4 fMiddle( ( fLeft + fRight ) / 2 );
        const float4 fNudgeAbove( 6 );
        const float4 fNudgeBelow( 1 );
        float fAreaNormal( pVessel->ResampleLumenAreaSquareMM( pVessel->GetMeasurementDomain().first ) );
        float fMeanDiameterNormal( 2 * sqrt( fAreaNormal / M_PI ) );
        float fMinDiameterNormal( pVessel->ResampleMinMaxDiameter( pVessel->GetMeasurementDomain().first ).GetMinimum() );

        // normal text
        if ( pVessel->GetMeasurementDomain().first >= 0 && pVessel->GetMeasurementDomain().first <= pVessel->size()-1 )
        {
          const float4 fScanline( vxVesselUtilities::GetScanlinePosition( pVessel->GetMeasurementDomain().first, pRibbon ) );
          const float4 y( Interpolate( 0.0F, fScanline, pRibbon->GetScanlineData().size()-1, 0.0F, GetEnvironment().GetWinRect().GetViewportHeight() - 1.0F ) );

          // left side area
          string sText( "Area: " + ToAscii( RoundDecimals( fAreaNormal / 100.0F, 2 ) ) + " cm^2 (normal)" );
          if ( bShowAreaGraph )
          {
            GetEnvironment().GetFont()->PutFormattedString( sText, GLFontGeorge::ALIGN_LEFT,
                                                            Point2D<int4>( fLeft, y + fNudgeAbove ), textColor );
          }

          // right side area
          const float4 fMinDiam( RoundDecimals( pVessel->ResampleMinMaxDiameter( pVessel->GetMeasurementDomain().first ).GetMinimum(), 1) );
          sText = "Min diam: " + ToAscii( fMinDiam ) + " mm";
          if ( bShowDiameterGraph )
          {
            GetEnvironment().GetFont()->PutFormattedString( sText, GLFontGeorge::ALIGN_RIGHT,
                                                            Point2D<int4>( fRight, y + fNudgeAbove ), textColor );
          }
        }

        if ( pVessel->GetMeasurementDomain().second >= 0 && pVessel->GetMeasurementDomain().second <= pVessel->size()-1 )
        {
          const float4 fScanline( vxVesselUtilities::GetScanlinePosition( pVessel->GetMeasurementDomain().second, pRibbon ) );
          const float4 y( Interpolate( 0.0F, fScanline, pRibbon->GetScanlineData().size()-1, 0.0F,
                                      GetEnvironment().GetWinRect().GetViewportHeight() - 1.0F ) );

          // stenosis calculation
          float4 fAreaStenotic( pVessel->ResampleLumenAreaSquareMM( pVessel->GetMeasurementDomain().second ) );
          float4 fMeanDiameterStenotic( 2 * sqrt( fAreaStenotic / M_PI ) );
          float4 fMinDiameterStenotic( pVessel->ResampleMinMaxDiameter( pVessel->GetMeasurementDomain().second ).GetMinimum() );  
          std::string sText = "Area: " + ToAscii( RoundDecimals( fAreaStenotic / 100.0F, 2 ) ) + " cm^2";
          if ( bShowAreaGraph )
          {
            GetEnvironment().GetFont()->PutFormattedString( sText, GLFontGeorge::ALIGN_LEFT,
                                                            Point2D<int4>( fLeft, y + fNudgeAbove ), textColor );
          }

          // stenotic area
          if ( bShowAreaGraph && pVessel->GetStenosisValid() )
          {
            float4 fPercentArea( vxVesselSection::CalculatePercentStenosis( fAreaNormal, fAreaStenotic ) );
            string sAreaStenosisText( "Area stenosis: " + ToAscii( RoundDecimals( fPercentArea, 1 ) ) + "%%" );
            GetEnvironment().GetFont()->PutFormattedString( sAreaStenosisText, GLFontGeorge::ALIGN_LEFT,
                                                              Point2D<int4>( fLeft, y - m_fInterlineSpacing + fNudgeBelow ),
                                                              textColor );
          }

          // stenotic mean diameter
          if ( bShowAreaGraph && pVessel->GetStenosisValid() )
          {
            float4 fPercentMeanDiameter( vxVesselSection::CalculatePercentStenosis( fMeanDiameterNormal, fMeanDiameterStenotic ) );
            string sAreaStenosisText( "Mean Diam stenosis: " + ToAscii( RoundDecimals( fPercentMeanDiameter, 1 ) ) + "%%" );
            GetEnvironment().GetFont()->PutFormattedString( sAreaStenosisText, GLFontGeorge::ALIGN_LEFT,
                                                              Point2D<int4>( fLeft, y - 2 * m_fInterlineSpacing + fNudgeBelow ),
                                                              textColor );
          }

          // right side area
          const float4 fMinDiam = RoundDecimals( pVessel->ResampleMinMaxDiameter( pVessel->GetMeasurementDomain().second ).GetMinimum(), 1 );
          sText = "Min diam: " + ToAscii( fMinDiam ) + " mm";
          if ( bShowDiameterGraph )
          {
            GetEnvironment().GetFont()->PutFormattedString( sText, GLFontGeorge::ALIGN_RIGHT,
                                                            Point2D<int4>( fRight, y + fNudgeAbove ), textColor );
          }
          if ( bShowDiameterGraph && pVessel->GetStenosisValid() )
          {
            float4 fPercentMinDiameter( vxVesselSection::CalculatePercentStenosis( fMinDiameterNormal, fMinDiameterStenotic ) );  
            string sDiamStenosisText( "Min Diam stenosis: " + ToAscii( RoundDecimals( fPercentMinDiameter, 0 ) ) + "%%" );  
            GetEnvironment().GetFont()->PutFormattedString( sDiamStenosisText, GLFontGeorge::ALIGN_RIGHT,
                                                              Point2D<int4>( fRight, y - m_fInterlineSpacing + fNudgeBelow ),
                                                              textColor );
          }
        }
      } // endif measure lines

    } // if display graphs


    // Render additional overlay elements such as the field of view
    vxOverlayStrings * pOverlayStrings = dynamic_cast< vxOverlayStrings * >( GetEnvironment().GetElement( vxIDs::OverlayStrings ) );
    if ( pOverlayStrings == NULL )
    {
      LogWrn( "Unable to find overlay strings", "", "" );
      return;
    }

		//TODO: Fix this after RSNA_2007
    // display the field of view
   // if ( GetEnvironment().GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_IMAGE_INFORMATION ) )
     // pOverlayStrings->AddElement( vxOverlayStrings::vxOverlayElement( GetFieldOfViewString(), vxOverlayStrings::vxOverlayElement::UpperRight ) );

    RestoreRenderState();

  }
  catch ( ... )
  {
    LogDbg( "Unhandled exception", "vxRendererOverlayMPRCurved", "Render" );
  }
} // Render()


/**
 * return the slice type text string
 *
 * @return the slice type string
 */
std::string vxRendererOverlayMPRCurved::GetSliceTypeString()
{

  try
  {
    std::string sPrefix( "" );
    const float4 fSlabThickness( GetEnvironment().GetRenderingMode()->GetSlabThickness() );

    if ( GetEnvironment().GetRenderingMode()->GetRenderingAlgorithm() == vxRenderingMode::MIP )
    {
      sPrefix = ToAscii( fSlabThickness ) + "mm MIP ";
    }
    else if ( GetEnvironment().GetRenderingMode()->GetRenderingAlgorithm() == vxRenderingMode::XRAY )
    {
      sPrefix = ToAscii( fSlabThickness ) + "mm x-ray ";
    }

    vxRibbon * pRibbon = dynamic_cast< vxRibbon * >( GetEnvironment().GetElement( vxIDs::Ribbon ) );
    if( pRibbon == NULL )
      return "";

    return sPrefix + pRibbon->GetProjectionType();
  }
  catch ( ... )
  {
    LogDbg( "Unhandled exception", "vxRendererOverlayMPRCurved", "Render" );
    return "ERROR";
  }

} // GetSliceTypeString()


/**
 * return the current slice number as a string
 *
 * @return the slice number as a string
 */
std::string vxRendererOverlayMPRCurved::GetSliceNumberString()
{

  try
  {
    // get the curved plane
    vxPlaneCurved * pCurvedPlane = dynamic_cast<vxPlaneCurved *>(GetEnvironment().GetPlaneCurved());
    if (pCurvedPlane == NULL)
    {
      LogDbg( "Curved Plane pointer is NULL", "vxRendererOverlayMPRCurved", "Render" );
      return "ERROR";
    }

    // create the angle string
    int4 iAngle( vxRibbon::GetProjectionAngleDegrees( pCurvedPlane ) + 0.5F );
    while ( iAngle < 0 ) iAngle += 360;
    iAngle %= 360;
    return "Angle: " + ToAscii( iAngle ) + "/360";
  }
  catch ( ... )
  {
    LogDbg( "Unhandled exception", "vxRendererOverlayMPRCurved", "Render" );
    return "ERROR";
  }

} // GetSliceNumberString()


/**
 * Return the field of view as a string.
 *
 * @return the field of view as a string suitable for framing
 */
std::string vxRendererOverlayMPRCurved::GetFieldOfViewString()
{
  try
  {
    int1 sTmpStr[201] = "\n";

    // get some data
    vxRibbon * pRibbon = static_cast<vxRibbon *>(GetEnvironment().GetElement( vxIDs::Ribbon ));

    // see if we have enough data to render
    if ( pRibbon != NULL && GetEnvironment().GetIntensityVolume() != NULL )
    {

      // get the current vessel
      vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );

      // if there is a current vessel and some scanlines
      if ( pVessel != NULL && pRibbon->GetScanlineData().size() > 0 )
      {
        // the width is the width of the ribbon
        const vxRibbon::ScanlineData & scanline = pRibbon->GetScanlineData()[ pRibbon->GetScanlineData().size() / 2 ];
        const float4 fWidthMM( ( scanline.m_rightPoint - scanline.m_leftPoint ).GetLength() );
        const float4 fHeightMM( pRibbon->GetHeightImageMM() );

        // render the string
        sprintf( sTmpStr, "FOV: %.1fcm x %.1fcm", fWidthMM / 10.0F, fHeightMM / 10.0F );

      } // if current vessel

    } // if enough data

    return sTmpStr;
  }
  catch ( ... )
  {
    LogDbg( "Unhandled exception", "vxRendererOverlayMPRCurved", "Render" );
    return "ERROR";
  }

} // GetFieldOfViewString()


// undefines
#undef FILE_REVISION


// $Log: vxRendererOverlayMPRCurved.C,v $
// Revision 1.4  2007/11/20 19:12:59  romy
// disabled the buggy display of FOV for RSNA_2007
//
// Revision 1.3  2006/09/25 13:22:53  geconomos
// commented out unneeded  log debugging
//
// Revision 1.2  2006/09/20 14:37:05  geconomos
// added checks for null ribbon pointer
//
// Revision 1.1  2006/08/17 14:01:12  gdavidson
// Moved CurvedMPR to vxViewer
//
// Revision 1.8  2006/06/05 15:07:38  frank
// updated for visual studio 2005
//
// Revision 1.7  2005/11/18 21:12:06  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.6.2.2  2005/07/26 13:43:44  frank
// added exception handling
//
// Revision 1.6.2.1  2005/06/22 17:30:48  frank
// removed dead code
//
// Revision 1.6  2005/06/08 15:00:49  vxconfig
// formatting
//
// Revision 1.5  2005/06/07 18:10:42  vxconfig
// removed the vessel array
//
// Revision 1.4  2004/12/22 14:11:35  frank
// commented out debug messages
//
// Revision 1.3  2004/10/23 10:25:19  frank
// made some functions not static
//
// Revision 1.2  2004/10/05 14:43:29  frank
// moving the rendering mode out of the data renderer software 3D class
//
// Revision 1.1  2004/08/09 19:29:34  frank
// first version in new V3D Viewer
//
// Revision 1.3  2004/06/17 12:59:48  frank
// avoided showing zero measurement values for simple curved MPR vessels
//
// Revision 1.2  2004/04/06 16:31:40  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.8  2004/03/02 01:10:31  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.7.2.1  2004/02/25 14:59:52  frank
// formatting
//
// Revision 1.7  2004/02/02 15:08:12  frank
// fixed the line width
//
// Revision 1.6  2004/01/05 20:11:51  frank
// formatting
//
// Revision 1.5  2003/12/22 20:03:16  frank
// should not show image information sometimes
//
// Revision 1.4  2003/12/18 17:05:31  frank
// Optimizing redraws
//
// Revision 1.3  2003/12/15 16:18:50  frank
// Was sometimes showing 360/360
//
// Revision 1.2  2003/11/26 02:27:58  frank
// Fixed display of projection type in upper right
//
// Revision 1.1  2003/11/24 16:42:10  frank
// Renamed the class
//
// Revision 1.21  2003/11/19 02:06:08  frank
// Adjusted the interline spacing
//
// Revision 1.20  2003/10/29 20:58:52  frank
// Added rotating curved MPR preference and disablement
//
// Revision 1.19  2003/10/24 15:41:54  frank
// Removed space in percentage display
//
// Revision 1.18  2003/09/30 18:51:35  frank
// Hid patient information on the curved MPR view
//
// Revision 1.17  2003/09/08 21:05:31  frank
// Added mean diameter stenosis calculation and display
//
// Revision 1.16  2003/09/08 19:53:58  frank
// Degree symbol is no longer displaying
//
// Revision 1.15  2003/08/25 15:22:12  frank
// Adjusted significant digits
//
// Revision 1.14  2003/08/13 13:31:43  michael
// accounted for change from OverlayStrings to vxOverlayStrings
//
// Revision 1.13  2003/07/24 17:53:53  michael
// Changed RenderWindow() to pass in thread ID so that the progress of a
// multi-threaded rendering can be monitored
//
// Revision 1.12  2003/07/09 19:19:16  frank
// added degree symbol for degrees
//
// Revision 1.11  2003/06/30 14:18:59  dongqing
// change the stenosis from Captial case to Lower case.
//
// Revision 1.10  2003/06/30 13:12:14  dongqing
// adjust display format of the stenosises
//
// Revision 1.9  2003/06/27 20:13:59  dongqing
// add the stenosis based on maximum diameter
//
// Revision 1.8  2003/06/09 14:30:46  frank
// Prevented stenosis text from dislaying unless the stenosis line is manipulated
//
// Revision 1.7  2003/06/04 19:05:48  michael
// added namespace declaration
//
// Revision 1.6  2003/05/30 12:32:22  frank
// Moved the vessel data text to the lower left
//
// Revision 1.5  2003/05/29 14:05:09  frank
// Fixed debug messages
//
// Revision 1.4  2003/05/28 17:46:07  frank
// Displayed the vessel wall area on the graph.
//
// Revision 1.3  2003/05/27 14:05:27  frank
// Throttled the dislay of image information
//
// Revision 1.2  2003/05/27 14:04:11  frank
// Made the cross-section area more specific to lumen
//
// Revision 1.1  2003/05/22 16:20:59  frank
// changed name to reflect class derivation
//
// Revision 1.3  2003/05/19 17:24:57  frank
// Removed unused return parameter
//
// Revision 1.2  2003/05/15 16:57:35  frank
// Fixed display of elements according to preferences
//
// Revision 1.1  2003/05/08 19:59:27  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.31  2003/05/05 19:07:09  geconomos
// Coding standards.
//
// Revision 1.30  2003/05/05 17:59:20  frank
// Issued #3173: Added angle display where slice would normally go
//
// Revision 1.29  2003/05/05 17:13:10  frank
// code review
//
// Revision 1.28  2003/05/05 16:06:58  frank
// Added the projection type string to the upper right corner overlay
//
// Revision 1.27  2003/05/01 14:35:29  frank
// reflected parameter change in vxRendererOverlay
//
// Revision 1.26  2003/04/15 18:45:20  frank
// Moving over to overlay strings
//
// Revision 1.25  2003/04/10 12:01:28  frank
// Removed debug information
//
// Revision 1.24  2003/04/09 13:56:54  frank
// Cleaned up includes
//
// Revision 1.23  2003/03/28 14:43:06  frank
// Moved text up a little
//
// Revision 1.22  2003/03/24 15:46:01  frank
// Reflected change in vxVesselUtilities
//
// Revision 1.21  2003/03/21 19:26:49  frank
// Fixed handle selection and flipped vessel so that up is up
//
// Revision 1.20  2003/03/14 16:09:14  frank
// Removed the maximum diameter graph and changed to resampled values
//
// Revision 1.19  2003/03/01 13:20:26  frank
// GetWinRect() was removed from vxManipulator
//
// Revision 1.18  2003/02/26 00:03:21  frank
// Fixed maximum area graph
//
// Revision 1.17  2003/02/25 21:03:22  wenli
// align the area and radius graph
//
// Revision 1.16  2003/02/18 17:57:10  frank
// Added graph on curved and fixed it on luminal
//
// Revision 1.15  2003/01/28 23:20:10  frank
// Added a preference for displaying the graph.
//
// Revision 1.14  2003/01/24 20:16:45  frank
// Overhauled the ribbon to contain more helpful data per scanline.
//
// Revision 1.13  2003/01/24 19:21:33  frank
// Fixed text overlays
//
// Revision 1.12  2003/01/20 19:34:40  wenli
// New Version with updated centerline selection and Hessian Filter
//
// Revision 1.11  2003/01/16 21:26:56  frank
// code review
//
// Revision 1.10  2003/01/14 16:17:37  dongqing
// Code review
//
// Revision 1.9  2002/12/16 23:54:44  frank
// Moved stenosis text out of the way of the stenosis image.
//
// Revision 1.8  2002/11/27 16:55:06  frank
// Added resample functions to improve the quality of graphs.
//
// Revision 1.7  2002/11/22 04:27:49  frank
// Added field of view
//
// Revision 1.6  2002/11/22 01:45:41  frank
// Displayed HU on luminal view
//
// Revision 1.5  2002/11/21 00:51:51  frank
// Let you go to vessel views without a vessel without a crash
//
// Revision 1.4  2002/11/18 16:55:41  michael
// made changes to account for changes to vx and v3D plane objects
//
// Revision 1.3  2002/11/13 02:16:28  frank
// formatting
//
// Revision 1.2  2002/11/13 01:09:51  frank
// Changed display from min diameter to mean diameter.
//
// Revision 1.1  2002/10/31 12:46:52  geconomos
// Moved from Viatronix.Visualization.Ext.Vascular
//
// Revision 1.16  2002/10/30 23:42:22  frank
// Improved look of area graph.
//
// Revision 1.15  2002/10/30 00:03:32  frank
// Changed interface.
//
// Revision 1.14  2002/10/29 16:50:29  frank
// revamped measurement
//
// Revision 1.13  2002/10/29 15:41:23  geconomos
// Added inclusion of header file.
//
// Revision 1.12  2002/10/25 16:07:34  frank
// Got percent stenosis to be live.
//
// Revision 1.11  2002/10/25 01:51:49  frank
// Moved working vessel into the VesselArray
//
// Revision 1.10  2002/10/18 20:39:02  frank
// Format
//
// Revision 1.9  2002/09/27 21:47:10  wenli
// add curved MPR camera
//
// Revision 1.8  2002/09/19 14:26:03  michael
// completed transition from vxTransferFunction to vxColormap
//
// Revision 1.7  2002/09/16 20:22:14  dmitry
// New vxID (s) used.
//
// Revision 1.6  2002/08/24 00:18:42  frank
// fixed % stenosis
//
// Revision 1.5  2002/08/23 23:31:07  frank
// Cleaned up text
//
// Revision 1.4  2002/08/23 17:30:23  frank
// Text output.
//
// Revision 1.3  2002/08/23 17:18:29  ana
// Percentage (%) simbol correct.
//
// Revision 1.2  2002/08/23 15:56:53  ana
// Percent stenosis.
//
// Revision 1.1  2002/08/22 21:28:56  ana
// Luminal graph.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererOverlayMPRCurved.C,v 1.4 2007/11/20 19:12:59 romy Exp $
// $Id: vxRendererOverlayMPRCurved.C,v 1.4 2007/11/20 19:12:59 romy Exp $