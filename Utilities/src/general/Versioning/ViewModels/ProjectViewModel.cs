using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using GalaSoft.MvvmLight;
using System.Xml;


namespace Versioning.ViewModels
{
  public class ProjectViewModel : ViewModelBase
  {
    #region class Attributes

    /// <summary>
    /// settings fields directly related to the settings table in the database
    /// </summary>
    public static class Attributes
    {
      public const string Release = "release";
      public const string Project = "project";
      public const string Product = "product";
      public const string Major = "major";
      public const string Minor = "minor";
      public const string Patch = "patch";
      public const string BuildNumber = "build";
      public const string Description = "desc";
    } // class Attributes

    #endregion

    #region fields

    private string m_description = string.Empty;

    private string m_project = string.Empty;

    private string m_release = string.Empty;


    private string m_product = string.Empty;

    private uint m_major = 1;

    private uint m_minor = 0;

    private uint m_patch = 0;

    private uint m_build = 0;


    #endregion



    #region properties

    public string Release
    { get { return m_release; } }

    public string Project
    { get { return m_project; } }

    public string Product
    { get { return m_product; } }

    public string Description
    { get { return m_description; } }

    public uint Major
    { get { return m_major; } }

    public uint Minor
    { get { return m_minor; } }

    public uint Patch
    { get { return m_patch; } }

    public uint BuildNumber
    { get { return m_build; } }

    #endregion

    #region methods

    public static ProjectViewModel Create(XmlNode node)
    {
      ProjectViewModel vm = new ProjectViewModel();

      if (vm == null)
        throw new OutOfMemoryException("Failed to allocate ProjectVersionViewModel");

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

      m_release = node.Attributes[Attributes.Release].Value;
      m_project = node.Attributes[Attributes.Project].Value;
      m_product = node.Attributes[Attributes.Product].Value;
      m_major = UInt32.Parse(node.Attributes[Attributes.Major].Value);
      m_minor = UInt32.Parse(node.Attributes[Attributes.Minor].Value);
      m_patch = UInt32.Parse(node.Attributes[Attributes.Patch].Value);
      m_build = UInt32.Parse(node.Attributes[Attributes.BuildNumber].Value);
      m_description = node.Attributes[Attributes.Description].Value;

 
    } // Load(XmlNode node)


    public string ToXml()
    {
      StringBuilder builder = new StringBuilder();
      using (XmlWriter writer = XmlWriter.Create(builder, new XmlWriterSettings() { OmitXmlDeclaration = true, Indent = false }))
      {


        writer.WriteStartElement("projects");
        {

          writer.WriteAttributeString(Attributes.Release, m_release);
          writer.WriteAttributeString(Attributes.Project, m_project);
          writer.WriteAttributeString(Attributes.Product, m_product);
          writer.WriteAttributeString(Attributes.Description, m_description);
          writer.WriteAttributeString(Attributes.Major, m_major.ToString());
          writer.WriteAttributeString(Attributes.Minor, m_minor.ToString());
          writer.WriteAttributeString(Attributes.Patch, m_patch.ToString());
          writer.WriteAttributeString(Attributes.BuildNumber, m_build.ToString());


        }

        writer.WriteEndElement();

      }

      return builder.ToString();

    }


    #endregion

  }
}
