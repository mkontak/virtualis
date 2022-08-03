using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.IO;
using System.Threading;
using System.Management;
using System.Security.Cryptography;
using System.Security.Principal;
using System.Security.AccessControl;

namespace Viatronix.Sql
{

  /// <summary>
  /// Class containing static helper methods
  /// </summary>
  public class Helper
  {
    
    #region fields
 


    #endregion

    #region properties

  
    #endregion


    #region methods


    #region event methods

    /// <summary>
    /// Sets the names event
    /// </summary>
    /// <param name="name"></param>
    /// <returns></returns>
    public static int SetNamedEvent(string logFilename, string name)
    {


      // Make sure the name is not null or empty
      if (string.IsNullOrEmpty(name))
        return 0;


      // Need to prepend Global\ to the name
      name = "Global\\" + name;

      // EventWaitHandle
      EventWaitHandle handle = null;

      try
      {



        LogMessage(logFilename, "SetNamedEvent", "Attempting to open " + name + " in global namespace");

        // Open existing event handle
        handle = EventWaitHandle.OpenExisting(name);

        LogMessage(logFilename, "SetNamedEvent", "Signaling " + name);

        // Set the event to allow others to process
        handle.Set();

        return 1;



      }
      catch (WaitHandleCannotBeOpenedException)
      {
        handle = null;
        LogMessage(logFilename, "SetNamedEvent", "Event handle " + name + " does not exist, no processes waiting");
      }
      catch (Exception ex)
      {
        LogMessage(logFilename, "SetNamedEvent", "Event handle failed to signal : " + ex.Message);
      }
      finally
      {
        if (handle != null)
          handle.Close();
      }

      return 0;


    } // SetNamedEvent(string name)



    /// <summary>
    /// Creates the name event
    /// </summary>
    /// <param name="name"></param>
    /// <returns></returns>
    public static int CreateNamedEvent(string logFilename, string name)
    {

      int rc = 0;

      // Make sure the name is not null or empty
      if (string.IsNullOrEmpty(name))
        return rc;

      name = "Global\\" + name;

      try
      {




        LogMessage(logFilename, "CreateNamedEvent", "Attempting to create " + name + " in global namespace");

        bool createdNew = false;

        // ======================================================================================================
        // Set up the security for the Event. Allow full rights to Administrators/Power Users and v3denterprise
        // ======================================================================================================

        // Event Security
        EventWaitHandleSecurity security = new EventWaitHandleSecurity();
        security.AddAccessRule(new EventWaitHandleAccessRule(new SecurityIdentifier(WellKnownSidType.WorldSid, null), EventWaitHandleRights.FullControl, AccessControlType.Allow));

        // Create the wait handle fpr detecting the start event
        EventWaitHandle handle = new EventWaitHandle(false, EventResetMode.ManualReset, name, out createdNew, security);


        if (createdNew)
        {
          LogMessage(logFilename, "CreateNamedEvent", string.Format("Event {0} was created", name));
          rc = 1;
        }
        else
        {
          LogMessage(logFilename, "CreateNamedEvent", string.Format("Event {0} already exists", name));
          rc = 0;
        }

      }
      catch (Exception ex)
      {
        LogMessage(logFilename, "CreateNamedEvent", "Named event " + name + " failed to create : " + ex.Message);
        throw;

      }

      return rc;


    } // CreateEvent(string name)

    /// <summary>
    /// Resets the named event
    /// </summary>
    /// <param name="name"></param>
    /// <returns></returns>
    public static int ResetNamedEvent(string logFilename, string name)
    {

      // Make sure the name is not null or empty
      if (string.IsNullOrEmpty(name))
        return 0;

      name = "Global\\" + name;

      EventWaitHandle handle = null;

      try
      {


        LogMessage(logFilename, "ResetNamedEvent", "Attempting to open " + name + " in global namespace");


        // Open existing event handle
        handle = EventWaitHandle.OpenExisting(name);

        LogMessage(logFilename, "ResetNamedEvent", "Resetting " + name);

        // Set the event to allow others to process
        handle.Reset();

        return 1;
      }
      catch (WaitHandleCannotBeOpenedException)
      {
        handle = null;
        LogMessage(logFilename, "ResetNamedEvent", "Event handle " + name + " does not exist, no processes waiting");
      }
      catch (Exception ex)
      {
        LogMessage(logFilename, "ResetNamedEvent", "Event handle failed to signal : " + ex.Message);

      }
      finally
      {
        if (handle != null)
          handle.Close();
      }

      return 0;


    } // ResetNamedEvent(string name)

    #endregion


    #region encryption/decryption

    /// <summary>
    /// Encrypts the supplied string and returned the encrypted value
    /// </summary>
    /// <param name="value">Value to be encrypted</param>
    /// <returns></returns>
    public static string Encrypt(string value)
    {
      return Viatronix.Encryption.Crypto.Encrypt(value);
    } // Encrypt(string value)

    /// <summary>
    /// Decrypts the supplied string and returned the decrypted value
    /// </summary>
    /// <param name="value">Value to be decrypted</param>
    /// <returns></returns>
    public static string Decrypt(string value)
    {
      return Viatronix.Encryption.Crypto.Decrypt(value);
    } // Decrypt(string value)

    #endregion

    /// <summary>
    /// Serializes the xml receieved. It is assumed the node's name is used for the filename and the location attribute is used to
    /// determine the directory.
    /// </summary>
    /// <param name="xml">xml</param>
    public static void LogErrorMessage(string database, string procedure, int errorCode, string errorMessage)
    {


      try
      {
        if (string.IsNullOrEmpty(database))
          database = "Database";

        string[] folders = new string[] { System.Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData), "Viatronix", "Logs" };

        string logPath = System.IO.Path.Combine(folders);

        if (!Directory.Exists(logPath))
          Directory.CreateDirectory(logPath);

        logPath = Path.Combine(logPath, database + ".log");

        using (System.IO.StreamWriter writer = new StreamWriter(logPath, true))
        {
          writer.WriteLine(string.Format("{0:dd/MM/yyyy-HH:mm:ss}  {1,-32}  {2,-8:D} {3}", DateTime.Now, procedure, errorCode.ToString(), errorMessage));
        }

      }
      catch (Exception)
      {
        // Ignore
      }
    }


    /// <summary>
    /// Deletes the specified direcrtory
    /// </summary>
    /// <param name="logFileName">Log file name</param>
    /// <param name="directory">Directory to delete</param>
    public static int DeleteDirectory(string logFileName, string directory)
    {

      int rc = 0;

      try
      {
        //========================================
        // Only need to delete if the path exists
        //=========================================
        if (Directory.Exists(directory))
        {
          rc = 1;

          //======================================
          // Delete all the directorories first
          //======================================
          foreach (string dir in Directory.GetDirectories(directory))
          {
            // Call ourself
            DeleteDirectory(logFileName, dir);
          } // END ... For each directory

          //====================
          // Delete all files
          //====================
          foreach (string file in Directory.GetFiles(directory))
          {
            LogMessage(logFileName, "DeleteDirectory", string.Format("Deleting file {0}", file));

            File.Delete(file);
          }   // END ... For each file

          LogMessage(logFileName, "DeleteDirectory", string.Format("Deleteing directory {0}", directory));

          /// Delete the path
          Directory.Delete(directory);

        } // END ... If the path exists 
      }
      catch (Exception ex)
      {
        LogMessage(logFileName, "DeleteDirectory", string.Format("Deleting directory {0} failed : {1}", directory, ex.Message));

        throw;
      }

      return rc;

    }

    /// <summary>
    /// Logs a message to the specified file
    /// </summary>
    /// <param name="path">path</param>
    /// <param name="message">Message to log</param>
    public static void LogMessage(string filename, string method, string message)
    {

      try
      {

        // ====================
        // No file name no log
        // ====================
        if (string.IsNullOrEmpty(filename))
          return;


        string[] folders = new string[] { System.Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData), "Viatronix", "Logs", "Database" };
        string logPath = System.IO.Path.Combine(folders);


        //==============================================
        // Only log if the path directory exists
        //=============================================
        if (System.IO.Directory.Exists(logPath))
        {

          string file = System.IO.Path.Combine(logPath, filename);

          using (System.IO.StreamWriter writer = new System.IO.StreamWriter(file, true))
          {
            writer.WriteLine(string.Format("{0:dd/MM/yyyy-HH:mm:ss}  {1,-32}  {2}", DateTime.Now, method, message));
          }


        } // END ... If the parent exists
      }
      catch (Exception)
      {
        // Ignore
      }
    }

    /// <summary>
    /// Gets the system id of the system the security system is running on
    /// </summary>    
    public static string GetSystemId(WindowsIdentity identity)
    {

      //// New impersonation context used so that we can access WMI
      WindowsImpersonationContext newContext = null;

      // Processor ID
      string processorId = string.Empty;

      try
      {
        //// Make sure the new identity is not NUMM
        if (identity != null)
        {
          // Create Impersontate the new identity
          newContext = identity.Impersonate();
        } // END ... New identity is not NULL

        // Get the processor ID
        processorId = GetManagementObjectValue("Win32_Processor", "ProcessorId");


      }
      catch
      {
        // Rethrow
        throw;
      }
      finally
      {
        ///////////////////////////////////////////////////////////////
        // If we have a new context then undo the impersontated user 
        ////////////////////////////////////////////////////////////////
        if (newContext != null)
        {
          newContext.Undo();
          newContext.Dispose();
        } // END ... If the impersontation was successful
      }

      // Set the system id
      return Viatronix.Encryption.Crypto.Encrypt(processorId);

    } // GetSystemId(out string systemid)

    /// <summary>
    /// Gets the management object value
    /// </summary>
    /// <param name="key">Key of group category (Win32_DiskDrive, Win32_Processor, ... )</param>
    /// <param name="propertyName">Property name we want to get the value for</param>
    /// <returns>Returnes the objects value</returns>
    private static string GetManagementObjectValue(string key, string propertyName)
    {

      // Object value
      string objectValue = string.Empty;


      // Create a search object with the select statement
      ManagementObjectSearcher searcher = new ManagementObjectSearcher("select * from " + key);

      try
      {
        /////////////////////////////////////
        // Loop through each object retreived
        /////////////////////////////////////
        foreach (ManagementObject share in searcher.Get())
        {
          ///////////////////////////////////////////////////////////////
          // If the share object contains properties we can lok for the 
          // specified property.
          //////////////////////////////////////////////////////////////
          if (share.Properties.Count > 0)
          {
            //////////////////////////////////////////
            // Check if the specified property exists
            //////////////////////////////////////////
            if (share.Properties[propertyName] != null)
            {
              // Get the object value (Assume string)
              objectValue = share.Properties[propertyName].Value.ToString();

              // Break out of loop we are done
              break;
            } // END ... If the property was found
          } // END ... If the share object contains properties
        } // END ... For each share object

      }
      catch (Exception ex)
      {
        throw ex;
      }
      finally
      {
        searcher.Dispose();
      }

      char[] spaces = { ' ' };

      // Returns the object value unless it starts with "Not" then return an empty string
      return (objectValue.StartsWith("Not") ? string.Empty : objectValue.TrimEnd(spaces));

    } // GetManagementObjectValue(string key, string propertyName)

    /// <summary>
    /// Hashes a list of values
    /// </summary>
    /// <param name="values">Values to be hased</param>
    /// <returns>Hashed value</returns>
    public static string Hash(List<string> values)
    {


      string str = string.Empty;
      foreach (string value in values)
      {
        str += value;
      }


      Viatronix.Encryption.Crypto.Encrypt(str);
 
      return Viatronix.Encryption.Crypto.Encrypt(str);

    } // Hash(string value)


    #endregion

  } // class Helpers

} // namespace Viatronix.Sql
