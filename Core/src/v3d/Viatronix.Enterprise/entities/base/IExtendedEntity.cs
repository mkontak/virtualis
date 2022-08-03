using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;

namespace Viatronix.Enterprise.Entities
{
  public interface IExtendedEntity : IEntity
  {

    string ExtendedProperties { get; set; }

  }

}
