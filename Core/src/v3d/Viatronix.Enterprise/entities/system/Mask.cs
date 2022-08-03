using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;


namespace Viatronix.Enterprise.Entities
{

  /// <summary>
  /// Implements a mask resource that holds an entry containing the information about the systems maskation
  /// </summary>
  public class Mask : Entity
  {


    #region class Attributes

    /// <summary>
    /// Mirrors the database
    /// </summary>
    public static class Attributes
    {
      public const string Name = "name";
      public const string BitName = "bn";
      public const string BitMask = "bit";
      public const string Description = "desc";
    };

    #endregion

    #region fields

    /// <summary>
    /// Mask name
    /// </summary>
    private string m_name = string.Empty;

    /// <summary>
    /// Bit name
    /// </summary>
    private string m_bitName = string.Empty;

    /// <summary>
    /// Bit mask
    /// </summary>
    private long m_bitMask = 0;

    /// <summary>
    /// Mask description
    /// </summary>
    private string m_description = string.Empty;

     #endregion


    #region properties


    /// <summary>
    /// Gets the masks name
    /// </summary>
    public string Name
    {
      get { return m_name; }
    }

    /// <summary>
    /// Gets bit name
    /// </summary>
    public string BitName
    {
      get { return m_bitName; }
    }

    /// <summary>
    /// Gets the bit mask
    /// </summary>
    public long BitMask
    {
      get { return m_bitMask; }
    }

    /// <summary>
    /// Descriptionb
    /// </summary>
    public string Description
    { get { return m_description; } }


    #endregion

    #region methods

    /// <summary>
    /// Provides a string representation of this object.
    /// </summary>
    /// <returns>A string representation of this object.</returns>
    public override string ToString()
    { return string.Format("{0}-{1}", m_name, m_bitName); } // ToString()


    /// <summary>
    /// Creates the Product from the xml
    /// </summary>
    /// <param name="xml">The xml to de-serialize.</param>
    /// <returns>Product object.</returns>
    static public Mask Create(string xml)
    {
      Mask mask = new Mask();

      if (mask == null)
      {
        throw new OutOfMemoryException("Failed to create Mask object");
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
    static public Mask Create(XmlNode node)
    {
      Mask mask = new Mask();

      if (mask == null)
        throw new OutOfMemoryException("Failed to create Mask object");

      mask.Load(node);

      return mask;
    } // Create( node )

    #endregion

    #region IEntity

    /// <summary>
    /// Creates an xml representation of a mask object.
    /// </summary>
    /// <returns>Xlm string representing the mask object.</returns>
    public override string ToXml()
    {
      StringBuilder builder = new StringBuilder();
      using (XmlWriter writer = XmlWriter.Create(builder, new XmlWriterSettings() { OmitXmlDeclaration = true, Indent = false }))
      {


        writer.WriteStartElement(this.Tag);
        {


          writer.WriteAttributeString(Attributes.Name, m_name);
          writer.WriteAttributeString(Attributes.BitName, m_bitName);
          writer.WriteAttributeString(Attributes.BitMask, m_bitMask.ToString());
          writer.WriteAttributeString(Attributes.Description, m_description);

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
      m_bitName = node.Attributes[Attributes.BitName].Value.ToString();
      m_bitMask = Int32.Parse(node.Attributes[Attributes.BitMask].Value.ToString());
      m_description = node.Attributes[Attributes.Description].Value.ToString();


    } // Load( node )

    /// <summary>
    /// Clones the mask object
    /// </summary>
    /// <returns></returns>
    public override object Clone()
    {
      Mask mask = new Mask();

      if (mask == null)
        throw new OutOfMemoryException("Failed to create Mask object");


      mask.m_name = m_name;
      mask.m_bitName = m_bitName;
      mask.m_bitMask = m_bitMask;
      mask.m_description = m_description;
 

      return mask;

    } // Clone()


    /// <summary>
    /// Checks for value equality of task objects
    /// </summary>
    /// <param name="obj">A job to compare with this instance or a null reference</param>
    /// <returns>true if they are equal values; otherwise false</returns>
    public override bool Equals(IEntity obj)
    {
      Mask other = obj as Mask;

      return other != null &&
             base.Equals(other) &&
              string.Equals(m_name, other.m_name) &&
                string.Equals(m_bitName, other.m_bitName);

    } // Equals( obj )


    #endregion

  } // class Mask

} // namespace Viatronix.Enterprise.Entities