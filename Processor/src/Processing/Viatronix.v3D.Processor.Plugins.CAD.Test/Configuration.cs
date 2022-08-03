// $Id: Configuration.cs,v 1.3 2006/12/21 15:33:02 romy Exp $ 
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// Author: Romy Sreedharan ( mailto:romy@viatronix.com )

#region using

using System;
using System.Collections.Generic;
using System.Text;
using System.Configuration;
using System.Xml;
using System.Reflection;
using System.IO;

#endregion

namespace Viatronix.v3D.Processor.Plugins.CAD.Test
{
  /// <summary>
  /// Class which reads and writes from the config file
  /// </summary>
  class Configuration
  {
    #region methods

    /// <summary>
    /// Read from the config file
    /// </summary>
    /// <param name="args">EnvrionmentArgs object</param>
    public static void ReadFromAppConfig(EnvrionmentArgs args)
    {
      args.DicomFolder = System.Configuration.ConfigurationManager.AppSettings["DicomFolder"];
      args.AppId = System.Configuration.ConfigurationManager.AppSettings["selectedAppId"];
      args.ConversionApp = System.Configuration.ConfigurationManager.AppSettings["conversionApp"];
      args.PipelineApp = System.Configuration.ConfigurationManager.AppSettings["pipelineApp"];
      args.CADPlugin = System.Configuration.ConfigurationManager.AppSettings["CADPlugin"];
      args.WorkingFolder = System.Configuration.ConfigurationManager.AppSettings["WorkingFolder"];

      string processed = System.Configuration.ConfigurationManager.AppSettings["process"];
      if (processed.ToLower() == "true")
        args.RunPreProcess = true;
      else
        args.RunPreProcess = false;
    }//ReadFromAppConfig(EnvrionmentArgs args)

    /// <summary>
    /// Returns all appIds
    /// </summary>
    /// <returns>ArrayList of appids</returns>
    public static System.Collections.ArrayList GetAllAppIds()
    {
      System.Collections.ArrayList allAppIds = new System.Collections.ArrayList();
      string appIDs = System.Configuration.ConfigurationManager.AppSettings["appIDs"];
      string[] AllAppIds = appIDs.Split(new char[] { ',' });
      foreach (string appId in AllAppIds)
        allAppIds.Add(appId);

      return allAppIds;
    }//GetAllAppIds()

    /// <summary>
    /// Saves the arguments in the config file
    /// </summary>
    /// <param name="args">arguments</param>
    public static void Save(EnvrionmentArgs args)
    {
      SaveSettings("DicomFolder", args.DicomFolder);

      SaveSettings("selectedAppId", args.AppId);

      SaveSettings("WorkingFolder", args.WorkingFolder);

      if (args.RunPreProcess)
        SaveSettings("process", "true");
      else
        SaveSettings("process", "false");


      string appIDs = System.Configuration.ConfigurationManager.AppSettings["appIDs"];
      string[] currentAppIds = appIDs.Split(new char[] { ',' });
      System.Collections.ArrayList availableAppIds = new System.Collections.ArrayList();
      //save the new item if there is one
      foreach (string appId in currentAppIds)
      {
        availableAppIds.Add(appId);
      }

      if (!availableAppIds.Contains(args.AppId) && args.AppId != string.Empty )
        appIDs += "," + args.AppId;

      SaveSettings("appIDs", appIDs);
    }//Save( EnvrionmentArgs args )

    /// <summary>
    /// Saves the Value
    /// </summary>
    /// <param name="setting">setting</param>
    /// <param name="settingValue">value</param>
    private static void SaveSettings(string setting, string settingValue)
    {
      XmlNodeList settings;
      XmlElement node;
      XmlElement appSettingsNode;
      string query;
      string configFile = Assembly.GetExecutingAssembly().Location + ".config";
      StreamWriter fileConfig;

      if (File.Exists(configFile) == false)
      {
        //Create the app.config file and write the setting in it
        fileConfig = File.CreateText(configFile);
        fileConfig.WriteLine(@"<?xml version=""1.0"" encoding=""utf-8"" ?><configuration><appSettings><add key=""" + setting + "\" value=\"" + settingValue + "\"" + @"/></appSettings></configuration>");
        fileConfig.Close();
        fileConfig = null;
        return;
      }

      //Load the config file
      XmlDocument doc = new XmlDocument();
      doc.Load(configFile);

      //Find the <appSettings> node
      query = "configuration/appSettings";
      appSettingsNode = (XmlElement)doc.SelectSingleNode(query);

      if (appSettingsNode == null)
      {
        fileConfig = File.CreateText(configFile);
        fileConfig.WriteLine(@"<?xml version=""1.0"" encoding=""utf-8"" ?><configuration><appSettings><add key=""" + setting + "\" value=\"" + settingValue + "\"" + @"/></appSettings></configuration>");
        fileConfig.Close();
        fileConfig = null;
        return;
      }

      //Find the setting
      query = "configuration/appSettings/add[@key='" + setting + "']";
      settings = doc.SelectNodes(query);

      //Check to see if the node already exists
      if (settings.Count > 0)
        node = (XmlElement)settings[0];
      else
      {
        node = doc.CreateElement("add");
        XmlAttribute attKey = doc.CreateAttribute("key");
        attKey.Value = setting;
        node.Attributes.SetNamedItem(attKey);
        XmlAttribute attVal = doc.CreateAttribute("value");
        node.Attributes.SetNamedItem(attVal);
        //Append the node
        appSettingsNode.AppendChild(node);
      }

      // Update the value attribute
      if (node.Attributes["value"].Value != settingValue)
      {
        node.Attributes["value"].Value = settingValue;
        doc.Save(configFile);
      }
      doc = null;
    }//SaveSettings(string , string )

    #endregion
  }//Configuration
}//namespace Viatronix.v3D.Processor.Plugins.CAD.Test



#region
// $Log: Configuration.cs,v $
// Revision 1.3  2006/12/21 15:33:02  romy
// check added for null appids
//
// Revision 1.2  2006/12/21 12:12:14  romy
// added more checks
//
// Revision 1.1  2006/12/21 11:27:23  romy
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD.Test/Configuration.cs,v 1.3 2006/12/21 15:33:02 romy Exp $
// $Id: Configuration.cs,v 1.3 2006/12/21 15:33:02 romy Exp $
#endregion