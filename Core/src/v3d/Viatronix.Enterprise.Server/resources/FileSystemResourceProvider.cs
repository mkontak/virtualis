// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: FileSystemServiceProvider.cs
//
// Description: 
//
// Owner: Mark Kontak  (mkontak@viatronix.com)


using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.IO;
using System.Drawing;
using Viatronix.Enterprise.Data;
using System.Reflection;
using Viatronix.Enterprise.Configuration;
using Viatronix.Enterprise.Logging;
using Viatronix.Enterprise.Messaging;

namespace Viatronix.Enterprise.Providers
{

  /// <summary>
  /// File System Resource Provider allows interfacing to the file system 
  /// </summary>
  public class FileSystemResourceProvider : ResourceProviderBase
  {


    #region IProvider interface



    /// <summary>
    /// Processes the request
    /// </summary>
    /// <param name="request">Http request</param>
    /// <returns>Xml results or error</returns>
    public override ResponseMessage ProcessRequest(RequestMessage request)
    {
      try
      {
        string results = string.Empty;

        bool bSupported = false;

        // =======================================================================
        // Standard methods names consist of operation + resource
        //   example:    
        //            operation = get
        //            resource  = study
        //            method    = getstudy() or GetStudy()
        // ====================================================================
        string method = request.Operation + request.Resource;

        // =====================================================================================
        // Loop through all the non-public instance methods for a operational matching method
        // =====================================================================================
        foreach (MethodInfo info in this.GetType().GetMethods(BindingFlags.NonPublic | BindingFlags.Instance | BindingFlags.DeclaredOnly))
        {
          if (info.MemberType == MemberTypes.Method)
          {
            if (string.Compare(info.Name, method, true) == 0)
            {

              bSupported = true;

              // ==============================================================================
              // Check for a return code of string so we can catch the returning information
              // ==============================================================================
              if (info.ReturnType == typeof(string))
                results = (string)info.Invoke(this, new object[] { request, request.Payload });
              else
                info.Invoke(this, new object[] { request, request.Payload });

              break;
            }
          }

        }

        // If no method was found we have an unsupported resource/operation pair
        if (!bSupported)
          throw new System.NotSupportedException(string.Format("{0}-{1} is not supported", request.Operation, request.Resource));


        // Returns the results
        return ResponseMessage.Create(MessageFormat.Xml, results);

      }
      catch (TargetInvocationException e)
      {

        // ================================================================================================
        // We Catch the target of invocation exception since we are Invoking the method and any exceptions 
        // thrown in the Invoked method is wrapped in this exception.
        // =================================================================================================

        Log.Error("Failed to process request : " + e.InnerException.Message, "ResourceProviderBase", "ProcessRequest");
        throw e.InnerException;

      }
      catch (Exception e)
      {
        Log.Error("Failed to process request : " + e.Message, "ResourceProviderBase", "ProcessRequest");
        throw;
      }

    } // ProcessRequest(RequestMessage request)

    #endregion
   

    /// <summary>
    /// Gets file list
    /// </summary>
    /// <param name="request">HTTP request</param>
    /// <returns></returns>
    private string GetFileList(RequestMessage request, string xml)
    {


      string path = string.Empty;

      if (!request.TryGetValue("path", out path))
        throw new InvalidDataException("Cannot get files, no path specified");


      List<string> files = new List<string>();

      /// Get the files and set the root to the path so that it will be stripped off.
      Helper.GetFiles(path, path, files);


      StringBuilder builder = new StringBuilder();

      builder.Append(string.Format("<files  path=\"{0}\" >",path));
      foreach (string file in files)
      {
        builder.Append(string.Format("<file name=\"{0}\" />", file));
      }
      builder.Append("</files>");

      return builder.ToString();

    } // GetFileList(RequestMessage request)



    /// <summary>
    /// Gets the host(s) specified
    /// </summary>
    /// <param name="request">HTTP request</param>
    /// <returns></returns>
    private string GetPreview(RequestMessage request, string xml)
    {


      
      string path = string.Empty;
      string uid = string.Empty;
    


      request.TryGetValue("uid", out uid);
      request.TryGetValue("path", out path);

      // Set the preview filename (either the uid or preview)
      string filename = ( string.IsNullOrEmpty(uid) ? "preview.png" : uid.Replace(".", "_") + ".png" );


      if (string.IsNullOrEmpty(path))
      {
        XmlDocument doc = new XmlDocument();
        doc.LoadXml(xml);

        if ( ! doc.DocumentElement.TryGetAttributeValue<string>("path", ref path) )
          path = StorageGateway.GetSeriesPath(request.ContextId,uid,"local");

      }

     
      // If the is no path we are done
      if ( path.Length == 0 )
        throw new InvalidDataException("No path specified, cannot get the preview");


      string imageFile = System.IO.Path.Combine(path, filename);

      // ========================================================================
      // If the file does not exist then check a file with the name preview.png
      // ========================================================================
      if ( ! File.Exists(imageFile) )
          throw new FileNotFoundException("No preview file was found");


      byte[] buffer = null;

      using (System.IO.FileStream fs = new FileStream(imageFile, FileMode.Open))
      {
        buffer = new byte[fs.Length];

        fs.Read(buffer, 0, (int)fs.Length);
      }

      return Convert.ToBase64String(buffer);

    } // GetPreview(RequestMessage request, string xml)


  } // class FileSystemResourceProvider

} // namespace Viatronix.Enterprise.Providers
