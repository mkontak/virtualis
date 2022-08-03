// $Id: vxComponentData.C,v 1.1 2004/07/12 18:02:40 frank Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Frank Dachille (frank@viatronix.com)


// includes
#include "StdAfx.h"
#include "vxComponentData.h"
#include "vxSerializer.h"


// defines
#define FILE_REVISION "$Revision: 1.1 $"


///**
// * generate Xml node
// *
// * @param spElement the xml node
// */
//void vxComponentData::ToXml( MSXML2::IXMLDOMElementPtr & spElement ) const
//{
//
//  vxComponentData sLabel(  );
//
//  vxSerializer::SetAttribute( spElement, "version", "1.0" );
//  vxSerializer::Write( vxSerializer::AddElement( spElement, vxSerializer::GetElementName( sLabel ), "label" ), sLabel );
//
//} // ToXml()
//
//
///**
// * initialize from Xml node
// *
// * @param spElement the xml node
// *
// * @return success
// */
//bool vxComponentData::FromXml( MSXML2::IXMLDOMElementPtr & spElement )
//{
//
//  if ( spElement == NULL ) return false;
//
//  bool bSuccess( true );
//
//  std::string sVersion( "0.0" );
//  bSuccess &= vxSerializer::GetAttribute( spElement, "version", sVersion );
//  if ( atof( sVersion.c_str() ) == 2.0 )
//  {
//
//    std::string sLabel("Label");
//
//    bSuccess &= vxSerializer::Read(vxSerializer::FindElement(spElement, vxSerializer::GetElementName(sLabel),        "label"),          sLabel);
//
//    SetLabel(sLabel);
//  }
//  else
//  {
//    LogDbg( "Unknown version number for XML: " + sVersion, "vxComponentData", "FromXml()" );
//  }
//
//  return bSuccess;
//
//} // FromXml()


// undefines
#undef FILE_REVISION


// $Log: vxComponentData.C,v $
// Revision 1.1  2004/07/12 18:02:40  frank
// added component data
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxComponentData.C,v 1.1 2004/07/12 18:02:40 frank Exp $
// $Id: vxComponentData.C,v 1.1 2004/07/12 18:02:40 frank Exp $
