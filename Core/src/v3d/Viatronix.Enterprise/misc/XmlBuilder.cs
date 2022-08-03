using System;
using System.Xml;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Globalization;
using System.Linq;
using System.Text;
using Viatronix.Enterprise.Entities;

namespace Viatronix.Enterprise
{


 
  /// <summary>
  /// Base Xml builder used for creating xml payloads
  /// </summary>
  public class XmlBuilder
  {

    #region fields

    /// <summary>
    /// Version 
    /// </summary>
    private readonly string m_version = "4.0";

    /// <summary>
    /// Resource name
    /// </summary>
    private string m_resource = string.Empty;

    /// <summary>
    /// Xml
    /// </summary>
    protected string m_xml = string.Empty;

    /// <summary>
    ///  Attributes
    /// </summary>
    protected Dictionary<string, string> m_attributes = new Dictionary<string, string>();

    /// <summary>
    /// Inner Xml
    /// </summary>
    protected string m_innerXml = string.Empty;

    #endregion


    #region constructors

    /// <summary>
    /// Constructs a query builder for the resource specified
    /// </summary>
    /// <param name="resource"></param>
    public XmlBuilder(string resource)
    {
      m_resource = resource;

      AddAttribute("ver", m_version);
    }

 
    #endregion



    #region properties

    /// <summary>
    /// Get/Set the inner xml
    /// </summary>
    public string InnerXml
    {
      get { return m_innerXml; }
      set
      {
        XmlDocument doc = new XmlDocument();
        doc.LoadXml(value);
        m_innerXml = value;
        m_xml = string.Empty;

      }
    } // InnerXml

    /// <summary>
    /// Gets the attributes
    /// </summary>
    public Dictionary<string, string> Attributes
    { get { return m_attributes;  } }

    /// <summary>
    /// Gets the resource
    /// </summary>
    public string Resource
    { get { return m_resource; } }

    #endregion


    #region create methods

    /// <summary>
    /// Creates the xml builder using just the entity
    /// </summary>
    /// <param name="entity"></param>
    /// <returns></returns>
    public static XmlBuilder Create(IEntity entity)
    { return Create(entity.Tag, entity.ToXml()); }

    /// <summary>
    /// Creates the xml builder
    /// </summary>
    /// <param name="resource">Specify the resource name (Lower Case)</param>
    /// <param name="xml">Xml body</param>
    /// <returns></returns>
    public static  XmlBuilder Create(string resource, string xml)
    {
      XmlBuilder builder = Create(resource);

      builder.m_innerXml = xml;

      XmlDocument doc = new XmlDocument();

      doc.LoadXml(xml);

      // ============================================================================================================================================
      // If the xml's tag name is the same as the resource or attributes then assume attributes and process as normal. Otherwise assume inner xml
      // ============================================================================================================================================
      if (string.Compare(doc.DocumentElement.Name, resource, true) == 0 || string.Compare(doc.DocumentElement.Name, "attributes", true) == 0)
      {
        builder.m_innerXml = string.Empty;

        foreach (XmlAttribute attribute in doc.DocumentElement.Attributes)
        {
          builder.AddAttribute<string>(attribute.Name, attribute.Value);
        }

        foreach (XmlNode child in doc.DocumentElement.ChildNodes)
        {
          string from = string.Empty;
          string to = string.Empty;

          if (child.TryGetAttributeValue<string>("from", ref from) && child.TryGetAttributeValue<string>("to", ref to))
          {
            builder.AddAttributeRange<string>(child.Name, from, to);
          }
        }
      }
        

      return builder;
    } // Create(string resource)


    /// <summary>
    /// Creates the query builder
    /// </summary>
    /// <param name="resource">Specify the resource name (Lower Case)</param>
    /// <returns></returns>
    public static XmlBuilder Create(string resource)
    {
      XmlBuilder query = new XmlBuilder(resource);

      if (query == null)
        throw new OutOfMemoryException("Failed to allocate XmlBuilder");



      return query;

    } // Create(string resource)

    /// <summary>
    /// Creates the xml builder
    /// </summary>
    /// <param name="resource">Specify the resource name (Lower Case)</param>
    /// <param name="attributes">Attribute list</param>
    /// <returns></returns>
    public static XmlBuilder Create(string resource, Dictionary<string, string> attributes)
    {
      XmlBuilder query = Create(resource);


      foreach (KeyValuePair<string, string> pair in attributes)
      {
        if ( ! query.m_attributes.ContainsKey(pair.Key) )
         query.m_attributes.Add(pair.Key, pair.Value);
      }

      return query;
    } // Create(string resource)

    #endregion


    #region methods

    /// <summary>
    /// Tries to get the attribute and if it exists will set the value and return true, otherwise returns false
    /// </summary>
    /// <typeparam name="T"></typeparam>
    /// <param name="name"></param>
    /// <param name="value"></param>
    /// <returns></returns>
    public bool TryGetAttributeValue<T>(string name, ref T value)
    {
      bool found = false;


      if (m_attributes.ContainsKey(name))
      {

        if (m_attributes[name] != null)
        {
          found = true;

          value = (T)Convert.ChangeType(m_attributes[name], typeof(T));
        }
      }

      return found;
    } // TryGetAttributeValue<T>(string name, ref T value)

    /// <summary>
    /// Adds attribute
    /// </summary>
    /// <param name="name"></param>
    /// <param name="value"></param>
    public void AddAttribute<T>(string name, T value)
    {
      if (m_attributes.ContainsKey(name))
        m_attributes[name] = ( value == null ? string.Empty : value.ToString() );
      else
        m_attributes.Add(name, (value == null ? string.Empty : value.ToString()));

      m_xml = string.Empty;

    }

     /// <summary>
    /// Adds attribute range
    /// </summary>
    /// <param name="name"></param>
    /// <param name="from"></param>
    /// <param name="to"></param>
    public void AddAttributeRange<T>(string name, T from, T to)
    {
      if (m_attributes.ContainsKey(name))
        m_attributes[name] = string.Format("RANGE:{0},{1}", from.ToString(), to.ToString());
      else
        m_attributes.Add(name, string.Format("RANGE:{0},{1}", from.ToString(), to.ToString()));

      m_xml = string.Empty;

    }


    /// <summary>
    /// Generates the XML for the query attributes defined
    /// </summary>
    /// <returns></returns>
    public string ToXml()
    {

      if (string.IsNullOrEmpty(m_xml))
      {

        // List of ranges
        List<KeyValuePair<string, string>> ranges = new List<KeyValuePair<string, string>>();

        StringBuilder xml = new StringBuilder();

        // Start tag
        xml.Append(string.Format("<{0} ", m_resource));

        // ===============================
        // Add each attribute to the xml
        // ===============================
        foreach (KeyValuePair<string, string> pair in m_attributes)
        {

          // ============================
          // Make sure the key is valid
          // =============================
          if (!string.IsNullOrEmpty(pair.Key))
          {

            // ==================================
            // Make sure the value is not null
            // ==================================
            if (pair.Value != null)
            {
              if (pair.Value.StartsWith("RANGE:"))
                ranges.Add(pair);
              else if (!string.IsNullOrEmpty(pair.Value))
                xml.Append(string.Format(" {0}=\"{1}\" ", pair.Key, pair.Value));
            } // END ... Value is not null

          } // END .. If the key is noy empty or NULL

        } // END ... For each attribute


        // ====================================
        // If there are ranges we add then now
        // ====================================
        if (ranges.Count > 0)
        {

          // End 
          xml.Append(">");
          Char[] del = { ',' };

          // ===========================================================
          // Loop through all the ranges and add the xml node elements
          // ===========================================================
          foreach (KeyValuePair<string, string> pair in ranges)
          {
            string[] fields = pair.Value.Substring(6).Split(del);
            xml.Append(string.Format("<{0}  from=\"{1}\" to=\"{2}\" />", pair.Key, fields[0], fields[1]));
          } // END ... For each range

          // If the inner xml is set then insert it
          if (!string.IsNullOrEmpty(m_innerXml))
            xml.Append(m_innerXml);

          // End Tag
          xml.Append(string.Format("</{0}>", m_resource));

        }
        else
        {

          // If the inner xml is set then insert it
          if (!string.IsNullOrEmpty(m_innerXml))
          {
            xml.Append(">");
            xml.Append(m_innerXml);
            xml.Append(string.Format("</{0}>", m_resource));
          }
          else
            xml.Append("/>");
        }

        m_xml =  xml.ToString();
      }

      return m_xml;

    } // ToXml()


    #endregion

  } // class XmlBuilder

} // namespace Viatronix.Enterprise
