// $Id: Variable.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Frank Dachille (frank@viatronix.com)

#ifndef Variable_h
#define Variable_h


// includes


/**
 * Represents a variable of any type.  It manages a name + value + description.
 */
class VX_BASE_DLL Variable 
{

// Member Functions
public:

  /// Default constructor.
  Variable();
  
  /**
   * Constructor
   */
  template <class T>
  inline Variable( const std::string & sName, const T & value, const std::string & sDescription )
  {
    SetName(sName);
    SetValue(value);
    SetDescription(sDescription);
  } // Variable()
  
  /// Copy constructor.
  Variable( const Variable & other );

  /// Destructor.
  ~Variable();

  /// Assignment operator.
  Variable & operator =( const Variable & other );

  /// Equality operator.
  bool operator ==( const Variable & other ) const;
  
  /// Inequality operator.
  bool operator !=( const Variable & other ) const;
  
  /// Return the name portion of the variable.
  inline const std::string & GetName() const { return m_sName; }
  
  /// Set the name portion of the variable.
  inline void SetName( const std::string & sName ) { m_sName = sName; }
  
  /// Get the value portion of the variable.
  inline const std::string & GetValue() const { return m_sValue; }
  
  /// Set the value portion of the variable using a string.
  inline void SetValueAsString( const std::string & sValue ) { m_sValue = sValue; }
  
  /// Set the value portion of the variable.
  template <class T>
  inline void SetValue( const T & value )
  {
    std::stringstream ss;
    ss << value;
    std::string sString(ss.str());
    m_sValue = sString;
  } // SetValue()
  
  /// Get the description portion of the variable.
  inline const std::string & GetDescription() const { return m_sDescription; }
  
  /// Set the description portion of the variable.
  inline void SetDescription( const std::string & sDescription ) { m_sDescription = sDescription; }

// Data Members
private:

  /// The name portion of the variable.
  std::string m_sName;

  /// The value portion of the variable.
  std::string m_sValue;

  /// The description portion of the variable.
  std::string m_sDescription;

}; // class Variable

#endif // Variable_h

// $Log: Variable.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.5  2003/05/20 16:28:52  frank
// code review
//
// Revision 3.4  2003/05/16 11:44:23  michael
// code review
//
// Revision 3.3  2003/05/15 19:05:37  dongqing
// code review
//
// Revision 3.2  2002/05/07 18:24:54  frank
// Moved code into .inl file and added comments.
//
// Revision 3.1  2001/12/24 15:36:56  ingmar
// added class Sec_20_image to structure the doxygen documentation
//
// Revision 3.0  2001/10/14 23:02:20  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:47:26   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:34:54   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:11:46  soeren
// renamed image directory
//
// Revision 1.6  2001/07/13 03:09:42  frank
// Updated header.
//
// Revision 1.5  2001/07/02 16:28:16  frank
// Added equality test.
//
// Revision 1.4  2001/06/25 18:44:11  frank
// Renamed functions to avoid overloading.
//
// Revision 1.3  2001/06/19 19:08:24  frank
// Updated usage.
//
// Revision 1.2  2001/06/19 16:29:52  frank
// Integrated V3DFile class into system.
//
// Revision 1.1  2001/06/19 15:53:06  frank
// Initial version.
//
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Variable.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: Variable.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
