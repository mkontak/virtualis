using System;
using System.Xml;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Viatronix.Services
{
  class ServerSectionHandler : System.Configuration.IConfigurationSectionHandler
  {
    public virtual object Create(object parent, object context, XmlNode section)
    {
      return section;
    }
  }

 
}
