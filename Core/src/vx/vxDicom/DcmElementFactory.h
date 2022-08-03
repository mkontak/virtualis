#pragma once


#include "DcmElement.h"


#pragma warning(push)
#pragma warning(disable:4251)


namespace dcm
{



/**
 * Abstracts the creation of the DcmElements which can either be 
 * DcmElement or DcmSequence objects.
 */
class DCM_DLL DcmElementFactory
{

public:

  /// Creates the element based on the node (May be an DcmElement or DcmSequence)
  static std::shared_ptr<dcm::DcmElement> Create(const xml::XmlNode & node);
 
};  // class DcmElementFactory



};  // namespace dcm