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
using System.Collections.ObjectModel;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Viatronix.Utilities.BackupAuditViewer
{
  /// <summary>
  /// Handles the sections that house search terms for conveinience.
  /// </summary>
  public class SearchTermsSectionHandler : IConfigurationSectionHandler
  {

    #region IConfigurationSectionHandler implementation

    /// <summary>
    /// Handles the section.
    /// </summary>
    /// <param name="parent">The parent node.</param>
    /// <param name="configContext">The context.</param>
    /// <param name="configSection">The section xml.</param>
    /// <returns>A list of terms that can be used in the search.</returns>
    public object Create(object parent, object configContext, XmlNode configSection)
    {
      ObservableCollection<string> terms = new ObservableCollection<string>();

      foreach (XmlNode node in configSection.ChildNodes)
      {
        terms.Add(node.InnerText);
      }

      return terms;
    } // Create( parent, configContext, configSection)

    #endregion

  } // class SearchTermsSectionHandler
} // namespace Viatronix.Utilities.BackupAuditViewer
