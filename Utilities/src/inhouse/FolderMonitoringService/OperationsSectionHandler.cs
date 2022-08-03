// Copyright © 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Kyle R Chalupa(mailto:kchalupa@viatronix.com)

using System;
using System.Xml;
using System.Configuration;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Viatronix.Operations;

namespace Viatronix
{
  /// <summary>
  /// Lazilly creates operations that should be done on a regular basis.
  /// </summary>
  public class OperationsSectionHandler : IConfigurationSectionHandler
  {

    #region methods

    /// <summary>
    /// Handles the operations section.
    /// </summary>
    /// <param name="parent">The parent section.</param>
    /// <param name="context">The context.</param>
    /// <param name="section">The section we are handling.</param>
    /// <returns>A list of operations that should be performed once a day.</returns>
    public object Create(object parent, object context, XmlNode section)
    {
      List<IFileWatcherOperation> operations = new List<Operations.IFileWatcherOperation>();

      foreach(XmlNode node in section.SelectNodes("operation"))
      {
        operations.Add((IFileWatcherOperation)Activator.CreateInstance(Type.GetType(node.Attributes["type"].Value)));
      }

      return operations;
    } // Create( parent, context, section )

    #endregion

  } // class OperationsSectionHandler
} // namespace Viatronix
