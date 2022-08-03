// usings
using System;
using System.Collections;
using System.Configuration;
using System.Xml;
using System.Windows.Forms;

namespace Viatronix.Utilities.Database
{

  /// <summary>
  /// Summary description for SettingsSectionHandler.
  /// </summary>
  public class SettingsSectionHandler : IConfigurationSectionHandler
  {
    /// <summary>
    ///  Parses the XML of the configuration section.
    /// </summary>
    /// <param name="parent">The configuration settings in a corresponding parent configuration section.</param>
    /// <param name="configContext">Not used.</param>
    /// <param name="section">The XmlNode that contains the configuration information from the configuration file.</param>
    /// <returns>Hashtable instance containg StudyType instances contained sestion.</returns>
    public object Create( object parent, object configContext, XmlNode  section )
    {
      // create tha hash map to contain the StudyType instances
      Hashtable map = new Hashtable();

      try
      {
        XmlNodeList list = section.SelectNodes("setting");
      

        foreach( XmlNode node in list )
        {         
          // add the session type to the map using the id as the key
          map.Add( node.Attributes["id"].Value.ToString(), node.Attributes["value"].Value.ToString() );
        } // foreach( XmlNode node in nodes )
      } // try
      catch( Exception e )
      {
        MessageBox.Show( "Error reading status configutaion from app.config : " + e.Message, "Configuration Error", MessageBoxButtons.OK, MessageBoxIcon.Error );
        throw;
      } // catch( Exception e )
      
      // return the map
      return map;
    } // Create( parent, configContext, section )
	
  }
}

