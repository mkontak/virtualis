// $Id: FormAbout.cs,v 1.7 2007/03/01 22:55:28 gdavidson Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

using System;
using System.Configuration;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Reflection;
using System.Xml;

namespace Viatronix.UI
{
  /// <summary>
  /// About Box for application. Displays information about the active plugin.
  /// </summary>
	public class FormAbout : System.Windows.Forms.Form
	{

    #region fields

    /// <summary>
    /// Label for Copy Right
    /// </summary>
    private System.Windows.Forms.Label m_copyright;
    
    /// <summary>
    /// Label for Legal
    /// </summary>
    private System.Windows.Forms.Label m_legal;
   
    /// <summary>
    /// Image for Background
    /// </summary>
    private static Image m_backgroundImage;

    /// <summary>
    /// close link
    /// </summary>
    private System.Windows.Forms.LinkLabel m_close;

    /// <summary>
    /// link to company website
    /// </summary>
    private Viatronix.UI.Panel m_webLinkPanel;

    /// <summary>
    /// software disclaimer
    /// </summary>
    private string m_disclaimer = string.Empty;

    /// <summary>
    /// company link
    /// </summary>
    private string m_link = string.Empty;

    /// <summary>
    /// Panel for logo
    /// </summary>
    private System.Windows.Forms.Panel m_logoArea;

    /// <summary>
    /// Image for logo.
    /// </summary>
    private Image m_logoImage = null;

    /// <summary>
    /// Panel for Bullet information
    /// </summary>
    private Viatronix.UI.Panel m_bulletsArea;

    /// <summary>
    /// Designer components object
    /// </summary>
    private System.ComponentModel.Container components = null;

    /// <summary>
    /// Image for bullet
    /// </summary>
    private static Image m_bulletImage = null;

    /// <summary>
    /// bullet information
    /// </summary>
    private System.Collections.Specialized.StringCollection m_bullets = new System.Collections.Specialized.StringCollection();

    /// <summary>
    /// draw bullets
    /// </summary>
    private bool m_drawBullets = true;

    /// <summary>
    /// version
    /// </summary>
    private string m_version = Assembly.GetExecutingAssembly().GetName().Version.ToString();

    /// <summary>
    /// site identifier
    /// </summary>
    private string m_siteID = string.Empty;

    /// <summary>
    /// computer name
    /// </summary>
    private string m_computerName = System.Environment.MachineName;

    /// <summary>
    /// product identifier
    /// </summary>
    private string m_productID = string.Empty;

    #endregion

    #region construction

    /// <summary>
    /// static constructor
    /// </summary>
    static FormAbout()
    {
      m_backgroundImage = new Bitmap(Assembly.GetExecutingAssembly().GetManifestResourceStream("Viatronix.UI.res.about.png"));
      m_bulletImage = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream("Viatronix.UI.res.bullet.png"));
    } // FormAbout
	
   
    /// <summary>
    /// Initializes FormLogin with default values.
    /// </summary>
    public FormAbout()
		{

      SetStyle( ControlStyles.AllPaintingInWmPaint, true );
      SetStyle( ControlStyles.DoubleBuffer, true );
      SetStyle( ControlStyles.UserPaint, true );
			
      InitializeComponent();

      this.Version = Assembly.GetEntryAssembly().GetName().Version.ToString();

      AboutConfigSettings settings = ( AboutConfigSettings ) ConfigurationManager.GetSection( "aboutInfo" );
      this.Legal = settings.Legal;
      this.Copyright = settings.Copyright;
      this.Disclaimer = settings.Disclaimer;
      this.Link = settings.Link;
      this.Logo = settings.Logo;

		} // FormAbout()
    
    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the version.
    /// </summary>
    public string Version
    {
      get { return m_version; }
      set { m_version = value; }
    } // Version


    /// <summary>
    /// Gets or sets the site id.
    /// </summary>
    public string SiteID
    {
      get { return m_siteID; }
      set { m_siteID = value; }
    } // SiteID


    /// <summary>
    /// Gets or sets the computer name.
    /// </summary>
    public string ComputerName
    {
      get { return m_computerName; }
      set { m_computerName = value; }
    } // ComputerName


    /// <summary>
    /// Gets or sets the version.
    /// </summary>
    public string ProductID
    {
      get { return m_productID; }
      set { m_productID = value; }
    } // ProductID


    /// <summary>
    /// Gets or sets the legal information.
    /// </summary>
    public string Legal
    {
      get { return m_legal.Text; }
      set { m_legal.Text = value; }
    } // Legal


    /// <summary>
    /// Gets or sets the copyright.
    /// </summary>
    public string Copyright
    {
      get { return m_copyright.Text; }
      set { m_copyright.Text = value; }
    } // Copyright


    /// <summary>
    /// Gets or sets the software disclaimer.
    /// </summary>
    public string Disclaimer
    {
      get { return m_disclaimer; }
      set { m_disclaimer = value; }
    } // Disclaimer


    /// <summary>
    /// Gets or sets the company link.
    /// </summary>
    public string Link
    {
      get { return m_link; }
      set { m_link = value; }
    } // Link


    /// <summary>
    /// Gets or sets if the close link is displayed.
    /// </summary>
    public bool AllowClose
    {
      get { return m_close.Visible; }
      set { m_close.Visible = value; }
    } // AllowClose


    /// <summary>
    /// Gets or sets the logo image.
    /// </summary>
    public Image Logo
    {
      get { return m_logoImage; }
      set { m_logoImage = value; }
    } // Logo


    /// <summary>
    /// Returns a reference to the bullets collection
    /// </summary>
    public System.Collections.Specialized.StringCollection Bullets
    {
      get { return m_bullets; }
    } // Bullets


    /// <summary>
    /// Gets or sets if bullet items are displayed.
    /// </summary>
    protected bool DrawBullets
    {
      get { return m_drawBullets; }
      set 
      { 
        m_drawBullets = value;
        m_bulletsArea.Visible = value; 
        m_bulletsArea.SendToBack();
        OnBulletsVisibleChanged( value );
      }
    } // DrawBullets

    #endregion

    #region methods

    /// <summary>
    /// Called when the visibility of the bullets area changes.
    /// </summary>
    /// <param name="visible">true if the area is visible; otherwise false</param>
    protected virtual void OnBulletsVisibleChanged( bool visible )
    {
    }


    /// <summary>
    /// Paint the control including diagonal disclaimer
    /// </summary>
    /// <param name="args">the paint event args</param>
    protected override void OnPaint(PaintEventArgs args)
    {
      base.OnPaint( args );

      Graphics gfx = args.Graphics;

      gfx.DrawImage( m_backgroundImage, this.ClientRectangle );

      if( m_logoImage != null )
        Viatronix.UI.Utilities.DrawImage( args.Graphics, m_logoImage, m_logoArea.Bounds );    

      if( this.DrawBullets && this.Bullets.Count > 0 )
      {
        using( SolidBrush brush = new SolidBrush( Color.Black ) )
          DrawBulletItems( gfx, brush );
       }

      DrawDisclaimer( gfx );

      System.Drawing.Font font = new System.Drawing.Font( "Microsoft Sans Serif", 8, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, 0 );
      gfx.DrawString(((m_siteID == string.Empty) ? "" : m_siteID  + " " )  + m_computerName, font, Brushes.Black, 10, this.Height - 40 );
      gfx.DrawString( m_productID + " " + m_version, font, Brushes.Black, 10, this.Height - 25 );
    } // OnPaint( PaintEventArgs args )

 
    /// <summary>
    /// Draw the disclaimer if it is present.
    /// </summary>
    /// <param name="gfx">Graphics object for drawing</param>
    private void DrawDisclaimer( Graphics gfx )
    {
      // draw the softwae disclaimer in bold, diagonal red, if any
      if( m_disclaimer != string.Empty )
      {
        // rotate it
        System.Drawing.Drawing2D.Matrix matrix = gfx.Transform;
        gfx.TranslateTransform( 0, 500 );
        gfx.RotateTransform( -30 );

        // size it to fit
        float fontSize = 100;
        System.Drawing.Font font = new System.Drawing.Font("Microsoft Sans Serif", fontSize, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
        SizeF size = gfx.MeasureString( m_disclaimer, font );
        while ( size.Width > 750 )
        {
          fontSize *= 0.9F;
          font = new System.Drawing.Font( font.FontFamily, fontSize, font.Style, font.Unit, ((System.Byte)(0)) );
          size = gfx.MeasureString( m_disclaimer, font );
        }

        // draw it
        gfx.DrawString( m_disclaimer, font, new System.Drawing.SolidBrush( Color.Red ), 0, 0 );
        gfx.Transform = matrix;
      } 
    } // DrawDisclaimer( Graphics gfx )


    /// <summary>
    /// Draws all the bullet items with the specified brush.
    /// </summary>
    /// <param name="gfx">Graphics object for drawing</param>
    /// <param name="brush">Text brush.</param>
    private void DrawBulletItems( Graphics gfx, SolidBrush brush )
    {
      // compute the available area for the bullet text
      SizeF layout = new SizeF( m_bulletsArea.Width - m_bulletImage.Width, m_bulletsArea.Height );

      // current y position
      int y = m_bulletsArea.Top;
      
      // add each bullet item
      foreach( string text in this.Bullets )
      {
        // get the required size for the bullet text
        SizeF size = gfx.MeasureString( text, this.Font, layout );
        
        // create a label to contain the bullet text
        RectangleF textLayout = new RectangleF();
        textLayout.Size =  new SizeF( size.Width + 10.0F, size.Height );

        // create a picture box to hold the bullet image
        Rectangle imageLayout = new Rectangle();
        imageLayout.Size = m_bulletImage.Size;
        
        // size and position the controls
        if( m_bulletImage.Height > size.Height )
        {
          imageLayout.Location = new Point( m_bulletsArea.Left, y );          
          textLayout.Location = new Point( m_bulletsArea.Left + m_bulletImage.Width, (int)( y + ( ( m_bulletImage.Height - size.Height ) / 2.0F )) );
        }
        else
        {
          textLayout.Location = new Point( m_bulletsArea.Left + m_bulletImage.Width, y );
          imageLayout.Location = new Point( m_bulletsArea.Left, y );
        }
        
        // draw the controls
        gfx.DrawImage( m_bulletImage, imageLayout );
        gfx.DrawString( text, this.Font, brush, textLayout );

        // adjust the next y position
        y += Math.Max( (int)( textLayout.Height + 0.5F ), imageLayout.Height ) + 10;
      } 
    } // DrawBulletItems( gfx, brush )

    
    /// <summary>
    /// Disposes the resources associated with this instance
    /// </summary>
    /// <param name="disposing">True to dispose managed and unmanaged resources; False to dispose unmanaged only</param>
    protected override void Dispose( bool disposing )
    {
      if( disposing )
      {
        if(components != null)
          components.Dispose();
      }
      base.Dispose( disposing );
    } // Dispose( bool disposing )


    /// <summary>
    /// Handles the close hyperlink being clicked.
    /// </summary>
    /// <param name="args">Arguments to take into account.</param>
    protected virtual void OnCloseLinkClicked(LinkLabelLinkClickedEventArgs args)
    {
      this.DialogResult = System.Windows.Forms.DialogResult.Cancel;
    } // OnCloseLinkClicked( args )

    #endregion

    #region event handlers

    /// <summary>
    /// Closes the dialog.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An LinkLabelLinkClickedEventArgs that contains the event data.</param>
    private void OnCloseClickHandler( object sender, LinkLabelLinkClickedEventArgs args )
    {
      OnCloseLinkClicked(args);
    } // OnCloseClickHandler( sender, args )


    /// <summary>
    /// Handles the mouse enter event.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnLinkEnterHandler( object sender, EventArgs args )
    {
      this.Cursor = Cursors.Hand;
    } // OnLinkEnterHandler( sender, args )


    /// <summary>
    /// Handles the mouse leave event.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnLinkLeaveHandler( object sender, EventArgs args )
    {
      this.Cursor = Cursors.Default;
    } // OnLinkLeaveHandler( sender, args )


    /// <summary>
    /// Handles the link click event.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnLinkClickHandler( object sender, EventArgs args )
    {
      if( m_link != string.Empty )
        System.Diagnostics.Process.Start( m_link );
    } // OnLinkClickHandler( sender, args )

    #endregion

		#region windows form designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FormAbout));
      this.m_copyright = new System.Windows.Forms.Label();
      this.m_legal = new System.Windows.Forms.Label();
      this.m_webLinkPanel = new Viatronix.UI.Panel();
      this.m_close = new System.Windows.Forms.LinkLabel();
      this.m_logoArea = new System.Windows.Forms.Panel();
      this.m_bulletsArea = new Viatronix.UI.Panel();
      this.SuspendLayout();
      // 
      // m_copyright
      // 
      this.m_copyright.BackColor = System.Drawing.Color.Transparent;
      this.m_copyright.ForeColor = System.Drawing.Color.Black;
      this.m_copyright.Location = new System.Drawing.Point(557, 463);
      this.m_copyright.Name = "m_copyright";
      this.m_copyright.Size = new System.Drawing.Size(231, 26);
      this.m_copyright.TabIndex = 0;
      this.m_copyright.Text = "Copyright © 2001-2006 Viatronix Inc.\nAll rights reserved. Patents pending.";
      this.m_copyright.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
      // 
      // m_legal
      // 
      this.m_legal.BackColor = System.Drawing.Color.Transparent;
      this.m_legal.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_legal.Location = new System.Drawing.Point(182, 549);
      this.m_legal.Name = "m_legal";
      this.m_legal.Size = new System.Drawing.Size(546, 26);
      this.m_legal.TabIndex = 0;
      this.m_legal.Text = resources.GetString("m_legal.Text");
      // 
      // m_webLinkPanel
      // 
      this.m_webLinkPanel.BackColor = System.Drawing.Color.Transparent;
      this.m_webLinkPanel.BorderColor = System.Drawing.Color.Transparent;
      this.m_webLinkPanel.BottomPadding = 10;
      this.m_webLinkPanel.Curve = 10;
      this.m_webLinkPanel.FillColor = System.Drawing.Color.Transparent;
      this.m_webLinkPanel.LeftPadding = 10;
      this.m_webLinkPanel.Location = new System.Drawing.Point(143, 14);
      this.m_webLinkPanel.Name = "m_webLinkPanel";
      this.m_webLinkPanel.RightPadding = 10;
      this.m_webLinkPanel.Size = new System.Drawing.Size(353, 65);
      this.m_webLinkPanel.TabIndex = 0;
      this.m_webLinkPanel.TopPadding = 10;
      this.m_webLinkPanel.MouseLeave += new System.EventHandler(this.OnLinkLeaveHandler);
      this.m_webLinkPanel.Click += new System.EventHandler(this.OnLinkClickHandler);
      this.m_webLinkPanel.MouseEnter += new System.EventHandler(this.OnLinkEnterHandler);
      // 
      // m_close
      // 
      this.m_close.ActiveLinkColor = System.Drawing.Color.Blue;
      this.m_close.BackColor = System.Drawing.Color.Transparent;
      this.m_close.LinkColor = System.Drawing.Color.White;
      this.m_close.Location = new System.Drawing.Point(752, 568);
      this.m_close.Name = "m_close";
      this.m_close.Size = new System.Drawing.Size(41, 23);
      this.m_close.TabIndex = 8;
      this.m_close.TabStop = true;
      this.m_close.Text = "Close";
      this.m_close.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      this.m_close.VisitedLinkColor = System.Drawing.Color.White;
      this.m_close.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.OnCloseClickHandler);
      // 
      // m_logoArea
      // 
      this.m_logoArea.BackColor = System.Drawing.Color.Transparent;
      this.m_logoArea.Location = new System.Drawing.Point(576, 123);
      this.m_logoArea.Name = "m_logoArea";
      this.m_logoArea.Size = new System.Drawing.Size(191, 71);
      this.m_logoArea.TabIndex = 0;
      this.m_logoArea.Visible = false;
      // 
      // m_bulletsArea
      // 
      this.m_bulletsArea.BackColor = System.Drawing.Color.Transparent;
      this.m_bulletsArea.BorderColor = System.Drawing.Color.Transparent;
      this.m_bulletsArea.BottomPadding = 10;
      this.m_bulletsArea.Curve = 10;
      this.m_bulletsArea.FillColor = System.Drawing.Color.Transparent;
      this.m_bulletsArea.LeftPadding = 10;
      this.m_bulletsArea.Location = new System.Drawing.Point(566, 205);
      this.m_bulletsArea.Name = "m_bulletsArea";
      this.m_bulletsArea.RightPadding = 10;
      this.m_bulletsArea.Size = new System.Drawing.Size(212, 255);
      this.m_bulletsArea.TabIndex = 0;
      this.m_bulletsArea.TopPadding = 10;
      this.m_bulletsArea.Visible = false;
      // 
      // FormAbout
      // 
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.ClientSize = new System.Drawing.Size(800, 600);
      this.Controls.Add(this.m_bulletsArea);
      this.Controls.Add(this.m_logoArea);
      this.Controls.Add(this.m_close);
      this.Controls.Add(this.m_webLinkPanel);
      this.Controls.Add(this.m_legal);
      this.Controls.Add(this.m_copyright);
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.Name = "FormAbout";
      this.ShowInTaskbar = false;
      this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
      this.ResumeLayout(false);

    }
		#endregion

  } // class FormAbout

  #region AboutConfigSectionHandler class

  /// <summary>
  /// Parses the console information contained in the app.config
  /// </summary>
  public class AboutConfigSectionHandler : IConfigurationSectionHandler
  {

    #region IConfigurationSectionHandler Members

    /// <summary>
    /// Parses the configuration information for the console's login dialog.
    /// </summary>
    /// <param name="parent">The configuration settings in a corresponding parent configuration section.</param>
    /// <param name="configContext">null reference</param>
    /// <param name="section">The XmlNode that contains the configuration information from the configuration file. Provides direct access to the XML contents of the configuration section.</param>
    /// <returns>A configuration object.</returns>
    public object Create( object parent, object configContext, XmlNode section )
    {
      AboutConfigSettings settings = new AboutConfigSettings();
      try
      {
        XmlNode node = section.SelectSingleNode( "legal" );
        if( node != null )
          settings.Legal = node.InnerText;

        node = section.SelectSingleNode( "copyright" );
        if( node != null )
          settings.Copyright = node.InnerText;

        node = section.SelectSingleNode( "link" );
        if( node != null )
          settings.Link = node.InnerText;

        node = section.SelectSingleNode( "disclaimer" );
        if( node != null )
          settings.Disclaimer = node.InnerText;

        node = section.SelectSingleNode( "logo" );
        if( node != null )
          settings.Logo = Viatronix.UI.Utilities.LoadImage( node.InnerText );
      }
      catch( XmlException e )
      {
        Viatronix.Logging.Log.Error("Unable to load about information from the configuration file [ERROR=" + e.Message + "]", "AboutConfigSectionHandler", "Create");
      }

      return settings;
    } // Create( parent, configContext, section )

    #endregion

  } // class AboutConfigSectionHandler

  #endregion

  #region AboutConfigSettings class

  /// <summary>
  /// Contains console information defined in the app.config
  /// </summary>
  public class AboutConfigSettings
  {

    #region fields

    /// <summary>
    /// legal information
    /// </summary>
    private string m_legal = string.Empty;

    /// <summary>
    /// copyright information
    /// </summary>
    private string m_copyright = string.Empty;

    /// <summary>
    /// link to the company website
    /// </summary>
    private string m_link = string.Empty;

    /// <summary>
    /// software disclaimer
    /// </summary>
    private string m_disclaimer = string.Empty;

    /// <summary>
    /// logo image
    /// </summary>
    private Image m_logo = null;

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the legal information
    /// </summary>
    public string Legal
    {
      get { return m_legal; }
      set { m_legal = value; }
    } // Legal


    /// <summary>
    /// Gets or sets the copyright information
    /// </summary>
    public string Copyright
    {
      get { return m_copyright; }
      set { m_copyright = value; }
    } // Copyright


    /// <summary>
    /// Gets or sets the website link
    /// </summary>
    public string Link
    {
      get { return m_link; }
      set { m_link = value; }
    } // Link


    /// <summary>
    /// Gets or sets the software dislaimer
    /// </summary>
    public string Disclaimer
    {
      get { return m_disclaimer; }
      set { m_disclaimer = value; }
    } // Disclaimer


    /// <summary>
    /// Gets or sets the logo image.
    /// </summary>
    public Image Logo
    {
      get { return m_logo; }
      set { m_logo = value; }
    } // Logo

    #endregion

  } // class AboutConfigSettings

  #endregion

} // namespace Viatronix.UI

#region revision history

// $Log: FormAbout.cs,v $
// Revision 1.7  2007/03/01 22:55:28  gdavidson
// Commented code
//
// Revision 1.6  2007/02/26 18:41:17  gdavidson
// Changed to Viatronix panels that use DoubleBuffering
//
// Revision 1.5  2006/06/02 12:47:39  mkontak
// Fixed deprecated code
//
// Revision 1.4  2006/05/09 20:17:47  gdavidson
// Added site and product information
//
// Revision 1.3  2006/04/19 14:31:02  gdavidson
// Issue #4701: Retrieve version number from assembly
//
// Revision 1.2  2006/03/28 18:00:16  gdavidson
// Added version node to the about configuration section.
//
// Revision 1.1  2006/01/18 15:41:00  gdavidson
// Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Core/ui/FormAbout.cs,v 1.7 2007/03/01 22:55:28 gdavidson Exp $
// $Id: FormAbout.cs,v 1.7 2007/03/01 22:55:28 gdavidson Exp $

#endregion
