// $Id: RemovableMediaTransferProvider.cs,v 1.6.2.1 2007/03/20 14:11:53 romy Exp $ 
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Romy Sreedharan ( mailto:romy@viatronix.com )

//using System;
//using Viatronix.v3D.Core;
//using System.Collections.Generic;
//using System.IO;
//using Viatronix.UI;
//using System.Collections;
//using System.Threading;
//using System.Xml;
//using Viatronix.Utilities.RemovableMedia;



//namespace Viatronix.Console
//{
//  /// <summary>
//  /// Removable Media Transfer Provider is an abstract class.
//  /// The derived classes should implement AddFiles to use transfer. 
//  /// </summary>
//  public abstract class RemovableMediaTransferProvider :  ITransferProvider
//  {
   
//    #region fields

//    /// <summary>
//    /// Load Progress Handler
//    /// </summary>
//    protected ProgressEventHandler m_progressHandler = null;

//    /// <summary>
//    /// Load event Handler
//    /// </summary>
//    protected LoadEventHandler m_loadDeviceHandler = null;

//    /// <summary>
//    /// Erasing event Handler
//    /// </summary>
//    protected EraseEventHandler m_erasingHandler = null;

//    /// <summary>
//    /// Erased event Handler
//    /// </summary>
//    protected EventHandler m_erasedHandler = null;

//    /// <summary>
//    /// Device object
//    /// </summary>
//    private Device m_device = null;

//    /// <summary>
//    /// Transfer progress event handler
//    /// </summary>
//    public event TransferProgressEventHandler Progress = null;

//    /// <summary>
//    /// ShowMessage handler
//    /// </summary>
//    private SetterDelegate< string > m_requiredDiskMessageHandler = null; 

//    /// <summary>
//    /// Cancel Warn message handler 
//    /// </summary>
//    private GetterDelegate< bool > m_cancelWarningMessageHandler = null;

//    /// <summary>
//    /// Flag indicates the state of Write Method.
//    /// </summary>
//    protected bool m_writing = false;

//    /// <summary>
//    /// Transfer Provider Args.
//    /// </summary>
//    protected TransferProviderArgs m_transferProviderArgs = null;
    
//    #endregion

//    #region construction

//    /// <summary>
//    /// Constructor
//    /// </summary>
//    public RemovableMediaTransferProvider()
//    {
//      m_progressHandler   = new ProgressEventHandler( this.OnProgressEventHandler );
//      m_loadDeviceHandler = new LoadEventHandler( this.OnLoadDeviceEventHandler );
//      m_erasingHandler    = new EraseEventHandler( this.OnErasingEventHandler );
//      m_erasedHandler     = new EventHandler( this.OnErasedEventHandler );

//      m_requiredDiskMessageHandler  = new SetterDelegate< string >( ShowDisksRequiredMessageBox );
//      m_cancelWarningMessageHandler = new GetterDelegate< bool >( ShowCancelWarningMessageBox );
//    }//RemovableMediaTransferProvider()


//    #endregion

//    #region abstract Methods

//    /// <summary>
//    /// Methods which needs an implementation.
//    /// </summary>
//    public abstract void AddFiles();

//    #endregion

//    #region properties

//    /// <summary>
//    /// Gets/Sets the Selected Deivce
//    /// </summary>
//    protected Device SelectedDevice
//    {
//      get
//      {
//        if ( m_device == null )
//        {
//          //Get the devices
//          List< Device > devices = DeviceFactory.GetDevices();
          
//          if( devices.Count == 0 )
//            throw new TransferException( "Failed to get device from the system." );
          
//          //Set the first device as selected. ??
//          m_device = devices[0];
//        }
//        return m_device; 
//      }//get
      
//      set { m_device = value; }

//    }//SelectedDevice

  

//    #endregion

//    #region ITransferProvider Members


//    /// <summary>
//    /// Implements the Interface method. 
//    /// </summary>
//    /// <param name="args"></param>
//    public virtual void Transfer( TransferProviderArgs args )
//    {
//      int totalFiles =  args.TotalSeriesFilesCount();
//      m_transferProviderArgs = args;

//      OnProgress( new TransferProgressEventArgs( "Transferring files ...", 0, totalFiles  ));

//      //Add all files to the Media.
//      AddFiles();

//      //Burn the added files.
//      if ( SelectedDevice.Writer.Files.Count  > 0 )
//        BurnMedia();

//    }// Transfer( TransferProviderArgs args )

  
//    /// <summary>
//    /// Abort method.
//    /// </summary>
//    public void Abort()
//    {
//      //If in the writing mode try to cancel that.
//      if ( SelectedDevice != null && m_writing )
//      {
//        bool result = (bool) m_transferProviderArgs.Frame.Invoke( m_cancelWarningMessageHandler, null );

//        if ( result )
//        {
//          this.SelectedDevice.Writer.Cancel();
//          throw new TransferAbortException( " User aborted transfer operation " );
//        }
//        else
//          throw new TransferAbortCancelException ();
//      }
//      else
//        throw new TransferAbortException( " User aborted transfer operation " );
//    }//Abort()


//    #endregion

//    #region methods

//    /// <summary>
//    /// Burns the media after adding all files.
//    /// </summary>
//    protected void BurnMedia ()
//    {
//      try
//      {
//        this.SelectedDevice.Writer.Progress += m_progressHandler;
//        this.SelectedDevice.LoadMedia       += m_loadDeviceHandler;
//        this.SelectedDevice.Writer.Erasing  += m_erasingHandler;
//        this.SelectedDevice.Writer.Erased   += m_erasedHandler;

//        EstimateRequiredDisks ();

//        m_writing = true;   

//        this.SelectedDevice.Writer.Write();
//        m_writing = false;

//      }
//      catch ( Viatronix.Utilities.RemovableMedia.AbortException ex )
//      {
//        Viatronix.Logging.Log.Error(ex.Message, "RemovableMediaTransferProvider", "BurnMedia");
//      }
//      catch ( TransferAbortException ex )
//      {
//        throw ex;
//      }
//      catch( Exception ex )
//      {
//        Viatronix.Logging.Log.Error("An exception occured during transfer " + ex.Message, "RemovableMediaTransferProvider", "BurnMedia");
//        throw new Exception (  "An exception occured during transfer. " + ex.Message ) ;
//      }
//      finally
//      {
//        this.SelectedDevice.LoadMedia -= m_loadDeviceHandler;
//        this.SelectedDevice.Writer.Progress -= m_progressHandler;
//        this.SelectedDevice.Writer.Erasing -= m_erasingHandler;
//        this.SelectedDevice.Writer.Erased -= m_erasedHandler;
//      }

//    }//BurnMedia ()

    
//    /// <summary>
//    /// Calculate the number of discs required.And displays a message box if it needs more than one disk.
//    /// </summary>
//    protected void EstimateRequiredDisks()
//    {
//      List< MediaInfo > disks = null;
//      try
//      {
//        disks  = SelectedDevice.Writer.MediaManager.OrganizeFiles();
//      }
//      catch ( Exception ex )
//      {
//        Viatronix.Logging.Log.Error("Exception caught in OrganizeFiles " + ex.Message, "RemovableMediaTransferProvider", "EstimateRequiredDisks");
//        throw ex;
//      }

//      if( disks.Count > 1 )
//      {
//        Viatronix.Logging.Log.Debug(" Needs " + disks.Count.ToString() + "disks", "RemovableMediaTransferProvider", "EstimateRequiredDisks");
//        ShowDisksRequiredMessageBox ( disks.Count.ToString() );
//      }
//    }//CalculateRequiredDiscs()


//    /// <summary>
//    /// Display the number of Disks reauired message box.
//    /// </summary>
//    /// <param name="number"></param>
//    private void ShowDisksRequiredMessageBox ( string disks )
//    {
//      // on a seperate thread invoke on UI thread
//      if( m_transferProviderArgs.Frame.InvokeRequired )
//        m_transferProviderArgs.Frame.Invoke( m_requiredDiskMessageHandler, new object[] { disks });
//      else
//      {
//        // on UI thread display dialog
//        if ( !Viatronix.UI.MessageBox.ShowQuestion( Global.Frame ," Transfer requires " + disks + " disks. Do you wish to continue ? " ) )
//          throw new TransferAbortException( "User aborted Transfer " );
//      }
//    }//DisksRequiredMessageBox ( string disks )


//    /// Display the number of Disks reauired message box.
//    /// </summary>
//    /// <param name="number"></param>
//    private bool ShowCancelWarningMessageBox ( )
//    {
//      // on UI thread display dialog
//      Viatronix.UI.MessageBox.ShowInfo( Global.Frame ," Canceling may make the disk unusable." );
//      return true;
//    }//DisksRequiredMessageBox ( string disks )

     
//    /// <summary>
//    /// Handles the Progress event.
//    /// </summary>
//    /// <param name="args"></param>
//    protected virtual void OnProgress( TransferProgressEventArgs args )
//    {
//      if( this.Progress != null )     
//        this.Progress( this, args );
//    }//OnProgress( TransferProgressEventArgs args )


//    #endregion

//    #region event handlers

//    /// <summary>
//    /// Handles the Progress Event
//    /// </summary>
//    /// <param name="sender">object</param>
//    /// <param name="args">ProgressEventArgs</param>
//    private void OnProgressEventHandler( object sender, ProgressEventArgs args )
//    {
//      OnProgress( new TransferProgressEventArgs( args.Stage.Message, args.Stage.Progress, args.Stage.Total ));
//    }//OnProgressEventHandler( object sender, ProgressEventArgs args )


//    /// <summary>
//    /// Handles the Device Load event
//    /// </summary>
//    /// <param name="sender">object</param>
//    /// <param name="args">LoadEventArgs</param>
//    private void OnLoadDeviceEventHandler( object sender, LoadEventArgs args )
//    {
//      // on a seperate thread invoke on UI thread
//      if( m_transferProviderArgs.Frame.InvokeRequired )
//        m_transferProviderArgs.Frame.Invoke( m_loadDeviceHandler, new object[] { sender, args });
//      else
//      {
//        // on UI thread display dialog
//        System.Windows.Forms.DialogResult result = MessageBox.ShowQuestion( Global.Frame ,args.Message, System.Windows.Forms.MessageBoxButtons.OKCancel );
//        args.Abort = ( result == System.Windows.Forms.DialogResult.Cancel );
//      }
//    }//OnLoadDeviceEventHandler( object sender, LoadEventArgs args )

 
//    /// <summary>
//    /// Handles the Erase event
//    /// </summary>
//    /// <param name="sender">object</param>
//    /// <param name="args">EraseEventArgs</param>
//    private void OnErasingEventHandler( object sender, EraseEventArgs args )
//    {
//      // on a seperate thread invoke on UI thread
//      if( m_transferProviderArgs.Frame.InvokeRequired )
//        m_transferProviderArgs.Frame.Invoke( m_erasingHandler, new object[] { sender, args });
//      else
//      {
//        // on UI thread display dialog
//        System.Windows.Forms.DialogResult result = MessageBox.ShowQuestion( Global.Frame ,"Do you want to erase the disk?", System.Windows.Forms.MessageBoxButtons.YesNoCancel );
//        if( result == System.Windows.Forms.DialogResult.Yes )
//        { 
//          args.Erase = true;
//          OnProgress( new TransferProgressEventArgs( "Erasing Media...", 0, 100 ));
//        }
//        else if ( result == System.Windows.Forms.DialogResult.Cancel )
//        {
//          try
//          {
//            //args.Erase = false;
//            args.Abort = true;
//           // this.SelectedDevice.Writer.Cancel();
//          }
//          catch ( Viatronix.Utilities.RemovableMedia.AbortException ex )
//          {
//            Viatronix.Logging.Log.Debug("User aborted the CD burning operation" + ex.Message, "RemovableMediaTransferProvider", "OnErasingEventHandler");
//            throw new TransferAbortException( " User aborted transfer operation " );
//          }
//        }
//        else
//        {
//          args.Erase = false;
//          //args.Abort = ( result == System.Windows.Forms.DialogResult.No );
//        }
//      }
//    }//OnErasingEventHandler( object sender, EraseEventArgs args )


//    /// <summary>
//    /// Handles the Erased event handler
//    /// </summary>
//    /// <param name="sender">object</param>
//    /// <param name="args">EventArgs</param>
//    private void OnErasedEventHandler( object sender, EventArgs args )
//    {
//      OnProgress( new TransferProgressEventArgs( "Media Erased.", 0, 100 ));
//    }//OnErasedEventHandler( object sender, EventArgs args )


//    #endregion

//  }//class RemovableMediaTransferProvider :  ITransferProvider
//}//Viatronix.Console


#region revision History

// $Log: RemovableMediaTransferProvider.cs,v $
// Revision 1.6.2.1  2007/03/20 14:11:53  romy
// issue #5500
//
// Revision 1.6  2007/03/14 19:26:04  romy
// Spell check for message - IssuE#5476
//
// Revision 1.5  2007/03/13 20:26:11  romy
// Fixed Issue#5476 Clicking 'No' to the cd writing cancel dialog, causes Console to crash after the cd is completed.
//
// Revision 1.4  2006/07/19 13:54:22  gdavidson
// Fixed the EstimateRequiredDisks method
//
// Revision 1.3  2006/06/05 18:35:27  gdavidson
// ported to vs2005
//
// Revision 1.2  2006/02/16 13:00:08  mkontak
// Transfer from PACS to V3D
//
// Revision 1.1  2005/12/13 20:45:59  romy
// re arranged
//
// Revision 1.20  2005/11/23 11:53:10  mkontak
// Using new MessageBox implementation
//
// Revision 1.19  2005/11/03 19:52:51  romy
// Redsigned this class. Made it as an abstract base class.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console/Transfer/RemovableMediaTransferProvider.cs,v 1.6.2.1 2007/03/20 14:11:53 romy Exp $
// $Id: RemovableMediaTransferProvider.cs,v 1.6.2.1 2007/03/20 14:11:53 romy Exp $

#endregion revision History