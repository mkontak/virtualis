using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;


namespace Viatronix.Enterprise.Entities
{

  /// <summary>
  /// Implements the ProcessHistory object from the auditing database
  /// </summary>
  public class AgentJobHistory : Entity
  {


    #region class Attributes

    /// <summary>
    /// Mirrors the database
    /// </summary>
    public static class Attributes
    {
      public const string Id = "id";
      public const string Date = "date";
      public const string Procedure = "proc";
      public const string Message = "msg";
      public const string State = "state";
      public const string JobName = "name";
    };

    #endregion

    #region fields

    /// <summary>
    /// Identifier
    /// </summary>
    private UInt64 m_id;

    /// <summary>
    /// Name
    /// </summary>
    private string m_name = string.Empty;

    /// <summary>
    /// procedure
    /// </summary>
    private string m_procedure = string.Empty;

    /// <summary>
    /// error message
    /// </summary>
    private string m_message = string.Empty;

    /// <summary>
    /// State
    /// </summary>
    private States m_state;

    /// <summary>
    /// Date
    /// </summary>
    private DateTimeOffset? m_date = null;

 
    #endregion

    #region properties

    /// <summary>
    /// Identifier
    /// </summary>
    public UInt64 Id
    { get { return m_id; } }

 
    /// <summary>
    /// Agent Name
    /// </summary>
    public String JobName
    { get { return m_name; } }

    /// <summary>
    /// Gets the procedure
    /// </summary>
    public string Procedure
    { get { return m_procedure; } }

    /// <summary>
    /// Gets Processing start date
    /// </summary>
    public DateTimeOffset Date
    { get { return m_date.Value; } }

    /// <summary>
    /// State of processings
    /// </summary>
    public States State
    { get { return m_state; } }

 
    /// <summary>
    /// Gets the error message
    /// </summary>
    public string ErrorMessage
    { get { return m_message; } }


    #endregion

    #region methods

    /// <summary>
    /// String representation of the object (uses the id)
    /// </summary>
    /// <returns></returns>
    public override string ToString()
    {
      return m_id.ToString();
    } // ToString()

    static public AgentJobHistory Create(AgentJob job, States state, string message = "")
    {
       AgentJobHistory history = new AgentJobHistory();

      if (history == null)
        throw new OutOfMemoryException("Failed to create AgentJobHistory object");

      history.m_date = DateTime.Now;
      history.m_name = job.Name;
      history.m_procedure = job.Procedure;
      history.m_state = state;
      history.m_message = message;

      return history;
    }

    /// <summary>
    /// Creates the Product from the xml
    /// </summary>
    /// <param name="xml">The xml to de-serialize.</param>
    /// <returns>Product object.</returns>
    static public AgentJobHistory Create(string xml)
    {

      XmlDocument doc = new XmlDocument();
      doc.LoadXml(xml);

      return Create(doc.DocumentElement);

    } // Create( xml )


    /// <summary>
    /// Creates the Product from the xml
    /// </summary>
    /// <param name="xml">The xml to de-serialize.</param>
    /// <returns>Product object.</returns>
    static public AgentJobHistory Create(XmlNode node)
    {
      AgentJobHistory history = new AgentJobHistory();

      if (history == null)
        throw new OutOfMemoryException("Failed to create AgentJobHistory object");

      history.Load(node);

      return history;
    } // Create( node )

    #endregion

    #region IEntity

    /// <summary>
    /// Creates an xml representation of a directory object.
    /// </summary>
    /// <returns>Xlm string representing the directory object.</returns>
    public override string ToXml()
    {
      StringBuilder builder = new StringBuilder();
      using (XmlWriter writer = XmlWriter.Create(builder, new XmlWriterSettings() { OmitXmlDeclaration = true, Indent = false }))
      {


        writer.WriteStartElement(this.Tag);
        {

          writer.WriteAttributeString(Attributes.Id, m_id.ToString());
          writer.WriteAttributeString(Attributes.Date, m_date.Value.ToString("o"));
          writer.WriteAttributeString(Attributes.Procedure, m_procedure);
          writer.WriteAttributeString(Attributes.JobName, m_name);
          writer.WriteAttributeString(Attributes.Message, m_message);
          writer.WriteAttributeString(Attributes.State, ((int)m_state).ToString() );
 
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

      m_id = UInt64.Parse(node.Attributes[Attributes.Id].Value.ToString());
      m_procedure = node.Attributes[Attributes.Procedure].Value.ToString();
      m_name = node.Attributes[Attributes.JobName].Value.ToString();
      m_date = DateTimeOffset.Parse(node.Attributes[Attributes.Date].Value.ToString());
      m_state = (States)Enum.Parse(typeof(States), node.Attributes[Attributes.State].Value);
      m_message = node.Attributes[Attributes.Message].Value.ToString();


    } // Load( node )

    /// <summary>
    /// Clones the directory object
    /// </summary>
    /// <returns></returns>
    public override object Clone()
    {
      AgentJobHistory history = new AgentJobHistory();

      if (history == null)
        throw new OutOfMemoryException("Failed to create AgentJobHistory object");


      history.m_id = m_id;
      history.m_date = m_date;
      history.m_message = m_message;
      history.m_procedure = m_procedure;
      history.m_state = m_state;
      history.m_name = m_name;

      return history;

    } // Clone()


    /// <summary>
    /// Checks for value equality of task objects
    /// </summary>
    /// <param name="obj">A job to compare with this instance or a null reference</param>
    /// <returns>true if they are equal values; otherwise false</returns>
    public override bool Equals(IEntity obj)
    {
      AgentJobHistory other = obj as AgentJobHistory;

      return other != null &&
             base.Equals(other) &&
              (m_id == other.m_id);

    } // Equals( obj )


    #endregion

  } // class AgentJobHistory

} // namespace Viatronix.Enterprise.Entities
