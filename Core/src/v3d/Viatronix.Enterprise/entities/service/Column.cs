using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;

namespace Viatronix.Enterprise.Entities
{

  public class Column : Entity
  {

    #region class Attributes

    /// <summary>
    /// Mirrors the database
    /// </summary>
    public static class Attributes
    {
      public const string Name = "name";
      public const string Description = "desc";
      public const string Length = "length";
      public const string Header = "hdr";
    };

    #endregion

    #region fields

    /// <summary>
    /// column name
    /// </summary>
    public string m_name = string.Empty;

    /// <summary>
    /// Description
    /// </summary>
    private string m_description = string.Empty;

    /// <summary>
    /// Length of column in bytes
    /// </summary>
    private int m_length = 0;

    /// <summary>
    /// Column header definition
    /// </summary>
    private string m_header = string.Empty;

    /// <summary>
    /// Tabel this column belongs to
    /// </summary>
    private Table m_table = null;

    #endregion

    #region properties

    /// <summary>
    /// Gets the column Name
    /// </summary>
    public string Name
    { get { return m_name; } }


    /// <summary>
    /// Gets the column description
    /// </summary>
    public string Description
    { get { return m_description; } }

    /// <summary>
    /// Gets the column header
    /// </summary>
    public string Header
    { get { return m_header; } }

    /// <summary>
    /// Gets the column length in bytes
    /// </summary>
    public int Length
    { get { return m_length; } }

    /// <summary>
    /// Gets the table this column belongs to
    /// </summary>
    public Table Table
    { get { return m_table; } }

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
    static public Column Create(Table table, string xml)
    {

      XmlDocument doc = new XmlDocument();
      doc.LoadXml(xml);

      return Create(table, doc.DocumentElement);

    } // Create( xml )


    /// <summary>
    /// Creates the Product from the xml
    /// </summary>
    /// <param name="xml">The xml to de-serialize.</param>
    /// <returns>Product object.</returns>
    static public Column Create(Table table, XmlNode node)
    {
      Column column = new Column();

      if (column == null)
        throw new OutOfMemoryException("Failed to create Column object");

      column.m_table = table;
      column.Load(node);

      return column;
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
          writer.WriteAttributeString(Attributes.Length, m_length.ToString());

          if ( m_header != m_name )
            writer.WriteAttributeString(Attributes.Header, m_header);


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
      node.TryGetAttribute<string>(Attributes.Description, ref m_description);
      if (!node.TryGetAttribute<string>(Attributes.Header, ref m_header))
        m_header = m_name;
      m_length = Int32.Parse(node.Attributes[Attributes.Length].Value.ToString());


    } // Load( node )

    /// <summary>
    /// Clones the application object
    /// </summary>
    /// <returns></returns>
    public override object Clone()
    {
      Column column = new Column();

      if (column == null)
        throw new OutOfMemoryException("Failed to create Column object");


      column.m_description = m_description;
      column.m_name = m_name;
      column.m_length = m_length;
      column.m_header = m_header;
      column.m_table = m_table;

      return column;

    } // Clone()


    /// <summary>
    /// Checks for value equality of task objects
    /// </summary>
    /// <param name="obj">A job to compare with this instance or a null reference</param>
    /// <returns>true if they are equal values; otherwise false</returns>
    public override bool Equals(IEntity obj)
    {
      Column other = obj as Column;

      return other != null &&
             base.Equals(other) &&
              Table.Equals(m_table, other.Table) &&
                string.Equals(m_name, other.m_name);  

    } // Equals( obj )


    #endregion


  } // class Column

} // namespace Viatronix.Enterprise.Entities
