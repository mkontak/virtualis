// PurgeLogs.C : Defines the entry point for the console application.
//


using namespace System;

/**
 * Prints the usage
 */
void PrintUsage()
{

 
  Console::WriteLine("ArchiveLogs [-logpath|-l PATH_TO_LOG_FILES]  [-maxdays|-d DAYS_TO_WAIT_BEFORE_DELETING] [-archive|-a ACHIVE_SUB_DIRECTORY] [-s]");
  Console::WriteLine("");
  Console::WriteLine("");
  Console::WriteLine("Synopsis:");
  Console::WriteLine("");
  Console::WriteLine("-logpath|-l   Specifies the log path to be purged. ( Default is C:\\Program Data\\Viatronix\\Log ).");
  Console::WriteLine("");    
  Console::WriteLine("-maxdays|-d   Specifies the maximum number of days to keep the old log files around (Deafult is 7).");
  Console::WriteLine("");
  Console::WriteLine("-archive|-a   Specifies the archive directory or sub-directoryto be used, (Default is \"ArchivedLogs\")");
  Console::WriteLine("");
  Console::WriteLine("-v            Specifies the program is verbose");
  Console::WriteLine("");
  Console::WriteLine("");
  Console::WriteLine("Description:");
  Console::WriteLine("");
  Console::WriteLine("This program will purge all the log files found in the path specified in the -logPath");
  Console::WriteLine("arguement. The files to be purged are only those with a .txt or .xml extension but all"); 
  Console::WriteLine("database backup log files will be ignored."); 
  Console::WriteLine("");
  Console::WriteLine("The first stage of the process will perform the purging by moving the files to the archive"); 
  Console::WriteLine("path which is usually a sub-directory off the -logpath. If the archive path does not"); 
  Console::WriteLine("exist it will be created.");
  Console::WriteLine("");
  Console::WriteLine("");
  Console::WriteLine("The second stage of the prcess will delete and files in the old log path that are older"); 
  Console::WriteLine("-maxdays old. ");
  Console::WriteLine("");
  Console::WriteLine("");
  Console::WriteLine("");
  Console::WriteLine("NOTE: This files will only be moved once a day, which means that repeatedly running this"); 
  Console::WriteLine("      application with the same parameters will have no affect after it was run the first");
  Console::WriteLine("      time. The preserves the old log files so that they are not overritten acidentally.");
  Console::WriteLine("");
  Console::WriteLine("");
  Console::WriteLine("Example(s):");
  Console::WriteLine("");
  Console::WriteLine("");
  Console::WriteLine("ArchiveLogs");
  Console::WriteLine("");
  Console::WriteLine("Executing the application with no arguements will purge all files in the C:\\Program Files\\Viatronix\\Log ");
  Console::WriteLine("directory and move the files to the C:\\Program Files\\Viatronix\\Log diectory. All files in the ");
  Console::WriteLine("C:\\Program Files\\Viatronix\\Log directory older then 7 days will be deleted.");
  Console::WriteLine("");
  Console::WriteLine("");
  Console::WriteLine("ArchiveLogs -maxdays 0");
  Console::WriteLine("");
  Console::WriteLine("Executing ths application with following arguements will purge the files in the C:\\Program Files\\Viatronix\\Log");
  Console::WriteLine("directory. No files will be saved since the numbar of days to save the files is 0.");
  Console::WriteLine("");
  Console::WriteLine("");
  Console::WriteLine("Possible Issues:");
  Console::WriteLine("");
  Console::WriteLine("If the log path changes then the old log saved in the previous old log path will never be");
  Console::WriteLine("cleaned up. The program has no memory of what it did only what it's about to do.");
  Console::WriteLine("");
  Console::WriteLine("");
} // PrintUsage()

/**
 * Moves the source directories files to the destination directory file
 *
 * @param msSource      Source directory
 * @param msDest        Destination directory
 * @param verbose       Verbose flag
 */
void Move(String ^ msSource, String ^ msDest, bool verbose)
{
  /// Get the current date and time
  DateTime now(DateTime::Now);
  int moved(0);
 
  try
  {
    array<String ^> ^ mpFiles = System::IO::Directory::GetFiles(msSource, "*");

    // =======================
    // Move all the log files
    // ========================
    for each ( String ^ msFile in mpFiles )
    {

      String ^ msFilename(System::IO::Path::GetFileName(msFile));
      int i(0);

      String ^ msDestFilePath(String::Empty);

      do
      {

        String ^ msDestFilename(  i == 0 ? String::Format("{0}.{1}",now.ToString("yyyy_MM_dd"),msFilename) : String::Format("{0}_{1,2:D2}.{2}",now.ToString("yyyy_MM_dd"),i,msFilename) );

        msDestFilePath = System::IO::Path::Combine(msDest, msDestFilename);

        i++;

      } while ( System::IO::File::Exists(msDestFilePath) );


      try
      {

        
        if ( verbose )
          Console::WriteLine(String::Format("Moving {0} to {1}",msFile,msDestFilePath));

        System::IO::File::Move(msFile,msDestFilePath);

        moved++;
      }
      catch ( Exception ^  )
      {
        Console::WriteLine(String::Format("Failed to move {0}, continuing", msFile));
      }


    } // END ... For each file

    if ( verbose )
      Console::WriteLine(String::Format("{0} logs moved", moved));

  }
  catch ( Exception ^ ex )
  {
    throw;
  }
} // Move()

/**
 * Purges the destination directory files that are older then the days specified
 *
 * @param msPath      Source directory
 * @param days        Days old
 * @param verbose     Verbose flag
 */
void Purge(String ^ msPath, unsigned int days , bool verbose)
{
  try
  {

    /// Get the current date and time
    DateTime now(DateTime::Now);


    array<String ^> ^ mpFiles = System::IO::Directory::GetFiles(msPath, "*");

    int purged(0);

    // ======================================
    // Purge all the log files from the path
    // ======================================
    for each ( String ^ msFile in mpFiles )
    {

      String ^ msFilename(System::IO::Path::GetFileName(msFile));

      if ( msFilename[10] == '.' )
      {
        int iYear = Int32::Parse(msFilename->Substring(0,4));
        int iMonth = Int32::Parse(msFilename->Substring(5,2));
        int iDay = Int32::Parse(msFilename->Substring(8,2));

        DateTime file = DateTime(iYear,iMonth,iDay);
        
        TimeSpan span = now.Subtract(file);

        if ( span.Days > (int)days )
        {
          if ( verbose )
            Console::WriteLine(String::Format("Purging {0}",msFile));

          System::IO::File::Delete(msFile);

          purged++;
        }
      }
 
    } // END ... For each file

    if ( verbose )
      Console::WriteLine(String::Format("{0} archived logs purged", purged));
   
   }
  catch ( Exception ^  )
  {
    throw;
  }
} // Purge()

/**
 * Main entry intro the application
 */
int main(cli::array<String ^> ^ mpArgs)
{


  String ^ msLogPath(System::IO::Path::Combine(Environment::GetFolderPath(Environment::SpecialFolder::CommonApplicationData),"Viatronix\\Logs"));
  String ^ msArchive("ArchivedLogs");
  unsigned int iMaxDays(7);
  bool bVerbose(false);


  try
  {

    // =================================================================
    // Loop through all the arguements to pick up any default changes
    // =================================================================
    for ( int i(0); i < mpArgs->Length; ++i ) 
    {

      if ( mpArgs[i] == "-logpath" || mpArgs[i] == "-l" )
      {
        if ( ++i == mpArgs->Length )
          throw gcnew Exception("No log path specified");


        msLogPath = mpArgs[i];
      }
      else if ( mpArgs[i] == "-archive" ||  mpArgs[i] == "-a" )
      {
        if ( ++i == mpArgs->Length )
          throw gcnew Exception("No archive directory/sub-directory specified");


        msArchive = mpArgs[i];

      }
      else if ( mpArgs[i] == "-maxdays" ||  mpArgs[i] == "-d")
      {
        if ( ++i == mpArgs->Length )
          throw gcnew Exception("No max days specified");


        msArchive = mpArgs[i];

      }
      else if ( mpArgs[i] == "-help" || mpArgs[i] == "-h" )
      {
        PrintUsage();
        return 0;
      }
      else if ( mpArgs[i] == "-v" )
        bVerbose = true;
      else
        throw gcnew Exception("Invalid arguement specified");



    } // END ... For each arguement


    // =========================================
    // Make sure the log path specified exists
    // =========================================
    if ( ! System::IO::Directory::Exists(msLogPath) )
      throw gcnew System::IO::FileNotFoundException(String::Format("Specified log path {0} does not exist", msLogPath));

    
    String ^ msArchivePath(  ( msArchive->Contains("\\") || msArchive->Contains("/") ? msArchive : System::IO::Path::Combine(msLogPath, msArchive) ) );
   
    String ^ msParent(System::IO::Path::GetDirectoryName(msArchivePath));

    // ==========================================
    // Make sure the archive paths parent exists
    // ==========================================
    if ( ! System::IO::Directory::Exists(msParent) )
      throw gcnew System::IO::FileNotFoundException(String::Format("Specified archive parent path {0} does not exist", msParent));

    // =====================================================
    // If the archive path does not exists then create it
    // ====================================================
    if ( ! System::IO::Directory::Exists(msArchivePath) )
      System::IO::Directory::CreateDirectory(msArchivePath);


    Move(msLogPath, msArchivePath, bVerbose);

    Purge(msArchivePath, iMaxDays, bVerbose);
 

    return 0;

  }
  catch ( Exception ^ ex )
  {
    Console::WriteLine(String::Format("PurgeLogs failed : {0}", ex->Message));
    return -1;
  }

} // main()
