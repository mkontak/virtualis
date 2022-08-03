// $Id: vxAnnotation.C,v 1.21 2006/11/23 07:45:43 romy Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Michael Meissner (mailto:meissner@viatronix.com)


// pragmas
#pragma warning (disable : 4786)


// includes
#include "stdafx.h"
#include "vxAnnotation.h"
#include "vxMouseCursor.h"
#include "vxEnvironment.h"
#include "vxViewerType.h"
#include "GLFontGeorge.h"
#include "vxIntensityConverter.h"
#include "vxIntensityStatistic.h"
#include "vxSerializer.h"


// namespaces
using namespace std;


// defines
#define FILE_REVISION "$Revision: 1.21 $"

const float4 vxAnnotation::m_fLineWidth( 2.0F);
const float4 vxAnnotation::m_fLineShadowWidth( 3.0F);
//Image<PixelRGBA<uint1> > vxAnnotation::m_image( PixelRGBA<uint1>( 0, 0, 0, 255 ) );


/**
 * Default constructor
 */
vxAnnotation::vxAnnotation()//:m_image( PixelRGBA<uint1>( 0, 0, 0, 255 ) )
//*******************************************************************
{
  CommonInit();

	
} // constructor


/** 
 * Copy constructor
 */
vxAnnotation::vxAnnotation( const vxAnnotation & other ) : vxHandlesObj( other )
{
	CommonInit();

	m_bDrawHandles  = other.m_bDrawHandles;
	m_positionText  = other.m_positionText;
	m_positionLabel = other.m_positionLabel;
	m_color         = other.m_color;
	m_colorSelected = other.m_colorSelected;
	m_colorText     = other.m_colorText;
	m_colorLabel    = other.m_colorLabel;
	m_bLabelSelected= other.m_bLabelSelected;
	m_bTextSelected = other.m_bTextSelected;
	m_bLabelEditable= other.m_bLabelEditable;
	m_sLabel        = other.m_sLabel;
	m_vsTextLines   = other.m_vsTextLines;
	m_eStatus       = other.m_eStatus;
	m_hModule			  = other.m_hModule;
	m_sMouseCursor  = other.m_sMouseCursor;
	m_iSelectedHandleIndex				= other.m_iSelectedHandleIndex;
	m_fLineSpacingFontPercentage  = other.m_fLineSpacingFontPercentage;
	m_pImage = other.m_pImage;
	
}//copy constructor


/**
 * Assignment operator
 */
vxAnnotation & vxAnnotation:: operator = ( const vxAnnotation & other ) 
{
	if ( this != &other )
	{
		vxHandlesObj::operator =( other );
		CommonInit();
		
		m_bDrawHandles  = other.m_bDrawHandles;
		m_positionText  = other.m_positionText;
		m_positionLabel = other.m_positionLabel;
		m_color         = other.m_color;
		m_colorSelected = other.m_colorSelected;
		m_colorText     = other.m_colorText;
		m_colorLabel    = other.m_colorLabel;
		m_bLabelSelected= other.m_bLabelSelected;
		m_bTextSelected = other.m_bTextSelected;
		m_bLabelEditable= other.m_bLabelEditable;
		m_sLabel        = other.m_sLabel;
		m_vsTextLines   = other.m_vsTextLines;
		m_eStatus       = other.m_eStatus;
		m_hModule			  = other.m_hModule;
		m_sMouseCursor  = other.m_sMouseCursor;
		m_iSelectedHandleIndex				= other.m_iSelectedHandleIndex;
		m_fLineSpacingFontPercentage = other.m_fLineSpacingFontPercentage;
		m_pImage = other.m_pImage;

	}
	return *this;

}//Assignment operator

vxAnnotation::~vxAnnotation()
{
	//don't delete it we got from the resource
	//delete m_pImage;
}


/**
 * CommonInit
 */
void vxAnnotation::CommonInit()
//*******************************************************************
{
  m_fLineSpacingFontPercentage = 0.1F;
  m_hModule = NULL;

	SetLabel("Label");
	m_bLabelEditable = true;
  SetLabelSelected( false );
  SetTextSelected( false );
	SetDrawHandles ( true );

	SetColor(PixelRGBA<uint1>(0, 127, 255, 255));
  SetColorSelected(PixelRGBA<uint1>(255, 0, 0, 255));

  SetColorText(PixelRGBA<uint1>(255, 255, 255, 255));
  SetColorLabel(PixelRGBA<uint1>(255, 255, 255, 255));

  m_iSelectedHandleIndex = -1;
  m_eStatus = vxAnnotation::DONE;
} // CommonInit()


/**
 * Reset
 */
void vxAnnotation::Reset()
//*******************************************************************
{
  // call parent class
  vxHandlesObj::Reset();

  // initialize all the local stuff
  CommonInit();
} // Reset()


/**
 * Add another handle
 * @param position of handle
 */
void vxAnnotation::AddHandle(const vxHandle & handle)
//*******************************************************************
{
  int4 iNumHandles(GetNumEditableHandles());

  if (GetStatus() == vxAnnotation::CREATE)
  {
    iNumHandles = GetNumCreationHandles();
  }

  if (GetNumHandles() >= iNumHandles)
  {
    throw ex::AbortOperationException(LogRec("All handles were already added (" + ToAscii(iNumHandles) + " " + ToAscii(GetNumHandles()), "vxHandlesObj", "AddHandle"));
  }

  // call parent class
  vxHandlesObj::AddHandle(handle);

  // make adjustments for label/text if needed
  if (GetNumHandles() == GetLabelHandleIndex()+1)
  {
    SetLabelPosition(handle.GetPosition());
  }
  if (GetNumHandles() == GetTextHandleIndex()+1)
  {
    SetTextPosition(handle.GetPosition());
  }

  return;
} // AddHandle()


/**
 * Get the position of a given handle.
 * @param index of handle
 * @param handle handle
 */
void vxAnnotation::SetHandle(const uint2 uHandleIndex, 
                             const vxHandle & handle)
//*******************************************************************
{
  // call parent class
  vxHandlesObj::SetHandle(uHandleIndex, handle);

  // make adjustments for label/text if needed
  if (uHandleIndex == GetLabelHandleIndex())
  {
    SetLabelPosition(handle.GetPosition());
  }
  if (uHandleIndex == GetTextHandleIndex())
  {
    SetTextPosition(handle.GetPosition());
  }
} // SetHandle()

/**
 * Get the position of a given handle.
 * @param index of handle
 * @param handle handle
 */
void vxAnnotation::Render( vxEnvironment & environment )
//*******************************************************************
{
	if ( m_bDrawHandles )
		vxHandlesObj::Render( environment );
}

/*
 *NULL Cloning
 */
vxAnnotation * vxAnnotation::Clone()const
{
	LogErr( "Null clone Implementation", "vxAnnotation", "Clone" );
	return NULL;
}//Clone()const

/**
 * Set selected handle index
 * @param node
 */
void vxAnnotation::SetSelectedHandleIndex(int iIndex)
//*******************************************************************
{
  static vxHandle handle;

  if (iIndex == -1)
  {
    if (m_iSelectedHandleIndex != -1)
    {
      handle = vxHandlesObj::GetHandle(m_iSelectedHandleIndex);
      handle.SetSelected(false);
      vxHandlesObj::SetHandle(m_iSelectedHandleIndex, handle);
    }
  }
  else
  {
    if (m_iSelectedHandleIndex != -1)
    {
      handle = vxHandlesObj::GetHandle(m_iSelectedHandleIndex);
      handle.SetSelected(false);
      vxHandlesObj::SetHandle(m_iSelectedHandleIndex, handle);
    }
    handle = vxHandlesObj::GetHandle(iIndex);
    handle.SetSelected(true);
    vxHandlesObj::SetHandle(iIndex, handle);
  }

  m_iSelectedHandleIndex = iIndex;

  return;
} // SetSelectedHandleIndex()


/**
 * Morph
 * @param environment
 */
void vxAnnotation::Morph(vxEnvironment & environment)
//*******************************************************************
{
  if (GetNumHandles() != GetNumCreationHandles())
  {
    throw ex::AbortOperationException(LogRec("Morphing only allowed if all creation handles are set", "vxHandlesObj", "Morph"));
  }

  SetStatus(vxAnnotation::DONE);
} // Morph()


/**
 * Write object to XML node
 * @param node
 */
void vxAnnotation::ToXml(vxDOMElement & element) const
//*******************************************************************
{
  // call base class
  vxHandlesObj::ToXml(vxSerializer::AddElement(element, vxHandlesObj::GetElementName(), "# parent class #"));

  // do local scope stuff
  std::string sLabel(GetLabel());
  std::vector<std::string> vsText(GetTextLines());
  Point<float4> labelPosition(GetLabelPosition());
  Point<float4> textPosition(GetTextPosition());
  PixelRGBA<uint1> color(GetColor());
  PixelRGBA<uint1> colorSelected(GetColorSelected());
  PixelRGBA<uint1> colorLabel(GetColorLabel());
  PixelRGBA<uint1> colorText(GetColorText());

  vxSerializer::SetAttribute(element, "version", "2.0");
  vxSerializer::Write(vxSerializer::AddElement(element, vxSerializer::GetElementName(sLabel),        "label"),          sLabel);
  vxSerializer::Write(vxSerializer::AddElement(element, vxSerializer::GetElementName(vsText),        "text lines"),     vsText);
  vxSerializer::Write(vxSerializer::AddElement(element, vxSerializer::GetElementName(labelPosition), "label position"), labelPosition);
  vxSerializer::Write(vxSerializer::AddElement(element, vxSerializer::GetElementName(textPosition),  "text position"),  textPosition);
  vxSerializer::Write(vxSerializer::AddElement(element, vxSerializer::GetElementName(color),         "color"),          color);
  vxSerializer::Write(vxSerializer::AddElement(element, vxSerializer::GetElementName(colorSelected), "color selected"), colorSelected);
  vxSerializer::Write(vxSerializer::AddElement(element, vxSerializer::GetElementName(colorLabel),    "color label"),    colorLabel);
  vxSerializer::Write(vxSerializer::AddElement(element, vxSerializer::GetElementName(colorText),     "color text"),     colorText);
} // ToXml()


/**
 * Initialize from XML node
 * @param viewer type
 * @return successful?
 */
bool vxAnnotation::FromXml(vxDOMElement & element)
//*******************************************************************
{
  if (element.IsNull()) 
    return false;

  bool bSuccess(true);

  // call base class
  bSuccess &= vxHandlesObj::FromXml(vxSerializer::FindElement(element, vxHandlesObj::GetElementName(), "# parent class #"));

  // do local scope stuff
  std::string sVersion("0.0");
  bSuccess &= vxSerializer::GetAttribute(element, "version", sVersion);
  if (atof(sVersion.c_str()) == 2.0)
  {
    std::string sLabel("Label");
    std::vector<std::string> vsText;
    Point<float4> labelPosition(0,0,0);
    Point<float4> textPosition(0,0,0);
    PixelRGBA<uint1> color(0, 127, 255, 255);
    PixelRGBA<uint1> colorSelected(255, 0, 0, 255);
    PixelRGBA<uint1> colorLabel(255, 255, 255, 255);
    PixelRGBA<uint1> colorText(255, 255, 255, 255);

    bSuccess &= vxSerializer::Read(vxSerializer::FindElement(element, vxSerializer::GetElementName(sLabel),        "label"),          sLabel);
    bSuccess &= vxSerializer::Read(vxSerializer::FindElement(element, vxSerializer::GetElementName(vsText),        "text lines"),     vsText);
    bSuccess &= vxSerializer::Read(vxSerializer::FindElement(element, vxSerializer::GetElementName(labelPosition), "label position"), labelPosition);
    bSuccess &= vxSerializer::Read(vxSerializer::FindElement(element, vxSerializer::GetElementName(textPosition),  "text position"),  textPosition);
    bSuccess &= vxSerializer::Read(vxSerializer::FindElement(element, vxSerializer::GetElementName(color),         "color"),          color);
    bSuccess &= vxSerializer::Read(vxSerializer::FindElement(element, vxSerializer::GetElementName(colorSelected), "color selected"), colorSelected);
    bSuccess &= vxSerializer::Read(vxSerializer::FindElement(element, vxSerializer::GetElementName(colorLabel),    "color label"),    colorLabel);
    bSuccess &= vxSerializer::Read(vxSerializer::FindElement(element, vxSerializer::GetElementName(colorText),     "color text"),     colorText);

    SetLabel(sLabel);
    SetText(vsText);
    SetLabelPosition(labelPosition);
    SetTextPosition(textPosition);
    SetColor(color);
    SetColorSelected(colorSelected);
    SetColorLabel(colorLabel);
    SetColorText(colorText);
  }
  else
  {
    LogDbg("Unknown version number for XML", "vxAnnotation", "FromXml()");
  }

  return bSuccess;
} // FromXml()


/**
 * Set the text
 * @param text string
 */
void vxAnnotation::SetText(const string & sText)
//*******************************************************************
{
  m_vsTextLines.clear();
  m_vsTextLines.push_back(sText);
} // SetText()


/**
 * Get the text
 * @return text string
 */
std::string vxAnnotation::GetText() const
//*******************************************************************
{
  string sText;
  std::vector<std::string>::iterator iter;

  for (iter=m_vsTextLines.begin(); iter!=m_vsTextLines.end(); ++iter)
  {
    if ( iter != m_vsTextLines.begin() )
      sText += "; ";
    sText += *iter;
  }

  return sText;
} // GetText()


/**
 * Get label text width
 * @return width in pixel
 */
int4 vxAnnotation::GetTextLinesWidth() const
//*******************************************************************
{
  int iWidth(-1);
  std::vector<std::string>::iterator iter;
  
  for (iter = m_vsTextLines.begin(); iter != m_vsTextLines.end(); ++iter)
  {
    if (GetEnvironment()->GetFont()->GetPixelWidth(*iter) > iWidth)
    {
      iWidth = GetEnvironment()->GetFont()->GetPixelWidth(*iter);
    }
  }

  return iWidth;
} // GetTextLinesWidth()


/**
 * Get label text height
 * @return height in pixel
 */
int4 vxAnnotation::GetTextLinesHeight() const
//*******************************************************************
{
  // total size of all text lines
  int iHeight(GetTextLines().size() * GetEnvironment()->GetFont()->GetPixelHeight());
  
  // add lines spacing
  if (GetTextLines().size() > 1)
  {
    iHeight += (GetTextLines().size()-1) * GetLineSpacingFontPercentage() * GetEnvironment()->GetFont()->GetPixelHeight();
  }

  return iHeight;
} // GetTextLinesHeight()


/**
 * render the annotation text
 * @param environment
 * @param viewport
 * @param model matrix
 * @param projection matrix
 */
void vxAnnotation::RenderText(vxEnvironment & environment,
                              GLint * piViewport,
                              GLdouble * pfModelviewMatrix,
                              GLdouble * pfProjectionMatrix) const
//*******************************************************************
{
  vxViewerType & viewerType(*environment.GetViewerType());

  if (viewerType.IsAnnotationSupported(*this))
  {
    if (viewerType.IsVisible(*this))
    {
      if (GetNumHandles() > 0)
      {
        if (GetLabel().empty() != true)
        {
          glColor3f(GetColorLabel()[0]/255.0, GetColorLabel()[0]/255.0, GetColorLabel()[0]/255.0);
          Point2D<int2> labelLocation;

          if ((viewerType.GetRenderCoordinateSystem() == vxViewerType::eScreenSpace &&
               viewerType.ConvertWorldToScreen(GetLabelPosition(), labelLocation) == true) ||
              (viewerType.GetRenderCoordinateSystem() == vxViewerType::eWorldSpace &&
               viewerType.ConvertWorldToScreen(GetLabelPosition(), labelLocation, piViewport, pfModelviewMatrix, pfProjectionMatrix) == true))
          {
            labelLocation.m_y = labelLocation.m_y - environment.GetFont()->GetPixelHeight();
            environment.GetFont()->PutFormattedString(GetLabel(), GLFontGeorge::ALIGN_LEFT, labelLocation, 
                                                         Triple<float4>(GetColorLabel()[0]/255.0F, 
                                                                        GetColorLabel()[1]/255.0F, 
                                                                        GetColorLabel()[2]/255.0F));
          }
        } // label not empty  
        if (GetText().empty() != true)
        {
          glColor3f(GetColorText()[0]/255.0, GetColorText()[0]/255.0, GetColorText()[0]/255.0);
          Point2D<int2> textLocation;

          if ((viewerType.GetRenderCoordinateSystem() == vxViewerType::eScreenSpace &&
                 viewerType.ConvertWorldToScreen(GetTextPosition(), textLocation) == true) ||
               (viewerType.GetRenderCoordinateSystem() == vxViewerType::eWorldSpace &&
                 viewerType.ConvertWorldToScreen(GetTextPosition(), textLocation, piViewport, pfModelviewMatrix, pfProjectionMatrix) == true))
          {
            if (GetTextPosition() == GetLabelPosition())
            {
              // don't draw text on top of eachother but rather below
              textLocation.m_y -= (1.0F + GetLineSpacingFontPercentage()) * environment.GetFont()->GetPixelHeight();
            }

            string sText;
            std::vector<std::string>::iterator iter;

            for (iter = m_vsTextLines.begin(); iter != m_vsTextLines.end(); ++iter)
            {
              textLocation.m_y -= environment.GetFont()->GetPixelHeight();
              sText = *iter;
              environment.GetFont()->PutFormattedString(sText, GLFontGeorge::ALIGN_LEFT, textLocation, 
                                                           Triple<float4>(GetColorText()[0]/255.0, 
                                                                          GetColorText()[1]/255.0, 
                                                                          GetColorText()[2]/255.0));
              textLocation.m_y -= GetLineSpacingFontPercentage() * environment.GetFont()->GetPixelHeight();
            }
          }
        } // text not empty
      } // more than zero handles
    } // annotation visible
  }  // annotation supported
} // RenderText()


/** 
 * Helper functions to format and set the text
 * @param statistic information
 * @param label for area or volume
 * @param area or volume value
 * @param label for area or volume units
 */
void vxAnnotation::FormatAndSetStatisticText(const vxIntensityStatistic & statistic,
                                             string & sAreaOrVolumeLabel,
                                             float4 fAreaOrVolumeValue,
                                             string & sAreaOrVolumeUnits)
//*******************************************************************
{
  vxVolumeHeader header(GetEnvironment()->GetIntensityVolume()->GetHeader());
  std::string sDataModality;
  header.GetHeaderEntry(vxVolumeHeader::DataModality, sDataModality);
  string sUnit(vxIntensityConverter::GetUnit(header));

  int2   iMaximum(vxIntensityConverter::InitFromShort(statistic.GetMaximum(), header, true));
  float4 fAverage(vxIntensityConverter::InitFromShort(statistic.GetAverage(), header, true));
  float4 fStdDev(statistic.GetStandardDeviation());

  char sText[250];

  std::vector<std::string> vsText;
  sprintf(sText, "%s: %.1f%s", sAreaOrVolumeLabel.c_str(), fAreaOrVolumeValue, sAreaOrVolumeUnits.c_str());
  vsText.push_back(sText);
  sprintf(sText, "Max %s: %d", sUnit.c_str(), iMaximum);
  vsText.push_back(sText);
  sprintf(sText, "Avg %s: %.1f", sUnit.c_str(), fAverage);
  vsText.push_back(sText);
  sprintf(sText, "StdDev %s: %.1f", sUnit.c_str(), fStdDev);
  vsText.push_back(sText);

  SetText(vsText);

  return;
} // FormatAndSetStatisticText()


/** 
 * Helper functions to format and set the text
 * @param statistic information
 * @param label for area or volume
 * @param area or volume value
 * @param label for area or volume units
 */
void vxAnnotation::FormatAndSetStatisticText(int4 iMinimum, int4 iMaximum,
                                             float4 fAverage, float4 fStdDeviation,
                                             string & sAreaOrVolumeLabel,
                                             float4 fAreaOrVolumeValue,
                                             string & sAreaOrVolumeUnits)
//*******************************************************************
{
  vxVolumeHeader header(GetEnvironment()->GetIntensityVolume()->GetHeader());
  std::string sDataModality;
  header.GetHeaderEntry(vxVolumeHeader::DataModality, sDataModality);
  string sUnit(vxIntensityConverter::GetUnit(header));
  const int4 iMinimumModality = static_cast<int4>( vxIntensityConverter::RawToModality( iMinimum, header ) );
  const int4 iMaximumModality = static_cast<int4>( vxIntensityConverter::RawToModality( iMaximum, header ) );
  const float4 fAverageModality = vxIntensityConverter::RawToModality( fAverage, header);
  const float4 fStdDeviationModality = vxIntensityConverter::InitFromShort(fStdDeviation, header, false);
 
  char sText[250];

  std::vector<std::string> vsText;
  sprintf(sText, "%s: %.1f%s", sAreaOrVolumeLabel.c_str(), fAreaOrVolumeValue, sAreaOrVolumeUnits.c_str());
  vsText.push_back(sText);
  sprintf(sText, "Min %s: %d", sUnit.c_str(), iMinimumModality);
  vsText.push_back(sText);
  sprintf(sText, "Max %s: %d", sUnit.c_str(), iMaximumModality);
  vsText.push_back(sText);
  sprintf(sText, "Avg %s: %.1f", sUnit.c_str(), fAverageModality);
  vsText.push_back(sText);
  sprintf(sText, "StdDev %s: %.1f", sUnit.c_str(), fStdDeviationModality);
  vsText.push_back(sText);

  SetText(vsText);

  return;
} // FormatAndSetStatisticText()


/** 
 * Sets the associated mouse cursor
 */
void vxAnnotation::SetCursor()
//*******************************************************************
{
  vxMouseCursor::SetCursor(m_hModule, GetMouseCursor());
} // SetCursor()

//void vxAnnotation::SetImage( const Image<PixelRGBA<uint1> > * pImage ) 
//{
//	m_image = pImage; 
//	int x =10;
//}


/**
 * Gets the bounding box of the selected annotation
 */
Box<float4> vxAnnotation::GetBoundingBox() 
//*******************************************************************
{
	if ( m_vHandles.empty() )
	{
		throw ex::VException(LogRec("Not enough points in the boundary to calculate a bounding box", "vxAnnotation", "GetBoundingRectangle"));
	}

	Point3Df fMaxPoint = m_vHandles.front().GetPosition();
	Point3Df fMinPoint = m_vHandles.front().GetPosition();

	for ( std::vector<vxHandle>::iterator iter = m_vHandles.begin(); iter != m_vHandles.end(); ++iter )
	{
		Point3Df fPoint (( * iter ).GetPosition());

	  fMinPoint.m_x = Min( fPoint.m_x, fMinPoint.m_x );
		fMinPoint.m_y = Min( fPoint.m_y, fMinPoint.m_y );
		fMinPoint.m_z = Min( fPoint.m_z, fMinPoint.m_z );

		fMaxPoint.m_x = Max( fPoint.m_x, fMaxPoint.m_x );
		fMaxPoint.m_y = Max( fPoint.m_y, fMaxPoint.m_y );
		fMaxPoint.m_z	= Max( fPoint.m_z, fMaxPoint.m_z );
	}

	Box<float4> bondBox( fMinPoint.m_x , fMinPoint.m_y , fMinPoint.m_z , fMaxPoint.m_x , fMaxPoint.m_y , fMaxPoint.m_z );

	float4 fDelta = 5.0;

	return bondBox.Inflate( fDelta );

} // GetBoundingRectangle()

#undef FILE_REVISION


// Revision History:
// $Log: vxAnnotation.C,v $
// Revision 1.21  2006/11/23 07:45:43  romy
// fixed linker error
//
// Revision 1.16  2006/11/23 06:29:43  romy
// cloning modified
//
// Revision 1.19  2006/09/27 21:31:17  romy
// put the resource pointer back
//
// Revision 1.17  2006/09/26 21:25:13  romy
// Modified the image object to static
//
// Revision 1.16  2006/09/25 21:31:16  romy
// commoninit modified for proper copying
//
// Revision 1.15  2006/09/25 20:47:06  romy
// added label editable flag for Volume annotations
//
// Revision 1.14  2006/08/10 16:43:45  romy
// code cleaning
//
// Revision 1.13  2006/08/08 13:50:39  romy
// Annotation length and Shadow length are now static consts
//
// Revision 1.12  2006/08/07 18:47:48  romy
// added CCtrs
//
// Revision 1.11  2006/08/03 20:28:09  romy
// added GetBounding Box method
//
// Revision 1.10  2006/08/02 21:28:27  romy
// asjusted the line shadow width
//
// Revision 1.9  2006/08/02 20:45:34  romy
// asjusted the line shadow width
//
// Revision 1.8  2006/08/02 18:44:31  romy
// add renderer method
//
// Revision 1.7  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.6  2005/11/18 21:14:53  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.5.2.1  2005/08/16 18:25:32  geconomos
// Issue# 4356: (George &Frank) Reworked computation of volume annotation.
//
// Revision 1.5  2005/04/12 15:23:32  geconomos
// changed xml library to the vxDOM classes
//
// Revision 1.4  2005/03/08 19:09:13  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.2.6.2  2005/02/17 21:50:24  frank
// Issue #3973: fixed text formatting
//
// Revision 1.2.6.1  2005/02/09 14:29:24  michael
// Merged from "trunc" to branch since I didn't have the trunc. Will do better next time.
//
// Revision 1.3  2005/02/09 14:21:21  michael
// In case text and label position are identical, draw them not on top of each other
// but label in first line and text below.
//
// Revision 1.2  2004/04/06 16:24:17  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.30  2004/01/23 03:51:31  michael
// reworked annotation to have a vxHandle class and a vxHandlesObj contains
// a vector of handles and access functions. The vxAnnotation derives from this
// one and overloads needed SetHandle() calls to make needed adjustments.
//
// Revision 1.29  2004/01/14 14:18:16  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.28  2004/01/05 18:58:29  michael
// added "mailto:"
//
// Revision 1.27  2003/06/04 18:33:54  michael
// accounting for new vxIntensityConverter parameters (header, not modality)
//
// Revision 1.26  2003/05/16 13:17:39  frank
// formatting
//
// Revision 1.25  2003/04/25 19:15:32  dongqing
// code review
//
// Revision 1.24  2003/04/24 13:46:29  michael
// initial code reviews
//
// Revision 1.23  2003/04/08 19:52:41  geconomos
// Updated for resource management.
//
// Revision 1.22  2003/04/01 17:53:05  michael
// cosmetics
//
// Revision 1.21  2003/04/01 00:35:59  michael
// fixed issue #2862
//
// Revision 1.20  2003/03/18 21:23:01  frank
// Distinguished between supporting specific annotations and supporting specific manipulators for viewer types. IsSupported() has become IsAnnotationSupported().
//
// Revision 1.19  2003/03/04 01:43:05  michael
// renaming of methods removing the double occurence of "Annotation", e.g.
// vxAnnotations::GetActiveAnnotation() --> vxAnnotations::GetActive()
//
// Revision 1.18  2003/02/28 05:44:26  michael
// fixed issue #2860
//
// Revision 1.17  2003/02/26 19:26:58  geconomos
// Implementation of associated image.
//
// Revision 1.16  2003/02/22 00:22:56  michael
// fixed annotations to serialize and deserialize
//
// Revision 1.15  2003/02/18 15:44:19  michael
// removed redundant member m_uCmponentLabel
//
// Revision 1.14  2003/02/13 16:56:46  jmeade
// Merge-to-Main_BeforeBranch_VC_1-3_to_UpdateAfterBranch_VC_1-3.
//
// Revision 1.13  2003/02/07 00:29:59  michael
// added annotation for components
//
// Revision 1.12.2.1  2003/02/12 22:49:48  jmeade
// Merged changes between main-branch tags "BeforeBranch_VC_1-3" and "UpdateAfterBranch_VC_1-3" that should've been in VC_1-3 branch.
//
// Revision 1.12  2003/01/30 00:36:29  michael
// using rayiterator and samples rather than voxels (in vxAnnotationBox)
//
// Revision 1.11  2003/01/28 17:02:05  michael
// fixed statistics of box annotation
//
// Revision 1.10  2003/01/27 23:18:49  michael
// added annotation stuff: flexible text location (handle index), multiple text lines etc.
//
// Revision 1.9  2003/01/20 04:52:13  michael
// added first version of box annotation. Not yet completed ...
//
// Revision 1.8  2003/01/17 23:22:58  michael
// fixed basic annotation, minor issue in angle still to be done
//
// Revision 1.7  2003/01/17 00:33:22  michael
// further iteration on new annotations. Text now shows up and there is a utility
// in the viewerType that allows to pass in the OpenGL parameters in case the
// current one should not be used, e.g. as in render text of annotations. Also extended
// the glFontGeorge class to return width and height of the given string
//
// Revision 1.6  2003/01/15 16:30:55  michael
// added defines for FILE_VERSION
//
// Revision 1.5  2003/01/14 23:36:27  michael
// furter adjustments to support point, arrow, distance and angle
//
// Revision 1.4  2003/01/13 23:20:33  michael
// made first annotation to work in new structure (vxAnnotations)
//
// Revision 1.3  2003/01/13 14:42:30  geconomos
// Did the following:
//   (1) Added mouse cursor handling.
//   (2) Set the active annotation in AddAnnotation().
//
// Revision 1.2  2003/01/09 14:55:44  michael
// some add-ons to the new annotation structure
//
// Revision 1.1  2003/01/06 17:12:18  michael
// Added initial version of annotations
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxAnnotation.C,v 1.21 2006/11/23 07:45:43 romy Exp $
// $Id: vxAnnotation.C,v 1.21 2006/11/23 07:45:43 romy Exp $
