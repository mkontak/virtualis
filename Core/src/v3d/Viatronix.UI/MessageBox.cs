// $Id: MessageBox.cs,v 1.14 2006/03/17 19:19:27 romy Exp $
//
// Copyright(c) 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)

#region usings

using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Text;
using System.Collections;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Windows.Forms;
using System.Reflection;
using System.IO;
using System.Text;

#endregion

namespace Viatronix.UI
{

  #region MessageBox

  /// <summary>
  /// Displays a message box that can contain text, buttons, and symbols 
  /// that inform and instruct the user.
  /// </summary>
  public class MessageBox
  {

    #region constants

    /// <summary>
    /// exception file location
    /// </summary>
    static private readonly string m_exceptionFile = "";

    #endregion
   
    #region methods

    /// <summary>
    /// Shows the specified error message.
    /// </summary>
    /// <param name="message">Message to display.</param>
    static public void ShowError( string message )
    {
      Show( message, "", MessageBoxButtons.OK, MessageBoxIcon.Error );
    } // ShowError( message )
    
    
    /// <summary>
    /// Shows the specified error message.
    /// </summary>
    /// <param name="message">Message to display.
    /// <param name="owner">The owner form.</param>
    static public void ShowError( System.Windows.Forms.IWin32Window owner, string message)
    {
      Show( owner, message, "", MessageBoxButtons.OK, MessageBoxIcon.Error );
    } // ShowError( message )
    

    /// <summary>
    /// Shows the text related to the Exception object and logs the exception to a logfile.
    /// </summary>
    /// <param name="e">Exception object to display and log.</param>
    static public void ShowError( System.Exception e )
    {
      if( e.Message != String.Empty )
      {
        ShowError( e.Message );
        //LogException( e );
      } // if( e.Message != String.Empty )
      else
      {
        ShowError( e.InnerException.Message );
        //LogException( e.InnerException );
      } // else
    } // ShowError( Exception e )

    
    /// <summary>
    /// Shows the text related to the Exception object and logs the exception to a logfile.
    /// </summary>
    /// <param name="e">Exception object to display and log.</param>
    static public void ShowError( System.Windows.Forms.IWin32Window owner, System.Exception e )
    {
      if( e.Message != String.Empty )
      {
        ShowError( owner, e.Message );
        //LogException( e );
      } // if( e.Message != String.Empty )
      else
      {
        ShowError(  owner, e.InnerException.Message);
        //LogExceptionowner e.InnerException );
      } // else
    } // ShowError( Exception e )
    

    /// <summary>
    /// Shows the specified warning message.
    /// </summary>
    /// <param name="message">Message to display.</param>
    static public void ShowWarning( string message )
    {
      Show( message, "", MessageBoxButtons.OK, MessageBoxIcon.Warning );
    } // ShowWarning( message )
    
    /// Shows the specified question warning icon.
    /// </summary>
    /// <param name="question">Question to prompt user.</param>
    /// <param name="owner">The owner form.</param>
    static public bool ShowQuestionWithWarning( System.Windows.Forms.IWin32Window owner, string question )
    {
      return Show( owner, question, "", MessageBoxButtons.YesNo, MessageBoxIcon.Warning ) == DialogResult.Yes;
    } // ShowQuestion( question )

    /// Shows the specified question with warning icon.
    /// </summary>
    /// <param name="question">Question to prompt user.</param>
    static public bool ShowQuestionWithWarning( string question )
    {
      return Show( question, "", MessageBoxButtons.YesNo, MessageBoxIcon.Warning ) == DialogResult.Yes;
    } // ShowQuestion( question )
    
    /// <summary>
    /// Shows the specified informational message.
    /// </summary>
    /// <param name="message">Message to display.</param>
    static public void ShowInfo( string message )
    {
      Show( message, "", MessageBoxButtons.OK, MessageBoxIcon.Information );
    } // ShowInfo( message )


    /// <summary>
    /// Shows the specified informational message.
    /// </summary>
    /// <param name="message">Message to display.</param>
    /// <param name="owner">The owner form.</param>
    static public void ShowInfo( System.Windows.Forms.IWin32Window owner, string message )
    {
      Show( owner, message, "", MessageBoxButtons.OK, MessageBoxIcon.Information );
    } // ShowInfo( message )


    /// <summary>
    /// Shows the specified question.
    /// </summary>
    /// <param name="question">Question to prompt user.</param>
    static public bool ShowQuestion( string question )
    {
      return Show( question, "", MessageBoxButtons.YesNo, MessageBoxIcon.Question ) == DialogResult.Yes;
    } // ShowQuestion( question )


    /// <summary>
    /// Shows the specified question.
    /// </summary>
    /// <param name="question">Question to prompt user.</param>
    /// <param name="owner">The owner form.</param>
    static public bool ShowQuestion( System.Windows.Forms.IWin32Window owner, string question )
    {
      return Show( owner, question, "", MessageBoxButtons.YesNo, MessageBoxIcon.Question ) == DialogResult.Yes;
    } // ShowQuestion( question )

    
    /// <summary>
    /// Shows the specified question.
    /// </summary>
    /// <param name="question">Question to prompt user.</param>
    static public System.Windows.Forms.DialogResult ShowQuestion( string question, MessageBoxButtons buttons )
    {
      return Show( question, "", buttons, MessageBoxIcon.Question );
    } // ShowQuestion( question, buttons )


    /// <summary>
    /// Shows the specified question.
    /// </summary>
    /// <param name="question">Question to prompt user.</param>
    /// <param name="buttons">Buttons being displayed</param>
    /// <param name="owner">The owner form.</param>
    /// <returns>DialogResult</returns>
    static public System.Windows.Forms.DialogResult ShowQuestion( System.Windows.Forms.IWin32Window owner, string question, MessageBoxButtons buttons )
    {
      return Show( owner, question, "", buttons, MessageBoxIcon.Question);
    } // ShowQuestion( question, buttons )


    /// <summary>
    /// Shows the specified information in a modeless dialog box.
    /// </summary>
    /// <param name="message">message to display.</param>
    static public void ShowModelessInformation( string message, uint seconds )
    {
      ModelessMessageBoxForm form = new ModelessMessageBoxForm( message, "", seconds );
    } // ShowModelessInformation( question, buttons )


    /// <summary>
    /// Displays a message box with specified text, caption, buttons, and icon.
    /// </summary>
    /// <param name="message">The text to display in the message box. </param>
    /// <param name="caption">The text to display in the title bar of the message box.</param>
    /// <param name="buttons">One of the MessageBoxButtons that specifies which buttons to display in the message box.</param>
    /// <param name="icon">One of the MessageBoxIcon values that specifies which icon to display in the message box.</param>
    /// <returns>One of the DialogResult values.</returns>
    static public DialogResult Show( string message, string caption, MessageBoxButtons buttons, MessageBoxIcon icon )
    {
      return Show( null, message, caption, buttons, icon);
    } // Show( message, caption, buttons, icon )


    /// <summary>
    /// Displays a message box with specified text, caption, buttons, and icon.
    /// </summary>
    /// <param name="message">The text to display in the message box. </param>
    /// <param name="caption">The text to display in the title bar of the message box.</param>
    /// <param name="buttons">One of the MessageBoxButtons that specifies which buttons to display in the message box.</param>
    /// <param name="icon">One of the MessageBoxIcon values that specifies which icon to display in the message box.</param>
    /// <param name="owner">The owner form for the dialog.</param>
    /// <returns>One of the DialogResult values.</returns>
    static public DialogResult Show( System.Windows.Forms.IWin32Window owner, string message, string caption, MessageBoxButtons buttons, MessageBoxIcon icon )
    {
      // create the message box form and set properties
      MessageBoxForm dialog = new MessageBoxForm();
      
      dialog.StartPosition = ( owner == null ? FormStartPosition.CenterScreen : FormStartPosition.CenterParent );
      dialog.TopLevel = true;
      dialog.Message = message;
      dialog.Text = caption;
      dialog.Buttons = buttons;
      dialog.Icon = icon;

      
      // show the dialog and return the result
      return ( owner == null ? dialog.ShowDialog() : dialog.ShowDialog(owner) );

    } // Show( message, caption, buttons, icon )

    
    /// <summary>
    /// Writes the specified exception to an exception file
    /// </summary>
    /// <param name="e">Exception object to log.</param>
    static private void LogException( System.Exception e )
    {
      using( FileStream fstream = File.Open( m_exceptionFile, FileMode.Append ) )
        fstream.Write( ASCIIEncoding.ASCII.GetBytes( e.ToString() ), 0, ASCIIEncoding.ASCII.GetByteCount( e.ToString() ) );
    } // LogException( e )

    
    #endregion
  
  } // class MessageBox

  #endregion
	
  #region MessageBoxForm
  
  /// <summary>
	/// Message box dialog.
	/// </summary>
	internal class MessageBoxForm : Viatronix.UI.Dialog
	{

    #region constants

    /// <summary>
    /// Information message box icon.
    /// </summary>
    static private readonly Image m_informationIcon;

    
    /// <summary>
    /// Error message box icon.
    /// </summary>
    static private readonly Image m_errorIcon;

    /// <summary>
    /// Default button width
    /// </summary>
    static private readonly Size m_buttonSize = new Size( 60, 25 );

  
    /// <summary>
    /// Default spacing
    /// </summary>
    static private readonly int m_spacing = 8;

    
    /// <summary>
    /// Warning message box icon.
    /// </summary>
    static private readonly Image m_warningIcon;


    /// <summary>
    /// Question message box icon.
    /// </summary>
    static private readonly Image m_questionIcon;

    /// <summary>
    /// Represents the '\t' character in spaces
    /// </summary>
    static private readonly string m_tabString = "    ";

    #endregion
    
    #region fields

    /// <summary>
    /// Center on owner
    /// </summary>
    private bool m_centerOnParent = true;

    /// <summary>
    /// Message text.
    /// </summary>
    private string m_message = string.Empty;

    /// <summary>
    /// Buttons to display for message box.
    /// </summary>
    private MessageBoxButtons m_buttons = MessageBoxButtons.OK;

    /// <summary>
    /// Icon to display for message box.
    /// </summary>
    private MessageBoxIcon m_icon = MessageBoxIcon.None;
    
    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.Container components = null;

    /// <summary>
    /// Image icon to draw.
    /// </summary>
    private Image m_iconImage = null;

    private PointF m_messageLocation = new PointF();

    /// <summary>
    /// Contains the message strings.
    /// </summary>
    private StringCollection m_strings = new StringCollection();

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the text the message box.
    /// </summary>
    public string Message
    {
      get { return m_message; }
      set { m_message = value; }
    } // Message

    /// <summary>
    /// Gets or sets the owner control for the message box.
    /// </summary>
    public new Control Parent
    {
      get { return base.Parent; }
      set { base.Parent = value; SetLocation(); }
    } // Parent


    
    /// <summary>
    /// Gets or sets the caption for the message box.
    /// </summary>
    public string Caption
    {
      get { return this.Text; }
      set { this.Text = value; }
    } // Caption

    
    /// <summary>
    /// Gets or sets the buttons for the message box.
    /// </summary>
    public MessageBoxButtons Buttons
    {
      get { return m_buttons; }
      set { m_buttons = value; }
    } // Buttons

    
    /// <summary>
    /// Gets or sets the icon to display for message box.
    /// </summary>
    public new MessageBoxIcon Icon 
    {
      get { return m_icon; }
      set { m_icon = value; }
    } // Icon

    /// <summary>
    /// Cneter On owner
    /// </summary>
    public bool CenterOnParent
    {
      get { return m_centerOnParent; }
      set { m_centerOnParent = value; SetLocation(); }

    } // CenterOnParent


    #endregion

    #region initialization
    
    /// <summary>
    /// Loads the static resources from the resources.
    /// </summary>
    static MessageBoxForm()
    {
      // load the default images from the assembly
      Assembly assembly = Assembly.GetExecutingAssembly();

      
      m_informationIcon = new Bitmap( assembly.GetManifestResourceStream("Viatronix.UI.res.mb_information.png") );
      m_warningIcon = new Bitmap( assembly.GetManifestResourceStream("Viatronix.UI.res.mb_warning.png") );
      m_errorIcon = new Bitmap( assembly.GetManifestResourceStream("Viatronix.UI.res.mb_error.png") );
      m_questionIcon = new Bitmap( assembly.GetManifestResourceStream("Viatronix.UI.res.mb_question.png") );

    } // MessageBoxForm
    
    
    /// <summary>
		/// Initializes the MessageBoxForm with default values.
		/// </summary>
    public MessageBoxForm()
		{
			InitializeComponent();

      this.ForeColor = Color.White;
      this.Font = new Font( "Arial", 8, FontStyle.Bold );
      this.ShowInTaskbar = false;
      this.StartPosition = FormStartPosition.CenterParent;
    } // MessageBoxForm()

    
    /// <summary>
    /// Occurs before a form is displayed for the first time.
    /// </summary>
    /// <param name="args"> EventArgs containing data related to this event</param>
    protected override void OnLoad( EventArgs args )
    {
      // call down to the base class
      base.OnLoad( args );

      // set the message box's properties
      Size iconSize = SetIcon();
      Size buttonSize = SetButtons();
      Size messageSize = SetMessage();
      this.ControlBox = false;

      // messageWidth represents the icon and message size plus the spacing
      int messageWidth = 
         m_spacing +  messageSize.Width + m_spacing;

      // are we drawing an icon?
      if( iconSize.Width > 0 )
        messageWidth += m_spacing + m_iconImage.Width;

      // buttonsWidth represents the buttons size plus spacing
      int buttonsWidth = m_spacing + buttonSize.Width + m_spacing;

      // get the maximum width
      int width = Math.Max( messageWidth, buttonsWidth );

      // height equals the maximum of the message and icon plus the button height and spacing
      int height = 
        m_spacing + Math.Max( iconSize.Height, messageSize.Height ) +
        m_spacing + buttonSize.Height + m_spacing; //+ 
        //this.Offset.Y;

      // set the overall size of the message box
      SetSize( width, height );

      // layout the buttons
      int y = this.Height - Dialog.FrameHeight - m_spacing - buttonSize.Height;
      int x = ( this.Width - buttonSize.Width ) / 2;
      foreach( Control control in this.Controls )
      {
        if( control is SystemMenu )
          continue;

        // set the location for the control
        control.Location = new Point( x, y );
        
        // next x position
        x += m_spacing + control.Width;
      } //foreach( Control control in this.Controls )

      // figure out the start x location for message display
      if( m_icon != MessageBoxIcon.None )
      {
        // fix the x position
        float fx = ( float )( this.Offset.X + m_spacing + m_iconImage.Width + m_spacing );
        
        // need to find the y position
        float fy = 0.0F;
        
        // is the icon height greather than the the message text height?
        if( iconSize.Height > messageSize.Height )
        {
          // center it vertically agaist icon image
          fy = 
            ( float ) this.Offset.Y + ( float ) m_spacing + ( float )( iconSize.Height - messageSize.Height  ) / 2.0F;
        }
        else
          fy = ( float )( this.Offset.Y + m_spacing );
        
        m_messageLocation = new PointF( fx, fy );
      } // if( m_icon != MessageBoxIcon.None )
      else
        m_messageLocation = new PointF( ( float )( this.Offset.X + m_spacing ), ( float )( this.Offset.Y + m_spacing ) );

      // manually center the dialog on the owner form
      //ManuallyCenterOnParent( m_ownerForm );

    } // OnLoad( args )

    
    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    /// <param name="disposing"></param>
    protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if( components != null )
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		} // Dispose( disposing )

    
    #endregion

    #region setup

    private void SetLocation()
    {
      if ( base.Parent != null && m_centerOnParent )
      {
        this.StartPosition = FormStartPosition.Manual;

        int newX = base.Parent.Location.X + (( base.Parent.Width - this.Width ) / 2 );
        int newY = base.Parent.Location.Y + (( base.Parent.Height - this.Height ) / 2 );
        this.Location = new Point( newX, newY );

      }
    } // SetLocation()



    /// <summary>
    /// Sets the message box's icon image.
    /// </summary>
    private Size SetIcon()
    { 
      // information icon
      if( m_icon == MessageBoxIcon.Asterisk  || m_icon == MessageBoxIcon.Information )
        m_iconImage = m_informationIcon;
      
      // question icon
      else if( m_icon == MessageBoxIcon.Question )
        m_iconImage = m_questionIcon;
      
      // error icon
      else if( m_icon == MessageBoxIcon.Error || m_icon == MessageBoxIcon.Hand || m_icon == MessageBoxIcon.Stop )
        m_iconImage = m_errorIcon;
      
      // waring icon
      else if( m_icon == MessageBoxIcon.Exclamation || m_icon == MessageBoxIcon.Warning  )
        m_iconImage = m_warningIcon;
      
      // all others
      else
        return new Size( 0, 0 );

      return new Size( m_iconImage.Width, m_iconImage.Height );
    } // SetIcon()


    /// <summary>
    /// Sets the message box's message.
    /// </summary>
    private Size SetMessage()
    {
      // replace all tabs with spaces
      m_message.Replace( "\t", m_tabString );

      // does the message contain multiple lines?
      if( m_message.IndexOf( "\n" ) != -1 )
        m_strings.AddRange( m_message.Split( '\n' ) );
      else
        m_strings.Add( m_message );

      // width and height of the message strings
      int width = 0;
      int height = 0;
      
      // create a graphics object based on this window
      using( Graphics gfx = Graphics.FromHwnd( this.Handle ) )
      {
        SizeF emptyLineSize = gfx.MeasureString( ".", this.Font );

        foreach( string s in m_strings )
        {
          // get the size of the strings in pixels
          SizeF size = gfx.MeasureString( s, this.Font );
          
          // find the maximum width of the message strings
          width = Math.Max( width, ( int )( size.Width + 0.5F ) );
          
          // accummulate the height
          // make sure to account for height of empty lines as well
          height += ( int ) ( Math.Max( size.Height, emptyLineSize.Height ) + 0.5F );
        } // foreach( string s in m_string )        
      } // using( Graphics gfx = Graphics.FromHwnd( this.Handle ) )

      return new Size( width, height );
    } // SetMessage()
    
    
    /// <summary>
    /// Sets the message box buttons for the message box.
    /// </summary>
    private Size SetButtons()
    {
      // number of buttons
      int m_numberOfButtons = 0;

      // MessageBoxButtons.OK
      if( m_buttons == MessageBoxButtons.OK )
      {
        AddButton( "OK", DialogResult.OK ).Focus();
        m_numberOfButtons = 1;
      } // MessageBoxButtons.OK
      
      
      // MessageBoxButtons.OkCancel
      else if( m_buttons == MessageBoxButtons.OKCancel)
      {
        AddButton( "OK",     DialogResult.OK ).Focus();
        this.CancelButton = AddButton( "Cancel", DialogResult.Cancel );
        m_numberOfButtons = 2;
      } // MessageBoxButtons.OkCancel

      
      // MessageBoxButtons.YesNo
      else if( m_buttons == MessageBoxButtons.YesNo )
      {
        AddButton( "Yes", DialogResult.Yes ).Focus();
        this.CancelButton = AddButton( "No",  DialogResult.No );
        m_numberOfButtons = 2;
      } // MessageBoxButtons.YesNo
      
      
      // MessageBoxButtons.YesNoCancel
      else if( m_buttons == MessageBoxButtons.YesNoCancel )
      {
        AddButton( "Yes",    DialogResult.Yes ).Focus();
        AddButton( "No",     DialogResult.No );
        this.CancelButton = AddButton( "Cancel", DialogResult.Cancel );
        m_numberOfButtons = 3;
      } // MessageBoxButtons.YesNoCancel
      
      
      // MessageBoxButtons.RetryCancel
      else if( m_buttons == MessageBoxButtons.RetryCancel )
      {
        AddButton( "Retry",  DialogResult.Retry ).Focus();
        this.CancelButton = AddButton( "Cancel", DialogResult.Cancel );
        m_numberOfButtons = 2;
      } // MessageBoxButtons.RetryCancel
      
      
      // MessageBoxButtons.AbortRetryIgnore
      else if( m_buttons == MessageBoxButtons.AbortRetryIgnore )
      {
        AddButton( "Abort",  DialogResult.Abort ).Focus();
        AddButton( "Retry",  DialogResult.Retry );
        this.CancelButton = AddButton( "Ignore", DialogResult.Cancel );
        m_numberOfButtons = 3;
      } // MessageBoxButtons.AbortRetryIgnore

  
      // figure out how much room the buttons need
      int width = ( m_numberOfButtons * m_buttonSize.Width ) + ( ( m_numberOfButtons - 1 ) * m_spacing );
      int height = m_buttonSize.Height;

      // return the size of the buttons
      return new Size( width, height );
    } // SetButtons()

   
    /// <summary>
    /// Creates a push button with the specified text and result.
    /// </summary>
    /// <param name="text">Button Text.</param>
    /// <param name="result">Button DialogResult.</param>
    /// <returns>Button object.</returns>
    private Viatronix.UI.Button AddButton( string text, DialogResult result )
    {
      // create the button
      Viatronix.UI.Button button = new Viatronix.UI.Button();

      // set the buttons properties
      button.Text = text;
      button.DialogResult = result;
      button.Size = m_buttonSize;
      button.Font = new Font( "Arial", 8 );

      // add the button to the form
      this.Controls.Add( button );

      // return the button
      return button;
    } // AddButton( text, result )
    
    
    #endregion

    #region drawing
      
    /// <summary>
    /// Event handler for the Paint event.
    /// </summary>
    /// <param name="args">PaintEventArgs containing data related to this event.</param>
    protected override void OnPaint( PaintEventArgs args )
    {
      // call down to the owner class
      base.OnPaint( args );
      
      // cache the grpahics object
      Graphics gfx = args.Graphics;

      // set to the highest quality settings
      gfx.SmoothingMode = SmoothingMode.HighQuality;
      gfx.TextRenderingHint = TextRenderingHint.ClearTypeGridFit;
      gfx.InterpolationMode = InterpolationMode.HighQualityBicubic;

      // do we need to draw the icon
      if( m_iconImage != null )
        gfx.DrawImage( m_iconImage, m_spacing + this.Offset.X, m_spacing + this.Offset.Y );

      // draw the message
      PointF position = m_messageLocation;
      using( SolidBrush brush = new SolidBrush( this.ForeColor ) )
      {
        // draw each string
        foreach( string s in m_strings )
        {
          gfx.DrawString( s, this.Font, brush, position );

          // offset the text position
          position.Y += ( float ) this.Font.Height;
        } // foreach( string s in m_strings )
          
      } // using( SolidBrush brush = new SolidBrush( this.ForeColor ) )

    } // OnPaint( args )

    
    #endregion

		#region windows form designer generated code
		
    /// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
      // 
      // MessageBoxForm
      // 
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.ClientSize = new System.Drawing.Size(206, 87);
      this.Name = "MessageBoxForm";
      this.Text = "MessageBoxForm";

    }
		
    #endregion
	
  } // class MessageBoxForm

  #endregion

  #region ModelessMessageBoxForm
  
  /// <summary>
  /// A modeless message box that appears for just one second
  /// </summary>
  public class ModelessMessageBoxForm
  {
    #region fields
    
    /// <summary>
    /// The timer that will initiate closing the form
    /// </summary>
    private System.Threading.Timer m_timer = null;

    /// <summary>
    /// A reference to the dialog so we can close it later
    /// </summary>
    private MessageBoxForm m_dialog = null;

    /// <summary>
    /// A collection of the active forms
    /// </summary>
    private static ArrayList m_activeModelessForms = new ArrayList();

    #endregion



    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="message">the message you want to display</param>
    /// <param name="caption">the caption in the title of the form</param>
    /// <param name="seconds">the number of second to display the form</param>
    public ModelessMessageBoxForm( string message, string caption, uint seconds )
    {

      // create the message box form and set properties
      m_dialog = new MessageBoxForm();
      m_dialog.Message = message;
      m_dialog.Text = caption;
      m_dialog.Buttons = MessageBoxButtons.OK;
      m_dialog.Icon = MessageBoxIcon.Information;
      m_dialog.StartPosition = FormStartPosition.CenterScreen;

      // show the dialog and return the result
      m_dialog.Show();
      m_dialog.BringToFront();

      // Create the delegate that invokes methods for the timer.
      System.Threading.TimerCallback timerDelegate = new System.Threading.TimerCallback( CheckStatus );

      // set the timer to fire in the near future
      m_timer = new System.Threading.Timer( timerDelegate, this, 1000 * seconds, 1000 * seconds );

      // keep track of these so that they are not GC'd
      m_activeModelessForms.Add( this );

    } // ModelessMessageBoxForm( message, caption )

    /// <summary>
    /// Check the status of the timer and close the dialog
    /// </summary>
    /// <param name="instance">a reference to the modeless message box form</param>
    private static void CheckStatus( Object instance )
    {

      // retreive the form
      ModelessMessageBoxForm form = (ModelessMessageBoxForm) instance;

      // close the dialog
      form.m_dialog.Close();

      // stop the timer
      form.m_timer.Dispose();

      // remove from list
      m_activeModelessForms.Remove( form );

    } // CheckStatus( Object instance )

  } // class ModelessMessageBoxForm

  #endregion
 
} // namespace Viatronix.UI

#region revision history

// $Log: MessageBox.cs,v $
// Revision 1.14  2006/03/17 19:19:27  romy
// added showmessage with warning
//
// Revision 1.13  2005/11/23 11:50:25  mkontak
// Modified MessageBox to allow for an owner for better control
//
// Revision 1.12  2005/11/21 22:57:25  gdavidson
// Added owner form as a parameter to ShowInfo and ShowQuestion methods
//
// Revision 1.11  2005/11/18 21:08:03  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.10.2.5  2005/08/08 19:37:31  geconomos
// moved SoftwareAgreementForm to it's own file
//
// Revision 1.10.2.4  2005/08/08 18:21:15  frank
// Issue #4351: Centered message box on desktop if no owner is supplied
//
// Revision 1.10.2.3  2005/08/08 12:43:12  frank
// Issue #4343: added a software license dialog
//
// Revision 1.10.2.2  2005/08/05 15:17:18  frank
// Issue #4338: Hid cancel button when license fails.
//
// Revision 1.10.2.1  2005/08/04 15:11:59  frank
// Issue #4340: Fixed problem with blank lines of text not increasing the height of the message box which resulted in text being obscured by the buttons.
//
// Revision 1.10  2005/03/08 19:11:06  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.9  2005/02/04 20:14:25  gdavidson
// Added focus to the message box buttons.
//
// Revision 1.8.2.2  2005/02/11 15:40:09  gdavidson
// Added code to center the message box when called from outside the gui's thread.
//
// Revision 1.8.2.1  2005/02/04 19:52:14  gdavidson
// Added focus to the message box buttons.
//
// Revision 1.8  2004/12/10 17:11:36  geconomos
// removed code that logs exception to file
//
// Revision 1.7  2004/11/19 22:30:27  gdavidson
// Added new skin.
//
// Revision 1.6  2004/11/19 16:12:00  gdavidson
// Reduced the button height.
//
// Revision 1.5  2004/10/13 16:13:38  geconomos
// new graphics
//
// Revision 1.4  2004/10/11 23:59:53  geconomos
// Initial "reskinning"
//
// Revision 1.3  2004/09/10 18:16:32  mkontak
// Using the SystemSettings in the Utilties::Registry
//
// Revision 1.2  2004/03/24 21:32:55  frank
// removed hard-coded paths to installation, log, license, and config directories
//
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.17  2003/12/11 21:23:09  frank
// Working on the modeless message box
//
// Revision 1.16  2003/12/11 16:00:46  frank
// Gave the modeless message box an adjustable lifetime.
//
// Revision 1.15  2003/12/10 13:55:52  frank
// Added a modeless message box form
//
// Revision 1.14  2003/12/09 16:07:45  frank
// Added overloaded question function
//
// Revision 1.13  2003/09/05 14:58:32  michael
// merged the branch "XP1_1" back into the trunc
//
// Revision 1.12  2003/08/20 14:23:25  michael
// added "mailto:"
//
// Revision 1.11  2003/05/25 18:12:07  michael
// .net --> .com
//
// Revision 1.10  2003/04/27 23:51:48  geconomos
// More code cleanup.
//
// Revision 1.3  2003/04/25 21:06:44  geconomos
// Endless code reviews.
//
// Revision 1.2  2003/04/17 19:41:19  geconomos
// Some more code cleanup.
//
// Revision 1.1  2003/04/14 13:07:56  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/MessageBox.cs,v 1.14 2006/03/17 19:19:27 romy Exp $
// $Id: MessageBox.cs,v 1.14 2006/03/17 19:19:27 romy Exp $

#endregion