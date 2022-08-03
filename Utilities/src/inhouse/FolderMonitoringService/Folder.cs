using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Viatronix
{
  public class Folder
  {

    #region fields

    private string m_path = string.Empty;

    private bool m_email = false;

    private string m_subject = string.Empty;

    private string m_body = string.Empty;

    private string m_type = string.Empty;

    private string m_source = string.Empty;

    private string m_system = string.Empty;


    #endregion

    #region contrsuctors

    public Folder( )
    {
    }
    #endregion

    #region properties

    public string Path
    {
      get { return m_path; }
      set { m_path = value; }
    } // Path

    public string Subject
    {
      get { return m_subject; }
      set { m_subject = value; }
    } // Subject

    public string Body
    {
      get { return m_body; }
      set { m_body = value; }
    } // Body

    public bool Email
    {
      get { return m_email; }
      set { m_email = true; }
    } // Email

    public string Type
    {
      get { return m_type; }
      set { m_type = value; }
    } // Type

    public string Source
    {
      get { return m_source; }
      set { m_source = value; }
    } // Source

    public string System
    {
      get { return m_system; }
      set { m_system = value; }
    } // System


    #endregion


  }
}
