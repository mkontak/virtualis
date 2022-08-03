using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Drawing;
using System.ComponentModel;
using Viatronix.Enterprise;
using Viatronix.Enterprise.Entities;

namespace Viatronix.Enterprise.SDK.Providers
{

  /// <summary>
  /// Implementation for a browser specific interface. Used for the retreival of studies and 
  /// series.
  /// </summary>
  public interface IAsynchronousBrowserProvider : IBrowserProvider, IAsynchronousProvider
  {


    #region methods

    #region Study methods


    /// <summary>
    /// Gets the studies from the provider asynchronously
    /// </summary>
    /// <param name="builder">Query</param>
    void GetStudiesAsynchronously(XmlBuilder query);

 
    #endregion


    #region Series methods

    /// <summary>
    /// Gets the series associated with the study specified asynchrnously
    /// </summary>
    /// <param name="study">Study</param>
    void GetSeriesAsynchronously(Study study);

    /// <summary>
    /// Gets the series associated with the study specified
    /// </summary>
    /// <param name="studies">Study</param>
    /// <returns>SeriesCollection</returns>
    void GetSeriesAsynchronously(List<Study> studies);

 
    #endregion


    #endregion



  } // interface IAsynchrounousBrowserProvider

} // namespace Viatronix.Enterprise.SDK.Providers
