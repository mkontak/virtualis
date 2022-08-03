// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DcmElementCollectionFactory.h
//
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once



namespace dcm
{


class DCM_DLL DcmElementCollection;


/**
 * Factory class used to abstract the creation of  an element collection which can be a DcmElementCollection or 
 * DcmSequnecItem
 */
class DCM_DLL DcmElementCollectionFactory
{

#pragma region Create Static Methods

public:
  
  /// Creates the DcmElementCollection from the xmlNodePtr
  static std::shared_ptr<DcmElementCollection> Create(const xml::XmlNode & node);
  
#pragma endregion



};  // class DcmElementCollectionFactory

};  // namespace dcm