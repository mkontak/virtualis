// $Id: FileExtensions.h,v 1.5 2006/10/02 19:59:57 frank Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Jeff Meade (jmeade@viatronix.com)

#ifndef FileExtension_h
#define FileExtension_h

// namespace
namespace vx
{
  // enums

  /**
   * Different classifications of files
   */
  enum FileClassificationEnum
  {
    eFileClassVtxVolume,   // Viatronix volume files
    eFileClassVtxData,     // Viatronix data files
    eFileClassVtxReport,   // Viatronix report files
    eFileClassVtxLog,      // Viatronix log files
    eFileClassVideo,       // AVIs, MPGs, etc.
    eFileClassAudio,       // MP3s, WAVs, etc.
    eFileClassImage,       // JPEGs, GIFs, etc.
    eFileClassText,        // text files
    eFileClassProprietary, // data specific to other applications
    eFileClassUnknown      // any other file types
  }; // FileClassificationEnum


  /**
   * Operations on file name extensions
   */
  class VX_BASE_DLL FileExtension
  {
  // functions
  public:

    /// constructor
    FileExtension(const std::string & s, const FileClassificationEnum e);

    /// std::string cast operator
    operator const std::string() const { return m_sExt; }

    /// const std::string isequal operator
    bool operator==(const std::string & sRHSFilename) const;

    /// const char * isequal operator
    inline bool operator==(const char * sRHSFilename) const { return (*this == std::string(sRHSFilename)); }

    /// const std::string not equal operator
    inline bool operator!=(const std::string & sRHSFilename) const { return !(*this == sRHSFilename); }

    /// const char * not equal operator
    inline bool operator!=(const char * sRHSFilename) const { return !(*this == sRHSFilename); }

    /// size of extension less one (i.e. text length minus the period)
    inline std::string::size_type GetExtensionLen() const { return m_iExtLen; }

    /// std::string cast operator
    inline const std::string GetExtensionAsString() const { return m_sExt; }

  private:

    /// returns the extension of <s> if any
    const std::string InterpretAsExtension(const std::string & s) const;

  // data
  public:

    /// classification of the file
    const FileClassificationEnum m_eFileClass;

  private:

    /// extension string, stored as '.' and extension
    const std::string m_sExt;

    /// size of extension less one (i.e. text length minus the period)
    const std::string::size_type m_iExtLen;
  }; // FileExtension


#ifdef FILE_EXTENSIONS_C

// NOTE: THESE ARE IN A L P H A B E T I C A L ORDER FOR EASY PERUSAL...TRY TO KEEP THE ORDER!

  /// Adobe PDF File
  VX_BASE_DLL FileExtension AdobePDFExt              (".pdf", eFileClassProprietary);
  /// AVI Movie
  VX_BASE_DLL FileExtension AVIExt                   (".avi", eFileClassVideo);
  /// Viatronix Colon Annotation file
  VX_BASE_DLL FileExtension AnnotationExt            (".amk", eFileClassVtxData);

  /// Bitmap file
  VX_BASE_DLL FileExtension BitmapExt                (".bmp", eFileClassImage);
  /// V3DColon Bookmark file
  VX_BASE_DLL FileExtension BookmarkExt              (".mrk", eFileClassVtxData);

  /// CAD (computer-aided detection) file
  VX_BASE_DLL FileExtension CADFileExt               (".cad", eFileClassVtxData);
  /// another difference volume (converts from uncleansed to cleansed)
  VX_BASE_DLL FileExtension CleansedDifferenceVolExt (".cdv", eFileClassVtxData);
  /// Classification volume
  VX_BASE_DLL FileExtension ClassificationVolExt     (".cla", eFileClassVtxData);
  /// Closest centerline index file
  VX_BASE_DLL FileExtension ClosestSkelIndexExt      (".csi", eFileClassVtxData);
  /// V3DColon Session file
  VX_BASE_DLL FileExtension ColonSessionXMLExt       (".vcsx",eFileClassVtxData);
  /// CT8 file
  VX_BASE_DLL FileExtension CT8Ext                   (".ct8", eFileClassVtxData);

  /// the difference volume (converts from cleansed to uncleansed)
  VX_BASE_DLL FileExtension DifferenceVolExt         (".dif", eFileClassVtxData);
  /// DICOM slice file
  VX_BASE_DLL FileExtension DicomExt                 (".dcm", eFileClassVtxData);

  /// Electronically-cleansed coronal slice
  VX_BASE_DLL FileExtension ECleansedCoronalExt      (".sec", eFileClassVtxData);
  /// Electronically-cleansed sagittal slice
  VX_BASE_DLL FileExtension ECleansedSagittalExt     (".ses", eFileClassVtxData);
  /// Electronically-cleansed transverse slice
  VX_BASE_DLL FileExtension ECleansedTransverseExt   (".set", eFileClassVtxData);
  /// ECG signal file
  VX_BASE_DLL FileExtension ECGSignalExt             (".iri", eFileClassVtxData);
  /// Electronically-cleansed volume
  VX_BASE_DLL FileExtension ElecCleansedVolExt       (".ecv", eFileClassVtxVolume);
  /// Electronically-cleansed mask file
  VX_BASE_DLL FileExtension ElecCleansedMaskExt      (".msk", eFileClassVtxData);

  /// Graphics interchange format file
  VX_BASE_DLL FileExtension GIFExt                   (".gif", eFileClassImage);

  /// Windows help file
  VX_BASE_DLL FileExtension HelpFileExt              (".hlp", eFileClassProprietary);

  /// Inside label volume
  VX_BASE_DLL FileExtension InsideLabelVolExt        (".ins", eFileClassVtxData);

  /// Joint Photographic Experts Group picture file
  VX_BASE_DLL FileExtension JPEGExt                  (".jpg", eFileClassImage);

  /// V3D study lock file
  VX_BASE_DLL FileExtension LockExt                  (".lck", eFileClassUnknown);

  /// Viewed surface file
  VX_BASE_DLL FileExtension MarkedVolumeExt          (".mkvl",eFileClassVtxData);
  /// Motion Pictures Experts Group file, video only
  VX_BASE_DLL FileExtension MPEGVideoExt             (".m1v", eFileClassVideo);
  /// Motion Pictures Experts Group movie file
  VX_BASE_DLL FileExtension MPEGExt                  (".mpg", eFileClassVideo);

  /// Pipeline processing time file
  VX_BASE_DLL FileExtension PipelineTimeExt          (".tim", eFileClassVtxData);
  /// Potential distribution root file
  VX_BASE_DLL FileExtension PotentialDistRootExt     (".pdr", eFileClassVtxData);

  /// V3DColon report file
  VX_BASE_DLL FileExtension ReportExt                (".vrx", eFileClassVtxReport);
  /// Retrospective meta file
  VX_BASE_DLL FileExtension RetroSpectiveMetaExt     (".rsp", eFileClassVtxData);

  /// Scout image file
  VX_BASE_DLL FileExtension ScoutExt                 (".sct", eFileClassVtxData);
  /// V3DColon colon segments file
  VX_BASE_DLL FileExtension SegmentListExt           (".seg", eFileClassVtxData);
  /// Voxel space leap distance volume visualization file for cleansed volume
  VX_BASE_DLL FileExtension SpaceLeapDistExt         (".sld", eFileClassVtxData);
  /// Voxel space leap distance volume visualization file for uncleansed volume
  VX_BASE_DLL FileExtension UncleansedSpaceLeapDistExt(".uld", eFileClassVtxData);
  /// Abbreviated space leap distance file
  VX_BASE_DLL FileExtension SmallSLDExt              (".slv", eFileClassVtxData);

  /// Processing timing file
  VX_BASE_DLL FileExtension TimingsExt               (".time",eFileClassVtxData);
  /// Voxel threshold file
  VX_BASE_DLL FileExtension ThresholdExt             (".thr", eFileClassVtxData);
  /// Simple text file
  VX_BASE_DLL FileExtension TXTExt                   (".txt", eFileClassText);
  /// V3DColon colon overview preview file
  VX_BASE_DLL FileExtension TrianglePrevHdrExt       (".tph", eFileClassVtxData);
  /// V3DColon colon overview file
  VX_BASE_DLL FileExtension TriangleExt              (".tri", eFileClassVtxData);
  /// Temp file
  VX_BASE_DLL FileExtension TemporaryFileExt         (".tmp", eFileClassUnknown);
  /// Transfer function file
  VX_BASE_DLL FileExtension TFuncExt                 (".tfn", eFileClassVtxData);

  /// V3D Volume file
  VX_BASE_DLL FileExtension VolumeExt                (".vol", eFileClassVtxVolume);
  /// Windows media video file
  VX_BASE_DLL FileExtension WindowsMediaVideoExt     (".wmv", eFileClassVideo);
  /// Processor warnings file
  VX_BASE_DLL FileExtension WarningsLogExt           (".wrn", eFileClassVtxLog);

  /// eXtensible Markup Language file
  VX_BASE_DLL FileExtension XMLExt                   (".xml", eFileClassProprietary);

#else

// NOTE: THESE ARE IN A L P H A B E T I C A L ORDER FOR EASY PERUSAL...TRY TO KEEP THE ORDER!

  VX_BASE_DLL extern FileExtension AdobePDFExt;
  VX_BASE_DLL extern FileExtension AVIExt;
  VX_BASE_DLL extern FileExtension AnnotationExt;

  VX_BASE_DLL extern FileExtension BitmapExt;
  VX_BASE_DLL extern FileExtension BookmarkExt;

  VX_BASE_DLL extern FileExtension CADFileExt;
  VX_BASE_DLL extern FileExtension ClassificationVolExt;
  VX_BASE_DLL extern FileExtension CleansedDifferenceVolExt;
  VX_BASE_DLL extern FileExtension ClosestSkelIndexExt;
  VX_BASE_DLL extern FileExtension ColonSessionXMLExt;
  VX_BASE_DLL extern FileExtension CT8Ext;

  VX_BASE_DLL extern FileExtension DicomExt;
  VX_BASE_DLL extern FileExtension DifferenceVolExt;

  VX_BASE_DLL extern FileExtension ECleansedCoronalExt;
  VX_BASE_DLL extern FileExtension ECleansedSagittalExt;
  VX_BASE_DLL extern FileExtension ECleansedTransverseExt;
  VX_BASE_DLL extern FileExtension ECGSignalExt;
  VX_BASE_DLL extern FileExtension ElecCleansedVolExt;
  VX_BASE_DLL extern FileExtension ElecCleansedMaskExt;

  VX_BASE_DLL extern FileExtension GIFExt;

  VX_BASE_DLL extern FileExtension HelpFileExt;

  VX_BASE_DLL extern FileExtension InsideLabelVolExt;

  VX_BASE_DLL extern FileExtension JPEGExt;

  VX_BASE_DLL extern FileExtension LockExt;

  VX_BASE_DLL extern FileExtension MarkedVolumeExt;
  VX_BASE_DLL extern FileExtension MPEGVideoExt;
  VX_BASE_DLL extern FileExtension MPEGExt;

  VX_BASE_DLL extern FileExtension PipelineTimeExt;
  VX_BASE_DLL extern FileExtension PotentialDistRootExt;

  VX_BASE_DLL extern FileExtension ReportExt;
  VX_BASE_DLL extern FileExtension RetroSpectiveMetaExt;

  VX_BASE_DLL extern FileExtension ScoutExt;
  VX_BASE_DLL extern FileExtension SegmentListExt;
  VX_BASE_DLL extern FileExtension SpaceLeapDistExt;
  VX_BASE_DLL extern FileExtension UncleansedSpaceLeapDistExt;
  VX_BASE_DLL extern FileExtension SmallSLDExt;

  VX_BASE_DLL extern FileExtension ThresholdExt;
  VX_BASE_DLL extern FileExtension TXTExt;
  VX_BASE_DLL extern FileExtension TrianglePrevHdrExt;
  VX_BASE_DLL extern FileExtension TriangleExt;
  VX_BASE_DLL extern FileExtension TemporaryFileExt;
  VX_BASE_DLL extern FileExtension TFuncExt;

  VX_BASE_DLL extern FileExtension VolumeExt;

  VX_BASE_DLL extern FileExtension WindowsMediaVideoExt;
  VX_BASE_DLL extern FileExtension WarningsLogExt;

  VX_BASE_DLL extern FileExtension XMLExt;

#endif
} // namespace vx


/// Appends a file extension to a string (i.e. a file title), ensuring a single '.' separator
inline std::string operator+(const std::string & sStr, const vx::FileExtension & fileExt);

/// Returns true if the string (i.e. filename) has the given extension 
inline bool operator==(const std::string & sLHS, const vx::FileExtension & fileExt);

/// Returns true if the string (i.e. filename) does not have the given extension 
inline bool operator!=(const std::string & sLHS, const vx::FileExtension & fileExt);

/// Returns true if the string (i.e. filename) has the given extension 
inline bool operator==(const char * sLHS, const vx::FileExtension & fileExt);

/// Returns true if the string (i.e. filename) does not have the given extension 
inline bool operator!=(const char * sLHS, const vx::FileExtension & fileExt);

/// Determine if the filename title corresponds to a supine dataset.
VX_BASE_DLL bool IsSupineFilePrefix(const std::string & sFileTitle);

/// Convert prone filename to supine.
VX_BASE_DLL std::string GetSupineFilePrefixFromProne(const std::string & sProneFileTitle);
/// Convert supine filename to prone.
VX_BASE_DLL std::string GetProneFilePrefixFromSupine(const std::string & sSupineFileTitle);

#include "FileExtensions.inl"

#endif // File_Extensions_h


// $Log: FileExtensions.h,v $
// Revision 1.5  2006/10/02 19:59:57  frank
// Issue #5020: Added the uld file
//
// Revision 1.4  2006/04/24 15:16:46  jmeade
// cleanup.
//
// Revision 1.3  2005/11/01 14:49:18  frank
// Issue #4503: Read new .cdv and .vol files instead of _hq files
//
// Revision 1.2  2005/08/05 13:02:46  geconomos
// added extensions for pdf
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.21  2003/07/08 14:31:38  jmeade
// Code standards, comments.
//
// Revision 3.20  2003/06/05 16:27:32  jmeade
// code review.
//
// Revision 3.19  2003/05/13 19:09:08  michael
// code review
//
// Revision 3.18  2003/01/30 21:47:26  jmeade
// XML Session data file for Colon.
//
// Revision 3.17  2003/01/07 15:55:46  kevin
// Returning much-used xml and txt file extension instances.
//
// Revision 3.16  2003/01/06 15:03:18  ingmar
// moved .xml and .txt from static instance to static member function
//
// Revision 3.15  2002/09/12 18:02:18  jmeade
// Check-in from defunct/unused VC_1-2-3 branch: file extension name cleanup.
//
// Revision 3.14  2002/08/14 16:11:36  wenli
// initial version - Vascular
//
// Revision 3.13  2002/07/19 15:17:47  mkontak
// MoreVXDLL
//
// Revision 3.12  2002/07/18 15:11:55  mkontak
// Add VXDLL to class definitions
//
// Revision 3.11  2002/07/03 03:23:43  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_07-02-2002)
//
// Revision 3.10  2002/06/10 22:39:24  jmeade
// Merged with VC_1-2_CS-1 branch, tag:  Joined_On_06-10-2002
//
// Revision 3.9  2002/06/06 20:06:34  binli
// added retrospective extension + ECG signal file extension
//
// Revision 3.8  2002/03/11 16:02:03  mkontak
// Merged VC 1.2/CS 1.0
//
// Revision 3.7  2002/03/11 15:19:45  mkontak
// Merged VC 1-2/CS-1 into main trunk
//
// Revision 3.6.2.1  2002/02/26 22:20:03  mkontak
// Added additional files extensions
//
// Revision 3.6.2.3  2002/07/02 16:42:08  jmeade
// Allow extensions of any length; ext's for VC Session file, timings file.
//
// Revision 3.6.2.2  2002/06/06 14:18:55  jmeade
// Issue 2270:  Annotation extension.
//
// Revision 3.6.2.1  2002/02/26 22:20:03  mkontak
// Added additional files extensions
//
// Revision 3.6  2002/01/24 02:10:32  jmeade
// Help file extension.
//
// Revision 3.5  2002/01/12 01:46:28  jmeade
// Added file classifications (identifying types of files).
//
// Revision 3.4  2001/12/28 16:13:07  jmeade
// Added windows media extension.
//
// Revision 3.3  2001/12/20 15:00:15  ingmar
// added class Sec_03_utils to structure the doxygen documentation
//
// Revision 3.2  2001/12/07 18:48:00  kevin
// Added movie extension for saving mpegs
//
// Revision 3.1  2001/11/15 18:20:54  dmitry
// TFuncExt added
//
// Revision 3.0  2001/10/14 23:02:15  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.2   Oct 01 2001 14:14:08   soeren
// removed operator >>, there is now a another funtion called GetExtensionAsTring(). 
// 
//    Rev 2.1   Sep 19 2001 14:29:56   liwei
// No change.
// 
//    Rev 2.0   Sep 16 2001 23:46:16   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:33:32   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:08:29  soeren
// renamed utils directory
//
// Revision 1.13  2001/08/22 23:23:56  soeren
// made some changes to make it to compile with the intel compiler
//
// Revision 1.12  2001/08/21 20:52:54  binli
// added TxtExtension.
//
// Revision 1.11  2001/08/10 21:48:28  binli
// added dicom file extension
//
// Revision 1.10  2001/07/09 18:40:59  jmeade
// Adobe pdf extension
//
// Revision 1.9  2001/06/29 19:36:55  dmitry
// Added XML file extension.
//
// Revision 1.8  2001/06/12 18:45:02  dmitry
// ScoutExt added
//
// Revision 1.7  2001/04/30 02:20:04  kevin
// First part of ReaderGlobal Re-organize for improved object
// encapsulation before the Code Walkthrough
//
// Revision 1.6  2001/04/13 00:13:58  jmeade
// more implementation improvements; added two more filetypes
//
// Revision 1.5  2001/04/12 17:00:25  jmeade
// FileExtension.C implementation file; further simplified implementation
//
// Revision 1.4  2001/04/12 16:24:32  jmeade
// extended functionality, simplified use of class
//
// Revision 1.3  2001/04/11 18:40:32  dmitry
// Sec_1_utils inheritance added
//
// Revision 1.2  2001/04/08 21:38:20  dave
// updated doc headers
//
// Revision 1.1  2001/04/06 12:55:38  frank
// All file extensions should be defined in here.
// No hardcoded file extension strings outside of this class...
//
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/FileExtensions.h,v 1.5 2006/10/02 19:59:57 frank Exp $
// $Id: FileExtensions.h,v 1.5 2006/10/02 19:59:57 frank Exp $
