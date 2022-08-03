using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;


namespace Viatronix.Enterprise.Entities
{

  /// <summary>
  /// Identifies a configuration
  /// </summary>
  public class SystemConfig : Entity
  {


    #region class Attributes

    /// <summary>
    /// Mirrors the database
    /// </summary>
    public static class Attributes
    {
      public const string Description = "desc";
      public const string Name = "name";
      public const string Settings = "set";
    };

    #endregion

    #region fields

    /// <summary>
    /// Application name
    /// </summary>
    public string m_name = string.Empty;

    /// <summary>
    /// Description
    /// </summary>
    private string m_description = string.Empty;

    /// <summary>
    /// Config settings
    /// </summary>
    private string m_settings = string.Empty;

    #endregion

    #region properties


    /// <summary>
    /// Gets the application Name
    /// </summary>
    public string Name
    {
      get { return m_name; }
      set { SetProperty<string>("Name", value, ref m_name); }
    }

    /// <summary>
    /// Gets the application description
    /// </summary>
    public string Description
    {
      get { return m_description; }
      private set { SetProperty<string>("Description", value, ref m_description); }
    }

    /// <summary>
    /// Gets the configuration settings
    /// </summary>
    public string Settings
    {
      get { return m_settings; }
      private set { SetProperty<string>("Settings", value, ref m_settings); }
    }


    #endregion

    #region methods

    /// <summary>
    /// Provides a string representation of this object.
    /// </summary>
    /// <returns>A string representation of this object.</returns>
    public override string ToString()
    { return m_name; } // ToString()


    /// <summary>
    /// Creates the Product from the xml
    /// </summary>
    /// <param name="xml">The xml to de-serialize.</param>
    /// <returns>Product object.</returns>
    static public SystemConfig Create(string xml)
    {
      SystemConfig configuration = new SystemConfig();

      if (configuration == null)
      {
        throw new OutOfMemoryException("Failed to create SystemConfig object");
      }

      XmlDocument doc = new XmlDocument();
      doc.LoadXml(xml);

      return Create(doc.DocumentElement);

    } // Create( xml )


    /// <summary>
    /// Creates the Product from the xml
    /// </summary>
    /// <param name="xml">The xml to de-serialize.</param>
    /// <returns>Product object.</returns>
    static public SystemConfig Create(XmlNode node)
    {
      SystemConfig configuration = new SystemConfig();

      if (configuration == null)
        throw new OutOfMemoryException("Failed to create SystemConfig object");

      configuration.Load(node);

      return configuration;
    } // Create( node )

    #endregion

    #region IEntity

    /// <summary>
    /// Creates an xml representation of a application object.
    /// </summary>
    /// <returns>Xlm string representing the application object.</returns>
    public override string ToXml()
    {
      StringBuilder builder = new StringBuilder();
      using (XmlWriter writer = XmlWriter.Create(builder, new XmlWriterSettings() { OmitXmlDeclaration = true, Indent = false }))
      {


        writer.WriteStartElement(this.Tag);
        {

          writer.WriteAttributeString(Attributes.Name, m_name);
          writer.WriteAttributeString(Attributes.Description, m_description);

          if (m_settings.Length > 0)
          {
            writer.WriteStartElement(Attributes.Settings);
            writer.WriteRaw(m_settings);
            writer.WriteEndElement();
          }


          WriteAttributeStrings(writer);
          WriteElements(writer);

        }

        writer.WriteEndElement();
      }

      return builder.ToString();
    } // ToXml()


    /// <summary>
    /// Creates the Product from the xml
    /// </summary>
    /// <param name="xml">The xml to de-serialize.</param>
    /// <returns>Product object.</returns>
    public override void Load(XmlNode node)
    {

      base.Load(node);

      m_name = node.Attributes[Attributes.Name].Value.ToString();
      m_description = node.Attributes[Attributes.Description].Value.ToString();

      XmlNode cfgNode = node.SelectSingleNode(Attributes.Settings);

      if ( cfgNode != null )
        m_settings = cfgNode.InnerXml;

    } // Load( node )

    /// <summary>
    /// Clones the application object
    /// </summary>
    /// <returns></returns>
    public override object Clone()
    {
      SystemConfig configuration = new SystemConfig();

      if (configuration == null)
        throw new OutOfMemoryException("Failed to create SystemConfig object");

      configuration.m_name = m_name;
      configuration.m_description = m_description;
      configuration.m_settings = m_settings;

      return configuration;

    } // Clone()


    /// <summary>
    /// Checks for value equality of task objects
    /// </summary>
    /// <param name="obj">A job to compare with this instance or a null reference</param>
    /// <returns>true if they are equal values; otherwise false</returns>
    public override bool Equals(IEntity obj)
    {
      SystemConfig other = obj as SystemConfig;

      return other != null &&
             base.Equals(other) &&
              string.Equals(m_name, other.m_name);

    } // Equals( obj )


    #endregion

  } // class SystemConfig

} // namespace Viatronix.Enterprise.Entities
