// $Id: PatientAnonymizer.C,v 1.2 2006/01/31 14:03:32 frank Exp $
//
// Copyright © 2000-2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: David dave@viatronix.com


// includes
#include "stdafx.h"
#include "PatientAnonymizer.h"
#include "Date.h"


// namespaces
using namespace std;


// defines
#define FILE_REVISION  "$Revision: 1.2 $"


/**
 * Constructor
 */
PatientAnonymizer::PatientAnonymizer(const PersonName & name, const std::string & sPatientID, 
                                     const std::string & sInstitution,int SiteNum,int PatNum ) : m_name(name), 
                                     m_sInstitution(sInstitution), m_sPatientID(sPatientID)
{
  m_nameAnon = AnonymizeName( SiteNum,PatNum);
  m_sPatientIDAnon = "xxxx";
}


/** 
 * Returns the anonymous name
 */
const PersonName & PatientAnonymizer::GetAnonymousName() const
{
  return (m_nameAnon);
}


/**
 * Returns the anonymous ID
 */
const string & PatientAnonymizer::GetAnonymousID() const 
{
  return (m_sPatientIDAnon);
} // GetAnonymousID


/** 
 * 
 */
void PatientAnonymizer::WriteToFile(string sFileName) const
{
  ofstream mapFile(sFileName.c_str(), ios::app);  // append mode
  if ( !mapFile ) 
  {
    
    throw ex::IOException( LogRec( "Could not open file", "PatientAnonymizer", "WriteToFile" ) );
  } else 
  {
    mapFile << m_name.GetFullName() <<" = name, ID = "<< m_sPatientID << "  =>  "; 
    mapFile << m_nameAnon.GetFullName() <<" = name, ID = "<< m_sPatientIDAnon<< endl;
  }
} // WriteToFile


/**
 * Returns the anonymous name.
 */
PersonName  PatientAnonymizer::AnonymizeName(int uSiteNum,int uPatNum) const
{
  PersonName newPatientName;
  // get last and first name into a string of length 4*n
  string sName = m_name.GetLast()+m_name.GetFirst();
  while ( sName.length() % 5 != 0 ) 
  {
    sName += "_";
  }
  
  // now turn this into a number within range of CVCVC
  string sVowels = "aiou";   // sometimes Y
  string sConsos = "qtypdfghjklzxvbn";
  uint4 uiRange  =  static_cast< uint4 >( sConsos.length() * sVowels.length() * sConsos.length() * sVowels.length() * sConsos.length() );
  uint4 uindex = 0;
  uint4 n, num = 0;
  for ( uint4 k=0; k<sName.length(); k+=4 ) {
    num = 0;
    n = sName[k+0]; n = n<<(0*8);  num += n;
    n = sName[k+1]; n = n<<(1*8);  num += n;
    n = sName[k+2]; n = n<<(2*8);  num += n;
    n = sName[k+3]; n = n<<(3*8);  num += n;
    num = num % uiRange;
    uindex = (uindex + num) % uiRange;
  }
  
  // and make the CVCV string
  char* cvcvc = "CVCVC";
  uint4 c0 = uindex % sConsos.length();  uindex /= static_cast< uint4 >( sConsos.length() );
  uint4 v1 = uindex % sVowels.length();  uindex /= static_cast< uint4 >( sVowels.length() );
  uint4 c2 = uindex % sConsos.length();  uindex /= static_cast< uint4 >( sConsos.length() );
  uint4 v3 = uindex % sVowels.length();  uindex /= static_cast< uint4 >( sVowels.length() );
  uint4 c4 = uindex % sConsos.length();  uindex /= static_cast< uint4 >( sConsos.length() );
  cvcvc[0] = sConsos[c0];  cvcvc[1] = sVowels[v1];
  cvcvc[2] = sConsos[c2];  cvcvc[3] = sVowels[v3];
  cvcvc[4] = sConsos[c4];  
  
  char tmpStr[20]="";
  
  
  // set the patients first name to the new constructed one
  sprintf(tmpStr, "%04d_%s",uSiteNum,cvcvc);
  newPatientName.SetFirst(tmpStr);
  //std::cout<<tmpStr;
  sprintf(tmpStr, "%03d", uSiteNum);
    // set the patients last name to the new constructed one
  newPatientName.SetLast(tmpStr);
//std::cout<<tmpStr;

  return (newPatientName);
} // AnonymizeName


// $Log: PatientAnonymizer.C,v $
// Revision 1.2  2006/01/31 14:03:32  frank
// code review
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.2  2003/05/16 11:53:11  michael
// code review
//
// Revision 3.1  2001/10/26 02:21:06  tushar
// removed errors due to changes made to logger class, updating again
//
// Revision 3.0  2001/10/14 23:02:21  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Oct 11 2001 14:50:34   sachin
// anonymizer function takes SiteNum and PatNum as parameters
// 
//    Rev 2.0   Sep 16 2001 23:47:38   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:35:06   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:12:20  soeren
// renamed volume  directory
//
// Revision 1.9  2001/07/25 01:40:12  tushar
// no message
//
// Revision 1.8  2001/07/25 01:31:22  tushar
// fixed some compiler errors (logger)
//
// Revision 1.7  2001/05/18 16:18:06  jmeade
// It's anonYmous, not anonomous or anonoymous or anonoumus or et al.
//
// Revision 1.6  2001/04/19 20:59:05  dave
// updated variable names
//
// Revision 1.5  2001/04/12 18:56:39  ingmar
// vx::Exception -> ex::Exception
//
// Revision 1.4  2001/04/12 15:50:51  dave
// added namespace vx for exceptions
//
// Revision 1.3  2001/04/10 17:39:52  dmitry
// updated datatypes
//
// Revision 1.2  2001/04/08 21:38:22  dave
// updated doc headers
//
// Revision 1.1  2001/03/12 18:48:36  dave
// added PatientAnonymizer and DirectoryAnonymizer
//
// $Id: PatientAnonymizer.C,v 1.2 2006/01/31 14:03:32 frank Exp $