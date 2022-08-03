// $Id: ButtonImagesEditor.cs,v 1.1.1.1 2004/03/02 03:58:38 geconomos Exp $
//
// Copyright(c) 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)

using System;
using System.Drawing;
using System.Drawing.Design;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Windows.Forms.Design;
using System.Windows.Forms.ComponentModel;

namespace Viatronix.UI.Desgin
{
  #region ButtonImagesEditor

  /// <summary>
  /// Type editor for the CustomImages property.
  /// </summary>
  internal class ButtonImagesEditor : System.Drawing.Design.UITypeEditor
  {    
    
    /// <summary>
    /// Edits the specified object's value using the editor style indicated by GetEditStyle.
    /// </summary>
    /// <param name="context">An ITypeDescriptorContext that can be used to gain additional context information.</param>
    /// <param name="provider">An IServiceProvider that this editor can use to obtain services.</param>
    /// <param name="value">The object to edit.</param>
    /// <returns>The new value of the object.</returns>
    public override object EditValue( ITypeDescriptorContext context, IServiceProvider provider, object value ) 
    {
      // get a reference to the editor service
      IWindowsFormsEditorService m_service = (IWindowsFormsEditorService) provider.GetService( typeof( IWindowsFormsEditorService ) );

      // create the editor form
      ButtonImagesEditorForm form = new ButtonImagesEditorForm( ( Image[] ) value );
      
      // show the dialog
      m_service.ShowDialog( form );      
      
      // return tyhe new values
      return form.CustomImages;
    } // EditValue( context, provider, value ) 

    
    /// <summary>
    /// Gets the editor style used by the EditValue method.
    /// </summary>
    /// <param name="context">An ITypeDescriptorContext that can be used to gain additional context information.</param>
    /// <returns>A UITypeEditorEditStyle value that indicates the style of editor used by EditValue.</returns>
    public override UITypeEditorEditStyle GetEditStyle(ITypeDescriptorContext context)
    {
      return UITypeEditorEditStyle.Modal;
    } // GetEditStyle( context )

  } // class ButtonImagesEditor


  #endregion
  
  #region ButtonImagesEditorForm

  /// <summary>
  /// Provides an user interface for setting the images for the "CutomImages" type editor.
  /// </summary>
  internal class ButtonImagesEditorForm : System.Windows.Forms.Form
  {
    #region fields

    /// <summary>
    /// Custom images array.
    /// </summary>
    private Bitmap[] m_customImages = new Bitmap[ 3 ];

    /// <summary>
    /// Control's components container.
    /// </summary>
    private System.ComponentModel.Container components = null;
    
    /// <summary>
    /// "Button Up" image.
    /// </summary>
    private System.Windows.Forms.PictureBox m_buttonUpImage;
    
    /// <summary>
    /// "Button Down" image.
    /// </summary>
    private System.Windows.Forms.PictureBox m_buttonDownImage;
    
    /// <summary>
    /// "Button Hover" image.
    /// </summary>
    private System.Windows.Forms.PictureBox m_buttonHoverImage;
    
    /// <summary>
    /// "Load Up State" button.
    /// </summary>
    private System.Windows.Forms.Button m_loadUpButton;
    
    /// <summary>
    /// "Load Down State" button.
    /// </summary>
    private System.Windows.Forms.Button m_loadDownButton;
    
    /// <summary>
    /// "Load Hover State" button.
    /// </summary>
    private System.Windows.Forms.Button m_loadHoverButton;
    
    /// <summary>
    /// "Clear Up State" button.
    /// </summary>
    private System.Windows.Forms.Button m_clearUpButton;
    
    /// <summary>
    /// "Clear Down State" button.
    /// </summary>
    private System.Windows.Forms.Button m_clearDownButton;
    
    /// <summary>
    /// "Clear Hover State" button.
    /// </summary>
    private System.Windows.Forms.Button m_clearHoverButton;
    
    /// <summary>
    /// "OK" button.
    /// </summary>
    private System.Windows.Forms.Button m_ok;
    
    /// <summary>
    /// "Cancel" button.
    /// </summary>
    private System.Windows.Forms.Button m_cancel;
    
    /// <summary>
    /// "Up State" group box.
    /// </summary>
    private System.Windows.Forms.GroupBox m_groupBoxUp;
    
    /// <summary>
    /// "Down State" group box.
    /// </summary>
    private System.Windows.Forms.GroupBox m_groupBoxDown;
    
    /// <summary>
    /// "Hover State" group box.
    /// </summary>
    private System.Windows.Forms.GroupBox m_groupBoxOver;

    #endregion

    #region properties

    /// <summary>
    /// Gets the custom image array.
    /// </summary>
    public Bitmap[] CustomImages
    {
      get { return m_customImages; } 
    } // CustomImages

    
    #endregion

    #region initialization

    /// <summary>
    /// Initializes a ButtonImagesEditorForm with the specified images.
    /// </summary>
    /// <param name="images">Image array for button states.</param>
    public ButtonImagesEditorForm( Image [] images )
    {
      InitializeComponent();

      // set the image for the button controls
      m_buttonUpImage.Image = images[ 0 ] != null ? (Image)images[ 0 ].Clone() : null;
      m_buttonDownImage.Image = images[ 1 ] != null ? (Image)images[ 1 ].Clone() : null;
      m_buttonHoverImage.Image = images[ 2 ] != null ? (Image)images[ 2 ].Clone() : null;
    } // ButtonImagesEditorForm(  images )

    
    /// <summary>
    /// Releases the unmanaged resources used by the Control and optionally releases the managed resources.
    /// </summary>
    /// <param name="disposing">true to release both managed and unmanaged resources; false to release only unmanaged resources.</param>
    protected override void Dispose( bool disposing )
    {
      if( disposing )
      {
        if(components != null)
          components.Dispose();
      } // if( disposing )
      base.Dispose( disposing );
    } // Dispose(  disposing )

    
    #endregion      

    #region child controls event handlers
        
    /// <summary>
    /// Click handler for the 3 "Load" buttons.
    /// </summary>
    /// <param name="sender">Sender of event.</param>
    /// <param name="args">EventArgs containing data specific to this event.</param>
    private void m_load_Click( object sender, System.EventArgs args )
    {
      // show the open file dialog
      OpenFileDialog dialog = new OpenFileDialog();
      if( dialog.ShowDialog() == DialogResult.OK )
      {
        // load the image from the specified file
        Image image = Bitmap.FromFile( dialog.FileName );
        
        if( sender == m_loadUpButton )
          m_buttonUpImage.Image = image;
          
        else if( sender == m_loadDownButton )
          m_buttonDownImage.Image = image;
          
        else if( sender == m_loadHoverButton )
          m_buttonHoverImage.Image = image;
      
      } // if( dialog.ShowDialog() == DialogResult.OK )
    } // m_load_Click( sender, args )

    
    /// <summary>
    /// Click handler for the 3 "Clear" buttons.
    /// </summary>
    /// <param name="sender">Sender of event.</param>
    /// <param name="args">EventArgs containing data specific to this event.</param>
    private void m_clear_Click( object sender, System.EventArgs args )
    {
      if( sender == m_clearUpButton )
        m_buttonUpImage.Image = null;
        
      else if( sender == m_clearDownButton )
        m_buttonDownImage.Image = null;
        
      else if( sender == m_clearHoverButton )
        m_buttonHoverImage.Image = null;
      
    } // m_clear_Click( sender, args )

        
    /// <summary>
    /// Click handler for the "OK" button.
    /// </summary>
    /// <param name="sender">Sender of event.</param>
    /// <param name="args">EventArgs containing data specific to this event.</param>
    private void m_ok_Click( object sender, System.EventArgs args )
    {
      m_customImages[ 0 ] = m_buttonUpImage.Image != null ? new Bitmap( m_buttonUpImage.Image ) : null;
      m_customImages[ 1 ] = m_buttonDownImage.Image != null ? new Bitmap( m_buttonDownImage.Image ) : null;
      m_customImages[ 2 ] = m_buttonHoverImage.Image != null ? new Bitmap( m_buttonHoverImage.Image ) : null;
    } // m_ok_Click( sender, args ) 

    
    #endregion

    #region windows form designer generated code
    
    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      this.m_loadDownButton = new System.Windows.Forms.Button();
      this.m_groupBoxDown = new System.Windows.Forms.GroupBox();
      this.m_clearDownButton = new System.Windows.Forms.Button();
      this.m_buttonDownImage = new System.Windows.Forms.PictureBox();
      this.m_loadUpButton = new System.Windows.Forms.Button();
      this.m_groupBoxUp = new System.Windows.Forms.GroupBox();
      this.m_clearUpButton = new System.Windows.Forms.Button();
      this.m_buttonUpImage = new System.Windows.Forms.PictureBox();
      this.m_cancel = new System.Windows.Forms.Button();
      this.m_ok = new System.Windows.Forms.Button();
      this.m_loadHoverButton = new System.Windows.Forms.Button();
      this.m_groupBoxOver = new System.Windows.Forms.GroupBox();
      this.m_clearHoverButton = new System.Windows.Forms.Button();
      this.m_buttonHoverImage = new System.Windows.Forms.PictureBox();
      this.m_groupBoxDown.SuspendLayout();
      this.m_groupBoxUp.SuspendLayout();
      this.m_groupBoxOver.SuspendLayout();
      this.SuspendLayout();
      // 
      // m_loadDownButton
      // 
      this.m_loadDownButton.Location = new System.Drawing.Point(11, 133);
      this.m_loadDownButton.Name = "m_loadDownButton";
      this.m_loadDownButton.Size = new System.Drawing.Size(50, 17);
      this.m_loadDownButton.TabIndex = 13;
      this.m_loadDownButton.Text = "Load";
      this.m_loadDownButton.Click += new System.EventHandler(this.m_load_Click);
      // 
      // m_groupBoxDown
      // 
      this.m_groupBoxDown.Controls.AddRange(new System.Windows.Forms.Control[] {
                                                                            this.m_clearDownButton,
                                                                            this.m_buttonDownImage,
                                                                            this.m_loadDownButton});
      this.m_groupBoxDown.Location = new System.Drawing.Point(131, 8);
      this.m_groupBoxDown.Name = "m_groupBoxDown";
      this.m_groupBoxDown.Size = new System.Drawing.Size(126, 157);
      this.m_groupBoxDown.TabIndex = 12;
      this.m_groupBoxDown.TabStop = false;
      this.m_groupBoxDown.Text = "Button Down";
      // 
      // m_clearDownButton
      // 
      this.m_clearDownButton.Location = new System.Drawing.Point(69, 133);
      this.m_clearDownButton.Name = "m_clearDownButton";
      this.m_clearDownButton.Size = new System.Drawing.Size(50, 17);
      this.m_clearDownButton.TabIndex = 14;
      this.m_clearDownButton.Text = "Clear";
      this.m_clearDownButton.Click += new System.EventHandler(this.m_clear_Click);
      // 
      // m_buttonDownImage
      // 
      this.m_buttonDownImage.Location = new System.Drawing.Point(13, 20);
      this.m_buttonDownImage.Name = "m_buttonDownImage";
      this.m_buttonDownImage.Size = new System.Drawing.Size(100, 100);
      this.m_buttonDownImage.SizeMode = System.Windows.Forms.PictureBoxSizeMode.CenterImage;
      this.m_buttonDownImage.TabIndex = 0;
      this.m_buttonDownImage.TabStop = false;
      // 
      // m_loadUpButton
      // 
      this.m_loadUpButton.Location = new System.Drawing.Point(8, 134);
      this.m_loadUpButton.Name = "m_loadUpButton";
      this.m_loadUpButton.Size = new System.Drawing.Size(50, 17);
      this.m_loadUpButton.TabIndex = 11;
      this.m_loadUpButton.Text = "Load";
      this.m_loadUpButton.Click += new System.EventHandler(this.m_load_Click);
      // 
      // m_groupBoxUp
      // 
      this.m_groupBoxUp.Controls.AddRange(new System.Windows.Forms.Control[] {
                                                                            this.m_clearUpButton,
                                                                            this.m_buttonUpImage,
                                                                            this.m_loadUpButton});
      this.m_groupBoxUp.Location = new System.Drawing.Point(0, 8);
      this.m_groupBoxUp.Name = "m_groupBoxUp";
      this.m_groupBoxUp.Size = new System.Drawing.Size(126, 157);
      this.m_groupBoxUp.TabIndex = 10;
      this.m_groupBoxUp.TabStop = false;
      this.m_groupBoxUp.Text = "Button Up";
      // 
      // m_clearUpButton
      // 
      this.m_clearUpButton.Location = new System.Drawing.Point(68, 134);
      this.m_clearUpButton.Name = "m_clearUpButton";
      this.m_clearUpButton.Size = new System.Drawing.Size(50, 17);
      this.m_clearUpButton.TabIndex = 12;
      this.m_clearUpButton.Text = "Clear";
      this.m_clearUpButton.Click += new System.EventHandler(this.m_clear_Click);
      // 
      // m_buttonUpImage
      // 
      this.m_buttonUpImage.Location = new System.Drawing.Point(14, 20);
      this.m_buttonUpImage.Name = "m_buttonUpImage";
      this.m_buttonUpImage.Size = new System.Drawing.Size(100, 100);
      this.m_buttonUpImage.SizeMode = System.Windows.Forms.PictureBoxSizeMode.CenterImage;
      this.m_buttonUpImage.TabIndex = 0;
      this.m_buttonUpImage.TabStop = false;
      // 
      // m_cancel
      // 
      this.m_cancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
      this.m_cancel.Location = new System.Drawing.Point(233, 171);
      this.m_cancel.Name = "m_cancel";
      this.m_cancel.TabIndex = 9;
      this.m_cancel.Text = "Cancel";
      // 
      // m_ok
      // 
      this.m_ok.DialogResult = System.Windows.Forms.DialogResult.OK;
      this.m_ok.Location = new System.Drawing.Point(311, 170);
      this.m_ok.Name = "m_ok";
      this.m_ok.TabIndex = 8;
      this.m_ok.Text = "OK";
      this.m_ok.Click += new System.EventHandler(this.m_ok_Click);
      // 
      // m_loadHoverButton
      // 
      this.m_loadHoverButton.Location = new System.Drawing.Point(11, 133);
      this.m_loadHoverButton.Name = "m_loadHoverButton";
      this.m_loadHoverButton.Size = new System.Drawing.Size(50, 17);
      this.m_loadHoverButton.TabIndex = 17;
      this.m_loadHoverButton.Text = "Load";
      this.m_loadHoverButton.Click += new System.EventHandler(this.m_load_Click);
      // 
      // m_groupBoxOver
      // 
      this.m_groupBoxOver.Controls.AddRange(new System.Windows.Forms.Control[] {
                                                                            this.m_clearHoverButton,
                                                                            this.m_buttonHoverImage,
                                                                            this.m_loadHoverButton});
      this.m_groupBoxOver.Location = new System.Drawing.Point(262, 8);
      this.m_groupBoxOver.Name = "m_groupBoxOver";
      this.m_groupBoxOver.Size = new System.Drawing.Size(126, 157);
      this.m_groupBoxOver.TabIndex = 16;
      this.m_groupBoxOver.TabStop = false;
      this.m_groupBoxOver.Text = "Button Hover";
      // 
      // m_clearHoverButton
      // 
      this.m_clearHoverButton.Location = new System.Drawing.Point(70, 133);
      this.m_clearHoverButton.Name = "m_clearHoverButton";
      this.m_clearHoverButton.Size = new System.Drawing.Size(50, 17);
      this.m_clearHoverButton.TabIndex = 18;
      this.m_clearHoverButton.Text = "Clear";
      this.m_clearHoverButton.Click += new System.EventHandler(this.m_clear_Click);
      // 
      // m_buttonHoverImage
      // 
      this.m_buttonHoverImage.Location = new System.Drawing.Point(13, 20);
      this.m_buttonHoverImage.Name = "m_buttonHoverImage";
      this.m_buttonHoverImage.Size = new System.Drawing.Size(100, 100);
      this.m_buttonHoverImage.SizeMode = System.Windows.Forms.PictureBoxSizeMode.CenterImage;
      this.m_buttonHoverImage.TabIndex = 0;
      this.m_buttonHoverImage.TabStop = false;
      // 
      // ButtonImagesEditorForm
      // 
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.ClientSize = new System.Drawing.Size(396, 206);
      this.ControlBox = false;
      this.Controls.AddRange(new System.Windows.Forms.Control[] {
                                                                  this.m_groupBoxOver,
                                                                  this.m_groupBoxDown,
                                                                  this.m_groupBoxUp,
                                                                  this.m_cancel,
                                                                  this.m_ok});
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
      this.Name = "ButtonImagesEditorForm";
      this.Text = "Viatronix";
      this.m_groupBoxDown.ResumeLayout(false);
      this.m_groupBoxUp.ResumeLayout(false);
      this.m_groupBoxOver.ResumeLayout(false);
      this.ResumeLayout(false);

    }
		
    #endregion
      
  } // class ButtonImagesEditorForm

  #endregion

} // namepsace Viatronix.UI.Desgin

#region revision history

// $Log: ButtonImagesEditor.cs,v $
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.6  2003/08/20 14:22:59  michael
// added "mailto:"
//
// Revision 1.5  2003/05/25 18:12:06  michael
// .net --> .com
//
// Revision 1.4  2003/04/28 01:24:33  geconomos
// Renamed from CustomImagesEditor.
//
// Revision 1.5  2003/04/27 23:51:48  geconomos
// More code cleanup.
//
// Revision 1.3  2003/04/25 21:06:44  geconomos
// Endless code reviews.
//
// Revision 1.2  2003/04/17 19:41:05  geconomos
// Some more code cleanup.
//
// Revision 1.1  2002/12/12 15:38:21  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/ButtonImagesEditor.cs,v 1.1.1.1 2004/03/02 03:58:38 geconomos Exp $
// $Id: ButtonImagesEditor.cs,v 1.1.1.1 2004/03/02 03:58:38 geconomos Exp $

#endregion
