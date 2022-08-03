// $Id: GroupSettingsSectionHandler.cs,v 1.2 2007/03/08 21:56:13 mkontak Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]


using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.Xml;

namespace Viatronix.v3D.Dicom.UI
{

  /// <summary>
  /// Section handler for parsing the group settings from the xml
  /// </summary>
  public class GroupSettingsSectionHandler : System.Configuration.IConfigurationSectionHandler
  {

    #region methods

    /// <summary>
    /// Creates the object for the group settings
    /// </summary>
    /// <param name="parent">Parent</param>
    /// <param name="context">Context</param>
    /// <param name="section">Xml node section</param>
    /// <returns>hash table of group settings</returns>
    public object Create(object parent, object context, System.Xml.XmlNode section)
    {
      try
      {

        Dictionary<string, Group> groupList = new Dictionary<string, Group>();

        XmlNodeList list = section.SelectNodes("group");

        foreach (XmlNode node in list)
        {
          Group group = Group.Create(node);
          groupList.Add(group.Id, group);
         }


        return groupList;
      }
      catch (Exception e)
      {
        Viatronix.Logging.Log.Error("Failed to read Group Colors section : " + e.Message, "GroupSettingSectionHandler", "Create");
        return null;
      }
    } // Create( parent, context, section )

    #endregion

  } // class ConsoleFrameConfigSectionHandler

} // namespace Viatronix.v3D.Dicom.UI


#region revision history

// $Log: GroupSettingsSectionHandler.cs,v $
// Revision 1.2  2007/03/08 21:56:13  mkontak
// Coding standards
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/GroupSettingsSectionHandler.cs,v 1.2 2007/03/08 21:56:13 mkontak Exp $
// $Id: GroupSettingsSectionHandler.cs,v 1.2 2007/03/08 21:56:13 mkontak Exp $

#endregion

