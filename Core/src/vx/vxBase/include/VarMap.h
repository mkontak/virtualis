// $Id: VarMap.h,v 1.5 2006/06/29 16:00:54 jmeade Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: David (dave@viatronix.com)

#ifndef VarMap_h
#define VarMap_h


// defines
#define FILE_REVISION "$Revision: 1.5 $"

// class definition
class VX_BASE_DLL VarMap
{
// functions
public:

  /// constructor
  VarMap();

  /// constructor from string
  VarMap(const std::string & text);

  /// copy constructor
  VarMap(const VarMap & rhs);

  /// assignment operator
  VarMap const & operator=(const VarMap & rhs);

  /// destructor
  ~VarMap();

  /// returns the map formatted to string.
  std::string ToString() const;

  /// returns the map Formatted to string.
  std::string ToString_1() const;

  /// used for vxBlockHeader specifically to isolate a set of varMaps inside a varMap
  void CreateMap(const std::string & text);
 
  /// returns the number of variables in the map
  uint4 VarCount() const;

  /// set a new header variable name, value and description
  template <class Type> 
  void SetHdrVar(const std::string & sVarName, const Type & value,  const std::string & sDesc)
  {
    // update value in map (if already exists)
    for (uint4 k(0); k<m_map.size(); ++k)
    {
      if (m_map[k][0] == sVarName) 
      {
		    m_map[k][1] = ToAscii(value);
		    m_map[k][2] = sDesc;
		    return;
      }
    } // for all map entries

	  // only if not found before, add it at the end
    std::vector<std::string> record;
    record.resize(3);
    record[0] = sVarName;
    record[1] = ToAscii(value);
    record[2] = sDesc;
    m_map.push_back(record);
  } // SetHdrVar()

  /// Remove a header variable value and description.
  void RemoveHdrVar(const std::string & sVarName)
  {
    // update value in map (if already exists)
    for ( uint4 k=0; k<m_map.size(); ++k ) 
    {
      if ( m_map[k][0] == sVarName ) 
      {
		    m_map.erase( m_map.begin()+k );
		    return;
      }
    } // for k=0; k<m_map.size()
  } // RemoveHdrVar

	
	/// looks up varName and assigns value
  template <class Type>
  bool GetHdrVar(const std::string & sVarName, Type & value) const
  {
    for (uint4 k(0); k<m_map.size(); ++k) 
    {
      if (m_map[k][0] == sVarName) 
      {
        std::istrstream is(m_map[k][1].c_str());
        is >> value;
				
        return true;
      }
    } // for all map entries

    return false; // varName does not exist
  } // GetHdrVar()


  /// string lookup (strings will not be pushed to the stream correctly by generic GetHdrVar)
  template <>
  bool GetHdrVar(const std::string & sVarName, std::string & value) const
  {
    for (uint4 k(0); k<m_map.size(); ++k) 
    {
      if (m_map[k][0] == sVarName) 
      {
        // strings will not be pushed to the stream correctly
        value = m_map[k][1];
        return true;
      }
    } // for all map entries

    return false; // varName does not exist
  } // GetHdrVar <string>

protected:

  /// copies all image data
  void Copy(VarMap const & rhs);

  /// destroys all map data
  void Destroy();

// data
protected:

  // vector used for simplicity, VC++ 6.0 map is buggy and crashes
  std::vector<std::vector<std::string> > m_map;
}; // VarMap


#undef FILE_REVISION 


#endif


// Revision History:
// $Log: VarMap.h,v $
// Revision 1.5  2006/06/29 16:00:54  jmeade
// const declarations on 'get' methods.
//
// Revision 1.4  2005/04/12 02:40:06  michael
// fixed closing comment
//
// Revision 1.3  2005/03/14 18:55:05  dongqing
// add RemoveHdrVar
//
// Revision 1.2  2004/10/25 23:41:36  michael
// chance ToString_1() to const so that one can call it on const objects, same
// for VarCount()
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.10  2003/06/04 21:32:00  jmeade
// code review.
//
// Revision 3.9  2003/05/16 13:08:20  frank
// code formatting
//
// Revision 3.8  2003/05/13 19:09:08  michael
// code review
//
// Revision 3.7  2003/01/22 21:10:15  ingmar
// code beautify
//
// Revision 3.6  2002/09/30 20:46:33  ingmar
// moved ToAscii() to 03_utils/utilities.h
//
// Revision 3.5  2002/07/18 15:11:55  mkontak
// Add VX_BLOCK_VOLUME_DLL to class definitions
//
// Revision 3.4  2002/05/17 00:48:48  jenny
// worked on VolumeHeader Version 4.0
//
// Revision 3.3  2001/12/20 14:59:48  ingmar
// added class Sec_03_utils to structure the doxygen documentation
//
// Revision 3.2  2001/12/12 20:18:09  jmeade
// Code standards.
//
// Revision 3.1  2001/11/08 16:47:00  frank
// Made constant what is constant.
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
//    Rev 1.0   Sep 16 2001 17:33:46   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:08:31  soeren
// renamed utils directory
//
// Revision 1.11  2001/05/22 16:26:28  dave
// moved typdef inside VarMap
//
// Revision 1.10  2001/05/20 20:14:31  dave
// more standards compliant
//
// Revision 1.9  2001/04/11 18:40:32  dmitry
// Sec_1_utils inheritance added
//
// Revision 1.8  2001/04/09 16:25:34  dave
// removed some uses of uint, uchar, etc
//
// Revision 1.7  2001/04/08 21:38:20  dave
// updated doc headers
//
// Revision 1.6  2001/03/27 19:10:51  dmitry
// Updated more types as def. in Typedef.h
//
// Revision 1.5  2001/03/21 18:18:36  dmitry
// Updated datatypes as defined in Typedef.h
//
// Revision 1.4  2001/03/13 19:04:02  dave
// corrected bug that placed too many spaces in string variables
//
// Revision 1.3  2001/02/12 22:43:28  dongqing
// made map entries unique
//
// Revision 1.2  2000/12/14 05:26:37  dave
// varMap string corrected
//
// Revision 1.1  2000/12/11 19:55:08  dave
// new libpng, exception, file versions etc
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/VarMap.h,v 1.5 2006/06/29 16:00:54 jmeade Exp $
// $Id: VarMap.h,v 1.5 2006/06/29 16:00:54 jmeade Exp $
