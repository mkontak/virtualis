using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Viatronix.Utilities.Importer.Data
{
  /// <summary>
  /// The file information.
  /// </summary>
  public class FileParseInfo
  {

    #region fields

    /// <summary>
    /// The source file.
    /// </summary>
    private string m_source = string.Empty;

    /// <summary>
    /// The relative destination of the file off of the series' top level directory.  
    /// For example, a session's movie file would be for example, Movies\whatever.avi would appear in Patient\Study\Series\Movies\whatever.avi.
    /// </summary>
    private string m_destination = string.Empty;

    /// <summary>
    /// The series type.
    /// </summary>
    private SeriesTypes m_seriesType = SeriesTypes.Dicom;

    /// <summary>
    /// The replacement characters for the placeholder.
    /// </summary>
    private string m_macroReplace = string.Empty;

    /// <summary>
    /// The dataset the file belongs to.
    /// </summary>
    private string m_dataset = "-1u";

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="source">The source file.</param>
    public FileParseInfo(string source)
    {
      m_source = source;

      // The source filename will appear at the top level of the series' directory.
      m_destination = Path.GetFileName(source);
      m_dataset = Utils.GetDataSet(source);
    } // FileParseInfo( source )


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="source">The source.</param>
    /// <param name="destination">The destination.</param>
    public FileParseInfo(string source, string destination)
    {
      m_source = source;
      m_destination = destination;
      m_dataset = Utils.GetDataSet(source);
    } // FileParseInfo( source, destination )

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the source.
    /// </summary>
    public string Source
    {
      get { return m_source; }
      set { m_source = value; }
    } // Source


    /// <summary>
    /// Gets or sets the destination.
    /// </summary>
    public string Destination
    {
      get { return m_destination; }
      set { m_destination = value; }
    } // Destination


    /// <summary>
    /// Gets or sets the series type.
    /// </summary>
    public SeriesTypes SeriesType
    {
      get { return m_seriesType; }
      set { m_seriesType = value; }
    } // SeriesType


    /// <summary>
    /// Gets or sets the macro replace.
    /// </summary>
    public string MacroReplace
    {
      get { return m_macroReplace; }
      set { m_macroReplace = value; }
    } // MacroReplace


    /// <summary>
    /// Gets or sets the dataset.
    /// </summary>
    public string Dataset
    {
      get { return m_dataset; }
      set { m_dataset = value; }
    } // Dataset

    #endregion

  } // class FileParseInfo
} // namespace Viatronix.Utilities.Importer.Data
