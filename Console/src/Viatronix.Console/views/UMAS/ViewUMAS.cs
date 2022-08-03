// $Id: ViewUMAS.cs,v 1.28.2.4 2009/10/15 15:20:58 kchalupa Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Romy Sreedharan ( mailto:romy@viatronix.com )

using System;
using System.Xml;
using System.Data;
using System.Drawing;
using System.Reflection;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Windows.Forms;
using System.Configuration;

using Viatronix.UI;
using Viatronix.Enterprise.Models;
using Viatronix.Enterprise.Entities;
using Viatronix.Enterprise.SDK.Providers;

namespace Viatronix.Console
{
  /// <summary>
  /// User Interface for Displaying the Users and Roles information
  /// </summary>
  public class ViewUMAS : UserControl, IView, IPersistable
  {

    #region fields

    /// <summary>
    /// Image index for service user.
    /// </summary>
    private static readonly int SERVICE_IMAGE_INDEX = -1;

    /// <summary>
    /// Image index for administrator user.
    /// </summary>
    private static readonly int ADMINISTRATOR_IMAGE_INDEX = -1;

    /// <summary>
    /// Image index for operator user.
    /// </summary>
    private static readonly int OPERATOR_IMAGE_INDEX = -1;

    /// <summary>
    /// Image index for physician user.
    /// </summary>
    private static readonly int PHYSICIAN_IMAGE_INDEX = -1;

    /// <summary>
    /// Image index for user.
    /// </summary>
    private static readonly int USER_IMAGE_INDEX = -1;

    /// <summary>
    /// Security provider.
    /// </summary>
    private ISecurityProvider m_security = null;

    /// <summary>
    /// The model.
    /// </summary>
    private UMASModel m_model = null;

    /// <summary>
    /// ArrayList for Users
    /// </summary>
    private List<User> m_users;

    /// <summary>
    /// Window components.
    /// </summary>
    private System.ComponentModel.IContainer components;

    /// <summary>
    /// List View object for users
    /// </summary>
    private Viatronix.UI.DynamicListView m_viewUsers;

    /// <summary>
    /// Small Image View List
    /// </summary>
    private static readonly ImageList m_usersListViewImageList = new ImageList ( );

    /// <summary>
    /// Label object for User
    /// </summary>
    private System.Windows.Forms.Label m_labelUser;

    /// <summary>
    /// Maintains the path to the user view settings.
    /// </summary>
    private readonly string m_userViewSettingsPath = string.Empty;

    /// <summary>
    /// Event handler for the view changing event.
    /// </summary>
    private CancelEventHandler m_viewChangingHandler = null;

    /// <summary>
    /// User View Panel
    /// </summary>
    private Viatronix.UI.Panel m_userViewPanel;

    /// <summary>
    /// Event handler for the closing event.
    /// </summary>
    private CancelEventHandler m_closingHandler = null;

    #endregion

    #region construction

    /// <summary>
    /// Static Constructor which Loads the Resources
    /// </summary>
    static ViewUMAS()
    {
      Assembly assembly = Assembly.GetExecutingAssembly();

      //Users's view Small Image List
      m_usersListViewImageList.ImageSize = new Size(12, 12);
      USER_IMAGE_INDEX          = m_usersListViewImageList.Images.Add(Bitmap.FromStream(assembly.GetManifestResourceStream("Viatronix.Console.res.user.ico")), Color.Transparent);
      OPERATOR_IMAGE_INDEX      = m_usersListViewImageList.Images.Add(Bitmap.FromStream(assembly.GetManifestResourceStream("Viatronix.Console.res.operator.ico")), Color.Transparent);
      PHYSICIAN_IMAGE_INDEX     = m_usersListViewImageList.Images.Add(Bitmap.FromStream(assembly.GetManifestResourceStream("Viatronix.Console.res.physician.ico")), Color.Transparent);
      ADMINISTRATOR_IMAGE_INDEX = m_usersListViewImageList.Images.Add(Bitmap.FromStream(assembly.GetManifestResourceStream("Viatronix.Console.res.administrator.ico")), Color.Transparent);
      SERVICE_IMAGE_INDEX       = m_usersListViewImageList.Images.Add(Bitmap.FromStream(assembly.GetManifestResourceStream("Viatronix.Console.res.service.ico")), Color.Transparent);
    } // ViewUMAS()


    /// <summary>
    /// Constructor ViewUMAS()
    /// </summary>
    public ViewUMAS()
    {
      SetStyle(ControlStyles.AllPaintingInWmPaint, true);
      SetStyle(ControlStyles.DoubleBuffer, true);
      SetStyle(ControlStyles.UserPaint, true);
      SetStyle(ControlStyles.SupportsTransparentBackColor, true);

      // This call is required by the Windows.Forms Form Designer.
      InitializeComponent();

      m_security = Global.SecurityProvider;
      m_model = new UMASModel(m_security);

      //Add  roles
      m_viewUsers.AllowColumnReorder = true;

      // Load the images in an ImageList.
      m_viewUsers.SmallImageList = m_usersListViewImageList;

      m_viewUsers.LargeImageList = m_usersListViewImageList;

      //m_viewUsers.LargeImageList.ImageSize = new Size(20,20);
      //m_viewRoles.LargeImageList.ImageSize = new Size(20,20);

      m_userViewSettingsPath = System.IO.Path.Combine(Global.CurrentUser.PreferencesDirectory, "View_User.xml");
    } // ViewUMAS()


    /// <summary> 
    /// Clean up any resources being used.
    /// </summary>
    protected override void Dispose(bool disposing)
    {
      if (disposing)
      {
        if (components != null)
        {
          components.Dispose();
        }
      }

      base.Dispose(disposing);
    }//Dispose( disposing )

    #endregion

    #region properties
 
    /// <summary>
    /// Gets or sets the Roles
    /// </summary>
    private List<User> Users
    {
      get { return m_model.Users; }
    } // SystemRigts

    #endregion

    #region methods
 
    /// <summary>
    /// Prepares the Users List View
    /// </summary>
    private void PrepareUserView()
    {
      //Get All Users from the database.
      m_viewUsers.Items.Clear();

      foreach (User user in m_model.Users)
      {
        System.Windows.Forms.ListViewItem item = m_viewUsers.AddItem(user);
        if ( user.IsOperator )
        {
          item.ImageIndex = OPERATOR_IMAGE_INDEX;
        }
        else if ( user.IsPhysician )
        {
          item.ImageIndex = PHYSICIAN_IMAGE_INDEX;
        }
        else if ( user.IsAdministrator )
        {
          item.ImageIndex = ADMINISTRATOR_IMAGE_INDEX;
        }
        else if ( user.IsService )
        {
          item.ImageIndex = SERVICE_IMAGE_INDEX;
        }
        else
        {
          item.ImageIndex = USER_IMAGE_INDEX;
        }
      }
    } // PrepareUserView()
    

    /// <summary>
    /// Get the role from the database for the selected role
    /// </summary>
    /// <returns>User Object</returns>
    private User GetSelectedUser()
    {
      foreach ( ListViewItem item in m_viewUsers.SelectedItems )
        return ( User ) item.Tag;

      return new User ( );
    } // GetSelectedUser()
    

    /// <summary>
    /// AdministerUsers, Administers the User Object
    /// </summary>
    /// <param name="user">User Object</param>
    private void AdministerUsers(User user)
    {
      try
      {
        // Only need to get the user if it is 
        if (!string.IsNullOrEmpty(user.Login))
        { 
          user = m_model.GetUser(user.Login);
        }
      }
      catch ( Exception )
      {
        // Ignore
      }

      using (FormUserManagement userManagement = new FormUserManagement(m_security, user, m_model.Roles))
      {
        userManagement.ShowDialog(this);
      }
       
      PrepareUserView();
    }//AdministerUsers( user )


    /// <summary>
    /// ShowUserManagementMenu, Menu items for User Management
    /// </summary>
    /// <param name="posx">int</param>
    /// <param name="posy">int</param>
    private void OnPopup(int posx, int posy)
    {
      bool loggedInUserSelected = false;
      Viatronix.UI.ContextMenu contextMenuUsers = new Viatronix.UI.ContextMenu ( );

      System.Windows.Forms.MenuItem menuItemNewUser = new System.Windows.Forms.MenuItem ( );
      System.Windows.Forms.MenuItem menuItemModifyUser = new System.Windows.Forms.MenuItem ( );
      System.Windows.Forms.MenuItem menuItemDeleteUser = new System.Windows.Forms.MenuItem ( );
      System.Windows.Forms.MenuItem menuItemChangePassword = new System.Windows.Forms.MenuItem ( );
      System.Windows.Forms.MenuItem menuItemUnlockUser = new System.Windows.Forms.MenuItem();

      ListViewItem item = this.m_viewUsers.GetItemAt ( posx, posy );

      bool allowDelete = true;
      bool isLocked = false;
      if ( item != null )
      {
        User user = ( User ) item.Tag;

        allowDelete = ( ! user.IsLoggedIn );
        isLocked    = user.IsLoggedIn;

        if (Global.CurrentUser.Login == user.Login)
        {
          loggedInUserSelected = true;
        }

        if (  ! user.ViatronixAuthentication )
          menuItemChangePassword.Enabled = false;
      }

      menuItemNewUser.Index = 0;
      menuItemNewUser.Text = "New User...";
      menuItemNewUser.Click += new System.EventHandler ( this.OnUserManageClickHandler );

      menuItemModifyUser.Index = 1;
      menuItemModifyUser.Text = "Properties...";
      menuItemModifyUser.Click += new System.EventHandler ( this.OnUserManageClickHandler );

      menuItemDeleteUser.Index = 2;
      menuItemDeleteUser.Text = "Delete User";
      menuItemDeleteUser.Click += new System.EventHandler(this.OnDeleteUserClickHandler);

      menuItemChangePassword.Index = 3;
      menuItemChangePassword.Text = "Change Password...";
      menuItemChangePassword.Click += new System.EventHandler ( this.OnChangePasswordClickHandler );

      menuItemUnlockUser.Index = 4;
      menuItemUnlockUser.Text = "Unlock User";
      menuItemUnlockUser.Visible = false;
      menuItemUnlockUser.Click += new System.EventHandler(this.OnUnlockUserClickHandler);

      if (item != null && item.Tag != null)
      {
        User user = (User)item.Tag;

        if (user.IsService && !Global.CurrentUser.IsService)
        {
          menuItemNewUser.Enabled = false;
          menuItemModifyUser.Enabled = false;
          menuItemDeleteUser.Enabled = false;
          menuItemChangePassword.Enabled = false;
          menuItemUnlockUser.Enabled = false;
        }
      }

      contextMenuUsers.MenuItems.AddRange ( new System.Windows.Forms.MenuItem [ ] {
        menuItemNewUser,
        menuItemModifyUser,
        menuItemDeleteUser,
        menuItemChangePassword,
        menuItemUnlockUser 
      });
      
      System.Drawing.Point pos = new Point ( posx, posy );

      //Show Modify & Delete only if there is a single selected item.
      if ( m_viewUsers.SelectedItems.Count == 1 )
      {
        menuItemNewUser.Visible = false;
        //Check for User Management Right
        if ( Global.CurrentUser.IsAllowed ( "UserManagement" ) )
        {
          menuItemModifyUser.Visible = true;
          menuItemDeleteUser.Visible = allowDelete;
          if ( Global.CurrentUser.ViatronixAuthentication ) menuItemChangePassword.Visible = true;
          if ( isLocked ) menuItemUnlockUser.Visible = true;

          if (loggedInUserSelected)
          {
            menuItemUnlockUser.Enabled = false;
            menuItemDeleteUser.Enabled = false;
          }
        }
        else
        {
          menuItemModifyUser.Visible = false;
          menuItemDeleteUser.Visible = false;

          if (Global.CurrentUser.ViatronixAuthentication && Global.CurrentUser.Login == GetSelectedUser().Login)
            menuItemChangePassword.Visible = true;
          else
            menuItemChangePassword.Visible = false;
        }
      }
      else
      {
        if ( Viatronix.Console.Global.CurrentUser.IsAllowed ( "UserManagement" ) )
          menuItemNewUser.Visible = true;
        else
          menuItemNewUser.Visible = false;

        menuItemUnlockUser.Visible = false;
        menuItemModifyUser.Visible = false;
        menuItemDeleteUser.Visible = false;
        menuItemChangePassword.Visible = false;
      }
      contextMenuUsers.Show ( m_viewUsers, pos );

    } // OnPopup( posx, posy )
    
    #endregion

    #region event handlers

    /// <summary>
    /// Handles the On Double click on the List View Control.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnViewUserItemDoubleClickHandler(object sender, System.EventArgs args)
    {
      // Check whether the current user has permissions to Modify a User and that if 
      // the selected user is a service user the logged in user must be a service user.
      if (!Viatronix.Console.Global.CurrentUser.IsAllowed("UserManagement") || (GetSelectedUser().IsService && !Global.CurrentUser.IsService))
      {
        Viatronix.UI.MessageBox.ShowWarning("You do not have access to modify the selected user.");
        return;
      }

      AdministerUsers(GetSelectedUser());
    } // OnViewUserItemDoubleClickHandler( object sender, EventArgs args )


    /// <summary>
    /// Saves out the user preferences for the listview controls.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnFrameClosingHandler(object sender, CancelEventArgs args)
    {
      OnViewChangingHandler(sender, EventArgs.Empty);
    } // OnFrameClosingHandler( sender, args )


    /// <summary>
    /// Saves out the user preferences for the listview controls.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnViewChangingHandler(object sender, EventArgs args)
    {
      Global.Frame.ViewChanging -= m_viewChangingHandler;
      Global.Frame.Closing -= m_closingHandler;

      m_viewUsers.Save(m_userViewSettingsPath);
    } // OnViewChangingHandler( sender, args )


    /// <summary>
    /// Handles the Context Menu display on mouse button up event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnViewUserMouseUpHandler(object sender, MouseEventArgs args)
    {
      if (args.Button == MouseButtons.Right)
      {
        OnPopup(args.X, args.Y);
      }
    } // OnViewUserMouseUpHandler( sender, args )


    /// <summary>
    /// Handles the User Managemnet for New/Update user events.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnUserManageClickHandler(object sender, EventArgs args)
    {
      AdministerUsers(GetSelectedUser());
    } // OnUserManageClickHandler( sender, args )


    /// <summary>
    /// OnDeleteUser, Deletes the Selected User from the Database.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnDeleteUserClickHandler(object sender, EventArgs args)
    {
      User user = GetSelectedUser();
      if (Viatronix.UI.MessageBox.ShowQuestion(Global.Frame, "Are you sure you want to delete the user: " + user.Login + "?", System.Windows.Forms.MessageBoxButtons.YesNo) == DialogResult.Yes)
      {
        m_model.Delete(user);

        //Update the User Tree View.
        PrepareUserView(); //InStead add an Item to the Tree View and Add an element in the User.
      }
    }//void OnDeleteUserClickHandler( sender, args )


    /// <summary>
    /// Change Password Method Menu event Handler
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnUnlockUserClickHandler(object sender, System.EventArgs args)
    {
      User user = GetSelectedUser();

      m_model.UnlockUser(user);

      PrepareUserView();
    } // OnUnlockUserClickHandler( sender, args )


    /// <summary>
    /// Change Password Method Menu event Handler
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnChangePasswordClickHandler(object sender, EventArgs args)
    {
      User user = null;
      if ((user = GetSelectedUser()) != null)
      {
        Viatronix.UI.FormChangePassword formChangePassword = new Viatronix.UI.FormChangePassword(m_security, Global.CurrentUser, user);
        formChangePassword.ShowDialog(this);
        formChangePassword.Dispose();
      }
    } // OnChangePasswordClickHandler( sender, args )

    #endregion

    #region IView implementation

    /// <summary>
    /// Called when the view is initially created.
    /// </summary>
    /// <param name="xml">Additional xml configuration data</param>
    void IView.Initialize(string xml)
    {
      // load the columns for the listview controls
      XmlDocument doc = new XmlDocument();
      doc.LoadXml(xml);

      XmlNode columnsNode = (XmlNode)ConfigurationManager.GetSection("dynamicColumns");
      XmlNode listviewNode = doc.DocumentElement.SelectSingleNode("listviews");

      //Load the Status Views with user preferences 
      string xpath = "columns[ @id='{0}' ]";
      XmlNode userViewColumns = columnsNode.SelectSingleNode(string.Format(xpath, new object[] { listviewNode.Attributes["userView"].Value }));
      m_viewUsers.Load(userViewColumns, m_userViewSettingsPath);


      m_closingHandler = new CancelEventHandler(this.OnFrameClosingHandler);
      m_viewChangingHandler = new CancelEventHandler(this.OnViewChangingHandler);

      Global.Frame.ViewChanging += m_viewChangingHandler;
      Global.Frame.Closing += m_closingHandler;

      //Load Users
      PrepareUserView();
    } // Initialize( string xml )

    #endregion

    #region IPersistable implementation

    /// <summary>
    /// Allows the view to load any persisted state.
    /// </summary>
    /// <param name="viewState">The view's persistable state.</param>
    void IPersistable.LoadViewState(Dictionary<string, object> viewState)
    {
      object state = null;
      if (viewState.TryGetValue("UserViewPanel", out state))
        m_userViewPanel.Size = (System.Drawing.Size)state;
    } // LoadViewState( viewState )


    /// <summary>
    /// Allows the view to persist its state.
    /// </summary>
    /// <param name="viewState">The view's persisted state.</param>
    void IPersistable.SaveViewState(Dictionary<string, object> viewState)
    {
      viewState["UserViewPanel"] = m_userViewPanel.Size;
    } // SaveViewState( viewState )

    #endregion

    #region Component Designer generated code
    /// <summary> 
    /// Required method for Designer support - do not modify 
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      this.m_userViewPanel = new Viatronix.UI.Panel();
      this.m_labelUser = new System.Windows.Forms.Label();
      this.m_viewUsers = new Viatronix.UI.DynamicListView();
      this.m_userViewPanel.SuspendLayout();
      this.SuspendLayout();
      this.BackColor = System.Drawing.Color.FromArgb(((System.Byte)(148)), ((System.Byte)(166)), ((System.Byte)(198)));
      // 
      // m_userViewPanel
      // 
      this.m_userViewPanel.BackColor = System.Drawing.Color.FromArgb(((System.Byte)(148)), ((System.Byte)(166)), ((System.Byte)(198)));
      this.m_userViewPanel.BorderColor = System.Drawing.Color.White;
      this.m_userViewPanel.BottomPadding = 0;
      this.m_userViewPanel.Controls.Add(this.m_labelUser);
      this.m_userViewPanel.Controls.Add(this.m_viewUsers);
      this.m_userViewPanel.Curve = 10;
      this.m_userViewPanel.Dock = System.Windows.Forms.DockStyle.Top;
      this.m_userViewPanel.Edges = Viatronix.UI.Edges.All;
      this.m_userViewPanel.FillColor = System.Drawing.Color.FromArgb(((System.Byte)(123)), ((System.Byte)(134)), ((System.Byte)(165)));
      this.m_userViewPanel.LeftPadding = 0;
      this.m_userViewPanel.Location = new System.Drawing.Point(0, 0);
      this.m_userViewPanel.Name = "m_userViewPanel";
      this.m_userViewPanel.RightPadding = 0;
      this.m_userViewPanel.Size = new System.Drawing.Size(864, 1014);
      this.m_userViewPanel.TabIndex = 2;
      this.m_userViewPanel.TopPadding = 0;
      this.m_userViewPanel.Dock = DockStyle.Fill;
      // 
      // m_labelUser
      // 
      this.m_labelUser.BackColor = System.Drawing.Color.Transparent;
      this.m_labelUser.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_labelUser.ForeColor = System.Drawing.SystemColors.ControlLightLight;
      this.m_labelUser.Location = new System.Drawing.Point(12, 8);
      this.m_labelUser.Name = "m_labelUser";
      this.m_labelUser.TabIndex = 1;
      this.m_labelUser.Text = "Users";
      this.m_labelUser.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // m_viewUsers
      // 
      this.m_viewUsers.AllowGroupToggle = false;
      this.m_viewUsers.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
        | System.Windows.Forms.AnchorStyles.Left)
        | System.Windows.Forms.AnchorStyles.Right)));
      this.m_viewUsers.ShowGroups = false;
      this.m_viewUsers.FullRowSelect = true;
      this.m_viewUsers.GridLines = true;
      this.m_viewUsers.HideSelection = false;
      this.m_viewUsers.Location = new System.Drawing.Point(12, 32);
      this.m_viewUsers.MultiSelect = false;
      this.m_viewUsers.Name = "m_viewUsers";
      this.m_viewUsers.Size = new System.Drawing.Size(840, 968);
      this.m_viewUsers.TabIndex = 0;
      this.m_viewUsers.Font = new System.Drawing.Font("Microsoft Sans Serif", 10);
      this.m_viewUsers.View = System.Windows.Forms.View.Details;
      this.m_viewUsers.DoubleClick += new System.EventHandler(this.OnViewUserItemDoubleClickHandler);
      this.m_viewUsers.MouseUp += new System.Windows.Forms.MouseEventHandler(this.OnViewUserMouseUpHandler);
      // 
      // ViewUMAS
      // 
      this.Controls.Add(this.m_userViewPanel);
      this.Name = "ViewUMAS";
      this.Size = new System.Drawing.Size(864, 672);
      this.m_userViewPanel.ResumeLayout(false);
      this.ResumeLayout(false);

    }
    #endregion Component Designer generated code

  } // class ViewUMAS
} // namespace Viatronix.Console

#region revision history

// $Log: ViewUMAS.cs,v $
// Revision 1.28.2.4  2009/10/15 15:20:58  kchalupa
// Need to check that item has been selected before using it's tag.
//
// Revision 1.28.2.3  2009/10/14 19:46:17  kchalupa
// Only Service users can modify Service User.  Also no one can modify the Service Role.
//
// Revision 1.28.2.2  2008/02/14 20:50:41  mkontak
// Issue 6015
//
// Revision 1.28.2.1  2007/05/11 19:10:33  mkontak
// Issue 5653
//
// Revision 1.28  2007/02/28 19:58:16  romy
// added comments before the code review
//
// Revision 1.27  2007/02/13 16:05:51  gdavidson
// Issue #5387: update currently logged in user when changing its properties
//
// Revision 1.26  2007/01/04 19:53:04  mkontak
// Issue 5262: Modified so that the user cannot be deleted if they are active or logged in
//
// Revision 1.25  2006/10/30 16:18:19  gdavidson
// Impliments Viatronix.UI.IPersistable
//
// Revision 1.24  2006/08/11 19:44:26  romy
// fixed Null user check
//
// Revision 1.23  2006/08/09 14:40:56  romy
// fixed mult viewer issue during Role and User admin
//
// Revision 1.22  2006/07/24 18:51:04  mkontak
// Issue 4393
//
// Revision 1.21  2006/06/28 18:05:38  romy
// Change password fix
//
// Revision 1.20  2006/06/06 19:49:00  mkontak
// changed deprecated code
//
// Revision 1.19  2006/05/19 17:41:22  mkontak
// Removed the default domain, modified to have the domain selectable only.
//
// Revision 1.18  2006/04/14 17:13:34  romy
// fixed GUI standards issue
//
// Revision 1.17  2006/03/13 14:06:48  gdavidson
// Set the parent form for the update role dialog before showing it
//
// Revision 1.16  2006/03/10 20:08:30  romy
// GUI Doctor made a quick visit
//
// Revision 1.15  2006/03/03 14:58:49  gdavidson
// Set the parent form for the update role dialog before showing it
//
// Revision 1.14  2006/02/17 19:20:49  romy
// row height adjusted
//
// Revision 1.13  2006/02/09 21:49:08  romy
// Bug fixes
//
// Revision 1.12  2006/02/09 19:03:50  mkontak
// User/Roles icons
//
// Revision 1.11  2006/02/07 15:02:00  romy
// keeps the size in cache
//
// Revision 1.10  2005/12/07 17:57:24  romy
// changed the popup menu text
//
// Revision 1.9  2005/12/06 20:04:22  romy
// removed non referenced local variables
//
// Revision 1.8  2005/12/02 02:23:08  romy
// coding standards and minor fixes
//
// Revision 1.7  2005/11/23 11:53:18  mkontak
// Using new MessageBox implementation
//
// Revision 1.6  2005/11/21 19:10:50  gdavidson
// Changed control locations
//
// Revision 1.5  2005/11/21 17:18:58  romy
// added configurable view columns
//
// Revision 1.4  2005/08/09 14:33:20  romy
// Updated with Readonly Roles
//
// Revision 1.3  2005/08/05 19:37:52  romy
// rearranged the menu creation
//
// Revision 1.2  2005/08/05 18:28:18  romy
// code cleaning
//
// Revision 1.1  2005/08/05 14:20:16  romy
// Rearranged Files
//
// Revision 1.2  2005/08/05 13:50:11  romy
// Bug fixes
//
// Revision 1.1  2005/08/04 14:48:28  romy
// UMAS UI files
//
// Revision 1.1  2005/06/28 14:05:37  romy
// Added User Management List View Controls
//
// Revision 1.20  2005/06/08 19:45:49  romy
// Removed Test Code
//
// Revision 1.19  2005/05/27 17:44:25  mkontak
// Made the change password form do the change password opertaion
// instead of the calling methods.
//
// Revision 1.18  2005/05/27 12:49:48  mkontak
// no message
//
// Revision 1.17  2005/05/26 18:03:49  mkontak
// Changes for UMAS
//
// Revision 1.16  2005/05/25 13:32:21  mkontak
// Changes to UMAS
//
// Revision 1.15  2005/05/19 18:44:30  romy
// Added Header Information
//

// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console/views/UMAS/ViewUMAS.cs,v 1.28.2.4 2009/10/15 15:20:58 kchalupa Exp $
// $Id: ViewUMAS.cs,v 1.28.2.4 2009/10/15 15:20:58 kchalupa Exp $

#endregion
