using System;
using System.Xml;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Viatronix.Utilities.Dicom
{
  class ServersSectionHandler : System.Configuration.IConfigurationSectionHandler
  {
    public virtual object Create(object parent, object context, XmlNode section)
    {
      return section.SelectNodes("server");
    }
  }
}
