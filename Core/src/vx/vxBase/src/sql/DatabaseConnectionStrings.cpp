// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: SqlCommand.cpp
//
// Owner: Mark Kontak  mailto:mkontak@viatronix.com

// includes
#include "stdafx.h"
#include "sql\DatabaseConnectionStrings.h"
#include "crypto\AESCrypt.h"


using namespace sql;

std::map<std::string, ConnectionStringCollection > DatabaseConnectionStrings::m_databaseConnectionStrings;


#define Attribute_ConnectionStrings           "connectionStrings"
#define Attribute_ConnectionStrings_Name      "name"
#define Attribute_ConnectionStrings_Server    "server"
#define Attribute_ConnectionStrings_Client    "client"
#define Attribute_ConnectionString            "connectionString"
#define Attribute_ConnectionString_Name       "name"
#define Attribute_ConnectionString_Value      "value"


DatabaseConnectionStrings::DatabaseConnectionStrings()
{

}

/**
 * Gets the specific databases connection string
 * 
 * @param sDatabase     Database name (lower case)
 * @param sApplication  Application (default is "default")
 *
 * @return connection string
 */
std::string DatabaseConnectionStrings::GetConnectionString(const std::string & sDatabase, const std::string & sApplication)
{
  ConnectionStringCollection collection = GetConnectionStrings(sApplication);

  ConnectionStringCollection::iterator iter = collection.find(sDatabase);

  if (iter == collection.end())
    throw ex::SqlNotFoundException(LogRec(util::Strings::Format("Database connection for %s was not found", sDatabase.c_str()), "DatabaseConnectionStrings", "GetConnectionString"));

  return iter->second; 
}

/**
 * Gets the application specific connection strings, will default to "default"
 *
 * @param sApplication    Application name
 */
ConnectionStringCollection & DatabaseConnectionStrings::GetConnectionStrings(const std::string & sApplication)
{
  BuildDatabaseConnectionStrings();

  std::map<std::string, ConnectionStringCollection >::iterator iter = m_databaseConnectionStrings.find(sApplication);

  if (iter == m_databaseConnectionStrings.end())
    throw ex::SqlNotFoundException(LogRec(util::Strings::Format("Database connection strings application group %s was not found", sApplication.c_str()), "DatabaseConnectionStrings", "GetConnectionStrings")); 
  
  return iter->second;
}

/**
 * Builds the database connection string from the file
 *
 */
void DatabaseConnectionStrings::BuildDatabaseConnectionStrings()
{
 


  if (m_databaseConnectionStrings.empty())
  {

      std::map<std::string, ConnectionStringCollection> connectionStrings;

      std::string filePath = util::IO::Combine(util::IO::GetDefaultViatronixDirectory(), "config\\connections.config");

      xml::XmlDocument doc;
      doc.LoadFile(filePath);

      std::string documentName = doc.GetDocumentElement().GetName();



      xml::XmlNodeList list = doc.GetDocumentElement().SelectNodes(Attribute_ConnectionStrings);


      for (xml::XmlNodeList::iterator it = list.begin(); it != list.end(); it++)
      {

        std::string name = "default";
        (*it).TryGetAttributeValue(Attribute_ConnectionStrings_Name, name);


        std::string server;
        if (!(*it).TryGetAttributeValue(Attribute_ConnectionStrings_Server, server))
          throw ex::InvalidDataException(LogRec("No server attribute found in connection strings element", "SqlConnectionStrings", "GetConnectionStrings"));

        std::string client = "10.0";
        (*it).TryGetAttributeValue(Attribute_ConnectionStrings_Client, client);


        ConnectionStringCollection collection;

        xml::XmlNodeList list = (*it).SelectNodes(Attribute_ConnectionString);

        std::for_each(list.begin(), list.end(), [&collection, server, client](xml::XmlNode cs)
        {
          std::string name;
          if (!cs.TryGetAttributeValue(Attribute_ConnectionString_Name, name))
            throw ex::InvalidDataException(LogRec("No name attribute found in connection string element", "SqlConnectionStrings", "GetConnectionStrings"));

          std::string value;
          if (!cs.TryGetAttributeValue(Attribute_ConnectionString_Value, value))
            throw ex::InvalidDataException(LogRec("No value attribute found in connection string element", "SqlConnectionStrings", "GetConnectionStrings"));

          std::string text = util::Strings::Format("Driver={SQL Server Native Client %s};", client.c_str()) + crypto::AESCrypt::Decrypt(value) + server + ";";


          collection.insert(std::pair<std::string, std::string>(name, text));

        });

        m_databaseConnectionStrings.insert(std::pair<std::string, ConnectionStringCollection>(name, collection));



      } // END ... For each node in the list



  }

}

