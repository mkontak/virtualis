using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Xml;
using System.Data;
using Viatronix.Enterprise.Entities;
using Viatronix.Enterprise.Data;

namespace Viatronix.Enterprise
{
  public static class Helper
  {

    private static byte[] m_key = null;



    public static byte[] GetKey()
    {
      if (m_key == null)
      {
        System.Reflection.Assembly current = System.Reflection.Assembly.GetExecutingAssembly();

        Stream stream = current.GetManifestResourceStream("Viatronix.Enterprise.res.webservice.data");
        m_key = new byte[stream.Length];
        stream.Read(m_key, 0, (int)stream.Length);
        
      }

      return m_key;

    }





 
 


    /// <summary>
    /// Parses the expression
    /// </summary>
    /// <param name="expression"></param>
    /// <returns></returns>
    public static string ParseExpression(string expression)
    {

      Dictionary<string, string> searchList = new Dictionary<string, string>()
                                                {  { "LogDirectory", "$(LogDirectory)" },
                                                   { "ViewersDirectory", "$(ViewersDirectory)" },
                                                   { "UtilitiesDirectory", "$(UtilitiesDirectory)" },
                                                   { "TempDirectory", "$(TempDirectory)" }, 
                                                   { "UsersDirectory", "$(UsersDirectory)" }, 
                                                   { "ServicesDirectory", "$(ServicesDirectory)" }, 
                                                   { "ProcessorDirectory", "$(ProcessorDirectory)" },
                                                   { "ConfigDirectory", "$(ConfigDirectory)" },
                                                   { "InstallDirectory", "$(InstallDirectory)" },
                                                   { "CacheBasePath", "$(CacheBasePath)" },
                                                   { "LicenseServer", "$(LicenseServer)" }
                                                };

      List<SystemSetting> settings = new List<SystemSetting>();

      EntityCollection.Load<SystemSetting>(settings, SystemGateway.Get(Viatronix.Encryption.Crypto.Decrypt("cbHmZgzHtMUP0s69dPhPei5bYZHW1k7QFyx0oEYkyE8Vqgeb4rY/lpbtIw23fbJJ"), "systemsetting",
                                                                    XmlBuilder.Create("systemsetting")));

      string parse = expression;

      foreach (KeyValuePair<string, string> pair in searchList)
      {

        SystemSetting setting = new SystemSetting();
        if ( settings.TryFind(Subsystems.System, pair.Key, ref setting))
          parse = parse.Replace(pair.Value, setting.Value);
      }

      parse = parse.Replace("$(ComputerName)", System.Environment.MachineName);
      parse = parse.Replace("$(APPDATA)", System.Environment.GetFolderPath( System.Environment.SpecialFolder.ApplicationData ) );
      parse = parse.Replace("$(PROGRAMFILESX86)", System.Environment.GetFolderPath(System.Environment.SpecialFolder.ProgramFilesX86));
      parse = parse.Replace("$(PROGRAMFILES)", System.Environment.GetFolderPath(System.Environment.SpecialFolder.ProgramFiles));

      return parse;
    }

 
    /// <summary>
    /// Recursively builds a file list 
    /// </summary>
    /// <param name="root">Root path o be stripped off if relative</param>
    /// <param name="path">Path to search files from</param>
    /// <param name="files">files list to build</param>
    public static void GetFiles(string root, string path, List<string> files)
    {

      string uroot = root.ToUpper();

      foreach (string file in System.IO.Directory.GetFiles(path))
      {
        files.Add(file.ToUpper().Replace(uroot,""));
      }

      foreach (string dir in System.IO.Directory.GetDirectories(path))
      {
        GetFiles( root, dir, files);
      }
    } //GetFiles(string path, List<string> files )

  } // class Helper

} // namespace
