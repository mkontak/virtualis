// $Id: CmdLineSwitches.C,v 1.2 2005/07/01 12:30:07 vxconfig Exp $
//
// Copyright 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Ceated: 7/19/00
// Author: Ingmar Bitter (ingmar@viatronix.com)

#include "stdafx.h"
#include "CmdLineSwitches.h"

using namespace std;


#define FILE_REVISION "$Revision: 1.2 $"


/**
 * Demo of a sample command line interface and CmdLineSwitches methods.
 *
 * @param argc
 * @param argv
 */
void CmdLineSwitches::Demo(int argc, char *argv[]) 
{
  cout << "Demo of class CmdLineSwitches:" << endl;

  // standard volume rendering switches
  CmdLineSwitches
  volRenSwitches(0, "-c4", "1", "Cube4 classic mode",
                 "-cl", "0", "Cube4 light   mode",
                 "-c5", "0", "Cube5 (perspective) mode",

                 "-fb", "1", "front to back processing/compositing",
                 "-bf", "0", "back to front processing/compositing",

                 "-ds", "bulb.slc", "dataset",
                 "-ct", "colorLookupTable.txt", "color lookup table",
                 "-ir", "image", "output image filename root",
                 "-pf", "renderingParameter.txt", "additional parameter file",

                 "-so", "0", "shading off",
                 "-sp", "0", "shading Phong",
                 "-sm", "0", "shading reflectance map",
                 "-sv", "1", "shading reflectance vector",
                 "-vrp", "0 0 2", "view reference point",
                 "-vup", "0 1 0", "view up vector",
                 "-vpn", "0 0 -1","view plane normal",

                 "-bg", "0 0 0", "background color",
                 "-ia", "1 1 1", "RGB intensity of ambient light",
                 "-ka", "0.5",   "ambient coefficient",
                 "-kd", "0.5",   "diffuse coefficient",
                 "-ks", "0.5",   "specular coefficient",
                 "-px", "50",    "Phong exponent",

                 "-nli", "14", "number of light sources used (max=16)",
                 "-li0", " 0.0  0.0  1.0   1 .5 .5  0.1", "light 0",
                 "-li1", " 0.0  0.0  1.0   1  0  0  60",  "light 1",
                 "-li2", "-1.0  1.0  1.0   8  8  9  30",  "light 2",
                 "-li3", "-1.0  1.0  1.0  -9 -9  9  50",  "light 3",
                 "-li4", " 1.0  1.0  1.0   8  8  9  30",  "light 4",
                 "-li5", " 1.0  1.0  1.0  -9 -9  9  50",  "light 5",
                 "-li6", "-1.0 -1.0  1.0   1  1  1  50",  "light 6",
                 "-li7", " 1.0 -1.0  1.0   1  1  1  50",  "light 7",
                 "-li8", "-0.7 -1.0  0.9   1  1  1  60",  "light 8",
                 "-li9", " 0.7 -1.0  0.9   1  1  1  60",  "light 9",
                 "-liA", "-0.3 -1.0  0.8   1  1  1  60",  "light A",
                 "-liB", " 0.3 -1.0  0.8   1  1  1  60",  "light B",
                 "-liC", " 0.0 -1.0  0.7   1  1  1  60",  "light C",
                 "-liD", " 0.0  0.0 -1.0   0  0  1  30",  "light D",
                 "-liE", " 0.0  1.0  0.0   0  1  0  30",  "light E",
                 "-liF", " 1.0  0.0  0.0   1  0  0  30",  "light F",

                 "-ic", "1",   "isosurface classification mode",
                 "-th", "0.7", "isosurface classification threshold",
                 "-mw", "1.5", "isosurface classification maximum width",
                 "-ma", "1.0", "isosurface classification alpha multiplyer",

                 "-h",    "0",   "display short help",
                 "-help", "0",   "display long help",
                 "$");  // "$" terminates the list of switches

  std::cout << "\n standard volume rendering switches:\n"
  <<volRenSwitches;

  // more complete demo of shorter switch list:
  CmdLineSwitches switches(0, "-pf", "renderingParameter.txt", "additional parameter file",
                           "-bg", "1 1 1", "background color",
                           "-h",    "0",   "display short help",
                           "-help", "0",   "display long help",
                           "$");  // "$" terminates the list of switches

  cout<<"\n default configuration:\n"
  <<switches;

  switches.ReadCommandLine(argc, argv);
  cout<<"\n current configuration after parsing commandline:\n"
  <<switches;

  cout<<"\n values retrieved by application:\n";
  cout.width(25); cout<<switches.DescriptionStr("-bg")<<" = "<<switches.ToString("-bg")<<endl;
  cout.width(25); cout<<switches.DescriptionStr("-h") <<" = "<<switches.ToString("-h")<<endl;
  cout.width(25); cout<<switches.DescriptionStr("-help")<<" = "<<switches.ToString("-help")<<endl;

  cout<<"\n wrong switch results in:\n"
  <<switches.DescriptionStr("-this switch doesn't exist");

  cout << "End of Demo of class CmdLineSwitches" << endl;
}   // Demo


/**
 * Constructor, initiates all values of m_switchMatrix
 *
 * @param strListStart List of all switches' id, value and description.
 */
CmdLineSwitches::CmdLineSwitches(int i, const char * list... ) 
{
  va_list  strList;
  va_start(strList, 1);
  Switch switchRD;
  string str;
  int numSwitches = 0;

  str = list;
  while ( str != "$" ) 
  {
    ++numSwitches;
    str = va_arg( strList, char * );
  }
  numSwitches /= 3; // 3 strings per switch

  //   run through formal parameters and store them in m_switchMatrix

  va_start(strList, list);

  str = list;
  switchRD.m_id = str;
  str = va_arg( strList, char * );
  switchRD.m_value = str;
  str = va_arg( strList, char * );
  switchRD.m_description = str;
  m_switchMatrix.resize(1);
  m_switchMatrix[0] = switchRD;

  // rest can be done in loop
 for ( int k=1; k < numSwitches; ++k ) 
 {

    str = va_arg( strList, char * );
    switchRD.m_id = str;
    str = va_arg( strList, char * );
    switchRD.m_value = str;
    str = va_arg( strList, char * );
    switchRD.m_description = str;
    m_switchMatrix.resize(k+1);
    m_switchMatrix[k] = switchRD;
  }

  va_end(strList);
} // constructor


/**
 * Destructor
 */
CmdLineSwitches::~CmdLineSwitches()
{
} // destructor


/**
 * Sends a list of all switches, values of and description to ostream.
 *
 * @param os     Reference passed to ostream.
 * @return
 */
std::ostream & CmdLineSwitches::ToOstream(std::ostream & os) const 
{
	for ( uint4 k=0; k < m_switchMatrix.size(); ++k ) 
	{
		if ( !m_switchMatrix[k].m_bHide ) 
    {
			if ( m_switchMatrix[k].m_id.length() <= 7 ) 
      {
				os.width(7);
				os<<m_switchMatrix[k].m_id;
				os<<" [ "; os.width(46);
			} 
      else 
      {
				os.width(8);
				os<<m_switchMatrix[k].m_id;
				os<<"[ "; os.width(46);
			}
			os<<m_switchMatrix[k].m_description <<" ]  : ";
			os<<m_switchMatrix[k].m_value;
			os<<endl;
		}
	}
	return (os);
} // AsOstream


/**
 * Reads in switches from command line.
 *
 * @param argc
 * @param argv
 */
void CmdLineSwitches::ReadCommandLine(int argc, char *argv[]) 
{
  m_argv0 = argv[0]; // assigne ex. name

  // then read command line
  for ( int k=1; k<argc; ++k ) 
  { // skipp first, because it is command name itself
    if ( *argv[k] == '-' ) 
    {
      // found a switch
      if ( k+1 < argc && *argv[k+1] != '-' ) 
      {
        // switch with argument
        SetSwitchValue(argv[k], argv[k+1]);
        ++k;
      } 
      else 
      {
        // switch without argument
        SetSwitchValue(argv[k], "");
      }
    } 
    else 
    {
      cout << "value without switch, skipp\n"; // value without switch, skipp
    }
  }
  if ( (IdIndex("-h") != -1) && ToInt("-h") > 0 ) 
  {
    Usage();
  } // allways print usage if "-h" switch is given
} // readCommandLine


/**
 * Reads in switches and values from parameter file, and updating switch values.
 *
 * @param parameterFileName
 */
void CmdLineSwitches::ReadParameterFile(const string & parameterFileName) 
{
  char charLine[256];
  string switchStr, valueStr;
  string fileName("");
  istrstream strParser(parameterFileName.c_str());

  // skipp whitespace in file name
  strParser >> fileName;
  if (fileName.length() == 0)
  {
    return;
  }

  ifstream parameterFile(fileName.c_str());
  if ( !parameterFile ) return;
  parameterFile.getline(charLine,255);
  //  strParser = istrstream(charLine);

  while ( parameterFile ) 
  {
    istrstream strParser(charLine);
    
    strParser >> switchStr;
    if ( (switchStr[0] == char(0)) || (switchStr.substr(0, 2) == "#") ) 
    {
      // line is empty or starts with comment, ignore
    } 
    else 
    {
      if ( switchStr[0] != '-' ) 
      {
        // switch doesn't start with dash, wrong syntax, ignore
      } 
      else 
      {
        // copy value
        strParser >> valueStr;
        if ( (valueStr[0] == char(0)) ||  (valueStr.substr(0, 2) == "#") ) 
        {
          // switch without value
          valueStr = "";
        }
        else 
        {
          if ( valueStr[0] == '"' ) 
          {
            valueStr = valueStr.substr(1, valueStr.length()-1);
            bool end = false;
            int k;
            for (k = -1; !end && valueStr[k] != '"'; ++k) 
            {
              if (k == valueStr.length()-1) 
              {
                string appendStr;
                strParser >> appendStr;
                if (appendStr == "") end = true; 
                valueStr += appendStr;
              }
            }
            if (!end) 
              valueStr = valueStr.erase(k);
          }
        }
        SetSwitchValue(switchStr, valueStr);
      }
    }
    parameterFile.getline(charLine,255);
   // strParser = istrstream(charLine);
  }
  parameterFile.close();
} // ReadParameterFile


/**
 * Sets switch hide value to true.
 * 
 * @param sId    Id of switch to hide.
 */
void CmdLineSwitches::Hide (const string & sId) 
{
  uint4 iIndex(0); 
  bool bFound(false);
  for ( ; iIndex < m_switchMatrix.size() && !bFound; ++iIndex )  
  {
    if ( m_switchMatrix[iIndex].m_id == sId ) 
    {
      bFound = true;
      m_switchMatrix[iIndex].m_bHide = true;
    }
  }
} // Hide


/**
 * Sets switch hide value to false.
 * 
 * @param sId    Id of switch to show.
 */
void CmdLineSwitches::Show (const string & sId) 
{
  uint4 iIndex(0); 
  bool bFound(false);
  for ( ; iIndex < m_switchMatrix.size() && !bFound; ++iIndex ) 
  {
    if ( m_switchMatrix[iIndex].m_id == sId ) 
    {
      bFound = true;
      m_switchMatrix[iIndex].m_bHide = false;
    }
  } 
} // Show


/**
 * Sets value of specified switch.
 *
 * @param idStr      Switch id.
 * @param valueStr   New switch value
 */
void CmdLineSwitches::SetSwitchValue(const string & idStr, string valueStr) 
{

  // first check if such idStr exists
  int idIndex = 0; 
  bool idFound = false;
  uint4 k;
  for ( k=0, idFound=false; k<m_switchMatrix.size() && !idFound; ++k ) 
  {
    if ( m_switchMatrix[k].m_id == idStr ) 
    {
      idFound = true;
      idIndex = k;
    }
  }
  if ( !idFound ) 
  {
    return;
  } // other program parts might have this switch

  // togle switches  (no value/parameter given)
  if ( (valueStr[0] == char(0)) || (valueStr[0] == '\0') ) 
  {
    // record how often togles where used and put the current number as value
    // application can then easily find out which togle was used last
    // ( the one with the higher value )  :-)
    static int numTogle(0); // increase every time a toggle switch is used
    ++numTogle;
    if ( numTogle > 100 )
      throw ex::VException(LogRec("Do not know","CmdLineSwitches","SetSwitchValue"));
              
    m_switchMatrix[idIndex].m_value = '1'+numTogle;
    return;
  }

  // switches with parameter
  // skipp whitespace before value
  while ( valueStr[0] == ' ' || valueStr[0] == '\t' ) valueStr = valueStr.substr(1, valueStr.length()-1);
    m_switchMatrix[idIndex].m_value = valueStr;
} // setSwitchValue


/**
 * Locates a switch by id, returns index.
 *
 * @param idStr  String id.
 * @return       Switch index value.
 */
int CmdLineSwitches::IdIndex(const std::string & id)
{
  // first check if such idStr exists
  int idIndex=-1;
  for ( uint4 k=0, idFound=false; k < m_switchMatrix.size() && !idFound; ++k ) 
  {
    if ( m_switchMatrix[k].m_id == id ) 
      idIndex = k;
  }
  // ok, we fond the matching index, so lets return it
  return (idIndex);
} // idIndex


/**
 * Locates a switch by id, returns description of argument or NULL.
 *
 * @param idStr  String id.
 * @return       Switch description.
 */
const char * CmdLineSwitches::DescriptionStr(const string & idStr) 
{
  return (m_switchMatrix[IdIndex(idStr)].m_description.c_str());
} // descriptionStr


/**
 * Prints *this pointer value to standard out.
 */
void CmdLineSwitches::Usage() 
{
  cout << "Usage: <command> followed by"<<endl;
  cout << *this;
} // usage


/**
 * Returns vector of "exName triple1 triple2, etc"
 */
vector<string> CmdLineSwitches::ToVector() const
{
  vector<string> cmdLine(m_switchMatrix.size() +1);
  cmdLine[0] = m_argv0;

  for(uint4 i=0; i < m_switchMatrix.size(); ++i) 
  {
    string line = m_switchMatrix[i].m_id; line += " ";
    line += m_switchMatrix[i].m_value;
    cmdLine[i+1] = line;
  }

  return cmdLine;
} // asVector


/**
 * Writes all switches, values of and description to file through ofstream variable.
 *
 * @param fileName Name of file to be written to.
 */
void CmdLineSwitches::WriteSwitchFile(const string & fileName) 
{
  ofstream switchFile(fileName.c_str());
  for ( uint4 k=0; k<m_switchMatrix.size(); ++k ) 
  { 
    if (!m_switchMatrix[k].m_bHide) 
    {
      switchFile.width(8) ; switchFile << m_switchMatrix[k].m_id << " ";
      switchFile.width(8) ; switchFile << m_switchMatrix[k].m_value << " # ";
      switchFile << m_switchMatrix[k].m_description << endl;
    }
  }
} // writeSwitchFile

#undef FILE_REVISION

// end of CmdLineSwitches.C


// $Log: CmdLineSwitches.C,v $
// Revision 1.2  2005/07/01 12:30:07  vxconfig
// Added new Logger.
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.4  2003/05/16 11:42:40  michael
// code review
//
// Revision 3.3  2003/05/13 21:09:18  dongqing
// code review
//
// Revision 3.2  2002/02/25 23:12:15  geconomos
// Updated for vc7. See George for a list of effected files.
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
//    Rev 2.1   Sep 19 2001 14:29:50   liwei
// No change.
// 
//    Rev 2.0   Sep 16 2001 23:46:08   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:33:26   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:08:28  soeren
// renamed utils directory
//
// Revision 1.30  2001/08/13 20:04:53  tushar
// removed errors due to the new logger
//
// Revision 1.29  2001/07/24 22:32:38  tushar
// fixed a byg
//
// Revision 1.28  2001/06/05 20:39:34  soeren
// changed comments from % to #
//
// Revision 1.27  2001/06/05 17:10:00  soeren
// comments begin now with % instead of //
//
// Revision 1.26  2001/04/25 21:30:30  soeren
// beautified
//
// Revision 1.25  2001/04/25 19:48:39  dmitry
// Created Hide and Show methods.
//
// Revision 1.24  2001/04/17 19:52:51  soeren
// fixed bugs
//
// Revision 1.23  2001/04/16 19:58:43  ingmar
// no longer reads parameter file before command line args
//
// Revision 1.22  2001/04/12 18:50:00  ingmar
// vx::Exception -> ex::Exception
//
// Revision 1.21  2001/04/12 15:50:50  dave
// added namespace vx for exceptions
//
// Revision 1.20  2001/04/09 22:23:15  dave
// changed from abort to exceptions
//
// Revision 1.19  2001/04/08 21:38:19  dave
// updated doc headers
//
// Revision 1.18  2001/03/16 15:49:09  dmitry
// CmdLine switch w/ no value handled
//
// Revision 1.17  2001/03/15 19:01:13  ingmar
// added check for empty parameter file
//
// Revision 1.16  2001/03/15 18:03:22  soeren
// Added new Function GetCommandLine()
//
// Revision 1.15  2001/03/01 14:51:34  dmitry
// Debuged
//
// Revision 1.14  2001/02/28 21:06:56  dmitry
// Replaced all (char *) with std::string (except for the constructor),
// rewrote ReadParameterFile method to use istrstream
//
// Revision 1.13  2001/02/28 00:10:04  ingmar
// Error::Abrt willnow be called upon using a nonexisting switch
//
// Revision 1.12  2001/02/26 17:05:16  dmitry
// IdIndex will not print error if switch is not present.
//
// Revision 1.11  2001/02/26 15:50:28  dmitry
// Changed method names to prog. conventions, added comments.
//
// Revision 1.10  2001/02/23 20:09:36  dmitry
// Replaced char ** array by std::string
//
// Revision 1.9  2001/02/12 17:10:34  jeff
// spelling in error message
//
// Revision 1.8  2001/01/19 18:21:02  ingmar
// print cosmetics
//
// Revision 1.7  2000/12/19 20:38:33  dave
// added some stuff
//
// Revision 1.6  2000/09/11 02:52:51  ingmar
// fixed copyright typo
//
// Revision 1.5  2000/09/11 02:47:38  ingmar
// code beautify
//
// Revision 1.4  2000/09/11 02:41:42  ingmar
// added description
//
// Revision 1.3  2000/09/11 02:38:24  ingmar
// added date
//
// Revision 1.2  2000/09/11 02:03:08  ingmar
// added author
//
// Revision 1.1.1.1  2000/08/11 22:15:50  ingmar
// unit tests complete
//
//
// 7     9/07/00 11:27a Lihong
// added destructor to delete allocated memory
//
// 6     8/29/00 12:29p Antonio
// updated header
//
// 5     8/17/00 1:39p Antonio
// modifed header
//
// *****************  Version 4  *****************
// User: Antonio      Date: 8/11/00    Time: 10:07a
// Updated in $/vx/src/Utils
// updated header of file
//
// *****************  Version 3  *****************
// User: Antonio      Date: 8/02/00    Time: 9:57p
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
// User: Ingmar          Date:  7/19/00  Time:  9:04a
// Created CmdLineSwitches.C
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/CmdLineSwitches.C,v 1.2 2005/07/01 12:30:07 vxconfig Exp $
// $Id: CmdLineSwitches.C,v 1.2 2005/07/01 12:30:07 vxconfig Exp $
