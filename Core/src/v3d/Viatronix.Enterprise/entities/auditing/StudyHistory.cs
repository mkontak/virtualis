using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;


namespace Viatronix.Enterprise.Entities
{

  /// <summary>
  /// Implements the StudyHistory object from the auditing database
  /// </summary>
  public class StudyHistory : Entity
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
      public const string Action = "action";
      public const string StudyUid = "uid";
      public const string Status = "status";
      public const string Login = "login";
      public const string UserName = "username";
      public const string Message = "msg";
      public const string ExtendedProperties = "ext";
    };

    #endregion

    #region fields

    /// <summary>
    /// Identifier
    /// </summary>
    private UInt64 m_id;

    /// <summary>
    /// Action (Insert, Delete, Update)
    /// </summary>
    private string m_action = string.Empty;

    /// <summary>
    /// Status
    /// </summary>
    private bool m_status = false;

    /// <summary>
    /// Login 
    /// </summary>
    private string m_login = string.Empty;

    /// <summary>
    /// User Name 
    /// </summary>
    private string m_userName = string.Empty;


    /// <summary>
    /// Study Uid 
    /// </summary>
    private string m_studyUid = string.Empty;

    /// <summary>
    /// Error message
    /// </summary>
    private string m_message = string.Empty;

 
    /// <summary>
    /// Date
    /// </summary>
    private DateTimeOffset? m_date = null;

 
    /// <summary>
    /// Extended properties
    /// </summary>
    private string m_properties;

    /// <summary>
    /// Patient name
    /// </summary>
    private string m_name = string.Empty;

    #endregion

    #region properties

    /// <summary>
    /// Identifier
    /// </summary>
    public UInt64 Id
    { get { return m_id; } }

    /// <summary>
    /// Gets patient name
    /// </summary>
    public string PatientName
    { get { return m_name; } }

    /// <summary>
    /// Gets studyt uid
    /// </summary>
    public string StudyUid
    { get { return m_studyUid; } }

    /// <summary>
    /// Gets Processing start date
    /// </summary>
    public string Action
    { get { return m_action; } }

    /// <summary>
    /// Gets the processing end date
    /// </summary>
    public DateTimeOffset Date
    { get { return m_date.Value; } }

    /// <summary>
    /// Status of the action
    /// </summary>
    public bool Succeded
    { get { return m_status; } }


    /// <summary>
    /// Gets the login
    /// </summary>
    public string Login
    { get { return m_login; } }

    /// <summary>
    /// Gets user's name
    /// </summary>
    public string UserName
    { get { return m_userName; } }



    /// <summary>
    /// Gets the error message
    /// </summary>
    public string ErrorMessage
    { get { return m_message; } }

    /// <summary>
    /// Gets the error message
    /// </summary>
    public string ExtendedProperties
    { get { return m_properties; } }





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
    static public StudyHistory Create(string xml)
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
    static public StudyHistory Create(XmlNode node)
    {
      StudyHistory history = new StudyHistory();

      if (history == null)
        throw new OutOfMemoryException("Failed to create StudyHistory object");

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
          writer.WriteAttributeString(Attributes.Action, m_action);
          writer.WriteAttributeString(Attributes.StudyUid, m_studyUid);
          writer.WriteAttributeString(Attributes.Status, (m_status ? "1" : "0" ));
          writer.WriteAttributeString(Attributes.Login, m_login);
          writer.WriteAttributeString(Attributes.UserName, m_userName);
          writer.WriteAttributeString(Attributes.Message, m_message);

          writer.WriteStartElement(Attributes.ExtendedProperties);
          writer.WriteRaw(m_properties);
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
      m_action = node.Attributes[Attributes.Action].Value.ToString();
      m_status = (node.Attributes[Attributes.Status].Value.ToString() == "1" ? true : false);

      m_login = node.Attributes[Attributes.Login].Value.ToString();
      m_userName = node.Attributes[Attributes.UserName].Value.ToString();
      m_studyUid = node.Attributes[Attributes.StudyUid].Value.ToString();
      m_message = node.Attributes[Attributes.Message].Value.ToString();
      m_date = DateTimeOffset.Parse(node.Attributes[Attributes.Date].Value.ToString());

      XmlNode ext = node.SelectSingleNode(Attributes.ExtendedProperties);

      if (ext != null && ext.ChildNodes.Count > 0)
      {
        m_properties = ext.InnerXml;
      }

    } // Load( node )

    /// <summary>
    /// Clones the directory object
    /// </summary>
    /// <returns></returns>
    public override object Clone()
    {
      StudyHistory history = new StudyHistory();

      if (history == null)
        throw new OutOfMemoryException("Failed to create StudyHistory object");


      history.m_id = m_id;
      history.m_action = m_action;
      history.m_message = m_message;
      history.m_status = m_status;
      history.m_date = m_date;
      history.m_properties = m_properties;
      history.m_name = m_name;
      history.m_userName = m_userName;
      history.m_login = m_login ;


      return history;

    } // Clone()


    /// <summary>
    /// Checks for value equality of task objects
    /// </summary>
    /// <param name="obj">A job to compare with this instance or a null reference</param>
    /// <returns>true if they are equal values; otherwise false</returns>
    public override bool Equals(IEntity obj)
    {
      StudyHistory other = obj as StudyHistory;

      return other != null &&
             base.Equals(other) &&
              (m_id == other.m_id);

    } // Equals( obj )


    #endregion

  } // class StudyHistory

} // namespace Viatronix.Enterprise.Entities
