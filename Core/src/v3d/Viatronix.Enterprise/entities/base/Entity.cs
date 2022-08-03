
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Xml;

namespace Viatronix.Enterprise.Entities
{
  public abstract class Entity : IEntity, INotifyPropertyChanged, INotifyPropertyChanging
  {
    #region fields

    /// <summary>
    /// Version
    /// </summary>
    private string m_version = "4.0";

    /// <summary>
    /// Double value of the version
    /// </summary>
    private Double m_actualVersion = 4.0;

    #endregion

    #region properties

    /// <summary>
    /// Returns the version
    /// </summary>
    public virtual string Version 
    {
      get { return m_version; }
    } // Version

    /// <summary>
    /// Returns the version as a double
    /// </summary>
    public virtual Double ActualVersion
    {
      get { return m_actualVersion; }
    } // ActualVersion

    /// <summary>
    /// Gets tag
    /// </summary>
    public string Tag
    {
      get { return this.GetType().Name.ToLower(); }
    } // Tag

    #endregion

    #region methods

    /// <summary>
    /// Converts the object to xml
    /// </summary>
    /// <returns>xml</returns>
    public abstract string ToXml();

    /// <summary>
    /// Loads the object from xml
    /// </summary>
    /// <param name="node">xml</param>
    public virtual void Load(XmlNode node)
    {
      // Make sure the node is not NULL
      if (node == null)
        throw new Viatronix.Enterprise.InvalidDateException("Xml node is null");

      node.TryGetAttributeValue<string>(EntityAttributes.Version, ref m_version);

      string[] fields = m_version.Split(new Char[] { '.' });
      string temp = m_version;

      if (fields.Length > 2)
        temp = String.Format("{0}.{1}{2}", fields[0].PadLeft(3, '0'), fields[1].PadRight(4, '0'), fields[2].PadRight(4, '0'));
      else if (fields.Length > 1)
        temp = String.Format("{0}.{1}", fields[0].PadLeft(3, '0'), fields[1].PadRight(4, '0'));
      else if (fields.Length > 0)
        temp = String.Format("{0}", fields[0].PadLeft(3, '0'));


      m_actualVersion = Double.Parse(temp);



    } // Load( node )

    /// <summary>
    /// Outputs attribute strings to xml
    /// </summary>
    /// <param name="writer">xml</param>
    protected virtual void WriteAttributeStrings(XmlWriter writer)
    {
      writer.WriteAttributeString(EntityAttributes.Version, m_version);
    } // ToXml( writer )


    /// <summary>
    /// Outputs elements to xml
    /// </summary>
    /// <param name="writer"></param>
    protected virtual void WriteElements(XmlWriter writer)
    {
    } // WriteElements( writer )



    /// <summary>
    /// Clones the object
    /// </summary>
    /// <returns>clone</returns>
    public virtual object Clone()
    {

      Entity cloned = null;

      cloned = (Entity)System.Reflection.Assembly.GetExecutingAssembly().CreateInstance("Viatronix.Enterprise.Entities." + Tag);

      if (cloned == null)
        throw new Exception("Failed to clone " + Tag);

      XmlDocument doc = new XmlDocument();
      doc.LoadXml(this.ToXml());

      cloned.Load(doc.DocumentElement);


      return cloned;
    } // Clone()


    /// <summary>
    /// Copies data to other IEntity object
    /// </summary>
    /// <param name="obj">other instance</param>
    protected virtual void CopyTo(IEntity obj)
    {
      
    } // CopyTo( obj )


    /// <summary>
    /// Checks for value equality of IModel objects
    /// </summary>
    /// <param name="other">IModel to compare with this instance or a null reference</param>
    /// <returns>true if they are equal values; otherwise false</returns>
    public virtual bool Equals(IEntity other)
    {
      return object.Equals(Tag, other.Tag);
    } //  Equals( other )


    /// <summary>
    /// Sets the property
    /// </summary>
    /// <typeparam name="T">Type for property</typeparam>
    /// <param name="propertyName">property name</param>
    /// <param name="newValue">new value</param>
    /// <param name="currentValue">reference to current value</param>
    protected virtual bool SetProperty<T>(string propertyName, T newValue, ref T currentValue)
    {
      if (!object.Equals(newValue, currentValue))
      {
        OnNotifyPropertyChanging(propertyName);

        currentValue = newValue;

        OnNotifyPropertyChanged(propertyName);

        return true;
      }

      return false;
    } // SetProperty<T>( propertyName, newValue, currentValue )

 
    #endregion

    #region INotifyPropertyChanged Members

    // property changed event
    public event PropertyChangedEventHandler PropertyChanged;

    /// <summary>
    /// Handles the notify property changed event
    /// </summary>
    /// <param name="propertyName"></param>
    protected virtual void OnNotifyPropertyChanged(string propertyName)
    {
      if (this.PropertyChanged != null)
        this.PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
    } // OnNotifyPropertyChanged(string propertyName)

    #endregion

    #region INotifyPropertyChanging Members

    /// <summary>
    ///  Property changeing event
    /// </summary>
    public event PropertyChangingEventHandler PropertyChanging;

    /// <summary>
    /// Handles the property changing event
    /// </summary>
    /// <param name="propertyName"></param>
    protected virtual void OnNotifyPropertyChanging(string propertyName)
    {
      if (this.PropertyChanging != null)
        this.PropertyChanging(this, new PropertyChangingEventArgs(propertyName));
    } // OnNotifyPropertyChanging(string propertyName)

    #endregion

  } // class Entity

} // namespace Viatronix
