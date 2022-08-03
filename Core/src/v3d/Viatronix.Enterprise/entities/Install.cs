using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;


namespace Viatronix.Enterprise.Entities
{

  /// <summary>
  /// Implements a install resource that holds an entry containing the information about the systems installation
  /// </summary>
  public class Install : Entity
  {


    #region class Attributes

    /// <summary>
    /// Mirrors the database
    /// </summary>
    public static class Attributes
    {
      public const string SystemId = "sysid";
      public const string Date = "date";
      public const string SystemDate = "sysdate";
      public const string ExpirationDate = "expdate";
      public const string Enabled = "enabled";
      public const string ErrorMessage = "msg";
      public const string LicenseCount = "cnt";
      public const string LicenseServer = "ls";
    };

    #endregion

    #region fields

    /// <summary>
    /// System id
    /// </summary>
    private string m_systemId = string.Empty;

    /// <summary>
    /// Enabled
    /// </summary>
    private bool m_enabled = true;

    /// <summary>
    /// License Server
    /// </summary>
    private string m_licenseServer = string.Empty;

    /// <summary>
    /// Error message
    /// </summary>
    private string m_message = string.Empty;

    /// <summary>
    /// Date
    /// </summary>
    private DateTimeOffset m_date = DateTimeOffset.Now;

    /// <summary>
    /// System Date
    /// </summary>
    private DateTimeOffset m_systemDate = DateTimeOffset.Now;

    /// <summary>
    /// Expiration Date
    /// </summary>
    private DateTimeOffset m_expirationDate = DateTimeOffset.Now;

    /// <summary>
    /// Number of licenses issued
    /// </summary>
    private int m_licenseCount = 0;

    #endregion


    #region properties


    /// <summary>
    /// Gets the system id
    /// </summary>
    public string SystemId
    {
      get { return m_systemId; }
    }

    /// <summary>
    /// Gets the install path
    /// </summary>
    public bool IsEnabled
    {
      get { return m_enabled; }
      set 
      { 
        SetProperty<bool>("IsEnabled", value, ref m_enabled);
        if (value)
          ErrorMessage = string.Empty;
      }
    }

    /// <summary>
    /// Gets the number of licenses issued
    /// </summary>
    public int LicensesIssued
    {
      get { return m_licenseCount; }
    }

    /// <summary>
    /// License server this system is linked to
    /// </summary>
    public string LicenseServer
    { get { return m_licenseServer; } }

    /// <summary>
    /// Gets the install error message
    /// </summary>
    public string ErrorMessage
    {
      get { return m_message; }
      set { SetProperty<string>("ErrorMessage", value, ref m_message); }
    }

    /// <summary>
    /// Gets the install date 
    /// </summary>
    public DateTimeOffset InstallDate
    {
      get { return m_date; }
    }

    /// <summary>
    /// Gets the system date 
    /// </summary>
    public DateTimeOffset SystemDate
    {
      get { return m_systemDate; }
    }

    /// <summary>
    /// Gets the system date 
    /// </summary>
    public DateTimeOffset ExpirationDate
    {
      get { return m_expirationDate; }
    }

    #endregion

    #region methods

    /// <summary>
    /// Provides a string representation of this object.
    /// </summary>
    /// <returns>A string representation of this object.</returns>
    public override string ToString()
    { return m_systemId; } // ToString()


    /// <summary>
    /// Creates the Product from the xml
    /// </summary>
    /// <param name="xml">The xml to de-serialize.</param>
    /// <returns>Product object.</returns>
    static public Install Create(string xml)
    {
      Install install = new Install();

      if (install == null)
      {
        throw new OutOfMemoryException("Failed to create Install object");
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
    static public Install Create(XmlNode node)
    {
      Install install = new Install();

      if (install == null)
        throw new OutOfMemoryException("Failed to create Install object");

      install.Load(node);

      return install;
    } // Create( node )

    #endregion

    #region IEntity

    /// <summary>
    /// Creates an xml representation of a install object.
    /// </summary>
    /// <returns>Xlm string representing the install object.</returns>
    public override string ToXml()
    {
      StringBuilder builder = new StringBuilder();
      using (XmlWriter writer = XmlWriter.Create(builder, new XmlWriterSettings() { OmitXmlDeclaration = true, Indent = false }))
      {


        writer.WriteStartElement(this.Tag);
        {


          writer.WriteAttributeString(Attributes.SystemId, m_systemId);
          writer.WriteAttributeString(Attributes.Enabled, m_enabled.ToString());
          writer.WriteAttributeString(Attributes.ErrorMessage, m_message);
          writer.WriteAttributeString(Attributes.LicenseCount, m_licenseCount.ToString());
          writer.WriteAttributeString(Attributes.LicenseServer, m_licenseServer);
          writer.WriteAttributeString(Attributes.Date, m_date.ToString("0"));
          writer.WriteAttributeString(Attributes.SystemDate, m_systemDate.ToString("0")); 
          writer.WriteAttributeString(Attributes.ExpirationDate, m_expirationDate.ToString("0"));

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

      m_systemId = node.Attributes[Attributes.SystemId].Value.ToString();
      m_licenseServer = node.Attributes[Attributes.LicenseServer].Value.ToString();
      m_licenseCount = Int32.Parse(node.Attributes[Attributes.LicenseCount].Value.ToString());
      int enabled = Int32.Parse(node.Attributes[Attributes.Enabled].Value.ToString());
      m_enabled = (enabled == 1 ? true : false);
      m_message = node.Attributes[Attributes.ErrorMessage].Value.ToString();

      m_date = DateTimeOffset.Parse(node.Attributes[Attributes.Date].Value.ToString());
      m_systemDate = DateTimeOffset.Parse(node.Attributes[Attributes.SystemDate].Value.ToString());
      m_expirationDate = DateTimeOffset.Parse(node.Attributes[Attributes.ExpirationDate].Value.ToString());

    } // Load( node )

    /// <summary>
    /// Clones the install object
    /// </summary>
    /// <returns></returns>
    public override object Clone()
    {
      Install install = new Install();

      if (install == null)
        throw new OutOfMemoryException("Failed to create Install object");


      install.m_systemId = m_systemId;
      install.m_licenseServer = m_licenseServer;
      install.m_message = m_message;
      install.m_licenseCount = m_licenseCount;
      install.m_date = m_date;
      install.m_systemDate = m_systemDate;
      install.m_expirationDate = m_expirationDate;
      install.m_enabled = m_enabled;


      return install;

    } // Clone()


    /// <summary>
    /// Checks for value equality of task objects
    /// </summary>
    /// <param name="obj">A job to compare with this instance or a null reference</param>
    /// <returns>true if they are equal values; otherwise false</returns>
    public override bool Equals(IEntity obj)
    {
      Install other = obj as Install;

      return other != null &&
             base.Equals(other) &&
              string.Equals(m_systemId, other.m_systemId);

    } // Equals( obj )


    #endregion

  } // class Install

} // namespace Viatronix.Enterprise.Entities