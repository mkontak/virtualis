// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: DatabaseServiceProvider.cs
//
// Description: 
//
// Owner: Mark Kontak  (mkontak@viatronix.com)

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using Viatronix.Enterprise.Data;
using System.Reflection;
using System.IO;
using Viatronix.Enterprise.Messaging;

namespace Viatronix.Enterprise.Providers
{

  /// <summary>
  /// Service Resource Provider
  /// 
  /// Provider interface to service resource in the V3D system.
  /// 
  /// </summary>
  public class ServiceResourceProvider : ResourceProviderBase
  {


    /// <summary>
    /// Gets all Viatronix databases from the system
    /// </summary>
    /// <returns></returns>
    private string GetDatabase(RequestMessage request)
    {
      return Viatronix.Enterprise.Data.SystemGateway.Get(request.ContextId, "database", XmlBuilder.Create("database"), request.Source);
    } // GetDatabases()

    /// <summary>
    /// Gets all the entities from the table specified
    /// </summary>
    /// <param name="table"></param>
    /// <returns></returns>
    private string GetService(RequestMessage request)
    {
      string entities = string.Empty;

      XmlDocument doc = new XmlDocument();

      doc.LoadXml(request.Payload);


      string table = doc.DocumentElement.Attributes["tbl"].Value;
      string database = doc.DocumentElement.Attributes["db"].Value;

      if (string.Compare(database, "System", true) == 0)
        entities = Viatronix.Enterprise.Data.SystemGateway.Get(request.ContextId, "service", request.Payload, request.Source);
      else if (string.Compare(database, "Auditing", true) == 0)
        entities = Viatronix.Enterprise.Data.AuditingGateway.Get(request.ContextId, "service", request.Payload, request.Source);
      else if (string.Compare(database, "Storage", true) == 0)
        entities = Viatronix.Enterprise.Data.StorageGateway.Get(request.ContextId, "service", request.Payload, request.Source);
      else if (string.Compare(database, "Security", true) == 0)
        entities = Viatronix.Enterprise.Data.SecurityGateway.Get(request.ContextId, "service", request.Payload, request.Source);
      else if (string.Compare(database, "Licensing", true) == 0)
        entities = Viatronix.Enterprise.Data.LicensingGateway.Get(request.ContextId, "service", request.Payload, request.Source);
      else
        throw new NotSupportedException(string.Format("Access to {0} is not supported", database));


      return entities;

    } // GetTable()

  }

} // namespace Viatronix.Enterprise
