using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using GalaSoft.MvvmLight;
using System.Xml;


namespace Versioning.ViewModels
{
  public class ReleaseViewModel : ViewModelBase
  {

    #region class Attributes

    /// <summary>
    /// settings fields directly related to the settings table in the database
    /// </summary>
    public static class Attributes
    {
      public const string Id = "id";
      public const string Description = "desc";
    } // class Attributes

    #endregion

    #region fields

    private string m_id = string.Empty;

    private string m_description = string.Empty;

    #endregion

    #region properties


    public String Id
    {
      get { return m_id; }
      set { RaisePropertyChanged<string>("Id", m_id, value, true); m_id = value;  }
    }

    public string Description
    {
      get { return m_description; }
      set { RaisePropertyChanged<string>("Description", m_description, value, true); m_description = value; }
    }

    #endregion

    #region methods

    public static ReleaseViewModel Create(XmlNode node)
    {
      ReleaseViewModel vm = new ReleaseViewModel();

      if (vm == null)
        throw new OutOfMemoryException("Failed to allocate ReleaseViewModel");

      vm.Load(node);

      return vm;
    }

    /// <summary>
    /// Creates a setting from the xml node
    /// </summary>
    /// <param name="node">Xml node</param>
    /// <returns>Setting</returns>
    public void Load(XmlNode node)
    {

      m_id = node.Attributes[Attributes.Id].Value;
      m_description = node.Attributes[Attributes.Description].Value;


    } // Load(XmlNode node)


    public string ToXml()
    {
      StringBuilder builder = new StringBuilder();
      using (XmlWriter writer = XmlWriter.Create(builder, new XmlWriterSettings() { OmitXmlDeclaration = true, Indent = false }))
      {


        writer.WriteStartElement("projects");
        {

          writer.WriteAttributeString(Attributes.Id, m_id);
          writer.WriteAttributeString(Attributes.Description, m_description);

        }

        writer.WriteEndElement();

      }

      return builder.ToString();

    }


    #endregion


  }
}
