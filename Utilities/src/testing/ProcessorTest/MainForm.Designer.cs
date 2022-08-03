namespace Viatronix.Utilities.Tests.ProcessorTest
{
  /// <summary>
  /// The main form for the test.
  /// </summary>
  partial class MainForm
  {

    #region fields

    /// <summary>
    /// Panel that evenly devides the space between the three text boxes.
    /// </summary>
    private System.Windows.Forms.TableLayoutPanel m_tableLayout;

    /// <summary>
    /// A group box to denote the text box's purpose.
    /// </summary>
    private System.Windows.Forms.GroupBox m_taskGroup;
    
    /// <summary>
    /// Text box to provide information about the task.
    /// </summary>
    private System.Windows.Forms.TextBox m_taskText;

    /// <summary>
    /// A group box to denote the text box's purpose.
    /// </summary>
    private System.Windows.Forms.GroupBox m_seriesGroup;

    /// <summary>
    /// Text box to provide information about the series.
    /// </summary>
    private System.Windows.Forms.TextBox m_seriesText;

    /// <summary>
    /// A group box to denote the text box's purpose.
    /// </summary>
    private System.Windows.Forms.GroupBox m_sourceGroup;

    /// <summary>
    /// Text box to provide information about the source.
    /// </summary>
    private System.Windows.Forms.TextBox m_sourceText;

    /// <summary>
    /// Button to stop the test.
    /// </summary>
    private System.Windows.Forms.Button m_stopButton;

    /// <summary>
    /// Button to start the test.
    /// </summary>
    private System.Windows.Forms.Button m_startButton;

    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    #endregion

    #region methods

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
    } // Dispose( disposing )


    /// <summary>
    /// Required method for Designer support.
    /// </summary>
    private void InitializeComponent()
    {
      this.m_stopButton = new System.Windows.Forms.Button();
      this.m_startButton = new System.Windows.Forms.Button();
      this.m_tableLayout = new System.Windows.Forms.TableLayoutPanel();
      this.m_taskGroup = new System.Windows.Forms.GroupBox();
      this.m_taskText = new System.Windows.Forms.TextBox();
      this.m_seriesGroup = new System.Windows.Forms.GroupBox();
      this.m_seriesText = new System.Windows.Forms.TextBox();
      this.m_sourceGroup = new System.Windows.Forms.GroupBox();
      this.m_sourceText = new System.Windows.Forms.TextBox();
      this.m_tableLayout.SuspendLayout();
      this.m_taskGroup.SuspendLayout();
      this.m_seriesGroup.SuspendLayout();
      this.m_sourceGroup.SuspendLayout();
      this.SuspendLayout();
      // 
      // m_stopButton
      // 
      this.m_stopButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.m_stopButton.Location = new System.Drawing.Point(365, 278);
      this.m_stopButton.Name = "m_stopButton";
      this.m_stopButton.Size = new System.Drawing.Size(75, 23);
      this.m_stopButton.TabIndex = 0;
      this.m_stopButton.Text = "Stop";
      this.m_stopButton.UseVisualStyleBackColor = true;
      this.m_stopButton.Click += new System.EventHandler(this.OnStopClickHandler);
      // 
      // m_startButton
      // 
      this.m_startButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.m_startButton.Location = new System.Drawing.Point(284, 278);
      this.m_startButton.Name = "m_startButton";
      this.m_startButton.Size = new System.Drawing.Size(75, 23);
      this.m_startButton.TabIndex = 1;
      this.m_startButton.Text = "Start";
      this.m_startButton.UseVisualStyleBackColor = true;
      this.m_startButton.Click += new System.EventHandler(this.OnStartClickHandler);
      // 
      // m_tableLayout
      // 
      this.m_tableLayout.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                  | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.m_tableLayout.ColumnCount = 1;
      this.m_tableLayout.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
      this.m_tableLayout.Controls.Add(this.m_taskGroup, 0, 2);
      this.m_tableLayout.Controls.Add(this.m_seriesGroup, 0, 1);
      this.m_tableLayout.Controls.Add(this.m_sourceGroup, 0, 0);
      this.m_tableLayout.Location = new System.Drawing.Point(13, 13);
      this.m_tableLayout.Name = "m_tableLayout";
      this.m_tableLayout.RowCount = 3;
      this.m_tableLayout.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 33.33333F));
      this.m_tableLayout.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 33.33333F));
      this.m_tableLayout.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 33.33333F));
      this.m_tableLayout.Size = new System.Drawing.Size(427, 259);
      this.m_tableLayout.TabIndex = 2;
      // 
      // m_taskGroup
      // 
      this.m_taskGroup.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                  | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.m_taskGroup.Controls.Add(this.m_taskText);
      this.m_taskGroup.Location = new System.Drawing.Point(3, 175);
      this.m_taskGroup.Name = "m_taskGroup";
      this.m_taskGroup.Size = new System.Drawing.Size(421, 81);
      this.m_taskGroup.TabIndex = 2;
      this.m_taskGroup.TabStop = false;
      this.m_taskGroup.Text = "Task";
      // 
      // m_taskText
      // 
      this.m_taskText.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                  | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.m_taskText.Location = new System.Drawing.Point(7, 16);
      this.m_taskText.Name = "m_taskText";
      this.m_taskText.Size = new System.Drawing.Size(408, 20);
      this.m_taskText.TabIndex = 0;
      // 
      // m_seriesGroup
      // 
      this.m_seriesGroup.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                  | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.m_seriesGroup.Controls.Add(this.m_seriesText);
      this.m_seriesGroup.Location = new System.Drawing.Point(3, 89);
      this.m_seriesGroup.Name = "m_seriesGroup";
      this.m_seriesGroup.Size = new System.Drawing.Size(421, 80);
      this.m_seriesGroup.TabIndex = 1;
      this.m_seriesGroup.TabStop = false;
      this.m_seriesGroup.Text = "Series";
      // 
      // m_seriesText
      // 
      this.m_seriesText.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                  | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.m_seriesText.Location = new System.Drawing.Point(7, 16);
      this.m_seriesText.Name = "m_seriesText";
      this.m_seriesText.Size = new System.Drawing.Size(408, 20);
      this.m_seriesText.TabIndex = 0;
      // 
      // m_sourceGroup
      // 
      this.m_sourceGroup.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                  | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.m_sourceGroup.Controls.Add(this.m_sourceText);
      this.m_sourceGroup.Location = new System.Drawing.Point(3, 3);
      this.m_sourceGroup.Name = "m_sourceGroup";
      this.m_sourceGroup.Size = new System.Drawing.Size(421, 80);
      this.m_sourceGroup.TabIndex = 0;
      this.m_sourceGroup.TabStop = false;
      this.m_sourceGroup.Text = "Source";
      // 
      // m_sourceText
      // 
      this.m_sourceText.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                  | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.m_sourceText.Location = new System.Drawing.Point(7, 16);
      this.m_sourceText.Name = "m_sourceText";
      this.m_sourceText.Size = new System.Drawing.Size(408, 20);
      this.m_sourceText.TabIndex = 0;
      // 
      // MainForm
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(452, 313);
      this.Controls.Add(this.m_tableLayout);
      this.Controls.Add(this.m_startButton);
      this.Controls.Add(this.m_stopButton);
      this.Name = "MainForm";
      this.Text = "Processor Test";
      this.m_tableLayout.ResumeLayout(false);
      this.m_taskGroup.ResumeLayout(false);
      this.m_taskGroup.PerformLayout();
      this.m_seriesGroup.ResumeLayout(false);
      this.m_seriesGroup.PerformLayout();
      this.m_sourceGroup.ResumeLayout(false);
      this.m_sourceGroup.PerformLayout();
      this.ResumeLayout(false);

    } // InitializeComponent()

    #endregion

  } // class MainForm
} // namespace Viatronix.Utilities.Tests.ProcessorTest

