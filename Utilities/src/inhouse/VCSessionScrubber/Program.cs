using System;
using System.IO;
using System.Xml;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Viatronix.Utilities.VCSessionScrubber
{
  /// <summary>
  /// Contains the main entry point into the program.
  /// </summary>
  public class Program
  {

    #region methods

    /// <summary>
    /// The main entry point into the program.
    /// </summary>
    /// <param name="args">Arguments to take into account.</param>
    static void Main(string[] args)
    {
      Console.WriteLine("Viatronix VC Session Scrubber Utility.");
      Console.WriteLine("--------------------------------------");
      Console.WriteLine();

      if (args.Length > 0)
      {
        if (!Directory.Exists(args[0]))
        {
          Console.WriteLine(string.Format("The specified directory does not exist. [DIRECTORY={0}]", args[0]));
          return;
        }

        string[] sessionFiles = Directory.GetFiles(args[0], "*.vcsx", SearchOption.AllDirectories);
        if (sessionFiles.Length > 0)
        {
          foreach (String sessionFile in sessionFiles)
          {
            Console.WriteLine(string.Format("Scrubbing Session File: {0}", sessionFile));

            XmlDocument doc = new XmlDocument();
            doc.Load(sessionFile);

            XmlNode bookmarks = doc.DocumentElement.SelectSingleNode("BookmarkList");
            XmlNode annotations = doc.DocumentElement.SelectSingleNode("AnnotationList");
            XmlNode srtFindings = doc.DocumentElement.SelectSingleNode("SRTFindings");
            XmlNode measureLineList = doc.DocumentElement.SelectSingleNode("MeasureLineList");

            if (bookmarks != null)
            {
              for (int i = bookmarks.ChildNodes.Count - 1; i > -1; --i)
              {
                bookmarks.RemoveChild(bookmarks.ChildNodes[i]);
              }
            }

            if (annotations != null)
            {
              for (int i = annotations.ChildNodes.Count - 1; i > -1; --i)
              {
                annotations.RemoveChild(annotations.ChildNodes[i]);
              }
            }

            // Need to remove SRT Findings, additionally because the VC Viewer will crash without their removal.
            if (srtFindings != null)
            {
              for (int i = srtFindings.ChildNodes.Count - 1; i > -1; --i)
              {
                srtFindings.RemoveChild(srtFindings.ChildNodes[i]);
              }
            }

            if (measureLineList != null)
            {
              for (int i = measureLineList.ChildNodes.Count - 1; i > -1; --i)
              {
                measureLineList.RemoveChild(measureLineList.ChildNodes[i]);
              }
            }

            doc.Save(sessionFile);
          }
        }
        else
        {
          Console.WriteLine("No files were found to scrub.");
        }
      }
      else
      {
        Console.WriteLine("No input directory was specified.");
      }
    } // void Main( args )

    #endregion

  } // class Program
} // namespace Viatronix.Utilities.VCSessionScrubber
