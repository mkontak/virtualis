// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once


#pragma warning(push)
#pragma warning(disable:4251)


namespace dcm
{

/**
 * Dicom Object
 */
class DCM_DLL DcmObject 
{
protected: 

  // Default constructor
  DcmObject();

  /// Create the element
  DcmObject(const DcmTag & tag);

  /// Constructor (Copy)
  DcmObject(const DcmObject & dcmElement);


protected:

  /// Abstract copy to
  virtual void CopyTo(std::shared_ptr<DcmObject> & pDcmObject);
 
  /// Initialize the DcmElement object
  virtual void Initialize( const xml::XmlNode & node);

public:

  /// String representation of the DcmObject
  virtual std::string ToString();
 
  /// Copy to 
  virtual void CopyTo(DcmItem * pDcmItem) = 0;

#pragma region operators

public:

 
  /// Overloaded == operator
  inline bool operator==(const dcm::DcmObject & rhs)
  { return ( m_tag.hash() == rhs.m_tag.hash() ); }

  
  /// Overloaded = operator
  inline void operator=(const dcm::DcmObject & rhs)
  { 
    m_tag = rhs.m_tag;
    m_sName = rhs.m_sName;
  }

#pragma endregion

#pragma region properties

public:

  /// Gets the tag
  std::string GetTag(const std::string & sFormat = "(%04x,%04x)" )
  { return util::Strings::Format(sFormat, m_tag.getGroup(), m_tag.getElement()); }

  /// Gets the Id of the element
  const int GetId() 
  { return (m_tag.getGroup() << 16) | m_tag.getElement(); }

  /// Gets the element name
  const std::string & GetName() const
  { return m_sName; }

  /// Gets the element VR
  const std::string GetVR() const
  { return std::string(m_tag.getVR().getVRName()); }


  /// Returns true if this is a private creator element
  bool IsPrivateCreator()
  { return ( m_sName == "PrivateCreator" ); }

    
  const unsigned short GetGroup() const 
  { return m_tag.getGroup(); }

  void SetGroup(unsigned short iGroup)
  { m_tag.setGroup(iGroup); }

  const unsigned short GetElement() const 
  { return m_tag.getElement(); }

  void SetElement(unsigned short iElement)
  { m_tag.setElement(iElement); }

   /// Gets the private creator
  const std::string GetPrivateCreator() const
  { return std::string( ( m_tag.getPrivateCreator() == __nullptr ? "" :  m_tag.getPrivateCreator())); }

  /// Sets the private creator
  virtual void SetPrivateCreator(const std::string & sPrivateCreator)
  { m_tag.setPrivateCreator(sPrivateCreator.c_str()); }

  /// Return true of the element is private
  bool IsPrivate()
  { return ( m_tag.getGroup() % 2 == 1 ); }

  /// Returns the internal tag
  const ::DcmTag GetDcmTag()
  { return m_tag; }

  /// Returns true if a sequence
  bool IsSequence()
  { return ( m_tag.getEVR() == EVR_SQ ); }

#pragma endregion

#pragma region fields

protected:



  /// Tag
  DcmTag m_tag;


  /// ELement name
  std::string m_sName;

#pragma endregion

}; // class DcmElement

}; // namespace dcm


#pragma warning(pop)