// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: DcmHostTable.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

// includes
#include "Stdafx.h"
#include "DcmHostTable.h"
#include "DcmExceptions.h"
#include "DcmLog.h"
#include "DcmUtility.h"
#include "DcmHost.h"
#include "sql\SqlCommand.h"
#include "sql\SqlConnection.h"
#include "sql\SqlHandle.h"
#include "sql\SqlError.h"
#include "sql\SqlParameter.h"
#include "sql\DatabaseConnectionStrings.h"
#include "crypto\AESCrypt.h"


// sing
using namespace dcm;




#pragma region Create Methods



/**
 * Creates the host table using the file specified
 *
 * @param sHostFilePath     Host file
 *
 * @return Host table
 */
std::shared_ptr<DcmHostTable> DcmHostTable::Create(const std::string & sXml)
{


 // Create the table
  std::shared_ptr<DcmHostTable> pHostTable(new DcmHostTable());

  // Make sure the allocation was successful
  if ( pHostTable == __nullptr )
    throw DcmOutOfMemoryException(LogRec("Failed to allocate DcmHostTable","DcmHostTable","Create"));

  pHostTable->Load(sXml);

  return pHostTable;

} // Create(const std::string & sHostFile)



/**
 * Creates the host table using the file specified
 *
 * @param pNode     Xml node ptr
 *
 * @return Host table
 */
std::shared_ptr<DcmHostTable> DcmHostTable::Create(const xml::XmlNode & node)
{
  // Create the table
  std::shared_ptr<DcmHostTable> pHostTable(new DcmHostTable());

  // Make sure the allocation was successful
  if ( pHostTable == __nullptr )
    throw DcmOutOfMemoryException(LogRec("Failed to allocate DcmHostTable","DcmHostTable","Create"));


  pHostTable->Load(node);

  return pHostTable;

} // Create(const xml::XmlNode & node)


#pragma endregion




/**
 * Returns the list of hosts that support the specified service
 *
 * @param eSupport    Service to check
 *
 * @return List of hosts that supports the specified service
 */
std::vector<std::shared_ptr<DcmHost>> DcmHostTable::GetHosts(DcmServiceTypes eSupports)
{
  std::vector<std::shared_ptr<DcmHost>> list;

  // ===========================
  // Loop through all the hosts
  // ===========================
  std::for_each ( m_hostsByName.begin(), m_hostsByName.end(), [ &list, &eSupports ] ( std::pair<std::string, std::shared_ptr<DcmHost>> item )
  {
    if ( item.second->Supports(eSupports) )
      list.push_back(item.second);
  });

  return list;

} // GetHosts(DcmServiceTypes eSupports)



/**
 * Loads the xml into the object
 *
 * @param sXml    XML
 */
void DcmHostTable::Load(const std::string & sXml)
{
  xml::XmlDocument document;

  document.LoadXml(sXml);
    
  Load(document.GetDocumentElement());


} // 


/**
 * Loads the xml into the object
 *
 * @param node    xml node
 */
void DcmHostTable::Load(const xml::XmlNode & node)
{
  // Make sure the node was valid
  if ( ! node.IsValid() )
    throw DcmInvalidDataException(LogRec("Invalid node received, cannot create host table","DcmHostTable","Create"));

 
  // Get all hosts
  std::vector<xml::XmlNode> hosts = const_cast<xml::XmlNode &>(node).SelectNodes("dicomhost");

  // Build the host table
  std::for_each( hosts.begin(), hosts.end(), [ this ] ( xml::XmlNode host ) 
  {
    AddHost(DcmHost::Create(host));
  } );

} // Load(const xml::XmlNode & node)


/**
 * Adds host to the list. Name and AETitle must be unique.
 *
 *  @param pHost
 *  @return pHost
 */
std::shared_ptr<DcmHost> & DcmHostTable::AddHost(std::shared_ptr<DcmHost> & pHost)
{

  // Make sure the name is not empty
  if ( pHost->GetName().empty() )
    throw DcmException(LogRec("Cannot add host with no name","DcmHostTable","AddHost"));

  // Make sure a host with the same name does not already exist
  if ( m_hostsByName.find(pHost->GetName()) != m_hostsByName.end() )
    throw DcmException(LogRec(util::Strings::Format("Duplicate name in host table was found (%s)", pHost->GetName().c_str()), "DcmHostTable", "AddHost"));


  // Make sure a host with the same aetitle does not already exist
  if ( m_hostsByAETitle.find(pHost->GetAETitle()) != m_hostsByAETitle.end() )
    throw DcmException(LogRec(util::Strings::Format("Duplicate AETitle in host table was found (%s)", pHost->GetAETitle().c_str()), "DcmHostTable", "AddHost"));

  m_hostsByName.insert(std::pair<std::string, std::shared_ptr<DcmHost>>(pHost->GetName(), pHost));
  m_hostsByAETitle.insert(std::pair<std::string, std::shared_ptr<DcmHost>>(pHost->GetAETitle(), pHost));

  return pHost;

} // AddHost(std::shared_ptr<DcmHost> pDcmHost)


#pragma region Get Methods


/**
 * Gets the host specified by the name
 *
 * @param sName   Name to retrieve
 * 
 * @return Host if found
 */
std::shared_ptr<DcmHost> DcmHostTable::GetHostByName(const std::string & sName)
{

 if ( m_hostsByName.find(sName) == m_hostsByName.end() )
  throw DcmNotFoundException(LogRec(util::Strings::Format("Host %s does not exist in the table",sName.c_str()),"DcmHostTable","GetHost"));

 return m_hostsByName[sName];

} // GetHost(const std::string & sName)


/**
 * Gets the host specified by the aeTitle
 *
 * @param sName   Name to retrieve
 * 
 * @return Host if found
 */
std::shared_ptr<DcmHost> DcmHostTable::GetHostByAETitle(const std::string & sAETitle)
{

  if ( m_hostsByAETitle.find(sAETitle) == m_hostsByName.end() )
   throw DcmNotFoundException(LogRec(util::Strings::Format("Host %s does not exist in the table",sAETitle.c_str()),"DcmHostTable","GetHostByAETitle"));

 return m_hostsByAETitle[sAETitle];

} // GetHostByAETitle(const std::string & sName)



/**
 * Gets the host specified by the name 
 *
 * @param sName     [in]  Name to retrieve
 * @param pDcmHost  [out] Return value 
 * 
 * @return true if the host is found, false if not
 */
bool DcmHostTable::TryGetHostByName(const std::string & sName, std::shared_ptr<DcmHost> & pDcmHost )
{
  bool bFound(false);
  pDcmHost = __nullptr;

  if ( m_hostsByName.find(sName) != m_hostsByName.end() )
  {
    bFound = true;
    pDcmHost = m_hostsByName[sName];
  }

  return bFound;

} // TryGetHostByName(const std::string & sName)



/**
 * Gets the host specified by the name 
 *
 * @param sAETitle  [in]  AETitle to retrieve
 * @param pDcmHost  [out] Return value 
 * 
 * @return true if the host is found, false if not
 */
bool DcmHostTable::TryGetHostByAETitle(const std::string & sAETitle, std::shared_ptr<DcmHost> & pDcmHost )
{
  bool bFound(false);
  pDcmHost = __nullptr;


  if ( m_hostsByAETitle.find(sAETitle) != m_hostsByAETitle.end() )
  {
    bFound = true;
    pDcmHost = m_hostsByAETitle[sAETitle];
  }


  return bFound;

} // TryGetHostByAETitle(const std::string & sAETitle)


#pragma endregion


/**->GetValue()_
 * Gets the current host table
 *
 * @param eMode     Mode of the file (CREATE or OPEN)
 *
 * @return Returns the host table
 */
std::shared_ptr<DcmHostTable> & DcmHostTable::GetCurrentHostTable()
{

  std::string contextId = "cbHmZgzHtMUP0s69dPhPei5bYZHW1k7QFyx0oEYkyE8Vqgeb4rY/lpbtIw23fbJJ";
  std::string query = "<dicomhost />";

  static std::shared_ptr<DcmHostTable> table = __nullptr; 

  // If the table has not been creates of.r the mode is different recreate
  if (table == __nullptr)
  {
   
    table = std::shared_ptr<DcmHostTable>(new DcmHostTable());


    std::shared_ptr<sql::SqlConnection> connection = sql::SqlConnection::Create(sql::DatabaseConnectionStrings::GetConnectionString("system"));

    connection->Open();

    std::shared_ptr<sql::SqlCommand> command = sql::SqlCommand::Create("[v3d].[sp_getDicomHostNative]", sql::SqlCommand::CT_StoredProcedure, connection);
    command->AddParameter(sql::SqlInParameter::Create<std::string>("ID", crypto::AESCrypt::Decrypt(contextId)));
    command->AddParameter(sql::SqlInParameter::Create<std::string>("QUERY", query));
    command->AddParameter(sql::SqlOutParameter::Create("RESULTS", 20000, sql::ST_VARCHAR));

    command->Execute();

    std::string results = (char *)command->GetParameter("RESULTS")->GetValue();

    if (!results.empty())
    {
      xml::XmlDocument doc;
      doc.LoadXml(results);

      xml::XmlNodeList list = doc.GetDocumentElement().SelectNodes("dicomhost");

      for ( xml::XmlNodeList::iterator it = list.begin(); it != list.end(); it++ )
      {
        table->AddHost(dcm::DcmHost::Create(*it));
      }
    }


  }

  return table;

} // GetHostTable()

