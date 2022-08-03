// $Id: VirtualFile.cs,v 1.1.2.4 2009/06/04 18:52:55 kchalupa Exp $
//
// Copyright © 2009, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Kyle R Chalupa] [kchalupa@viatronix.com]

using System;
using System.IO;
using System.Collections.Generic;
using System.Text;
using System.Xml;

using Viatronix.Logging;
using Viatronix.Enterprise.Entities;
using Viatronix.Utilities.IO;

namespace Viatronix.Utilities.Importer.Data
{
  /// <summary>
  /// A humble class to hold where the file was and where it will be copied to.
  /// </summary>
  public class VirtualFile : IFile
  {

    #region fields

    /// <summary>
    /// The series to which this file belongs.
    /// </summary>
    private ParsedSeries m_parsedSeries = null;

    /// <summary>
    /// The contents of the file as a string.
    /// </summary>
    private string m_contents = null;

    /// <summary>
    /// The final location of the file (relative.)
    /// </summary>
    private string m_relativeDestination = string.Empty;

    /// <summary>
    /// The UID to insert into the provided {0} spot.
    /// </summary>
    private string m_uidMacroReplace = string.Empty;

    #endregion

    #region construction

    /// <summary>
    /// Verbose constructor.  Sets the fields to the given values.
    /// </summary>
    /// <param name="contents">The contents of the file as a string.</param>
    /// <param name="relativeDestination">The final ioArgs where the file will end up.</param>
    /// <param name="uidMacroReplace">
    /// The UID that should replace the placeholder character in 
    /// destination, if one exists.  If null or "" pSeries.Series.UID is used.
    /// </param>
    public VirtualFile(ParsedSeries pSeries, string contents, string relativeDestination, string uidMacroReplace)
    {
      m_parsedSeries = pSeries;
      m_contents = contents;
      m_relativeDestination = relativeDestination;

      if (uidMacroReplace == null || uidMacroReplace == string.Empty)
      {
        m_uidMacroReplace = pSeries.Series.Uid.Replace(".", "_");
      } // if(uidMacroReplace != null && uidMacroReplace != string.Empty)
      else
      {
        m_uidMacroReplace = uidMacroReplace;
      }
    } // VirtualFile(ParsedSeries pSeries, string contents, string relativeDestination, string uidMacroReplace)

    #endregion

    #region properties

    /// <summary>
    /// The contents that will be written into the file.
    /// </summary>
    public string Contents
    {
      get { return m_contents; }
      set { m_contents = value; }
    } // Contents


    /// <summary>
    /// The relative destination of where the file will end up.
    /// </summary>
    public string Destination
    {
      get { return GenerateFileName(); }
      set { m_relativeDestination = value; }
    } // Destination

    #endregion

    #region methods

    /// <summary>
    /// Gets the extension of the file.
    /// </summary>
    /// <returns>
    /// The return of which falls into the following.
    /// 
    ///     Value          Returns
    /// xxxxxxx_1p.dcm      .dcm
    /// xxxxxxx_1p.ecv      .ecv
    /// xxxxxxx_1p_0.pdr    _0.pdr
    /// xxxxxxx_1p_1.csi    _1.csi
    /// </returns>
    public string GetExtension()
    {
      // Get the filename and the extension without the path.
      string fullname = Path.GetFileName(m_relativeDestination.ToLower());
      // Find the index of the first period.
      int index = fullname.IndexOf('.');
      // If the character two to the lhs is an underbar return that position and beyond, if not,
      // return the position of the period and beyond.

      if( index > 2 )
      {
        index = ( fullname[index - 2] == '_' ) ? index - 2 : index;
      } // if(index > 2)

      // Fix for _hq removal causing file exists conflicts.
      // If three before the index equals _hq return three less than index.
      if( index > 3 )
      {
        index = ( fullname.Substring(index - 3, 3) == "_hq" ) ? index - 3 : index;
      } // if(index > 3)

      return fullname.Substring(index);
    } // GetExtension()


    /// <summary>
    /// If the relative destination includes a placeholder character, return the UID as the fileName
    /// and the results of GetExtension.
    /// </summary>
    /// <returns>
    /// Either the current relative destination if it doesn't include a placeholder character
    /// or the UID and extension.
    /// </returns>
    public string GenerateFileName()
    {
      return string.Format(m_relativeDestination, m_uidMacroReplace + GetExtension());
    } // GenerateFileName()


    /// <summary>
    /// Performs a copy of the file, creating all subdirectories that are included in GenerateFileName().
    /// </summary>
    /// <param name="baseDirectory">The base directory that we should copy the file to. Ex C:\</param>
    public virtual void Copy(string baseDirectory)
    {
      // Cut the filesize down to 232 characters.
      FileSize fSize = new FileSize(GenerateFileName(), 255);
      string fileName = fSize.FilePath;

      // Get all directories within the GenerateFileName() return.
      string[] segments = GenerateFileName().Split('\\');

      string path = Path.Combine(baseDirectory, m_parsedSeries.Series.Directory);

      // Loop through the inner directories in GenerateFileName() creating them as they are found.
      for( int i = 0; i < segments.Length - 1; i++ )
      {
        path = Path.Combine(path, segments[i]);
        if( !Directory.Exists(path) )
        {
          Directory.CreateDirectory(path);
        } // if(!Directory.Exists(path))
      } //for(int i=0;i<segments.Length-1;i++)

      // If there are more than one segment take the final one and copy the file to that name.
      if( segments.Length > 0 )
      {
        fileName = segments[segments.Length - 1];
      } // if(segments.Length > 0)

      try
      {
        // Check to see if the file exists if not, copy it.
        if( !File.Exists(Path.Combine(path, fileName)) )
        {

          // Write out the file from the string.
          using( TextWriter writer = new StreamWriter(Path.Combine(path, fileName), true, Encoding.ASCII) )
          {
            writer.WriteLine(m_contents);

            writer.Flush();
          } // using(TextWriter writer = new StreamWriter(m_logFileName, true))
        } // if( !File.Exists(Path.Combine(path, fileName)) )
        else
        {
          // Log that the file already exists.

          Log.Warning(m_parsedSeries.Series.StudyUid.Replace(".", "_") + "; Series: " + 
            m_parsedSeries.Series.Uid.Replace(".", "_") + "; " + Path.Combine(path, fileName) + 
            ";Copying of file failed.  File with same name exists within directory.", "VirtualFile", "Copy");

        } // else
      } // try
      catch( IOException e )
      {
        Log.Warning(m_parsedSeries.Series.StudyUid.Replace(".", "_") + "; Series: " +
          m_parsedSeries.Series.Uid.Replace(".", "_") + "; " + Path.Combine(path, fileName) +
          "; Copying of file failed.  Due to IO Error " + e.Message, "VirtualFile", "Copy");

        throw;
      } // catch(IOException e)
    } // Copy(string baseDirectory)


    /// <summary>
    /// Gets the source path for the file.
    /// </summary>
    /// <returns>The original location of the file.</returns>
    public string GetSourceFileName()
    {
      return Path.GetFileName(Destination);
    } // GetSourceFileName()


    /// <summary>
    /// Get the FileName of the destination.
    /// </summary>
    public string GetDestinationFileName()
    {
      return Path.GetFileName(Destination);
    } // GetDestinationFileName()

    #endregion

  } // class VirtualFile
} // namespace Viatronix.Utilities.Importer.Data

#region revision history

// $Log: VirtualFile.cs,v $
// Revision 1.1.2.4  2009/06/04 18:52:55  kchalupa
// Coding standards.
//
// Revision 1.1.2.3  2008/10/20 16:06:21  kchalupa
// Fix for VC Sessions to remove extra datasets that the user may have sent that threw off the VRX file's integrity.
//
// Revision 1.1.2.2  2008/10/08 18:00:47  kchalupa
// Added login/context feature for luanching from Console and stand-alone
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities.Importer/VirtualFile.cs,v 1.1.2.4 2009/06/04 18:52:55 kchalupa Exp $
// $Date: 2009/06/04 18:52:55 $24 July 2008
// $Id: VirtualFile.cs,v 1.1.2.4 2009/06/04 18:52:55 kchalupa Exp $000000004

#endregion
