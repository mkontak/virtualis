using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace Viatronix.v3D.UI.Test
{
	/// <summary>
	/// Applications main Form.
	/// </summary>
	public class MainForm : Viatronix.v3D.UI.Dialog
	{
    #region fields

    private Viatronix.v3D.UI.Rollout m_rollout;
    private Viatronix.v3D.UI.RolloutPane rolloutTab1;
    private Viatronix.v3D.UI.ProgressBar m_progressBar;
    private Viatronix.v3D.UI.CheckBox m_progressAnimate;
    private Viatronix.v3D.UI.CheckBox m_progressDisplayText;
    private System.Windows.Forms.Label label1;
    private System.Windows.Forms.Label label2;
    private System.Windows.Forms.Label label3;
    private System.Windows.Forms.Label label4;
    private Viatronix.v3D.UI.ColorAdjustor m_progressTextHighlightColor;
    private Viatronix.v3D.UI.ColorAdjustor m_progressTextColor;
    private Viatronix.v3D.UI.ColorAdjustor m_progressBackColor;
    private Viatronix.v3D.UI.ColorAdjustor m_progressFillColor;
    private System.Windows.Forms.Label label5;
    private Viatronix.v3D.UI.ColorAdjustor m_progressBorderColor;
    private Viatronix.v3D.UI.RolloutPane rolloutTab2;
    private System.Windows.Forms.Label m_popupMenuLabel;
    private Viatronix.v3D.UI.RolloutPane rolloutTab3;
    private Viatronix.v3D.UI.Button m_showMessageBox;
    private Viatronix.v3D.UI.GroupBox groupBox1;
    private Viatronix.v3D.UI.RadioButton m_mbIconNone;
    private Viatronix.v3D.UI.RadioButton m_mbIconInfo;
    private Viatronix.v3D.UI.RadioButton m_mbIconWarning;
    private Viatronix.v3D.UI.RadioButton m_mbIconQuestion;
    private Viatronix.v3D.UI.RadioButton m_mbIconError;
    private Viatronix.v3D.UI.GroupBox groupBox2;
    private Viatronix.v3D.UI.RadioButton m_mbButtonOkCancel;
    private Viatronix.v3D.UI.RadioButton m_mbButtonOk;
    private Viatronix.v3D.UI.Button m_showWinformsMessageBox;
    private Viatronix.v3D.UI.RadioButton m_mbButtonYesNoCancel;
    private Viatronix.v3D.UI.RadioButton m_mbButtonYesNo;
    private System.Windows.Forms.Label label6;
    private System.Windows.Forms.TextBox m_mbCaption;
    private System.Windows.Forms.TextBox m_mbMessage;
    private System.Windows.Forms.Label label7;
    private Viatronix.v3D.UI.RadioButton m_mbButtonAbortRetryIgnore;
    private Viatronix.v3D.UI.RadioButton m_mbButtonRetryCancel;
    private Viatronix.v3D.UI.RolloutPane rolloutTab4;
    private Viatronix.v3D.UI.ListBox listBox1;
    private Viatronix.v3D.UI.RolloutPane rolloutTab5;
    private Viatronix.v3D.UI.RangeSlider rangeSlider1;
    private Viatronix.v3D.UI.RolloutPane rolloutPane1;
    private Viatronix.v3D.UI.RolloutPane rolloutPane2;
    private Viatronix.v3D.UI.VScrollBar vScrollBar1;
    private Viatronix.v3D.UI.Slider slider1;
    private Viatronix.v3D.UI.ComboBox comboBox1;
    private Viatronix.v3D.UI.Rebar rebar1;
		private System.ComponentModel.Container components = null;

    #endregion

    #region constructor

    public MainForm()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//
      comboBox1.DataSource = new Viatronix.v3D.UI.ComboBox.ObjectString[] {
                                                                            new Viatronix.v3D.UI.ComboBox.ObjectString(rolloutTab1, "1"),
                                                                            new Viatronix.v3D.UI.ComboBox.ObjectString(rolloutTab2, "2"),
                                                                            new Viatronix.v3D.UI.ComboBox.ObjectString(rolloutTab3, "3")};
      comboBox1.DisplayMember = "String";
      comboBox1.ValueMember = "Object";

      comboBox1.SelectedValue = rolloutTab1;

      if ((Viatronix.v3D.UI.RolloutPane) comboBox1.SelectedValue == rolloutTab2)
      {
        comboBox1.SelectedValue = rolloutTab3;
      }
		}

		
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
		}


    #endregion
		
    #region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
      this.m_rollout = new Viatronix.v3D.UI.Rollout();
      this.rolloutTab4 = new Viatronix.v3D.UI.RolloutPane();
      this.listBox1 = new Viatronix.v3D.UI.ListBox();
      this.rolloutTab3 = new Viatronix.v3D.UI.RolloutPane();
      this.m_mbMessage = new System.Windows.Forms.TextBox();
      this.label7 = new System.Windows.Forms.Label();
      this.m_mbCaption = new System.Windows.Forms.TextBox();
      this.label6 = new System.Windows.Forms.Label();
      this.m_showWinformsMessageBox = new Viatronix.v3D.UI.Button();
      this.groupBox2 = new Viatronix.v3D.UI.GroupBox();
      this.m_mbButtonAbortRetryIgnore = new Viatronix.v3D.UI.RadioButton();
      this.m_mbButtonRetryCancel = new Viatronix.v3D.UI.RadioButton();
      this.m_mbButtonYesNoCancel = new Viatronix.v3D.UI.RadioButton();
      this.m_mbButtonYesNo = new Viatronix.v3D.UI.RadioButton();
      this.m_mbButtonOkCancel = new Viatronix.v3D.UI.RadioButton();
      this.m_mbButtonOk = new Viatronix.v3D.UI.RadioButton();
      this.groupBox1 = new Viatronix.v3D.UI.GroupBox();
      this.m_mbIconQuestion = new Viatronix.v3D.UI.RadioButton();
      this.m_mbIconError = new Viatronix.v3D.UI.RadioButton();
      this.m_mbIconWarning = new Viatronix.v3D.UI.RadioButton();
      this.m_mbIconInfo = new Viatronix.v3D.UI.RadioButton();
      this.m_mbIconNone = new Viatronix.v3D.UI.RadioButton();
      this.m_showMessageBox = new Viatronix.v3D.UI.Button();
      this.rolloutTab2 = new Viatronix.v3D.UI.RolloutPane();
      this.m_popupMenuLabel = new System.Windows.Forms.Label();
      this.rolloutTab1 = new Viatronix.v3D.UI.RolloutPane();
      this.label5 = new System.Windows.Forms.Label();
      this.m_progressBorderColor = new Viatronix.v3D.UI.ColorAdjustor();
      this.label4 = new System.Windows.Forms.Label();
      this.m_progressTextHighlightColor = new Viatronix.v3D.UI.ColorAdjustor();
      this.label3 = new System.Windows.Forms.Label();
      this.m_progressTextColor = new Viatronix.v3D.UI.ColorAdjustor();
      this.label2 = new System.Windows.Forms.Label();
      this.m_progressBackColor = new Viatronix.v3D.UI.ColorAdjustor();
      this.label1 = new System.Windows.Forms.Label();
      this.m_progressFillColor = new Viatronix.v3D.UI.ColorAdjustor();
      this.m_progressDisplayText = new Viatronix.v3D.UI.CheckBox();
      this.m_progressAnimate = new Viatronix.v3D.UI.CheckBox();
      this.m_progressBar = new Viatronix.v3D.UI.ProgressBar();
      this.rolloutTab5 = new Viatronix.v3D.UI.RolloutPane();
      this.rangeSlider1 = new Viatronix.v3D.UI.RangeSlider();
      this.rolloutPane2 = new Viatronix.v3D.UI.RolloutPane();
      this.vScrollBar1 = new Viatronix.v3D.UI.VScrollBar();
      this.rolloutPane1 = new Viatronix.v3D.UI.RolloutPane();
      this.comboBox1 = new Viatronix.v3D.UI.ComboBox();
      this.slider1 = new Viatronix.v3D.UI.Slider();
      this.rebar1 = new Viatronix.v3D.UI.Rebar();
      this.m_rollout.SuspendLayout();
      this.rolloutTab4.SuspendLayout();
      this.rolloutTab3.SuspendLayout();
      this.groupBox2.SuspendLayout();
      this.groupBox1.SuspendLayout();
      this.rolloutTab2.SuspendLayout();
      this.rolloutTab1.SuspendLayout();
      this.rolloutTab5.SuspendLayout();
      this.rolloutPane2.SuspendLayout();
      this.rolloutPane1.SuspendLayout();
      this.SuspendLayout();
      // 
      // m_rollout
      // 
      this.m_rollout.CaptionFont = new System.Drawing.Font("Arial", 10F);
      this.m_rollout.Controls.Add(this.rolloutTab4);
      this.m_rollout.Controls.Add(this.rolloutTab3);
      this.m_rollout.Controls.Add(this.rolloutTab2);
      this.m_rollout.Controls.Add(this.rolloutTab1);
      this.m_rollout.Controls.Add(this.rolloutTab5);
      this.m_rollout.Controls.Add(this.rolloutPane2);
      this.m_rollout.Controls.Add(this.rolloutPane1);
      this.m_rollout.ForeColor = System.Drawing.Color.White;
      this.m_rollout.GradientLeft = System.Drawing.Color.FromArgb(((System.Byte)(91)), ((System.Byte)(61)), ((System.Byte)(204)));
      this.m_rollout.GradientRight = System.Drawing.Color.FromArgb(((System.Byte)(162)), ((System.Byte)(174)), ((System.Byte)(246)));
      this.m_rollout.Location = new System.Drawing.Point(5, 63);
      this.m_rollout.Name = "m_rollout";
      this.m_rollout.Size = new System.Drawing.Size(303, 496);
      this.m_rollout.TabIndex = 0;
      this.m_rollout.Text = "m_rollout";
      // 
      // rolloutTab4
      // 
      this.rolloutTab4.Controls.Add(this.listBox1);
      this.rolloutTab4.Expanded = false;
      this.rolloutTab4.GradientLeft = System.Drawing.Color.FromArgb(((System.Byte)(91)), ((System.Byte)(61)), ((System.Byte)(204)));
      this.rolloutTab4.GradientRight = System.Drawing.Color.FromArgb(((System.Byte)(162)), ((System.Byte)(174)), ((System.Byte)(246)));
      this.rolloutTab4.MaximizedHeight = 200;
      this.rolloutTab4.Text = "List Box";
      // 
      // listBox1
      // 
      this.listBox1.AllowSelection = true;
      this.listBox1.BackColor = System.Drawing.Color.Transparent;
      this.listBox1.Bevel = 10;
      this.listBox1.FillColor = System.Drawing.Color.Black;
      this.listBox1.Location = new System.Drawing.Point(16, 29);
      this.listBox1.Name = "listBox1";
      this.listBox1.Size = new System.Drawing.Size(150, 164);
      this.listBox1.Spacing = 2;
      this.listBox1.TabIndex = 13;
      this.listBox1.ViewRectangle = new System.Drawing.Rectangle(0, 0, 133, 164);
      // 
      // rolloutTab3
      // 
      this.rolloutTab3.Controls.Add(this.m_mbMessage);
      this.rolloutTab3.Controls.Add(this.label7);
      this.rolloutTab3.Controls.Add(this.m_mbCaption);
      this.rolloutTab3.Controls.Add(this.label6);
      this.rolloutTab3.Controls.Add(this.m_showWinformsMessageBox);
      this.rolloutTab3.Controls.Add(this.groupBox2);
      this.rolloutTab3.Controls.Add(this.groupBox1);
      this.rolloutTab3.Controls.Add(this.m_showMessageBox);
      this.rolloutTab3.Expanded = false;
      this.rolloutTab3.GradientLeft = System.Drawing.Color.FromArgb(((System.Byte)(91)), ((System.Byte)(61)), ((System.Byte)(204)));
      this.rolloutTab3.GradientRight = System.Drawing.Color.FromArgb(((System.Byte)(162)), ((System.Byte)(174)), ((System.Byte)(246)));
      this.rolloutTab3.MaximizedHeight = 310;
      this.rolloutTab3.Text = "Message Box";
      // 
      // m_mbMessage
      // 
      this.m_mbMessage.BackColor = System.Drawing.Color.FromArgb(((System.Byte)(240)), ((System.Byte)(240)), ((System.Byte)(240)));
      this.m_mbMessage.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
      this.m_mbMessage.Location = new System.Drawing.Point(6, 241);
      this.m_mbMessage.Multiline = true;
      this.m_mbMessage.Name = "m_mbMessage";
      this.m_mbMessage.Size = new System.Drawing.Size(202, 62);
      this.m_mbMessage.TabIndex = 15;
      this.m_mbMessage.Text = "Sample Message Text";
      // 
      // label7
      // 
      this.label7.Location = new System.Drawing.Point(6, 224);
      this.label7.Name = "label7";
      this.label7.Size = new System.Drawing.Size(56, 13);
      this.label7.TabIndex = 14;
      this.label7.Text = "Message";
      // 
      // m_mbCaption
      // 
      this.m_mbCaption.BackColor = System.Drawing.Color.FromArgb(((System.Byte)(240)), ((System.Byte)(240)), ((System.Byte)(240)));
      this.m_mbCaption.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
      this.m_mbCaption.Location = new System.Drawing.Point(6, 203);
      this.m_mbCaption.Name = "m_mbCaption";
      this.m_mbCaption.Size = new System.Drawing.Size(202, 20);
      this.m_mbCaption.TabIndex = 13;
      this.m_mbCaption.Text = "";
      // 
      // label6
      // 
      this.label6.Location = new System.Drawing.Point(6, 187);
      this.label6.Name = "label6";
      this.label6.Size = new System.Drawing.Size(45, 13);
      this.label6.TabIndex = 13;
      this.label6.Text = "Caption";
      // 
      // m_showWinformsMessageBox
      // 
      this.m_showWinformsMessageBox.CustomImages = new System.Drawing.Bitmap[] {
                                                                                 null,
                                                                                 null,
                                                                                 null};
      this.m_showWinformsMessageBox.ForeColor = System.Drawing.Color.Black;
      this.m_showWinformsMessageBox.Location = new System.Drawing.Point(221, 243);
      this.m_showWinformsMessageBox.Name = "m_showWinformsMessageBox";
      this.m_showWinformsMessageBox.Size = new System.Drawing.Size(65, 37);
      this.m_showWinformsMessageBox.TabIndex = 13;
      this.m_showWinformsMessageBox.Text = "Show Winforms";
      this.m_showWinformsMessageBox.Click += new System.EventHandler(this.m_showMessageBox_Click);
      // 
      // groupBox2
      // 
      this.groupBox2.BorderColor = System.Drawing.Color.FromArgb(((System.Byte)(89)), ((System.Byte)(116)), ((System.Byte)(153)));
      this.groupBox2.Controls.Add(this.m_mbButtonAbortRetryIgnore);
      this.groupBox2.Controls.Add(this.m_mbButtonRetryCancel);
      this.groupBox2.Controls.Add(this.m_mbButtonYesNoCancel);
      this.groupBox2.Controls.Add(this.m_mbButtonYesNo);
      this.groupBox2.Controls.Add(this.m_mbButtonOkCancel);
      this.groupBox2.Controls.Add(this.m_mbButtonOk);
      this.groupBox2.FillColor = System.Drawing.Color.FromArgb(((System.Byte)(240)), ((System.Byte)(240)), ((System.Byte)(240)));
      this.groupBox2.Location = new System.Drawing.Point(152, 30);
      this.groupBox2.Name = "groupBox2";
      this.groupBox2.Size = new System.Drawing.Size(140, 151);
      this.groupBox2.TabColor = System.Drawing.Color.FromArgb(((System.Byte)(128)), ((System.Byte)(143)), ((System.Byte)(180)));
      this.groupBox2.TabIndex = 13;
      this.groupBox2.TabStop = false;
      this.groupBox2.Text = "Buttons";
      // 
      // m_mbButtonAbortRetryIgnore
      // 
      this.m_mbButtonAbortRetryIgnore.BackColor = System.Drawing.Color.FromArgb(((System.Byte)(240)), ((System.Byte)(240)), ((System.Byte)(240)));
      this.m_mbButtonAbortRetryIgnore.ForeColor = System.Drawing.Color.Black;
      this.m_mbButtonAbortRetryIgnore.Location = new System.Drawing.Point(12, 123);
      this.m_mbButtonAbortRetryIgnore.Name = "m_mbButtonAbortRetryIgnore";
      this.m_mbButtonAbortRetryIgnore.Size = new System.Drawing.Size(118, 18);
      this.m_mbButtonAbortRetryIgnore.TabIndex = 23;
      this.m_mbButtonAbortRetryIgnore.Text = "Abort Retry Ingore";
      // 
      // m_mbButtonRetryCancel
      // 
      this.m_mbButtonRetryCancel.BackColor = System.Drawing.Color.FromArgb(((System.Byte)(240)), ((System.Byte)(240)), ((System.Byte)(240)));
      this.m_mbButtonRetryCancel.ForeColor = System.Drawing.Color.Black;
      this.m_mbButtonRetryCancel.Location = new System.Drawing.Point(12, 104);
      this.m_mbButtonRetryCancel.Name = "m_mbButtonRetryCancel";
      this.m_mbButtonRetryCancel.Size = new System.Drawing.Size(96, 18);
      this.m_mbButtonRetryCancel.TabIndex = 22;
      this.m_mbButtonRetryCancel.Text = "Retry Cancel";
      // 
      // m_mbButtonYesNoCancel
      // 
      this.m_mbButtonYesNoCancel.BackColor = System.Drawing.Color.FromArgb(((System.Byte)(240)), ((System.Byte)(240)), ((System.Byte)(240)));
      this.m_mbButtonYesNoCancel.ForeColor = System.Drawing.Color.Black;
      this.m_mbButtonYesNoCancel.Location = new System.Drawing.Point(12, 85);
      this.m_mbButtonYesNoCancel.Name = "m_mbButtonYesNoCancel";
      this.m_mbButtonYesNoCancel.Size = new System.Drawing.Size(104, 18);
      this.m_mbButtonYesNoCancel.TabIndex = 21;
      this.m_mbButtonYesNoCancel.Text = "Yes No Cancel";
      // 
      // m_mbButtonYesNo
      // 
      this.m_mbButtonYesNo.BackColor = System.Drawing.Color.FromArgb(((System.Byte)(240)), ((System.Byte)(240)), ((System.Byte)(240)));
      this.m_mbButtonYesNo.ForeColor = System.Drawing.Color.Black;
      this.m_mbButtonYesNo.Location = new System.Drawing.Point(12, 66);
      this.m_mbButtonYesNo.Name = "m_mbButtonYesNo";
      this.m_mbButtonYesNo.Size = new System.Drawing.Size(84, 18);
      this.m_mbButtonYesNo.TabIndex = 20;
      this.m_mbButtonYesNo.Text = "Yes No";
      // 
      // m_mbButtonOkCancel
      // 
      this.m_mbButtonOkCancel.BackColor = System.Drawing.Color.FromArgb(((System.Byte)(240)), ((System.Byte)(240)), ((System.Byte)(240)));
      this.m_mbButtonOkCancel.ForeColor = System.Drawing.Color.Black;
      this.m_mbButtonOkCancel.Location = new System.Drawing.Point(12, 49);
      this.m_mbButtonOkCancel.Name = "m_mbButtonOkCancel";
      this.m_mbButtonOkCancel.Size = new System.Drawing.Size(84, 16);
      this.m_mbButtonOkCancel.TabIndex = 19;
      this.m_mbButtonOkCancel.Text = "Ok Cancel";
      // 
      // m_mbButtonOk
      // 
      this.m_mbButtonOk.BackColor = System.Drawing.Color.FromArgb(((System.Byte)(240)), ((System.Byte)(240)), ((System.Byte)(240)));
      this.m_mbButtonOk.Checked = true;
      this.m_mbButtonOk.ForeColor = System.Drawing.Color.Black;
      this.m_mbButtonOk.Location = new System.Drawing.Point(12, 30);
      this.m_mbButtonOk.Name = "m_mbButtonOk";
      this.m_mbButtonOk.Size = new System.Drawing.Size(84, 16);
      this.m_mbButtonOk.TabIndex = 18;
      this.m_mbButtonOk.TabStop = true;
      this.m_mbButtonOk.Text = "Ok";
      // 
      // groupBox1
      // 
      this.groupBox1.BorderColor = System.Drawing.Color.FromArgb(((System.Byte)(89)), ((System.Byte)(116)), ((System.Byte)(153)));
      this.groupBox1.Controls.Add(this.m_mbIconQuestion);
      this.groupBox1.Controls.Add(this.m_mbIconError);
      this.groupBox1.Controls.Add(this.m_mbIconWarning);
      this.groupBox1.Controls.Add(this.m_mbIconInfo);
      this.groupBox1.Controls.Add(this.m_mbIconNone);
      this.groupBox1.FillColor = System.Drawing.Color.FromArgb(((System.Byte)(240)), ((System.Byte)(240)), ((System.Byte)(240)));
      this.groupBox1.Location = new System.Drawing.Point(5, 30);
      this.groupBox1.Name = "groupBox1";
      this.groupBox1.Size = new System.Drawing.Size(140, 151);
      this.groupBox1.TabColor = System.Drawing.Color.FromArgb(((System.Byte)(128)), ((System.Byte)(143)), ((System.Byte)(180)));
      this.groupBox1.TabIndex = 13;
      this.groupBox1.TabStop = false;
      this.groupBox1.Text = "Icon";
      // 
      // m_mbIconQuestion
      // 
      this.m_mbIconQuestion.BackColor = System.Drawing.Color.FromArgb(((System.Byte)(240)), ((System.Byte)(240)), ((System.Byte)(240)));
      this.m_mbIconQuestion.ForeColor = System.Drawing.Color.Black;
      this.m_mbIconQuestion.Location = new System.Drawing.Point(13, 104);
      this.m_mbIconQuestion.Name = "m_mbIconQuestion";
      this.m_mbIconQuestion.Size = new System.Drawing.Size(84, 18);
      this.m_mbIconQuestion.TabIndex = 17;
      this.m_mbIconQuestion.Text = "Question";
      // 
      // m_mbIconError
      // 
      this.m_mbIconError.BackColor = System.Drawing.Color.FromArgb(((System.Byte)(240)), ((System.Byte)(240)), ((System.Byte)(240)));
      this.m_mbIconError.ForeColor = System.Drawing.Color.Black;
      this.m_mbIconError.Location = new System.Drawing.Point(13, 85);
      this.m_mbIconError.Name = "m_mbIconError";
      this.m_mbIconError.Size = new System.Drawing.Size(84, 18);
      this.m_mbIconError.TabIndex = 16;
      this.m_mbIconError.Text = "Error";
      // 
      // m_mbIconWarning
      // 
      this.m_mbIconWarning.BackColor = System.Drawing.Color.FromArgb(((System.Byte)(240)), ((System.Byte)(240)), ((System.Byte)(240)));
      this.m_mbIconWarning.ForeColor = System.Drawing.Color.Black;
      this.m_mbIconWarning.Location = new System.Drawing.Point(13, 66);
      this.m_mbIconWarning.Name = "m_mbIconWarning";
      this.m_mbIconWarning.Size = new System.Drawing.Size(84, 18);
      this.m_mbIconWarning.TabIndex = 15;
      this.m_mbIconWarning.Text = "Warning";
      // 
      // m_mbIconInfo
      // 
      this.m_mbIconInfo.BackColor = System.Drawing.Color.FromArgb(((System.Byte)(240)), ((System.Byte)(240)), ((System.Byte)(240)));
      this.m_mbIconInfo.ForeColor = System.Drawing.Color.Black;
      this.m_mbIconInfo.Location = new System.Drawing.Point(13, 49);
      this.m_mbIconInfo.Name = "m_mbIconInfo";
      this.m_mbIconInfo.Size = new System.Drawing.Size(84, 16);
      this.m_mbIconInfo.TabIndex = 14;
      this.m_mbIconInfo.Text = "Information";
      // 
      // m_mbIconNone
      // 
      this.m_mbIconNone.BackColor = System.Drawing.Color.FromArgb(((System.Byte)(240)), ((System.Byte)(240)), ((System.Byte)(240)));
      this.m_mbIconNone.Checked = true;
      this.m_mbIconNone.ForeColor = System.Drawing.Color.Black;
      this.m_mbIconNone.Location = new System.Drawing.Point(13, 30);
      this.m_mbIconNone.Name = "m_mbIconNone";
      this.m_mbIconNone.Size = new System.Drawing.Size(84, 16);
      this.m_mbIconNone.TabIndex = 13;
      this.m_mbIconNone.TabStop = true;
      this.m_mbIconNone.Text = "None";
      // 
      // m_showMessageBox
      // 
      this.m_showMessageBox.CustomImages = new System.Drawing.Bitmap[] {
                                                                         null,
                                                                         null,
                                                                         null};
      this.m_showMessageBox.ForeColor = System.Drawing.Color.Black;
      this.m_showMessageBox.Location = new System.Drawing.Point(221, 203);
      this.m_showMessageBox.Name = "m_showMessageBox";
      this.m_showMessageBox.Size = new System.Drawing.Size(65, 37);
      this.m_showMessageBox.TabIndex = 13;
      this.m_showMessageBox.Text = "Show";
      this.m_showMessageBox.Click += new System.EventHandler(this.m_showMessageBox_Click);
      // 
      // rolloutTab2
      // 
      this.rolloutTab2.Controls.Add(this.m_popupMenuLabel);
      this.rolloutTab2.Expanded = false;
      this.rolloutTab2.GradientLeft = System.Drawing.Color.FromArgb(((System.Byte)(91)), ((System.Byte)(61)), ((System.Byte)(204)));
      this.rolloutTab2.GradientRight = System.Drawing.Color.FromArgb(((System.Byte)(162)), ((System.Byte)(174)), ((System.Byte)(246)));
      this.rolloutTab2.MaximizedHeight = 100;
      this.rolloutTab2.Text = "Popup Menu";
      // 
      // m_popupMenuLabel
      // 
      this.m_popupMenuLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_popupMenuLabel.ForeColor = System.Drawing.Color.Red;
      this.m_popupMenuLabel.Location = new System.Drawing.Point(6, 34);
      this.m_popupMenuLabel.Name = "m_popupMenuLabel";
      this.m_popupMenuLabel.Size = new System.Drawing.Size(292, 61);
      this.m_popupMenuLabel.TabIndex = 13;
      this.m_popupMenuLabel.Text = "Right Click Me!";
      this.m_popupMenuLabel.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
      this.m_popupMenuLabel.MouseUp += new System.Windows.Forms.MouseEventHandler(this.m_popupMenuLabel_MouseUp);
      // 
      // rolloutTab1
      // 
      this.rolloutTab1.Controls.Add(this.label5);
      this.rolloutTab1.Controls.Add(this.m_progressBorderColor);
      this.rolloutTab1.Controls.Add(this.label4);
      this.rolloutTab1.Controls.Add(this.m_progressTextHighlightColor);
      this.rolloutTab1.Controls.Add(this.label3);
      this.rolloutTab1.Controls.Add(this.m_progressTextColor);
      this.rolloutTab1.Controls.Add(this.label2);
      this.rolloutTab1.Controls.Add(this.m_progressBackColor);
      this.rolloutTab1.Controls.Add(this.label1);
      this.rolloutTab1.Controls.Add(this.m_progressFillColor);
      this.rolloutTab1.Controls.Add(this.m_progressDisplayText);
      this.rolloutTab1.Controls.Add(this.m_progressAnimate);
      this.rolloutTab1.Controls.Add(this.m_progressBar);
      this.rolloutTab1.Expanded = false;
      this.rolloutTab1.GradientLeft = System.Drawing.Color.FromArgb(((System.Byte)(91)), ((System.Byte)(61)), ((System.Byte)(204)));
      this.rolloutTab1.GradientRight = System.Drawing.Color.FromArgb(((System.Byte)(162)), ((System.Byte)(174)), ((System.Byte)(246)));
      this.rolloutTab1.MaximizedHeight = 200;
      this.rolloutTab1.Text = "ProgressBar";
      // 
      // label5
      // 
      this.label5.Location = new System.Drawing.Point(9, 123);
      this.label5.Name = "label5";
      this.label5.Size = new System.Drawing.Size(110, 17);
      this.label5.TabIndex = 12;
      this.label5.Text = "Border Color";
      // 
      // m_progressBorderColor
      // 
      this.m_progressBorderColor.Color = System.Drawing.Color.Black;
      this.m_progressBorderColor.Location = new System.Drawing.Point(127, 123);
      this.m_progressBorderColor.Name = "m_progressBorderColor";
      this.m_progressBorderColor.Size = new System.Drawing.Size(63, 23);
      this.m_progressBorderColor.TabIndex = 11;
      this.m_progressBorderColor.Text = "colorAdjustor4";
      this.m_progressBorderColor.Changed += new System.EventHandler(this.OnProgressColorChangedHandler);
      // 
      // label4
      // 
      this.label4.Location = new System.Drawing.Point(9, 169);
      this.label4.Name = "label4";
      this.label4.Size = new System.Drawing.Size(110, 17);
      this.label4.TabIndex = 10;
      this.label4.Text = "Text Highlight Color";
      // 
      // m_progressTextHighlightColor
      // 
      this.m_progressTextHighlightColor.Color = System.Drawing.Color.White;
      this.m_progressTextHighlightColor.Location = new System.Drawing.Point(127, 170);
      this.m_progressTextHighlightColor.Name = "m_progressTextHighlightColor";
      this.m_progressTextHighlightColor.Size = new System.Drawing.Size(63, 23);
      this.m_progressTextHighlightColor.TabIndex = 9;
      this.m_progressTextHighlightColor.Text = "colorAdjustor3";
      this.m_progressTextHighlightColor.Changed += new System.EventHandler(this.OnProgressColorChangedHandler);
      // 
      // label3
      // 
      this.label3.Location = new System.Drawing.Point(9, 146);
      this.label3.Name = "label3";
      this.label3.Size = new System.Drawing.Size(58, 17);
      this.label3.TabIndex = 8;
      this.label3.Text = "Text Color";
      // 
      // m_progressTextColor
      // 
      this.m_progressTextColor.Color = System.Drawing.Color.Black;
      this.m_progressTextColor.Location = new System.Drawing.Point(127, 146);
      this.m_progressTextColor.Name = "m_progressTextColor";
      this.m_progressTextColor.Size = new System.Drawing.Size(63, 23);
      this.m_progressTextColor.TabIndex = 7;
      this.m_progressTextColor.Text = "colorAdjustor1";
      this.m_progressTextColor.Changed += new System.EventHandler(this.OnProgressColorChangedHandler);
      // 
      // label2
      // 
      this.label2.Location = new System.Drawing.Point(9, 100);
      this.label2.Name = "label2";
      this.label2.Size = new System.Drawing.Size(110, 17);
      this.label2.TabIndex = 6;
      this.label2.Text = "Background Color";
      // 
      // m_progressBackColor
      // 
      this.m_progressBackColor.Color = System.Drawing.Color.FromArgb(((System.Byte)(162)), ((System.Byte)(174)), ((System.Byte)(246)));
      this.m_progressBackColor.Location = new System.Drawing.Point(127, 101);
      this.m_progressBackColor.Name = "m_progressBackColor";
      this.m_progressBackColor.Size = new System.Drawing.Size(63, 23);
      this.m_progressBackColor.TabIndex = 5;
      this.m_progressBackColor.Text = "colorAdjustor1";
      this.m_progressBackColor.Changed += new System.EventHandler(this.OnProgressColorChangedHandler);
      // 
      // label1
      // 
      this.label1.Location = new System.Drawing.Point(9, 76);
      this.label1.Name = "label1";
      this.label1.Size = new System.Drawing.Size(58, 17);
      this.label1.TabIndex = 4;
      this.label1.Text = "Fill Color";
      // 
      // m_progressFillColor
      // 
      this.m_progressFillColor.Color = System.Drawing.Color.FromArgb(((System.Byte)(91)), ((System.Byte)(61)), ((System.Byte)(204)));
      this.m_progressFillColor.Location = new System.Drawing.Point(127, 76);
      this.m_progressFillColor.Name = "m_progressFillColor";
      this.m_progressFillColor.Size = new System.Drawing.Size(63, 23);
      this.m_progressFillColor.TabIndex = 3;
      this.m_progressFillColor.Text = "colorAdjustor1";
      this.m_progressFillColor.Changed += new System.EventHandler(this.OnProgressColorChangedHandler);
      // 
      // m_progressDisplayText
      // 
      this.m_progressDisplayText.CustomImages = new System.Drawing.Bitmap[] {
                                                                              null,
                                                                              null,
                                                                              null};
      this.m_progressDisplayText.Location = new System.Drawing.Point(202, 97);
      this.m_progressDisplayText.Name = "m_progressDisplayText";
      this.m_progressDisplayText.Size = new System.Drawing.Size(86, 16);
      this.m_progressDisplayText.TabIndex = 2;
      this.m_progressDisplayText.Text = "Draw Text";
      this.m_progressDisplayText.CheckedChanged += new System.EventHandler(this.m_progressDisplayText_CheckedChanged);
      // 
      // m_progressAnimate
      // 
      this.m_progressAnimate.CustomImages = new System.Drawing.Bitmap[] {
                                                                          null,
                                                                          null,
                                                                          null};
      this.m_progressAnimate.Location = new System.Drawing.Point(202, 74);
      this.m_progressAnimate.Name = "m_progressAnimate";
      this.m_progressAnimate.Size = new System.Drawing.Size(70, 18);
      this.m_progressAnimate.TabIndex = 1;
      this.m_progressAnimate.Text = "Animate";
      this.m_progressAnimate.CheckedChanged += new System.EventHandler(this.m_progressAnimate_CheckedChanged);
      // 
      // m_progressBar
      // 
      this.m_progressBar.AnimationInterval = 100;
      this.m_progressBar.BackColor = System.Drawing.Color.FromArgb(((System.Byte)(162)), ((System.Byte)(174)), ((System.Byte)(246)));
      this.m_progressBar.BorderColor = System.Drawing.Color.Black;
      this.m_progressBar.DrawText = false;
      this.m_progressBar.ForeColor = System.Drawing.Color.FromArgb(((System.Byte)(91)), ((System.Byte)(61)), ((System.Byte)(204)));
      this.m_progressBar.Location = new System.Drawing.Point(6, 42);
      this.m_progressBar.Maximum = 100;
      this.m_progressBar.Minimum = 0;
      this.m_progressBar.Name = "m_progressBar";
      this.m_progressBar.Size = new System.Drawing.Size(286, 25);
      this.m_progressBar.Step = 1;
      this.m_progressBar.TabIndex = 0;
      this.m_progressBar.Text = "Sample Text";
      this.m_progressBar.TextColor = System.Drawing.Color.Black;
      this.m_progressBar.TextHilightColor = System.Drawing.Color.White;
      this.m_progressBar.Value = 50;
      this.m_progressBar.Wrap = true;
      // 
      // rolloutTab5
      // 
      this.rolloutTab5.Controls.Add(this.rangeSlider1);
      this.rolloutTab5.Expanded = false;
      this.rolloutTab5.GradientLeft = System.Drawing.Color.FromArgb(((System.Byte)(91)), ((System.Byte)(61)), ((System.Byte)(204)));
      this.rolloutTab5.GradientRight = System.Drawing.Color.FromArgb(((System.Byte)(162)), ((System.Byte)(174)), ((System.Byte)(246)));
      this.rolloutTab5.MaximizedHeight = 200;
      this.rolloutTab5.Text = "Range Slider";
      // 
      // rangeSlider1
      // 
      this.rangeSlider1.BackColor = System.Drawing.Color.Transparent;
      this.rangeSlider1.Color = System.Drawing.Color.FromArgb(((System.Byte)(91)), ((System.Byte)(61)), ((System.Byte)(204)));
      this.rangeSlider1.FillColor = System.Drawing.Color.FromArgb(((System.Byte)(91)), ((System.Byte)(61)), ((System.Byte)(204)));
      this.rangeSlider1.Location = new System.Drawing.Point(10, 40);
      this.rangeSlider1.Max = 75;
      this.rangeSlider1.Min = 25;
      this.rangeSlider1.Name = "rangeSlider1";
      this.rangeSlider1.Size = new System.Drawing.Size(271, 35);
      this.rangeSlider1.SliderMax = 100;
      this.rangeSlider1.SliderMin = 0;
      this.rangeSlider1.TabIndex = 13;
      this.rangeSlider1.Text = "m_rangeSlider";
      // 
      // rolloutPane2
      // 
      this.rolloutPane2.Controls.Add(this.vScrollBar1);
      this.rolloutPane2.Expanded = false;
      this.rolloutPane2.GradientLeft = System.Drawing.Color.FromArgb(((System.Byte)(91)), ((System.Byte)(61)), ((System.Byte)(204)));
      this.rolloutPane2.GradientRight = System.Drawing.Color.FromArgb(((System.Byte)(162)), ((System.Byte)(174)), ((System.Byte)(246)));
      this.rolloutPane2.MaximizedHeight = 200;
      this.rolloutPane2.Text = "Scrollbars";
      // 
      // vScrollBar1
      // 
      this.vScrollBar1.LargeChange = 10;
      this.vScrollBar1.Location = new System.Drawing.Point(17, 42);
      this.vScrollBar1.Maximum = 100;
      this.vScrollBar1.Minimum = 0;
      this.vScrollBar1.Name = "vScrollBar1";
      this.vScrollBar1.Size = new System.Drawing.Size(27, 127);
      this.vScrollBar1.SmallChange = 1;
      this.vScrollBar1.TabIndex = 13;
      this.vScrollBar1.Text = "vScrollBar1";
      this.vScrollBar1.Value = 0;
      // 
      // rolloutPane1
      // 
      this.rolloutPane1.Controls.Add(this.comboBox1);
      this.rolloutPane1.Controls.Add(this.slider1);
      this.rolloutPane1.Expanded = true;
      this.rolloutPane1.GradientLeft = System.Drawing.Color.FromArgb(((System.Byte)(91)), ((System.Byte)(61)), ((System.Byte)(204)));
      this.rolloutPane1.GradientRight = System.Drawing.Color.FromArgb(((System.Byte)(162)), ((System.Byte)(174)), ((System.Byte)(246)));
      this.rolloutPane1.MaximizedHeight = 150;
      this.rolloutPane1.Text = "Slider";
      // 
      // comboBox1
      // 
      this.comboBox1.DisplayMember = "String";
      this.comboBox1.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
      this.comboBox1.ItemHeight = 13;
      this.comboBox1.Location = new System.Drawing.Point(136, 120);
      this.comboBox1.MaxDropDownItems = 50;
      this.comboBox1.Name = "comboBox1";
      this.comboBox1.Size = new System.Drawing.Size(136, 21);
      this.comboBox1.TabIndex = 14;
      this.comboBox1.ValueMember = "Object";
      // 
      // slider1
      // 
      this.slider1.EventsWhildAdjusting = false;
      this.slider1.Location = new System.Drawing.Point(15, 49);
      this.slider1.Max = 100;
      this.slider1.Min = 0;
      this.slider1.Name = "slider1";
      this.slider1.ShowText = false;
      this.slider1.Size = new System.Drawing.Size(262, 36);
      this.slider1.SliderColor = System.Drawing.Color.FromArgb(((System.Byte)(89)), ((System.Byte)(116)), ((System.Byte)(153)));
      this.slider1.SliderType = Viatronix.v3D.UI.SliderType.Wedge;
      this.slider1.Suffix = "";
      this.slider1.TabIndex = 14;
      this.slider1.Text = "slider1";
      this.slider1.Value = 50;
      // 
      // rebar1
      // 
      this.rebar1.BackColor = System.Drawing.Color.FromArgb(((System.Byte)(128)), ((System.Byte)(143)), ((System.Byte)(180)));
      this.rebar1.Location = new System.Drawing.Point(0, 0);
      this.rebar1.Name = "rebar1";
      this.rebar1.ShowBackgroundImage = false;
      this.rebar1.Size = new System.Drawing.Size(314, 4);
      this.rebar1.TabIndex = 14;
      this.rebar1.Text = "rebar1";
      // 
      // MainForm
      // 
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.ClientSize = new System.Drawing.Size(314, 569);
      this.Controls.Add(this.rebar1);
      this.Controls.Add(this.m_rollout);
      this.Name = "MainForm";
      this.Text = "Viatronix UI Class Library";
      this.m_rollout.ResumeLayout(false);
      this.rolloutTab4.ResumeLayout(false);
      this.rolloutTab3.ResumeLayout(false);
      this.groupBox2.ResumeLayout(false);
      this.groupBox1.ResumeLayout(false);
      this.rolloutTab2.ResumeLayout(false);
      this.rolloutTab1.ResumeLayout(false);
      this.rolloutTab5.ResumeLayout(false);
      this.rolloutPane2.ResumeLayout(false);
      this.rolloutPane1.ResumeLayout(false);
      this.ResumeLayout(false);

    }
		#endregion

    #region entry point
		
    [STAThread]
    static void Main() 
    {
      System.Windows.Forms.Application.Run(new MainForm());
    }
   

    #endregion

    #region ProgressBar
    
    /// <summary>
    /// Event handler for the CheckedChanged event for m_progressAnimate.
    /// </summary>
    /// <param name="sender">Sender of the event.</param>
    /// <param name="args">EventArgs containing data related to this event.</param>
    private void m_progressAnimate_CheckedChanged( object sender, System.EventArgs args )
    {
      // toggle the animation handler
      if( m_progressAnimate.Checked )
        m_progressBar.StartAnimation();
      else
        m_progressBar.StopAnimation();
    } // m_progressAnimate_CheckedChanged( sender, args )

    
    /// <summary>
    /// Event handler for the CheckedChanged event for m_progressDisplayText.
    /// </summary>
    /// <param name="sender">Sender of the event.</param>
    /// <param name="args">EventArgs containing data related to this event.</param>
    private void m_progressDisplayText_CheckedChanged( object sender, System.EventArgs args )
    {
      // toggle display text property
      m_progressBar.DrawText = m_progressDisplayText.Checked;
    } // m_progressDisplayText_CheckedChanged( sender, args )

    
    /// <summary>
    /// Event handler for the Changed event for the color adjustors.
    /// </summary>
    /// <param name="sender">Sender of the event.</param>
    /// <param name="args">EventArgs containing data related to this event.</param>
    private void OnProgressColorChangedHandler( object sender, EventArgs args )
    {
      if( sender == m_progressBackColor )
        m_progressBar.BackColor = m_progressBackColor.Color;
      
      else if( sender == m_progressFillColor )
        m_progressBar.ForeColor = m_progressFillColor.Color;
     
      else if( sender == m_progressBorderColor )
        m_progressBar.BorderColor = m_progressBorderColor.Color;
      
      else if( sender == m_progressTextColor )
        m_progressBar.TextColor = m_progressTextColor.Color;
      
      else if( sender == m_progressTextHighlightColor )
        m_progressBar.TextHilightColor = m_progressTextHighlightColor.Color;
    } // OnProgressColorChangedHandler( sender, args )


    #endregion

    #region PopupMenu

    /// <summary>
    /// Event handler for the click event for m_popupMenuLabel.
    /// </summary>
    /// <param name="sender">Sender of event.</param>
    /// <param name="args">MouseEventArgs containing data related to this event.</param>
    private void m_popupMenuLabel_MouseUp( object sender, System.Windows.Forms.MouseEventArgs args )
    {
      // only handle the right mouse button.
      if( args.Button != MouseButtons.Right )
        return;

      // create the popup menu
      PopupMenu popup = new PopupMenu();

      // add the popup menu items
      popup.Items.Add( "&Red" );    // 0
      popup.Items.Add( "W&hite" );  // 1
      popup.Items.Add( "Blue&" );   // 2
      popup.Items.Add( "-" );       // 3

      // create an event handler for the sub menu items
      EventHandler handler = new EventHandler( this.OnMenuHandler );

      // create a sub menu passing them the click handler
      PopupItem subMenu = new PopupItem( "Sub Menu" );
      subMenu.Items.Add( new PopupItem( "Red && Green", "<ctrl>-R", handler ) );
      subMenu.Items.Add( new PopupItem( "White", "<ctrl>-W", handler ) );
      subMenu.Items.Add( new PopupItem( "Blue", "<ctrl>-B", handler ) );

      // add the sub menu to the popup menu
      popup.Items.Add( subMenu );   // 4

      // create a disabled menu item, and a disabled popup menu
      popup.Items.Add( "&Mauve" );  // 5
      popup.Items[ 5 ].Enabled = false;

      PopupItem disSubMenu = new PopupItem ( "&Off-Colors" );
      disSubMenu.Items.Add( "Brick" );
      disSubMenu.Items.Add( "Aqua" );
      disSubMenu.Enabled = false;
      popup.Items.Add( disSubMenu );

      // update the check state
      popup.Items[ 0 ].Checked = m_popupMenuLabel.ForeColor == Color.Red;
      popup.Items[ 1 ].Checked = m_popupMenuLabel.ForeColor == Color.White;
      popup.Items[ 2 ].Checked = m_popupMenuLabel.ForeColor == Color.Blue;
      subMenu.Items[ 0 ].Checked = m_popupMenuLabel.ForeColor == Color.Red;
      subMenu.Items[ 1 ].Checked = m_popupMenuLabel.ForeColor == Color.White;
      subMenu.Items[ 2 ].Checked = m_popupMenuLabel.ForeColor == Color.Blue;
      
      // show the popup menu and handle the return value
      switch( popup.Show( this, Control.MousePosition ) )
      {
        case 0 : m_popupMenuLabel.ForeColor = Color.Red;    break;
        case 1 : m_popupMenuLabel.ForeColor = Color.White;  break;
        case 2 : m_popupMenuLabel.ForeColor = Color.Blue;   break;
        default: break;
      } // switch( popup.Show( this, Control.MousePosition ) )
    } // m_popupMenuLabel_MouseUp

    
    /// <summary>
    /// Event handler for the Click event for a menu item.
    /// </summary>
    /// <param name="sender">Sender of this event.</param>
    /// <param name="args">EventArgs containing data related to this event.</param>
    private void OnMenuHandler( object sender, EventArgs args )
    {
      // cast the sender to the menu item that raised the click event
      PopupItem item = ( PopupItem ) sender;

      switch( item.Index )
      {
        case 0 : m_popupMenuLabel.ForeColor = Color.Red;    break;
        case 1 : m_popupMenuLabel.ForeColor = Color.White;  break;
        case 2 : m_popupMenuLabel.ForeColor = Color.Blue;   break;
        default: break;
      }
    } // OnMenuHandler()

    
    #endregion

    #region MessageBox

    /// <summary>
    /// 
    /// </summary>
    /// <param name="sender">Sender of event.</param>
    /// <param name="e">EventArgs containing data related to this event.</param>
    private void m_showMessageBox_Click( object sender, System.EventArgs args )
    {     
      // fetch the values from the gui
      MessageBoxIcon icon = GetMessageBoxIcon();
      MessageBoxButtons buttons = GetMessageBoxButtons();
      string caption = m_mbCaption.Text;
      string message = m_mbMessage.Text;
      
      if( sender == m_showWinformsMessageBox )
        System.Windows.Forms.MessageBox.Show( message, caption,  buttons, icon );
      else
        Viatronix.v3D.UI.MessageBox.Show( message, caption,  buttons, icon );
    } // m_showMessageBox( sender, args )

    
    /// <summary>
    /// Returns the selected MessageBoxIcon type from the gui.
    /// </summary>
    /// <returns>One of the MessageBoxIcon types.</returns>
    private MessageBoxIcon GetMessageBoxIcon()
    {
      if( m_mbIconNone.Checked )
        return MessageBoxIcon.None;
      
      else if( m_mbIconInfo.Checked )
        return MessageBoxIcon.Information;
 
      else if( m_mbIconWarning.Checked )
        return MessageBoxIcon.Warning;

      else if( m_mbIconQuestion.Checked )
        return MessageBoxIcon.Question;

      else if( m_mbIconError.Checked )
        return MessageBoxIcon.Error;

      return MessageBoxIcon.None;
    
    } // GetMessageBoxIcon()

    
    /// <summary>
    /// Returns the selected MessageBoxButtons type from the gui.
    /// </summary>
    /// <returns>One of the MessageBoxButtons types.</returns>
    private MessageBoxButtons GetMessageBoxButtons()
    {
      if( m_mbButtonOk.Checked )
        return MessageBoxButtons.OK;
      
      else if( m_mbButtonOkCancel.Checked )
        return MessageBoxButtons.OKCancel;
 
      else if( m_mbButtonYesNo.Checked )
        return MessageBoxButtons.YesNo;

      else if( m_mbButtonYesNoCancel.Checked )
        return MessageBoxButtons.YesNoCancel;

      else if( m_mbButtonRetryCancel.Checked )
        return MessageBoxButtons.RetryCancel;

      else if( m_mbButtonAbortRetryIgnore.Checked )
        return MessageBoxButtons.AbortRetryIgnore;
     
      return MessageBoxButtons.OK;
    
    } // GetMessageBoxButtons()


    #endregion


	} // class MainForm
} // Namespace Viatronix.v3D.UI.Test
