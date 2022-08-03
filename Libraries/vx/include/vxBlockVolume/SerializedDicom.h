// $Id: SerializedDicom.h,v 1.4 2006/06/08 18:19:03 frank Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Mark Kontak (mkontak@viatronix.com)


// pragmas
#pragma once


// typedefs
typedef std::map<std::string,std::string> ELEMENTS;
typedef ELEMENTS::iterator ELEMENTS_ITERATOR;
typedef Triple<float> Coordinate;
typedef std::vector< Triple<float> > Cosines;


/**
 * Class implementing a DICOM files Element List for iteration
 */
class VX_BLOCK_VOLUME_DLL SerializedDicom  
{
// enums
public:

  /// the image orientation
  typedef enum { ROW, COLUMN } ImageOrientation;
  
  /// the patient orientation
  typedef enum { UNKNOWN = 0, PRONE, SUPINE, DECUBITUS, DECUBITUS_LEFT, DECUBITUS_RIGHT } PatientOrientation;

public:

  /// Default Constructor
  SerializedDicom();

  /// Default Constructor
  SerializedDicom(const std::string & sSerializationString);

  /// Destructor
	virtual ~SerializedDicom();

  /// Returns the unit cosies (Image Orientation)
  std::vector< Triple<float> >  GetImageOrientation();

  /// Returns the coordinates from the serialize dicom header
  Coordinate GetCoordinates();

  /// Gets the patients orientation
  PatientOrientation GetPatientOrientation();


  /// Creates a serailzie dicom from the string
  static SerializedDicom Create(const std::string & sDicomHeader);


  /// Returns the fields value
  std::string GetFieldValue(const std::string & sGroup, const std::string & sField, const std::string & sDefaultValue = "");
  
  /// Sets the dicom field value ( returns true if successfule false if the field does not exists )
  bool SetFieldValue(const std::string & sGroup, const std::string & sField, const std::string & sValue);

  /// Adds/Sets a dicom field value in the list
  void AddFieldValue(const std::string & sGroup, const std::string & sField, const std::string & sValue);

  /// Converts the element list to a string for serialization
  std::string ToString();

    /// Returns the starting iterator
  ELEMENTS_ITERATOR begin() { return(m_Elements.begin()); }

  /// Returns the ending iterator
  ELEMENTS_ITERATOR end()  { return(m_Elements.end()); }

  /// Returns the number of elements
  int4 size() { return( static_cast< int4 >( m_Elements.size()) ); }

  /// Returns true if the element list is emptry
  bool empty() { return(m_Elements.empty()); }

  /// Dumps the element list to a file
  void DumpElementList(const std::string & sLogFile);

  /// operator<< overloading for class ElementList
  friend std::ostream& operator<<(std::ostream & os, SerializedDicom & elementList) ;

  /// operator>> overloading for class ElementList
  friend std::istream& operator>>(std::istream & is, SerializedDicom & elementList) ;

public:     // Public Properties
  

  /// Sets the serialization string (Used to replace ' ' in a field)
  void SetSerializationString(const std::string & sSerializationString) { m_sSerializationString = sSerializationString; }

  /// Returns the serialization string (Used to replace ' ' in a field)
  const std::string GetSerializationString() { return(m_sSerializationString); }

protected:    // Protected Methods

  /// Serialize the element list from an input stream
  std::istream & SerializeFromIStream(std::istream & is);
  
  /// Serailize a field by replacing the spaces with the serialization string
  std::string SerializeField(const std::string & sFieldValue);

  /// DeSerializes the field by replacing the serialization string with a space
  std::string DeSerializeField(const std::string & sFieldValue);


protected:  // Protected Members

  /// Elements List
  ELEMENTS m_Elements;

private:    // Private  Members

  /// Class Name
  static const std::string m_sClassName;
 
  /// Serialization String
  std::string m_sSerializationString;

};  // class SerializedDicom


/**
 * operator<< overloading for class SerializedDicom
 * @param os
 * @param elementList
 * @return stream
 */
inline std::ostream & operator<<(std::ostream & os, SerializedDicom & elementList) 
{  
  os << elementList.ToString();
  return(os);
} // operator<<

/**
 * operator<< overloading for class SerializedDicom
 * @param is
 * @param elementList
 * @return istream
 */
inline std::istream & operator>>(std::istream & is, SerializedDicom & elementList) 
{
  return(elementList.SerializeFromIStream(is));
} // operator>>


// $Log: SerializedDicom.h,v $
// Revision 1.4  2006/06/08 18:19:03  frank
// moved the enumerations into the class to avoid ambiguity
//
// Revision 1.3  2006/04/01 04:05:14  mkontak
// Added funtianality
//
// Revision 1.2  2006/03/30 18:58:02  mkontak
// Added GetImageOrientation() and GetCoordinates()
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.11  2003/05/22 16:34:43  dongqing
// code review
//
// Revision 1.10  2003/05/16 13:15:17  frank
// formatting
//
// Revision 1.9  2003/05/16 11:44:21  michael
// code review
//
// Revision 1.8  2002/11/07 21:42:30  mkontak
// Added ASddFieldValue()
//
// Revision 1.7  2002/09/11 21:46:23  jmeade
// Id header.
//
// Revision 1.6  2002/07/18 15:12:08  mkontak
// Add VX_BLOCK_VOLUME_DLL to class definitions
//
// Revision 1.5  2002/05/20 03:52:14  mkontak
// Added SetFieldVlaue() so that anonymization can be done.
//
// Revision 1.4  2002/04/30 14:14:53  mkontak
// Moved constructor and destrcutor into .C
//
// Revision 1.3  2002/04/26 20:43:36  mkontak
// *** empty log message ***
//
// Revision 1.2  2002/04/26 20:28:15  mkontak
// Modified constructors
//
// Revision 1.1  2002/04/26 20:03:25  mkontak
// Again a name change, but only the file names were missspelled.
//
// Revision 1.1  2002/04/26 19:44:23  mkontak
// Changed class from DicomHeader to SerializedDicom
//
// Revision 1.2  2002/04/26 18:10:42  mkontak
// Fixed compiling
//
// Revision 1.1  2002/04/26 18:05:14  mkontak
// Initial Check in
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/SerializedDicom.h,v 1.4 2006/06/08 18:19:03 frank Exp $
// $Id: SerializedDicom.h,v 1.4 2006/06/08 18:19:03 frank Exp $
