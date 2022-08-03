// $Id: SeriesVersion.h,v 1.3 2006/09/26 19:33:42 frank Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Frank Dachille (frank@viatronix.com)


// pragmas
#pragma once


// includes
#include <string>


// forward declaration
class vxDOMElement;


/**
 * Handles the version of a series stored in an XML file
 */
class SeriesVersion
{
// member functions
public:

  /// default constructor
  SeriesVersion();

  /// constructor
  SeriesVersion( const std::string & sVersion );

  /// copy constructor
  SeriesVersion( const SeriesVersion & other );

  /// assignment operator
  SeriesVersion & operator =( const SeriesVersion & other );

  /// serialize object to XML
  void ToXml( vxDOMElement & element ) const;

  /// deserialize object from XML
  static SeriesVersion FromXml( vxDOMElement & element );

  /// serialize object to disk
  void ToFile( const std::string & sFilePath ) const;

  /// deserialize object from disk
  static SeriesVersion FromFile( const std::string & sFilePath );

  /// get the version
  inline const std::string & GetVersion() { return m_sVersion; }

// data members
private:
  
  /// the version
  std::string m_sVersion;

}; // class SeriesVersion


// $Log: SeriesVersion.h,v $
// Revision 1.3  2006/09/26 19:33:42  frank
// added default constructor
//
// Revision 1.2  2006/06/07 19:21:05  frank
// added accessor
//
// Revision 1.1  2006/06/07 14:00:10  frank
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/SeriesVersion.h,v 1.3 2006/09/26 19:33:42 frank Exp $
// $Id: SeriesVersion.h,v 1.3 2006/09/26 19:33:42 frank Exp $
