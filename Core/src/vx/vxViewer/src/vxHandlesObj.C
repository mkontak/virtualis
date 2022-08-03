// $Id: vxHandlesObj.C,v 1.4 2006/08/07 18:47:48 romy Exp $
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
#include "vxHandlesObj.h"
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
#define FILE_REVISION "$Revision: 1.4 $"


/**
 * Default constructor
 */
vxHandlesObj::vxHandlesObj()
//*******************************************************************
{
  m_vHandles.reserve(0);
} // constructor


/**
 * Destructor
 */
vxHandlesObj::~vxHandlesObj()
//*******************************************************************
{
  m_vHandles.clear();
} // destructor

/**
 * copy constructor
 */
vxHandlesObj::vxHandlesObj( const vxHandlesObj & other ) 
{
  m_vHandles.reserve(0);

	m_vHandles     = other.m_vHandles;
	m_pEnvironment = other.m_pEnvironment;

}// copy constructor


/**
 * Assignment operator
 */
vxHandlesObj& vxHandlesObj::operator =( const vxHandlesObj & other )
{
	if ( this != & other )
	{
		m_vHandles     = other.m_vHandles;
		m_pEnvironment = other.m_pEnvironment;
	}
	return *this;
}//Assignment operator


/**
 * Reset
 */
void vxHandlesObj::Reset()
//*******************************************************************
{
  m_vHandles.clear();
} // Reset()


/**
 * Reset
 */
uint4 vxHandlesObj::GetNumHandles() const
//*******************************************************************
{
  return m_vHandles.size();
} // GetNumHandles()


/**
 * Add handle
 * @param hanlde handle to be added
 */
void vxHandlesObj::AddHandle(const vxHandle & handle)
//*******************************************************************
{
  m_vHandles.push_back(handle);
} // AddHandle()


/**
 * Remove a handle
 * @param position of handle
 */
void vxHandlesObj::RemoveHandle(const uint2 uIndex)
//*******************************************************************
{
  if (m_vHandles.size() >= uIndex)
  {
    throw ex::AbortOperationException(LogRec("Handle is not existing (" + ToAscii(uIndex) + " out of " + ToAscii(m_vHandles.size()), "vxHandlesObj", "RemoveHandle"));
  }

  std::vector<vxHandle>::iterator (m_vHandles.begin());

  m_vHandles.erase(m_vHandles.begin() + uIndex);
} // AddHandle()


/**
 * Get the position of a given handle
 * @param uHandleIndex index of handle
 * @return handle
 */
vxHandle vxHandlesObj::GetHandle(const uint2 uHandleIndex) const
//*******************************************************************
{
  if (uHandleIndex < 0 || uHandleIndex >= m_vHandles.size())
  {
    throw ex::IndexOutOfBoundsException(LogRec("invalid index", "vxHandlesObj", "GetHandle"));
  }
  else
  {
    return m_vHandles[uHandleIndex];
  }
} // GetHandle()


/**
 * Get the position of a given handle.
 * @param uHandleIndex index of handle
 * @param handle handle
 */
void vxHandlesObj::SetHandle(const uint2 uHandleIndex, 
                             const vxHandle & handle)
//*******************************************************************
{
  if (uHandleIndex < 0 || uHandleIndex >= m_vHandles.size())
  {
    throw ex::IndexOutOfBoundsException(LogRec("invalid index", "vxHandlesObj", "SetHandle"));
  }

  m_vHandles[uHandleIndex] = handle;
} // SetHandle()


/**
 * Determine parameters needed to be computed once
 * @param environment
 */
void vxHandlesObj::PreRender(vxEnvironment & environment)
//*******************************************************************
{
  SetEnvironment(&environment);

  for (int i(0); i<GetNumHandles(); i++)
  {
    m_vHandles[i].PreRender(environment);
  }
} // PreRender()


/**
 * Update the annotation text
 * @param environment
 */
void vxHandlesObj::Render(vxEnvironment & environment)
//*******************************************************************
{
  SetEnvironment(&environment);

  for (int i(0); i<GetNumHandles(); i++)
  {
    GetHandle(i).Render(environment);
  }

  return;
} // Render()


/**
 * Write object to XML node
 * @param node
 */
void vxHandlesObj::ToXml( vxDOMElement & element ) const
//*******************************************************************
{
  // call base class
  //vxHandlesObj::ToXml(vxSerializer::AddElement(element, vxHandlesObj::GetElementName(), "# parent class #"));

  // do local scope stuff
  vxSerializer::SetAttribute(element, "version", "2.0");
  vxSerializer::Write(vxSerializer::AddElement(element, vxSerializer::GetElementName(m_vHandles), "handles"), m_vHandles);

  return;
} // ToXml()


/**
 * Initialize from XML node
 * @param viewer type
 * @return successful?
 */
bool vxHandlesObj::FromXml( vxDOMElement & element )
//*******************************************************************
{
  if (element.IsNull() ) 
    return false;

  bool bSuccess(true);

  // call base class
  //bSuccess &= vxHandlesObj::FromXml(vxSerializer::FindElement(element, vxHandlesObj::GetElementName(), "# parent class #"));

  // do local scope stuff
  std::string sVersion("0.0");
  bSuccess &= vxSerializer::GetAttribute(element, "version", sVersion);

  if (atof(sVersion.c_str()) == 2.0)
  {
    std::vector<vxHandle> vHandles;

    bSuccess &= vxSerializer::Read(vxSerializer::FindElement(element, vxSerializer::GetElementName(vHandles), "handles"), vHandles);

    Reset();
    for (int i(0); i< vHandles.size(); i++)
    {
      AddHandle(vHandles[i]);
    }
  }
  else
  {
    LogDbg("Unknown version number for XML", "vxHandlesObj", "FromXml()");
  }

  return bSuccess;
} // FromXml()


#undef FILE_REVISION


// Revision History:
// $Log: vxHandlesObj.C,v $
// Revision 1.4  2006/08/07 18:47:48  romy
// added CCtrs
//
// Revision 1.3  2005/04/12 15:23:32  geconomos
// changed xml library to the vxDOM classes
//
// Revision 1.2  2004/03/11 17:39:22  frank
// named the parameters consistently
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.28  2004/01/23 03:51:32  michael
// reworked annotation to have a vxHandle class and a vxHandlesObj contains
// a vector of handles and access functions. The vxAnnotation derives from this
// one and overloads needed SetHandle() calls to make needed adjustments.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxHandlesObj.C,v 1.4 2006/08/07 18:47:48 romy Exp $
// $Id: vxHandlesObj.C,v 1.4 2006/08/07 18:47:48 romy Exp $
