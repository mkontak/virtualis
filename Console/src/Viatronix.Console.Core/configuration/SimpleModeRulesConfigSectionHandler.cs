// $Id: SimpleModeRulesConfigSectionHandler.cs,v 1.1.2.4 2008/01/16 14:54:09 mkontak Exp $
//
// Copyright © 20XX, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]


using System;
using System.Configuration;
using System.Collections;
using System.Collections.Specialized;
using System.Collections.Generic;
using System.Text;
using System.Xml;

namespace Viatronix.Console
{

  class SimpleModeRulesConfigSectionHandler : IConfigurationSectionHandler
  {

    #region IConfigurationSectionHandler Members


    /// <summary>
    /// Creates a collection of launch rules from xml.
    /// </summary>
    /// <param name="parent">The configuration settings in a corresponding parent configuration section.</param>
    /// <param name="configContext">null</param>
    /// <param name="section">The XmlNode that contains the configuration information from the configuration file. Provides direct access to the XML contents of the configuration section.</param>
    /// <returns>A collection of launch rules.</returns>
    public object Create(object parent, object configContext, XmlNode section)
    {

      SimpleModeRuleCollection col = new SimpleModeRuleCollection();

      try
      {
        
        // create a launch rule for each config entry
        foreach (XmlNode node in section.SelectNodes("rule"))
        {
          col.Add(SimpleModeRule.FromXml(node));
        }

      }
      catch (Exception e)
      {
        Viatronix.Logging.Log.Error("Console was unable to load the simple mode rules hints. [ERROR=" + e.Message + "]", "SimpleModeRulesConfigSectionHandler", "Create");
      }

      return col;

    } // Create( parent, configContext, section )



    #endregion

  }
}


#region revision history

// $Log: SimpleModeRulesConfigSectionHandler.cs,v $
// Revision 1.1.2.4  2008/01/16 14:54:09  mkontak
// no message
//
// Revision 1.1.2.3  2008/01/10 18:30:24  mkontak
// Added new features
//
// Revision 1.1.2.2  2008/01/09 20:09:20  mkontak
// no message
//
// Revision 1.1.2.1  2008/01/04 15:30:15  mkontak
// no message
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console.Core/misc/SimpleModeRulesConfigSectionHandler.cs,v 1.1.2.4 2008/01/16 14:54:09 mkontak Exp $
// $Id: SimpleModeRulesConfigSectionHandler.cs,v 1.1.2.4 2008/01/16 14:54:09 mkontak Exp $

#endregion
