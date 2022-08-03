// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once

#include "DcmObject.h"

#pragma warning(push)
#pragma warning(disable:4251)


namespace dcm
{

// forward declarations
class DCM_DLL DcmSequenceItem;
class DCM_DLL DcmElementCollection;


/**
 * Dicom Element 
 *
 * Extends the DcmObject by adding
 *    VM    Value multiplier
 *    Value Value
 *    Hidden flag
 *    Binary flag
 */
class DCM_DLL DcmElement : public DcmObject
{
protected: 

  // Default constructor
  DcmElement();

  /// Create the element
  DcmElement(const DcmTag & tag, const std::string & sValue = "");

  /// Constructor (Copy)
  DcmElement(const DcmElement & dcmElement);


  /// Constructor (From the serialize string ( sTag = GGGG_EEEE, sValue = Serialized Value ) )
  DcmElement(const std::string & sTag, const std::string & sValue);

 
public:

 
#pragma region Create Methods

  /// Creates the DcmElement 
  template<class T>
  static std::shared_ptr<dcm::DcmElement> Create(const DcmTag & tag, T value)
  {
    std::shared_ptr<dcm::DcmElement> pDcmElement = std::shared_ptr<dcm::DcmElement>(new dcm::DcmElement(tag));
    
    if ( pDcmElement == __nullptr )
      throw DcmOutOfMemoryException(LogRec("Failed to allocate DcmElement", "DcmElement", "Create" ));

    std::ostringstream os;

    os << value;

    pDcmElement->m_sValue = os.str();

    return pDcmElement;
  }

  /// Creates a new DcmElement
  static std::shared_ptr<dcm::DcmElement> Create();
 
  /// Create the DcmElelem from the XML node
  static std::shared_ptr<dcm::DcmElement> Create(const xml::XmlNode & node);

  /// Create the DcmElelem from the serialize string ( sTag = GGGG_EEEE, sValue = Serialized Value )
  static std::shared_ptr<dcm::DcmElement> Create(const std::string & sTag, const std::string & sValue);


  /// Creates the XML representation of the element
  virtual std::string ToXml();
  
  /// Returns the serialize representation of this object (Used for the old volume headers)
  virtual std::string  ToSerializedString()
  { return util::Strings::Format("%s %s", GetTag("%04x_%04x").c_str(), GetSerializedValue().c_str() ); }


#pragma endregion

public:

  /// Clones the object
  virtual std::shared_ptr<dcm::DcmElement> Clone();

  /// Copy this element to supplied element
  virtual void CopyTo(std::shared_ptr<dcm::DcmElement> & pDcmElement);

  /// DeSerializes Value
  static std::string DeSerializeValue(const std::string & sValue);

  /// Serialize Value
  static std::string SerializeValue(const std::string & sValue);

  /// Gets the serialize string
  std::string GetSerializedValue()
  { return SerializeValue(GetValue<std::string>()); }

  /// Get the value as the type specified
  template<class T>
  T GetValue()
  {
    T returnValue;

    std::istringstream is(m_sValue);

    is >> returnValue;

    return returnValue;
  } // GetValue()

    /// Get the value as the type specified
  template<class T>
  void SetValue(T value)
  {
    std::ostringstream os;

    os << value;

    m_sValue = os.str();
  } // SetValue<T>(T value)




protected:

  /// Gets the value for the specified macro
  std::string GetMacroValue(const std::string & macro);

  /// Parses the value for special macros
  std::string ParseValue(const std::string & sValue);

  /// Initialize the DcmElement object
  virtual void Initialize( xml::XmlNode & node);

  /// Copies the element to the DcmItem
  virtual void CopyTo(DcmItem * pDcmItem);


#pragma region operators

public:

  /// Overloaded == operator
  inline bool operator==(const dcm::DcmElement & rhs)
  { return ( m_tag.hash() == rhs.m_tag.hash() ); }

  
  /// Overloaded = operator
  inline void operator=(const dcm::DcmElement & rhs)
  { 
    m_tag = rhs.m_tag;
    m_sName = rhs.m_sName;
    m_sValue = rhs.m_sValue;
    m_bBinary = rhs.m_bHidden;
    m_bHidden = rhs.m_bHidden;
    m_iVM = rhs.m_iVM;
    m_iLength = rhs.m_iLength; 
  }

#pragma endregion

#pragma region properties

public:


  /// Gets the element VM
  const unsigned short GetVM() const
  { return m_iVM; }


  /// Determines if the value is empty
  bool IsEmpty()
  { return m_sValue.empty(); }

  /// Returns true of the element's value is hidden
  bool IsHidden()
  { return m_bHidden; }

  /// Returns true if the element's value is binary
  bool IsBinary()
  { return m_bBinary; }


  /// Returns true if this is a private creator element
  bool IsPrivateCreator()
  { return ( m_sName == "PrivateCreator" ); }


  /// Gets the length
  const unsigned int GetLength() const
  { return m_iLength; }

  /// Gets the sub-element flag
  virtual const bool IsSubElement() const
  { return m_bIsSubElement; }

  /// Gets the level
  virtual const unsigned short GetLevel() const
  { return m_iLevel; }

  /// Sets the level
  virtual void SetLevel(unsigned short iLevel)
  { m_iLevel = iLevel; }

    /// Gets the value
  virtual const std::string & GetValue() const
  { return m_sValue; }

  /// Sets the value of the element
  virtual void SetValue(const std::string & sValue)
  { m_sValue = ParseValue(sValue); }

  /// Returns true or false whether the element has sub-elements (false for this object)
  virtual bool HasSubElements()
  { return false; }

  /// Returns true or false whether the element is expanded
  virtual const bool GetExpanded() const
  { return m_bExpanded; }


  /// Returns true or false whether the element is expanded
  virtual void SetExpanded(bool bExpanded)
  { m_bExpanded = bExpanded; }



#pragma endregion

#pragma region fields

protected:

  /// Element Value Multiplicity
  unsigned short m_iVM;

  /// ELement value
  std::string m_sValue;

  /// Binary flag
  bool m_bBinary;

  /// Hidden flag
  bool m_bHidden;

  /// Length
  unsigned int m_iLength;

  /// Sub-Element level (0) it Top level
  unsigned short m_iLevel;

  /// Flag indicating we are a sub element
  bool m_bIsSubElement;

  /// Expanded
  bool m_bExpanded;

  /// Friends
  friend class DcmSequenceItem;
  friend class DcmElementCollection;

#pragma endregion

}; // class DcmElement

}; // namespace dcm


#pragma warning(pop)