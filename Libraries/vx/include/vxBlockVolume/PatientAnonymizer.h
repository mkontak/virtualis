// $Id: PatientAnonymizer.h,v 1.2 2006/01/31 14:03:32 frank Exp $
//
// Copyright © 2000-2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: David dave@viatronix.com


// pragmas
#pragma once


// includes
#include "PersonName.h"


// forward declarations
class ex::IOException;


/**
 * Class to determine anonymous values for patient ID & name.
*/
class PatientAnonymizer
{
public:

  /// Constructor
  PatientAnonymizer(const PersonName & name, const std::string & sPatientID, const std::string & sInstitution,int,int);

  /// Returns the anonymous name
  const PersonName  & GetAnonymousName() const;
  
  /// Returns the anonymous ID
  const std::string & GetAnonymousID() const;
  
  /// Stores something to the speceified file
  void WriteToFile(std::string sFileName = "W:/db/patientIdentities.txt") const;

// functions
private: 
  
  /// does the work
  PersonName AnonymizeName(int,int) const;

private:
  
  // anonymous values kept so they don't need to be generated again in WriteToFile()
  PersonName  m_name, m_nameAnon;
  
  std::string m_sPatientID, m_sPatientIDAnon;
  
  std::string m_sInstitution;

};


// $Log: PatientAnonymizer.h,v $
// Revision 1.2  2006/01/31 14:03:32  frank
// code review
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.2  2003/05/16 11:53:11  michael
// code review
//
// Revision 3.1  2001/12/24 16:46:19  ingmar
// added class Sec_30_volume to structure the doxygen documentation
//
// Revision 3.0  2001/10/14 23:02:21  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Oct 11 2001 14:50:40   sachin
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
// Revision 1.10  2001/05/18 16:18:06  jmeade
// It's anonYmous, not anonomous or anonoymous or anonoumus or et al.
//
// Revision 1.9  2001/04/19 20:59:05  dave
// updated variable names
//
// Revision 1.8  2001/04/12 18:56:39  ingmar
// vx::Exception -> ex::Exception
//
// Revision 1.7  2001/04/12 18:10:50  dmitry
// modified base class for documentation
//
// Revision 1.6  2001/04/12 15:50:51  dave
// added namespace vx for exceptions
//
// Revision 1.5  2001/04/10 17:58:50  dmitry
// Removed Sec_8_volume Temporarily
//
// Revision 1.4  2001/04/08 22:04:17  ingmar
// prepended Sec_ to documentation section dummy classes
//
// Revision 1.3  2001/04/08 21:38:22  dave
// updated doc headers
//
// Revision 1.2  2001/04/03 05:46:59  soeren
// added documentation
//
// Revision 1.1  2001/03/12 18:48:36  dave
// added PatientAnonymizer and DirectoryAnonymizer
//
// $Id: PatientAnonymizer.h,v 1.2 2006/01/31 14:03:32 frank Exp $