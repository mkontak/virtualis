
using System;
using System.Configuration;
using System.Collections.Generic;
using System.Text;
using System.Xml;

namespace Viatronix.Enterprise.Configuration
{

  /// <summary>
  /// Provides the xml information from the configuration file.
  /// </summary>
  public class XmlConfigSectionHandler : IConfigurationSectionHandler
  {

    #region IConfigurationSectionHandler Members

    /// <summary>
    /// REturns the XMlNode containing the configuration information.
    /// </summary>
    /// <param name="parent">The configuration settings in a corresponding parent configuration section.</param>
    /// <param name="configContext">This parameter is reserved and is a null reference.</param>
    /// <param name="section">The XmlNode that contains the configuration information from the configuration file. 
    /// Provides direct access to the XML contents of the configuration section. </param>
    /// <returns>The XmlNode that contains the configuration information from the configuration file.</returns>
    public object Create( object parent, object configContext, XmlNode section )
    {
      return section;
    } // Create( parent, configContext, section )

    #endregion

  } // class XmlConfigSectionHandler
} // namespace Viatronix.Enterprise.Entities


#region revision history

// $Log: XmlConfigSectionHandler.cs,v $
// Revision 1.1  2006/10/30 15:57:37  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Enterprise.Entities/data/XmlConfigSectionHandler.cs,v 1.1 2006/10/30 15:57:37 gdavidson Exp $
// $Id: XmlConfigSectionHandler.cs,v 1.1 2006/10/30 15:57:37 gdavidson Exp $

#endregion
