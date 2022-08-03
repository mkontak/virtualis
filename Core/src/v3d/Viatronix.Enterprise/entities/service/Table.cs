using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;

namespace Viatronix.Enterprise.Entities
{
  [Flags]
  public enum Permissions
  {
    None   = 0x00,
    Insert = 0x01,
    Update = 0x02,
    Delete = 0x04,
    All    = 0xff
  }

  public class Table : Entity
  {

    #region class Attributes

    /// <summary>
    /// Mirrors the database
    /// </summary>
    public static class Attributes
    {
      public const string Name = "name";
      public const string Description = "desc";
      public const string Columns = "columns";
      public const string Resource = "res";
      public const string ServicePermissions = "sp";
     };

    #endregion

    #region fields

    /// <summary>
    /// Resouyrce name for the table (Usually the singular name for the table)
    /// </summary>
    public string m_resource = string.Empty;

    /// <summary>
    /// table name
    /// </summary>
    public string m_name = string.Empty;

    /// <summary>
    /// Description
    /// </summary>
    private string m_description = string.Empty;

    /// <summary>
    /// List of columns for this table
    /// </summary>
    private List<Column> m_columns = new List<Column>();

    /// <summary>
    /// Database this table is linked to
    /// </summary>
    private Database m_database = new Database();

    /// <summary>
    /// Entity Type
    /// </summary>
    private Type m_entityType = null;

    /// <summary>
    /// Service permissions
    /// </summary>
    private Permissions m_servicePermissions = Permissions.None;

    #endregion

    #region properties

    /// <summary>
    /// Gets the service permissions mask
    /// </summary>
    public Permissions ServicePermissions
    { get { return m_servicePermissions; } }

    /// <summary>
    /// Gets the flag indicating if insertion is allowed for this table
    /// </summary>
    public bool CanServiceDelete
    { get { return ( m_servicePermissions & Permissions.Delete ) == Permissions.Delete; } }

    /// <summary>
    /// Gets the flag indicating if insertion is allowed for this table
    /// </summary>
    public bool CanServiceInsert
    { get { return (m_servicePermissions & Permissions.Insert) == Permissions.Insert; } }

    /// <summary>
    /// Gets the flag indicating if update is allowed for this table
    /// </summary>
    public bool CanServiceUpdate
    { get { return (m_servicePermissions & Permissions.Update) == Permissions.Update; } }

    /// <summary>
    /// Gets the entity type associated with this table
    /// </summary>
    public Type EntityType
    { get { return m_entityType; } }

    /// <summary>
    /// Gets the resource name for the table
    /// </summary>
    public string Resource
    { get { return m_resource; } }

    /// <summary>
    /// Gets the table Name
    /// </summary>
    public string Name
    { get { return m_name; } }



    /// <summary>
    /// Gets the table description
    /// </summary>
    public string Description
    { get { return m_description; } }


    /// <summary>
    /// Gets the list ofcolumnsz for the table
    /// </summary>
    public List<Column> Columns
    { get { return m_columns; } }

    /// <summary>
    /// Gets the database this table belongs to
    /// </summary>
    public Database Database
    { get { return m_database; } }

    #endregion

    #region methods

    /// <summary>
    /// Determines if the two tables are eaual
    /// </summary>
    /// <param name="table1"></param>
    /// <param name="table2"></param>
    /// <returns></returns>
    public bool Equals(Table table1, Table table2)
    {
      bool equals = false;

      if (table1 != null && table2 != null)
        equals = table1.Equals(table2);
      else if (table1 == null && table2 == null)
        equals = true;

      return equals;

    } // Equals(Table table1, Table table2)


    /// <summary>
    /// Provides a string representation of this object.
    /// </summary>
    /// <returns>A string representation of this object.</returns>
    public override string ToString()
    { return m_name; } // ToString()


    /// <summary>
    /// Creates the Product from the xml
    /// </summary>
    /// <param name="database">Database this table is linked to</param>
    /// <param name="xml">The xml to de-serialize.</param>
    /// <returns>Product object.</returns>
    static public Table Create(Database database, string xml)
    {
 
      XmlDocument doc = new XmlDocument();
      doc.LoadXml(xml);

      return Create(database, doc.DocumentElement);

    } // Create( xml )


    /// <summary>
    /// Creates the Product from the xml
    /// </summary>
    /// <param name="xml">The xml to de-serialize.</param>
    /// <returns>Product object.</returns>
    static public Table Create(Database database, XmlNode node)
    {
      Table table = new Table();

      if (table == null)
        throw new OutOfMemoryException("Failed to create Table object");

      table.m_database = database;
      table.Load(node);

      return table;
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
          writer.WriteAttributeString(Attributes.Resource, m_resource);
          writer.WriteAttributeString(Attributes.ServicePermissions, ((Int32)m_servicePermissions).ToString());

          if (m_columns.Count > 0)
          {
            writer.WriteStartElement(Attributes.Columns);

            foreach (Column column in m_columns)
            {
              writer.WriteRaw(column.ToXml());
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
      node.TryGetAttribute<string>(Attributes.Description, ref m_description);


      string perms = string.Empty;

      if (node.TryGetAttribute<string>(Attributes.ServicePermissions, ref perms))
        m_servicePermissions = (Permissions)Enum.Parse(typeof(Permissions), perms);


      if (!node.TryGetAttribute<string>(Attributes.Resource, ref m_resource))
      {
        m_resource = m_name;
        m_entityType = Type.GetType(string.Format("Viatronix.Enterprise.Entities.{0}", m_resource));

        if (m_entityType == null)
        {
          if (m_name.EndsWith("ies"))
            m_resource = m_name.Substring(0, m_name.Length - 3) + "y";
          else if (m_name.EndsWith("s"))
            m_resource = m_name.Substring(0, m_name.Length - 1);


         
          m_entityType = Type.GetType(string.Format("Viatronix.Enterprise.Entities.{0}", m_resource));
        }
      }


      if (m_entityType  == null)
        throw new ViatronixException(string.Format("No entity defined for this resource {0}", m_resource), ErrorCodes.None);

      XmlNode columns = node.SelectSingleNode(Attributes.Columns);

      if (columns != null)
      {

        foreach ( XmlNode column in columns.SelectNodes("column") )
        {
          m_columns.Add(Column.Create(this, column));
        }
      }


    } // Load( node )

    /// <summary>
    /// Clones the application object
    /// </summary>
    /// <returns></returns>
    public override object Clone()
    {
      Table table = new Table();

      if (table == null)
        throw new OutOfMemoryException("Failed to create Table object");

      table.m_database = m_database;
      table.m_description = m_description;
      table.m_name = m_name;
      table.m_resource = m_resource;
      table.m_columns.AddRange(m_columns);

      return table;

    } // Clone()


    /// <summary>
    /// Checks for value equality of task objects
    /// </summary>
    /// <param name="obj">A job to compare with this instance or a null reference</param>
    /// <returns>true if they are equal values; otherwise false</returns>
    public override bool Equals(IEntity obj)
    {
      Table other = obj as Table;

      return other != null &&
             base.Equals(other) && 
              Database.Equals(m_database, other.m_database ) &&
                string.Equals(m_name, other.m_name);

    } // Equals( obj )


    #endregion


  } // class Table

} // namespace Viatronix.Enterprise.Entities
