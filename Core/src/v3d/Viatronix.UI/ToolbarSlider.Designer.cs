// $Id: ToolbarSlider.Designer.cs,v 1.1.2.1 2009/03/24 20:46:06 kchalupa Exp $
//
// Copyright © 2009, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Kyle R Chalupa] [kchalupa@viatronix.com]

namespace Viatronix.v3D.UI
{

  /// <summary>
  /// A slider usable inside a toolbar setting.
  /// </summary>
  partial class ToolbarSlider
  {
    /// <summary> 
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    /// <summary> 
    /// Clean up any resources being used.
    /// </summary>
    /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    protected override void Dispose(bool disposing)
    {
      if (disposing && (components != null))
      {
        components.Dispose();
      }
      base.Dispose(disposing);
    }

    #region Component Designer generated code

    /// <summary> 
    /// Required method for Designer support - do not modify 
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      this.components = new System.ComponentModel.Container();
      this.m_slider = new Viatronix.v3D.UI.Slider();
      this.m_valueLabel = new System.Windows.Forms.Label();
      this.m_toolTip = new System.Windows.Forms.ToolTip(this.components);
      this.SuspendLayout();
      // 
      // m_slider
      // 
      this.m_slider.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.m_slider.DisplayBorder = false;
      this.m_slider.EventsWhildAdjusting = false;
      this.m_slider.Location = new System.Drawing.Point(-2, 1);
      this.m_slider.Max = 100;
      this.m_slider.Min = 0;
      this.m_slider.Name = "m_slider";
      this.m_slider.ShowText = false;
      this.m_slider.Size = new System.Drawing.Size(85, 33);
      this.m_slider.SliderColor = System.Drawing.Color.FromArgb(((int)(((byte)(89)))), ((int)(((byte)(116)))), ((int)(((byte)(153)))));
      this.m_slider.SliderType = Viatronix.v3D.UI.SliderType.Wedge;
      this.m_slider.Suffix = "";
      this.m_slider.TabIndex = 0;
      this.m_slider.Text = "slider1";
      this.m_slider.Value = 50;
      this.m_slider.Changed += new System.EventHandler(this.OnSliderValueChanged);
      // 
      // m_valueLabel
      // 
      this.m_valueLabel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.m_valueLabel.AutoSize = true;
      this.m_valueLabel.Location = new System.Drawing.Point(80, 4);
      this.m_valueLabel.Name = "m_valueLabel";
      this.m_valueLabel.Size = new System.Drawing.Size(13, 13);
      this.m_valueLabel.TabIndex = 1;
      this.m_valueLabel.Text = "0";
      this.m_valueLabel.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
      // 
      // ToolbarSlider
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(217)))), ((int)(((byte)(222)))), ((int)(((byte)(237)))));
      this.Controls.Add(this.m_valueLabel);
      this.Controls.Add(this.m_slider);
      this.ForeColor = System.Drawing.Color.Black;
      this.Name = "ToolbarSlider";
      this.Size = new System.Drawing.Size(114, 29);
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    /// <summary>
    /// The slider that will be manipulated.
    /// </summary>
    private Slider m_slider;

    /// <summary>
    /// The label that will display the slider's current value.
    /// </summary>
    private System.Windows.Forms.Label m_valueLabel;

    /// <summary>
    /// The tooltip that displays information about the control's internal components.
    /// </summary>
    private System.Windows.Forms.ToolTip m_toolTip;

  } // ToolbarSlider
} // namespace Viatronix.v3D.UI

#region revision history

// $Log: ToolbarSlider.Designer.cs,v $
// Revision 1.1.2.1  2009/03/24 20:46:06  kchalupa
// Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.UI/ToolbarSlider.Designer.cs,v 1.1.2.1 2009/03/24 20:46:06 kchalupa Exp $
// $Id: ToolbarSlider.Designer.cs,v 1.1.2.1 2009/03/24 20:46:06 kchalupa Exp $

#endregion