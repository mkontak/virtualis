using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Configuration;
using System.Collections;
using System.Collections.Specialized;

using System.Xml;


namespace Viatronix
{

  class FoldersConfigSectionHandler : IConfigurationSectionHandler
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

      Dictionary<string, Folder> folders = new Dictionary<string, Folder>();

      try
      {

        // create a launch rule for each config entry
        foreach (XmlNode node in section.SelectNodes("folder"))
        {
          Folder folder = new Folder();
          folder.Path = node.Attributes["path"].Value.ToString();
          folder.Body = node.Attributes["body"].Value.ToString();
          folder.Subject = node.Attributes["subject"].Value.ToString();
          folder.Email = Boolean.Parse(node.Attributes["email"].Value);
          folder.Source = node.Attributes["source"].Value.ToString();
          folder.Type = node.Attributes["type"].Value.ToString();
          folder.System = node.Attributes["system"].Value.ToString();
          folders.Add(folder.Path.ToUpper(), folder);
        }

      }
      catch (Exception e)
      {
        throw;
      }

      return folders;

    } // Create( parent, configContext, section )



    #endregion

  }
}

