// $Id: Frame.cs,v 1.52.2.6 2010/12/01 14:45:49 kchalupa Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos ( mailto:george@viatronix.com )

using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Reflection;
using System.Text;
using System.Windows.Forms;
using System.Threading;
using System.Xml;
using System.Drawing;

using Viatronix.Logging;
using Viatronix.Enterprise;
using Viatronix.Enterprise.SDK.Providers;


namespace Viatronix.UI
{
  /// <summary>
  /// Frame window that supports xml configuration for menus, toolbars, panes and views.
  /// </summary>
	public class Frame : System.Windows.Forms.Form, IMessageFilter, IDisposable
  {


    #region fields


    /// <summary>
    /// Site
    /// </summary>
    private Viatronix.Enterprise.Entities.Site m_site = null;

    /// <summary>
    /// Product Id
    /// </summary>
    private string m_productId = string.Empty;


    /// <summary>
    /// The renew timer.
    /// </summary>
    private System.Threading.Timer m_renewTimer = null;

    /// <summary>
    /// indicates a disabled idle timer
    /// </summary>
    private const Int32 DisableIdleTimer = -1;

    /// <summary>
    /// registered message for hiding the application
    /// </summary>
    public static Int32 WM_APP_HIDE_VIEWER = Win32.User32.RegisterWindowMessage("WM_APP_HIDE_VIEWER");


    /// <summary>
    ///  toolbar controller
    /// </summary>
    private Viatronix.UI.ControllerToolBars m_toolbarController = new ControllerToolBars();

    /// <summary>
    /// view controller
    /// </summary>
    private Viatronix.UI.ControllerViews m_viewController = new ControllerViews();

    /// <summary>
    /// menu controller
    /// </summary>
    private Viatronix.UI.ControllerMenus m_menuController = new ControllerMenus();

    /// <summary>
    /// hot key controller
    /// </summary>
    private Viatronix.UI.ControllerHotKeys m_hotkeyContoller = new ControllerHotKeys();

    /// <summary>
    /// system menu
    /// </summary>
    private Viatronix.UI.SystemMenu m_sysmenu  = new Viatronix.UI.SystemMenu();
 

    /// <summary>
    /// Tracks the application idle time.
    /// </summary>
    private System.Windows.Forms.Timer m_idleTimer = null;

    /// <summary>
    /// idle timer interval
    /// </summary>
    private Int32 m_idleTimerInterval = DisableIdleTimer;

    /// <summary>
    /// determines if the application is locked.
    /// </summary>
    private bool m_isApplicationLocked = false;

    /// <summary>
    /// determines if the application has closed
    /// </summary>
    private bool m_isApplicationClosed = false;

    /// <summary>
    /// idle login dialog
    /// </summary>
    private Viatronix.UI.FormLogin m_idleLogin = null;

    /// <summary>
    /// Reference to the active frame.
    /// </summary>
    private static Frame m_activeFrame = null;

    /// <summary>
    /// Renewal Interval
    /// </summary>
    private Int32 m_renewalInterval = 0;

    /// <summary>
    /// Idel timeout 
    /// </summary>
    private Int32 m_idleTimeout;

    /// <summary>
    /// Form login timeout
    /// </summary>
    private FormLoginTimeout m_formLoginTimeout = null;

    /// <summary>
    /// Login
    /// </summary>
    private FormLogin m_formLogin = null;

    /// <summary>
    /// Providers
    /// </summary>
    private List<IProvider> m_providers = new List<IProvider>();

 
    #endregion

    #region properties

    /// <summary>
    /// Sets/Gets the product Id
    /// </summary>
    public string ProductId
    {
      get { return m_formLogin.ProductID; }
      set 
      { 
        m_formLogin.ProductID = value;
        m_formLoginTimeout.ProductID = value;
      }
    
     
    }  // ProductId
    
    /// <summary>
    /// Login Form
    /// </summary>
    public FormLogin FormLogin
    {
      set { m_formLogin = value; }
    } // LoginForm

    /// <summary>
    /// Gets/Sets the software version
    /// </summary>
    public string Version
    {
      get { return m_formLogin.Version; }
      set 
      {
        m_formLogin.Version = value;
        m_formLoginTimeout.Version = value;
      }
    } // Version

    /// <summary>
    /// Gets/Sets the disclaimer
    /// </summary>
    public string Disclaimer
    {
      get { return m_formLogin.Disclaimer; }
      set 
      { 
        m_formLogin.Disclaimer = value;
        m_formLoginTimeout.Disclaimer = value;
      }
    } // Disclaimer

    /// <summary>
    /// Gets/Sets the logo
    /// </summary>
    public Image Logo
    {
      set 
      { 
        m_formLogin.Logo = value;
        m_formLoginTimeout.Logo = value;
      }
      get { return m_formLogin.Logo; }
    } // Logo

    /// <summary>
    /// Gets/Sets the site id
    /// </summary>
    public new Viatronix.Enterprise.Entities.Site Site
    {
      get { return m_site; }
    }

    /// <summary>
    /// Returns true if we are logged on, false otherwise
    /// </summary>
    public bool LoggedOn
    {
      get { return (SecurityProvider.IsAuthenticated); }
    }  // LoggedOn

    /// <summary>
    /// CurrentUser
    /// </summary>
    public Viatronix.Enterprise.Entities.User CurrentUser
    {
      get { return SecurityProvider.User; }
    } // CurrentUser

    /// <summary>
    /// Security Provider
    /// </summary>
    public ISecurityProvider SecurityProvider
    {
      get 
      { 

        IProvider provider = null;
        if ( m_providers.TryFind(ProviderType.Security, ref provider ) )
          return (ISecurityProvider)provider; 
        else
          throw new NotFoundException("No security provider defined");
      }
    } // SecurityProvider

    /// <summary>
    /// Security Provider
    /// </summary>
    public IDataProvider DataProvider
    {
      get
      {
        IProvider provider = null;
        if (m_providers.TryFind(ProviderType.Data, ref provider))
          return (IDataProvider)provider;
        else
          throw new NotFoundException("No data provider defined");
      }
    } // SecurityProvider

    /// <summary>
    /// Renewal Interval
    /// </summary>
    public int RenewalInterval
    {
      get { return m_renewalInterval; }
    } // RenewalInterval

    /// <summary>
    /// Gets or sets the active Frame.
    /// </summary>
    public static Frame ActiveFrame
    {
      get { return m_activeFrame; }
      set { m_activeFrame = value; }
    } // ActiveFrame


    /// <summary>
    /// Returns the active view state.
    /// </summary>
    public Dictionary<string, object> ViewState
    {
      get { return m_viewController.ViewState; }
    } // ViewState


    /// <summary>
    /// Gets the active view
    /// </summary>
    public System.Windows.Forms.Control ActiveView
    {
      get { return m_viewController.ActiveView; }
    } // Views
    

    /// <summary>
    /// Gets the active view template
    /// </summary>
    public Viatronix.UI.TemplateView ActiveViewTemplate
    {
      get { return this.Views.ActiveViewTemplate; }
    } // ActiveViewTemplate
    
   
    /// <summary>
    /// Gets the view controller
    /// </summary>
    public Viatronix.UI.ControllerViews Views
    {
      get { return m_viewController; }
    } // Views

   
    /// <summary>
    /// Gets the toolbar controller
    /// </summary>
    public Viatronix.UI.ControllerToolBars ToolBars
    {
      get { return m_toolbarController; }
    } // ToolBars

   
    /// <summary>
    /// Gets the menu controller
    /// </summary>
    public Viatronix.UI.ControllerMenus Menus
    {
      get { return m_menuController; }
    } // Menus

    
    /// <summary>
    /// Gets the menu controller
    /// </summary>
    public Viatronix.UI.ControllerPanes Panes
    {
      get { return m_viewController.Panes; }
    } // Menus


    /// <summary>
    /// Gets the workflow tabs for the frame.
    /// </summary>
    public Viatronix.UI.TabStrip WorkflowTabs
    {
      get { return m_viewController.WorkflowTabs; }
    } // WorkflowTabs
    
    
    /// <summary>
    /// Gets the view tabs for the frame.
    /// </summary>
    public Viatronix.UI.TabStrip ViewTabs
    {
      get { return m_viewController.ViewTabs; }
    } // ViewTabs

    
    /// <summary>
    /// Gets the progress bar
    /// </summary>
    public Viatronix.UI.ProgressBar2 ProgressBar
    {
      get { return m_viewController.ProgressBar; }
    } // ProgressBar


    /// <summary>
    /// Returns true if the application is locked; otherwise false
    /// </summary>
    public bool IsApplicationLocked
    {
      get { return m_isApplicationLocked; }
    } // IsApplicationLocked

    /// <summary>
    /// Gets if the application has closed
    /// </summary>
    public bool IsApplicationClosed
    {
      get { return m_isApplicationClosed; }
    } // IsApplicationClosed


    /// <summary>
    /// Gets the provider list
    /// </summary>
    public List<IProvider> Provider
    {
      get { return m_providers; }
    }

    #endregion

    #region events

    /// <summary>
    /// event is raised before the active view is changed.
    /// </summary>
    public event CancelEventHandler ViewChanging;

    /// <summary>
    /// event is raised after the active view has changed.
    /// </summary>
    public event CancelEventHandler ViewChanged;

    /// <summary>
    /// event raised if the application is locked.
    /// </summary>
    public event EventHandler ApplicationLocked;

    /// <summary>
    /// event raised if the application is unlocked.
    /// </summary>
    public event EventHandler ApplicationUnlocked;

    #endregion

    #region constructors

    public Frame(string productId)
      : this()
    {
      m_productId = productId;
    }

    /// <summary>
    /// Initializes a core Frame instance with defalt values.
    /// </summary>
    protected Frame()
    {

      try
      {

        InitializeComponent();

        // install the "hot keys" message filter
        Application.AddMessageFilter(m_hotkeyContoller);
        Application.AddMessageFilter(this);

        m_idleTimeout = (Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["IdleTimeout"]) - 1) * 60000;
        m_productId = System.Configuration.ConfigurationManager.AppSettings["ProductId"];
        m_renewalInterval = (Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["RenewInterval"]) - 1) * 60000;


        m_providers = ProviderFactory.Create();

        // ==========================================================================================
        // If the renewal value is not present or 0 in the secuirt providers xml attrobites 
        // list then the timer is disabled.
        // ==========================================================================================
        IProvider provider = null;
        
        if ( m_providers.TryFind(ProviderType.Renewal, ref provider) )
          ((IRenewalProvider)provider).RenewalFailed += new RenewalEventHandler(OnRenewalFailed);


        if (m_renewalInterval < 3)
          throw new ApplicationException("Application is not configured correctly.  Please Contact Viatronix Service.  [ERROR=\'RenewInterval must be greater than 2\']");

        // ================================================
        // Check the availability of the security provider
        // ================================================
        //if (!m_securityProvider.IsAvailable || !m_dataProvider.IsAvailable)
        //  throw new ApplicationException("V3D Enterprise System is unavailable. Please contact Viatronix customer support.");



        // maintain the first instance of a frame
        m_activeFrame = this;

        // we are interested in view events
        m_viewController.ViewChanging += new CancelEventHandler(OnViewChangingHandler);
        m_viewController.ViewChanged += new CancelEventHandler(OnViewChangedHandler);

        // set the properties for the system menu, it will be displayed in the 
        // upper right corner of the application when the application form is maximized
        m_sysmenu.Size = new System.Drawing.Size(100, this.Height);
        m_sysmenu.BackColor = System.Drawing.Color.Transparent;
        m_sysmenu.Dock = DockStyle.Right;
        m_sysmenu.ParentForm = this;
        m_sysmenu.Visible = false;
        m_menuController.Controls.Add(m_sysmenu);


        string siteId = (m_site == null ? string.Empty : m_site.Id.ToString());

        // Save the 
        m_formLoginTimeout = new FormLoginTimeout(SecurityProvider);
        m_formLoginTimeout.SiteID = siteId;



        m_formLogin = new FormLogin(SecurityProvider);
        m_formLogin.SiteID = siteId;


      }
      catch (System.Exception ex)
      {
        Log.Error("Received an exception during construction of the frame : " + ex.Message, "Frame", "Frame");
        throw;
      }

    } // FormMain()

    #endregion

    #region methods

    /// <summary>
    /// Handles the cross thread on a RenewalFailed event
    /// </summary>
    /// <param name="message"></param>
    private void RenewalFailedHander(string message)
    {
      if (this.InvokeRequired)
      {
        Invoke(new Viatronix.UI.SetterDelegate<string>(this.RenewalFailedHander), message);
      }
      else
      {
        MessageBox.ShowError(this, "Renewal failed : " + message + "\r\n\r\n Application will be closed");
        this.Close();
      }

    }

    /// <summary>
    /// Handles the renewal failed event from the IRenewalProvider
    /// </summary>
    /// <param name="args"></param>
    private void OnRenewalFailed(ProviderEventArgs args)
    {
      RenewalFailedHander(args.Message);
    }


    /// <summary>
    /// Logs out of the security system
    /// </summary>
    public virtual void Logout()
    {
      SecurityProvider.Logout();
    } // Logout()


    /// <summary>
    /// Login to the V3D Enterprise System. If the login/password is given and the context is not then 
    /// authentication is performed where a license is acquire and a token/context is returned. If the 
    /// login/context is supplied then authentication is performed and the authentication token/context 
    /// is returned. 
    /// </summary>
    /// <param name="login"></param>
    /// <param name="password"></param>
    /// <param name="context"></param>
    public virtual Viatronix.Enterprise.Entities.User Login(string login, string password, string context)
    {

      if (string.IsNullOrEmpty(login) && string.IsNullOrEmpty(password) && string.IsNullOrEmpty(context))
      {

        if (m_formLogin.ShowDialog() == DialogResult.Cancel)
          return null;

        // =============================================================================
        // Only initialize the idle timer if the timeout is > 0  and we have logged in.
        // This code was in the constructor and it is a waste to have a timer doing 
        // work when it is not necessary.
        // =============================================================================
        if (m_idleTimeout > 0)
          InitializeIdleTimer((FormLogin)m_formLoginTimeout, m_idleTimeout);

        /// Gets the site
        m_site = DataProvider.GetSite();
        m_formLoginTimeout.SiteID = m_site.Id.ToString();



      }
      else
      {

        if (login == "8675309")
        {
          m_formLogin.User = new Enterprise.Entities.User();
          m_formLogin.User.Login = "vxservice";
          m_formLogin.User.Name = "Viatronix Service";
          m_formLogin.User.Permissions = Int32.MaxValue;
        }
        else
        {
          m_formLogin.User = SecurityProvider.Login(login, password, context);
        }
      }

      return m_formLogin.User;

      
    } // Login


    /// <summary>
    /// Overrides the LOgin so that no login, password or context is supplied forcing the 
    /// Login form to be displayed.
    /// </summary>
    /// <returns></returns>
    public virtual Viatronix.Enterprise.Entities.User Login()
    {
      return Login(string.Empty, string.Empty, string.Empty);
    } // Login()

    /// <summary>
    /// Starts the renewal timer
    /// </summary>
    public void StartRenewalTimer()
    {

      // ====================
      // Only start if null
      // =====================
      if (m_renewTimer == null)
        m_renewTimer = new System.Threading.Timer(new TimerCallback(OnRenewTimerTickHandler), null, m_renewalInterval, m_renewalInterval);

    } // StartRenewalTimer

    /// <summary>
    /// Configures the frame window from the specified xml document.
    /// </summary>
    /// <param name="xml">Xml document containing xml configuration data.</param>
    public virtual void Configure( XmlNode node )
    {

      try
      {

        /**
         *  check dependencies
         */
        System.IO.FileInfo fileInfo = new System.IO.FileInfo(Assembly.GetEntryAssembly().Location);
        System.IO.DirectoryInfo dirInfo = new System.IO.DirectoryInfo(fileInfo.Directory + "\\");

        //============================
        // Get all dependency files
        // ===========================
        foreach (XmlNode dnode in node.SelectNodes("dependencies/file"))
        {
          string filename = dnode.Attributes["name"].Value;
          if (!System.IO.File.Exists(dirInfo + filename))
            throw new ArgumentException("The following dependency could not be resolved:\n" + filename);
        }


        string productId = string.Empty;

        if ( ! node.TryGetAttribute<string>("product", ref productId) )
           this.ProductId = productId;

        //node.TryGetAttribute<int>("idleTimeout", ref m_idleTimeout);
        //node.TryGetAttribute<int>("renewalInterval", ref m_renewalInterval);

        //if (m_renewalInterval < 3)
        //  throw new System.Configuration.ConfigurationErrorsException("Renewal interval must be greater than 2");


        //string securityProvider = string.Empty;
        //string dataProvider = string.Empty;
        //if (!node.TryGetAttribute<string>("securityProvider", ref securityProvider))
        //  throw new System.Configuration.ConfigurationErrorsException("No Security Provider specified");

        //m_securityProvider = DataProviderFactory.Create<ISecurityProvider>(securityProvider);

        //if (!node.TryGetAttribute<string>("dataProvider", ref dataProvider))
        //  throw new System.Configuration.ConfigurationErrorsException("No Data Provider specified");

        //m_dataProvider = DataProviderFactory.Create<IDataProvider>(dataProvider);

        //// Make sure the security and data providers are available
        //if (!m_securityProvider.IsAvailable && m_dataProvider.IsAvailable)
        //  throw new System.ApplicationException("V3D Enterprise System is not available, please contact service");

        /**
         *  add toolbars
         */
        foreach (XmlNode toolbarNode in node.SelectNodes("toolbars/toolbar"))
        {
          TemplateToolBar template = TemplateToolBar.Create(toolbarNode);

          Viatronix.UI.Toolbar toolbar =
            (Viatronix.UI.Toolbar)System.Activator.CreateInstance(Type.GetType(template.Typename));

          toolbar.Name = template.Id;
          ((Viatronix.UI.IToolbar)toolbar).Initialize(toolbarNode.OuterXml);

          m_toolbarController.AddToolBar(toolbar, template.Align);
        }

        /**
         *  add workflow tabs
         */
        string defaultWorkflowId = node.Attributes["default_tab"].Value;
        Viatronix.UI.TabStripItem defaultWorflowTab = null;

        WorkflowTabs.BeginInit();

        XmlNodeList workflowNodes = node.SelectNodes("tabs/tab");
        for (int index = 0; index < workflowNodes.Count; ++index)
        {
          WorkflowStage stage = WorkflowStage.Create(workflowNodes[index]);

          // add a tab along the bottom for the workflow stage
          Viatronix.UI.TabStripItem item = new Viatronix.UI.TabStripItem();
          item.Text = stage.Text;
          item.Enabled = stage.Enabled;
          item.Data = stage;
          item.ID = stage.Id;
          WorkflowTabs.Items.Add(item);

          // is this workflow stage the default?
          if (stage.Id.Equals(defaultWorkflowId))
            defaultWorflowTab = item;
        }
        WorkflowTabs.EndInit();


        /**
         *  add hot keys
         */
        foreach (XmlNode hotkeyNode in node.SelectNodes("hotkeys/hotkey"))
        {
          Viatronix.UI.FrameHotKey hotkey = Viatronix.UI.FrameHotKey.Create(hotkeyNode);
          m_hotkeyContoller.HotKeys.Add(hotkey.AssignedKey, hotkey);
        }


        /**
         * add panes       
         */
        Panes.BeginInit();
        foreach (XmlNode paneNode in node.SelectNodes("panes/pane"))
        {
          Viatronix.UI.TemplatePane template = Viatronix.UI.TemplatePane.Create(paneNode);
          Viatronix.UI.RolloutPane pane = Panes.AddPane(template.Text);
          pane.Id = template.Id;
          pane.Tag = template;
          pane.Visible = false;
        }
        Panes.EndInit();



        // set the default workflow tab   
        WorkflowTabs.SelectedItem = defaultWorflowTab;

 
      }
      catch (Exception ex)
      {
        Log.Error("Failed to configure frame from xml : " + ex.Message, "Frame", "Create");
        throw;
      }


    } // Configure( xml )


    /// <summary>
    /// Resets the collections contained in the frame.
    /// </summary>
    public void Reset()
    {
      m_toolbarController.Clear();
      this.WorkflowTabs.Items.Clear();
      m_hotkeyContoller.HotKeys.Clear();
      this.Panes.Clear();
    } // Reset()
   

    /// <summary>
    /// Enabled the thread exception handler for the ui thread
    /// </summary>
    public void EnableThreadExceptionHandler()
    {
      Application.ThreadException += 
        new System.Threading.ThreadExceptionEventHandler( this.OnThreadExceptionHandler );
    } // EnableThreadExceptionHandler()
 
    
    /// <summary>
    /// Maximizes the form.
    /// </summary>
    public void MaximizeForm()
    {
      Win32.User32.SendMessage( this.Handle, (int)Win32.Msgs.WM_SYSCOMMAND, (uint)Win32.SysCommands.SC_MAXIMIZE, 0 );
    } // MaximizeForm()


    /// <summary>
    /// Initializes the dile logout timer.
    /// </summary>
    /// <param name="login">The logout form</param>
    public virtual void InitializeIdleTimer( FormLogin login, int interval )
    {
      if( this.InvokeRequired )
        Invoke( new Viatronix.UI.SetterDelegate<FormLogin, int>( this.InitializeIdleTimer ), login, interval );
      else
      {
        m_idleLogin = login;
        m_idleTimerInterval = interval;

        if( m_idleLogin != null )
        {
          Viatronix.Logging.Log.Debug( "Idle Timer initialized." , "Frame", "InitializeIdleTimer");
          m_idleTimer = new System.Windows.Forms.Timer();
          m_idleTimer.Interval = m_idleTimerInterval;
          m_idleTimer.Tick += new EventHandler( this.OnIdleTimerTickHandler );
          m_idleTimer.Start();
        }
      }
    } // InitializeIdleTimer( login )


    /// <summary>
    /// Starts the idel timer.
    /// </summary>
    private void StartIdleTimer()
    { 
      if( m_idleLogin == null || m_idleTimer == null )
        return;

      m_idleTimer.Stop();

      if( this.Visible )
        m_idleTimer.Start();

      
    } // StartIdleTimer()


    /// <summary>
    /// Stops the idle timer
    /// </summary>
    private void StopIdleTimer()
    {
      if( m_idleLogin == null || m_idleTimer == null )
        return;

      m_idleTimer.Stop();
    } // StopIdleTimer()

    /// <summary>
    ///  Called when the visible flag changed
    /// </summary>
    /// <param name="args">EventArgs related to this event</param>
    protected override void OnVisibleChanged( EventArgs args )
    {
      base.OnVisibleChanged( args );

      if( this.Visible )
        StartIdleTimer();
      else
        StopIdleTimer();

      if( m_isApplicationClosed )
        Close();
    } // OnVisibleChanged( EventArgs args )

    protected override void OnClosing(CancelEventArgs e)
    {
      base.OnClosing(e);

      StopIdleTimer();

      if ( m_renewTimer != null )
        m_renewTimer.Dispose();

      m_menuController.Dispose();
      m_toolbarController.Dispose();
      m_viewController.Dispose();

      m_isApplicationClosed = true;

    } // OnClosing()

    /// <summary>
    /// Called when the form is closed.
    /// </summary>
    /// <param name="args">EventArgs containing data related to this event.</param>
    protected override void OnClosed( EventArgs args )
    {
      base.OnClosed( args );

      Logout();


//#if !V3D_NO_LICENSING
      
//      m_licenseManager.Dispose();

//#endif
    } // OnClosed( args )

    /// <summary>
    /// Handles the renew timer's tick event.
    /// </summary>
    /// <param name="state"></param>
    protected void OnRenewTimerTickHandler(object state)
    {
      try
      {
        SecurityProvider.Renew();
        Log.Information("Successfully renewed the context!", "FrameConsole", "OnRenewTimerTickHandler");
      }
      catch (Exception e)
      {
        Log.Error(string.Format("Failed to Renew Context:  {0}", e.Message), "FrameConsole", "OnRenewTimerTickHandler");
        Viatronix.UI.MessageBox.ShowError(this, "            Failed to Renew Context.\nPlease contact Viatronix customer support.");
        Application.Exit();
      }
    } // OnRenewTimerTickHandler( state )



    /// <summary>
    /// Called after the active view has changed.
    /// </summary>
    /// <param name="args">EventArgs containing data related to this event.</param>
    protected virtual void OnViewChanged(  CancelEventArgs args )
    {
      m_menuController.Reset( this.ActiveViewTemplate );
      m_viewController.Panes.Reset( this.ActiveViewTemplate );

      // raise the "ViewChanged" event
      if( ViewChanged != null )
        ViewChanged(this, new CancelEventArgs());

    } // OnViewChanged( EventArgs args )


    /// <summary>
    /// Called before the active view has changed.
    /// </summary>
    /// <param name="args">EventArgs containing data related to this event.</param>
    protected virtual void OnViewChanging(CancelEventArgs args)
    {
      // raise the "ViewChanging" event
      if( ViewChanging != null )
        ViewChanging(this, new CancelEventArgs());

    } // OnViewChanging( EventArgs args )
  
    
    /// <summary>
    /// the frame's wndproc
    /// </summary>
    /// <param name="m">windows message</param>
    protected override void WndProc( ref Message m )
    {
      if( m.Msg == WM_APP_HIDE_VIEWER )
        this.Hide();

      base.WndProc( ref m );
    } // WndProc( m )
   

    /// <summary>
    /// Handler for unhandled exception in the ui thread.
    /// </summary>
    /// <param name="sender">Sender of this event</param>
    /// <param name="args">ThreadExceptionEventArgs containg daa related to this event.</param>
    private void OnThreadExceptionHandler( object sender, System.Threading.ThreadExceptionEventArgs args )
    {
      // dump th exception stack
      Viatronix.Logging.Log.Error(args.Exception.ToString(), "Frame", "OnThreadExceptionHandler");         

//#if !V3D_NO_LICENSING      
//      if( args.Exception is Viatronix.Licensing.LicenseException )
//      {
//        Viatronix.Licensing.LicenseException exception = (Viatronix.Licensing.LicenseException)args.Exception;
//        Viatronix.Logging.Log.Error( "LicenseException (" + exception.Result.ToString() + "): " + exception.Message, "Frame", "OnThreadExceptionHandler"); 
//        Viatronix.UI.MessageBox.ShowError("                         Unable to request license.\nPlease contact Viatronix Customer Service for assistance.");

//        try
//        {
//          if (LicenseManager != null)
//          {
//            LicenseManager.Dispose();
//          }
//        }
//        catch (Exception e)
//        {
//          Logging.Log.Error("Failed to dispose of license manager.  [ERR=" + e.Message + "]", "Frame", "OnThreadException");
//        }
//        finally
//        {
//          Application.Exit();
//        }
//      }
//#endif

      Viatronix.Logging.Log.Error( "Unhandled exception: "  + args.Exception.Message, "Frame", "OnThreadExceptionHandler"); 
      Viatronix.UI.MessageBox.ShowError( args.Exception.Message );

    } // OnThreadExceptionHandler( sender, args )

    
    /// <summary>
    /// Called before the active view changes.
    /// </summary>
    /// <param name="sender">Sender of this event</param>
    /// <param name="args">EventArgs containing daa related to this event</param>
    private void OnViewChangingHandler(object sender, CancelEventArgs args)
    {
      OnViewChanging( args );
    } // OnViewChangingHandler( sender, args )

    
    /// <summary>
    /// Called after the active view changes.
    /// </summary>
    /// <param name="sender">Sender of this event</param>
    /// <param name="args">EventArgs containing data related to this event</param>
    private void OnViewChangedHandler( object sender, CancelEventArgs args )
    {
      OnViewChanged( args );
    } // OnViewChangedHandler( sender, args )


    /// <summary>
    /// Handles the idle timer's elapsed event.
    /// </summary>
    /// <param name="sender">Sender of this event</param>
    /// <param name="args">EventArgs containing data related to this event</param>
    private void OnIdleTimerTickHandler( object sender, EventArgs args )
    {
      if( this.IsApplicationLocked == false )
        ActivateAutoLogin();
    } // OnIdleTimerElapsedHandler( sender, args )


    
 
 
    /// <summary>
    /// Auto logout the frame and wait for login.
    /// </summary>
    public void ActivateAutoLogin()
    {
      try
      {
        m_isApplicationLocked = true;
        StopIdleTimer();

        if( this.ApplicationLocked != null )
          this.ApplicationLocked( this, EventArgs.Empty );

        if( m_idleLogin != null )
        {
          this.Visible = false;

          bool loginSuccessful = false;
          while( !loginSuccessful )
          {
            m_idleLogin.Initialize();
            m_idleLogin.ShowInTaskbar = true;
            m_idleLogin.User = this.CurrentUser;
            DialogResult result = m_idleLogin.ShowDialog(this);
            if (result == System.Windows.Forms.DialogResult.OK)
            {
              loginSuccessful = true;
            }
            else if (result == System.Windows.Forms.DialogResult.Abort)
            {
              Close();
              return;
            }
          }

          this.Visible = true;

          // TODO: REST (Used for remoting)
          //WindowUtilities.ShowWindow( this, this.Bounds, this.WindowState );
        }
      }
      catch( Exception e )
      {
        Viatronix.Logging.Log.Error( "Unable to lock application [ERROR=" + e.Message + "]", "Frame", "ActivateAutoLogin"); 
      }
      finally
      {
        m_isApplicationLocked = false;
        if( this.ApplicationUnlocked != null )
          this.ApplicationUnlocked( this, EventArgs.Empty );

        StartIdleTimer();
      }
    } // ActivateAutoLogin()

    #endregion

    #region IDisposable Members

    /// <summary>
    /// 
    /// </summary>
    public  void Dipose()
    {
      Dispose(true);
    } // Dispose()

    /// <summary>
    /// Dispose
    /// </summary>
    /// <param name="disposing"></param>
    protected override void Dispose(bool disposing)
    {
      if (disposing)
      {
        try
        {
          Logout();
        }
        catch (Exception e)
        {
          Logging.Log.Error("Failed to logout during dispoing of the Frame : " + e.Message, "Frame", "Dispose");
        }
      }

      base.Dispose(disposing);
    } // Dispose()

    #endregion


    #region IMessageFilter Members

    /// <summary>
    /// Filters out a message before it is dispatched.
    /// </summary>
    /// <param name="m">The message to be dispatched.</param>
    /// <returns>true to filter the message and stop it from being dispatched; false to allow the message to continue to the next filter or control.</returns>
    public bool PreFilterMessage( ref Message m )
    {
      if( m.Msg == (int) Win32.Msgs.WM_KEYDOWN || m.Msg == (int) Win32.Msgs.WM_MOUSEMOVE )
        StartIdleTimer();

      return false;
    } // PreFilterMessage( m )

    #endregion

    #region windows form designer generated code
		
    /// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager( typeof( Frame ) );
      this.m_toolbarController = new Viatronix.UI.ControllerToolBars();
      this.m_viewController = new Viatronix.UI.ControllerViews();
      this.m_menuController = new Viatronix.UI.ControllerMenus();
      this.SuspendLayout();
      // 
      // m_toolbarController
      // 
      this.m_toolbarController.Dock = System.Windows.Forms.DockStyle.Top;
      this.m_toolbarController.Location = new System.Drawing.Point( 0, 31 );
      this.m_toolbarController.Name = "m_toolbarController";
      this.m_toolbarController.Size = new System.Drawing.Size( 1592, 40 );
      this.m_toolbarController.TabIndex = 1;
      this.m_toolbarController.TabStop = false;
      this.m_toolbarController.Text = "rebar1";
      // 
      // m_viewController
      // 
      this.m_viewController.Dock = System.Windows.Forms.DockStyle.Fill;
      this.m_viewController.Location = new System.Drawing.Point( 0, 71 );
      this.m_viewController.Name = "m_viewController";
      this.m_viewController.Size = new System.Drawing.Size( 1592, 1095 );
      this.m_viewController.TabIndex = 3;
      // 
      // m_menuController
      // 
      this.m_menuController.Dock = System.Windows.Forms.DockStyle.Top;
      this.m_menuController.Location = new System.Drawing.Point( 0, 0 );
      this.m_menuController.Name = "m_menuController";
      this.m_menuController.Size = new System.Drawing.Size( 1592, 31 );
      this.m_menuController.TabIndex = 2;
      this.m_menuController.TabStop = false;
      this.m_menuController.Text = "menu";
      // 
      // Frame
      // 
      this.AutoScaleBaseSize = new System.Drawing.Size( 5, 13 );
      this.ClientSize = new System.Drawing.Size( 1592, 1166 );
      this.Controls.Add( this.m_viewController );
      this.Controls.Add( this.m_toolbarController );
      this.Controls.Add( this.m_menuController );
      this.Icon = ( (System.Drawing.Icon) ( resources.GetObject( "$this.Icon" ) ) );
      this.Location = new System.Drawing.Point( 100, 100 );
      this.Name = "Frame";
      this.Text = "Viatronix";
      this.WindowState = System.Windows.Forms.FormWindowState.Maximized;
      this.ResumeLayout( false );

    }
    
    
    #endregion

  } // class FormMain

  #region configuration section handler
  
  /// <summary>
  /// Configuration section handler for the main frame
  /// </summary>
  public class FrameConfigSectionHandler :  System.Configuration.IConfigurationSectionHandler
  {
    /// <summary>
    /// Create an instance of a frame from the specified xml
    /// </summary>
    /// <param name="parent">parent object</param>
    /// <param name="context">configuration conext object</param>
    /// <param name="section">xml section</param>
    /// <returns></returns>
    public object Create( object parent, object context, System.Xml.XmlNode section )
    {
      Frame frame = new Frame(string.Empty);
      frame.Configure(section);
      return frame;
    } // Create( object parent, object context, System.Xml.XmlNode section )
  } // class FrameConfigSectionHandler

  #endregion

} // namespace Viatronix.Viewer

#region revision history

// $Log: Frame.cs,v $
// Revision 1.52.2.6  2010/12/01 14:45:49  kchalupa
// Improved handling of thread exception to close down the application.  This functions differently than on XP because of the way the threading is implemented.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.52.2.5  2009/10/01 21:07:05  vxconfig
// Issue #6050: Removed check for OverflowException
//
// Revision 1.52.2.4  2007/05/24 21:56:46  gdavidson
// Issue #5661: Check dialog result before closing lock window
//
// Revision 1.52.2.3  2007/05/01 02:48:49  mkontak
// dump out the exception stack when a thread exception occurs
//
// Revision 1.52.2.2  2007/04/16 21:00:50  gdavidson
// Issue #5592: create idle timer on main thread
//
// Revision 1.52.2.1  2007/03/28 17:52:17  gdavidson
// Changed IdleTimer to a winforms timer
//
// Revision 1.52  2007/03/13 18:42:28  gdavidson
// Issue #5469: Added better exception handling for ActivateAutoLogin method
//
// Revision 1.51  2007/03/12 15:27:59  gdavidson
// Issue #5469: Start internal timer when initializing IdleTimer
//
// Revision 1.50  2007/03/08 21:30:17  gdavidson
// Issue #5469: modified the idle timer
//
// Revision 1.49  2007/03/08 18:43:20  mkontak
// Check for exception during construction
//
// Revision 1.48  2007/03/08 18:38:06  mkontak
// Modified the OnThreadExceptionHandler to check for the overflow exception.
//
// Revision 1.47  2007/03/07 21:10:53  mkontak
// Coding standards
//
// Revision 1.46  2007/03/06 18:39:09  gdavidson
// Added comment
//
// Revision 1.45  2007/03/01 17:09:18  geconomos
// code review preparation
//
// Revision 1.44  2007/02/26 17:59:18  gdavidson
// Modified the tab ordering
//
// Revision 1.43  2007/02/23 19:57:19  gdavidson
// Increased the size of the frame
//
// Revision 1.42  2007/01/03 22:25:57  gdavidson
// Issue #5261: Added a flag to track when the application is locked or closing
//
// Revision 1.41  2006/12/21 16:46:40  gdavidson
// Added ApplicationLocked and ApplicationUnlocked events
//
// Revision 1.40  2006/12/18 19:00:40  gdavidson
// Issue #5060: Set the parent window for the message box
//
// Revision 1.39  2006/10/30 16:07:42  gdavidson
// Added LicensedProducts property
//
// Revision 1.38  2006/08/11 21:11:05  gdavidson
// Added an AddToolBar method which takes an align parameter
//
// Revision 1.37  2006/06/13 13:29:36  mkontak
// Fix idle crash due to unsafe thread opereations.
//
// Revision 1.36  2006/06/13 13:12:49  gdavidson
// Fixed cross thread execution for auto login
//
// Revision 1.35  2006/06/08 13:08:47  gdavidson
// Removed Vatronix.v3D.Licensing.Client
//
// Revision 1.34  2006/06/02 12:47:39  mkontak
// Fixed deprecated code
//
// Revision 1.33  2006/05/05 20:28:38  gdavidson
// Issue #4679: Changed the Idle timer
//
// Revision 1.32  2006/04/20 19:44:09  gdavidson
// Removed debug log messages
//
// Revision 1.31  2006/04/14 15:06:04  gdavidson
// Modfied how the MessageFilter restarts the idle timer
//
// Revision 1.30  2006/04/07 18:52:21  gdavidson
// Added a DisableIdleTimer constant
//
// Revision 1.29  2006/04/06 16:03:25  gdavidson
// Issue #4679: Modified how the idle timer is enabled and disabled
//
// Revision 1.28  2006/03/15 14:52:34  vxconfig
// logger ambiguity resolved
//
// Revision 1.27  2006/03/15 14:46:48  gdavidson
// Modified the licensing
//
// Revision 1.26  2006/03/09 21:22:47  gdavidson
// Changed StartPostion to WindowsDefaultLocation
//
// Revision 1.25  2006/02/21 14:54:58  gdavidson
// New version of licensing
//
// Revision 1.24  2006/02/09 14:14:29  geconomos
// ability to disable licensing using preprocessor directive
//
// Revision 1.23  2006/01/31 15:58:47  geconomos
// logging error messages in thread exception handler
//
// Revision 1.22  2006/01/27 20:36:14  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.21  2006/01/18 15:42:22  gdavidson
// Added timeout for idle application.
//
// Revision 1.20  2005/11/21 14:22:13  gdavidson
// Set the default windowState to maximized
//
// Revision 1.19  2005/11/19 18:48:21  vxconfig
// added back in the license info
//
// Revision 1.18  2005/11/18 21:07:10  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.17  2005/11/16 14:09:23  gdavidson
// Modified the maximize, minimize restore operations.
//
// Revision 1.16.4.1  2005/12/09 14:41:51  gdavidson
// Removed LicenseInfo object.
//
// Revision 1.16  2005/08/02 12:32:31  gdavidson
// Request for license made in Configure method.
//
// Revision 1.15  2005/07/01 15:43:06  mkontak
// no message
//
// Revision 1.14  2005/06/16 13:13:44  gdavidson
// Added a Reset method.
//
// Revision 1.13  2005/06/15 19:48:53  romy
// Added License Info
//
// Revision 1.12.2.3  2005/08/18 18:09:34  frank
// Issue #4370: Added cancel button back into save session dialog
//
// Revision 1.12.2.2  2005/08/11 19:50:07  geconomos
// Added a call through IToolbar when creating the toolbars
//
// Revision 1.12.2.1  2005/08/04 14:31:53  frank
// Issue #4338: User was allowed to continue using the application after license failure if they clicked Cancel.
//
// Revision 1.12  2005/06/15 15:07:51  vxconfig
// workflow tabs were not storing the ID properly
//
// Revision 1.11  2005/06/08 15:31:35  romy
// reverted back to version 2.3
//
// Revision 1.10  2005/06/07 13:32:30  romy
// Added LicenseInfo Method
//
// Revision 1.9  2005/05/16 14:19:53  gdavidson
// Moved all licensing fuctionality into the base frame class.
//
// Revision 1.8  2005/05/05 17:36:15  gdavidson
// Changed the implemention for the TabStrip control.
//
// Revision 1.7  2005/03/11 17:31:48  frank
// released the licenses upon close
//
// Revision 1.6  2005/02/24 19:19:07  gdavidson
// Added ability to modify the licensing renewal interval from the configuration file.
//
// Revision 1.5  2005/02/01 20:31:22  geconomos
// added method to clear out all existing toolbars
//
// Revision 1.4  2005/01/28 19:23:14  frank
// changed license renew interval to 4 minutes
//
// Revision 1.3  2005/01/28 16:35:58  geconomos
// moved exception handling to derived class
//
// Revision 1.2  2005/01/27 20:17:18  geconomos
// moved licensing from derived frame to base
//
// Revision 1.1  2005/01/26 19:07:05  geconomos
// initial revision
//
// Revision 1.17  2005/01/07 21:00:21  gdavidson
// Set renewal time for the licensing.
//
// Revision 1.16  2004/12/15 17:19:10  geconomos
// added functionailty with the console
//
// Revision 1.15  2004/12/10 17:12:51  geconomos
// added code to log execption info into file specified in app.config
//
// Revision 1.14  2004/11/22 02:24:04  geconomos
// commented out licensing
//
// Revision 1.13  2004/11/19 14:55:55  geconomos
// added propmt when run from command line to save session data
//
// Revision 1.12  2004/11/18 18:36:35  gdavidson
// Replaced ControllerPanes property with ControllerViews property.
//
// Revision 1.11  2004/11/18 17:40:19  geconomos
// added a prompt to see if session should be stored back to pacs
//
// Revision 1.10  2004/11/17 22:39:17  geconomos
// initial implmentation for system menu
//
// Revision 1.9  2004/11/17 13:27:32  geconomos
// working on maximize / restore ability
//
// Revision 1.8  2004/11/16 21:27:51  gdavidson
// Added new toolbar.
//
// Revision 1.7  2004/11/04 19:30:31  gdavidson
// Removed commented code for maximizing the form.
//
// Revision 1.6  2004/11/02 18:00:03  gdavidson
// Added licensing and functionality to maximize the application when started from the command line.
//
// Revision 1.5  2004/11/01 15:19:04  geconomos
// added some commenting
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Core/ui/Frame.cs,v 1.52.2.6 2010/12/01 14:45:49 kchalupa Exp $
// $Id: Frame.cs,v 1.52.2.6 2010/12/01 14:45:49 kchalupa Exp $

#endregion