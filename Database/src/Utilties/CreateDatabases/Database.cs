using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;

namespace Viatronix.Utilities.Database
{
  public class Database
  {

    private string m_name = string.Empty;
    private string m_description = string.Empty;
    private bool m_selectable = false;
    private string m_system = string.Empty;


    private List<Command> m_commands = new List<Command>();




    #region mthods

    public static Database Create(XmlNode node, V3DSystem system)
    {
      Database db = new Database();

      if (db == null)
        throw new OutOfMemoryException("Failed to allocate Database");

      db.m_system = system.Name;
      db.m_name = node.Attributes["name"].Value.ToString();
      db.m_description = node.Attributes["desc"].Value.ToString();
      db.m_selectable = (!system.Selectable ? false : ( node.Attributes["selectable"] == null ? true : Boolean.Parse(node.Attributes["selectable"].Value.ToString())));

      foreach ( XmlNode command in node.SelectNodes("commands/command") )
      {
        db.m_commands.Add(Command.Create(command, db));
      }

      

      return db;
    }


    #endregion

    #region properties

    /// <summary>
    /// Selectable
    /// </summary>
    public bool Selectable
    {
      get { return m_selectable; }
    }

    /// <summary>
    /// Commands
    /// </summary>
    public List<Command> Commands
    {
      get { return m_commands; }
    }

    /// <summary>
    /// Sets/Gets the Description
    /// </summary>
    public string Description
    {
      get { return m_description; }
    }

    /// <summary>
    /// Set/Get the Database Name
    /// </summary>
    public string Name
    {
      get { return m_name; }
    }

    #endregion


  }
}
