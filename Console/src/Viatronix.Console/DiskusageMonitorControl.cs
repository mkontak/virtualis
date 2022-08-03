// $Id: DiskusageMonitorControl.cs,v 1.2 2007/03/02 14:13:41 romy Exp $ 
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Romy Sreedharan ( mailto:romy@viatronix.com )

using System;
using System.Windows.Forms;
using System.Drawing;

using Viatronix.UI;
using Viatronix.Utilities;
using Viatronix.Logging;

namespace Viatronix.Console
{

	/// <summary>
	/// Summary description for DiskusageControl.
	/// </summary>
	public class DiskusageMonitorControl : UserControl
	{

    #region fields

    /// <summary>
    /// Disk usage progress bar
    /// </summary>
    private Viatronix.UI.ProgressBar m_diskusageProgressbar;

		/// <summary> 
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

    /// <summary>
    /// Disk to monitor
    /// </summary>
    private string m_diskToMonitor = string.Empty;//"D:\\" ;

    /// <summary>
    /// Timer object which refreshes the Disk Usage
    /// </summary>
    private System.Windows.Forms.Timer m_timer;

    /// <summary>
    /// Refresh interval
    /// </summary>
    private int m_refreshInterval = 1000;

    #endregion

    #region properties
    
    /// <summary>
    /// sets or gets the Disc to monitor
    /// </summary>
    public string Disc
    {
      get { return m_diskToMonitor; }
      set { m_diskToMonitor = value;}
    }//Disc


    /// <summary>
    /// gets or sets the refresh Interval
    /// </summary>
    public int RefreshInterval
    {
      get { return m_refreshInterval/1000; }

      set
      { 
        m_refreshInterval = value * 1000; 
        
        if ( m_refreshInterval < 1000 )
        {
          m_timer.Stop();
          m_timer.Enabled = false;
        }
        else
        {
          m_timer.Enabled = true;
          m_timer.Start();
        }
      }
    }//RefreshInterval

    #endregion
    
    #region construction

    /// <summary>
    /// Constructor
    /// </summary>
    public DiskusageMonitorControl()
    {
      // This call is required by the Windows.Forms Form Designer.
      InitializeComponent();
      InitializeTimer ();
    }//DiskusageMonitorControl()


    /// <summary>
    /// Constructor
    /// </summary>
    public DiskusageMonitorControl( string discToMonitor, int refreshRate )
    {
      // This call is required by the Windows.Forms Form Designer.
      InitializeComponent();

      m_diskToMonitor   = discToMonitor;
      m_refreshInterval = refreshRate * 1000;

      InitializeTimer ();
      DisplayDiskUsage();
    }//DiskusageMonitorControl( string discToMonitor, int refreshRate )



      
    /// <summary> 
    /// Clean up any resources being used.
    /// </summary>
    protected override void Dispose( bool disposing )
    {
      if( disposing )
      {
        if(components != null)
        {
          components.Dispose();
        }
      }
      base.Dispose( disposing );
    }//Dispose( bool disposing )


    #region Component Designer generated code
    /// <summary> 
    /// Required method for Designer support - do not modify 
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      this.m_diskusageProgressbar = new Viatronix.UI.ProgressBar();
      this.SuspendLayout();
      // 
      // progressBar1
      // 
      this.m_diskusageProgressbar.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
        | System.Windows.Forms.AnchorStyles.Left) 
        | System.Windows.Forms.AnchorStyles.Right)));
      this.m_diskusageProgressbar.AnimationInterval = 100;
      this.m_diskusageProgressbar.BackColor = System.Drawing.Color.White;
      this.m_diskusageProgressbar.BorderColor = System.Drawing.Color.Black;
      this.m_diskusageProgressbar.DrawText = false;
      this.m_diskusageProgressbar.Location = new System.Drawing.Point(4, 4);
      this.m_diskusageProgressbar.Maximum = 100;
      this.m_diskusageProgressbar.Minimum = 0;
      this.m_diskusageProgressbar.Name = "progressBar1";
      this.m_diskusageProgressbar.Size = new System.Drawing.Size(234, 19);
      this.m_diskusageProgressbar.Step = 1;
      this.m_diskusageProgressbar.TabIndex = 0;
      this.m_diskusageProgressbar.TextColor = System.Drawing.Color.Black;
      this.m_diskusageProgressbar.TextHilightColor = System.Drawing.Color.White;
      this.m_diskusageProgressbar.Value = 50;
      this.m_diskusageProgressbar.Wrap = false;
      // 
      // DiskusageControl
      // 
      this.BackColor = System.Drawing.SystemColors.Control;
      this.Controls.Add(this.m_diskusageProgressbar);
      this.Name = "DiskusageControl";
      this.Size = new System.Drawing.Size(240, 27);
      this.ResumeLayout(false);

    }

    #endregion

    #endregion

    #region methods

    /// <summary>
    /// Initializes the Timer
    /// </summary>
    private void InitializeTimer ()
    {
      m_diskusageProgressbar.Minimum = 0;
      m_diskusageProgressbar.Maximum = 100;
      m_diskusageProgressbar.Step    = 1;
    
      m_timer = new System.Windows.Forms.Timer();
      m_timer.Tick += new System.EventHandler(this.OnDiskUsageTimerTick);

      if ( m_refreshInterval >= 1000 )
      {
        m_timer.Interval = m_refreshInterval;
        m_timer.Enabled = true;
        m_timer.Start();
      }
    }//InitializeTimer ()
  
    
    /// <summary>
    /// Displays the Disk Usage
    /// </summary>
    public void DisplayDiskUsage()
    {
      try
      {
        if ( Disc == string.Empty )
          return;

        Viatronix.Utilities.DiskSpace diskSpace = Viatronix.Utilities.SystemInfo.GetDiskSpace(Disc);
        m_diskusageProgressbar.Value  = (int)(diskSpace.UsedGbs / diskSpace.TotalGbs * 100.0);

        m_diskusageProgressbar.ForeColor = Color.Navy;
        m_diskusageProgressbar.TextHilightColor = Color.White;

        if ( m_diskusageProgressbar.Value >= 90 )
        {
          m_diskusageProgressbar.ForeColor = Color.Crimson;
        }
        else if ( m_diskusageProgressbar.Value >= 75 )
        {
          m_diskusageProgressbar.ForeColor = Color.Yellow;
          m_diskusageProgressbar.TextHilightColor = Color.Navy;
        }
      }
      catch ( Exception ex )
      {
        Viatronix.Logging.Log.Error( "Failed to get the disk space" + ex.Message, "DiskusageMonitorControl", "DisplayDiskUsage" );
      }
    }//DisplayDiskUsage()


    #endregion

    #region eventHandlers
    
    /// <summary>
    /// Handles the tick event to update the disk usage
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnDiskUsageTimerTick(object sender, System.EventArgs e)
    {
      m_timer.Stop();
      DisplayDiskUsage();
      m_timer.Start();
    }//OnDiskUsageTimerTick(object sender, System.EventArgs e)
    

    #endregion

	}//public class DiskusageMonitorControl : UserControl
}//namespace Viatronix.Console

#region revision History
// $Log: DiskusageMonitorControl.cs,v $
// Revision 1.2  2007/03/02 14:13:41  romy
// added code comments
//
// Revision 1.1  2006/02/02 21:37:05  romy
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console/DiskusageMonitorControl.cs,v 1.2 2007/03/02 14:13:41 romy Exp $
// $Id: DiskusageMonitorControl.cs,v 1.2 2007/03/02 14:13:41 romy Exp $
#endregion revision History
