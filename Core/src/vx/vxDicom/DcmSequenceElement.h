// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once

#include "DcmElement.h"

#pragma warning(push)
#pragma warning(disable:4251)


namespace dcm
{

/**
 * Dicom Sequence Element 
 *
 * Extends the DcmElement by adding
 *    Card number
 */
class DCM_DLL DcmSequenceElement : public DcmElement
{
protected: 

  // Default constructor
  DcmSequenceElement();

  /// Create the element
  DcmSequenceElement(const DcmTag & tag, const std::string & sValue = "");

  /// Constructor (Copy)
  DcmSequenceElement(const DcmElement & dcmElement);

  /// Constructor (From xml node)
  DcmSequenceElement(const xml::XmlNode & pNode);

  /// Constructor (From the serialize string ( sTag = GGGG_EEEE, sValue = Serialized Value ) )
  DcmSequenceElement(const std::string & sTag, const std::string & sValue);

 
public:

 
#pragma region Create Methods

  /// Creates the DcmElement 
  template<class T>
  static std::shared_ptr<dcm::DcmSequenceElement> Create(const DcmTag & tag, T value)
  {
    std::shared_ptr<dcm::DcmSequenceElement> pDcmElement = std::shared_ptr<dcm::DcmSequenceElement>(new dcm::DcmElement(tag));
    
    if ( pDcmElement == __nullptr )
      throw DcmOutOfMemoryException(LogRec("Failed to allocate DcmElement", "DcmElement", "Create" ));

    std::ostringstream os;

    os << value;

    pDcmElement->m_sValue = os.str();

    return pDcmElement;
  }

  /// Creates a new DcmElement
  static std::shared_ptr<dcm::DcmSequenceElement> Create();
 
  /// Create the DcmElelem from the xml node
  static std::shared_ptr<dcm::DcmSequenceElement> Create(const xml::XmlNode & element);

  /// Create the DcmElelem from the serialize string ( sTag = GGGG_EEEE, sValue = Serialized Value )
  static std::shared_ptr<dcm::DcmSequenceElement> Create(const std::string & sTag, const std::string & sValue);

#pragma endregion


 
protected:

  /// Initialize the DcmElement object
  virtual void Initialize( xml::XmlNode & node);



#pragma region operators

public:

  /// Overloaded == operator
  inline bool operator==(const dcm::DcmSequenceElement & rhs)
  { return ( m_tag.hash() == rhs.m_tag.hash() ); }

  
  /// Overloaded = operator
  inline void operator=(const dcm::DcmSequenceElement & rhs)
  { 
    m_tag = rhs.m_tag;
    m_sName = rhs.m_sName;
    m_sValue = rhs.m_sValue;
    m_bBinary = rhs.m_bHidden;
    m_bHidden = rhs.m_bHidden;
    m_iVM = rhs.m_iVM;
    m_iLength = rhs.m_iLength; 
    m_iCard = rhs.m_iCard;
  }

#pragma endregion

#pragma region properties

public:

  /// Gets the length
  const unsigned int GetCard() const
  { return m_iCard; }


#pragma endregion

#pragma region fields

protected:

  /// Card number
  unsigned short m_iCard;

 
#pragma endregion

}; // class DcmSequenceElement

}; // namespace dcm


#pragma warning(pop)