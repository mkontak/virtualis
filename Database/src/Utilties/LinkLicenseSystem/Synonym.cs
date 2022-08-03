using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Threading.Tasks;

namespace Viatronix.Utilities.Database
{
  class Synonym
  {

    #region fields


    private string m_source;

    private string m_destination;

    private string m_database;

    #endregion


    #region properties

    public string Database
    {
      get { return m_database; }
    }

    public string Source
    {
      get { return m_source; }
    }

    public string Destination
    {
      get { return m_destination; }
    }

    #endregion


    #region methods


    public static Synonym Create(XmlNode node)
    {

      Synonym syn = new Synonym();

      if (syn == null)
        throw new OutOfMemoryException("Failed to allocate Synonym object");

      syn.m_source = node.Attributes["source"].Value;
      syn.m_destination = node.Attributes["dest"].Value;
      syn.m_database = node.Attributes["database"].Value;


      return syn;

    }

    #endregion

  }
}
