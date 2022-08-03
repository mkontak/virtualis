// $Id: Parameter.cs,v 1.13.2.1 2008/01/03 16:14:04 mkontak Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]

using System;
using System.Collections;
using System.Collections.Specialized;
using System.IO;
using System.Text;
using System.Xml;
using System.Data;
using System.Runtime.Serialization;
using Viatronix.Enterprise.Logging;

namespace Viatronix.Enterprise.Entities
{



  #region Parameter class

  /// <summary>
  /// Parameter object relates directly to database parameter table
  /// </summary>
  public class Parameter : Entity
  {

    #region Attributes class

    public class Attributes
    {
      public const string Name        = "name";
      public const string Value       = "value";
      public const string CommandLine = "cmd";
    };  // class Attributes

    #endregion


    #region constants


    /// <summary>
    /// xml tag
    /// </summary>
    public const string XmlTag = "parameter";


    #endregion

    #region fields

    /// <summary>
    /// Version
    /// </summary>
    private readonly string m_version = "3.0";


    /// <summary>
    /// Parameter name
    /// </summary>
    private string m_name = string.Empty;

    /// <summary>
    /// Parameter value
    /// </summary>
    private string m_value = string.Empty;

    /// <summary>
    /// Command line
    /// </summary>
    private string m_commandLine = string.Empty;

 
    #endregion

    #region properties


    /// <summary>
    /// Gets the version
    /// </summary>
    public override string Version
    { get { return m_version; } }

    /// <summary>
    /// Sets/Gets Name
    /// </summary>
    public string Name
    {
      get { return m_name; }
      set { m_name = value; }
    } // Name

    /// <summary>
    /// Sets/Gets parameter value
    /// </summary>
    public string Value
    {
      get { return m_value; }
      set { m_value = value; }
    } // Value

    /// <summary>
    /// Gets/Sets command line
    /// </summary>
    public string CommandLine
    {
      get { return m_commandLine; }
      set { m_commandLine = value; }
    } // CommandLine

    #endregion


    #region methods

     /// <summary>
    /// String representation of the parameters (Used for logging)
    /// </summary>
    /// <returns>Returns the string representation of the Parameter</returns>
    public override string ToString()
    {
      return "[NAME=" + this.Name + "] [VALUE=" + this.Value + "] [COMMAND_LINE=" + this.CommandLine + "]";
    } // ToString()

    /// <summary>
    /// Determines equality
    /// </summary>
    /// <param name="obj">Object to determine equality with</param>
    /// <returns>Return true if equal</returns>
    public override bool Equals(object obj)
    {
      if (!(obj is Parameter))
      {
        throw new ArgumentException("Invalid type not Parameter");
      }

      return Equals((Parameter)obj);

    } // Equals

    /// <summary>
    /// Type specific equals
    /// </summary>
    /// <param name="parameter">Parameter to check equality against</param>
    /// <returns>TRue if equal</returns>
    public bool Equals(Parameter parameter)
    {
      return (string.Compare(parameter.Name, this.Name, true) == 0);
    } // Equals

    /// <summary>
    /// Gets the hash code for this object
    /// </summary>
    /// <returns></returns>
    public override int GetHashCode()
    {
      return base.GetHashCode();
    } // GetHashCode()

    /// <summary>
    /// Dumps the parameter object to the logger
    /// </summary>
    public virtual void ToLogger()
    {
      Log.Information("Parameter ... " + this.ToString(), "Parameter", "ToLogger");
    } // ToLogger()

 
    #endregion


    #region IEntity

    /// <summary>
    /// Creates an xml representation of a Series object.
    /// </summary>
    /// <returns>Xml string representing the Series object.</returns>
    public override string ToXml()
    {
      StringBuilder builder = new StringBuilder();
      using (XmlWriter writer = XmlWriter.Create(builder, new XmlWriterSettings() { OmitXmlDeclaration = true, Indent = false }))
      {

        writer.WriteStartElement(XmlTag);
        {

          writer.WriteAttributeString(Attributes.Name, m_name);
          writer.WriteAttributeString(Attributes.Value, m_value);
          writer.WriteAttributeString(Attributes.CommandLine, m_commandLine);
 
          WriteAttributeStrings(writer);
          WriteElements(writer);

        }
        writer.WriteEndElement();

       }

      return builder.ToString();

    } // ToXml()

    /// <summary>
    /// Loads the xml into the object
    /// </summary>
    /// <param name="node">The xml used to reconstruct a parameter object.</param>
    public override void Load(XmlNode node)
    {

      ///
      // node.ValidateSchema(Schema.Settings);

      m_name = node.Attributes[Attributes.Name].Value;
      m_value = node.Attributes[Attributes.Value].Value;

      if (node.Attributes[Attributes.CommandLine] != null)
      {
        m_commandLine = node.Attributes[Attributes.CommandLine].Value;
      }
      else if(node.Attributes["commandline"] != null)
      {
        m_commandLine = node.Attributes["commandline"].Value;
      }
    } // Load( node )

    /// <summary>
    /// Clones the object
    /// </summary>
    /// <returns>clone</returns>
    public override object Clone()
    {
      Parameter parameter = new Parameter();
      CopyTo(parameter);
    

      parameter.m_value = m_value;
      parameter.m_commandLine = m_commandLine;
      parameter.m_name = m_name;


      return parameter;
    } // Clone()


    /// <summary>
    /// Checks for value equality of task objects
    /// </summary>
    /// <param name="obj">A job to compare with this instance or a null reference</param>
    /// <returns>true if they are equal values; otherwise false</returns>
    public override bool Equals(IEntity obj)
    {
      Parameter other = obj as Parameter;

      return other != null &&
             base.Equals(other) &&
              string.Equals(m_name, other.m_name);

    } // Equals( obj )

    #endregion

    #region static methods

    /// <summary>
    /// Creates a new instance of a parameter object from xml.
    /// </summary>
    /// <param name="node">The xml used to reconstruct a parameter object.</param>
    /// <returns>A parameter object.</returns>
    public static Parameter Create(XmlNode node)
    {
      Parameter parameter = new Parameter();

      if (parameter == null)
        throw new OutOfMemoryException("Failed to allocate parameter");

      parameter.Load(node);

      return parameter;
    } // Create( node )


    #endregion static methods


  } // class Parameter


  #endregion Parameter class


} // namespace Viatronix.Enterprise.Entities

