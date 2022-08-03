// Copyright © 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Kyle R Chalupa(mailto:kchalupa@viatronix.com)

using System;
using System.IO;
using System.Data;
using System.Data.Common;
using System.Data.SqlTypes;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Microsoft.Practices.EnterpriseLibrary.Data;
using Microsoft.Practices.EnterpriseLibrary.Common;

namespace Viatronix.Utilities.InHouse.DatabaseErrorCodeGenerator
{
  /// <summary>
  /// Contains the entry point of the program.
  /// </summary>
  internal class Program
  {

    #region methods

    /// <summary>
    /// The entry point of the program.
    /// </summary>
    /// <param name="args">Arguments provided to the program.</param>
    internal static void Main(string[] args)
    {
      Dictionary<string, List<DatabaseError>> errors = ReadDatabase();
      Dictionary<DatabaseError, List<DatabaseError>> synonymMap = null;
      List<DatabaseError> duplicates = null;

      FindDuplications(errors, out synonymMap, out duplicates);
      WriteReadme(errors);
      WriteErrorCodes(errors, synonymMap, duplicates);
      WriteExceptions(errors, synonymMap, duplicates);
      WriteExceptionFactory(errors, synonymMap, duplicates);
    } // Main( args )


    /// <summary>
    /// Reads the database and builds up the list of errors.
    /// </summary>
    /// <returns>A list of database errors.</returns>
    private static Dictionary<string, List<DatabaseError>> ReadDatabase()
    {
      Dictionary<string, List<DatabaseError>> typeErrorsMap = new Dictionary<string,List<DatabaseError>>();
      Database system = DatabaseFactory.CreateDatabase("system");
      using (DbConnection conn = system.CreateConnection())
      {
        DbCommand command = system.GetSqlStringCommand("SELECT * FROM [System].[v3d].[ErrorCodes]");
        DataSet ds = system.ExecuteDataSet(command);

        if (ds.Tables.Count > 0)
        {
          foreach (DataRow row in ds.Tables[0].Rows)
          {
            DatabaseError error = new DatabaseError();
            
            error.Code = row[0].ToString();
            error.Type = row[1].ToString();
            error.Name = row[2].ToString();
            error.Description = row[3].ToString();

            List<DatabaseError> errors = null;
            if(!typeErrorsMap.TryGetValue(error.Type, out errors))
            {
              errors = new List<DatabaseError>();
              typeErrorsMap.Add(error.Type, errors);
            }

            errors.Add(error);
          }
        }
      }

      return typeErrorsMap;
    } // ReadDatabase()


    /// <summary>
    /// Generates test data to check the duplication handling.
    /// </summary>
    /// <returns>Sample data to provide a spot check that the duplications work correctly.</returns>
    private static Dictionary<string, List<DatabaseError>> GenerateTestData()
    {
      Dictionary<string, List<DatabaseError>> errors = new Dictionary<string, List<DatabaseError>>();

      List<DatabaseError> processingErrors = new List<DatabaseError>();
      List<DatabaseError> storageErrors = new List<DatabaseError>();
      List<DatabaseError> securityErrors = new List<DatabaseError>();
      List<DatabaseError> systemErrors = new List<DatabaseError>();

      // Create some processing errors.
      processingErrors.Add(new DatabaseError("Error1", "VX0001", "processing", "This is error one"));
      processingErrors.Add(new DatabaseError("Error2", "VX0002", "processing", "This is error two"));
      processingErrors.Add(new DatabaseError("Error3", "VX0003", "processing", "This is error three"));
      processingErrors.Add(new DatabaseError("Error4", "VX0004", "processing", "This is error four"));
      processingErrors.Add(new DatabaseError("Error5", "VX0005", "processing", "This is error five"));
      processingErrors.Add(new DatabaseError("Error1", "VX1001", "processing", "This is error six"));

      storageErrors.Add(new DatabaseError("Error7", "VX0007", "storage", "This is error seven"));
      storageErrors.Add(new DatabaseError("Error8", "VX0008", "storage", "This is error eight"));
      storageErrors.Add(new DatabaseError("Error9", "VX0009", "storage", "This is error nine"));
      storageErrors.Add(new DatabaseError("Error4", "VX0332", "storage", "This is error ten"));
      storageErrors.Add(new DatabaseError("Error9", "VX2000", "storage", "This is error eleven"));

      errors.Add("Processing", processingErrors);
      errors.Add("Storage", storageErrors);
      //errors.Add("Security", securityErrors);
      //errors.Add("System", systemErrors);

      return errors;
    } // GenerateTestData()


    /// <summary>
    /// Finds the duplications 
    /// </summary>
    /// <param name="typeErrorsMap">A map of types to the errors that have the type.</param>
    /// <param name="synonymMap">A map from the error to a list of similar errors.</param>
    /// <param name="duplicates">List of duplicates, to exclude additional items in the output.</param>
    private static void FindDuplications(Dictionary<string, List<DatabaseError>> typeErrorsMap, out Dictionary<DatabaseError, List<DatabaseError>> synonymMap, out List<DatabaseError> duplicates)
    {
      synonymMap = new Dictionary<DatabaseError, List<DatabaseError>>();
      duplicates = new List<DatabaseError>();

      List<DatabaseError> errors = new List<DatabaseError>();

      foreach (KeyValuePair<string, List<DatabaseError>> keyPair in typeErrorsMap)
      {
        errors.AddRange(keyPair.Value);
      }

      for(int i = 0; i < errors.Count; ++i)
      {
        for (int j = errors.Count - 1; j > -1; --j)
        {
          if (errors[i] != errors[j] && errors[i].Name == errors[j].Name)
          {
            List<DatabaseError> duplicateErrors = null;
            if (!synonymMap.TryGetValue(errors[i], out duplicateErrors))
            {
              duplicateErrors = new List<DatabaseError>();
              synonymMap.Add(errors[i], duplicateErrors);
            }

            duplicateErrors.Add(errors[j]);
            duplicates.Add(errors[j]);
            errors.Remove(errors[j]);
          }
        }
      }
    } // FindDuplications( typeErrorsMap, synonymMap, duplicates )


    /// <summary>
    /// Writes out the error codes cs file.
    /// </summary>
    /// <param name="errors">Map of error type to list of errors of that type, read from the database.</param>
    /// <param name="synonymMap">Maps an error to a list of errors that have the same name.  We want to show all error codes for a particular name.</param>
    /// <param name="duplicates">List of errors that duplicate others.  We don't want to add two errors with the same name.</param>
    private static void WriteErrorCodes(Dictionary<string, List<DatabaseError>> errors, Dictionary<DatabaseError, List<DatabaseError>> synonymMap, List<DatabaseError> duplicates)
    {
      using (TextWriter writer = new StreamWriter("ErrorCodes.cs", false))
      {
        writer.WriteLine(string.Format("// Copyright © {0}, Viatronix Inc., All Rights Reserved.\r\n//\r\n// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file\r\n// may not be disclosed to third parties, copied or duplicated in any form,\r\n// in whole or in part, without the prior written permission of Viatronix Inc.\r\n//\r\n// Owner: Kyle R Chalupa(mailto:kchalupa@viatronix.com)\r\n", DateTime.Now.Year));
        writer.WriteLine("using System;\r\n\r\nnamespace Viatronix\r\n{\r\n  /// <summary>\r\n  /// The list of error codes that the database emits.\r\n  /// </summary>\r\n  public static class ErrorCodes\r\n  {\r\n");

        foreach (KeyValuePair<string, List<DatabaseError>> keyPair in errors)
        {
          writer.WriteLine(string.Format("    #region {0} errors", keyPair.Key.ToLower()));

          foreach(DatabaseError error in keyPair.Value)
          {
            // We don't want to output a duplicated error.
            if (duplicates.IndexOf(error) < 0)
            {
              writer.WriteLine();
              writer.WriteLine("    /// <summary>");
              writer.WriteLine(string.Format("    /// {0}.  Error code: {1}.", error.Description.Trim(), error.Code));

              List<DatabaseError> synonyms = null;
              if(synonymMap.TryGetValue(error, out synonyms))
              {
                StringBuilder errorCodeBuilder = new StringBuilder("    /// This error is duplicated by:");
                foreach (DatabaseError synonym in synonyms)
                {
                  if (error.Code != synonym.Code)
                  {
                    errorCodeBuilder.Append(" " + synonym.Code);
                  }
                }

                writer.WriteLine(errorCodeBuilder.ToString() + ".");
              }

              writer.WriteLine("    /// </summary>");
              writer.WriteLine(string.Format("    public const string {0} = \"{1}\";", error.Name, error.Code));
            }
          }

          writer.WriteLine();
          writer.WriteLine("    #endregion");
          writer.WriteLine();
        }

        writer.WriteLine("  } // class ErrorCodes\r\n} // namespace Viatronix");
      }
    } // WriteErrorCodes( errors, synonymMap, duplicates )


    /// <summary>
    /// Writes the exceptions cs file.
    /// </summary>
    /// <param name="errors">Map of the type to a list of errors.</param>
    /// <param name="synonymMap">Map of error to list of errors with same name.  Use this to write out duplicate codes.</param>
    /// <param name="duplicates">List of duplicate errors.  We don't want to write out multiple exceptions with the same name.</param>
    public static void WriteExceptions(Dictionary<string, List<DatabaseError>> errors, Dictionary<DatabaseError, List<DatabaseError>> synonymMap, List<DatabaseError> duplicates)
    {
      foreach (KeyValuePair<string, List<DatabaseError>> keyPair in errors)
      {
        string system = keyPair.Key.ToLower();
        if (system.Length > 0)
        {
          string firstChar = keyPair.Key.Substring(0, 1).ToUpper();
          system = firstChar + keyPair.Key.Substring(1, system.Length - 1).ToLower();
        }

        using (TextWriter writer = new StreamWriter(system + "Exceptions.cs", false))
        {
          writer.WriteLine(string.Format("// Copyright © {0}, Viatronix Inc., All Rights Reserved.\r\n//\r\n// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file\r\n// may not be disclosed to third parties, copied or duplicated in any form,\r\n// in whole or in part, without the prior written permission of Viatronix Inc.\r\n//\r\n// Owner: Kyle R Chalupa(mailto:kchalupa@viatronix.com)\r\n", DateTime.Now.Year));
          writer.WriteLine("using System;\r\nusing System.Runtime.Serialization;\r\n\r\nnamespace Viatronix\r\n{");

          foreach (DatabaseError error in keyPair.Value)
          {
            // Make sure we're not writing out a duplicate exception.
            if (duplicates.IndexOf(error) < 0)
            {
              writer.WriteLine();
              writer.WriteLine(string.Format("  #region {0}Exception class", error.Name));

              writer.WriteLine("\r\n  /// <summary>");
              writer.WriteLine(string.Format("  /// Implements a {0} exception.  Error code: {1}.", error.Name, error.Code));

              List<DatabaseError> synonyms = null;
              if (synonymMap.TryGetValue(error, out synonyms))
              {
                StringBuilder errorCodeBuilder = new StringBuilder("  /// This error is duplicated by:");
                foreach (DatabaseError synonym in synonyms)
                {
                  if (error.Code != synonym.Code)
                  {
                    errorCodeBuilder.Append(" " + synonym.Code);
                  }
                }

                writer.WriteLine(errorCodeBuilder.ToString() + ".");
              }

              writer.WriteLine(string.Format("  /// {0}", error.Description));
              writer.WriteLine("  /// </summary>");
              writer.WriteLine("  public class " + error.Name + "Exception : ViatronixException\r\n  {");

              // Write out the base constructor.
              writer.WriteLine("\r\n    /// <summary>\r\n    /// Base constructor.\r\n    /// </summary>\r\n    public " + error.Name + "Exception() { }");

              // Write out the constructor that takes an error message.
              writer.WriteLine("\r\n    /// <summary>\r\n    /// Constructor with a message.\r\n    /// </summary>\r\n    /// <param name=\"message\">A message indicating details of the error.\r\n    public " + error.Name + "Exception(string message) : base(message) { }");

              // Write out the constructor that takes an error message and inner exception.
              writer.WriteLine("\r\n    /// <summary>\r\n    /// Constructor with a message and inner exception.\r\n    /// </summary>\r\n    /// <param name=\"message\">A message indicating details of the error.</param>\r\n    /// <param name=\"exception\">The inner exception</param>\r\n    public " + error.Name + "Exception(string message, Exception exception) : base(message, exception) { }");

              // Write out the constructor that deserializes the exception.
              writer.WriteLine("\r\n    /// <summary>\r\n    /// Constructor with a serialized exception.\r\n    /// </summary>\r\n    /// <param name=\"info\">The serialized data.</param>\r\n    /// <param name=\"context\">The serialization context.</param>\r\n    public " + error.Name + "Exception(SerializationInfo info, StreamingContext context) : base(info, context) { }");

              writer.WriteLine("\r\n  } // class " + error.Name + "Exception\r\n\r\n  #endregion");
            }
          }

          writer.WriteLine();
          writer.WriteLine("} // namespace Viatronix");
        }
      }
    } // WriteExceptions( errors, synonymMap, duplicates )


    /// <summary>
    /// Writes the exception factory cs file.
    /// </summary>
    /// <param name="errors">Map of the type to a list of errors.</param>
    /// <param name="synonymMap">Map of error to list of errors with same name.  Use this to write out duplicate codes.</param>
    /// <param name="duplicates">List of duplicate errors.  We don't want to write out multiple exceptions with the same name.</param>
    public static void WriteExceptionFactory(Dictionary<string, List<DatabaseError>> errors, Dictionary<DatabaseError, List<DatabaseError>> synonymMap, List<DatabaseError> duplicates)
    {
      using (TextWriter writer = new StreamWriter("DatabaseExceptionFactory.cs", false))
      {
        writer.WriteLine(string.Format("// Copyright © {0}, Viatronix Inc., All Rights Reserved.\r\n//\r\n// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file\r\n// may not be disclosed to third parties, copied or duplicated in any form,\r\n// in whole or in part, without the prior written permission of Viatronix Inc.\r\n//\r\n// Owner: Kyle R Chalupa(mailto:kchalupa@viatronix.com)\r\n", DateTime.Now.Year));
        writer.WriteLine("using System;\r\n\r\nnamespace Viatronix.Enterprise.Data\r\n{");
        writer.WriteLine("  /// <summary>\r\n  /// Creates an Enterprise Exception from the provided error code.\r\n  /// </summary>\r\n  public static class DatabaseExceptionFactory\r\n  {");
        writer.WriteLine("\r\n    /// <summary>\r\n    /// Creates an exception of the specified type with the provided message.\r\n    /// </summary>\r\n    /// <param name=\"e\">The provided exception.</param>\r\n    /// <returns>Type specific exception with the provided message.</returns>\r\n    public static Exception Create( Exception e )\r\n    {");
        writer.WriteLine("      string message = e.Message.Substring( 7 ).Trim();\r\n      switch( e.Message.Substring( 0, 6 ))\r\n      {");

        foreach(KeyValuePair<string, List<DatabaseError>> keyPair in errors)
        {
          foreach (DatabaseError error in keyPair.Value)
          {
            // We don't want to put out a duplicate code.
            if (duplicates.IndexOf(error) < 0)
            {
              writer.WriteLine("        case ErrorCodes." + error.Name + ":");

              List<DatabaseError> synonyms = null;
              if (synonymMap.TryGetValue(error, out synonyms))
              {
                foreach (DatabaseError synonym in synonyms)
                {
                  if (error.Code != synonym.Code)
                  {
                    writer.WriteLine("        case \"" + synonym.Code + "\":");
                  }
                }
              }

              writer.WriteLine("          return new " + error.Name + "Exception(message);");
            }
          }
        }

        writer.WriteLine("        default:");
        writer.WriteLine("          return new ApplicationException( e.Message );");

        writer.WriteLine("      }\r\n    } // Create( e )\r\n\r\n  } // class DatabaseExceptionFactory\r\n} // namespace Viatronix.Enterprise.Data");
      }
    } // WriteExceptionFactory( errors, synonymMap, duplicates )


    /// <summary>
    /// Writes out a readme file that describes the files and where they need to be moved to in the Enterprise Repository.
    /// </summary>
    /// <param name="errors">Map of types to list of errors that have that type.</param>
    private static void WriteReadme(Dictionary<string, List<DatabaseError>> errors)
    {
      List<string> entrants = new List<string>();
      entrants.Add("ErrorCodes.cs");
      entrants.Add("DatabaseExceptionFactory.cs");
      foreach (KeyValuePair<string, List<DatabaseError>> keyPair in errors)
      {
        entrants.Add(keyPair.Key + "Exceptions.cs");
      }

      // Get the maximum needed length.
      int maxLen = 0;
      foreach (string entrant in entrants)
      {
        if (entrant.Length > maxLen)
        {
          maxLen = entrant.Length;
        }
      }

      // Allow for spacing between the two columns.
      maxLen += 2;

      using (TextWriter writer = new StreamWriter("Readme.txt", false))
      {
        writer.WriteLine("----------------------------------------------------------------------------------------------------");
        writer.WriteLine("This file describes the purposes and intended locations of the files that this utility generates.");
        writer.WriteLine("----------------------------------------------------------------------------------------------------");
        writer.WriteLine();

        writer.WriteLine("This utility is intended to simplify and shorten the time to create the source code that handles to Error Codes in the database.  The needed functionality encompasses three parts:");
        writer.WriteLine();

        writer.WriteLine(PadStringSpaces("ErrorCodes.cs", maxLen) + "This file lists out the errors in the database sorted by subsystem.  With a description of the error and the error code(s) that the database emits.");
        writer.WriteLine(PadStringSpaces(string.Empty, maxLen) + "This file should be checked into the Enterprise repository in src\\Viatronix.Enterprise\\ErrorCodes.cs");

        foreach(KeyValuePair<string, List<DatabaseError>> keyPair in errors)
        {
          string excepFileName = keyPair.Key.ToLower();
          if (excepFileName.Length > 0)
          {
            string firstChar = keyPair.Key.Substring(0, 1).ToUpper();
            excepFileName = firstChar + keyPair.Key.Substring(1, excepFileName.Length - 1).ToLower() + "Exceptions.cs";
          }

          writer.WriteLine(PadStringSpaces(excepFileName, maxLen) + "This file lists out the exceptions that relate to the " + excepFileName + " subsystem.");
          writer.WriteLine(PadStringSpaces(string.Empty, maxLen) + "This file should be checked into the Enterprise repository in src\\Viatronix.Enterprise\\exceptions\\" + excepFileName);
        }

        writer.WriteLine(PadStringSpaces("DatabaseExceptionFactory.cs", maxLen) + "This file provides a factory that can create our Exception derived objects from the database error message.");
        writer.WriteLine(PadStringSpaces(string.Empty, maxLen) + "This file should be checked into the Enterprise repository in src\\Viatronix.Enterprise.Server\\database\\DatabaseExceptionFactory.cs");
      }
    } // WriteReadme( errors )


    /// <summary>
    /// Pads the string to the provided length with spaces.
    /// </summary>
    /// <param name="str">The input string.</param>
    /// <param name="length">The length of the desired string.</param>
    /// <returns>The padded string.</returns>
    private static string PadStringSpaces(string str, int length)
    {
      int padLength = length - str.Length;
      StringBuilder padding = new StringBuilder();
      
      // Make sure the string needs to be padded.
      if (padLength > 0)
      {
        for (int i = 0; i < padLength; ++i)
        {
          padding.Append(' ');
        }
      }

      return str + padding.ToString();
    } // PadStringSpaces( str, length )

    #endregion

  } // class Program
} // namespace Viatronix.Utilities.InHouse.DatabaseErrorCodeGenerator
