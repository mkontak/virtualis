using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Drawing;

namespace Viatronix.v3D.Dicom.UI
{
  class Group
  {
    #region fields

    private string m_id = string.Empty;

    private string m_desc = string.Empty;

    private Color m_color = SystemColors.Control;

    #endregion


    public Group()  { }

    public static Group Create(XmlNode node)
    {
      Group group = new Group();

      group.m_id = node.Attributes["id"].Value.ToString();
      group.m_color = System.Drawing.Color.FromName(node.Attributes["color"].Value.ToString());
      group.m_desc = node.Attributes["desc"].Value.ToString();

      return group;

    }

    public string Id
    {
      get { return m_id; }
    }

    public string Description
    {
      get { return m_desc;  }
    }

    public Color Color
    {
      get { return m_color;  }
    }


  }
}
