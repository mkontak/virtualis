using System;
using System.Data;
using System.Drawing;
using System.Windows.Forms;
namespace Viatronix.UI
{
	/// <summary>
	/// Summary description for AutoResizeDataGridTableStyle.
	/// </summary>
	public class AutoResizeDataGridTableStyle: DataGridTableStyle
	{

    #region constants

    /// <summary>
    /// Offset from the the end of the grid. Used to prevent the scroll barfrom being displayed.
    /// </summary>
		private const int m_endOfGridOffset = 5;

    /// <summary>
    /// Best guess for the number of pixels in a character
    /// </summary>
    private const int m_columnWidthMultiplier = 8;

    #endregion

    #region field

    /// <summary>
    /// Flag to indicate that the data is to be resized to the columns row data
    /// </summary>
    private bool m_resizeColumnsBasedOnRowData = false;

    #endregion

    #region constructors

    /// <summary>
    /// Constructor
    /// </summary>
		public AutoResizeDataGridTableStyle(): base()
		{
		}

    /// <summary>
    /// Constructor
    /// </summary>
    public AutoResizeDataGridTableStyle(string mappingName): base()
    {
      this.MappingName = mappingName;
    }

    /// <summary>
    /// Constructor
    /// </summary>
    public AutoResizeDataGridTableStyle(string mappingName, bool resizeColumnsBasedOnRowData): base()
    {
      this.MappingName    = mappingName;
      m_resizeColumnsBasedOnRowData = resizeColumnsBasedOnRowData;
    }

    #endregion

    #region properties

   
    /// <summary>
    /// Sets/Gets the DataGrid
    /// </summary>
    public override System.Windows.Forms.DataGrid DataGrid
    {
      get
      {
        return base.DataGrid;
      }
      set
      {
        base.DataGrid = value;

        BackColor = base.DataGrid.BackColor;
        AlternatingBackColor = base.DataGrid.AlternatingBackColor;
        ForeColor = base.DataGrid.ForeColor;
        GridLineColor = base.DataGrid.GridLineColor;
        LinkColor = base.DataGrid.LinkColor;
        SelectionBackColor = base.DataGrid.SelectionBackColor;
        SelectionForeColor = base.DataGrid.SelectionForeColor;
        ColumnHeadersVisible = base.DataGrid.ColumnHeadersVisible;
        RowHeadersVisible = base.DataGrid.RowHeadersVisible;
        HeaderFont = base.DataGrid.HeaderFont;
        HeaderBackColor = base.DataGrid.HeaderBackColor;
        HeaderForeColor = base.DataGrid.HeaderForeColor;

        /// Set the event handlers
        base.DataGrid.DataSourceChanged += new EventHandler(this.OnDataSourceChanged);
        base.DataGrid.Resize += new EventHandler(this.OnDataGridResize);


      }
    } // DataGrid 

    #endregion

    #region event handlers

		/// <summary>
		/// Called when the DataSource property of the parent DataGrid 
		/// changes. When the new source is a DataTable, rebuild the 
		/// DataGridColumnStyles and resize.
		/// </summary>
		/// <param name="sender">Sending object</param>
		/// <param name="e">Event arguements</param>
		public void OnDataSourceChanged(object sender, EventArgs e)
		{


      /// Clear all previsoud styles
			GridColumnStyles.Clear();

			if(DataGrid != null && DataGrid.DataSource != null && ( DataGrid.DataSource is DataTable || ( DataGrid.DataMember.Length > 0 && DataGrid.DataSource is DataSet ) ) )
			{
				DataTable table = (DataGrid.DataSource is DataTable ? (DataTable)DataGrid.DataSource : ((DataSet)DataGrid.DataSource).Tables[DataGrid.DataMember] );

				foreach(DataColumn column in table.Columns)
				{
        
          int width = -1;
          if ( column.ExtendedProperties["width"] != null )
          {
            width = Int32.Parse(column.ExtendedProperties["width"].ToString());
          }

          DataGridColumnStyle style = null;

          if ( column.DataType == typeof(bool) )
          {
            style = new DataGridBoolColumn();
          }
          else
          {
            style = new DataGridTextBoxColumn();
          }

					style.HeaderText = ( column.Caption.Length > 0 ? column.Caption : column.ColumnName );
					style.MappingName = column.ColumnName;
          style.NullText = string.Empty;
 
          int rowWidth      = ( m_resizeColumnsBasedOnRowData ? GetMaximumRowWidth(table, column.ColumnName) : GetAverageRowWidth(table, column.ColumnName));
          int captionLength = column.Caption.Length;

          style.Width = (width == -1 ? ( rowWidth < captionLength ? captionLength : rowWidth )  : width ) * m_columnWidthMultiplier;
          
					GridColumnStyles.Add(style);
				}
			}
			// Call the eventhandler for resize events
			OnDataGridResize(this,new EventArgs());
		} // OnDataSourceChanged()

    /// <summary>
    /// Handles the Data Grid Resize Event
    /// </summary>
    /// <param name="sender">Sending object</param>
    /// <param name="e">Event arguments</param>
		public void OnDataGridResize(object sender, EventArgs e)
		{
			// Parent?
			if(DataGrid != null)
			{
        
				// Get column width
				int columnWidth = GetGridColumnWidth();

        ////////////////////////////////
        // Make sure there is a width
        ////////////////////////////////
				if( columnWidth != -1 )
				{
					// Get the client width
					int clientWidth = DataGrid.ClientSize.Width;

					// Are there columns? redundant check
					if( GridColumnStyles.Count > 0 )
					{
						// whats the newWidth
						int newWidth = GridColumnStyles[GridColumnStyles.Count - 1].Width + clientWidth - columnWidth;

						// is the new width valid?
						if( newWidth > PreferredColumnWidth )
							GridColumnStyles[GridColumnStyles.Count - 1].Width = newWidth;
						else
							GridColumnStyles[GridColumnStyles.Count - 1].Width = PreferredColumnWidth;
					}
				}
				// Redraw
				DataGrid.Invalidate(true);
			}
		} // OnDataGridResize()

    #endregion

    #region methods

    /// <summary>
    /// Gets the avregae length of the row data for the table/column specified
    /// </summary>
    /// <param name="table">Table to get row data from</param>
    /// <param name="columnName">Column name to average the row length</param>
    /// <returns>Returns thee average length of the data in the rows</returns>
    private int GetAverageRowWidth(DataTable table, string columnName)
    {

      int avgRowWidth = 0;
      foreach( DataRow row in table.Rows )
      {
        avgRowWidth += row[columnName].ToString().Length;
      }

      return ( table.Rows.Count == 0 ? 0 : (avgRowWidth / table.Rows.Count));

    } // GetAverageRowWidth()

    /// <summary>
    /// Gets the maximum row length of the row data for the table/column specified
    /// </summary>
    /// <param name="table">Table to get row data from</param>
    /// <param name="columnName">Column name to average the row length</param>
    /// <returns>Returns thee average length of the data in the rows</returns>
    private int GetMaximumRowWidth(DataTable table, string columnName)
    {

      int maxRowWidth = 0;
      foreach( DataRow row in table.Rows )
      {
        maxRowWidth = System.Math.Max(maxRowWidth, row[columnName].ToString().Length);
      }

      return maxRowWidth;

    } // GetMaximumRowLength()

    /// <summary>
    /// Gets the Grid Column Width
    /// </summary>
    /// <returns>Returns the grid column width</returns>
		private int GetGridColumnWidth()
		{
			// No columns, return error
			if(GridColumnStyles.Count == 0)
				return -1;
		
      /// Initialize the width to 0
			int width = 0;

      //////////////////////////////////////////////////////
      // Iterate through all the data grid column styles
      //////////////////////////////////////////////////////
			foreach( DataGridColumnStyle columnStyle in GridColumnStyles )
			{
				width += columnStyle.Width;
			} // END .. For each data grid column style
		
      // Returns the width
			return (width + m_endOfGridOffset);

		} // GetGridColumnWidth()

    #endregion

	}
}
