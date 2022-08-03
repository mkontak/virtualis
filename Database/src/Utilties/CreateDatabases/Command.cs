using System;
using System.Xml;

namespace Viatronix.Utilities.Database
{
  /// <summary>
  /// Summary description for SqlCommand.
  /// </summary>
  public class Command
  {

    private string m_database = string.Empty;
    private string m_file = string.Empty;
    private string m_description = string.Empty;
    private bool m_selectable = true;
    private bool m_optional = false;

    public Command()
    {
      //
      // TODO: Add constructor logic here
      //
    }


    #region properties

    public bool Optional
    {
      get { return m_optional; }
    }


    public bool Selectable
    {
      get { return m_selectable; }
    }

    public string Database
    {
      get { return m_database; }
    }

    public string Description
    {
      get { return m_description; }
    }

    public string File
    {
      get { return m_file; }
    }



    #endregion



    #region static methods

    public static Command Create(XmlNode node, Database database)
    {
      Command cmd = new Command();

      cmd.m_database = (node.Attributes["database"] == null ? database.Name : node.Attributes["database"].Value.ToString());
      cmd.m_description = node.Attributes["desc"].Value.ToString();
      cmd.m_file = node.Attributes["file"].Value.ToString();
      cmd.m_selectable = (!database.Selectable ? false : ( node.Attributes["selectable"] == null ? true : Boolean.Parse(node.Attributes["selectable"].Value.ToString())));
      cmd.m_optional = (node.Attributes["opt"] == null ? false : Boolean.Parse(node.Attributes["opt"].Value.ToString()));

      return cmd;
    }

    #endregion

  }
}
