// $Id: DiskMonitorPane.cs,v 1.1.2.3 2011/05/18 01:56:59 mkontak Exp $
//
// Copyright © 2008, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]


using System;
using System.Reflection;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Data;
using System.Windows.Forms;
using System.Runtime.Remoting.Messaging;
using Viatronix.UI;
using Viatronix.v3D.Core;
using Viatronix.Utilities.Registry;
using Viatronix.Enterprise.Entities;
using Viatronix.Enterprise.SDK.Providers;

namespace Viatronix.Console
{


  /// <summary>
  /// Disk Monitor Control will monitor the data drive 
  /// </summary>
  public partial class DiskMonitorPane : UserControl
  {

    #region constants

    private static Int64 m_gbDivisor = 1073741824;
    #endregion


    #region fields

    /// <summary>
    /// Data provider
    /// </summary>
    private IDataProvider m_dataProvider;

    /// <summary>
    /// Display drive system flag
    /// </summary>
    private bool m_displayDriveSystem = true;

    /// <summary>
    /// System the data drive is one
    /// </summary>
    private string m_dataDriveSystem = string.Empty;

    /// <summary>
    /// Capacity of the data drive in bytes
    /// </summary>
    private Int64 m_capacity = 0;

    /// <summary>
    /// Used space in bytes
    /// </summary>
    private Int64 m_usedSpace = 0;

    /// <summary>
    /// Free space in bytes
    /// </summary>
    private Int64 m_freeSpace = 0;

    /// <summary>
    /// Center point of panel control
    /// </summary>
    private Point m_center;

    /// <summary>
    /// Time interval
    /// </summary>
    private int m_diskMonitorInterval = 5000;

    /// <summary>
    /// Monitor bar rectangle
    /// </summary>
    private Rectangle m_monitorBarRect;

     /// <summary>
    /// Disk warning threshold
    /// </summary>
    private int m_diskWarningThreshold = SystemSettings.CurrentSystemSettings.DiskWarningThreshold;

    /// <summary>
    /// Disk error threshold
    /// </summary>
    private int m_diskErrorThreshold = SystemSettings.CurrentSystemSettings.DiskErrorThreshold;

    #endregion

    #region constructors

    /// <summary>
    /// Constructor
    /// </summary>
    public DiskMonitorPane()
    {
      m_dataProvider = Global.DataProvider;

      SetStyle(ControlStyles.AllPaintingInWmPaint, true);
      SetStyle(ControlStyles.DoubleBuffer, true);
      SetStyle(ControlStyles.UserPaint, true);

      SetStyle(ControlStyles.SupportsTransparentBackColor, true);

      this.BackColor = Theme.PaneBackColor;
      this.TabStop = false;

      InitializeComponent();

      // 
      Global.Frame.Panes.PaneExpanded += new RolloutEventHandler(this.OnPaneExpandedHandler);

      // Calculate the center point
      m_center =  new Point(this.Width / 2, this.Height / 2);

      // Start X for monitor bar
      int startX = m_center.X - 125;

      // Start y for monitor bar
      int startY = m_center.Y - 15;

      // Monitor bars rectangle
      m_monitorBarRect = new Rectangle(startX, startY, 250, 30);

      // Get the data drive's system
      m_dataDriveSystem = SystemSettings.CurrentSystemSettings.CacheServer;
      if (m_dataDriveSystem.Length == 0)
        m_dataDriveSystem = System.Windows.Forms.SystemInformation.ComputerName;

      // Update the drive inrofmation
      UpdateData();

      //////////////////////////////////////////
      // Get the disk monitors update interval
      //////////////////////////////////////////
      if ( System.Configuration.ConfigurationManager.AppSettings["DiskMonitorInterval"] != null )
      {
        int appSetting = System.Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["DiskMonitorInterval"]);
        m_diskMonitorInterval = appSetting  * 1000 * 60 * 60;
      } // END .... If the DiskMonitorInterval is in the app config

      //////////////////////////////////////////////////////////
      // Get the display drive system flag from the app config
      ///////////////////////////////////////////////////////////
      if (System.Configuration.ConfigurationManager.AppSettings["DiskMonitorDisplayDriveSystem"] != null)
      {
        m_displayDriveSystem = System.Convert.ToBoolean(System.Configuration.ConfigurationManager.AppSettings["DiskMonitorDisplayDriveSystem"]);
      } // END ... If the DiskMonitorDisplayDriveSystem is in the app config

      // Set up the timer
      m_timer.Interval = m_diskMonitorInterval;
      m_timer.Tick += new EventHandler(this.OnTickHandler);

      //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // If there is a capacity then start the timer. No capacity means we cannot get the information so the control will be 
      // blank
      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      if (m_capacity > 0)
      {
        m_timer.Start();
      }

    }

    #endregion


    #region methods

    /// <summary>
    /// Update the drive data
    /// </summary>
    private void UpdateData()
    {

      
      Store store = m_dataProvider.GetStore( "Default");

      m_capacity = store.Capacity;
      m_usedSpace = store.Used;
      m_freeSpace = store.Available;

    } // UodateData()

    #endregion


    #region overrides

    /// <summary>
    /// Overrides the OnMouseHover handler so that a tool tip can be displayed
    /// </summary>
    /// <param name="e"></param>
    protected override void OnMouseHover(EventArgs e)
    {

      base.OnMouseHover(e);


      UpdateData();

      if ( m_capacity > 0 )
      {

        float capacityGB = m_capacity / m_gbDivisor;
        float freeSpaceGB = m_freeSpace / m_gbDivisor;
        float usedSpaceGB = m_usedSpace / m_gbDivisor;

        m_toolTip.SetToolTip(this, "Total Capacity: " + capacityGB.ToString() + "GB,   Used Space: " + usedSpaceGB.ToString() + "GB,   Free Space: " + freeSpaceGB.ToString() + "GB");

      }
    } // OnMouseHover(EventArgs e)

    /// <summary>
    /// Overrides the OnPaint handler so that I can paint the monitor bar
    /// </summary>
    /// <param name="e"></param>
    protected override void OnPaint(PaintEventArgs e)
    {


      base.OnPaint(e);

      UpdateData();

      if (m_capacity > 0)
      {

        Pen pen = new Pen(Color.Black, 2);

        e.Graphics.DrawRectangle(pen, m_monitorBarRect);
        e.Graphics.FillRectangle(Brushes.White, m_monitorBarRect);

        float capacityGB = m_capacity / m_gbDivisor;
        float freeSpaceGB = m_freeSpace / m_gbDivisor;
        float usedSpaceGB = m_usedSpace / m_gbDivisor;

        float used = (float)(usedSpaceGB / capacityGB);
        int pctUsed = (int)(used * 100.0F);


        Rectangle pctRect = new Rectangle(m_monitorBarRect.X, m_monitorBarRect.Y, (int)(250 * used), 30);

        Brush brush = new SolidBrush(Color.FromArgb(107, 252, 82));

        if (pctUsed >= m_diskErrorThreshold)
        {
          brush = new SolidBrush(Color.FromArgb(228, 7, 7));
        }
        else if (pctUsed >= m_diskWarningThreshold)
        {
          brush = Brushes.Yellow;
        }

        e.Graphics.FillRectangle(brush, pctRect);

        float actualPct = (pctUsed * 100.0F);
        string pct = pctUsed.ToString() + "%";
        e.Graphics.DrawString(pct, this.Font, Brushes.Black, new Point(m_center.X - 5, m_center.Y - 5));

        Font font = new Font(this.Font.FontFamily, this.Font.Size,  FontStyle.Bold);

        string dataDrive = "Data Drive" + (m_displayDriveSystem ? " on " + m_dataDriveSystem : "");
        e.Graphics.DrawString( dataDrive, font, Brushes.White, new Point(m_monitorBarRect.X, m_monitorBarRect.Y - 17));

      }
    } // OnPaint(PaintEventArgs e)

    #endregion




    #region event handlers

    /// <summary>
    /// Hanldes the Tick event from the timer so the monitor can be updated.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="args"></param>
    private void OnTickHandler(object sender, EventArgs args)
    {
      // Stop timer
      m_timer.Stop();

      // Invalidate the control so that it is repainted
      this.Invalidate();

      // Start the timer
      m_timer.Start();
    } // OnTickHandler(object sender, EventArgs args)

    /// <summary>
    /// Assigns the SearchManager.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An RolloutEventArgs that contains the event data.</param>
    private void OnPaneExpandedHandler(object sender, RolloutEventArgs args)
    {
      // Nothing
    }//OnPaneExpandedHandler( sender, args )

    #endregion

  } // class DiskMonitorPane

} // Vatronix.v3D.Console


#region revision history

// $Log: DiskMonitorPane.cs,v $
// Revision 1.1.2.3  2011/05/18 01:56:59  mkontak
// Issue CON-1640: Windows 7 Compliance by removing the SystemSettings
// registry to a global config file. Move the default log directory to the windows
// common application data directory (XP - C:\Document and Settings\All Users\Application Data , Win7 - C:\ProgramData ).
// Merged the Viatronix.v3D.Logger project into the Viatronix.v3D.Utilities project and
// signed the Utilities assembly.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.1.2.2  2008/04/17 19:04:34  mkontak
// no message
//
// Revision 1.1.2.1  2008/04/17 18:07:29  mkontak
// Added disk monitor
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console/panes/DiskMonitorPane.cs,v 1.1.2.3 2011/05/18 01:56:59 mkontak Exp $
// $Id: DiskMonitorPane.cs,v 1.1.2.3 2011/05/18 01:56:59 mkontak Exp $

#endregion
