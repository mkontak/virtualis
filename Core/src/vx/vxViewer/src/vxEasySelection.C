// $Id: vxEasySelection.C,v 1.2 2006/12/14 22:02:05 romy Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Frank Dachille( frank@viatronix.com )

/**
 * Class to maintain the data associated with an easy selected component
 */

// includes
#include "StdAfx.h"
#include "vxEasySelection.h"
#include "vxSerializer.h"


// Defines
#define FILE_REVISION "$Revision: 1.2 $"


// namespaces
using namespace Serialization;
using namespace std;


/**
 * Constructor
 */
vxEasySelection::vxEasySelection()
{
} // vxEasySelection()


/**
 * Destructor
 */
vxEasySelection::~vxEasySelection()
{
} // ~vxEasySelection()

/*
 * Clones an instance
 */
vxEasySelection * vxEasySelection::Clone() const
{
  return new vxEasySelection( *this );
}///Clone


/**
 * generate Xml node
 *
 * @param element the xml node
 */
void vxEasySelection::ToXml( vxDOMElement & element ) const
{

  uint2 uLabel = GetLabel();
  bool bVesselness = m_bVesselness;

  vxSerializer::SetAttribute(element, "version", "1.0");
  vxSerializer::Write(vxSerializer::AddElement(element, vxSerializer::GetElementName(uLabel), "label"), uLabel);
  vxSerializer::Write(vxSerializer::AddElement(element, vxSerializer::GetElementName(bVesselness), "vesselness"), bVesselness);

} // ToXml()


/**
 * initialize from Xml node
 *
 * @param element the xml node
 *
 * @return success
 */
bool vxEasySelection::FromXml( vxDOMElement & element )
{

  bool bSuccess(false);

  if( element.IsNull() ) return bSuccess;

  string sVersion("0.0");
  vxSerializer::GetAttribute(element, "version", sVersion);

  if (sVersion == "1.0")
  {

    std::string name("");
    uint2 uLabel(0);
    bool bVesselness(false);

    bSuccess = true;
    bSuccess &= vxSerializer::Read(vxSerializer::FindElement(element, vxSerializer::GetElementName(uLabel), "label"), uLabel);
    bSuccess &= vxSerializer::Read(vxSerializer::FindElement(element, vxSerializer::GetElementName(bVesselness), "vesselness"), bVesselness);

    if ( bSuccess )
    {
      SetLabel(uLabel);
      SetVesselness(bVesselness);
    }

  } // if version 1.0
  else
  {
    LogErr("Could not read format of vxEasySelection", "vxSerializer", "Read");
  }

  return bSuccess;

} // FromXml()


// undefines
#undef FILE_REVISION


// $Log: vxEasySelection.C,v $
// Revision 1.2  2006/12/14 22:02:05  romy
// added Clone interface to Component Data
//
// Revision 1.1  2006/09/25 13:17:05  geconomos
// moved from vascular
//
// Revision 1.2  2005/06/08 14:58:16  vxconfig
// formatting
//
// Revision 1.1  2005/06/07 20:05:47  vxconfig
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxEasySelection.C,v 1.2 2006/12/14 22:02:05 romy Exp $
// $Id: vxEasySelection.C,v 1.2 2006/12/14 22:02:05 romy Exp $
