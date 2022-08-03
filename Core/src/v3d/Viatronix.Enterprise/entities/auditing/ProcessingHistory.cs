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
  public class ProcessingHistory : Entity
  {


    #region class Attributes

    /// <summary>
    /// Mirrors the database
    /// </summary>
    public static class Attributes
    {
      public const string Id = "id";
      public const string Date = "date";
      public const string Name = "name";
      public const string SeriesUid = "series";
      public const string StudyUid = "study";
      public const string StartDate = "stdte";
      public const string Modality = "mod";
      public const string Application = "app";
      public const string TemplateId = "tid";
      public const string Message = "msg";
      public const string State = "state";
      public const string Orientation = "ort";
      public const string Process = "proc";
    };

    #endregion

    #region fields

    /// <summary>
    /// Identifier
    /// </summary>
    private UInt64 m_id;

    /// <summary>
    /// application
    /// </summary>
    private string m_application = string.Empty;

    /// <summary>
    /// template id
    /// </summary>
    private string m_templateId = string.Empty;

    /// <summary>
    /// Modality 
    /// </summary>
    private string m_modality = string.Empty;

    /// <summary>
    /// PatientName 
    /// </summary>
    private string m_name = string.Empty;

    /// <summary>
    /// Series Uid 
    /// </summary>
    private string m_seriesUid = string.Empty;

    /// <summary>
    /// Study Uid 
    /// </summary>
    private string m_studyUid = string.Empty;

    /// <summary>
    /// Error message
    /// </summary>
    private string m_message = string.Empty;

    /// <summary>
    /// Orientation
    /// </summary>
    private string m_orientation = string.Empty;

    /// <summary>
    /// State
    /// </summary>
    private States m_state;

    /// <summary>
    /// Date
    /// </summary>
    private DateTimeOffset? m_date = null;

    /// <summary>
    /// StartDate
    /// </summary>
    private DateTimeOffset? m_startDate = null;

    /// <summary>
    /// Task
    /// </summary>
    private Task m_task;

    #endregion

    #region properties

    /// <summary>
    /// Identifier
    /// </summary>
    public UInt64 Id
    { get { return m_id; } }

    /// <summary>
    /// Gets Processing start date
    /// </summary>
    public DateTimeOffset StartDate
    { get { return m_startDate.Value; } }

    /// <summary>
    /// Gets the processing end date
    /// </summary>
    public DateTimeOffset EndDate
    { get { return m_date.Value; }  }

    /// <summary>
    /// State of processings
    /// </summary>
    public States State
    { get { return m_state; } }

    /// <summary>
    /// Gets patient name
    /// </summary>
    public string PatientName
    {  get { return m_name; }  }

    /// <summary>
    /// Gets the orientation
    /// </summary>
    public string Orientation
    { get { return m_orientation; } }

    /// <summary>
    /// Gets application code
    /// </summary>
    public string Application
    { get { return m_application; } }

    /// <summary>
    /// Gets modality code
    /// </summary>
    public string Modality
    { get { return m_modality; } }

    /// <summary>
    /// Gets studyt uid
    /// </summary>
    public string StudyUid
    { get { return m_studyUid; } }

    /// <summary>
    /// Gets series uid
    /// </summary>
    public string SeriesUid
    { get { return m_seriesUid; } }

    /// <summary>
    /// Gets the task
    /// </summary>
    public Task Task
    { get { return m_task; } }

    /// <summary>
    /// Gets template id
    /// </summary>
    public string TemplateId
    { get { return m_templateId; } }

 
    /// <summary>
    /// Gets the error message
    /// </summary>
    public string ErrorMessage
    {  get { return m_message; } }

    

 
 

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


    /// <summary>
    /// Creates the Product from the xml
    /// </summary>
    /// <param name="xml">The xml to de-serialize.</param>
    /// <returns>Product object.</returns>
    static public ProcessingHistory Create(string xml)
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
    static public ProcessingHistory Create(XmlNode node)
    {
      ProcessingHistory history = new ProcessingHistory();

      if (history == null)
        throw new OutOfMemoryException("Failed to create ProcessingHistory object");

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
          writer.WriteAttributeString(Attributes.Name, m_name);
          writer.WriteAttributeString(Attributes.SeriesUid , m_seriesUid);
          writer.WriteAttributeString(Attributes.StudyUid, m_studyUid);
          writer.WriteAttributeString(Attributes.StartDate, m_startDate.Value.ToString("o"));
          writer.WriteAttributeString(Attributes.Modality, m_modality);
          writer.WriteAttributeString(Attributes.Application, m_application);
          writer.WriteAttributeString(Attributes.TemplateId, m_templateId);

          writer.WriteAttributeString(Attributes.Message, m_message);
          writer.WriteAttributeString(Attributes.State, ((int)m_state).ToString());
          writer.WriteAttributeString(Attributes.Orientation, m_orientation);

          writer.WriteStartElement(Attributes.Process);
          writer.WriteRaw(m_task.ToXml());
          writer.WriteEndElement();

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

      m_id = UInt64.Parse(node.Attributes[Attributes.Id].Value.ToString());

      m_name = node.Attributes[Attributes.Name].Value.ToString();
      m_modality = node.Attributes[Attributes.Modality].Value.ToString();
      m_application = node.Attributes[Attributes.Application].Value.ToString();
      m_orientation = node.Attributes[Attributes.Orientation].Value.ToString();
      m_templateId = node.Attributes[Attributes.TemplateId].Value.ToString();


      m_seriesUid = node.Attributes[Attributes.SeriesUid].Value.ToString();
      m_studyUid = node.Attributes[Attributes.StudyUid  ].Value.ToString();
      m_message = node.Attributes[Attributes.Message].Value.ToString();
      m_date = DateTimeOffset.Parse(node.Attributes[Attributes.Date].Value.ToString());
      m_startDate = DateTimeOffset.Parse(node.Attributes[Attributes.StartDate].Value.ToString());
      m_state = (States)Enum.Parse(typeof(States),node.Attributes[Attributes.State].Value);


      XmlNode process = node.SelectSingleNode(Attributes.Process);

      if (process != null && process.ChildNodes.Count > 0 )
      {
        m_task = Task.Create(process.ChildNodes[0]);
      }

    } // Load( node )

    /// <summary>
    /// Clones the directory object
    /// </summary>
    /// <returns></returns>
    public override object Clone()
    {
      ProcessingHistory history = new ProcessingHistory();

      if (history == null)
        throw new OutOfMemoryException("Failed to create ProcessingHistory object");


      history.m_id = m_id;
      history.m_startDate = m_startDate;
      history.m_message = m_message;
      history.m_seriesUid = m_seriesUid;
      history.m_date = m_date;
      history.m_orientation = m_orientation;
      history.m_name = m_name;
      history.m_modality = m_modality;
      history.m_application = m_application;
      history.m_templateId = m_templateId;
      history.m_task = (Task)m_task.Clone();
      history.m_state = m_state;


      return history;

    } // Clone()


    /// <summary>
    /// Checks for value equality of task objects
    /// </summary>
    /// <param name="obj">A job to compare with this instance or a null reference</param>
    /// <returns>true if they are equal values; otherwise false</returns>
    public override bool Equals(IEntity obj)
    {
      ProcessingHistory other = obj as ProcessingHistory;

      return other != null &&
             base.Equals(other) &&
              (m_id == other.m_id);

    } // Equals( obj )


    #endregion

  } // class ProcessingHistory

} // namespace Viatronix.Enterprise.Entities
