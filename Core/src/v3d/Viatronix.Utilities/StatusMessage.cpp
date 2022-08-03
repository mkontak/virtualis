// $Id: StatusMessage.cpp,v 1.2.2.1 2011/05/18 01:56:58 mkontak Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]


// Include
#include "stdafx.h"
#include "StatusMessage.h"


// Namespace
using namespace System;
using namespace System::Text;
using namespace System::Xml;
using namespace System::IO;
using namespace Viatronix::Logging;

USING_UTILITIES_NS
USING_PIPELINE_NS

/**
 * Constructor.
 */
 StatusMessage::StatusMessage() :
/**********************************************************************************************************************/
m_sMessage(String::Empty),
m_sModuleType(String::Empty),
m_iStageProgress(0),
m_iOverallProgress(0),
m_eMessageType(MessageTypes::Debug),
m_sStage(String::Empty)
{

} // StatusMessage()



/**
 * Returns the string as a delimited message
 *
 * @return The xml representation of the status message
 */
String ^ StatusMessage::ToXml()
{

  MemoryStream ^ pStream = gcnew MemoryStream();

    XmlTextWriter ^ pWriter = gcnew XmlTextWriter( gcnew StreamWriter( pStream ));

    pWriter->WriteStartElement( "Status" );

    pWriter->WriteAttributeString( "moduleType",  m_sModuleType );
    pWriter->WriteAttributeString( "stage",  m_sStage );
    pWriter->WriteAttributeString( "stageProgress",  System::Convert::ToString(m_iStageProgress) );
    pWriter->WriteAttributeString( "overallProgress",  System::Convert::ToString(m_iOverallProgress) );
    pWriter->WriteAttributeString( "messageType", System::Convert::ToString((int)m_eMessageType) );
    pWriter->WriteAttributeString( "message",  m_sMessage );


    pWriter->WriteEndElement();
    pWriter->Flush();

    ASCIIEncoding ^ pEncoding = gcnew ASCIIEncoding();
    return pEncoding->GetString( pStream->ToArray() );

} // ToXml()

/// <summary>
/// Creates a new instance of a Patient object from xml.
/// </summary>
/// <param name="node">The xml used to reconstruct a Patient object.</param>
/// <returns>A Patient object.</returns>
StatusMessage ^ StatusMessage::FromXml( XmlNode ^pNode )
{
  StatusMessage ^ pStatus = gcnew StatusMessage();

  try
  {
    pStatus->m_sMessage = pNode->Attributes["message"]->Value;
    pStatus->m_sModuleType = pNode->Attributes["moduleType"]->Value;
    pStatus->m_sStage = pNode->Attributes["stage"]->Value;
    pStatus->m_iStageProgress = int::Parse(pNode->Attributes["stageProgress"]->Value);
    pStatus->m_eMessageType = ( pNode->Attributes["messageType"] != __nullptr ? 
                                  (MessageTypes)Enum::Parse( MessageTypes::typeid , pNode->Attributes["messageType"]->Value ): 
                                  MessageTypes::Information );
    pStatus->m_iOverallProgress = int::Parse(pNode->Attributes["overallProgress"]->Value);
    

    return pStatus;
  }
  catch ( Exception ^ ex )
  {
    Log::Error("Failed to create StatusMessage from XML : " +  ex->Message, "StatusMessage", "FromXml");
    throw;
  }

} // FromXml( node )

// $Log: StatusMessage.cpp,v $
// Revision 1.2.2.1  2011/05/18 01:56:58  mkontak
// Issue CON-1640: Windows 7 Compliance by removing the SystemSettings
// registry to a global config file. Move the default log directory to the windows
// common application data directory (XP - C:\Document and Settings\All Users\Application Data , Win7 - C:\ProgramData ).
// Merged the Viatronix.Logger project into the Viatronix.Utilities project and
// signed the Utilities assembly.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.2  2007/03/09 21:07:03  mkontak
// Coding standards
//
// Revision 1.1  2006/12/05 19:16:17  gdavidson
// Consolidated Viatronix.Utilities.Pipelining
//
// Revision 1.8  2006/04/27 23:55:57  mkontak
// no message
//
// Revision 1.7  2006/04/27 14:05:49  mkontak
// Minor changes to messages
//
// Revision 1.6  2006/04/27 13:55:38  mkontak
// Minor changes to messages
//
// Revision 1.5  2006/04/11 19:41:47  mkontak
// Added message type to allow for message sending of different types and a
// default of DEBUG allowing the message to be logged to the WQ logger
//
// Revision 1.4  2005/06/08 13:39:04  mkontak
// Change message communication with executables
//
// Revision 1.3  2004/06/03 15:52:34  mkontak
// Change in the message handler port assignment
//
// Revision 1.2  2004/05/21 18:01:14  mkontak
// Fixes to pipelining
//
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.2  2003/09/18 01:58:04  mkontak
// Coding standards
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/StatusMessage.cpp,v 1.2.2.1 2011/05/18 01:56:58 mkontak Exp $
// $Id: StatusMessage.cpp,v 1.2.2.1 2011/05/18 01:56:58 mkontak Exp $

