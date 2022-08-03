using System;

namespace DeepCopy
{
  class Program
  {
    static void Main(string[] args)
    {
      if (args.Length != 2)
      {
        Console.WriteLine("Invaid arguement count!");
        return;
      }

      CopyDirectory(args[0], args[1]);

    }

    static void CopyDirectory(string source, string dest)
    {

      Console.WriteLine("");

      try
      {

        Console.WriteLine("Setting current directory to " + source);

        // Set the current directory to the destination
        System.IO.Directory.SetCurrentDirectory(source);

        string dirName = System.IO.Path.GetFileName(source);
        string destDir;

        if (dest.EndsWith("\\."))
        {
          
          Console.WriteLine("Copying contents of " + source + " to " + dest);
          destDir = dest.Substring(0, dest.Length - 2);
          if (destDir.EndsWith(":"))
            destDir = destDir + "\\";
        }
        else
        {

          Console.WriteLine("Copying " + source + " to " + dest);
          destDir = System.IO.Path.Combine(dest, dirName);

        }


        //==========================================================
        // Create the destination directory if it does not exist
        //=========================================================
        if (!System.IO.Directory.Exists(destDir))
        {

          try
          {
            System.IO.Directory.CreateDirectory(destDir);
          }
          catch (Exception ex)
          {
            Console.WriteLine("**ERROR .. Failed to create destination directory " + destDir + " : " + ex.Message);
            return;
          }
        }
        
        CopyFiles(source, destDir);

        string[] dirs = System.IO.Directory.GetDirectories(source);

        foreach (string dir in dirs)
        {
          CopyDirectory(dir, destDir);
        }

      }
      catch (Exception ex)
      {
        Console.WriteLine("**ERROR .. Failed to copy directory [" + source + "] to [" + dest + "]  : " + ex.Message);
        return;
      }

    }

    /// <summary>
    /// Cop all files in souce to dest
    /// </summary>
    /// <param name="source"></param>
    static void CopyFiles(string source, string dest)
    {

      Console.WriteLine(string.Empty);
      Console.WriteLine("Copying files from " + source + " to " + dest);
      Console.WriteLine(string.Empty);
      Console.Write("Getting files from " + source);

      string[] files = System.IO.Directory.GetFiles(source);

      Console.WriteLine(", found " + files.Length + " files. ");

      //==========================
      // Loop through all file
      //==========================
      foreach (string file in files)
      {

        string fileName = System.IO.Path.GetFileName(file);
        string destFile = System.IO.Path.Combine(dest, fileName);


        Console.Write("Copying " + fileName + "  .... ");

        try
        {
          System.IO.File.Copy(fileName, destFile, true);
          Console.WriteLine("Success");
        }
        catch (Exception ex)
        {
          Console.WriteLine("Failed");
          Console.WriteLine("**ERROR .. Failed to copy [" + file + "] to [" + destFile + "] :" + ex.Message);
          Console.WriteLine(ex.ToString());
        }

      }
    }
  }



}
