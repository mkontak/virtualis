using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Configuration;
using System.Xml;
using System.Xml.XPath;
using System.Xml.Serialization;

namespace Viatronix.Enterprise.Configuration
{


  public class XmlSerializerSectionHandler : IConfigurationSectionHandler
  {
    public object Create(object parent, object context, XmlNode section)
    {
      XPathNavigator xpn = section.CreateNavigator();
      string typeName = xpn.Evaluate("string(@type)").ToString();
      Type type = Type.GetType(typeName);
      XmlSerializer xs = new XmlSerializer(type);
      return xs.Deserialize(new XmlNodeReader(section));
    }
  };
}
