// $Id: Pipeline.cs,v 1.9 2006/01/27 16:47:57 romy Exp $
//
// Copyright (c) 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: dongqing chen (dongqing@viatronix.net)


using System;
using System.Threading;
using Viatronix.v3D.Utilities;
using Viatronix.v3D.Utilities.Pipelining;
using Viatronix.v3D.Logging;
using Viatronix.v3D.Visualization;
using Viatronix.v3D.UI;


namespace Viatronix.v3D.Processor.Plugins.Vascular
{
	/// <summary>
	/// Summary description for Class1.	
	/**
   *   This is the vascular preprocessing pipeline control class. It needs command line 
   *   input. The format of input is as in the following.
   *   -sourceFilePath  Source of data to be used for processing
   *   -destFilePath    Destination for results
   *   -paraSet         presetParameterOption
   *   where
   *   -paraSet : smallv, brain, cow, carotid, pa, coronary, limb
   *              mediumv, liver, renal, iliac
   *              largev, arota, aaa, pt, runoff
   *              angiorecon, 
   *              mrtof,
   *              default
   *   Lengend: Pulmonary Artery (PA);    
   *            Pulmonary Trunk (PT); 
   *            Circle of Willis (COW);   
   *            Abdomenal Aorta Aneurysm (AAA);
   *            3D Angiogram Reconstruction (angiorecon);
   *            Magnetic Resonance Imaging Time-Of-Flight (mrtof);
   */   
	/// </summary>
	class Pipeline
	{

    #region constants


    #endregion

    /// <summary>
    /// Format string for licensing error message.
    /// </summary>
    private const string m_licenseErrorMessage = "LicenseException (result: {0}) - {1}";


    #region fields


    /// <summary>
    /// LicenseClient used to request, renew and release a license. 
    /// </summary>
    private Viatronix.v3D.Licensing.LicenseClient m_licenseClient = null;

    /// <summary>
    /// Command line arguments specified for the application.
    /// </summary>
    private Viatronix.v3D.Utilities.CommandLineArguments m_args = null;

    /// <summary>
    /// Brief exit message.
    /// </summary>
    private string m_exitMessage = string.Empty;

    /// <summary>
    /// Exit code for the application.
    /// </summary>
    private int m_exitCode = 0;

    #endregion

    #region construction

    /// <summary>
    /// Creates a new instance of the Pipline object.
    /// </summary>
    public Pipeline()
    {
      m_licenseClient = new Viatronix.v3D.Licensing.LicenseClient( new TimeSpan( 0, 4, 0 ), "vaspp", "1", "1" );
      m_licenseClient.Failed += new Viatronix.v3D.Licensing.LicenseEventHandler( OnLicenseFailedHandler );
    } // Pipeline()

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets a brief description of why the application exited.
    /// </summary>
    public string ExitMessage
    {
      get { return m_exitMessage; }
      set { m_exitMessage = value; }
    } // ExitMessage


    /// <summary>
    /// Gets or sets the exit code for the application.
    /// </summary>
    public int ExitCode
    {
      get { return m_exitCode; }
      set { m_exitCode = value; }
    } // ExitCode


    /// <summary>
    /// Returns a reference to the pipelines license client.
    /// </summary>
    public Viatronix.v3D.Licensing.LicenseClient LicenseClient 
    {
      get { return m_licenseClient; }
    } // LicenseClient

    #endregion

    #region methods


    /// <summary>
    /// The main entry point for the application.
    /// </summary>
    /// <param name="Args">commend line parameters</param>
    [STAThread]
    public static int Main( string[] args )
    {
      Pipeline pipeline = null;
      try
      {
        pipeline = new Pipeline();

        if ( pipeline == null )
        {
          throw new OutOfMemoryException("Could not allocate pipeline");
        }
        
        // check the command line args
        pipeline.InitializeArgs( args );
  
        // check licensing
        pipeline.LicenseClient.Request();
        pipeline.LicenseClient.StartRenewThread();

        pipeline.RunPipeline();
      }
      catch( Viatronix.v3D.Licensing.LicenseException licEx )
      {
        pipeline.ExitMessage = String.Format( m_licenseErrorMessage, licEx.Result.ToString(), licEx.Message );
        pipeline.ExitCode = (int)(( licEx.Result == Viatronix.v3D.Licensing.LicenseResult.NoMoreFloatingLicenses ) ? ExitCodes.NoLicenseAvailable : ExitCodes.InvalidLicense );
      }
      catch( Exception e )
      {
        pipeline.ExitMessage = e.Message;
        pipeline.ExitCode = 1;
      }    
      finally
      {
        pipeline.Close();
      }

      return ( pipeline != null ) ? pipeline.ExitCode : 1;
    } // Main( args )


    /// <summary>
    /// Initalizes the CommandLineArguments object and checks there validity.
    /// </summary>
    /// <param name="args">Command line args.</param>
    private void InitializeArgs( string[] args )
    { 
      m_args = new CommandLineArguments( args );

      if( m_args.Arguments.Count == 0 )
        throw new ArgumentNullException("No arguments specified");

      if( !m_args.Arguments.Contains( "destFilePath" ))
        throw new ArgumentNullException( "Missing destination file path" );

      if( !m_args.Arguments.Contains( "sourceFilePath" ))
        throw new ArgumentNullException( "Missing source file path" );

      string logFilePath =  m_args.Arguments[ "destFilePath" ].ToString() + Viatronix.v3D.Utilities.IO.FileExtensions.Log;
      if( m_args.Arguments.Contains( "logFilePath" ))
      {
        logFilePath = m_args.Arguments[ "logFilePath"].ToString() + Viatronix.v3D.Utilities.IO.FileExtensions.Log;
      }

      int logLevel = 3;
      if( m_args.Arguments.Contains( "logLevel" ))
      {
        logLevel = System.Convert.ToInt32(m_args.Arguments["logLevel"]);
      }

      Viatronix.v3D.Logging.Logger.Initialize( logFilePath, (Viatronix.v3D.Logging.LogLevel)logLevel );

      Viatronix.v3D.Logging.Logger.Fyi("Pipeline started [" + m_args.ToString() + "]");

    } // InitializeArgs( args )


    /// <summary>
    /// Runs the vascular pipeline.
    /// </summary>
    private void RunPipeline()
    {
      //initialize the pipeline
      Viatronix.v3D.Viewer.Plugins.Vascular.Pipeline vascularPipeline = new Viatronix.v3D.Viewer.Plugins.Vascular.Pipeline( m_args );   

      if ( vascularPipeline == null )
      {
        throw new OutOfMemoryException("Failed to allocate vascular pipeline, out of memory");
      }
          
      Viatronix.v3D.Logging.Logger.Fyi( "Vascular preprocessing pipeline start" );

      // run the preprocessing pipeline
      vascularPipeline.Run();
    } // RunPipeline()


    /// <summary>
    /// Closes the vascular pipeline.
    /// </summary>
    private void Close()
    {
      if( this.ExitMessage != string.Empty )
      {
        Viatronix.v3D.Logging.Logger.Err( this.ExitMessage );
      }

      // dispose of any resource used by the licensing
      if( this.LicenseClient != null )
        this.LicenseClient.Dispose();

    } // Close()

    #endregion

    #region event handlers
    
    /// <summary>
    /// Handles any licensing failure events.
    /// </summary>
    /// <param name="sender">The license client.</param>
    /// <param name="args">LicenseEventArgs containing event information.</param>
    private void OnLicenseFailedHandler( object sender, Viatronix.v3D.Licensing.LicenseEventArgs args )
    {
      this.ExitCode = (int)(( args.Result == Viatronix.v3D.Licensing.LicenseResult.NoMoreFloatingLicenses ) ? ExitCodes.NoLicenseAvailable : ExitCodes.InvalidLicense );
      this.ExitMessage = String.Format( m_licenseErrorMessage, args.Result.ToString(), args.Message );
      this.Close();

      System.Environment.Exit( this.ExitCode );
    } // OnLicenseFailedHandler( sender, args )

    #endregion
   
	} // Class pipeline
} // namespace Viatronix.v3D.Processor.Plugins.Vascular

#region revision history

// $Log: Pipeline.cs,v $
// Revision 1.9  2006/01/27 16:47:57  romy
// added code to printout the commandline args
//
// Revision 1.8  2006/01/27 16:26:07  romy
// added code to printout the commandline args
//
// Revision 1.7  2005/11/23 21:22:30  mkontak
// Fix dump of command lines
//
// Revision 1.6  2005/10/17 12:46:06  mkontak
// Fixed to work with new arch.
//
// Revision 1.5  2005/06/08 12:23:58  vxconfig
// stopped promoting the licensing namespace
//
// Revision 1.4  2005/01/28 18:10:12  frank
// changed license renew interval to 4 minutes
//
// Revision 1.3  2005/01/12 20:29:22  gdavidson
// Added reference to Viatronix.v3D.Viewer.Core
// Removed commented code.
//
// Revision 1.2  2005/01/12 20:02:17  gdavidson
// Added concurrent licensing.
//
// Revision 1.1  2004/11/17 05:00:01  frank
// moved to new namespace
//
// Revision 1.5  2004/09/10 18:14:22  mkontak
// Using the SystemSettings in the Utilties::Registry
//
// Revision 1.4  2004/04/07 19:36:48  frank
// created auto directory to hold the automatically generated vessel session
//
// Revision 1.3  2004/03/24 21:32:55  frank
// removed hard-coded paths to installation, log, license, and config directories
//
// Revision 1.2  2004/03/02 16:46:39  geconomos
// Updated logger.
//
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.20  2003/08/19 21:15:16  dongqing
// add cleaning up for m_mutex if exception thrown
//
// Revision 1.19  2003/07/18 13:56:35  dongqing
// comment
//
// Revision 1.18  2003/07/17 14:12:12  dongqing
// comments
//
// Revision 1.17  2003/07/16 19:45:12  dongqing
// reconfigure the log info
//
// Revision 1.16  2003/07/15 19:42:29  dongqing
// comments
//
// Revision 1.15  2003/07/15 13:00:34  dongqing
// fix a bug in the mutex
//
// Revision 1.14  2003/07/11 20:36:41  dongqing
// move mutex check up front
//
// Revision 1.13  2003/07/01 12:25:25  dongqing
// remove the while loop of the mutex
//
// Revision 1.12  2003/06/30 20:36:46  dongqing
// add mutex instance check
//
// Revision 1.11  2003/06/24 14:21:20  dongqing
// comment
//
// Revision 1.10  2003/06/18 16:07:45  dongqing
// add license check
//
// Revision 1.9  2003/05/19 14:57:58  dongqing
// add return value for the main()
//
// Revision 1.8  2003/05/13 21:25:40  dongqing
// code review
//
// Revision 1.7  2003/05/13 13:48:33  dongqing
// comments
//
// Revision 1.6  2003/05/12 21:08:53  dongqing
// change the name of the namespace
//
// Revision 1.5  2003/05/09 18:18:44  dongqing
// comments out one Disposal for finally clean up
//
// Revision 1.4  2003/05/09 13:32:05  geconomos
// Changed remaining references from CTA to Vascular.
//
// Revision 1.3  2003/05/08 21:06:55  geconomos
// Fixed Michael's stuff.
//
// Revision 1.2  2003/05/08 20:12:47  geconomos
// Moved from Viatronix.v3D.Processors.Explorer.Plugins.CTA and forgot to update.
//
// Revision 1.14  2003/05/08 18:43:42  dongqing
// add messagers
//
// Revision 1.13  2003/05/07 13:20:10  mkontak
// Modified to interface with the messenger
//
// Revision 1.12  2003/05/06 20:53:10  dongqing
// code walkthrough
//
// Revision 1.11  2003/05/06 20:36:43  dongqing
// comments
//
// Revision 1.10  2003/05/06 16:09:15  dongqing
// replace return with throw( ArgumentNullException() )
//
// Revision 1.9  2003/04/04 18:23:48  dongqing
// add adaptive memory management
//
// Revision 1.8  2003/03/31 21:53:07  dongqing
// add exception handler
//
// Revision 1.7  2003/03/21 20:18:50  dongqing
// tune up parameter sets
//
// Revision 1.6  2003/03/17 21:45:08  dongqing
// add modality check
//
// Revision 1.5  2003/03/13 21:58:30  dongqing
// add batch test codes
//
// Revision 1.4  2003/03/12 21:21:12  dongqing
// add code for recognize the new constructor for adaptive preset
//
// Revision 1.3  2003/03/11 20:04:05  dongqing
// fix the string compare bug
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Vascular/Pipeline.cs,v 1.9 2006/01/27 16:47:57 romy Exp $
// $Id: Pipeline.cs,v 1.9 2006/01/27 16:47:57 romy Exp $

#endregion