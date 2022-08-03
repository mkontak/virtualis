// $Id: vxColorTableCPU.C,v 1.3 2006/03/02 15:49:14 frank Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Wei Li (liwei@viatronix.com)


// includes
#include "stdafx.h"
#include "vxColorTableCPU.h"
#include "utilities.h"
#include <math.h>


// namespaces
using namespace nsCOLOR;


// defines
#define FILE_REVISION "$Revision: 1.3 $"


/**
 * Constructor
 * @param number of entries in table
 */
vxColorTableCPU::vxColorTableCPU(uint2 uMaxIndex)
{
  int FixThisClassToWriteFloatingPointValuesInAUsefulRange;
	m_uNumEntries    = uMaxIndex+1; 
  m_uMaxColorValue = 255;
	m_uMaxColorIndex = uMaxIndex;
	m_uMaxAlphaIndex = uMaxIndex;
	m_uMaxAlphaValue = uMaxIndex;

	m_pfTable = new float4[m_uNumEntries*4];
} // constructor


/**
 * Copy constructor
 * @param colortable
 */
vxColorTableCPU::vxColorTableCPU(const vxColorTableCPU & other)
{
  if (this != &other)
  {
    m_uNumEntries = other.m_uNumEntries;
    m_pfTable     = new float4[m_uNumEntries*4];

    for (uint4 i(0); i<m_uNumEntries; ++i)
    {
      m_pfTable[i] = other.m_pfTable[i];
    }
  }
} // copy constructor


/**
 * assignment operator
 * @param colortable
 * @return const reference to this
 */
const vxColorTableCPU & vxColorTableCPU::operator=(const vxColorTableCPU & other)
{
  vxColorTable::operator = (other);

  if (this != &other)
  {
    m_uNumEntries = other.m_uNumEntries;
    m_pfTable     = new float4[m_uNumEntries*4];

    memcpy(m_pfTable, other.m_pfTable, 4 * m_uNumEntries * sizeof(float4));
  }

  return *this;
} // operator=()


/**
 * Destructor
 */
vxColorTableCPU::~vxColorTableCPU()
{
	delete[] m_pfTable;
}


/**
 * Return the value of the specifice channel at position index
 * @param color channel
 * @param entry index
 * @return value
 */
const uint2 vxColorTableCPU::GetColor(const COLOR_CHANNEL color, 
                                      const uint2 uIndex)
{
  if (uIndex < m_uNumEntries)
  {
	  switch(color)
    {
	  case RED:   return GetRed(  uIndex); break;
	  case GREEN: return GetGreen(uIndex); break;
	  case BLUE:  return GetBlue( uIndex); break;
	  case ALPHA: return GetAlpha(uIndex); break;
    }

    return 0;
	}
  else
  {
    throw ex::IndexOutOfBoundsException(LogRec("index out of bounds", "vxColorTableCPU", "GetColor()"));
  }
} // GetColor()


/**
 * Set the value of the specifice channel at position index
 * @param color color channel
 * @param uIndex entry index
 * @param value
 */
void vxColorTableCPU::SetColor(const COLOR_CHANNEL color, 
                               const uint2 uIndex, 
                               const uint2 uValue)
{
  if (uIndex < m_uNumEntries)
  {
	  switch(color)
    {
	  case RED:   SetRed(  uIndex, uValue); break;
	  case GREEN: SetGreen(uIndex, uValue); break;
	  case BLUE:  SetBlue( uIndex, uValue); break;
	  case ALPHA: SetAlpha(uIndex, uValue); break;
	  }
    return;
  }
  else
  {
    throw ex::IndexOutOfBoundsException(LogRec("index out of bounds", "vxColorTableCPU", "SetColor()"));
  }
} // SetColor()


/**
 * Get a value from red channel at index
 * @param uIndex entry index
 * @return value
 */
const uint2 vxColorTableCPU::GetRed(const uint2 uIndex) const
{
  if (uIndex < m_uNumEntries)
  {
    return static_cast<uint2>(m_pfTable[uIndex * 4 + RED]);
  }
  else
  {
    throw ex::IndexOutOfBoundsException(LogRec("index out of bounds", "vxColorTableCPU", "GetRed()"));
  }
} // GetRed()


/**
 * Set a value to alpha channel at index
 * @param uIndex entry index
 * @param value
 */
void vxColorTableCPU::SetRed(const uint2 uIndex, 
                             const uint2 uValue)
{
  if (uIndex < m_uNumEntries)
  {
    m_pfTable[uIndex * 4 + RED] = uValue;

  }
  else
  {
    throw ex::IndexOutOfBoundsException(LogRec("index out of bounds", "vxColorTableCPU", "SetRed()"));
  }
}


/**
 * Get a value from green channel at index
 * @param uIndex entry index
 * @return value
 */
const uint2 vxColorTableCPU::GetGreen(const uint2 uIndex) const
{
  if (uIndex < m_uNumEntries)
  {
    return static_cast<uint2>(m_pfTable[uIndex * 4 + GREEN]);
  }
  else
  {
    throw ex::IndexOutOfBoundsException(LogRec("index out of bounds", "vxColorTableCPU", "GetGreen()"));
  }
} // GetGreen()


/**
 * Set a value to green channel at index
 * @param uIndex entry index
 * @param value
 */
void vxColorTableCPU::SetGreen(const uint2 uIndex, 
                               const uint2 uValue)
{
  if (uIndex < m_uNumEntries)
  {
    m_pfTable[uIndex * 4 + GREEN] = uValue;
  }
  else
  {
    throw ex::IndexOutOfBoundsException(LogRec("index out of bounds", "vxColorTableCPU", "SetGreen()"));
  }
} // SetGreen()


/**
 * Get a value from blue channel at index
 * @param uIndex entry index
 * @return value
 */
const uint2 vxColorTableCPU::GetBlue(const uint2 uIndex) const
{
  if (uIndex < m_uNumEntries)
  {
    return static_cast<uint2>(m_pfTable[uIndex * 4 + BLUE]); 
  }
  else
  {
    throw ex::IndexOutOfBoundsException(LogRec("index out of bounds", "vxColorTableCPU", "GetBlue()"));
  }
} // GetBlue()


/**
 * Set a value to blue channel at index
 * @param uIndex entry index
 * @param value
 */
void vxColorTableCPU::SetBlue(const uint2 uIndex, 
                              const uint2 uValue)
{
  if (uIndex < m_uNumEntries)
  {
    m_pfTable[uIndex * 4 + BLUE] = uValue;
  }
  else
  {
    throw ex::IndexOutOfBoundsException(LogRec("index out of bounds", "vxColorTableCPU", "SetBlue()"));
  }
} // SetBlue()


/**
 * Get a value from alpha channel at index
 * @param uIndex entry index
 * @return value
 */
const uint2 vxColorTableCPU::GetAlpha(const uint2 uIndex) const
{
  if (uIndex < m_uNumEntries)
  {
    return static_cast<uint2>(m_pfTable[uIndex * 4 + ALPHA]);
  }
  else
  {
    throw ex::IndexOutOfBoundsException(LogRec("index out of bounds", "vxColorTableCPU", "GetAlpha()"));
  }
} // GetAlpha()


/**
 * Set a value to alpha channel at index
 * @param uIndex entry index
 * @param value
 */
void vxColorTableCPU::SetAlpha(const uint2 uIndex, 
                               const uint2 uValue)
{
  if (uIndex < m_uNumEntries)
  {
    m_pfTable[uIndex * 4 + ALPHA] = uValue;
  }
  else
  {
    throw ex::IndexOutOfBoundsException(LogRec("index out of bounds", "vxColorTableCPU", "SetAlpha()"));
  }
} // SetAlpha()


// undefines
#undef FILE_REVISION


// $Log: vxColorTableCPU.C,v $
// Revision 1.3  2006/03/02 15:49:14  frank
// spelling
//
// Revision 1.2  2005/11/18 21:14:53  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.1.1.1.12.1  2005/08/01 18:51:03  frank
// added warning
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.10  2003/06/09 12:29:54  michael
// Implemented a true colormap object that returns discretized colortables based
// on either volume header (taking modality and bits stored) or on the maximum
// index needed -> size = max index + 1
//
// Revision 1.9  2003/05/16 11:45:01  michael
// code review
//
// Revision 1.8  2003/04/19 01:49:08  michael
// some more code review
//
// Revision 1.7  2003/04/17 14:19:15  michael
// fixed reoccuring debug message (log file)
//
// Revision 1.6  2003/04/16 12:47:27  michael
// performed initial code review
//
// Revision 1.5  2003/03/30 20:58:31  michael
// formatting and cosmetics
//
// Revision 1.4  2002/10/02 22:49:24  frank
// Removed promotion of namespaces in header files!
//
// Revision 1.3  2002/09/25 20:36:41  michael
// Completed vxTransferFunction to vxColormap transition
//
// Revision 1.1  2002/07/30 20:21:05  dmitry
// Moved from 74_vxUtils.
//
// Revision 3.3  2002/07/23 22:45:03  ingmar
// code beautify
//
// Revision 3.2  2002/07/18 15:18:53  ingmar
// removed obsolete Initialize()
// added destructor to clean up memory leak
//
// Revision 3.1  2002/05/03 20:31:22  soeren
// Range something  cleaned out.
//
// Revision 3.0  2001/10/14 23:02:30  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:48  ingmar
// moved from pvcs
//
// 
//    Rev 2.2   Sep 27 2001 14:35:02   liwei
// Added bool flag for gradient opacity modulation.Bugs cause RenderVolumePro to render incorrectly fixed.
// 
//    Rev 2.1   Sep 17 2001 19:39:00   geconomos
// Made MaxAlphaValue back to 255 since uNumEntries-1 gave an image that was too dull
// 
//    Rev 2.0   Sep 16 2001 23:49:46   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:37:24   ingmar
// Initial revision.
// Revision 1.5  2001/08/23 21:40:21  liwei
// {Set, Get}{Red, Green, Blue, Alpha}() are changed to inline functions while
// {Set, Get}Color() changed to virtual functions.
//
// Revision 1.4  2001/08/09 14:41:04  liwei
// The max value of color is always 255.
//
// Revision 1.3  2001/08/03 23:42:51  liwei
// TransferFunction now contains several lookup tables as its data members.
//
// Revision 1.2  2001/08/03 22:58:05  liwei
// Directly store the table as float*, not using PixelRGBA any more.
//
// Revision 1.1  2001/08/03 22:32:56  liwei
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxColorTableCPU.C,v 1.3 2006/03/02 15:49:14 frank Exp $
// $Id: vxColorTableCPU.C,v 1.3 2006/03/02 15:49:14 frank Exp $
