using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;

namespace Viatronix.Enterprise.Entities
{

  public class Database : Entity
  {

    #region class Attributes

    /// <summary>
    /// Mirrors the database
    /// </summary>
    public static class Attributes
    {
      public const string Name = "name";
      public const string Description = "desc";
      public const string Filename = "filename";
      public const string Tables = "tables";
    };

    #endregion

    #region fields

    /// <summary>
    /// database name
    /// </summary>
    public string m_name = string.Empty;

    /// <summary>
    /// Description
    /// </summary>
    private string m_description = string.Empty;

    /// <summary>
    /// Database file location
    /// </summary>
    private string m_filename = string.Empty;

    /// <summary>
    ///  Tables in the database
    /// </summary>
    public List<Table> m_tables = new List<Table>();

    #endregion

    #region properties

    /// <summary>
    /// Gets the database Name
    /// </summary>
    public string Name
    { get { return m_name; } }


    /// <summary>
    /// Gets the database description
    /// </summary>
    public string Description
    { get { return m_description; } }

    /// <summary>
    /// Gets the database Filename
    /// </summary>
    public string Filename
    { get { return m_filename; } }


    /// <summary>
    /// Gets the list of database tables
    /// </summary>
    public List<Table> Tables
    { get { return m_tables; } }

    #endregion

    #region methods

    /// <summary>
    /// Determines if the two database are eaual
    /// </summary>
    /// <param name="db1"></param>
    /// <param name="db2"></param>
    /// <returns></returns>
    public bool Equals(Database db1, Database db2)
    {
      bool equals = false;

      if (db1 != null && db2 != null)
        equals = db1.Equals(db2);
      else if ( db1 == null  && db2 == null )
        equals = true;

      return equals;

    } // Equals(Database db1, Database db2)

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
    static public Database Create(string xml)
    {
      Database database = new Database();

      if (database == null)
      {
        throw new OutOfMemoryException("Failed to create Database object");
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
    static public Database Create(XmlNode node)
    {
      Database database = new Database();

      if (database == null)
        throw new OutOfMemoryException("Failed to create Database object");

      database.Load(node);

      return database;
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
          writer.WriteAttributeString(Attributes.Filename, m_filename);

          if (m_tables.Count > 0)
          {
            writer.WriteStartElement(Attributes.Tables);

            foreach (Table table in m_tables)
            {
              writer.WriteRaw(table.ToXml());
            }

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
      m_filename = node.Attributes[Attributes.Filename].Value.ToString();
      

      XmlNode tables = node.SelectSingleNode(Attributes.Tables);

      if (tables != null)
      {
        foreach (XmlNode table in tables.SelectNodes("table"))
        {
          m_tables.Add(Table.Create(this, table));
        }
      }


    } // Load( node )

    /// <summary>
    /// Clones the application object
    /// </summary>
    /// <returns></returns>
    public override object Clone()
    {
      Database database = new Database();

      if (database == null)
        throw new OutOfMemoryException("Failed to create Database object");


      database.m_description = m_description;
      database.m_name = m_name;
      database.m_filename = m_filename;
      database.m_tables.AddRange(m_tables);

      return database;

    } // Clone()


    /// <summary>
    /// Checks for value equality of task objects
    /// </summary>
    /// <param name="obj">A job to compare with this instance or a null reference</param>
    /// <returns>true if they are equal values; otherwise false</returns>
    public override bool Equals(IEntity obj)
    {
      Database other = obj as Database;

      return other != null &&
             base.Equals(other) &&
              string.Equals(m_name, other.m_name);

    } // Equals( obj )


    #endregion


  } // class Database

} // namespace Viatronix.Enterprise.Entities
