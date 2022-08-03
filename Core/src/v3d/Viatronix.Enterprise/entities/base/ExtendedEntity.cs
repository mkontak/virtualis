using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;

namespace Viatronix.Enterprise.Entities
{
  public abstract class ExtendedEntity : Entity, IExtendedEntity
  {

    #region fields

    /// <summary>
    /// extended properties
    /// </summary>
    private string m_extendedProperties = string.Empty;

    #endregion

    #region properties

    /// <summary>
    /// Sets/Gets the ExtendedProperties 
    /// </summary>
    public virtual string ExtendedProperties
    {
      get { return m_extendedProperties; }
      set
      {
        string extended = string.Empty;
        if (!string.IsNullOrEmpty(value))
        {
          XmlDocument doc = new XmlDocument();
          doc.LoadXml(value);

          // Uncomment if we want to validate the schema of the extended properties
          //doc.DocumentElement.ValidateSchema(Schema.Settings);

          if (doc.DocumentElement.Attributes.Count > 0 || doc.DocumentElement.ChildNodes.Count > 0)
            extended = value;
        }

        SetProperty("ExtendedProperties", extended, ref m_extendedProperties);
      }
    } // ExtendedProperties


    #endregion

    #region methods

    /// <summary>
    /// Outputs elements to xml
    /// </summary>
    /// <param name="writer"></param>
    protected override void WriteElements(XmlWriter writer)
    {
      // output extended properties
      if (!string.IsNullOrEmpty(m_extendedProperties))
        writer.WriteRaw(m_extendedProperties);

      base.WriteElements(writer);
    } // WriteElements( writer )


    /// <summary>
    /// Loads the object from xml
    /// </summary>
    /// <param name="node">xml</param>
    public override void Load(XmlNode node)
    {
      // retrieve extended properties
      XmlNode extendedProperties = node.SelectSingleNode(EntityAttributes.ExtendedProperties);
      if (extendedProperties != null)
        this.ExtendedProperties = extendedProperties.OuterXml;

      base.Load(node);
    } // Load( node )


    /// <summary>
    /// Copies data to other IExtendedModel object
    /// </summary>
    /// <param name="obj">other instance</param>
    protected override void CopyTo(IEntity obj)
    {
      base.CopyTo(obj);

      IExtendedEntity model = (IExtendedEntity)obj;
      model.ExtendedProperties = this.ExtendedProperties;
    } // CopyTo( obj )


    /// <summary>
    /// Checks for value equality of IExtendedModel objects
    /// </summary>
    /// <param name="obj">IModel to compare with this instance or a null reference</param>
    /// <returns>true if they are equal values; otherwise false</returns>
    public override bool Equals(IEntity obj)
    {
      IExtendedEntity other = obj as IExtendedEntity;

      return other != null &&
             base.Equals(other) &&
             string.Equals(this.ExtendedProperties, other.ExtendedProperties);
    } //  Equals( obj )

    #endregion

  } // class ModelBaseExt
} // namespace Viatronix


