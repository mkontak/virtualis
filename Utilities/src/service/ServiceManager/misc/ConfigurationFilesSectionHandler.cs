using System;
using System.Xml;
using System.Xml.Schema;
using System.Configuration;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Viatronix.Utilities.Service.ServiceManager.ViewModels;

namespace Viatronix.Utilities.Service.ServiceManager
{
  /// <summary>
  /// Handler for the configuration files section.
  /// </summary>
  public class ConfigurationFilesSectionHandler : IConfigurationSectionHandler
  {

    #region IConfigurationSectionHandler implementation

    /// <summary>
    /// Creates the section as an object.
    /// </summary>
    /// <param name="parent">Parent object.</param>
    /// <param name="configContext">Configuration context object.</param>
    /// <param name="section">Section XML node.</param>
    /// <returns></returns>
    public object Create(object parent, object configContext, XmlNode section)
    {
      List<ConfigurationFileViewModel> configurationFiles = new List<ConfigurationFileViewModel>();

      foreach(XmlNode configFileNode in section.SelectNodes("file"))
      {
        ConfigurationFileViewModel configFile = new ConfigurationFileViewModel();

        XmlAttribute name = configFileNode.Attributes["name"];
        if (name != null)
        {
          configFile.Name = name.Value;
        }
        else
        {
          throw new XmlSchemaException("The application's configuration is incorrect.  Please contact Viatronix Customer Service.");
        }

        XmlAttribute filename = configFileNode.Attributes["filename"];
        if (filename != null)
        {
          configFile.Filename = filename.Value;
        }
        else
        {
          throw new XmlSchemaException("The application's configuration is incorrect.  Please contact Viatronix Customer Service.");
        }

        configurationFiles.Add(configFile);
      }

      return configurationFiles;
    } // Create( parent, configContext, section )

    #endregion

  } // class ConfigurationFilesSectionHandler
} // namespace Viatronix.Utilities.Service.ServiceManager
