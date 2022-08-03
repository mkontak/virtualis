


using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.IO;
using System.Xml.Schema;
using System.Configuration;

using Viatronix.Enterprise;

namespace Viatronix
{
  public static class Schema
  {

    #region fields

    /// <summary>
    /// XmlReaderSettings
    /// </summary>
    private static XmlReaderSettings m_settings = null;

    /// <summary>
    /// Load schema event
    /// </summary>
    public static event EventHandler<EventArgs<XmlSchemaSet>> Load = null;

    #endregion

    #region properties

    /// <summary>
    /// Returns the xml schemas for core
    /// </summary>
    public static XmlReaderSettings Settings
    {
      get
      {
        LoadSchemas();

        return m_settings;
      }
    } // Core

    #endregion

    #region methods

    /// <summary>
    /// Loads all the schemas into memory
    /// </summary>
    public static void LoadSchemas()
    {
      if (m_settings == null)
      {
        m_settings = new XmlReaderSettings() { ValidationType = ValidationType.Schema };
        OnLoad();
      }

    } // LoadSchemas()


    /// <summary>
    /// Load event
    /// </summary>
    /// <param name="args">args</param>
    private static void OnLoad()
    {
      string schemaDirectory = ConfigurationManager.AppSettings["schemas"];

      if ( string.IsNullOrEmpty(schemaDirectory) )
        throw new MissingAttributeException("Missing schemas attribute");

      if (!Directory.Exists(schemaDirectory))
        throw new NotFoundException(String.Format("Specifies schema directory [{0}] was not found", schemaDirectory));

      Logging.Log.Information(string.Format("Loading schemas from {0}", schemaDirectory), "Schema", "OnLoad");
 
      string[] schemas = System.IO.Directory.GetFiles(schemaDirectory, "*.xsd");

      // ================================================
      // Loop though all the schema files and load them
      // ================================================
      foreach (string schema in schemas)
      {
        try
        {
          m_settings.Schemas.Add(null, XmlReader.Create(new FileStream(schema, FileMode.Open)));
        }
        catch ( Exception e )
        {
          throw new ViatronixException(string.Format("Bad schema {0} : {1}", schema, e.Message), e, ErrorCodes.None); 
        }
      }


    } // OnLoad( args )

    #endregion

  } // class Schema
} // namespace Viatronix
