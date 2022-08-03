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
  public interface IBrowserProvider
  {


    #region methods

    #region Study methods

    /// <summary>
    /// Gets the studies from the provider
    /// </summary>
    /// <param name="builder">Query</param>
    /// <returns>List<Study></returns>
    List<Study> GetStudies(XmlBuilder query);

    /// <summary>
    /// Gets a single study specified by the uid
    /// </summary>
    /// <param name="uid"></param>
    /// <returns></returns>
    Study GetStudy(string uid);

    /// <summary>
    /// Gets a single study specified by the series
    /// </summary>
    /// <param name="series">Series</param>
    /// <returns></returns>
    Study GetStudy(Series series);

    /// <summary>
    /// Refreshes the study
    /// </summary>
    /// <param name="uid">Series uid</param>
    /// <returns>Series</returns>
    void GetStudy(Study study);

    #endregion


    #region Series methods

    /// <summary>
    /// Gets the series associated with the study specified
    /// </summary>
    /// <param name="study">Study</param>
    /// <returns>SeriesCollection</returns>
    List<Series> GetSeries(Study study);

    /// <summary>
    /// Gets the series associated with the study specified
    /// </summary>
    /// <param name="studies">Study</param>
    /// <returns>SeriesCollection</returns>
    List<Series> GetSeries(List<Study> studies);

    /// <summary>
    /// Gets a single series specified by the uid
    /// </summary>
    /// <param name="uid">Series uid</param>
    /// <returns>Series</returns>
    Series GetSeries(string uid);

    /// <summary>
    /// Refreshes the series
    /// </summary>
    /// <param name="uid">Series uid</param>
    /// <returns>Series</returns>
    void GetSeries(Series series);

    #endregion


    #endregion



  } // interface IBrowerProvider

} // namespace Viatronix.Enterprise.SDK.Providers
