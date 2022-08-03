// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxIMessage.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

#pragma once



#pragma warning(push)
#pragma warning(disable:4251)

namespace messaging
{

/**
  * Message interface
  */
class MSG_DLL IMessage
{
public:

  ///
  virtual std::string ToXml() = 0;

};  // IMessage


}  // namespace messaging