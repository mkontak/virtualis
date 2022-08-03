using System;
using System.Configuration;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;






namespace Viatronix.Utilities.Dicom
{
  class Parameters : Viatronix.Dicom.CommandLineParameters
  {

    #region fields

  
    /// <summary>
    /// Pattern ( Default is *.dcm )
    /// </summary>
    private string m_pattern = "*.dcm";

    /// <summary>
    /// Files list
    /// </summary>
    private List<string> m_files = new List<string>();

    #endregion

    #region constructor

    /// <summary>
    /// Constructor
    /// </summary>
    private Parameters() { }

    #endregion

    #region properties

    /// <summary>
    /// Gets the files list
    /// </summary>
    public List<string> Files
    {
      get { return m_files; }
    } // Files

 
    #endregion

    #region methods

    /// <summary>
    /// 
    /// </summary>
    /// <param name="node"></param>
    /// <returns></returns>
    public static Parameters Create(XmlNode node)
    {
      Parameters parameters = new Parameters();

      if (parameters == null)
        throw new System.OutOfMemoryException("Failed to allocate parameters object");

      parameters.m_msProgram = "SendImage";
      parameters.m_msDescription = "Sends one or more dicom images to the specified server. All parameters listed below can be set in the in the SendImage.exe.config file. The parameters are used to override these default values if necessary.";
      parameters.Initialize(node);

      return parameters;

    }

    /// <summary>
    /// Updates the Parameters object using the arguments supplied
    /// </summary>
    /// <param name="args">Arguments</param>
    /// <returns></returns>
    public override int Update(string[] args) 
    {

      int index = base.Update(args);

      if ( m_mpParameters.ContainsKey("-f") )
        m_pattern = m_mpParameters["-f"];
 
 
      // Make sure there were files and or directories specified
      if (index >= args.Length)
        throw new System.IO.InvalidDataException("No files/directories specified");

      // =============================================================
      // Loop though the specified list of files and/or directories
      // =============================================================
      for (int i = index; i < args.Length; ++i)
      {

        if (System.IO.Directory.Exists(args[i]))
          m_files.AddRange(System.IO.Directory.GetFiles(args[i], m_pattern));
        else if (System.IO.File.Exists(args[i]))
          m_files.Add(args[i]);
        else
          throw new System.IO.InvalidDataException("Invalid file or directory " + args[i]);

      }  // END ... For each file/directory
 
      return index;

    } // Update( string [] args )

    /// <summary>
    /// Dumps the help to the console
    /// </summary>
    public override void Help()
    {
      Console.WriteLine("");
      Console.WriteLine("Program:     SendImage.exe");
      Console.WriteLine("");
      Console.WriteLine("Description: ");
      Console.WriteLine("");
      Console.WriteLine("Sends one or more dicom images to the specified server. All  parameters listed ");
      Console.WriteLine("below can be set in the in the SendImage.exe.config file. The parameters are ");
      Console.WriteLine("used to override these default values if necessary.");
      Console.WriteLine("");
      base.Help();
      Console.WriteLine("");
      Console.WriteLine("-f             File pattern to look for in the directory specified. Not ");
      Console.WriteLine("               required if a file list is specified. (ex. *.dcm, IM*, ... )");
      Console.WriteLine("");
      Console.WriteLine("<Files/Directories>    List of files and or directories to send.");
      Console.WriteLine("");
      Console.ReadLine();

    } // Help


    #endregion

  }
}

