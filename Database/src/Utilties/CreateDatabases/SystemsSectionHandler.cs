using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections;
using System.Configuration;
using System.Xml;



namespace Viatronix.Utilities.Database
{
  /// <summary>
  /// Summary description for SettingsSectionHandler.
  /// </summary>
  public class SystemsSectionHandler : IConfigurationSectionHandler
  {
    /// <summary>
    ///  Parses the XML of the configuration section.
    /// </summary>
    /// <param name="parent">The configuration settings in a corresponding parent configuration section.</param>
    /// <param name="configContext">Not used.</param>
    /// <param name="section">The XmlNode that contains the configuration information from the configuration file.</param>
    /// <returns>Hashtable instance containg StudyType instances contained sestion.</returns>
    public object Create(object parent, object configContext, XmlNode section)
    {
      // create tha hash map to contain the StudyType instances
      List<V3DSystem> systems = new List<V3DSystem>();

      try
      {


        foreach (XmlNode node in section.SelectNodes("system"))
        {
          // add the session type to the map using the id as the key
          systems.Add(V3DSystem.Create(node));
        } // foreach( XmlNode node in nodes )
      } // try
      catch (Exception e)
      {
        throw new Exception("Error reading status configutaion from app.config : " + e.Message);
      } // catch( Exception e )

      // return the map
      return systems;
    } // Create( parent, configContext, section )

  }
}
