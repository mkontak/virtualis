// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once

#include "DcmElementCollection.h"

#pragma warning(push)
#pragma warning(disable:4251)

namespace dcm
{



/**
 * Implements a sequence which is a collection of element list
 */
class DCM_DLL DcmSequenceItem : public DcmElementCollection
{
protected:

  /// Constructor (Default)
  DcmSequenceItem();

  /// Constructor with group/element
  DcmSequenceItem(const unsigned int & iCard);


public:

#pragma region Create Methods


  /// Create the DcmSeqeunce from the xml node
  static std::shared_ptr<dcm::DcmSequenceItem> Create(const xml::XmlNode & node);

  /// Create the DcmSeqeunce from the xml node
  static std::shared_ptr<dcm::DcmSequenceItem> Create(const unsigned int  iCard);


#pragma endregion

public:

  
  virtual void Add(std::shared_ptr<dcm::DcmElement> & element);

  /// Clones the sequence item
  virtual std::shared_ptr<dcm::DcmElementCollection> Clone();

  /// Copies the data from this object to the supplied object
  virtual void CopyTo(std::shared_ptr<DcmSequenceItem> & pDcmSequenceItem, bool clone = false);


  /// Generates the xml representation of the sequence item list
  virtual std::string ToXml(const std::string & sRoot = "");

private:

   virtual void Initialize(xml::XmlNode & node);

public:


  /// Gets the items card number
  const unsigned int & GetCard() const
  { return m_iCard; }

#pragma region operators

public:

  /// Overloaded == operator
  inline bool operator==(const dcm::DcmSequenceItem & rhs)
  { return ( m_iCard == rhs.m_iCard ); }

 
#pragma endregion


private:


  /// Card number
  unsigned int m_iCard;


};  // class DcmSequenceItem

};  // namespace dcm

#pragma warning(pop)