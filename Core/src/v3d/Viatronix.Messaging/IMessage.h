// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: IMessage.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

#pragma once

using namespace System;

OPEN_MESSAGE_NS

/**
 * Message interface
 */
public interface class IMessage
{
  /// Generates the XML for the message
  String ^ ToXml();

};  // interface class IMessage


CLOSE_MESSAGE_NS
