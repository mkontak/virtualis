// $Id: Rebar.cs,v 1.8 2005/03/08 19:11:06 frank Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos ( mailto:george@viatronix.com )

using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.ComponentModel;
using System.ComponentModel.Design;
using System.Collections;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.Windows.Forms.Design;
using Win32;

namespace Viatronix.UI
{

  #region Rebar

  /// <summary>
  /// Represents a standard windows rebar control.
  /// </summary>
  [ Designer( typeof(Viatronix.UI.RebarDesigner ) ) ]
  public class Rebar : System.Windows.Forms.Control
  {
  
    #region fields
 
    /// <summary>
    /// left-part toolbar image
    /// </summary>
    private static Bitmap m_toolbarLeft = null;
    
    /// <summary>
    /// middle-part toolbar image
    /// </summary>
    private static Bitmap m_toolbarMiddle = null;
    
    /// <summary>
    /// right-part toolbar image
    /// </summary>
    private static Bitmap m_toolbarRight = null;

    /// <summary>
    /// rebar band collection
    /// </summary>
    private RebarBandCollection m_bands;

    /// <summary>
    /// contained rebar control
    /// </summary>
    private NativeRebar m_rebar = null;
    
    /// <summary>
    /// indicates if the rebar control is being resized
    /// </summary>
    private bool m_resizing = false;
    
    #endregion  

    #region properties
     
    /// <summary>
    /// Gets the bands collection
    /// </summary>
    [ DesignerSerializationVisibility( DesignerSerializationVisibility.Content ), Editor( typeof( Viatronix.UI.RebarBandCollectionEditor ), typeof( System.Drawing.Design.UITypeEditor ) ),  NotifyParentProperty( true ) ]
    public RebarBandCollection Bands
    {
      get
      {
        return m_bands;
      }
    } // Bands

    
    /// <summary>
    /// Gets the internal rebar control.
    /// </summary>
    internal NativeRebar InternalRebar
    {
      get
      {
        return m_rebar;
      }
    } // InternalRebar

    
    /// <summary>
    /// Get the HWND of the internal rebar control
    /// </summary>
    internal IntPtr RebarHwnd
    {
      get
      {
        if( m_rebar == null )
          return IntPtr.Zero;
        else
          return m_rebar.Handle;
      }
    } // RebarHwnd
    
    #endregion

    #region initialization

    /// <summary>
    /// static initialization
    /// </summary>
    static Rebar()
    {
      Assembly assembly = Assembly.GetExecutingAssembly();

      m_toolbarLeft = new Bitmap( assembly.GetManifestResourceStream("Viatronix.UI.res.toolbar-left.png") );
      m_toolbarMiddle = new Bitmap( assembly.GetManifestResourceStream("Viatronix.UI.res.toolbar-middle.png") );
      m_toolbarRight = new Bitmap( assembly.GetManifestResourceStream("Viatronix.UI.res.toolbar-right.png") );
    } // Rebar()
  

    /// <summary>
    /// Initializes a Rebar instance with default values.
    /// </summary>
    public Rebar()
    {
      SetStyle( ControlStyles.StandardClick, true );
      SetStyle( ControlStyles.StandardDoubleClick,true );
			
      SetStyle( ControlStyles.DoubleBuffer, true );
      SetStyle( ControlStyles.UserPaint, true );
      SetStyle( ControlStyles.AllPaintingInWmPaint, true );

      InitializeComponent();

      m_bands = new RebarBandCollection( this );
    } // Rebar()

    /// <summary>
    /// Disposes the current instance
    /// </summary>
    /// <param name="disposing">True to dispose of managed as well</param>
    protected override void Dispose( bool disposing )
    {
      if( m_rebar != null )
        m_rebar.DestroyHandle();
     
      base.Dispose( disposing );
    } // Dispose( bool disposing )

    #endregion
    
    #region methods
    
    /// <summary>
    /// Returns the rebar band that contains the specified point
    /// </summary>
    /// <param name="point">Point to check.</param>
    /// <returns>Band containing point; otherwise null</returns>
    internal RebarBand BandHitTest( Point point )
    {
      foreach( RebarBand band in m_bands )
      {
        if( band.Bounds.Contains( point ) )
          return band;
      }
      return null;
    } // BandHitTest( Point point )

    
    /// <summary>
    /// Called when the control is being created.
    /// </summary>
    protected override void OnCreateControl()
    {
      base.OnCreateControl();

      // set the style for the contained rebar control
      int style = ( int )(
        WindowStyles.WS_BORDER          | 
        WindowStyles.WS_CHILD           | 
        WindowStyles.WS_CLIPCHILDREN    |
        WindowStyles.WS_CLIPSIBLINGS    | 
        WindowStyles.CCS_NODIVIDER      | 
        WindowStyles.CCS_NOPARENTALIGN  |
        WindowStyles.RBS_AUTOSIZE       |
        WindowStyles.CCS_TOP            |
        WindowStyles.RBS_VARHEIGHT      |
        WindowStyles.WS_VISIBLE );

			// create the rebar control
      m_rebar = new NativeRebar( User32.CreateWindowEx( 0U, "ReBarWindow32", null, ( uint )style, 0, 0,this.Width, this.Height, this.Handle, ( IntPtr ) 1, IntPtr.Zero, 0 ) );

      // disable themes for the control
      if( Utilities.WinXpOrGreater )
        Win32.Themes.SetWindowTheme( m_rebar.Handle, string.Empty, string.Empty );
      
      //User32.SetWindowLongPtr( m_rebar.Handle, (int) WindowLongConstants.GWL_EXSTYLE, (IntPtr)0x80 );
      
      // wire up so that we can notified when the contained rebar position changes
      m_rebar.WindowPosChanging += new EventHandler( OnWindowPosChanging );
      
      // add all band to the contained control
      foreach( RebarBand band in m_bands )
        band.CreateBand();
    } // OnCreateControl()

    
    /// <summary>
    /// Called when the control is being resized.
    /// </summary>
    /// <param name="args">EventArgs containg data related to this event.</param>
    protected override void OnResize( System.EventArgs args )    
    {
      bool resizing = m_resizing;
      
      // indicate that we are resizing
      m_resizing = true;
      
      // has the internal rebar control been created?
      if( m_rebar != null )
      {
        // get the height of the contained rebar control
        int height = m_rebar.BarHeight;
        
        // do we need to adjust the height?
        if( this.Height != height )
          this.Height = height;
        
        // do we need to adjust the width?
        if( this.Width != m_rebar.BarWidth && this.Width != 0 )
        {
          // reposition and resize the contained rebar control
          if( User32.MoveWindow( m_rebar.Handle, 0, 0, this.Width, height, true ) != 0)
            this.Refresh();
        }
      } // if( m_rebar != null )
      
      base.OnResize( args );
      
      m_resizing = resizing;
    } // OnResize( System.EventArgs args )    


    /// <summary>
    /// Control's WndProc.
    /// </summary>
    /// <param name="m">Window message</param>
    protected override void WndProc( ref Message m )
    {
      // don't process the WM_ERASEBKGND message
      if( m.Msg == (int) Win32.Msgs.WM_ERASEBKGND )
        return;

      base.WndProc( ref m );
      
      // we are providing custom draw services for the rebar control and contained toolbars
      if( m.Msg == (int) Win32.Msgs.WM_NOTIFY )
      {        
        // get the message header structure for this mesage
        Win32.NMHDR nmhdr = ( Win32.NMHDR ) m.GetLParam( typeof( Win32.NMHDR ) );

        // is it a NM_CUSTOMDRAW notification?
        if( nmhdr.code == ( int )Win32.WindowsNotifyConstants.NM_CUSTOMDRAW )
        {
          if( this.Width <= 0 || this.Height <= 0 )
            return;

          // get the custom draw structure assocaited with this notification
          Win32.NMCUSTOMDRAW nmdraw = ( Win32.NMCUSTOMDRAW ) m.GetLParam( typeof( Win32.NMCUSTOMDRAW ) );

          // is this message for the contained rebar control?
          if( nmhdr.hwndFrom == m_rebar.Handle )
          {
   
            // double buffering
            Bitmap buffer = new Bitmap( this.Width, this.Height, PixelFormat.Format24bppRgb );

            // create a graphics object based the image
            using( Graphics gfx = Graphics.FromImage( buffer ) )
            {
              // clear out the background of the rebar control
              gfx.Clear( Color.FromArgb( 239, 239, 239 ) );
              
              // draw each band
              foreach( RebarBand band in m_bands )
                DrawBand( gfx, band );
            }

            // render the buffer
            using( Graphics gfx = Graphics.FromHdc( nmdraw.hdc ) )
              gfx.DrawImageUnscaled( buffer, 0, 0 );

            // dispose of the buffer
            buffer.Dispose();

            // we are going to draw the entire rebar
            m.Result = new IntPtr(  (int)Win32.CDRF.CDRF_SKIPDEFAULT );
          } // if( nmhdr.hwndFrom == m_rebar.Handle )
          else
          {
            // double buffering
            Bitmap buffer = new Bitmap( this.Width, this.Height, PixelFormat.Format24bppRgb );

            // get the instance of the control this message is for
            Control control = Control.FromChildHandle( nmhdr.hwndFrom );

            // is the control a toolbar?
            if( control is ToolBar )
            {
              // draw the toolbar into the buffer
              using( Graphics gfx = Graphics.FromImage( buffer ) )
                DrawToolBar( gfx, ( ToolBar ) control );

              // render the buffer
              using( Graphics gfx = Graphics.FromHdc( nmdraw.hdc ) )
                gfx.DrawImageUnscaled( buffer, 0, 0 );
              
              // dispose of the buffer
              buffer.Dispose();
              
              // we are going to draw the entire rebar
              m.Result = new IntPtr(  (int)Win32.CDRF.CDRF_SKIPDEFAULT );
            } // if( control is ToolBar )
            else
            {
              // otherwise do the default processing 
              m.Result = new IntPtr(  (int)Win32.CDRF.CDRF_DODEFAULT );
            }
          } // else
        } // if( nmhdr.code == ( int )Win32.WindowsNotifyConstants.NM_CUSTOMDRAW )
      } // if( m.Msg == (int) Win32.Msgs.WM_NOTIFY )
    } // WndProc( ref Message m )

    
    /// <summary>
    /// Draws the specified RebarBand instance
    /// </summary>
    /// <param name="gfx">Graphics object to drawinto.</param>
    /// <param name="band">RebarBand to draw</param>
    private void DrawBand( Graphics gfx, RebarBand band )
    {
      // adjust the bounds of the band
      Rectangle bounds = band.Bounds;
      bounds.Inflate( 0, -2 );

      // draw the left-part if the band
      gfx.DrawImage( m_toolbarLeft, bounds.X, bounds.Y, m_toolbarLeft.Width, bounds.Height );

      // draw the right-part of the band
      int x = bounds.X + band.Width - m_toolbarRight.Width;
      gfx.DrawImage( m_toolbarRight, x, bounds.Y, m_toolbarRight.Width, bounds.Height );
    } // DrawBand( Graphics gfx, RebarBand band )
  
    
    /// <summary>
    /// Draws the specified ToolBar instance.
    /// </summary>
    /// <param name="gfx">Graphics instance to draw into.</param>
    /// <param name="toolbar">ToolBar instance to draw.</param>
    private void DrawToolBar( Graphics gfx, ToolBar toolbar )
    {

      // draw the background for the toolbar
      for( int index = 0; index < toolbar.Width; index += m_toolbarMiddle.Width )
        gfx.DrawImage( m_toolbarMiddle, index, 0,m_toolbarMiddle.Width, toolbar.Height  );

      // get the current mouse position relative to the toolbar
      Point pos = toolbar.PointToClient( Control.MousePosition );

      foreach( ToolBarButton button in toolbar.Buttons )
      {
        // does the button have an image?
        if( button.ImageIndex != -1 )
        {
          Image image = toolbar.ImageList.Images[ button.ImageIndex ];

          // adjust the button's bounds
          Rectangle bounds = button.Rectangle;
          bounds.Inflate( -2, -2 );
          
          // is the button in the pushed state
          if( button.Pushed )
          {
            gfx.FillRectangle( Theme.SelectedBrush, bounds );
            gfx.DrawRectangle( Theme.SelectedPen, bounds );
          }
          // is the mouse over the button?
          else if( bounds.Contains( pos ) )
          {
            using( Pen pen = new Pen( Color.White ) )
              gfx.DrawRectangle( pen, bounds );
          }

          // adjust the bounds for the image
          bounds.Inflate( -2, -2 );

          // draw the button's image
          gfx.DrawImageUnscaled( image, bounds.X, bounds.Y, image.Width, image.Height );
        
        } // if( button.ImageIndex != -1 )
      } // foreach( ToolBarButton button in toolbar.Buttons )
    } // DrawToolBar( Graphics gfx, ToolBar toolbar )
   
    
    /// <summary>
    /// Handler for the WindowPosChanging event for the internal rebar control.
    /// </summary>
    /// <param name="sender">Sender of this event.</param>
    /// <param name="args">EventArgs containg data related to this event</param>
    private void OnWindowPosChanging( object sender, EventArgs args )
    {
      // are we already resizing?
      if( m_resizing ) 
        return;
      
      // do we need to resize the parent
      if( this.Height != m_rebar.BarHeight )
        this.OnResize( EventArgs.Empty );
    } // OnWindowPosChanging( object sender, EventArgs args )
   
    #endregion
    
    #region NativeRebar

    /// <summary>
    /// Contained rebar control.
    /// </summary>
    internal sealed class NativeRebar: NativeWindow
    {

      #region events
      
      /// <summary>
      /// raised when the control receives the WM_WINDOWPOSCHANGING message
      /// </summary>
      public event EventHandler WindowPosChanging;

      #endregion

      #region properties

      /// <summary>
      /// Gets the height of the rebar control
      /// </summary>
      internal int BarHeight
      {
        get
        {
          RECT rect = new RECT();
          User32.GetWindowRect( this.Handle, ref rect );
          return rect.bottom - rect.top;
        }
      } // BarHeight


      /// <summary>
      /// Gets the width of the rebar control
      /// </summary>
      internal int BarWidth
      {
        get
        {
          RECT rect = new RECT();
          User32.GetWindowRect(Handle, ref rect);
          return rect.right - rect.left;
        }
      } // BarWidth

      #endregion
      
      #region initialization

      /// <summary>
      /// Initializes the NativeRebar instance from the specified HWND
      /// </summary>
      /// <param name="handle">HWND assigned to this control</param>
      internal NativeRebar( IntPtr handle )
      {
        base.AssignHandle( handle );
      } // NativeRebar( IntPtr handle )

      #endregion

      #region methods
      
      /// <summary>
      /// Control WndProc method
      /// </summary>
      /// <param name="m">Window message</param>
      protected override void WndProc( ref System.Windows.Forms.Message m )
      {
        if( m.Msg == ( int ) Msgs.WM_ERASEBKGND )
          return;

        if( m.Msg == ( int ) Msgs.WM_WINDOWPOSCHANGING )
        {
          if( WindowPosChanging != null )
            WindowPosChanging( this, EventArgs.Empty );
        }


        this.DefWndProc( ref m );				
      } // WndProc( ref System.Windows.Forms.Message m )
 
      #endregion

    } // class NativeRebar
    
    #endregion

    #region component designer generated code
   
    /// <summary>
    /// Required method for Designer support - do not modify 
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      // 
      // ReBarWrapper
      // 
      this.Name = "ReBarWrapper";
      this.Size = new System.Drawing.Size(464, 64);
    } // InitializeComponent()
   
    #endregion

  } // class Rebar

  #endregion

  #region RebarDesigner

  /// <summary>
  /// The designer for the rebar control
  /// </summary>
  public class RebarDesigner: System.Windows.Forms.Design.ParentControlDesigner
  {

    #region fields
    
    /// <summary>
    /// rebar control 
    /// </summary>
    private Rebar m_rebar;
    
    /// <summary>
    /// designer host
    /// </summary>
    private IDesignerHost m_host;
    
    /// <summary>
    /// selection service
    /// </summary>
    private ISelectionService m_selectionService;
    
    /// <summary>
    /// verb collection
    /// </summary>
    private DesignerVerbCollection m_verbs;

    #endregion

    #region properties
    
    /// <summary>
    /// Gets the components associated with the rebar control
    /// </summary>
    public override ICollection AssociatedComponents
    {
      get
      {	
        ArrayList components = new ArrayList();
        
        // add each band to the components
        foreach( RebarBand band in m_rebar.Bands )
        {
          components.Add( band );
          
          // if the band has an associated child control add tat as well
          if( band.Child != null )
            components.Add( band.Child );
        } // foreach( RebarBand band in m_rebar.Bands )
        return components;
      } // get
    
    } // AssociatedComponents

    
    /// <summary>
    /// Gets the verb collection.
    /// </summary>
    public override DesignerVerbCollection Verbs
    {
      get { return m_verbs; }
    } // Verbs

    
    /// <summary>
    /// Gets if the drawh rectangle is enabled.
    /// </summary>
    protected override bool EnableDragRect
    {
      get { return m_rebar.Dock == DockStyle.None; }
    } // EnableDragRect

   
    /// <summary>
    /// Gets the selction rules for the designer
    /// </summary>
    public override SelectionRules SelectionRules
    {
      get 
      {
        if( m_rebar != null )
          return SelectionRules.LeftSizeable|SelectionRules.RightSizeable| SelectionRules.Moveable | SelectionRules.Visible;
        else
          return SelectionRules.AllSizeable | SelectionRules.Moveable | SelectionRules.Visible;
      }
    } // SelectionRules

    #endregion

    #region initialization

    /// <summary>
    /// Initializes the RebaDesigner with default values.
    /// </summary>
    public RebarDesigner()
    {
      // craete and initialize the verb collection
      m_verbs = new DesignerVerbCollection();
      m_verbs.Add(new DesignerVerb( "Add Band", new EventHandler( OnAddBandHandler ) ) );
      m_verbs[ 0 ].Enabled = false;
    } // RebarDesigner()

    #endregion
   
    #region methods

    /// <summary>
    /// Called when the designer is being initialized
    /// </summary>
    /// <param name="component"></param>
    public override void Initialize( IComponent component )
    {
      base.Initialize( component );

      // is the control as Rebar?
      if( Control is Rebar )
      {
        m_rebar = ( Rebar ) Control;
        m_host = ( IDesignerHost ) GetService( typeof( IDesignerHost ) );
        m_selectionService = ( ISelectionService ) GetService( typeof( ISelectionService ) );
      }
    } // Initialize( IComponent component )


    /// <summary>
    /// Handler for the "Add" verb
    /// </summary>
    /// <param name="sender">Sender of this event</param>
    /// <param name="args">EventArgs containg data related to this event</param>
    public void OnAddBandHandler( object sender, EventArgs args )
    {
      // create a new band
      RebarBand band = ( RebarBand ) m_host.CreateComponent( typeof( RebarBand ) );
      
      // add it to the rebar control
      m_rebar.Bands.Add( band );
    } // OnAddBandHandler( object sender, EventArgs args )

    
    /// <summary>
    /// WndProc method
    /// </summary>
    /// <param name="m">Window's message</param>
    protected override void WndProc( ref System.Windows.Forms.Message m )
    {
      // has the rebar control been created?
      if( m_rebar.Created )
      {
        if( m.Msg == (int) Msgs.WM_LBUTTONDBLCLK )
        {
          // select the rebar control that was double clicked
          RebarBand band = m_rebar.BandHitTest( new Point(m.LParam.ToInt32() ) );
          if( band != null )
            m_selectionService.SetSelectedComponents( new Component[] { band } );
          return;
        } // if( m.Msg == (int) Msgs.WM_LBUTTONDBLCLK )
      } // if( m_rebar.Created )
      
      base.WndProc(ref m);
      
      // no  rectangle selection when you tinker with the bands
      if( m.Msg == (int) Msgs.WM_LBUTTONDOWN )
        User32.SendMessage( m_rebar.Handle, (int) Msgs.WM_LBUTTONUP, m.WParam, m.LParam );
    } // WndProc( ref System.Windows.Forms.Message m )
   
    #endregion
 
  } // class RebarDesigner


  #endregion

  #region RebarBand

  /// <summary>
  /// the gripper settings
  /// </summary>
  public enum GripperSettings{ Always, Auto, Never };

  [ ToolboxItem( false ) ]
  public class RebarBand: Component, IDisposable
  {

    #region fields
		
    /// <summary>
    /// the collection of bands
    /// </summary>
    private RebarBandCollection m_bands;
    
    /// <summary>
    /// if allowed to be vertical
    /// </summary>
    private bool m_allowVertical = true;
    
    /// <summary>
    /// the background color
    /// </summary>
    private Color m_backColor;
    
    /// <summary>
    /// the caption
    /// </summary>
    private string m_caption = "";
    
    /// <summary>
    /// if created
    /// </summary>
    private bool m_created = false;
    
    /// <summary>
    /// the child control
    /// </summary>
    private Control m_child = null;

    /// <summary>
    /// if disposed
    /// </summary>
    protected bool m_disposed = false;

    /// <summary>
    /// if should emboss the pictures
    /// </summary>
    private bool m_embossPicture = true;
    
    /// <summary>
    /// if the background is fixed
    /// </summary>
    private bool m_fixedBackground = true;
    
    /// <summary>
    /// if the band has a fixed size
    /// </summary>
    private bool m_fixedSize = false;
    
    /// <summary>
    /// the foreground color
    /// </summary>
    private Color m_foreColor;

    /// <summary>
    /// the grip settings
    /// </summary>
    private GripperSettings m_gripSettings = GripperSettings.Auto;
    
    /// <summary>
    /// the header
    /// </summary>
    private int m_header = -1;
    
    /// <summary>
    /// the id
    /// </summary>
    private int m_id = -1;
    
    /// <summary>
    /// the ideal width
    /// </summary>
    private int m_idealWidth = 0;
    
    /// <summary>
    /// the image
    /// </summary>
    private int m_image = -1;
    
    /// <summary>
    /// the integral
    /// </summary>
    private int m_integral = 1;
    
    /// <summary>
    /// the key
    /// </summary>
    private string m_key = "";
    
    /// <summary>
    /// the maximum height
    /// </summary>
    private int m_maxHeight = 0;
    
    /// <summary>
    /// the minimum height
    /// </summary>
    private int m_minHeight = 24;
    
    /// <summary>
    /// the minimum width
    /// </summary>
    private int m_minWidth = 24;
    
    /// <summary>
    /// if this band starts a new row
    /// </summary>
    private bool m_newRow = true;

    /// <summary>
    /// the background image
    /// </summary>
    private Bitmap m_backgroundImage = null;
    
    /// <summary>
    /// a handle to the picture
    /// </summary>
    private IntPtr m_pictureHandle = IntPtr.Zero;
    
    /// <summary>
    /// if should show a caption
    /// </summary>
    private bool m_showCaption = true;
    
    /// <summary>
    /// if should show an icon
    /// </summary>
    private bool m_showIcon = false;
    
    /// <summary>
    /// the tag object
    /// </summary>
    private object m_tag = null;
    
    /// <summary>
    /// use coolbar colors?
    /// </summary>
    private bool m_useCoolbarColors = false;
    
    /// <summary>
    /// use coolbar picture?
    /// </summary>
    private bool m_useCoolbarPicture = false;

    /// <summary>
    /// if visible
    /// </summary>
    private bool m_visible = true;

    #endregion

    #region properties

    /// <summary>
    /// gets or sets the allow vertical property
    /// </summary>
    [ Browsable(true), DefaultValue( true ), NotifyParentProperty (true ) ]
    public bool AllowVertical
    {
      get { return m_allowVertical; }
      set
      {
        if( value != m_allowVertical )
        {
          m_allowVertical = value;
          UpdateStyles();
        }
      }
    } // AllowVertical

    
    /// <summary>
    /// gets or sets the background color
    /// </summary>
    [ Browsable( true ), DefaultValue( typeof( Color ), "Control" ), NotifyParentProperty( true ) ]
    public Color BackColor
    {
      get
      {
        return m_backColor;
      }
      set
      {
        if(value != m_backColor)
        {
          //Code to set BackColor
          m_backColor = value;
        }
      }
    } // BackColor

    
    /// <summary>
    /// gets or sets the background image
    /// </summary>
    [Browsable(true)]
    [DefaultValue(null)]
    [NotifyParentProperty(true)]
    public Bitmap BackgroundImage
    {
      get
      {
        return m_backgroundImage;
      }
      set
      {
        if(value != m_backgroundImage)
        {
          if(m_pictureHandle != IntPtr.Zero) Gdi32.DeleteObject(m_pictureHandle);
          m_backgroundImage = value;
          m_pictureHandle = (value == null)?IntPtr.Zero:m_backgroundImage.GetHbitmap();
          UpdatePicture();
        }
      }
    } // BackgroundImage
		
    
    /// <summary>
    /// gets or sets the band index
    /// </summary>
    [Browsable(false),
    System.ComponentModel.DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden),
    EditorBrowsable(EditorBrowsableState.Always)]
    public int BandIndex
    {
      get
      {
        if(Created)
        {
          return (int)User32.SendMessage(m_bands.Rebar.RebarHwnd, (int)Msgs.RB_IDTOINDEX, (uint)m_id, 0U);
        }
        else
        {
          return -1;
        }
      }
    } // BandIndex

    
    /// <summary>
    /// gets or sets the bands
    /// </summary>
    [Browsable(false),
    System.ComponentModel.DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden),
    EditorBrowsable(EditorBrowsableState.Always)]
    public RebarBandCollection Bands
    {
      get
      {
        return m_bands;
      }

      set
      {
        if(!Created)
        {
          m_bands = value;
          m_id = m_bands.NextID();
          CreateBand();
        }
      }
    } // Bands

    
    /// <summary>
    /// gets or sets the bounds
    /// </summary>
    [Browsable(false),
    System.ComponentModel.DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden),
    EditorBrowsable(EditorBrowsableState.Always)]
    public Rectangle Bounds
    {
      get
      {
        if(Created)
        {
          RECT rect = new RECT();
          User32.SendMessage(m_bands.Rebar.RebarHwnd, (int)Msgs.RB_GETRECT, BandIndex ,ref rect);
          return new Rectangle(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
        }
        else
        {
          return new Rectangle(0,0,0,0);
        }
      }
    } // Bounds

   
    /// <summary>
    /// gets or sets the caption
    /// </summary>
    [Browsable(true),
    DefaultValue(""),
    NotifyParentProperty(true)]
    public string Caption
    {
      get
      {
        return m_caption;
      }
      set
      {
        if(value != m_caption)
        {
          //Code to set Caption
          m_caption = value;
          UpdateCaption();
        }
      }
    } // Caption

    
    /// <summary>
    /// gets or sets the child control
    /// </summary>
    [Browsable(true),
    DefaultValue(null),
    NotifyParentProperty(true)]
    public Control Child
    {
      get
      {
        return m_child;
      }
      set
      {
        if(value != m_child)
        {
          if(m_child != null && Created)
          {
            m_child.Parent = m_bands.Rebar.Parent;
          }
          //Code to set Child
					
          m_child = value;
          if(m_bands !=null)
          {
            m_child.Parent = m_bands.Rebar;
          }
          UpdateChild();
        }

      }
    } // Child

    
    /// <summary>
    /// gets the created state
    /// </summary>
    [Browsable(false),
    EditorBrowsable(EditorBrowsableState.Always)]
    public bool Created
    {
      get
      {
        return (m_created);
      }
    } // Created

   
    /// <summary>
    /// gets or sets the emboss picture setting
    /// </summary>
    [Browsable(true),
    DefaultValue(true),
    NotifyParentProperty(true)]
    public bool EmbossPicture
    {
      get
      {
        return m_embossPicture;
      }
      set
      {
        if(value != m_embossPicture)
        {
          //Code to set Style
          m_embossPicture = value;
          UpdateStyles();
        }
      }
    } // EmbossPicture

    
    /// <summary>
    /// gets or sets the fixed background property
    /// </summary>
    [Browsable(true),
    DefaultValue(true),
    NotifyParentProperty(true)]
    public bool FixedBackground
    {
      get
      {
        return m_fixedBackground;
      }
      set
      {
        if(value != m_fixedBackground)
        {
          //Code to set Style
          m_fixedBackground = value;
          UpdateStyles();
        }
      }
    } // FixedBackground

    
    /// <summary>
    /// gets or sets the fixed size property
    /// </summary>
    [Browsable(true),
    DefaultValue(false),
    NotifyParentProperty(true)]
    public bool FixedSize
    {
      get
      {
        return m_fixedSize;
      }
      set
      {
        if(value != m_fixedSize)
        {
          //Code to set Style
          m_fixedSize = value;
          UpdateStyles();
        }
      }
    } // FixedSize

    
    /// <summary>
    /// gets or sets the foreground color
    /// </summary>
    [Browsable(true),
    DefaultValue(typeof(Color), "ControlText"),
    NotifyParentProperty(true)]
    public Color ForeColor
    {
      get
      {
        return m_foreColor;
      }
      set
      {
        if(value != m_foreColor)
        {
          //Code to set ForeColor
          m_foreColor = value;
          UpdateColors();
        }
      }
    } // ForeColor

    
    /// <summary>
    /// gets or sets the gripper settings
    /// </summary>
    [Browsable(true),
    DefaultValue(GripperSettings.Auto),
    NotifyParentProperty(true)]
    public GripperSettings GripSettings
    {
      get
      {
        return m_gripSettings;
      }
      set
      {
        if(value != m_gripSettings)
        {
          //Code to set Caption
          m_gripSettings = value;
          UpdateStyles();
        }
      }
    } // GripSettings

    
    /// <summary>
    /// gets or sets the band header
    /// </summary>
    [Browsable(true),
    DefaultValue(0),
    NotifyParentProperty(true)]
    public int Header
    {
      get
      {
        return m_header;
      }
      set
      {
        if(value != m_header)
        {
          //Set Band Header
          m_header = value;
          UpdateMinimums();
        }
      }
    } // Header

    
    /// <summary>
    /// gets the height
    /// </summary>
    [Browsable(false),
    EditorBrowsable(EditorBrowsableState.Always)]
    public int Height
    {
      get
      {
        return Bounds.Height;
      }
    } // Height

    
    /// <summary>
    /// gets the ID
    /// </summary>
    [Browsable(false),
    EditorBrowsable(EditorBrowsableState.Always)]
    public int ID
    {
      get
      {
        if(m_bands != null)
          return m_id;
        else
          return -1;
      }
    } // ID

    
    /// <summary>
    /// gets or sets the ideal width
    /// </summary>
    [Browsable(true),
    DefaultValue(0),
    NotifyParentProperty(true)]
    public int IdealWidth
    {
      get
      {
        return m_idealWidth;
      }
      set
      {
        if(value != m_idealWidth)
        {
          m_idealWidth = value;
          UpdateMinimums();
        }
      }
    } // IdealWidth

    
    /// <summary>
    /// gets or sets the image
    /// </summary>
    [Browsable(true),
    DefaultValue(-1),
    NotifyParentProperty(true)]
    public int Image
    {
      get
      {
        return m_image;
      }
      set
      {
        if(value != m_image)
        {
          //Set Image for band
          m_image = value;
          UpdateIcon();
        }
      }
    } // Image

    
    /// <summary>
    /// gets the index
    /// </summary>
    [Browsable(false),
    EditorBrowsable(EditorBrowsableState.Always)]
    public int Index
    {
      get
      {
        if(m_bands != null)
          return m_bands.IndexOf(this);
        else
          return -1;
      }
    } // Index

    
    /// <summary>
    /// gets or sets the integral value
    /// </summary>
    [Browsable(true),
    DefaultValue("1")]
    public int Integral
    {
      get
      {
        return m_integral;
      }
      set
      {
        if(value != m_integral)
        {
          m_integral = value;
          UpdateMinimums();
        }
      }
    } // Integral

    
    /// <summary>
    /// gets or sets the key
    /// </summary>
    [Browsable(true),
    DefaultValue("")]
    public string Key
    {
      get
      {
        return m_key;
      }
      set
      {
        if(value != m_key)
        {
          if(m_bands != null & value != "")
          {
            if(m_bands[value] != null)
              throw new ArgumentException( "The key specified is not unique.", "Key" );
          }
          m_key = value;
        }
      }
    } // Key

    
    /// <summary>
    /// gets the left boundary
    /// </summary>
    [Browsable(false),
    EditorBrowsable(EditorBrowsableState.Always)]
    public int Left
    {
      get
      {
        return Bounds.Left;
      }
    } // Left

    
    /// <summary>
    /// gets the locattion of the control
    /// </summary>
    [Browsable(false),
    EditorBrowsable(EditorBrowsableState.Always)]    
    public Point Location
    {
      get
      {
        return Bounds.Location;
      }
    } // Location

    
    /// <summary>
    /// gets or sets the maximum height
    /// </summary>
    [Browsable(true),
    DefaultValue(24),
    NotifyParentProperty(true)]
    public int MaxHeight
    {
      get
      {
        return m_maxHeight;
      }
      set
      {
        if(value != m_maxHeight)
        {
          //Set Band Height
          m_maxHeight = value;
          UpdateMinimums();
        }
      }
    } // MaxHeight
		
    
    /// <summary>
    /// gets or sets the minimum height
    /// </summary>
    [Browsable(true),
    DefaultValue(24),
    NotifyParentProperty(true)]
    public int MinHeight
    {
      get
      {
        return m_minHeight;
      }
      set
      {
        if(value != m_minHeight)
        {
          //Set Band Height
          m_minHeight = value;
          UpdateMinimums();
        }
      }
    } // MinHeight

    
    /// <summary>
    /// gets or sets the minimum width
    /// </summary>
    [Browsable(true),
    DefaultValue(24),
    NotifyParentProperty(true)]
    public int MinWidth
    {
      get
      {
        return m_minWidth;
      }
      set
      {
        if(value != m_minWidth)
        {
          //Set Band Width
          m_minWidth = value;
          UpdateMinimums();
        }
      }
    } // MinWidth

    
    /// <summary>
    /// gets or sets the new row property
    /// </summary>
    [Browsable(true),
    DefaultValue(true),
    NotifyParentProperty(true)]
    public bool NewRow
    {
      get
      {
        return m_newRow;
      }
      set
      {
        if(value != m_newRow)
        {
          //Set Style
          m_newRow = value;
          UpdateStyles();
        }
      }
    } // NewRow


    /// <summary>
    /// gets the position
    /// </summary>
    [Browsable(false)]
    [EditorBrowsable(EditorBrowsableState.Always)]
    public int Position
    {
      get
      {
        if(Created)
        {
          return BandIndex;
        }
        else if(m_bands != null)
        {
          return Index;
        }
        else
        {
          return -1;
        }
      }
    } // Position

    
    /// <summary>
    /// gets or sets the show caption property
    /// </summary>
    [Browsable(true)]
    [DefaultValue(true)]
    [NotifyParentProperty(true)]
    public bool ShowCaption
    {
      get
      {
        return m_showCaption;
      }
      set
      {
        if(value != m_showCaption)
        {
          //Set band style
          m_showCaption = value;
          UpdateStyles();
        }
      }
    } // ShowCaption

    
    /// <summary>
    /// gets or sets the show icon property
    /// </summary>
    [Browsable(true)]
    [DefaultValue(false)]
    [NotifyParentProperty(true)]
    public bool ShowIcon
    {
      get
      {
        return m_showIcon;
      }
      set
      {
        if(value != m_showIcon)
        {
          m_showIcon = value;
          UpdateIcon();
        }
      }
    } // ShowIcon

    
    /// <summary>
    /// gets the size of the control bounds
    /// </summary>
    [Browsable(false),
    EditorBrowsable(EditorBrowsableState.Always)]
    public Size Size
    {
      get
      {
        return Bounds.Size;
      }
    } // Size

    
    /// <summary>
    /// gets or sets the rebar band style
    /// </summary>
    [Browsable(false)]
    [EditorBrowsable(EditorBrowsableState.Always)]
    protected int Style
    {
      get
      {
        int style = 0;
        if(!m_allowVertical)
          style |= (int)RebarBandStyles.RBBS_NOVERT;
        if(m_embossPicture)
          style |= (int)RebarBandStyles.RBBS_CHILDEDGE;
        if(m_fixedBackground)
          style |= (int)RebarBandStyles.RBBS_FIXEDBMP;
        if(m_fixedSize)
          style |= (int)RebarBandStyles.RBBS_FIXEDSIZE;
        if(m_newRow)
          style |= (int)RebarBandStyles.RBBS_BREAK;
        if(!m_showCaption)
          style |= (int)RebarBandStyles.RBBS_HIDETITLE;
        if(!m_visible)
          style |= (int)RebarBandStyles.RBBS_HIDDEN;
        if(m_gripSettings == GripperSettings.Always)
          style |= (int)RebarBandStyles.RBBS_GRIPPERALWAYS;
        else if(m_gripSettings == GripperSettings.Never)
          style |= (int)RebarBandStyles.RBBS_NOGRIPPER;
        return style;
      }
      set
      {
        m_allowVertical = !((value & (int)RebarBandStyles.RBBS_NOVERT) 
          == (int)RebarBandStyles.RBBS_NOVERT);
        m_embossPicture = (value & (int)RebarBandStyles.RBBS_CHILDEDGE)
          == (int)RebarBandStyles.RBBS_CHILDEDGE;
        m_fixedBackground = (value & (int)RebarBandStyles.RBBS_FIXEDBMP)
          == (int)RebarBandStyles.RBBS_FIXEDBMP;
        m_fixedSize = (value & (int)RebarBandStyles.RBBS_FIXEDSIZE)
          == (int)RebarBandStyles.RBBS_FIXEDSIZE;
        m_newRow = (value & (int)RebarBandStyles.RBBS_BREAK)
          == (int)RebarBandStyles.RBBS_BREAK;
        m_showCaption = !((value & (int)RebarBandStyles.RBBS_HIDETITLE)
          == (int)RebarBandStyles.RBBS_HIDETITLE);
        m_visible = !((value & (int)RebarBandStyles.RBBS_HIDDEN)
          == (int)RebarBandStyles.RBBS_HIDDEN);
        if((value & (int)RebarBandStyles.RBBS_GRIPPERALWAYS)
          == (int)RebarBandStyles.RBBS_GRIPPERALWAYS)
          m_gripSettings = GripperSettings.Always;
        else if ((value & (int)RebarBandStyles.RBBS_NOGRIPPER)
          == (int)RebarBandStyles.RBBS_NOGRIPPER)
          m_gripSettings = GripperSettings.Never;
        else
          m_gripSettings = GripperSettings.Auto;
        UpdateStyles();
      }
    } // Style

    
    /// <summary>
    /// gets or sets the object tag
    /// </summary>
    [Browsable(true),
    DefaultValue(null)]
    public object Tag
    {
      get
      {
        return m_tag;
      }
      set
      {
        m_tag = value;
      }
    } // Tag

    
    /// <summary>
    /// gets or sets the use coolbar colors property
    /// </summary>
    [Browsable(true)]
    [DefaultValue(true)]
    [NotifyParentProperty(true)]
    public bool UseCoolbarColors
    {
      get
      {
        return m_useCoolbarColors;
      }
      set
      {
        if(value != m_useCoolbarColors)
        {
          //Set the Colors
          m_useCoolbarColors = value;
          UpdateColors();
        }
      }
    } // UseCoolbarColors

    
    /// <summary>
    /// gets or sets the use coolbar picture property
    /// </summary>
    [Browsable(true)]
    [DefaultValue(true)]
    [NotifyParentProperty(true)]
    public bool UseCoolbarPicture
    {
      get
      {
        return m_useCoolbarPicture;
      }
      set
      {
        if(value != m_useCoolbarPicture)
        {
          //Set the Picture
          m_useCoolbarPicture = value;
          UpdatePicture();
        }
      }
    } // UseCoolbarPicture


    /// <summary>
    /// gets or sets the visility of the band
    /// </summary>
    [Browsable(true)]
    [DefaultValue(true)]
    public bool Visible
    {
      get
      {
        return m_visible;
      }
      set
      {
        if(value != m_visible)
        {
          //Set band style
          m_visible = value;
          if(Created)
          {
            User32.SendMessage(m_bands.Rebar.RebarHwnd, (int) Msgs.RB_SHOWBAND, (uint)BandIndex, (m_visible)?1U:0U);
            //OnVisibleChanged(new System.EventArgs());
          }
        }
      }
    } // Visible
		
    
    /// <summary>
    /// gets the width of the control
    /// </summary>
    [Browsable(false),
    EditorBrowsable(EditorBrowsableState.Always)]
    public int Width
    {
      get
      {
        return Bounds.Width;
      }
    } // Width
    
    #endregion

    #region methods
    
    /// <summary>
    /// Initializes a RebarBand instance with default values.
    /// </summary>
    public RebarBand()
    {
      m_foreColor = SystemColors.ControlText;
      m_backColor = SystemColors.Control;
      this.Header = 21;
    } // RebarBand()

    
    /// <summary>
    /// Destructor
    /// </summary>
    ~RebarBand()
    {
      Dispose( false );
    } // ~RebarBand()


    /// <summary>
    /// Releases the unmanaged resources used by the Control and optionally releases the managed resources.
    /// </summary>
    /// <param name="disposing">true to release both managed and unmanaged resources; false to release only unmanaged resources.</param>
    protected override void Dispose( bool disposing )
    {
      DestroyBand();
      if( m_pictureHandle != IntPtr.Zero ) 
        Gdi32.DeleteObject( m_pictureHandle );
      if(disposing)
      {
				
      }
      m_disposed = true;
    } // Dispose( bool disposing )


    /// <summary>
    /// maximizes the band
    /// </summary>
    public void Maximize()
    {
      if( this.Created )
        User32.SendMessage( m_bands.Rebar.RebarHwnd, ( int ) Msgs.RB_MAXIMIZEBAND, ( uint )this.BandIndex, ( uint ) m_idealWidth );
    } // Maximize()

    
    /// <summary>
    /// minimizes the band
    /// </summary>
    public void Minimize()
    {
      if( this.Created )
        User32.SendMessage( m_bands.Rebar.RebarHwnd, ( int )Msgs.RB_MINIMIZEBAND, ( uint )this.BandIndex, ( uint ) m_idealWidth );
    } // Minimize()

    
    /// <summary>
    /// create the band
    /// </summary>
    internal void CreateBand()
    {
      if( !this.Created && m_bands != null && m_bands.Rebar.InternalRebar != null )
      {
        if( m_child != null ) 
          m_child.Parent = m_bands.Rebar;
        
        REBARBANDINFO rbBand = new REBARBANDINFO();
        rbBand.cbSize = ( uint ) Marshal.SizeOf( rbBand );
        rbBand.fMask = ( uint )( RebarBandInfoFlags.RBBIM_STYLE | RebarBandInfoFlags.RBBIM_ID | RebarBandInfoFlags.RBBIM_TEXT );
        
        if( !m_useCoolbarColors )
          rbBand.fMask |= (uint) RebarBandInfoFlags.RBBIM_COLORS;

        if( m_child != null )
          rbBand.fMask |= ( uint ) ( RebarBandInfoFlags.RBBIM_CHILD | RebarBandInfoFlags.RBBIM_CHILDSIZE );
        
        if(m_image >= 0)
          rbBand.fMask |= (uint) RebarBandInfoFlags.RBBIM_IMAGE;
        
        if(m_backgroundImage != null)
          rbBand.fMask |= (uint) RebarBandInfoFlags.RBBIM_BACKGROUND;

        if( m_header != -1 )
          rbBand.fMask  |= ( uint )RebarBandInfoFlags.RBBIM_HEADERSIZE;
        
        rbBand.clrFore = new COLORREF(ForeColor);
        rbBand.clrBack = new COLORREF(BackColor);
        rbBand.fStyle = ( uint )Style; 
       
        if(m_backgroundImage != null)
        {
          rbBand.hbmBack = m_pictureHandle;
        }
        rbBand.lpText = m_caption;
        
        if(m_child != null)
        {
          rbBand.hwndChild = m_child.Handle;
          rbBand.cxMinChild = (uint)m_minWidth;
          rbBand.cyMinChild = (uint)m_minHeight;
          rbBand.cyIntegral = (uint)m_integral;//0;
          rbBand.cyChild = (uint)m_minHeight;
          rbBand.cyMaxChild = 40;
          rbBand.cxIdeal = (uint)m_idealWidth;
        }
        if(m_showIcon)
        {
          rbBand.iImage = m_image;
        }
        rbBand.wID = (uint)m_id;
        rbBand.cxHeader = (uint)m_header;
				
        if( User32.SendMessage( m_bands.Rebar.RebarHwnd, (int)Msgs.RB_INSERTBANDA, -1,ref rbBand ) != 0)
          m_created = true;
      }
    } // CreateBand()

    
    /// <summary>
    /// destroys the band
    /// </summary>
    internal void DestroyBand()
    {
      if( this.Created )
      {
        User32.SendMessage( m_bands.Rebar.RebarHwnd, (int)Msgs.RB_DELETEBAND, ( uint ) this.BandIndex, 0U );
        m_bands = null;
        m_created = false;
      }
    } // DestroyBand()


    /// <summary>
    /// update the caption
    /// </summary>
    protected void UpdateCaption()
    {
      if( this.Created )
      {
        REBARBANDINFO rbBand = new REBARBANDINFO();
        rbBand.cbSize = ( uint )Marshal.SizeOf( rbBand );
        rbBand.fMask = ( uint )RebarBandInfoFlags.RBBIM_TEXT;
        rbBand.lpText = m_caption;
        User32.SendMessage( m_bands.Rebar.RebarHwnd, (int)Msgs.RB_SETBANDINFOA, BandIndex ,ref rbBand );
      }
    } // UpdateCaption()

    
    /// <summary>
    /// update the child control
    /// </summary>
    protected void UpdateChild()
    {
      if( !this.Created )
        return;

      REBARBANDINFO rbBand = new REBARBANDINFO();
      rbBand.cbSize = ( uint ) Marshal.SizeOf( rbBand );
      rbBand.fMask = ( uint ) RebarBandInfoFlags.RBBIM_CHILD;
      
      if( m_child == null )
        rbBand.hwndChild = IntPtr.Zero;
      else
        rbBand.hwndChild = m_child.Handle;
		
      User32.SendMessage( m_bands.Rebar.RebarHwnd, (int)Msgs.RB_SETBANDINFOA, this.BandIndex, ref rbBand );

      UpdateMinimums();

    } // UpdateChild()


    /// <summary>
    /// update the colors
    /// </summary>
    protected void UpdateColors()
    {
      if( !this.Created )
        return;

      REBARBANDINFO rbBand = new REBARBANDINFO();
      rbBand.cbSize = ( uint ) Marshal.SizeOf( rbBand );
      rbBand.fMask = ( uint ) RebarBandInfoFlags.RBBIM_COLORS;
      
      if( m_useCoolbarColors )
      {
        rbBand.clrBack = new COLORREF();
        rbBand.clrBack.m_color = ( uint ) ColorConstants.CLR_DEFAULT;
        rbBand.clrFore = new COLORREF();
        rbBand.clrFore.m_color = ( uint ) ColorConstants.CLR_DEFAULT;
      }
      else
      {
        rbBand.clrBack = new COLORREF( m_backColor );
        rbBand.clrFore = new COLORREF( m_foreColor );
      }
			
      User32.SendMessage( m_bands.Rebar.RebarHwnd, (int)Msgs.RB_SETBANDINFOA, this.BandIndex , ref rbBand );
    } // UpdateColors()

    
    /// <summary>
    /// update the icon
    /// </summary>
    protected void UpdateIcon()
    {
      if( !this.Created )
        return;
      
      REBARBANDINFO rbBand = new REBARBANDINFO();
      rbBand.cbSize = ( uint ) Marshal.SizeOf( rbBand );
      rbBand.fMask = ( uint ) RebarBandInfoFlags.RBBIM_IMAGE;

      if( m_showIcon )
        rbBand.iImage = m_image;
      else
        rbBand.iImage = -1;
			
      User32.SendMessage( m_bands.Rebar.RebarHwnd, (int)Msgs.RB_SETBANDINFOA, this.BandIndex ,ref rbBand );
    } // UpdateIcon()


    /// <summary>
    /// update the minimums
    /// </summary>
    protected void UpdateMinimums()
    {
      if( this.Created )
      {
        REBARBANDINFO rbBand = new REBARBANDINFO();
        rbBand.cbSize = ( uint )Marshal.SizeOf( rbBand );
        rbBand.fMask = ( uint )( RebarBandInfoFlags.RBBIM_CHILDSIZE|RebarBandInfoFlags.RBBIM_IDEALSIZE );
        
        if( m_header != -1 ) 
          rbBand.fMask |= ( uint )RebarBandInfoFlags.RBBIM_HEADERSIZE;
        
        rbBand.cxMinChild = ( uint ) m_minWidth;
        rbBand.cyMinChild = ( uint ) m_minHeight;
        rbBand.cyIntegral = ( uint ) m_integral;
        rbBand.cyChild = ( uint ) m_minHeight;
        rbBand.cyMaxChild = 300;
        rbBand.cxIdeal = ( uint ) m_idealWidth;
        rbBand.cxHeader = ( uint ) m_header;
        
        User32.SendMessage( m_bands.Rebar.RebarHwnd, ( int ) Msgs.RB_SETBANDINFOA, BandIndex ,ref rbBand );
      }
    } // UpdateMinimums()


    /// <summary>
    /// update the picture
    /// </summary>
    protected void UpdatePicture()
    {
      if( !this.Created )
        return;
      REBARBANDINFO rbBand = new REBARBANDINFO();
      rbBand.cbSize = ( uint ) Marshal.SizeOf( rbBand );
      rbBand.fMask = ( uint )  RebarBandInfoFlags.RBBIM_BACKGROUND;
      rbBand.hbmBack = m_pictureHandle;
			
      User32.SendMessage( m_bands.Rebar.RebarHwnd, (int)Msgs.RB_SETBANDINFOA, BandIndex ,ref rbBand );
    } // UpdatePicture()

    
    /// <summary>
    /// update the style
    /// </summary>
    protected void UpdateStyles()
    {
      if( this.Created )
      {
        REBARBANDINFO rbBand = new REBARBANDINFO();
        rbBand.cbSize = ( uint )Marshal.SizeOf( rbBand );
        rbBand.fMask = ( uint )  RebarBandInfoFlags.RBBIM_STYLE;
        rbBand.fStyle = ( uint ) this.Style; 
				
        User32.SendMessage(m_bands.Rebar.RebarHwnd, (int)Msgs.RB_SETBANDINFOA, BandIndex ,ref rbBand );
      }
   } // UpdateStyles()
  
   #endregion

  } // class RebarBand
  
  #endregion

  #region RebarBandCollection

  /// <summary>
  /// a collection of rebar bands
  /// </summary>
  [ Editor( typeof( Viatronix.UI.RebarBandCollectionEditor ), typeof( System.Drawing.Design.UITypeEditor ) ) ]
  public class RebarBandCollection: CollectionBase, IEnumerable
  {
  
    #region fields

    /// <summary>
    /// the rebar
    /// </summary>
    private Rebar m_rebar;
    
    /// <summary>
    /// the counter
    /// </summary>
    private int m_counter = 0;

    #endregion

    #region properties

    /// <summary>
    /// gets the contained rebar
    /// </summary>
    public Rebar Rebar
    {
      get
      {
        return m_rebar;
      }
    } // Rebar

    #endregion

    #region methods

    /// <summary>
    /// Initializes a RebarBandCollection instance with the specified rebar
    /// </summary>
    /// <param name="rebar">the specified rebar</param>
    public RebarBandCollection( Rebar rebar )
    {
      m_rebar = rebar;
    } // RebarBandCollection( Rebar rebar )

    
    /// <summary>
    /// indexer
    /// </summary>
    public RebarBand this[ int Index ]
    {
      get
      {
        return (RebarBand)List[Index];
      }
    } // this[ int Index ]

    
    /// <summary>
    /// indexer
    /// </summary>
    public RebarBand this[ string Key ]
    {
      get
      {
        if(Key == null) return null;
        foreach (RebarBand band in this)
        {
          if (band.Key == Key) return band;
        }
        return null;
      }
    } // this[ string Key ]

    
    /// <summary>
    /// add the specified rebar band to this collection
    /// </summary>
    /// <param name="band">the band to add</param>
    /// <returns>the index of the added band</returns>
    public RebarBand Add( RebarBand band )
    {
      List.Add(band);
      band.Bands = this;
      return band;
    } // Add( RebarBand band )

    
    /// <summary>
    /// gets the band specified by the given ID
    /// </summary>
    /// <param name="ID">the ID to look up in the band</param>
    /// <returns>the matching band, or null if not found</returns>
    public RebarBand BandFromID( int ID )
    {
      foreach(RebarBand band in this)
      {
        if(band.ID == ID)
          return band;
      }
      return null;
    } // BandFromID( int ID )

    
    /// <summary>
    /// clear out the collection
    /// </summary>
    public new void Clear()
    {
      for(;List.Count > 0;)
      {
        Remove(0);
      }
      base.Clear();
    } // Clear()


    /// <summary>
    /// gets an enumerator for the collection
    /// </summary>
    /// <returns>the enumerator</returns>
    public new RebarBandEnumerator GetEnumerator()
    {
      return new RebarBandEnumerator(this);
    } // GetEnumerator()

    
    /// <summary>
    /// gets the enumerator for the collection
    /// </summary>
    /// <returns></returns>
    IEnumerator IEnumerable.GetEnumerator()
    {
      return GetEnumerator();
    } // GetEnumerator()

    
    /// <summary>
    /// gets the index of the specified band in the collection
    /// </summary>
    /// <param name="band">the band to find</param>
    /// <returns>the index of the band</returns>
    public int IndexOf( RebarBand band )
    {
      return List.IndexOf(band);
    } // IndexOf( RebarBand band )

    
    /// <summary>
    /// obtain the next valid ID
    /// </summary>
    /// <returns>a valid ID</returns>
    internal int NextID()
    {
      return m_counter++;
    } // NextID()

    
    /// <summary>
    /// removes the specified band from the collection by index
    /// </summary>
    /// <param name="Index">the index of the band to remove</param>
    public void Remove( int Index )
    {
      Remove(this[Index]);
    } // Remove( int Index )

    
    /// <summary>
    /// removes the band with the specified key from the collection
    /// </summary>
    /// <param name="Key">the key of the band to remove</param>
    public void Remove( string Key )
    {
      Remove(this[Key]);
    } // Remove( string Key )


    /// <summary>
    /// removes the specified band from the collection
    /// </summary>
    /// <param name="band">the band to remove</param>
    public void Remove( RebarBand band )
    {
      band.DestroyBand();
      List.Remove(band);
      band.Dispose();
    } // Remove( RebarBand band )

    #endregion
    
    #region RebarBandEnumerator
    
    /// <summary>
    /// enumerator for the RebarBandCollection
    /// </summary>
    public class RebarBandEnumerator : IEnumerator
    {

      #region fields

      /// <summary>
      /// the current index
      /// </summary>
      private int Index;

      /// <summary>
      /// the collection we are enumerating
      /// </summary>
      private RebarBandCollection Collection;

      #endregion

      #region properties

      /// <summary>
      /// gets the current RebarBand
      /// </summary>
      public RebarBand Current
      {
        get
        {
          return Collection[Index];
        }
      } // Current


      /// <summary>
      /// gets the current RebarBand
      /// </summary>
      object IEnumerator.Current
      {
        get
        {
          return Current;
        }
      } // IEnumerator.Current

      #endregion

      #region methods

      /// <summary>
      /// initializes a RebarBandEnumerator with the given collection
      /// </summary>
      /// <param name="Bands">the collection to initialize on</param>
      public RebarBandEnumerator( RebarBandCollection Bands )
      {
        Collection = Bands;
        Index = -1;
      } // RebarBandEnumerator( RebarBandCollection Bands )


      /// <summary>
      /// move to the next RebarBand in the collection
      /// </summary>
      /// <returns>if on a valid RebarBand</returns>
      public bool MoveNext()
      {
        Index++;
        return (Index < Collection.Count);
      } // MoveNext()


      /// <summary>
      /// start over again at the beginning
      /// </summary>
      public void Reset()
      {
        Index = -1;
      } // Reset()

      #endregion

    } // class RebarBandEnumerator

    #endregion

  } // class RebarRebarBandCollection

  #endregion

  #region RebarBandCollectionEditor

  /// <summary>
  /// editor for a rebar band collection
  /// </summary>
  public class RebarBandCollectionEditor : System.ComponentModel.Design.CollectionEditor
  {

    #region methods

    /// <summary>
    /// Initializes a RebarBandCollectionEditor instance with the specified type.
    /// </summary>
    /// <param name="type">the type to initialize on</param>
    public RebarBandCollectionEditor( System.Type type ): base( type )
    {
    } // RebarBandCollectionEditor( System.Type type )


    /// <summary>
    /// sets the items in the collection
    /// </summary>
    /// <param name="editValue">the collection within which to set</param>
    /// <param name="value">the RebarBands to set</param>
    /// <returns>the collection within which to set</returns>
    protected override object SetItems( object editValue, object[] value )
    {
      foreach( RebarBand band in value )
      {
        if( !band.Created )
          ( (RebarBandCollection)editValue ).Add( band );
      }
      return editValue;
    } // SetItems( object editValue, object[] value )

    #endregion
		
  } // class RebarBandCollectionEditor

  #endregion

} // namespace Viatronix.UI

#region revision history

// $Log: Rebar.cs,v $
// Revision 1.8  2005/03/08 19:11:06  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.7.2.1  2005/03/02 14:55:18  frank
// code review
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/Rebar.cs,v 1.8 2005/03/08 19:11:06 frank Exp $
// $Id: Rebar.cs,v 1.8 2005/03/08 19:11:06 frank Exp $

#endregion
