using System;
using System.Collections;
using System.IO;
using System.Text;
using System.Xml;
using System.Data;
using System.Data.SqlClient;
using System.Collections.Generic;
using System.Runtime.Serialization;
using Viatronix.Enterprise.Logging;


namespace Viatronix.Enterprise.Entities
{
  /// <summary>
  /// Volume data 
  /// </summary>
  public class Volume : ExtendedEntity
  {

    #region constructors

    private Volume() { }

    #endregion

    #region class Attributes

    /// <summary>
    /// user fields directly related to the user table in the database
    /// </summary>
    public static class Attributes
    {
      public const string ConversionId        = "cid";
      public const string Uid                 = "uid";
      public const string Name                = "name";
      public const string Application         = "app";
      public const string Date                = "date";
      public const string FileName            = "file";
      public const string ImageCount          = "icnt";
      public const string ExtendedProperties  = "ext";
      public const string ProcessedAs         = "as";
      public const string Url                 = "url";

    } // class Attributes

    #endregion


    #region fields

    /// <summary>
    /// Xml Node Name
    /// </summary>
    public const string XmlTag = "volume";

    /// <summary>
    /// Version
    /// </summary>
    private const string m_version = "4.0";

    /// <summary>
    /// Conversion id
    /// </summary>
    private string m_conversionId = string.Empty;

    /// <summary>
    /// Volume file name
    /// </summary>
    private string m_filename = string.Empty;

    /// <summary>
    /// Series Uid
    /// </summary>
    private string m_uid = string.Empty;

    /// <summary>
    /// Volume name
    /// </summary>
    private string m_name = string.Empty;

    /// <summary>
    /// Dtae the volume was created 
    /// </summary>
    private DateTimeOffset m_date = System.DateTimeOffset.Now;

    /// <summary>
    /// Application
    /// </summary>
    private string m_application = string.Empty;

    /// <summary>
    /// Number of images in series
    /// </summary>
    private int m_imageCount = 0;


    /// <summary>
    /// Extended properties
    /// </summary>
    private string m_extendedProperties = string.Empty;

    /// <summary>
    /// URL (Location of volume)
    /// </summary>
    private string m_url = string.Empty;

    #endregion


    #region properties

    /// <summary>
    /// Gets the objcet version
    /// </summary>
    public override string Version
    {
      get { return m_version; }
    }

    /// <summary>
    /// Gets the conversion id
    /// </summary>
    public string ConversionId
    { get { return m_conversionId; } }

    /// <summary>
    /// Volume filename 
    /// </summary>
    public string FileName
    {
      get { return m_filename; }
      set { SetProperty<string>("FileName", value, ref m_filename); }
    } // File



    /// <summary>
    /// Volume name
    /// </summary>
    public string Name
    {
      get { return m_name; }
      set { SetProperty<string>("Name", value, ref m_name); }
    } // Name


 
    /// <summary>
    /// Gets/Sets the Series Uid
    /// </summary>
    public string SeriesUid
    {
      get { return m_uid; } 
    } // Uid



    /// <summary>
    /// Gets/Sets the volume Date and Time
    /// </summary>
    public DateTimeOffset Date
    {
      get { return m_date; }
      set { SetProperty<DateTimeOffset>("Date", value, ref m_date); }
    } // Date

    /// <summary>
    /// Gets the URL
    /// </summary>
    public string Url
    {
      get { return m_url ; }
    } // Url


    /// <summary>
    /// Gets/Sets the volume application type
    /// </summary>
    public string Application
    {
      get { return m_application; }
      set { SetProperty<string>("Application", value, ref m_application); }
    } // Application

    /// <summary>
    /// Gets/Sets the number of images in the series
    /// </summary>
    public int ImageCount
    {
      get { return m_imageCount; }
      set { SetProperty<int>("ImageCount", value, ref m_imageCount); }
    } // ImageCOunt

    #endregion

    #region methods




    /// <summary>
    /// Check the subtype specified agains the internal field case insensitive
    /// </summary>
    /// <param name="application">application to compare to</param>
    /// <returns>true of the application equals the one specified</returns>
    public bool IsApplication(string application)
    {
      return (string.Compare(m_application, application, true) == 0);
    } // IsSubType(string subType)

     /// <summary>
    /// Returns String Representation of this object (Uid)
    /// </summary>
    /// <returns>string representation of the object</returns>
    public override string ToString()
    {
      return string.Format("{0}-{1}",m_uid,m_name);
    } // ToString()


    /// <summary>
    /// Gets the hash code for this object
    /// </summary>
    /// <returns></returns>
    public override int GetHashCode()
    {
      return ToString().GetHashCode();
    } // GetHashCode()



    #endregion

    #region static methods



    /// <summary>
    /// Creates a new instance of a volume object from xml file.
    /// </summary>
    /// <param name="conversionId">Conversion id (bucket or bin name)</param>
    /// <param name="uid">Series uid</param>
    /// <param name="name">Name of volume</param>
    /// <returns>A Study object.</returns>
    public static Volume Create(string conversionId, string uid, string name)
    {
      Volume volume = new Volume();
      if (volume == null)
        throw new OutOfMemoryException("Failed to allocate volume");

      volume.m_conversionId = conversionId;
      volume.m_uid = uid;
      volume.m_name = name;

      return volume;

    } // Create(string uid, string name)


    /// <summary>
    /// Creates a new instance of a volume object from xml file.
    /// </summary>
    /// <param name="xmlOrFile">The xml file.</param>
    /// <returns>A Study object.</returns>
    public static Volume Create(string xmlOrFile)
    {

      XmlDocument doc = new XmlDocument();

      if (!xmlOrFile.StartsWith("<"))
      {

        if (!System.IO.File.Exists(xmlOrFile))
        {
          Log.Error("The specified series xml file could not be found [FILE=" + xmlOrFile + "]", "Server", "FromFile");
          throw new System.IO.FileNotFoundException("Series xml file was not found");
        }

        doc.Load(xmlOrFile);

      }
      else
        doc.LoadXml(xmlOrFile);



      return Create(doc.DocumentElement);

    } // Create(string xmlFile)

    /// <summary>
    /// Creates a new instance of a volume object from xml.
    /// </summary>
    /// <param name="node">The xml used to reconstruct a Series object.</param>
    /// <returns>A Series object.</returns>
    public static Volume Create(XmlNode node)
    {
      Volume volume = new Volume();

      if (volume == null)
        throw new OutOfMemoryException("Failed to allocation volume");


      volume.Load(node);



      return volume;
    } // Create( node )





    #endregion


    #region IEntity

    /// <summary>
    /// Creates an xml representation of a User object.
    /// </summary>
    /// <returns>Xml string representing the User object.</returns>
    public override string ToXml()
    {

      StringBuilder builder = new StringBuilder();
      using (XmlWriter writer = XmlWriter.Create(builder, new XmlWriterSettings() { OmitXmlDeclaration = true, Indent = false }))
      {


        writer.WriteStartElement(XmlTag);
        {


          writer.WriteAttributeString(Attributes.Uid, m_uid);
          writer.WriteAttributeString(Attributes.ConversionId, m_conversionId);
          writer.WriteAttributeString(Attributes.Name, m_name);
          writer.WriteAttributeString(Attributes.Date, m_date.ToString("O"));
          writer.WriteAttributeString(Attributes.Application, m_application);
          writer.WriteAttributeString(Attributes.FileName, m_filename);
          writer.WriteAttributeString(Attributes.ImageCount, m_imageCount.ToString());

          if ( ! string.IsNullOrEmpty(m_url) )
            writer.WriteAttributeString(Attributes.Url, m_url);


          WriteAttributeStrings(writer);

          if (m_extendedProperties.Length > 0)
          {
            writer.WriteStartElement("ext");
            writer.WriteRaw(m_extendedProperties);
            writer.WriteEndElement();

          }

          WriteElements(writer);


        }

        writer.WriteEndElement();
      }

      return builder.ToString();

    } // ToXml()


    /// <summary>
    /// Creates a new instance of a volume object from xml.
    /// </summary>
    /// <param name="node">The xml used to reconstruct a Series object.</param>
    /// <returns>A Series object.</returns>
    public override void Load(XmlNode node)
    {

        base.Load(node);

      string version = string.Empty;
      node.TryGetAttributeValue<string>(EntityAttributes.Version, ref version);



      m_uid = node.Attributes[Attributes.Uid].Value;
      m_conversionId = node.Attributes[Attributes.ConversionId].Value;
      m_name = (node.Attributes[Attributes.Name].Value);
      m_date = DateTimeOffset.Parse(node.Attributes[Attributes.Date].Value);
      m_filename = node.Attributes[Attributes.FileName].Value;
  
      m_application = node.Attributes[Attributes.Application].Value;
      m_imageCount = Int32.Parse(node.Attributes[Attributes.ImageCount].Value);
      node.TryGetNodeValue(Attributes.ExtendedProperties, ref m_extendedProperties);
      node.TryGetAttributeValue<string>(Attributes.Url, ref m_url);

    } // Load( node )


    /// <summary>
    /// Clones the volue
    /// </summary>
    /// <returns></returns>
    public override object Clone()
    {
      Volume volume = new Volume();

      if (volume == null)
        throw new OutOfMemoryException("Failed to allocation volume");


      volume.m_uid = m_uid;
      volume.m_conversionId = m_conversionId;
      volume.m_name = m_name;
      volume.m_date = m_date;
      volume.m_filename = m_filename;
      volume.m_application = m_application;
      volume.m_date = m_date;
      volume.m_extendedProperties = m_extendedProperties;
      volume.m_url = m_url;

      return volume;

    } // Clone()

    /// <summary>
    /// Checks for value equality of task objects
    /// </summary>
    /// <param name="obj">A job to compare with this instance or a null reference</param>
    /// <returns>true if they are equal values; otherwise false</returns>
    public override bool Equals(IEntity obj)
    {
      Volume other = obj as Volume;

      return other != null &&
             base.Equals(other) &&
              string.Equals(m_uid, other.m_uid) &&
                string.Equals(m_conversionId, other.m_conversionId) &&
                  string.Equals(m_name, other.m_name);

    } // Equals( obj )


    #endregion

  } // class Volume

}
