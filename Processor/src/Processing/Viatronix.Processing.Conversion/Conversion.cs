using System;
using System.Threading;
using Viatronix.Dicom;
using Viatronix.Logging;

namespace Viatronix.Processing
{

	/// <summary>
	/// Conversion
	/// </summary>
	class Conversion
	{

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static int Main(string[] args)
		{
      
      if ( args.Length == 0 )
      {
        Usage("");
        return 1;
      }

      Convert convert = null;

      try
      {
        convert = new Convert(args);
      }
      catch ( Exception ex )
      {
        string sErrorMessage = "Failed to create conversion object : " + ex.Message;
        Log.Error(sErrorMessage, "Main", "Main");
        Console.WriteLine(sErrorMessage);
        return 1; 
      }                            
      
      try
      {
        convert.Run();


        int errroCode = convert.ErrorCode;

        Log.Debug(string.Format("Returning exit code {0}", errroCode), "Conversion", "Main");

        return errroCode;

      }
      catch ( System.UnauthorizedAccessException ex )
      {
        string sErrorMessage = "Failed to create volume : " + ex.Message;
        Log.Error(sErrorMessage, "Main", "Main");
        Console.WriteLine(sErrorMessage);
        return 3; 
      }
      catch ( Exception ex )
      {
        string sErrorMessage = "Failed to create volume : " + ex.Message;
        Log.Error(sErrorMessage, "Main", "Main");
        Console.WriteLine(sErrorMessage);
        return 2;
      }
                                      
		} // Main()


    static void Usage(string message)
    {

      Console.WriteLine(message);
      Console.WriteLine("");
      Console.WriteLine("Conversion [ARGUEMENTS]");
      Console.WriteLine("");
      Console.WriteLine("Generates V3D volume files using the supplied command line arguments. The description of valid");
      Console.WriteLine("command line arguments appears below.");
      Console.WriteLine("");

      Console.WriteLine("[-messageHost HOST]          Specifies the host name (or IP address) of the status message receiver");
      Console.WriteLine("");

      Console.WriteLine("[-messagePort PORT]          Specifies the port the message receiver is listening on");
      Console.WriteLine("");

      Console.WriteLine("[-sourceFilePath PATH]       Source file path is the source directory with the root file added");
      Console.WriteLine("");

      Console.WriteLine("[-destFilePath PATH]         Destination file path including root file name of the volume file to be");
      Console.WriteLine("                             written.");
      Console.WriteLine("");

      Console.WriteLine("[-logFilePath PATH]          Destination of the log file to be generated.");
      Console.WriteLine("");

      Console.WriteLine("[-logLevel <0-5>]            Logging level (Default is 5).");
      Console.WriteLine("");

      Console.WriteLine("[-logType <type>]            Logger type (Default is 0).");
      Console.WriteLine("");

      Console.WriteLine("[-ignoreProjectionImages <0|1>]   If 0 then projection images are included in volume, 1 will exclude these images.");
      Console.WriteLine("                             Projection images are images from an MR that contain 'PROJECTION IMAGE' in the");
      Console.WriteLine("                             image type dicom field (0008, 0008).");
      Console.WriteLine("");

      Console.WriteLine("[-interpolate VALUE]         Value to interpolate datasets down to if there spacing is greater.");
      Console.WriteLine("");

      Console.WriteLine("[-retrospective <0|1>]       If 1 then retrospective gating is checked, 0 will not check for gating.");
      Console.WriteLine("                             (Default is 0 for not checking).");
      Console.WriteLine("");

      Console.WriteLine("[-delta VALUE]               The time delta to be used to determine retrospective gating (default is 0.001).");
      Console.WriteLine("");

      Console.WriteLine("[-maxDetectore VALUE]        The maximum detectors allowed when determining retrospective gating (default is 32).");
      Console.WriteLine("");

      Console.WriteLine("[-defaultPixelPadding VALUE] The default pixel padding value to be used if one is not supplied in the ");
      Console.WriteLine("                             dicom header (default is 0).");
      Console.WriteLine("");

      Console.WriteLine("[-volumeType <b|l>]          If 'b' then a block volume will be generated, 'l' will generate a linear.");
      Console.WriteLine("                             If the studyType is set to 1 then this will be ignored and a linear volume");
      Console.WriteLine("                             will be generated (deafult is 'b').");
      Console.WriteLine("");

      Console.WriteLine("[-presort <-1 - 2>]          Type of sorting to be done on the images: ");
      Console.WriteLine("                             -1 - No sorting will be done.");
      Console.WriteLine("                              0 - Images are sorted by their Z position (Default).");
      Console.WriteLine("                              1 - Images are sorted by their image number (0020,0013).");
      Console.WriteLine("                              2 - Images are sorted by their acquisition time (0008, 0032).");
      Console.WriteLine("");

      Console.WriteLine("[-timeVolumeSort <-1 - 2>]   Type of sorting to be done on the time volume images (Retrospective only): ");
      Console.WriteLine("                             -1 - No sorting will be done.");
      Console.WriteLine("                              0 - Images are sorted by their Z position (Default).");
      Console.WriteLine("                              1 - Images are sorted by their image number (0020,0013).");
      Console.WriteLine("                              2 - Images are sorted by their acquisition time (0008, 0032).");
      Console.WriteLine("");

      Console.WriteLine("[-ignoreAcquisitionNo <0|1>] If 0 then the acquisition number is included as part of the dataset key, 1");
      Console.WriteLine("                             ignore the acqusition number and use only the series UID. On some customer");
      Console.WriteLine("                             it may be necessary to set this to true of the acquisition number is used");
      Console.WriteLine("                             incorrectly (Imagetron).");

      Console.WriteLine("[-downSampleLargeDatasets <0|1>] If 0 then large datasets (> 512)are not down sampled, but are processed");
      Console.WriteLine("                                 as is. If 1 then every other image is not loaded into the volume file");

      Console.WriteLine("");
      Console.WriteLine("");

    } // Usage

	} // Conversion class

} // namespace
