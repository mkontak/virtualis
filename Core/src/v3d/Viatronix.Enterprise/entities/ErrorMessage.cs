using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;


namespace Viatronix.Enterprise.Entities
{

  /// <summary>
  /// Implements the ErrorMessage resource. This is a read-only object that holds any errors that occurred at the resource level.
  /// </summary>
  public class ErrorMessage : Entity
  {


    #region class Attributes

    /// <summary>
    /// Mirrors the database
    /// </summary>
    public static class Attributes
    {
      public const string Number = "num";
      public const string Severity = "severity";
      public const string State = "state";
      public const string Procedure = "proc";
      public const string LineNumber = "line";
      public const string Message = "msg";
      public const string Date = "date";
    };

    #endregion

    #region fields

    /// <summary>
    /// ErrorMessage name
    /// </summary>
    public int m_number = 0;

    /// <summary>
    /// ErrorMessage severity
    /// </summary>
    private int m_severity = 0;

    /// <summary>
    /// ErrorMessage state
    /// </summary>
    private int m_state = 0;

    /// <summary>
    /// Procedure that generated the error
    /// </summary>
    private string m_procedure = string.Empty;

    /// <summary>
    /// Line Number in procedure
    /// </summary>
    private int m_lineNumber = 0;

    /// <summary>
    /// Error Message
    /// </summary>
    private string m_message = string.Empty;

    /// <summary>
    /// Date of error
    /// </summary>
    private DateTimeOffset m_date = DateTimeOffset.Now;

 
    #endregion

    #region properties


    /// <summary>
    /// Gets the error rnumber
    /// </summary>
    public int Number
    {
      get { return m_number; }
    }

    /// <summary>
    /// Gets the error severity
    /// </summary>
    public int Severity
    {
      get { return m_severity; }
    }

    /// <summary>
    /// Gets the error state
    /// </summary>
    public int State
    {
      get { return m_state; }
    }

    /// <summary>
    /// Gets the procedure the error occurred
    /// </summary>
    public string Procedure
    {
      get { return m_procedure; }
    }

    /// <summary>
    /// Gets the line number in the procedure
    /// </summary>
    public int LineNumber
    {
      get { return m_lineNumber; }
    }

    /// <summary>
    /// Gets the message 
    /// </summary>
    public string Message
    {
      get { return m_message; }
    }

    /// <summary>
    /// Date of the error
    /// </summary>
    public DateTimeOffset Date
    {
      get { return m_date; }
    }

 
    #endregion

    #region methods

    /// <summary>
    /// Provides a string representation of this object.
    /// </summary>
    /// <returns>A string representation of this object.</returns>
    public override string ToString()
    { return string.Format("{0}-{1}", m_number, m_message); } // ToString()


    /// <summary>
    /// Creates the Product from the xml
    /// </summary>
    /// <param name="xml">The xml to de-serialize.</param>
    /// <returns>Product object.</returns>
    static public ErrorMessage Create(string xml)
    {
      ErrorMessage message = new ErrorMessage();

      if (message == null)
      {
        throw new OutOfMemoryException("Failed to create ErrorMessage object");
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
    static public ErrorMessage Create(XmlNode node)
    {
      ErrorMessage message = new ErrorMessage();

      if (message == null)
        throw new OutOfMemoryException("Failed to create ErrorMessage object");

      message.Load(node);

      return message;
    } // Create( node )

    #endregion

    #region IEntity

    /// <summary>
    /// Creates an xml representation of a message object.
    /// </summary>
    /// <returns>Xlm string representing the message object.</returns>
    public override string ToXml()
    {
      StringBuilder builder = new StringBuilder();
      using (XmlWriter writer = XmlWriter.Create(builder, new XmlWriterSettings() { OmitXmlDeclaration = true, Indent = false }))
      {


        writer.WriteStartElement(this.Tag);
        {


          writer.WriteAttributeString(Attributes.Number, m_number.ToString());
          writer.WriteAttributeString(Attributes.Severity, m_severity.ToString());
          writer.WriteAttributeString(Attributes.State, m_state.ToString());
          writer.WriteAttributeString(Attributes.Procedure, m_procedure);
          writer.WriteAttributeString(Attributes.LineNumber, m_lineNumber.ToString());
          writer.WriteAttributeString(Attributes.Message, m_message);
          writer.WriteAttributeString(Attributes.Date, m_date.ToString("0"));
 

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

      m_number = Int32.Parse(node.Attributes[Attributes.Number].Value.ToString());
      m_severity = Int32.Parse(node.Attributes[Attributes.Severity].Value.ToString());
      m_state = Int32.Parse(node.Attributes[Attributes.State].Value.ToString());
      m_procedure = node.Attributes[Attributes.Procedure].Value.ToString();
      m_message = node.Attributes[Attributes.Message].Value.ToString();
      m_lineNumber = Int32.Parse(node.Attributes[Attributes.LineNumber].Value.ToString());
      m_date = DateTimeOffset.Parse(node.Attributes[Attributes.Date].Value.ToString());



    } // Load( node )

    /// <summary>
    /// Clones the message object
    /// </summary>
    /// <returns></returns>
    public override object Clone()
    {
      ErrorMessage message = new ErrorMessage();

      if (message == null)
        throw new OutOfMemoryException("Failed to create ErrorMessage object");


      message.m_number = m_number;
      message.m_severity = m_severity;
      message.m_state = m_state;
      message.m_procedure = m_procedure;
      message.m_message = m_message;
      message.m_lineNumber = m_lineNumber;
      message.m_date = m_date;


      return message;

    } // Clone()


    /// <summary>
    /// Checks for value equality of task objects
    /// </summary>
    /// <param name="obj">A job to compare with this instance or a null reference</param>
    /// <returns>true if they are equal values; otherwise false</returns>
    public override bool Equals(IEntity obj)
    {
      return false;
    } // Equals( obj )


    #endregion

  } // class ErrorMessage

} // namespace Viatronix.Enterprise.Entities
