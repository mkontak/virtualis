// $Id: InstalledModules.h,v 1.4 2005/02/22 14:23:10 mkontak Exp $
//
// Copyright(c) 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Mark Kontak  mkontak@viatronix.com


// Pragmas
#pragma once

// Include
#include "UtilityEnums.h"

// Namespaces
using namespace System;
using namespace System::Collections;
using namespace System::Collections::Specialized;

OPEN_REGISTRY_NS


/**
 * Implements a single installed module registry entry.
 */
__sealed public __gc class InstalledModule
{
public:

  /// Constructor
  InstalledModule(String * sSubType, bool bSortByImage, bool bExcludeScanTime, 
                  bool bExcludeUID, bool bDefault ) :
                  m_sSubType(sSubType), 
                  m_bSortByImage(bSortByImage), m_bExcludeScanTime(bExcludeScanTime), 
                  m_bExcludeUIDInPath(bExcludeUID),  m_bDefault(bDefault) { }

  /// Constructor
  InstalledModule(String * sSubType ) :
                  m_sSubType(sSubType),  
                  m_bSortByImage(false), m_bExcludeScanTime(false), 
                  m_bExcludeUIDInPath(false), m_bDefault(false) {  }

  /// Destructor
  virtual ~InstalledModule() {  }



public:

  /// Returns the study type id associated with this installed module
  __property String * get_SubType() { return m_sSubType; }

  /// Returns true if the moldue is to be sorted by image number 
  __property bool get_SortByImage() { return(m_bSortByImage); }

  /// Sets the sort by image flag 
  __property void set_SortByImage(bool bSortByImage) { m_bSortByImage = bSortByImage; }

  /// Returns true if the Scan Time is to be excluded from the database
  __property bool get_ExcludeScanTime() { return m_bExcludeScanTime; }

  /// Sets the exclude scan time flag
  __property void set_ExcludeScanTime(bool bExcludeScanTime) { m_bExcludeScanTime = bExcludeScanTime; }

  /// Returns true if the UID is to be excluded from the study path
  __property bool get_ExcludeUIDInPath() { return m_bExcludeUIDInPath;}

  /// Sets the exclude UID in path flag
  __property void set_ExcludeUIDInPath(bool bExcludeUIDInPath) { m_bExcludeUIDInPath = bExcludeUIDInPath; }

  /// Returns true if the module is the defualt
  __property bool get_Default() { return m_bDefault; }

  /// Returns true if the module is the defualt
  __property void set_Default(bool bDefault) { m_bDefault = bDefault; }

private:

  /// SubType
  String * m_sSubType;

  /// installed modules defualt flag
  bool m_bDefault;

  /// installed modules sort by image flag
  bool m_bSortByImage;

  /// installed modules exclude scan time flag
  bool m_bExcludeScanTime;

  /// installed modules exclude UID in path flag
  bool m_bExcludeUIDInPath;


};  // class InstalledModule


/**
  * Implements a list of installed modules through the registry.
  */
public __gc class InstalledModules : public System::Collections::CollectionBase , public System::IDisposable
{
public:

  /// Constructor
  InstalledModules();

  /// Constructor
  InstalledModules(System::String * sComputer);

  /// Destructor
  virtual ~InstalledModules(); 

  /// Sets the default module using the study type
  void SetDefaultModule(String * sSubType);

  /// Returns the installed modules using the study type id
  Viatronix::v3D::Utilities::Registry::InstalledModule * GetInstalledModule(String * sSubType);

  /// Adds the module to the list
  int Add(Viatronix::v3D::Utilities::Registry::InstalledModule * pModule); 

  /// Inserts the module at the position specified
  void Insert(int iIndex, InstalledModule * pModule) { List->Insert(iIndex, pModule); }

  /// Removes the supplied installed module from the list.
  void Remove(Viatronix::v3D::Utilities::Registry::InstalledModule * pModule) { List->Remove(pModule); }

  /// Returns true of the list contains the supplied module
  bool Contains(Viatronix::v3D::Utilities::Registry::InstalledModule * pModule);

  /// Dispose of object
  void Dispose();

  /// Reads the installed modules from the registry
  void Read();

  /// Write the installed modules to the regirsty
  void Write();

  /// Updates the dataset to the list
  void Update(System::Data::DataSet * pInstalledModulesDS);

  /// Update the internal dataset to the list
  inline void Update() { Update(m_pDataset); }

protected:

  /// Dispose of objet
  virtual void Dispose(bool bDisposing);

private:

  /// Converts to a Boolean Value
  __forceinline bool ToBoolean(System::String * sValue) 
  { return ( System::String::Compare(sValue,"1") == 0 || System::String::Compare(sValue->ToLower(), "true") == 0 ? true : false ); }


  /// Returns the local machine
  Microsoft::Win32::RegistryKey * GetLocalMachineKey(System::String * sComputer);

  /// Returns the local machine
  void CloseLocalMachineKey(System::String * sComputer, Microsoft::Win32::RegistryKey * pKey);

  /// Initializes the Dataset
  void Init();

public:

  /// Returns the default installed modules
  __property Viatronix::v3D::Utilities::Registry::InstalledModule * get_DefaultModule();

  /// Returns the installed modules at the specified index
  __property Viatronix::v3D::Utilities::Registry::InstalledModule * get_InstalledModule(int iIndex) { return dynamic_cast<Viatronix::v3D::Utilities::Registry::InstalledModule *>(Item[iIndex]); }

  /// Returns the dataset for the installed modules
  __property System::Data::DataSet * get_Dataset() { return m_pDataset; }

  /// Returns the system
  __property System::String * get_Computer() { return m_sComputer; }

private:

  /// Software Registry (Access to the registry)
  //Viatronix::v3D::Utilities::Registry::SoftwareRegistry * m_pSoftwareRegistry;

  /// Flag to indicate that the object has been disposed
  bool m_bDisposed;
  
  /// Dataset
  System::Data::DataSet * m_pDataset;

  /// Installed Modules
  static String * m_sInstalledModulesSubKey = "SOFTWARE\\Viatronix\\Installed";

  /// DataSet table name
  static String * m_sTableName = "INSTALLED_MODULES";

  /// Computer
  System::String * m_sComputer;
    
};  // class InstalledModules



CLOSE_REGISTRY_NS


// Revision History:
// $Log: InstalledModules.h,v $
// Revision 1.4  2005/02/22 14:23:10  mkontak
// Modified to use strings instead of ints
//
// Revision 1.3  2004/05/11 19:13:18  mkontak
// Changes for the WQ
//
// Revision 1.2  2004/04/20 17:54:46  mkontak
// Chaged the way the installed modules interfaces with the registry
//
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.5  2003/10/31 13:24:34  mkontak
// Added remote registry access
//
// Revision 1.4  2003/10/30 14:30:29  mkontak
// Copied the ENUMS from BRL to utilities, added GetStudyPath to File and added
// some properties to DNSRegistery
//
// Revision 1.3  2003/07/22 18:34:39  mkontak
// Fixes for configuration
//
// Revision 1.2  2003/06/20 18:33:08  mkontak
// Coding standards
//
// Revision 1.1  2003/05/05 12:24:16  mkontak
// Reorganized registry added new funtionality
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Utilities/InstalledModules.h,v 1.4 2005/02/22 14:23:10 mkontak Exp $
// $Id: InstalledModules.h,v 1.4 2005/02/22 14:23:10 mkontak Exp $
