// $Id: vxColormap.C,v 1.4.8.1 2010/08/05 14:17:18 dongqing Exp $
//
// Copyright © 2002 Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Michael Meissner (mailto:michael@viatronix.com)


// includes
#include "stdafx.h"
#include "vxColormap.h"
#include "Point2D.h"
#include "ZeroOne.h"
#include "vxWindowLevel.h"
#include "vxControlPoint.h"
#include "vxIntensityConverter.h"
#include "vxVolumeHeader.h"
#include "vxColorTableCPU.h"

#include "vxDOMDocument.h"
#include "vxDOMElement.h"
#include "vxSerializer.h"

// namespaces
using namespace nsCOLOR;
using namespace std;


// defines
#define FILE_REVISION "$Revision: 1.4.8.1 $"


/**
 * Default constructor
 */
vxColormap::vxColormap(): 
m_sName("undefined")
//*******************************************************************
{
  Point2D<ZeroOne> point0(0.0F, 1.0F);
  Point2D<ZeroOne> point1(1.0F, 1.0F);
  
  // pure white
  m_vRed.clear  (); m_vRed  .push_back(point0); m_vRed  .push_back(point1);
  m_vGreen.clear(); m_vGreen.push_back(point0); m_vGreen.push_back(point1);
  m_vBlue.clear (); m_vBlue .push_back(point0); m_vBlue .push_back(point1);

  // alpha is a ramp
  point0.m_y = 0.0F;
  m_vAlpha.clear(); m_vAlpha.push_back(point0); m_vAlpha.push_back(point1);
} // vxColormap()


/**
 * Copy constructor
 * @param colormap
 */
vxColormap::vxColormap(const vxColormap & other)
//*******************************************************************
{
  * this = other;
} // copy constructor()


/**
 * Destructor
 */
vxColormap::~vxColormap()
//*******************************************************************
{
  list<pair<int, pair<bool, vxColorTableCPU *> > >::iterator iter(m_lColorTables.begin());

  while (iter != m_lColorTables.end())
  {
    if ( (*iter).second.second != NULL )
    {
      delete (*iter).second.second;
      (*iter).second.second = NULL;
    }
    ++iter;
  }

} // destructor()


/**
 * Assignment operator
 * @param colormap
 * @return const reference to this
 */
const vxColormap & vxColormap::operator=(const vxColormap & other)
//*******************************************************************
{
  if (this != &other)
  {
    m_sName           = other.m_sName;
    m_windowLevel     = other.m_windowLevel;
    m_vRed.clear();   m_vRed   = other.m_vRed;
    m_vGreen.clear(); m_vGreen = other.m_vGreen;
    m_vBlue.clear (); m_vBlue  = other.m_vBlue;
    m_vAlpha.clear(); m_vAlpha = other.m_vAlpha;

    list<pair<int, pair<bool, vxColorTableCPU *> > >::iterator iter(m_lColorTables.begin());

    while (iter != m_lColorTables.end())
    {
      if ( (*iter).second.second != NULL )
      {
        delete (*iter).second.second;
        (*iter).second.second = NULL;
      }
      ++iter;
    } // while
    m_lColorTables.clear(); // no discrete tables available

  } // if

  return * this;
} // operator=()


/**
 * Sets all current discrete color tables dirty
 */
void vxColormap::SetDirty()
//*******************************************************************
{
  std::list<std::pair<int, std::pair<bool, vxColorTableCPU *> > >::iterator iter(m_lColorTables.begin());

  while (iter != m_lColorTables.end())
  {
    (*iter).second.first = true;
    (*iter).second.second->SetActiveRange(IsActiveRange()); // in case it changed
    ++iter;
  }
} // SetDirty()


/**
 * Get discrete table pointer RGBA
 * @param header volume header
 * @return pointer to a float array
 */
float4 * vxColormap::GetTablePointerRGBA(const vxVolumeHeader & header)
//*******************************************************************
{
  int4 iMaxIndex(vxIntensityConverter::GetMaxValue(header, false));

  return GetTablePointerRGBA(iMaxIndex);
} // GetTablePointerRGBA()


/**
* Get discrete table pointer RGBA
* @param iMaxIndex maximum index to be used
* @return pointer to a float array
*/
float4 * vxColormap::GetTablePointerRGBA(const uint2 & iMaxIndex)
//*******************************************************************
{
  list<pair<int, pair<bool, vxColorTableCPU *> > >::iterator iter(m_lColorTables.begin());

  while (iter != m_lColorTables.end())
  {
    if ((*iter).first == iMaxIndex)
    {
      pair<bool, vxColorTableCPU *> element((*iter).second);
      if (element.first == true)
      {
        element.second->UpdateLookupTable(*this);
        element.first = false;
        (*iter).second = element;
      }
      return element.second->GetTablePointerRGBA();
    }
    ++iter;
  }

  vxColorTableCPU * pColorTable = new vxColorTableCPU(iMaxIndex);
  pColorTable->SetActiveRange(IsActiveRange());
  pColorTable->UpdateLookupTable(*this);
  pair<int, pair<bool, vxColorTableCPU *> > element;
  element.first = iMaxIndex;
  element.second.first = false;
  element.second.second = pColorTable;
  m_lColorTables.push_back(element);

  return pColorTable->GetTablePointerRGBA();
} // GetTablePointerRGBA()


/**
 * Get a color curve
 * @param color channel
 * @return vector of points
 */
const std::vector<Point2D<ZeroOne> > & vxColormap::GetList(COLOR_CHANNEL color) const
//*******************************************************************
{
  switch (color)
  {
    case RED:   return GetRedList(); 
    case GREEN: return GetGreenList();
    case BLUE:  return GetBlueList();
    case ALPHA: return GetAlphaList();
    default:
      throw ex::IllegalArgumentException(LogRec("the specified color channel is not supported", "vxColormap", "GetList"));
  }
} // GetList()

/**
 * Get a colormap
 * @param colormap file
 * @return vxColormap object
 */
vxColormap * vxColormap::FromFile( const std::string & file ) 
{
 	vxDOMDocument document = vxDOMDocument::FromFile( file );
  return FromElement( document.GetDocumentElement() );
} // FromFile()


/**
 * Get a colormap
 * @param colormap xml
 * @return vxColormap object
 */
vxColormap * vxColormap::FromXml( const std::string & xml ) 
{
  vxDOMDocument document = vxDOMDocument::FromXml( xml );
  return FromElement( document.GetDocumentElement() );
} // FromFile()


/**
 * Get a colormap
 * @param colormap element
 * @return vxColormap object
 */
vxColormap * vxColormap::FromElement( vxDOMElement & element ) 
{
  try
  {
    // element.
		std::string sVersion("1.0");
    vxSerializer::GetAttribute(element, "version", sVersion);

  	// create new colormap object
    vxColormap * pColormap = new vxColormap();

		if ( sVersion == "2.0" )
    {
      // read colormap
			vxSerializer::Read( element , *pColormap );
    }
    return pColormap;
  } // try
  catch(ex::VException & e)
  {
    throw &e;
  }
} // FromFile()

#undef FILE_REVISION


// Revision History:
// $Log: vxColormap.C,v $
// Revision 1.4.8.1  2010/08/05 14:17:18  dongqing
// fix the memory leak in vxColormap = operator. and roll back the previous
// fix in vxPlanetracerSetting.C/h
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.4  2006/08/23 19:29:12  romy
// added Colormap read for Colon Application
//
// Revision 1.3  2006/06/28 01:36:29  geconomos
// proper vxColorTable deletion in destructor
//
// Revision 1.2  2006/06/23 19:19:38  frank
// somebody left a dangling pointer after deleting the object
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.18  2003/11/20 05:27:08  michael
// took out logging message when colormap is set dirty
//
// Revision 1.17  2003/11/18 14:05:39  michael
// fixed the dirty flag that never worked before and caused recomputation of the
// colormaps in every frame. :-(
//
// Revision 1.16  2003/11/18 02:13:39  michael
// miracle bug ... not fixed yet :-(
//
// Revision 1.15  2003/08/05 13:37:23  frank
// formatting
//
// Revision 1.14  2003/06/09 12:29:54  michael
// Implemented a true colormap object that returns discretized colortables based
// on either volume header (taking modality and bits stored) or on the maximum
// index needed -> size = max index + 1
//
// Revision 1.13  2003/06/07 16:18:33  michael
// added additional functionality to support on demand lookup tables
//
// Revision 1.12  2003/04/25 19:16:09  dongqing
// code review
//
// Revision 1.11  2003/04/16 12:47:27  michael
// performed initial code review
//
// Revision 1.10  2003/03/30 20:58:32  michael
// formatting and cosmetics
//
// Revision 1.9  2003/02/19 18:22:59  michael
// changes to account for serialization of annotations
//
// Revision 1.8  2002/10/14 15:30:49  michael
// maded Get-methods const and added Set-methods for RGBA vectors
//
// Revision 1.7  2002/10/02 22:49:24  frank
// Removed promotion of namespaces in header files!
//
// Revision 1.6  2002/10/01 13:03:24  michael
// cosmetics
//
// Revision 1.5  2002/09/26 23:38:07  dmitry
// Assignment operator was missing  a set on the window level.
//
// Revision 1.4  2002/09/25 20:36:41  michael
// Completed vxTransferFunction to vxColormap transition
//
// Revision 1.3  2002/09/24 16:58:04  michael
// cleaned-up colortable and colormap ... hopefully we'll be able to remove
// member colortable from colormap.
//
// Revision 1.2  2002/09/23 14:53:50  michael
// added name
//
// Revision 1.1  2002/09/19 15:12:56  michael
// Added vxColormap and vxColormapEditorSharedData
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxColormap.C,v 1.4.8.1 2010/08/05 14:17:18 dongqing Exp $
// $Id: vxColormap.C,v 1.4.8.1 2010/08/05 14:17:18 dongqing Exp $
