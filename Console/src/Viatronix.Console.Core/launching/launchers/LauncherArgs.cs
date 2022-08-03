// $Id: LauncherArgs.cs,v 1.4 2007/03/13 12:18:09 mkontak Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using Viatronix.v3D.Core;
using Viatronix.Enterprise.Entities;
using Viatronix.Enterprise.SDK.Providers;

namespace Viatronix.Console
{

  /// <summary>
  /// Event data for launch operations.
  /// </summary>
  public class LauncherArgs : EventArgs
  {

    #region fields

    /// <summary>
    /// the parent winform
    /// </summary>
    private System.Windows.Forms.Form m_form = null;

    /// <summary>
    /// the load dataprovider
    /// </summary>
    private IDataProvider m_provider = null;

    /// <summary>
    /// are these args readonly
    /// </summary>
    private bool m_readonly = false;

    /// <summary>
    /// the current user
    /// </summary>
    private User m_user = new User();

    /// <summary>
    /// site information
    /// </summary>
    private Site m_site = new Site();

    /// <summary>
    /// the isle interval
    /// </summary>
    private int m_idleInterval = 0;

    /// <summary>
    /// can the viewer navigate back to the console
    /// </summary>
    private bool m_canShowConsole = true;

    /// <summary>
    /// empty LaunchArgs object.
    /// </summary>
    public new static readonly LauncherArgs Empty = new LauncherArgs( true );

    #endregion

    #region construction

    /// <summary>
    /// default constructor
    /// </summary>
    public LauncherArgs()
    {
    } // LauncherArgs()


    /// <summary>
    /// private constructor for the Empty field.
    /// </summary>
    /// <param name="readOnly">determines if the current laucher args are readonly</param>
    private LauncherArgs( bool readOnly )
    {
      m_readonly = readOnly;
    } // LauncherArgs( readOnly )


    /// <summary>
    /// Creates a new instance of a LaucherArgs.
    /// </summary>
    /// <param name="form">parent form</param>
    /// <param name="user">the current user</param>
    /// <param name="site">the site information</param>
    /// <param name="interval">the idle interval</param>
    public LauncherArgs( Form form, User user, Site site, int interval ) : this( form, null, user, site, interval, true )
    {
    } // LauncherArgs( Form form, User user, Site site, int interval )


    /// <summary>
    /// Creates a new instance of a LaucherArgs.
    /// </summary>
    /// <param name="form">parent form</param>
    /// <param name="provider">the data provider</param>
    /// <param name="user">the current user</param>
    /// <param name="site">the site information</param>
    /// <param name="interval">the idle interval</param>
    public LauncherArgs( Form form, IDataProvider provider, User user, Site site, int interval ) : this( form, provider, user, site, interval, true )
    {
    } // LauncherArgs( Form form, IDataProvider provider, User user, Site site, int interval )


    /// <summary>
    /// Creates a new instance of a LaucherArgs.
    /// </summary>
    /// <param name="form">parent form</param>
    /// <param name="user">the current user</param>
    /// <param name="site">the site information</param>
    /// <param name="interval">the idle interval</param>
    /// <param name="canShowConsole">can the viewer navigate back to the console</param>
    public LauncherArgs( Form form, User user, Site site, int interval, bool canShowConsole ) : this( form, null, user, site, interval, canShowConsole )
    {
    } // LauncherArgs( Form form, User user, Site site, int interval, bool canShowConsole )


    /// <summary>
    /// Creates a new instance of a LaucherArgs.
    /// </summary>
    /// <param name="form">parent form</param>
    /// <param name="provider">the data provider</param>
    /// <param name="user">the current user</param>
    /// <param name="site">the site information</param>
    /// <param name="interval">the idle interval</param>
    /// <param name="canShowConsole">can the viewer navigate back to the console</param>
    public LauncherArgs( Form form, IDataProvider provider, User user, Site site, int interval, bool canShowConsole )
    {
      m_user = user;
      m_site = site;
      m_idleInterval = interval;
      m_canShowConsole = canShowConsole;
      m_form = form;
      m_provider = provider;
    } // LauncherArgs( form, provider, user, site, interval, canShowConsole )

    #endregion

    #region properties

    /// <summary>
    /// Get or sets the parent form.
    /// </summary>
    public System.Windows.Forms.Form Form
    {
      get { return m_form; }
      set 
      { 
        if( m_readonly )
          throw new ArgumentException( "This object is readonly." );

        m_form = value; 
      }
    } // Form


    /// <summary>
    /// Gets or sets the current data provider.
    /// </summary>
    public IDataProvider Provider
    {
      get { return m_provider; }
      set 
      { 
        if( m_readonly )
          throw new ArgumentException( "This object is readonly." );

        m_provider = value;
      }
    } // DataProvider
    

    /// <summary>
    /// Gets or sets the current user.
    /// </summary>
    public User User
    {
      get { return m_user; }
      set 
      { 
        if( m_readonly )
          throw new ArgumentException( "This object is readonly." );

        m_user = value; 
      }
    } // User


    /// <summary>
    /// Gets or sets the site information.
    /// </summary>
    public Site Site
    {
      get { return m_site; }
      set 
      { 
        if( m_readonly )
          throw new ArgumentException( "This object is readonly." );

        m_site = value; 
      }
    } // Site


    /// <summary>
    /// Gets or sets idle interval.
    /// </summary>
    public int IdleInterval
    {
      get { return m_idleInterval; }
      set 
      { 
        if( m_readonly )
          throw new ArgumentException( "This object is readonly." );

        m_idleInterval = value; 
      }
    } // IdleInterval


    /// <summary>
    /// Gets or sets if the viewer can navigate back to the console.
    /// </summary>
    public bool CanShowConsole
    {
      get { return m_canShowConsole; }
      set 
      { 
        if( m_readonly )
          throw new ArgumentException( "This object is readonly." );

        m_canShowConsole = value; 
      }
    } // CanShowConsole

    #endregion

  } // namespace Viatronix.Console
} // class LauncherArgs


#region revision history

// $Log: LauncherArgs.cs,v $
// Revision 1.4  2007/03/13 12:18:09  mkontak
// coding standards
//
// Revision 1.3  2007/03/06 19:14:59  gdavidson
// Added class comments
//
// Revision 1.2  2007/03/02 22:09:09  gdavidson
// Modified log messages
//
// Revision 1.1  2006/10/30 15:41:34  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console.Core/launching/launchers/LauncherArgs.cs,v 1.4 2007/03/13 12:18:09 mkontak Exp $
// $Id: LauncherArgs.cs,v 1.4 2007/03/13 12:18:09 mkontak Exp $

#endregion