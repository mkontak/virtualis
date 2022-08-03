// $Id: CmdLineSwitches.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Created:
// Author: [Name] [e-mail]
//
// Complete history at bottom of file.

#define FILE_REVISION "$Revision: 1.1.1.1 $"

/**
 *  @return
 */
std::string CmdLineSwitches::GetCommandLine()
{
  std::string tmp;
  for ( uint4 k=0; k<m_switchMatrix.size(); ++k )
    tmp += m_switchMatrix[k].m_id + " " + m_switchMatrix[k].m_value + " ";
  return tmp;
} // GetCommandLine


/**
 * Accesses a specific switch by its index value.
 *
 * @param idStr  Identification String information of a switch
 * @return       std::string cast of a switch value
 */
std::string CmdLineSwitches::ToString(const std::string & id)
{
  int4 idIndex = IdIndex(id);
  if ( idIndex == -1 )
  {
    throw ex::VException(LogRec(ToAscii(id)+" is not a valid parameter","CmdLineSwitches","ToString"));
    return "";
  }   
  
  return (m_switchMatrix[idIndex].m_value);
} // asString


/**
 * Accesses a specific switch by its index value.
 *
 * @param idStr  Identification String information of a switch
 * @return       boolean cast of a switch value
 */
bool CmdLineSwitches::ToBool(const std::string & id)
{
  int4 idIndex = IdIndex(id);
  if ( idIndex == -1 )
  {
    throw ex::VException(LogRec(ToAscii(id)+" is not a valid parameter","CmdLineSwitches","ToBool"));
    return 0;
  }     
  std::string value = m_switchMatrix[idIndex].m_value;
  if ( value == "true" || value == "True" || value == "TRUE" )
    return (true);
  else if ( value == "false" )
    return (false);
  else
    return(0 != atoi(value.c_str()));
} // asBool


/**
 * Accesses a specific switch by its index value.
 *
 * @param idStr  Identification String information of a switch
 * @return        cast reference of a switch value
 */
int4 CmdLineSwitches::ToInt(const std::string & id)
{
  int4 idIndex = IdIndex(id);
  if ( idIndex == -1 )
  {
    LogDbg(ToAscii(id)+" is not a valid parameter","CmdLineSwitches","ToInt");
    throw ex::VException(LogRec(ToAscii(id)+" is not a valid parameter","CmdLineSwitches","ToInt"));
    return 0;
  }       
  return (atoi(m_switchMatrix[idIndex].m_value.c_str()));
} // asInt


/**
 * Accesses a specific switch by its index value.
 *
 * @param idStr  Identification String information of a switch
 * @return       double cast of a switch value
 */
float8 CmdLineSwitches::ToDouble(const std::string & id)
{
  int4 idIndex = IdIndex(id);
  if ( idIndex == -1 )
  {
    throw ex::VException(LogRec(ToAscii(id)+" is not a valid parameter","CmdLineSwitches","ToDouble"));
    return 0.0;
  }       
  // note that atof confusingly returns a double, could be named atod()
  return (atof(m_switchMatrix[idIndex].m_value.c_str()));
} // asDouble


/**
 * Accesses a specific switch by its index value.
 *
 * @param idStr  Identification String information of a switch
 * @return       unsigned long cast of a switch value
 */
uint8 CmdLineSwitches::ToULong(const std::string & id)
{
  int4 idIndex = IdIndex(id);
  if ( idIndex == -1 )
  {
    throw ex::VException(LogRec(ToAscii(id)+" is not a valid parameter","CmdLineSwitches","ToULong"));
    return 0;
  }       
  unsigned long res; // damn atol() returns long
  ToType(m_switchMatrix[idIndex].m_value, res); // call to Global.h
  return (static_cast<uint8>(res));
} // asULong

#undef FILE_REVISION

// $Log: CmdLineSwitches.inl,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.2  2003/05/13 21:09:19  dongqing
// code review
//
// Revision 3.1  2001/10/26 02:21:06  tushar
// removed errors due to changes made to logger class, updating again
//
// Revision 3.0  2001/10/14 23:02:14  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Sep 19 2001 14:29:52   liwei
// No change.
// 
//    Rev 2.0   Sep 16 2001 23:46:08   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:33:26   ingmar
// Initial revision.
// Revision 1.6  2001/09/13 21:34:33  ingmar
// removed ToStrstream( )
//
// Revision 1.5  2001/09/13 17:42:48  soeren
// added quotes to function name in logger macro
//
// Revision 1.4  2001/09/10 09:04:59  ingmar
// fixed use of logger defines
//
// Revision 1.3  2001/09/07 21:21:17  soeren
// added #undef FILE_VERSION in the end of each file
//
// Revision 1.2  2001/09/07 17:59:07  uday
// Logger::xyz  -> LogXyz
//
// Revision 1.1  2001/09/01 00:08:28  soeren
// renamed utils directory
//
// Revision 1.18  2001/08/13 20:04:53  tushar
// removed errors due to the new logger
//
// Revision 1.17  2001/08/01 14:49:06  soeren
// changed exceptions to logger::execptions
//
// Revision 1.16  2001/08/01 14:45:18  soeren
// changed exceptions to logger::execptions
//
// Revision 1.15  2001/07/24 22:32:38  tushar
// fixed a byg
//
// Revision 1.14  2001/04/25 19:49:37  dmitry
// Created handle for hidden switches in the ToOStream method.
//
// Revision 1.13  2001/04/12 18:50:18  ingmar
// vx::Exception -> ex::Exception
//
// Revision 1.12  2001/04/12 15:50:50  dave
// added namespace vx for exceptions
//
// Revision 1.11  2001/04/09 22:23:15  dave
// changed from abort to exceptions
//
// Revision 1.10  2001/04/05 17:59:53  dmitry
// updated datatypes
//
// Revision 1.9  2001/03/15 18:03:22  soeren
// Added new Function GetCommandLine()
//
// Revision 1.8  2001/02/28 00:10:04  ingmar
// Error::Abrt willnow be called upon using a nonexisting switch
//
// Revision 1.7  2001/02/26 15:50:28  dmitry
// Changed method names to prog. conventions, added comments.
//
// Revision 1.6  2001/02/23 20:09:36  dmitry
// Replaced char ** array by std::string
//
// Revision 1.5  2000/12/19 20:38:33  dave
// added some stuff
//
// Revision 1.4  2000/10/24 02:56:46  ingmar
// removed VxIterator and added the functionality to the VolIterator
//
// Revision 1.3  2000/10/23 18:01:34  ingmar
// added asBool member function
//
// Revision 1.2  2000/10/23 16:34:14  dave
// asBool added to CmdLine
//
// Revision 1.1.1.1  2000/08/11 22:15:50  ingmar
// unit tests complete
//
//
// 5     8/29/00 12:23p Antonio
// updated header
//
// 4     8/21/00 12:28a Sarang
// modified header and deleted a few lines of dead code
//
// *****************  Version 3  *****************
// User: Antonio      Date: 8/02/00    Time: 10:14p
// Updated in $/vx/src/Utils
// added header and complete history to end of file
//
// *****************  Version 2  *****************
// User: Ingmar          Date:  7/19/00  Time: 10:30a
// Checked in $/v2k/src/Utils
// Comment:
//   added CmdLineSwitches to Utils library
//
// *****************  Version 1  *****************
// User: Ingmar          Date:  7/19/00  Time:  9:22a
// Created CmdLineSwitches.inl
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/CmdLineSwitches.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: CmdLineSwitches.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
