using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;

namespace Viatronix.Enterprise.Entities
{


  /// <summary>
  /// 
  /// </summary>
  public interface IEntity : ICloneable, IEquatable<IEntity>
  {

    string Version { get; }

    string Tag { get; }

    string ToXml();

    void Load(XmlNode node);

  }

  /// <summary>
  /// fields directly related to the table in the database
  /// </summary>
  public static class EntityAttributes
  {
    public const string Version = "ver";
    public const string ExtendedProperties = "ext";
  } // class ModelAttributes

}
