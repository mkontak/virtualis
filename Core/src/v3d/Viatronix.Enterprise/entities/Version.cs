using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;


namespace Viatronix.Enterprise.Entities
{

  /// <summary>
  /// Implements a version resource that holds an entry containing the information about the systems object versions
  /// </summary>
  public class Version : Entity
  {


    #region class Attributes

    /// <summary>
    /// Mirrors the database
    /// </summary>
    public static class Attributes
    {
      public const string Object = "object";
      public const string Type = "type";
      public const string Version = "ver";
    };

    #endregion

    #region fields

    /// <summary>
    /// Object
    /// </summary>
    private string m_object = string.Empty;

    /// <summary>
    /// Object type
    /// </summary>
    private string m_type = string.Empty;

    /// <summary>
    /// version
    /// </summary>
    private string m_version = string.Empty;


    #endregion


    #region properties


    /// <summary>
    /// Gets the obect
    /// </summary>
    public string Object
    {
      get { return m_object; }
    }

    /// <summary>
    /// Gets object type
    /// </summary>
    public string ObjectType
    {
      get { return m_type; }
    }

    /// <summary>
    /// Gets version
    /// </summary>
    public string ObjectVersion
    {
      get { return m_version; } 
    }


    #endregion

    #region methods

    /// <summary>
    /// Provides a string representation of this object.
    /// </summary>
    /// <returns>A string representation of this object.</returns>
    public override string ToString()
    { return m_object; } // ToString()


    /// <summary>
    /// Creates the Product from the xml
    /// </summary>
    /// <param name="xml">The xml to de-serialize.</param>
    /// <returns>Product object.</returns>
    static public Version Create(string xml)
    {
      Version version = new Version();

      if (version == null)
      {
        throw new OutOfMemoryException("Failed to create Version object");
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
    static public Version Create(XmlNode node)
    {
      Version version = new Version();

      if (version == null)
        throw new OutOfMemoryException("Failed to create Version object");

      version.Load(node);

      return version;
    } // Create( node )

    #endregion

    #region IEntity

    /// <summary>
    /// Creates an xml representation of a version object.
    /// </summary>
    /// <returns>Xlm string representing the version object.</returns>
    public override string ToXml()
    {
      StringBuilder builder = new StringBuilder();
      using (XmlWriter writer = XmlWriter.Create(builder, new XmlWriterSettings() { OmitXmlDeclaration = true, Indent = false }))
      {


        writer.WriteStartElement(this.Tag);
        {


          writer.WriteAttributeString(Attributes.Object, m_object);
          writer.WriteAttributeString(Attributes.Type, m_type);
          writer.WriteAttributeString(Attributes.Version, m_version);

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

      m_object = node.Attributes[Attributes.Object].Value.ToString();
      m_type = node.Attributes[Attributes.Type].Value.ToString();
      m_version = node.Attributes[Attributes.Version].Value.ToString();


    } // Load( node )

    /// <summary>
    /// Clones the version object
    /// </summary>
    /// <returns></returns>
    public override object Clone()
    {
      Version version = new Version();

      if (version == null)
        throw new OutOfMemoryException("Failed to create Version object");


      version.m_object = m_object;
      version.m_type = m_type;
      version.m_version = m_version;


      return version;

    } // Clone()


    /// <summary>
    /// Checks for value equality of task objects
    /// </summary>
    /// <param name="obj">A job to compare with this instance or a null reference</param>
    /// <returns>true if they are equal values; otherwise false</returns>
    public override bool Equals(IEntity obj)
    {
      Version other = obj as Version;

      return other != null &&
             base.Equals(other) &&
              string.Equals(m_object, other.m_object);

    } // Equals( obj )


    #endregion

  } // class Version

} // namespace Viatronix.Enterprise.Entities