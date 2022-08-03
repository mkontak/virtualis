// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Greg Davidson  gdavidson@viatronix.net

#include "StdAfx.h"
#include "XmlUtils.h"
#include <memory>
#include "vxDOMDocument.h"
#include "vxDOMNodeList.h"
#include "vxDOMNamedNodeMap.h"



USING_UTILITIES_NS


/**
 * Is the xml empty (no children or attributes)
 */
bool Xml::IsEmpty( const std::string & xml )
{
  if( xml.empty() )
    return true;

  vxDOMDocument doc( vxDOMDocument::FromXml( xml ));
  return !doc.GetDocumentElement().HasChildNodes() && doc.GetDocumentElement().GetAttributes().GetLength() == 0;
} // IsEmpty( xml )