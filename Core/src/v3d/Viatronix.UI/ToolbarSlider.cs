// $Id: ToolbarSlider.cs,v 1.1.2.1 2009/03/24 20:46:06 kchalupa Exp $
//
// Copyright © 2009, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Kyle R Chalupa] [kchalupa@viatronix.com]

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace Viatronix.v3D.UI
{
  /// <summary>
  /// A slider usable inside a toolbar setting.
  /// </summary>
  public partial class ToolbarSlider : UserControl
  {

    #region events

    /// <summary>
    /// Indicates the value is about to be changed.
    /// </summary>
    public event EventHandler ValueChanging;

    /// <summary>
    /// Indicates that the value has changed.
    /// </summary>
    public event EventHandler ValueChanged;

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public ToolbarSlider()
    {
      InitializeComponent();
    } // ToolbarSlider()

    #endregion

    #region properties

    /// <summary>
    /// The value the slider currently has.
    /// </summary>
    public int Value
    {
      get { return m_slider.Value;  }
      set { m_slider.Value = value; }
    } // Value


    /// <summary>
    /// The minimum value the slider can take on.
    /// </summary>
    public int Minimum
    {
      get { return m_slider.Min;  }
      set { m_slider.Min = value; }
    } // Minimum


    /// <summary>
    /// The maximum value the slider can take on.
    /// </summary>
    public int Maximum
    {
      get { return m_slider.Max; }
      set { m_slider.Max = value; }
    } // Minimum

    #endregion

    #region event handlers

    /// <summary>
    /// Paints a border around the control.
    /// </summary>
    /// <param name="e">Arguments that assist in painting.</param>
    protected override void OnPaint(PaintEventArgs e)
    {
      using (Pen pen = new Pen(Color.FromArgb(201, 206, 221)))
      {
        e.Graphics.DrawPath(pen, Utilities.GetRectangleWithRounedEdges(this.Bounds, 10));
      }
      
      base.OnPaint(e);
    } // OnPaint(PaintEventArgs e)


    /// <summary>
    /// Handles the Slider Vale Changed event.
    /// </summary>
    /// <param name="sender">The point of origin.</param>
    /// <param name="e">The arguments to take into account.</param>
    private void OnSliderValueChanged(object sender, EventArgs e)
    {
      if(ValueChanging != null)
      {
        ValueChanging(this, System.EventArgs.Empty);
      }

      m_valueLabel.Text = Value.ToString();

      if (ValueChanged != null)
      {
        ValueChanged(this, System.EventArgs.Empty);
      }
    } // OnSliderValueChanged(object sender, EventArgs e)

    #endregion

  } // ToolbarSlider
} // namespace Viatronix.v3D.UI

#region revision history

// $Log: ToolbarSlider.cs,v $
// Revision 1.1.2.1  2009/03/24 20:46:06  kchalupa
// Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.UI/ToolbarSlider.cs,v 1.1.2.1 2009/03/24 20:46:06 kchalupa Exp $
// $Id: ToolbarSlider.cs,v 1.1.2.1 2009/03/24 20:46:06 kchalupa Exp $

#endregion