using System;
using System.Xml;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Viatronix.Utilities.Dicom
{

  /// <summary>
  /// Class to read in the client section of the application config file
  /// </summary>
  class ClientSectionHandler : System.Configuration.IConfigurationSectionHandler
  {
    public virtual object Create(object parent, object context, XmlNode section)
    {
      return section;
    }

  } // class ClientSectionHandler
    
} // namespace Viatroinix.Utilities.Dicom
