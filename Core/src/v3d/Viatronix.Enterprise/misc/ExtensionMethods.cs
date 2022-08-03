// Copyright © 2010, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Mark Kontak ( mailto:mkontak@viatronix.com )

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Data;
using System.Text.RegularExpressions;
using System.Text;
using System.Xml;
using System.Xml.Linq;
using System.Windows;
using System.Reflection;

using Viatronix.Enterprise;
using Viatronix.Enterprise.Data;
using Viatronix.Enterprise.Entities;


namespace Viatronix
{
  public static class ExtensionMethods
  {

    #region fields

    /// <summary>
    /// nullable type
    /// </summary>
    private static readonly Type NullableType = typeof(Nullable<>);

    /// <summary>
    /// empty string
    /// </summary>
    private static readonly object EmptyString = string.Empty;

    #endregion


    #region core methods

    /// <summary>
    /// Converts a XmlNode to a XElement
    /// </summary>
    /// <param name="node">XmlNode</param>
    /// <returns>XElement</returns>
    public static XElement GetXElement(this XmlNode node)
    {
      XDocument doc = new XDocument();
      using (XmlWriter writer = doc.CreateWriter())
        node.WriteTo(writer);

      return doc.Root;
    } // GetXElement( node )


    /// <summary>
    /// Converts a XElement to a XmlNode
    /// </summary>
    /// <param name="element">XElement</param>
    /// <returns>XmlNode</returns>
    public static XmlNode GetXmlNode(this XElement element)
    {
      using (XmlReader reader = element.CreateReader())
      {
        XmlDocument doc = new XmlDocument();
        doc.Load(reader);

        return doc;
      }
    } // GetXmlNode( element )


    /// <summary>
    /// Converts a string into XmlNode
    /// </summary>
    /// <param name="xml">xml string</param>
    /// <returns>XmlNode</returns>
    public static XmlNode ToXmlNode(this string xml)
    {
      XmlDocument doc = new XmlDocument();
      if (!string.IsNullOrWhiteSpace(xml))
        doc.LoadXml(xml);

      return doc.DocumentElement;
    } // ToXmlNode( xml )


    /// <summary>
    /// Converts a string into XElement
    /// </summary>
    /// <param name="xml">xml string</param>
    /// <returns>XElement</returns>
    public static XElement ToXElement(this string xml)
    {
      if (string.IsNullOrWhiteSpace(xml))
        throw new ArgumentNullException("Parameter is not xml data.");

      XDocument doc = XDocument.Parse(xml);
      return doc.Root;
    } // ToXmlNode( xml )


    /// <summary>
    /// Returns the short representaiont of a gender
    /// </summary>
    /// <param name="gender">gender</param>
    public static string ToShortString(this Genders gender)
    {
      return (gender == Genders.Unknown) ? string.Empty : gender.ToString().ToUpper().Substring(0, 1);
    } // ToShortString( settings )


    /// <summary>
    /// Attempts to retrieve the attribute as a string
    /// </summary>
    /// <param name="node">XmlNode</param>
    /// <param name="name">attribute name</param>
    /// <param name="obj">reference to string</param>
    /// <returns>true if successful; otherwise false</returns>
    public static bool TryGetAttribute(this XmlNode node, string name, ref string obj)
    {
      XmlAttribute attribute = node.Attributes[name];
      if (attribute != null && !string.IsNullOrEmpty(attribute.Value))
      {
        obj = attribute.Value;
        return true;
      }

      return false;
    } // TryGetAttribute( node, name, string )


    /// <summary>
    /// Attempts to retrieve the attribute as a integer (Invariant culture)
    /// </summary>
    /// <param name="node">XmlNode</param>
    /// <param name="name">attribute name</param>
    /// <param name="obj">reference to int</param>
    /// <returns>true if successful; otherwise false</returns>
    public static bool TryGetAttribute(this XmlNode node, string name, ref int obj)
    {
      XmlAttribute attribute = node.Attributes[name];
      if (attribute != null && !string.IsNullOrEmpty(attribute.Value))
      {
        obj = int.Parse(attribute.Value, CultureInfo.InvariantCulture);
        return true;
      }

      return false;
    } // TryGetAttribute( node, name, string )


    /// <summary>
    /// Attempts to retrieve the attribute as a integer (Invariant culture)
    /// </summary>
    /// <param name="node">XmlNode</param>
    /// <param name="name">attribute name</param>
    /// <param name="obj">reference to int</param>
    /// <returns>true if successful; otherwise false</returns>
    public static bool TryGetAttribute(this XmlNode node, string name, ref bool obj)
    {
      XmlAttribute attribute = node.Attributes[name];
      if (attribute != null && !string.IsNullOrEmpty(attribute.Value))
      {
        if (!bool.TryParse(attribute.Value, out obj))
          obj = (int.Parse(attribute.Value, CultureInfo.InvariantCulture) > 0);
        return true;
      }

      return false;
    } // TryGetAttribute( node, name, bool )


    /// <summary>
    /// Attempts to retrieve the attribute as a integer (Invariant culture)
    /// </summary>
    /// <param name="node">XmlNode</param>
    /// <param name="name">attribute name</param>
    /// <param name="obj">reference to int</param>
    /// <returns>true if successful; otherwise false</returns>
    public static bool TryGetAttribute(this XmlNode node, string name, ref DateTimeOffset obj)
    {
      XmlAttribute attribute = node.Attributes[name];
      if (attribute != null && !string.IsNullOrEmpty(attribute.Value))
        return DateTimeOffset.TryParse(attribute.Value, out obj);

      return false;
    } // TryGetAttribute( node, name, dateTimeOffset )


    /// <summary>
    /// Attempts to retrieve the attribute as a gender
    /// </summary>
    /// <param name="node">XmlNode</param>
    /// <param name="name">attribute name</param>
    /// <param name="obj">reference to int</param>
    /// <returns>true if successful; otherwise false</returns>
    public static bool TryGetAttribute(this XmlNode node, string name, ref Genders gender)
    {
      XmlAttribute attribute = node.Attributes[name];
      if (attribute != null && !string.IsNullOrEmpty(attribute.Value))
      {
        switch (attribute.Value)
        {
          case "M":
            gender = Genders.Male;
            break;
          case "F":
            gender = Genders.Female;
            break;
          case "O":
            gender = Genders.Other;
            break;
          default:
            gender = Genders.Unknown;
            return false;
        }

        return true;
      }

      return false;
    } // TryGetAttribute( node, name, gender )


    /// <summary>
    /// Attempts to retrieve the attribute (Invariant culture)
    /// </summary>
    /// <typeparam name="T">Type of the reference value</typeparam>
    /// <param name="node">XmlNode</param>
    /// <param name="name">attribute name</param>
    /// <param name="obj">reference to object</param>
    /// <returns>true if successful; otherwise false</returns>
    public static bool TryGetAttribute<T>(this XmlNode node, string name, ref T obj)
    {
      Type type = typeof(T);

      try
      {
        XmlAttribute attribute = node.Attributes[name];
        if (attribute != null && !string.IsNullOrEmpty(attribute.Value))
        {
          if (type.IsEnum)
            obj = (T)Enum.Parse(type, attribute.Value, true);

          else if (type.IsGenericType && type.GetGenericTypeDefinition().Equals(NullableType))
          {
            NullableConverter converter = new NullableConverter(type);
            obj = (T)converter.ConvertFromInvariantString(attribute.Value);
          }

          else
            obj = (T)Convert.ChangeType(attribute.Value, type, CultureInfo.InvariantCulture);

          return true;
        }
      }
      catch (Exception)
      {
        // Don't need to do anything, default value will be set for out parameter
      }

      return false;
    } // TryGetAttribute( name, obj )


    /// <summary>
    /// Compares the attribute's value and the object
    /// </summary>
    /// <typeparam name="T">Type of comparison</typeparam>
    /// <param name="node">XmlNode</param>
    /// <param name="name">attribute name</param>
    /// <param name="obj">object being compared</param>
    /// <returns></returns>
    public static int CompareAttribute<T>(this XmlNode node, string name, T obj) where T : class, IComparable
    {
      return node.CompareAttribute<T>(name, obj, null);
    } // CompareAttribute( name, obj )


    /// <summary>
    /// Compares the attribute's value and the object
    /// </summary>
    /// <typeparam name="T">Type of comparison</typeparam>
    /// <param name="node">XmlNode</param>
    /// <param name="name">attribute name</param>
    /// <param name="obj">object being compared</param>
    /// <returns></returns>
    public static int CompareAttribute<T>(this XmlNode node, string name, T obj, IComparer<T> comparer) where T : class, IComparable
    {
      T value = default(T);
      if (node.TryGetAttribute<T>(name, ref value))
      {
        if (comparer != null)
          return comparer.Compare(obj, value);

        IComparable<T> compareObj = obj as IComparable<T>;
        if (compareObj != null)
          return compareObj.CompareTo(value);
      }

      // are both the attribute and comparison object null
      if (obj == null)
        return 0;

      // the comparison object comes after
      return -1;
    } // CompareAttribute( name, obj )


    /// <summary>
    /// Validates xml against a schema
    /// </summary>
    /// <param name="node">xml</param>
    /// <param name="settings">XmlReaderSettings</param>
    public static void ValidateSchema(this XmlNode node, XmlReaderSettings settings)
    {
      // validate schema
      using (XmlReader reader = XmlReader.Create(new XmlNodeReader(node), settings))
        while (reader.Read()) ;
    } // ValidateSchema( settings )


    /// <summary>
    /// Serializes ICoreObjects to a stream
    /// </summary>
    /// <param name="obj">ICoreObject</param>
    /// <param name="stream">output stream</param>
    public static void Serialize(this IEntity obj, Stream stream)
    {
      using (StreamWriter writer = new StreamWriter(stream))
        writer.Write(obj.ToXml());
    } // Serialize( obj, stream )


    /// <summary>
    /// Serializes ICoreObjects to a file
    /// </summary>
    /// <param name="obj">ICoreObject</param>
    /// <param name="file">file path</param>
    public static void Serialize(this IEntity obj, string file)
    {
      using (FileStream stream = File.Create(file))
        obj.Serialize(stream);
    } // Serialize( obj, file )


    /// <summary>
    /// Writes out the attribute with the specified local name and value
    /// </summary>
    /// <param name="writer">XmlWriter instance</param>
    /// <param name="attribute">The local name of the attribute.</param>
    /// <param name="formattable">The value of the attribute.</param>
    /// <returns>true if attribute is written to XmlWriter; otherwise false</returns>
    public static bool WriteAttributeString(this XmlWriter writer, string attribute, IFormattable formattable)
    {
      if (formattable != null)
      {
        writer.WriteAttributeString(attribute, formattable.ToString(null, CultureInfo.InvariantCulture));
        return true;
      }

      return false;
    } // WriteAttributeString( attribute, formattable )


    /// <summary>
    /// Writes out the attribute with the specified local name and value
    /// </summary>
    /// <param name="writer">XmlWriter instance</param>
    /// <param name="attribute">The local name of the attribute.</param>
    /// <param name="dateTime">DateTime value</param>
    /// <returns>true if attribute is written to XmlWriter; otherwise false</returns>
    public static bool WriteAttributeString(this XmlWriter writer, string attribute, DateTimeOffset? dateTime)
    {
      return writer.WriteAttributeString(attribute, dateTime, false);
    } // WriteAttributeString( attribute, dateTime )


    /// <summary>
    /// Writes out the attribute with the specified local name and value
    /// </summary>
    /// <param name="writer">XmlWriter instance</param>
    /// <param name="attribute">The local name of the attribute.</param>
    /// <param name="dateTime">DateTime value</param>
    /// <returns>true if attribute is written to XmlWriter; otherwise false</returns>
    public static bool WriteAttributeString(this XmlWriter writer, string attribute, DateTimeOffset? dateTime, bool dateOnly)
    {
      if (dateTime != null)
      {
        string value = (dateOnly) ? dateTime.Value.ToXmlDateString() : dateTime.Value.ToXmlDateTimeString();
        writer.WriteAttributeString(attribute, value);
        return true;
      }

      return false;
    } // WriteAttributeString( attribute, dateTime, dateOnly )


    /// <summary>
    /// Writes out the attribute with the specified local name and value
    /// </summary>
    /// <param name="writer">XmlWriter instance</param>
    /// <param name="attribute">The local name of the attribute.</param>
    /// <param name="boolean">boolean value.</param>
    public static void WriteAttributeString(this XmlWriter writer, string attribute, bool boolean)
    {
      writer.WriteAttributeString(attribute, boolean.ToString().ToLower());
    } // WriteAttributeString( attribute, boolean )


    /// <summary>
    /// Writes out the attribute with the specified local name and value
    /// </summary>
    /// <param name="writer">XmlWriter instance</param>
    /// <param name="attribute">The local name of the attribute.</param>
    /// <param name="enumeration">Enum value.</param>
    public static void WriteAttributeString(this XmlWriter writer, string attribute, Enum enumeration)
    {
      writer.WriteAttributeString(attribute, enumeration.ToString());
    } // WriteAttributeString( attribute, enumeration )


    /// <summary>
    /// Returns an xml date only string
    /// </summary>
    /// <param name="date">dateTime struct</param>
    /// <returns>xml date string</returns>
    public static string ToXmlDateString(this DateTimeOffset date)
    {
      return date.ToString("s").Substring(0, 10);
    } // ToXmlDateString( date )


    /// <summary>
    /// Returns an xml date time string
    /// </summary>
    /// <param name="date">dateTime struct</param>
    /// <returns>xml date time string</returns>
    public static string ToXmlDateTimeString(this DateTimeOffset date)
    {
      return date.ToString("o");
    } // ToXmlDateString( date )

    #endregion

    #region List<string> Extension Methods


    /// <summary>
    /// Generates a delimted list of string values ( value1|value2|...)
    /// </summary>
    /// <param name="list"></param>
    public static string ToDelimtedString(this List<string> list, char delimiter)
    {

      string delimitedList = string.Empty;

      foreach (string str in list)
      {
        delimitedList = delimitedList + str + delimiter;
      }

      return delimitedList.TrimEnd(new char[] { delimiter });
    }

    #endregion


    #region List<IEntity> Extension Methods

    /// <summary>
    /// Gets the list of series based on type
    /// </summary>
    /// <param name="list"></param>
    /// <param name="type"></param>
    /// <returns></returns>
    public static DataView ToDataView(this List<IEntity> list, bool readOnly)
    {


      DataView dataView = new DataView();

      if (list.Count > 0)
      {
        IEntity item = list[0];

        EntityDataTable dataTable = new EntityDataTable(item.Tag);
        dataView.Table = (DataTable)dataTable;

        dataTable.Load(list, readOnly);

      }

      return dataView;
    } // ToDataView(this List<IEntity> list)



    #endregion

    #region List<Application> Extension Methods

    /// <summary>
    /// Gets the list of series based on type
    /// </summary>
    /// <param name="list"></param>
    /// <param name="type"></param>
    /// <returns></returns>
    public static bool TryFind(this List<Application> list, string id, ref Application application)
    {


      foreach (Application app in list)
      {
        if (app.Id == id)
        {
          application = app;
          return true;
        }
      }

      return false;
    } // TryFind(this List<Application> list, string id, ref Application application)


    #endregion

    #region List<AuthenticationType> Extension Methods


    /// <summary>
    /// Gets the list of series based on type
    /// </summary>
    /// <param name="list"></param>
    /// <param name="type"></param>
    /// <returns></returns>
    public static AuthenticationType FindDefault(this List<AuthenticationType> list)
    {

      AuthenticationType defaultType = null;

      foreach (AuthenticationType type in list)
      {
        if (type.Default)
        {
          defaultType = type;
          break;
        }
      }

      return defaultType;
    } // FindDefault(this List<AuthenticationType> list)

 


    #endregion

    #region List<Setting> Extension Methods


    /// <summary>
    /// Gets the list of series based on type
    /// </summary>
    /// <param name="list"></param>
    /// <param name="type"></param>
    /// <returns></returns>
    public static bool TryFind(this List<Setting> list, string name, ref Setting setting)
    {
      foreach (Setting item in list)
      {
        if (string.Compare(item.Name, name, true) == 0)
        {
          setting = item;
          return true;
        }
       }

      return false;
    } // TryFind(this List<Setting> list, string name, ref string value)

 


    #endregion

    #region List<Database> Extension Methods

     /// <summary>
    /// Gets the list of series based on type
    /// </summary>
    /// <param name="list"></param>
    /// <param name="type"></param>
    /// <returns></returns>
    public static bool TryFind(this List<Database> list, string name, ref Database database)
    {
      foreach (Database d in list)
      {
        if (string.Compare(d.Name, name, true) == 0 )
        {
          database = d;
          return true;
        }
      }

      return false;
    } // TryFind(this List<Database> list, string name, ref Database database)


    /// <summary>
    /// Returns true if the series exists in the list
    /// </summary>
    /// <param name="list"></param>
    /// <param name="uid"></param>
    /// <returns></returns>
    public static bool Contains(this List<Database> list, string name)
    {

      foreach (Database d in list)
      {
        if (string.Compare(d.Name, name, true) == 0 )
          return true;
      }

      return false;
    } // Contains(this List<Series> list, string uid)

    /// <summary>
    /// Returns the xml for the series list
    /// </summary>
    /// <param name="list"></param>
    /// <returns></returns>
    public static string ToXml(this List<Database> list)
    {
      StringBuilder builder = new StringBuilder();
      using (XmlWriter writer = XmlWriter.Create(builder, new XmlWriterSettings() { OmitXmlDeclaration = true, Indent = false }))
      {


        writer.WriteStartElement("collection");
        {


          writer.WriteAttributeString("id", "database");


          foreach (Database database in list)
          {
            writer.WriteRaw(database.ToXml());
          }
        }

        writer.WriteEndElement();
      }

      return builder.ToString();
    } // ToXml(this List<Database> list)

    #endregion


    #region List<Database> Extension Methods

    /// <summary>
    /// Gets the list of series based on type
    /// </summary>
    /// <param name="list"></param>
    /// <param name="type"></param>
    /// <returns></returns>
    public static bool TryFind(this List<Table> list, string name, ref Table table)
    {
      foreach (Table t in list)
      {
        if (string.Compare(t.Name, name, true) == 0)
        {
          table = t;
          return true;
        }
      }

      return false;
    } // TryFind(this List<Table> list, string name, ref Database database)


    /// <summary>
    /// Returns true if the series exists in the list
    /// </summary>
    /// <param name="list"></param>
    /// <param name="uid"></param>
    /// <returns></returns>
    public static bool Contains(this List<Table> list, string name)
    {

      foreach (Table t in list)
      {
        if (string.Compare(t.Name, name, true) == 0)
          return true;
      }

      return false;
    } // Contains(this List<Table> list, string uid)


    #endregion

    #region List<SystemSetting> Extension Methods

    /// <summary>
    /// Gets the list of series based on type
    /// </summary>
    /// <param name="list"></param>
    /// <param name="type"></param>
    /// <returns></returns>
    public static bool TryFind(this List<SystemSetting> list, Subsystems subsystem, string name, ref SystemSetting setting)
    {
      return TryFind(list, subsystem.ToString(), name, ref setting);
    } // TryFind(this List<Application> list, string id, ref Application application)


    /// <summary>
    /// Gets the list of series based on type
    /// </summary>
    /// <param name="list"></param>
    /// <param name="type"></param>
    /// <returns></returns>
    public static bool TryFind(this List<SystemSetting> list, string subsystem, string name, ref SystemSetting setting)
    {


      foreach (SystemSetting systemSetting in list)
      {
        if (systemSetting.Subsystem == subsystem && systemSetting.Name == name)
        {
          setting = systemSetting;
          return true;
        }
      }

      return false;
    } // TryFind(this List<Application> list, string id, ref Application application)


    #endregion



    #region List<Series> Extension Methods

    /// <summary>
    /// Gets the list of uids delimited by |
    /// </summary>
    /// <param name="list">Series list</param>
    /// <returns>uid|uid|uid|...</returns>
    public static string GetUids(this List<Series> list)
    {
      string uids = string.Empty;


      foreach (Series series in list)
      {
        uids = uids + series.Uid + "|";
      }

      return uids.TrimEnd(new char[] { '|' });

    } // GetUids()

    /// <summary>
    /// Gets the list of series based on type
    /// </summary>
    /// <param name="list"></param>
    /// <param name="type"></param>
    /// <returns></returns>
    public static bool TryFindType(this List<Series> list, SeriesTypes type, ref Series series)
    {


      foreach (Series s in list)
      {
        if (s.Type == type)
        {
          series = s;
          return true;
        }
      }

      return false;
    }

    /// <summary>
    /// Gets the list of series based on type
    /// </summary>
    /// <param name="list"></param>
    /// <param name="type"></param>
    /// <returns></returns>
    public static bool TryFind(this List<Series> list, string uid, ref Series series)
    {
      foreach (Series s in list)
      {
        if (s.Uid == uid)
        {
          series = s;
          return true;
        }
      }

      return false;
    } // TryFind(this List<Series> list, string uid, ref Series series)

    /// <summary>
    /// Gets the list of series based on type
    /// </summary>
    /// <param name="list"></param>
    /// <param name="type"></param>
    /// <returns></returns>
    public static List<Series> FindAll(this List<Series> list, SeriesTypes type)
    {
      List<Series> seriesList = new List<Series>();


      foreach (Series series in list)
      {
        if (series.Type == type)
          seriesList.Add(series);
      }

      return seriesList;
    } // FindAll(this List<Series> list, string type)

    /// <summary>
    /// Returns true if the series exists in the list
    /// </summary>
    /// <param name="list"></param>
    /// <param name="uid"></param>
    /// <returns></returns>
    public static bool Contains(this List<Series> list, string uid)
    {
 
      foreach (Series series in list)
      {
        if (series.Uid == uid)
          return true;
      }

      return false;
    } // Contains(this List<Series> list, string uid)

    /// <summary>
    /// Returns true if the series typeexists in the list
    /// </summary>
    /// <param name="list"></param>
    /// <param name="type"></param>
    /// <returns></returns>
    public static bool ContainsType(this List<Series> list, SeriesTypes type)
    {

      foreach (Series series in list)
      {
        if (series.Type == type)
          return true;
      }

      return false;
    } // ContainsType(this List<Series> list, string type)


    /// <summary>
    /// Returns the xml for the series list
    /// </summary>
    /// <param name="list"></param>
    /// <returns></returns>
    public static string ToXml(this List<Series> list)
    {
      StringBuilder builder = new StringBuilder();
      using (XmlWriter writer = XmlWriter.Create(builder, new XmlWriterSettings() { OmitXmlDeclaration = true, Indent = false }))
      {


        writer.WriteStartElement("collection");
        {


          writer.WriteAttributeString("id", "series");


          foreach (Series series in list)
          {
            writer.WriteRaw(series.ToXml());
          }
        }

        writer.WriteEndElement();
      }

      return builder.ToString();
    } // ToXml(this List<Series> list)

    #endregion



    #region List<Study> Extension Methods

    /// <summary>
    /// Gets the list of uids delimited by |
    /// </summary>
    /// <param name="list">Study list</param>
    /// <returns>uid|uid|uid|...</returns>
    public static string GetUids(this List<Study> list)
    {
      string uids = string.Empty;

      foreach (Study study in list)
      {
        uids = uids + study.Uid + "|";
      }

      return uids.TrimEnd(new char[] { '|' });

    } // GetUids()


    /// <summary>
    /// Gets the list of series based on type
    /// </summary>
    /// <param name="list"></param>
    /// <param name="type"></param>
    /// <returns></returns>
    public static bool TryFind(this List<Study> list, string uid, ref Study study)
    {


      foreach (Study s in list)
      {
        if (s.Uid == uid)
        {
          study = s;
          return true;
        }
      }

      return false;
    } // TryFind(this List<Study> list, string uid, ref Study study)


    /// <summary>
    /// Returns true if the series exists in the list
    /// </summary>
    /// <param name="list"></param>
    /// <param name="uid"></param>
    /// <returns></returns>
    public static bool Contains(this List<Study> list, string uid)
    {

      foreach (Study study in list)
      {
        if (study.Uid == uid)
          return true;
      }

      return false;
    } // Contains(this List<Study> list, string uid)

    /// <summary>
    /// Returns the xml for the study list
    /// </summary>
    /// <param name="list"></param>
    /// <returns></returns>
    public static string ToXml(this List<Study> list)
    {
      StringBuilder builder = new StringBuilder();
      using (XmlWriter writer = XmlWriter.Create(builder, new XmlWriterSettings() { OmitXmlDeclaration = true, Indent = false }))
      {


        writer.WriteStartElement("collection");
        {


          writer.WriteAttributeString("id", "study");


          foreach (Study study in list)
          {
            writer.WriteRaw(study.ToXml());
          }
        }

        writer.WriteEndElement();
      }

      return builder.ToString();
    } // ToXml(this List<Study> list)

    #endregion



    #region framework methods


    /// <summary>
    /// Adds a list to the collection
    /// </summary>
    /// <param name="collection">observable collection</param>
    /// <param name="list">IEnumerable</param>
    public static void AddRange<T>(this ObservableCollection<T> collection, IEnumerable<T> items)
    {
      if (collection == null)
        throw new ArgumentNullException("collection");

      if (items == null)
        throw new ArgumentNullException("items");

      foreach (T item in items)
        collection.Add(item);
    } // AddRange<T>(collection, list )


    /// <summary>
    /// Replaces the collections content with a new list
    /// </summary>
    /// <param name="collection">observable collection</param>
    /// <param name="list">IEnumerable</param>
    public static void Replace<T>(this ObservableCollection<T> collection, IEnumerable<T> items)
    {
      if (collection == null)
        throw new ArgumentNullException("collection");

      if (items == null)
        throw new ArgumentNullException("items");

      collection.Clear();
      foreach (T item in items)
        collection.Add(item);
    } // Replace<T>(collection, list )


    #endregion



    #region XmlNode Extensions

    /// <summary>
    /// Gets the attribute value if possible
    /// </summary>
    /// <param name="attribute"></param>
    /// <returns></returns>
    public static bool TryGetAttributeValue<T>(this XmlNode node, string name, ref T value)
    {
      bool bReturn = false;

      XmlAttribute attribute = node.Attributes[name];

      if (attribute != null)
      {
        bReturn = true;

        if ( typeof(T) == typeof(DateTimeOffset?) )
        {
          DateTimeOffset date;
          if ( DateTimeOffset.TryParse(attribute.Value.ToString(), out date))
            value = (T)((object)date);
        }
        else if (typeof(T) == typeof(DateTime?))
        {
          DateTime date;
          if (DateTime.TryParse(attribute.Value.ToString(), out date))
            value = (T)((object)date);
        }

        else
         value = (T)Convert.ChangeType(attribute.Value, typeof(T));
      }

      return bReturn;

    } // TryGetValue(this XmlAttribute attribute, ref string value)



    /// <summary>
    /// Gets the attribute value if possible
    /// </summary>
    /// <param name="attribute"></param>
    /// <returns></returns>
    public static bool TryGetAttributeValue<T>(this XmlNode node, string name, ref T? value) where T : struct 
    {
      bool bReturn = false;

      XmlAttribute attribute = node.Attributes[name];

      if (attribute != null)
      {
        bReturn = true;

        if (attribute.Value.ToString() != "")
        {
          if (typeof(T) == typeof(DateTimeOffset) )
          {
            DateTimeOffset date;
            if (DateTimeOffset.TryParse(attribute.Value.ToString(), out date))
              value = (T)((object)date);
          }
        }
        else
            value = (T)Convert.ChangeType(attribute.Value, typeof(T));
      }

      return bReturn;

    } // TryGetValue(this XmlAttribute attribute, ref string value)


    /// <summary>
    /// Gets the attribute value if possible
    /// </summary>
    /// <param name="attribute"></param>
    /// <returns></returns>
    public static bool TryGetNodeValue(this XmlNode node, string name, ref string value) 
    {
      bool bReturn = false;

      value = string.Empty;

      XmlNode child = node.SelectSingleNode(name);

      if (child != null)
      {
        bReturn = true;

        value = child.InnerText;
      }

      return bReturn;

    } // TryGetNodeValue(this XmlNode node, string name, ref string value) 
    #endregion

    /// <summary>
    /// Removes empty attributes from xml strings.
    /// </summary>
    /// <param name="xml">xml</param>
    /// <returns>xml without empty attributes</returns>
    public static string TrimEmptyAttributes(this string xml)
    {
      return Regex.Replace(xml, @"\b(\w+)=""""\s*", string.Empty);
    } // TrimEmptyAttributes( xml )

    /// <summary>
    /// Converts the string to Base64 encoding
    /// </summary>
    /// <param name="data">string data</param>
    /// <returns>Base64 encoding</returns>
    public static string ToBase64( this string data )
    {
      return Convert.ToBase64String( ASCIIEncoding.ASCII.GetBytes( data ));
    } // ToBase64( data )


    /// <summary>
    /// Converts Base64 encoding to string
    /// </summary>
    /// <param name="data">Base64 encoding</param>
    /// <returns>string data</returns>
    public static string FromBase64( this string data )
    {
      return ASCIIEncoding.ASCII.GetString( Convert.FromBase64String( data ));
    } // FromBase64( data )




    public static T Clone<T>(this T source)
    {
      T cloned = (T)Activator.CreateInstance(source.GetType());

      foreach (PropertyInfo curPropInfo in source.GetType().GetProperties())
      {
        if (curPropInfo.GetGetMethod() != null && (curPropInfo.GetSetMethod() != null))
        {
          // Handle Non-indexer properties
          if (curPropInfo.Name != "Item")
          {
            // get property from source
            object getValue = curPropInfo.GetGetMethod().Invoke(source, new object[] { });

            // clone if needed
            if (getValue != null && getValue is DependencyObject)
              getValue = Clone((DependencyObject)getValue);

            // set property on cloned
            curPropInfo.GetSetMethod().Invoke(cloned, new object[] { getValue });
          }
          // handle indexer
          else
          {
            // get count for indexer
            int numberofItemInColleciton = (int)curPropInfo.ReflectedType.GetProperty("Count").GetGetMethod().Invoke(source, new object[] { });

            // run on indexer
            for (int i = 0; i < numberofItemInColleciton; i++)
            {
              // get item through Indexer
              object getValue = curPropInfo.GetGetMethod().Invoke(source, new object[] { i });

              // clone if needed
              if (getValue != null && getValue is DependencyObject)
                getValue = Clone((DependencyObject)getValue);
              // add item to collection
              curPropInfo.ReflectedType.GetMethod("Add").Invoke(cloned, new object[] { getValue });
            }
          }
        }
      }

      return cloned;
    }


  } // class ExtensionMethods
} // namespace Viatronix.Enterprise
