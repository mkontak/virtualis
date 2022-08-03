using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;

namespace Viatronix.Enterprise.Entities
{

    public interface IProcessEntity
    {
      /// <summary>
      /// Returns true if the object can process
      /// </summary>
      /// <returns></returns>
      bool CanProcess
      { get;  }

      /// <summary>
      /// Gets the application
      /// </summary>
      string Application
      { get;  }

      /// <summary>
      /// Gets the modality
      /// </summary>
      string Modality
      { get;  }

    } // interface IProcessEntity
}
