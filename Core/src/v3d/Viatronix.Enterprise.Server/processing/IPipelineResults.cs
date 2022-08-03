using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Viatronix.Enterprise.Processing
{
  public interface IPipelineResults
  {

    /// <summary>
    /// Generate the xml representation for the results
    /// </summary>
    /// <returns></returns>
    string ToXml();



  }
}
