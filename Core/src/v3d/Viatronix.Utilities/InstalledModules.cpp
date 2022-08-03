// $Id: InstalledModules.cpp,v 1.7 2005/08/05 11:40:44 mkontak Exp $
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Utilities/InstalledModules.cpp,v 1.7 2005/08/05 11:40:44 mkontak Exp $
//
// Copyright© 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Mark Kontak mkontak@viatronix.net


// Includes
#include "Stdafx.h"
#include "InstalledModules.h"


// Namespaces
using namespace Microsoft;
using namespace System;
using namespace System::Data;
using namespace System::Windows::Forms;

USING_REGISTRY_NS
USING_TYPES_NS



/**
 *  Installed Modules Construtor
 */
InstalledModules::InstalledModules() :
/************************************************************************************************************************/
m_sComputer(String::Empty),
m_bDisposed(false)
{

  Init();

  Read();


} // InstalledModules

/**
*  Installed Modules Construtor
*
* @param sSystem          System
* @param sCompanyName     Name of company used for SOFTWARE\Company
*/
InstalledModules::InstalledModules(String * sSystem ) :
/************************************************************************************************************************/
m_sComputer(sSystem),
m_bDisposed(false)
{

  Init();

  Read();


} // InstalledModules


/**
 * Destructor
 */
InstalledModules::~InstalledModules()
{
  Dispose(false);
} // ~InstalledModules();

/**
 * Disposing of the pipeline object.
 *
 * @return void.
 */
void InstalledModules::Dispose()
{
  Dispose(true);
  System::GC::SuppressFinalize(this);
} // Dispose()

/**
 * Dispose of object.
 *
 * @param  bDisposing     Flag to indicate if the object is being disposed or detroyed.
 *
 * @return void.
 */
void InstalledModules::Dispose(bool bDisposing)
{

  //////////////////////////////////////////////////////////
  // Make sure the object has not already been disposed
  //////////////////////////////////////////////////////////
  if ( ! m_bDisposed )
  {

    ////////////////////////////////////////
    // If we being disposed not deleted
    ////////////////////////////////////////
    if ( bDisposing )
    {
      // Do Nothing
    }

    //////////////////////////////////////////////////
    // If we created the dataset then dispose of it
    /////////////////////////////////////////////////
    if ( m_pDataset != NULL )
    {
      m_pDataset->Dispose();
    } // END ... If the dataset was created


  } // END ... If the object has not already been disposed
} // Dispose()

/**
 *
 */
void InstalledModules::Init()
{

  m_pDataset = new System::Data::DataSet();

  /////////////////////////////////////
  // make sure the new did not fail
  ////////////////////////////////////
  if ( m_pDataset == NULL )
  {
    throw new OutOfMemoryException(L"Could not allocate new DataSet");
  } // END ... IIf the new failed

   // Add table to dataset
  System::Data::DataTable * pTable = m_pDataset->Tables->Add(m_sTableName);


  // Set up primary KEY
  System::Data::DataColumn * PrimaryKey[] = new System::Data::DataColumn * [1];
  System::Data::DataColumn * pColumn  = pTable->Columns->Add(L"SUB_TYPE", Type::GetType(L"System.String"));
  pColumn->Unique = true;
  PrimaryKey[0] = pColumn;
  

  // Set up Schema
  pTable->Columns->Add(L"SORT_BY_IMAGE", Type::GetType(L"System.Boolean"))->DefaultValue = false;
  pTable->Columns->Add(L"EXCLUDE_UID", Type::GetType(L"System.Boolean"))->DefaultValue = false;
  pTable->Columns->Add(L"EXCLUDE_SCAN_TIME",Type::GetType(L"System.Boolean"))->DefaultValue = false;
  pTable->Columns->Add(L"DEFAULT", Type::GetType(L"System.Boolean"))->DefaultValue = false;
  pTable->Columns->Add(L"INSTALLED", Type::GetType(L"System.Boolean"))->DefaultValue = false;

  pTable->PrimaryKey = PrimaryKey;

  // Set up the default view
  pTable->DefaultView->AllowDelete = true;
  pTable->DefaultView->AllowEdit = true;
  pTable->DefaultView->AllowNew = true;
  pTable->DefaultView->Sort = L"SUB_TYPE";
  pTable->DefaultView->ApplyDefaultSort = true;
 
    ///Local machine key
  Microsoft::Win32::RegistryKey * pLocalMachineKey(GetLocalMachineKey(m_sComputer));

  Microsoft::Win32::RegistryKey * pInstalledKey(NULL);
  try
  {
    pInstalledKey = pLocalMachineKey->OpenSubKey(m_sInstalledModulesSubKey, true);

    if ( pInstalledKey == NULL )
    {
      pInstalledKey = pLocalMachineKey->CreateSubKey(m_sInstalledModulesSubKey);
      if ( pInstalledKey == NULL )
      {
        throw new Exception(String::Concat("Could not create subkey ", m_sInstalledModulesSubKey));
      }
    }
  }
  catch ( Exception *  )
  {
    throw;
  }
  __finally 
  {
    pInstalledKey->Close();
    CloseLocalMachineKey(m_sComputer, pLocalMachineKey);
  }

} // Init()

/**
 * Adds modules to list
 *
 * @param pModule           Refernce to module to add to list.
 *
 * @return Returns the index the module resides at in the list.
 */
int InstalledModules::Add(Viatronix::v3D::Utilities::Registry::InstalledModule * pModule)
{

  ////////////////////////////////////////
  // Check if the module already exists
  ///////////////////////////////////////
  if ( List->Contains(pModule) )
  {
    throw new System::Exception("Module already exist");
  } // END ... If module already exists

  return List->Add(pModule); 

} // Add()

/**
 * Returns true of the list contains the supplied module
 *
 * @param pModule     Reference to module to check.
 *
 * @return Returns true if the modules already exists in the list. 
 */
bool InstalledModules::Contains(Viatronix::v3D::Utilities::Registry::InstalledModule * pModule) 
{ 

  bool bReturn(false);

  ////////////////////////////////////////////
  // Look for a duplicate module in the list
  ////////////////////////////////////////////
  for ( int i(0); i < Count; i++ )
  {
    Viatronix::v3D::Utilities::Registry::InstalledModule * pMod = dynamic_cast<Viatronix::v3D::Utilities::Registry::InstalledModule *>(Item[i]);

    if ( String::Compare(pMod->SubType,pModule->SubType) == 0 )
    {
      bReturn = true;
      break;
    } // END ... If the module is the same

  } // END ... For each module in the list

  return bReturn; 
} // Contains()


/**
 * Updates the dataset to the collection
 */
void InstalledModules::Read()
{

  // Clears the list
  List->Clear();


  // Clears the dataset
  System::Data::DataTable * pTable = m_pDataset->Tables->get_Item(m_sTableName);

  /// Clear the rows
  pTable->Rows->Clear();


  ///Local machine key
  Microsoft::Win32::RegistryKey * pLocalMachineKey(GetLocalMachineKey(m_sComputer));


  try
  {


    ////////////////////////////////
    // Populate the internal list
    ///////////////////////////////
    for ( int i(1); i < 100 ; i++ )
    {

      String * sSubKey(String::Format("{0}\\Module{1}",m_sInstalledModulesSubKey,__box(i)));
      
      Microsoft::Win32::RegistryKey * pModule(pLocalMachineKey->OpenSubKey(sSubKey));

      ///////////////////////////////////
      // Make sure the section exists
      ///////////////////////////////////
      if ( pModule != NULL  )
      {
        try
        {
          String * sSubType(pModule->GetValue("SubType", String::Empty)->ToString());
          bool bSortByImage(ToBoolean(pModule->GetValue("SortBy", new String(L"0"))->ToString()->Trim()));
          bool bExcludeUID(ToBoolean(pModule->GetValue("ExcludeUIDInPath", new String(L"0"))->ToString()->Trim()));
          bool bExcludeScanTime(ToBoolean(pModule->GetValue("ExcludeScanTime", new String(L"0"))->ToString()->Trim()));
          bool bDefault(ToBoolean(pModule->GetValue("Default", new String(L"0"))->ToString()->Trim()));       
          
          // Add the new installed module object to the list
          Add(new Viatronix::v3D::Utilities::Registry::InstalledModule(sSubType, bSortByImage, bExcludeScanTime, bExcludeUID, bDefault));

          Data::DataRow * pRow = pTable->NewRow();
          pRow->set_Item(L"SUB_TYPE", sSubType);
           pRow->set_Item(L"SORT_BY_IMAGE", __box(bSortByImage)); 
          pRow->set_Item(L"EXCLUDE_UID", __box(bExcludeUID));
          pRow->set_Item(L"EXCLUDE_SCAN_TIME",__box(bExcludeScanTime));
          pRow->set_Item(L"DEFAULT", __box(bDefault) );
          pRow->set_Item(L"INSTALLED", __box(true));

         
          pTable->Rows->Add(pRow);

        }
        catch ( Exception * )
        {
          throw;
        }
        __finally
        {
          pModule->Close();
        }

      } // END .... If the SUBKEY exists
      else
      {
        break;
      }
      
    } // END ... for each installed module in the registry

  }
  catch ( Exception * )
  {
    throw;
  }
  __finally
  {
    CloseLocalMachineKey(m_sComputer, pLocalMachineKey);
  }

} // Read()

/**
 * Reads the installed modules from the registry
 */
void InstalledModules::Update(DataSet * pInstalledModulesDataset)
{

  // Clears the list
  List->Clear();

  System::Data::DataTable * pTable = pInstalledModulesDataset->Tables->get_Item(m_sTableName);


  ////////////////////////////////
  // Populate the internal list
  ///////////////////////////////
  for ( int i(0); i < pTable->Rows->Count; i++ )
  {

    Data::DataRow * pRow = pTable->Rows->Item[i];

    bool bInstalled = ( System::Convert::ToBoolean(pRow->Item[L"INSTALLED"]));

    if ( bInstalled )
    {
        
      String * sSubType(pRow->Item[L"SUB_TYPE"]->ToString());
      bool bSortByImage(System::Convert::ToBoolean(pRow->Item[L"SORT_BY_IMAGE"]));
      bool bExcludeUID(System::Convert::ToBoolean(pRow->Item[L"EXCLUDE_UID"]));
      bool bExcludeScanTime(System::Convert::ToBoolean(pRow->Item[L"EXCLUDE_SCAN_TIME"]));
      bool bDefault( System::Convert::ToBoolean(pRow->Item[L"DEFAULT"]));
   
    

      // Add the new installed module object to the list
      Add(new Viatronix::v3D::Utilities::Registry::InstalledModule(sSubType, bSortByImage, bExcludeScanTime, bExcludeUID, bDefault));

    } // END ... Only add if installed
        
  } // END ... for each installed module in the registry

} // Read()


/** 
 * Returns the registry key for the local machine
 */
Microsoft::Win32::RegistryKey * InstalledModules::GetLocalMachineKey(String * sComputer)
{
  Microsoft::Win32::RegistryKey * pLocalMachine = Microsoft::Win32::Registry::LocalMachine;

  if ( sComputer->Length > 0 && String::Compare(sComputer->ToUpper(),System::Windows::Forms::SystemInformation::ComputerName->ToUpper()) != 0 )
  {
    pLocalMachine = Microsoft::Win32::RegistryKey::OpenRemoteBaseKey(Microsoft::Win32::RegistryHive::LocalMachine, sComputer);
  }

  return pLocalMachine;

} // GetLocalMachine()


/** 
 * Closes the specified key
 */
void InstalledModules::CloseLocalMachineKey(String * sComputer, Microsoft::Win32::RegistryKey * pKey)
{
  if ( sComputer->Length > 0 && String::Compare(sComputer->ToUpper(),System::Windows::Forms::SystemInformation::ComputerName->ToUpper()) != 0 )
  {
    pKey->Close();
  } 

} // CloseKey()

/**
 * Writes the installed modules to the registry
 */
void InstalledModules::Write()
{


  Microsoft::Win32::RegistryKey * pLocalMachineKey = GetLocalMachineKey(m_sComputer);


  try
  {

    ////////////////////////////////
    // Delete any module subkeys
    ///////////////////////////////
    for ( int i(1); i < 100 ; i++ )
    { 
      try
      {
        pLocalMachineKey->DeleteSubKey(String::Format("{0}\\Module{1}",m_sInstalledModulesSubKey,__box(i)));
      }
      catch ( System::ArgumentException * )
      {
        break;
      }
    } // END ... For each installed module in the registry
   

    for ( int i(0); i < Count; i++ )
    {
      Microsoft::Win32::RegistryKey * pModuleKey(NULL);
      String * sSubKey(String::Empty);

      try
      {

        sSubKey = String::Format("{0}\\Module{1}",m_sInstalledModulesSubKey,__box(i + 1));
        pModuleKey = pLocalMachineKey->CreateSubKey(sSubKey);

        if ( pModuleKey != NULL )
        {
          Viatronix::v3D::Utilities::Registry::InstalledModule * pModule = dynamic_cast<Viatronix::v3D::Utilities::Registry::InstalledModule *>(Item[i]);

          pModuleKey->SetValue(L"SubType", pModule->SubType);
          pModuleKey->SetValue(L"SortBy", System::Convert::ToString(pModule->SortByImage) );
          pModuleKey->SetValue(L"ExcludeUIDInPath",System::Convert::ToString(pModule->ExcludeUIDInPath) );
          pModuleKey->SetValue(L"ExcludeScanTime",System::Convert::ToString(pModule->ExcludeScanTime) );
          pModuleKey->SetValue(L"Default", System::Convert::ToString(pModule->Default) );
        }
        else
        {
          Viatronix::v3D::Logging::Logger::Wrn(String::Concat("Could not create sub key ", sSubKey));
        }
      }
      catch ( Exception * ex)
      {
        Viatronix::v3D::Logging::Logger::Wrn(String::Concat("Could not create sub key ", sSubKey, " : ", ex->Message));
      }
      __finally
      {
        if ( pModuleKey != NULL )
        {
          pModuleKey->Flush();
          pModuleKey->Close();
        }
      }

    } // END ... For each module in list

  }
  catch ( Exception * )
  {
    throw;
  }
  __finally
  {
    CloseLocalMachineKey(m_sComputer, pLocalMachineKey);
  }

} // Write()

/**
 * Sets the Default module using the study type
 *
 * @param eStudyType        Study type to set as default.
 *
 * @return void.
 */
void InstalledModules::SetDefaultModule(String * sSubType)
{

  /////////////////////////////////////////////////////////////////////
  // Search though all the installed modules looking for the 
  // first to be flagged as the default. Since there is only one 
  // default this should be fine.
  ////////////////////////////////////////////////////////////////////
  for ( int i(0); i < Count; i++ )
  {

    Viatronix::v3D::Utilities::Registry::InstalledModule * pModule = dynamic_cast<Viatronix::v3D::Utilities::Registry::InstalledModule *>(Item[i]);

    pModule->Default = false;

    if ( String::Compare(pModule->SubType, sSubType,true) == 0 )
    {
      pModule->Default = true;
    }

  } // END ... for each installed module in the list


} // SetDefaultModule()


/**
 *  Property to return the default module
 */
InstalledModule * InstalledModules::get_DefaultModule()
{

  Viatronix::v3D::Utilities::Registry::InstalledModule * pReturn = NULL;

  /////////////////////////////////////////////////////////////////////
  // Search though all the installed modules looking for the 
  // first to be flagged as the default. Since there is only one 
  // default this should be fine.
  ////////////////////////////////////////////////////////////////////
  for ( int i(0); i < Count; i++ )
  {

    Viatronix::v3D::Utilities::Registry::InstalledModule * pModule = dynamic_cast<Viatronix::v3D::Utilities::Registry::InstalledModule *>(Item[i]);

    /////////////////////////////////////////////
    // If the module is the default we are done
    /////////////////////////////////////////////
    if ( pModule->Default )
    {
      pReturn = pModule;
      break;
    } // END ... If the module is the default

  } // END ... for each installed module in the list

  return(pReturn);

} // get_DefaultModule()



/**
 * Returns the installed module associated with the study type id specified
 *
 * @param  iStudyType   Study type id to look for
 *
 * @return  The installed module that is associated with the study type id
 *          specified.
 */
Viatronix::v3D::Utilities::Registry::InstalledModule * InstalledModules::GetInstalledModule(String * sSubType)
{
  Viatronix::v3D::Utilities::Registry::InstalledModule * pReturn = NULL;

  /////////////////////////////////////////////////////////////////////
  // Search all installed modules for the the one associated with 
  // the specified study type id
  ////////////////////////////////////////////////////////////////////
  for ( int i(0); i <Count ; i++ )
  {
    Viatronix::v3D::Utilities::Registry::InstalledModule * pModule = dynamic_cast<Viatronix::v3D::Utilities::Registry::InstalledModule *>(List->get_Item(i));

    if ( String::Compare(pModule->SubType, sSubType, true) == 0 )
    {
      pReturn = pModule;
      break;
    } // END .. If the study type id is found in the list

  } // END ... for each installed module in the list

  return(pReturn);

} // GetInstalledModule()

// $Log: InstalledModules.cpp,v $
// Revision 1.7  2005/08/05 11:40:44  mkontak
// Deleted old logger include lines
//
// Revision 1.6  2005/07/01 12:39:08  vxconfig
// Added new Logger.
//
// Revision 1.5  2005/02/22 14:23:09  mkontak
// Modified to use strings instead of ints
//
// Revision 1.4  2004/09/10 18:05:23  mkontak
// Moved to SystemSettings and usage of System::Microsoft::Win32::Registry
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
// Revision 1.6  2003/10/31 13:24:34  mkontak
// Added remote registry access
//
// Revision 1.5  2003/10/30 14:30:29  mkontak
// Copied the ENUMS from BRL to utilities, added GetStudyPath to File and added
// some properties to DNSRegistery
//
// Revision 1.4  2003/07/22 18:34:39  mkontak
// Fixes for configuration
//
// Revision 1.3  2003/06/20 18:33:08  mkontak
// Coding standards
//
// Revision 1.2  2003/05/12 14:46:07  mkontak
// Cosmetic
//
// Revision 1.1  2003/05/05 12:24:16  mkontak
// Reorganized registry added new funtionality
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Utilities/InstalledModules.cpp,v 1.7 2005/08/05 11:40:44 mkontak Exp $
// $Id: InstalledModules.cpp,v 1.7 2005/08/05 11:40:44 mkontak Exp $
