using System;

namespace Viatronix.v3D.Dicom.UI
{

  public enum NodeType { PATIENT, STUDY, SERIES, IMAGE };

  /// <summary>
  /// Summary description for NodeTag.
  /// </summary>
  public class NodeTag
  {
    private NodeType m_nodeType = NodeType.IMAGE;
    private object m_tag = null;
    private static string [] m_nodeTypeDesc = { "Patient", "Study", "Series", "Image" };

    public NodeTag(NodeType nodeType, object tag)
    {
      m_nodeType = nodeType;
      m_tag = tag;
    }

    public NodeTag(NodeType nodeType)
    {
      m_nodeType = nodeType;
    }

    /// <summary>
    /// Returns the NODE TYPE
    /// </summary>
    public NodeType NodeType 
    {
      get { return m_nodeType; }
    }

    /// <summary>
    /// Returns the TAG object
    /// </summary>
    public object Tag
    {
      get { return m_tag; }
    }

    public string NodeTypeDesc
    {
      get { return m_nodeTypeDesc[(int)m_nodeType]; }
    }

	}
}
