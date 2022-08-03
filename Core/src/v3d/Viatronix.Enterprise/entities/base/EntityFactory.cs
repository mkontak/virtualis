using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Xml;
using System.Xml.Linq;
using Viatronix.Enterprise.Logging;
using Viatronix.Enterprise.Entities;

namespace Viatronix.Enterprise
{


  /// <summary>
  /// Factory used to create core data entity objects from xml
  /// </summary>
  public static class EntityFactory
  {

    /// <summary>
    /// Creates an IEntity from xml
    /// </summary>
    /// <typeparam name="T">IModel type</typeparam>
    /// <param name="node">xml</param>
    /// <returns>instance of IModel</returns>
    public static IEntity Create(XmlNode node)
    {
      if (node == null)
      {
        Log.Error("Unknown object xml.", "CoreFactory", "Create");
        throw new NullReferenceException("Unknown object xml.");
      }


      string name = node.Name;
 

      System.Reflection.Assembly assembly = System.Reflection.Assembly.GetExecutingAssembly();
      string[] fields = assembly.ManifestModule.Name.Split(new char[] { '.' });
      Type entityType = null;

      foreach (Type type in assembly.GetTypes())
      {
        if (string.Compare(type.Name, name, true) == 0)
        {
          entityType = type;
          break;
        }
      }


      if (entityType == null)
        throw new NotImplementedException(string.Format("Entity {0} is not implemented/defind cannot create", name));

      Entity entity  = (Entity)Activator.CreateInstance(entityType);

      entity.Load(node);

      return entity;
    } // Create( node )


    /// <summary>
    /// Creates an IModel from xml
    /// </summary>
    /// <typeparam name="T">IModel type</typeparam>
    /// <param name="node">xml</param>
    /// <returns>instance of IModel</returns>
    public static T Create<T>(XmlNode node) where T : IEntity
    {
      T instance = Activator.CreateInstance<T>();
      instance.Load(node);
      return instance;
    } // Create( node )


    /// <summary>
    /// Creates an IModel from xml
    /// </summary>
    /// <typeparam name="T">IModel type</typeparam>
    /// <param name="node">xml</param>
    /// <returns>instance of IModel</returns>
    public static IEntity Create(string xml)
    {
      XmlDocument doc = new XmlDocument();
      doc.LoadXml(xml);

      return Create(doc.DocumentElement);
    } // Create( xml )


    /// <summary>
    /// Creates an IModel from xml
    /// </summary>
    /// <typeparam name="T">IModel type</typeparam>
    /// <param name="node">xml</param>
    /// <returns>instance of IModel</returns>
    public static T Create<T>(string xml) where T : IEntity
    {
      if (string.IsNullOrWhiteSpace(xml))
        return default(T);

      return Create<T>(xml.ToXmlNode());
    } //  Create( xml )


    /// <summary>
    /// Creates an IModel from xml
    /// </summary>
    /// <typeparam name="T">IModel type</typeparam>
    /// <param name="node">xml</param>
    /// <returns>instance of IModel</returns>
    public static IEntity Create(XElement element)
    {
      return Create(element.GetXmlNode());
    } // Create( element )


    /// <summary>
    /// Creates an IModel from xml
    /// </summary>
    /// <typeparam name="T">IModel type</typeparam>
    /// <param name="node">xml</param>
    /// <returns>instance of IModel</returns>
    public static T Create<T>(XElement element) where T : IEntity
    {
      return Create<T>(element.GetXmlNode());
    } // Create( element )


    /// <summary>
    /// Creates an IModel from a file
    /// </summary>
    /// <typeparam name="T">IModel type</typeparam>
    /// <param name="file">xml file</param>
    /// <returns>instance of IModel</returns>
    public static IEntity CreateFromFile(string file)
    {
      using (FileStream stream = File.OpenRead(file))
        return CreateFromStream(stream);
    } // CreateFromFile( file )


    /// <summary>
    /// Creates an IModel from a file
    /// </summary>
    /// <typeparam name="T">IModel type</typeparam>
    /// <param name="file">xml file</param>
    /// <returns>instance of IModel</returns>
    public static T CreateFromFile<T>(string file) where T : IEntity
    {
      using (FileStream stream = File.OpenRead(file))
        return (T)CreateFromStream(stream);
    } // CreateFromFile( file )


    /// <summary>
    /// Creates an IModel from a stream
    /// </summary>
    /// <typeparam name="T">IModel type</typeparam>
    /// <param name="stream">xml</param>
    /// <returns>instance of IModel</returns>
    public static IEntity CreateFromStream(Stream stream)
    {
      XmlDocument doc = new XmlDocument();
      doc.Load(stream);

      return Create(doc.DocumentElement);
    } // CreateFromStream( stream )


    /// <summary>
    /// Creates an IModel from a stream
    /// </summary>
    /// <typeparam name="T">IModel type</typeparam>
    /// <param name="stream">xml</param>
    /// <returns>instance of IModel</returns>
    public static T CreateFromStream<T>(Stream stream) where T : IEntity
    {
      XmlDocument doc = new XmlDocument();
      doc.Load(stream);

      return Create<T>(doc.DocumentElement);
    } // CreateFromStream( stream )


 


  } // class CoreObjectFactory
} // namespace Viatronix
