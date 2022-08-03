// $Id: VarMap.C,v 1.3 2006/06/29 16:00:54 jmeade Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: David (dave@viatronix.com)


// includes
#include "stdafx.h"
#include "VarMap.h"


// namespaces
using namespace std;


/**
 * Constructor.
 */
VarMap::VarMap()
:m_map()
{
}


/**
 * Constructor from string
 *
 * @param text   Input varmap stream.
 */
VarMap::VarMap(const string & text)
{
  istrstream textS(text.c_str());
  const int4 bSize(16384); //WARNING WITH THIS BUFFER SIZE IT IS ONLY POSSIBLE TO READ ABOUT 800 SEEDPOINTS

  std::shared_ptr<int1> buffer = std::shared_ptr<int1>( new int1 [bSize], [] ( int1 * value ) { delete [] value; } );


  int4 pound = '#';
  int4 slashR = '\r';
  int4 extraVars;

  textS.getline(buffer.get(), bSize); // get first empty line
  textS >> extraVars; textS.getline(buffer.get(), bSize);

  for (int4 i(0); i < extraVars; ++i)
  {
    // example text line
    // myVariable 1.234 # this is my favorite variable\r\n
    string varName, value, dummyStr;
  
    textS >> varName;
    textS.getline(buffer.get(), bSize, pound);
    value = buffer.get();

    // want to remove leading,trailing spaces but not intermediate ones
    // do so in this crappy way, since bug corrected late, live vols may have more than one space
    while(value.size() > 0 && value[0] == ' ')
    {
      value = value.substr(1);
    }
    while(value.size() > 0 && value[value.size()-1] == ' ')
    {
      value = value.substr(0, value.size()-1);
    }

    textS.read(buffer.get(), 1); // get the #
    textS.getline(buffer.get(), bSize, slashR); // get to the \r
    buffer.get()[textS.gcount()] = '\0';
    string sDescription(buffer.get());
    textS.read(buffer.get(), 1);
    SetHdrVar(varName, value, sDescription);
  }
} // Constructor


/**
 * Copy Constructor
 *
 * @param rhs   Source.
 */
VarMap::VarMap(const VarMap & rhs)
{
  Copy(rhs);
} // copy constructor


/**
 * Assignment operator
 *
 * @param rhs   Source.
 * @return      const reference to this after copy.
 */
VarMap const & VarMap::operator=(const VarMap & rhs)
{
  if (this != &rhs) 
  {
    Destroy();
    Copy(rhs);
  }

  return *this;
} // operator==()


/**
 * Destructor
 */
VarMap::~VarMap()
{
  Destroy();
}


/**
 * Returns the map formatted to string.
 *
 * @return string   Representation of the varmap.
 */
string VarMap::ToString() const
{
  ostrstream os;

  os << "\r\n" << VarCount() << " # Number of Map Variables \r\n";

  // write extra header variables if they exist
  for (uint4 i(0); i < m_map.size(); ++i)
  {
    os << m_map[i][0]<<" "<< m_map[i][1]<<" # "<< m_map[i][2]<<"\r\n";
  }
  os << ends;
  
  return os.str();
} // ToString()


/**
 * Returns the map Formatted to string, is used to disect varMap into smaller varMaps
 *
 * @return string   Alternative representation of varmap.
 */
string VarMap::ToString_1() const
{
  ostrstream os;

  os << "\r\n" << VarCount() << " # Number of Map Variables \r\n";

  // write extra header variables if they exist
  for (uint4 i(0); i < m_map.size(); ++i)
  {
    os << m_map[i][0] << " <<<" << m_map[i][1] << ">>> " << m_map[i][2] << "\r\n";
  }
  os << ends;

  return os.str();
} // ToString_1()


/**
 * Create map
 *
 * @param test   Input varmap stream.
 */
void VarMap::CreateMap(const string & text)
{
  istrstream textS(text.c_str());
  const int4 bSize(16384); //WARNING WITH THIS BUFFER SIZE IT IS ONLY POSSIBLE TO READ ABOUT 800 SEEDPOINTS

  std::shared_ptr<int1> buffer = std::shared_ptr<int1>( new int1 [bSize], [] ( int1 * value ) { delete [] value; } );

  int4 extraVars;

  textS.getline(buffer.get(), bSize); // get first empty line
  textS >> extraVars; textS.getline(buffer.get(), bSize);

  for (int4 i(0); i < extraVars; ++i)
  {
    // new varMap name
    string varMapName, valueStr;
    std::ostrstream value;

    value << "\r\n";
    textS >> varMapName;

    // eat the rest of the characters and start reading the essential info
    textS.getline(buffer.get(), bSize);
    textS.getline(buffer.get(), bSize);
    valueStr = buffer.get();

    while (valueStr.find(">>>",0) == std::string::npos)
    {
      value << valueStr << "\r\n";
      textS.getline(buffer.get(), bSize);
      valueStr = buffer.get();
    }
    value << std::ends;
    SetHdrVar(varMapName, value.str(), "");
  }
} // CreateMap()


/**
 * Returns the number of variables in the map
 *
 * @return size of the variable map list.
 */
uint4 VarMap::VarCount() const
{
  return ( static_cast< uint4 >( m_map.size()));
}


/**
 * Copies all image data
 *
 * @param rhs   Source.
 */
void VarMap::Copy(VarMap const & rhs)
{
  m_map = rhs.m_map;
}


/**
 * Destroys all map data
 */
void VarMap::Destroy()
{
  m_map = vector<std::vector<std::string> >();
}


// Revision History:
// $Log: VarMap.C,v $
// Revision 1.3  2006/06/29 16:00:54  jmeade
// const declarations on 'get' methods.
//
// Revision 1.2  2004/10/25 23:41:12  michael
// chance ToString_1() to const so that one can call it on const objects
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.4  2003/06/04 21:31:59  jmeade
// code review.
//
// Revision 3.3  2003/05/13 19:09:08  michael
// code review
//
// Revision 3.2  2002/05/17 00:48:48  jenny
// worked on VolumeHeader Version 4.0
//
// Revision 3.1  2001/11/05 23:42:49  soeren
// Fixed reading of more than 20 seedpoints, no the limit is at 800 seedpoints
//
// Revision 3.0  2001/10/14 23:02:16  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
// 
//    Rev 2.1   Sep 19 2001 14:30:04   liwei
// No change.
// 
//    Rev 2.0   Sep 16 2001 23:46:36   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:33:44   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:08:31  soeren
// renamed utils directory
//
// Revision 1.7  2001/05/22 16:26:27  dave
// moved typdef inside VarMap
//
// Revision 1.6  2001/05/20 20:14:32  dave
// more standards compliant
//
// Revision 1.5  2001/04/09 16:25:34  dave
// removed some uses of uint, uchar, etc
//
// Revision 1.4  2001/03/27 19:10:51  dmitry
// Updated more types as def. in Typedef.h
//
// Revision 1.3  2001/03/21 18:18:36  dmitry
// Updated datatypes as defined in Typedef.h
//
// Revision 1.2  2001/03/13 19:04:02  dave
// corrected bug that placed too many spaces in string variables
//
// Revision 1.1  2000/12/11 19:55:08  dave
// new libpng, exception, file versions etc
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/VarMap.C,v 1.3 2006/06/29 16:00:54 jmeade Exp $
// $Id: VarMap.C,v 1.3 2006/06/29 16:00:54 jmeade Exp $
