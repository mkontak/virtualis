using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;

namespace Viatronix.Utilities.Database
{



  public class V3DSystem
  {

    #region fields


    /// <summary>
    /// System Id
    /// </summary>
    private string m_id = string.Empty;

    /// <summary>
    /// System Name
    /// </summary>
    private string m_name = string.Empty;


    /// <summary>
    /// Selectable flag
    /// </summary>
    private bool m_selectable = false;


    /// <summary>
    /// Databases associated with this system
    /// </summary>
    private List<Database> m_databases = new List<Database>();

    #endregion


    #region properties

    /// <summary>
    /// Systems Id
    /// </summary>
    public string Id
    {
      get { return m_id; }
    } // Id

    /// <summary>
    /// System Name
    /// </summary>
    public string Name
    {
      get { return m_name; }
    } // Name


    /// <summary>
    /// Databases associated with the system
    /// </summary>
    public List<Database> Databases
    {
      get { return m_databases; }
    } // Databases

    /// <summary>
    /// Selectable flag
    /// </summary>
    public bool Selectable
    {
      get { return m_selectable; }
    } // Selectable

    #endregion



    #region methods


    public static V3DSystem Create(XmlNode node)
    {

      V3DSystem sys = new V3DSystem();

      if (sys == null)
        throw new OutOfMemoryException("Failed to allocate System object");


      sys.m_id = node.Attributes["id"].Value;
      sys.m_name = node.Attributes["name"].Value;
      sys.m_selectable = (node.Attributes["selectable"] == null ? true : Boolean.Parse(node.Attributes["selectable"].Value.ToString()));


      foreach (XmlNode database in node.SelectNodes("databases/database"))
      {
        Database db = Database.Create(database, sys);
        sys.m_databases.Add(Database.Create(database, sys));
      }


      return sys;
    }

    #endregion


  }




}
