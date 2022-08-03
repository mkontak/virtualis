using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Xml;
using System.Xml.Linq;
using Viatronix.Enterprise.Entities;

namespace Viatronix.Enterprise.Entities
{
  public static class EntityCollection
  {


    /// <summary>
    /// Loads CoreObjects from xml
    /// </summary>
    /// <typeparam name="T">element Type</typeparam>
    /// <param name="list">collection of CoreObjects</param>
    /// <param name="node">xml</param>
    public static void Load(this IList<IEntity> list, XmlNode node)
    {
      if (node == null)
        return;

      string xpath = (node.Name == "collection") ? "*" : "collection/*";
      foreach (XmlNode child in node.SelectNodes(xpath))
        list.Add(EntityFactory.Create(child));
    } // Load( node )


    /// <summary>
    /// Loads CoreObjects from xml
    /// </summary>
    /// <typeparam name="T">element Type</typeparam>
    /// <param name="list">collection of CoreObjects</param>
    /// <param name="node">xml</param>
    public static void Load<T>(this IList<T> list, XmlNode node) where T : IEntity
    {
      if (node == null)
        return;

      string tag = typeof(T).Name.ToLower();

      // Check for nodes directly under the the current  node
      XmlNodeList entityNodes = node.SelectNodes(tag);

      // =================================================================
      // If we have not found any then assume a sub-element collection
      // ================================================================
      if ( entityNodes.Count == 0  )
        entityNodes = node.SelectNodes(string.Format("collection/{0}", tag));

      foreach (XmlNode child in entityNodes)
        list.Add(EntityFactory.Create<T>(child));
    } // Load< T >( node )


    /// <summary>
    /// Loads CoreObjects from xml
    /// </summary>
    /// <typeparam name="TParent">parent Type</typeparam>
    /// <typeparam name="TChild">child Type</typeparam>
    /// <param name="list">collection of CoreObjects</param>
    /// <param name="node">xml</param>
    public static void Load<TParent, TChild>(this IList<Tuple<TParent, TChild>> list, XmlNode node)
      where TParent : IEntity
      where TChild : IEntity
    {
      string parentTag = typeof(TParent).Name.ToLower();
      string childTag = typeof(TChild).Name.ToLower();

      string xpathParent = (node.Name == "collection") ? parentTag : string.Format("collection/{0}", parentTag);
      foreach (XmlNode parentNode in node.SelectNodes(xpathParent))
      {
        TParent parent = EntityFactory.Create<TParent>(parentNode);
        foreach (XmlNode childNode in parentNode.SelectNodes(string.Format("collection/{0}", childTag)))
          list.Add(Tuple.Create<TParent, TChild>(parent, EntityFactory.Create<TChild>(childNode)));
      }
    } // Load< T >( node )


    /// <summary>
    /// Loads CoreObjects from xml
    /// </summary>
    /// <typeparam name="T">element Type</typeparam>
    /// <param name="list">collection of CoreObjects</param>
    /// <param name="element">xml</param>
    public static void Load(this IList<IEntity> list, XElement element)
    {
      list.Load(element.GetXmlNode());
    } // Load( element )


    /// <summary>
    /// Loads CoreObjects from xml
    /// </summary>
    /// <typeparam name="T">element Type</typeparam>
    /// <param name="list">collection of CoreObjects</param>
    /// <param name="element">xml</param>
    public static void Load<T>(this IList<T> list, XElement element) where T : IEntity
    {
      list.Load<T>(element.GetXmlNode());
    } // Load< T >( element )


    /// <summary>
    /// Loads CoreObjects from xml
    /// </summary>
    /// <typeparam name="TParent">parent Type</typeparam>
    /// <typeparam name="TChild">child Type</typeparam>
    /// <param name="list">collection of CoreObjects</param>
    /// <param name="element">xml</param>
    public static void Load<TParent, TChild>(this IList<Tuple<TParent, TChild>> list, XElement element)
      where TParent : IEntity
      where TChild : IEntity
    {
      list.Load<TParent, TChild>(element.GetXmlNode());
    } // Load< TParent, TChild >( XElement element )


    /// <summary>
    /// Loads CoreObjects from xml
    /// </summary>
    /// <typeparam name="T">element Type</typeparam>
    /// <param name="list">collection of CoreObjects</param>
    /// <param name="xml">xml</param>
    public static void Load(this IList<IEntity> list, string xml)
    {
      XDocument doc = XDocument.Parse(xml);
      list.Load(doc.Root);
    } // Load( xml )


    /// <summary>
    /// Loads CoreObjects from xml
    /// </summary>
    /// <typeparam name="T">element Type</typeparam>
    /// <param name="list">collection of CoreObjects</param>
    /// <param name="xml">xml</param>
    public static void Load<T>(this IList<T> list, string xml) where T : IEntity
    {
      list.Load<T>(xml.ToXmlNode());
    } // Load< T >( xml )


    /// <summary>
    /// Loads CoreObjects from xml
    /// </summary>
    /// <typeparam name="TParent">parent Type</typeparam>
    /// <typeparam name="TChild">child Type</typeparam>
    /// <param name="list">collection of CoreObjects</param>
    /// <param name="xml">xml</param>
    public static void Load<TParent, TChild>(this IList<Tuple<TParent, TChild>> list, string xml)
      where TParent : IEntity
      where TChild : IEntity
    {
      list.Load<TParent, TChild>(xml.ToXmlNode());
    } // Load< TParent, TChild >( xml )


    /// <summary>
    /// Outputs the collection as xml
    /// </summary>
    /// <param name="list">IModel list</param>
    /// <returns>xml</returns>
    public static string ToXml<T>(this IList<T> list) where T : IEntity
    {
      StringBuilder builder = new StringBuilder();
      builder.AppendLine("<collection>");

      foreach (IEntity obj in list)
        builder.AppendLine(obj.ToXml());

      builder.AppendLine("</collection>");
      return builder.ToString();
    } // ToXml( list )


    /// <summary>
    /// Checks for equality in IEntity collections
    /// </summary>
    /// <param name="list">IEntity list</param>
    /// <param name="other">other IEntity list</param>
    /// <returns>true if lists are equal; otherwise false</returns>
    public static bool Equals<T>(this IList<T> list, IList<T> other) where T : IEntity
    {
      bool result = (list.Count == other.Count);
      for (int i = 0; i < list.Count && result; ++i)
        result &= list[i].Equals(other[i]);

      return result;
    } // Equals<T>( list, other )

  } // class CoreObjectCollection
} // namespace Viatronix
