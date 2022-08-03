// $Id: LaunchRuleConfigSectionHandler.cs,v 1.4.2.2 2011/05/18 01:57:00 mkontak Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

using System;
using System.Configuration;
using System.Collections.Generic;
using System.Text;
using System.Xml;


namespace Viatronix.Console
{
  /// <summary>
  /// Creates all launch rules from xml.
  /// </summary>
  public class LaunchRuleConfigSectionHandler : IConfigurationSectionHandler
  {

    #region IConfigurationSectionHandler Members

    /// <summary>
    /// Creates a collection of launch rules from xml.
    /// </summary>
    /// <param name="parent">The configuration settings in a corresponding parent configuration section.</param>
    /// <param name="configContext">null</param>
    /// <param name="section">The XmlNode that contains the configuration information from the configuration file. Provides direct access to the XML contents of the configuration section.</param>
    /// <returns>A collection of launch rules.</returns>
    public object Create( object parent, object configContext, XmlNode section )
    {
      List<ILaunchRule> rules = new List<ILaunchRule>();
      try
      {
        // create a launch rule for each config entry
        foreach( XmlNode node in section.SelectNodes( "rule" ))
        {
          ILaunchRule rule = (ILaunchRule) Activator.CreateInstance( Type.GetType( node.Attributes[ "typename" ].Value ));
          rule.ID = node.Attributes[ "id" ].Value;
          rule.Text = node.Attributes[ "text" ].Value;
          rule.CommandLine = (node.Attributes["commandLine"] != null ? node.Attributes["commandLine"].Value : string.Empty);
          rule.Executable = Viatronix.Utilities.Registry.SystemSettings.CurrentSystemSettings.ParseExpression( node.Attributes[ "executable" ].Value );
          rule.LauncherTypename = node.Attributes[ "launcher" ].Value;

          if( node.Attributes[ "image" ] != null && node.Attributes[ "image" ].Value != string.Empty )
            rule.Logo = Viatronix.UI.Utilities.LoadImage( node.Attributes[ "image" ].Value );

          if( node.Attributes[ "commandLine" ] != null )
            rule.CommandLine = node.Attributes[ "commandLine" ].Value;

          rule.Xml = node.OuterXml;
          rule.Initialize( node.OuterXml );
          rules.Add( rule );
        }
      }
      catch( Exception e )
      {
        Viatronix.Logging.Log.Error("Console was unable to load the launch rules. [ERROR=" + e.Message + "]", "LaunchRulConfigSectionHandler", "Create");
        Viatronix.UI.MessageBox.ShowError( "Console was unable to load the launch rules." );
      }

      return rules;
    } // Create( parent, configContext, section )


    #endregion

  } // class ViewerLauncherConfigurationSectionHandler
} // namespace Viatronix.v3D.Core

#region revision history

// $Log: LaunchRuleConfigSectionHandler.cs,v $
// Revision 1.4.2.2  2011/05/18 01:57:00  mkontak
// Issue CON-1640: Windows 7 Compliance by removing the SystemSettings
// registry to a global config file. Move the default log directory to the windows
// common application data directory (XP - C:\Document and Settings\All Users\Application Data , Win7 - C:\ProgramData ).
// Merged the Viatronix.v3D.Logger project into the Viatronix.Utilities project and
// signed the Utilities assembly.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.4.2.1  2009/04/03 18:25:14  mkontak
// Implemenetd commandLine feature
//
// Revision 1.4  2007/03/06 19:14:59  gdavidson
// Added class comments
//
// Revision 1.3  2006/12/18 16:10:30  gdavidson
// Added functionality to intialize with additional xml data
//
// Revision 1.2  2006/11/02 22:41:25  gdavidson
// Added configurable command line arguments
//
// Revision 1.1  2006/10/30 15:38:56  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console.Core/launching/rules/LaunchRuleConfigSectionHandler.cs,v 1.4.2.2 2011/05/18 01:57:00 mkontak Exp $
// $Id: LaunchRuleConfigSectionHandler.cs,v 1.4.2.2 2011/05/18 01:57:00 mkontak Exp $

#endregion