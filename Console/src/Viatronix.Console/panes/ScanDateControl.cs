using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using Viatronix.v3D.Core;
using Viatronix.UI;
using Viatronix.Enterprise;

namespace Viatronix.Console
{

  #region enum ScanDateStyle

  /// <summary>
  /// Enumeration of scan date display styles
  /// </summary>
  public enum ScanDateStyle : int
  {
    None      = 0,
    Specific  = 1,
    Range     = 2
  }; // ScanDateStyle

  #endregion

  public partial class ScanDateControl : UserControl
  {

    #region fields

    /// <summary>
    /// Provideds an GUI for retrieving the scan date.
    /// </summary>
    private Viatronix.UI.DateChooser m_scanDate;

    /// <summary>
    /// Determines which type of search, descrete or range, is being performed.
    /// </summary>
    private Viatronix.UI.ComboBox m_scanDateStyle;

    /// <summary>
    /// Label for the scan date area.
    /// </summary>
    private System.Windows.Forms.Label m_scanDateLabel;

 
    #endregion

    #region constructors
    
    public ScanDateControl()
    {
      SetStyle(ControlStyles.AllPaintingInWmPaint, true);
      SetStyle(ControlStyles.DoubleBuffer, true);
      SetStyle(ControlStyles.UserPaint, true);

      SetStyle(ControlStyles.SupportsTransparentBackColor, true);


      InitializeComponent();
    }

    #endregion

    #region proprties

    /// <summary>
    /// Sets/Gets the style
    /// </summary>
    public ScanDateStyle Style
    {
      get { return (ScanDateStyle)m_scanDateStyle.SelectedIndex; }
      set { m_scanDateStyle.SelectedIndex = (int)value; }
    }

    #endregion

    #region methods

    /// <summary>
    /// Appends the dat information to the querybuilder.
    /// </summary>
    /// <param name="builder">QueryBuilder object</param>
    public bool AddDate(XmlBuilder query)
    {
      if (m_scanDateStyle.SelectedIndex == 0)
        return false;

      RelationalOp op = GetScanDateRelation(m_scanDate.SelectedComparisions[0]);

      if (m_scanDateStyle.SelectedIndex == 1)
      {
        switch (op)
        {
          case RelationalOp.Equals:
            query.AddAttributeRange("date", m_scanDate.SelectedDates[0].ToString("O"), m_scanDate.SelectedDates[0].ToString("O"));
            break;
          case RelationalOp.LessThan:
            query.AddAttributeRange("date", "", m_scanDate.SelectedDates[0].ToString("O"));
            break;
          case RelationalOp.LessThanOrEqual:
            query.AddAttributeRange("date", "", m_scanDate.SelectedDates[0].ToString("O"));
            break;
          case RelationalOp.GreaterThan:
            query.AddAttributeRange("date", m_scanDate.SelectedDates[0].ToString("O"), "");
            break;
          case RelationalOp.GreaterThanOrEqual:
            query.AddAttributeRange("date", m_scanDate.SelectedDates[0].ToString("O"), "");
            break;


        }

      }
      else if (m_scanDateStyle.SelectedIndex == 2)
        query.AddAttributeRange("date", m_scanDate.SelectedDates[0].ToString("O"), m_scanDate.SelectedDates[1].ToString("O"));


      return true;
    } // AppendDate( builder )



    /// <summary>
    /// Determines the scan date relational operator.
    /// </summary>
    /// <param name="comparision">An enumeration of date copmarisions.</param>
    /// <returns>The relational operation associated with the date comparision.</returns>
    private RelationalOp GetScanDateRelation(DateComparisons comparision)
    {
      switch (comparision)
      {
        case DateComparisons.Equal:
          return RelationalOp.Equals;

        case DateComparisons.LessThan:
          return RelationalOp.LessThan;

        case DateComparisons.LessThanEqual:
          return RelationalOp.LessThanOrEqual;

        case DateComparisons.GreaterThan:
          return RelationalOp.GreaterThan;

        case DateComparisons.GreaterThanEqual:
          return RelationalOp.GreaterThanOrEqual;

        default:
          throw new NotSupportedException("The selected comparision is not supported.");
      }
    } // GetScanDateRelation( comparision )

    #endregion

    #region event handlers

    /// <summary>
    /// Tracks the change of the scan date.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnScanDateChangedHandler(object sender, EventArgs args)
    {
      DateTime[] dates = m_scanDate.SelectedDates;
      if (m_scanDateStyle.SelectedIndex == 1)
        m_scanDate.IsRangeSearch = false;
      else if (m_scanDateStyle.SelectedIndex == 2)
        m_scanDate.IsRangeSearch = true;

      m_scanDate.SetDates(dates);
      m_scanDate.Enabled = (m_scanDateStyle.SelectedIndex != 0);
    } // OnScanDateChangedHandler( sender, args )

    #endregion

  }
}
