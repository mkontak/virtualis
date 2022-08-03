using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;


namespace Viatronix.Enterprise.Entities
{

  


  /// <summary>
  /// Identifies a application
  /// </summary>
  public class Application : Entity
  {


    #region class Attributes 

    /// <summary>
    /// Mirrors the database
    /// </summary>
    public static class Attributes
    {
      public const string Id            = "id";
      public const string Description   = "desc";
      public const string Name          = "name";
      public const string Cad           = "cad";
      public const string Mask          = "mask";
      public const string Installed     = "inst";
      public const string Default       = "default";
      public const string LuanchRule    = "launch";
      public const string Url           = "url";
      public const string Priority      = "pri";
    };

    #endregion

    #region fields

    /// <summary>
    /// Application name
    /// </summary>
    public string m_name = string.Empty;

    /// <summary>
    /// Installed flag
    /// </summary>
    private bool m_installed = false;
  
    /// <summary>
    /// id of application
    /// </summary>
    private string m_id = string.Empty;

    /// <summary>
    /// Description
    /// </summary>
    private string m_description = string.Empty;

    /// <summary>
    /// Types
    /// </summary>
    private string m_types = string.Empty;

    /// <summary>
    /// Default processing flag
    /// </summary>
    private bool m_default = false;

    /// <summary>
    /// Mask
    /// </summary>
    private int m_mask = 0;

    /// <summary>
    /// Cad Installed Count for application  
    /// </summary>
    private int m_cadInstalled;

    /// <summary>
    /// Launchg rule
    /// </summary>
    private string m_launchRule = string.Empty;

    /// <summary>
    /// Url
    /// </summary>
    private string m_url = string.Empty;

    /// <summary>
    /// Priority
    /// </summary>
    private int m_priority = 1;

    #endregion

    #region properties


    /// <summary>
    /// Gets the application id ( VC, VAS, CAR, ... )
    /// </summary>
    public string Id
    {
      get { return m_id; }
      private set { SetProperty<string>("Id", value, ref m_id); }
    }

    /// <summary>
    /// Gets the application Name
    /// </summary>
    public string Name
    {
      get { return m_name; }
      set { SetProperty<string>("Name", value, ref m_name); }
    }

    /// <summary>
    /// Gets the application description
    /// </summary>
    public string Description
    {
      get { return m_description; }
      private set { SetProperty<string>("Description", value, ref m_description); }
    }

    /// <summary>
    /// Gets the application types
    /// </summary>
    public string Types
    {
      get { return m_types; }
      private set { SetProperty<string>("Types", value, ref m_types); }
    }

    /// <summary>
    /// Gets the application Mask
    /// </summary>
    public int Mask
    {
      get { return m_mask; }
      private set { SetProperty<int>("Mask", value, ref m_mask); }
    }

    /// <summary>
    /// Gets the application has cad
    /// </summary>
    public bool HasCad
    {
      get { return ( m_cadInstalled > 0 ? true : false ) ; }
    }

    /// <summary>
    /// INstalled flag (True if the application is installed, flase if not )
    /// </summary>
    public bool Installed
    {
      get { return m_installed; }
      set { SetProperty<bool>("Installed", value, ref m_installed); }
    }

    /// <summary>
    /// Deaault flag (True if the application is a the default for processing if we cannt determine a application type  )
    /// </summary>
    public bool Default
    {
      get { return m_default; }
      set { SetProperty<bool>("Default", value, ref m_default); }
    }

    /// <summary>
    /// Launch rule
    /// </summary>
    public string LaunchRule
    {
      get { return m_launchRule; }
      set { SetProperty<string>("LaunchRule", value, ref m_launchRule); }
    }

    /// <summary>
    /// Url
    /// </summary>
    public string Url
    {
      get { return m_url; }
      set { SetProperty<string>("Url", value, ref m_url); }
    }

    /// <summary>
    /// Gets/Sets the applications priority in the ordering/launching
    /// </summary>
    public int Priority
    { 
      get { return m_priority;  }
      set { SetProperty<int>("Priority", value, ref m_priority);  }
    }

    #endregion

    #region methods

    /// <summary>
    /// Provides a string representation of this object.
    /// </summary>
    /// <returns>A string representation of this object.</returns>
    public override string ToString()
    { return m_name;  } // ToString()

 
    /// <summary>
    /// Creates the Product from the xml
    /// </summary>
    /// <param name="xml">The xml to de-serialize.</param>
    /// <returns>Product object.</returns>
    static public Application Create(string xml)
    {
      Application application = new Application();

      if (application == null)
      {
        throw new OutOfMemoryException("Failed to create Application object");
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
    static public Application Create(XmlNode node)
    {
      Application application = new Application();

      if (application == null)
        throw new OutOfMemoryException("Failed to create Application object");

      application.Load(node);

      return application;
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


          writer.WriteAttributeString(Attributes.Id, m_id);
          writer.WriteAttributeString(Attributes.Name, m_name);
          writer.WriteAttributeString(Attributes.Description, m_description);
          writer.WriteAttributeString(Attributes.Cad, m_cadInstalled.ToString());
          writer.WriteAttributeString(Attributes.Mask, m_mask);
          writer.WriteAttributeString(Attributes.Url, m_url);
          writer.WriteAttributeString(Attributes.Installed, m_installed);
          writer.WriteAttributeString(Attributes.Priority, m_priority.ToString());

          if (!string.IsNullOrEmpty(m_launchRule))
          {
            writer.WriteStartElement(Attributes.LuanchRule);
            writer.WriteAttributeString(Attributes.Url, m_url);
            
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

      m_id = node.Attributes[Attributes.Id].Value.ToString();
      m_name = node.Attributes[Attributes.Name].Value.ToString();
      m_description = node.Attributes[Attributes.Description].Value.ToString();
      node.TryGetAttributeValue<string>(Attributes.LuanchRule, ref m_launchRule);
      node.TryGetAttributeValue<string>(Attributes.Url, ref m_url);
      node.TryGetAttributeValue<int>(Attributes.Mask, ref m_mask);
      node.TryGetAttributeValue<int>(Attributes.Cad, ref m_cadInstalled);
      node.TryGetAttributeValue<int>(Attributes.Priority, ref m_priority);


      int installed = 0;
      node.TryGetAttributeValue<int>(Attributes.Installed, ref installed);
      m_installed = (installed == 0 ? false : true);

      int defaultProcessing = 0;
      node.TryGetAttributeValue<int>(Attributes.Default, ref defaultProcessing);
      m_default = (defaultProcessing == 0 ? false : true);



    } // Load( node )

    /// <summary>
    /// Clones the application object
    /// </summary>
    /// <returns></returns>
    public override object Clone()
    {
      Application application = new Application();

      if (application == null)
        throw new OutOfMemoryException("Failed to create Application object");


      application.m_id = m_id;
      application.m_name = m_name;
      application.m_description = m_description;
      application.m_types = m_types;
      application.m_mask = m_mask;
      application.m_cadInstalled = m_cadInstalled;
      application.m_installed = m_installed;
      application.m_default = m_default;
      application.m_url = m_url;
      application.m_launchRule = m_launchRule;
      application.m_priority = m_priority;


      return application;

    } // Clone()


    /// <summary>
    /// Checks for value equality of task objects
    /// </summary>
    /// <param name="obj">A job to compare with this instance or a null reference</param>
    /// <returns>true if they are equal values; otherwise false</returns>
    public override bool Equals(IEntity obj)
    {
      Application other = obj as Application;

      return other != null &&
             base.Equals(other) &&
              string.Equals(m_id, other.m_id);

    } // Equals( obj )


    #endregion

  } // class Application

} // namespace Viatronix.Enterprise.Entities
