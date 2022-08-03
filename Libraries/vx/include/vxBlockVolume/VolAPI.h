// $Id: VolAPI.h,v 1.3 2006/02/03 15:22:11 frank Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Created: Summer 00
// Author: Dave & Jay  dave@viatronix.com
//
// Complete History at bottom of file.


#ifndef VolAPI_h
#define VolAPI_h


// includes
#include "Triple.h"
#include "Matrix.h"
#include "Date.h"
#include "VarMap.h"
#include "vxInflate.h"


// forward declarations
class vx::Date;
template <class T> class Point;
template <class T> class Vector;
template <class T> class LinearVolume;
template <class T> class BlockVolume;
template <class T> class RecBlockVolume;

namespace hdr {

   enum ExtensionEnum
    /**
     * enumeration of possible volume file extensions.
     
     * This is a potential bug, if new extensions are added inbetween old ones. 
     * Upon change to the next volume file version, 
     * this should change to become the associated string only.
     */
  {
    VOL,   ECV,   INS,   SKE,   DFB,   DDB,   SLD,   DFS,   DFE,   PDE,   SLC,   UNKNOWN_EXT, NUM_OF_EXTENSION_ENUM
  };
#define EXTENSION_STRINGS \
  {".VOL",".ECV",".INS",".SKE",".DFB",".DDB",".SLD",".DFS",".DFE",".PDE",".SLC",".UNKNOWN"};
  
#define UNIT_TYPE_STRINGS \
  {"METER","MILLIMETER","MICRON","FOOT","INCH"};
  enum UnitTypeEnum
  {
    METER,  MILLIMETER,  MICRON,  FOOT,  INCH,  NUM_OF_UNIT_TYPE_ENUM
  };

#define MEMORY_LAYOUT_STRINGS \
  {"LINEAR","HASH","BLOCK","RECURSIVE BLOCKING"};
  enum MemoryLayoutEnum
  {
    LINEAR, HASH, BLOCK, REC_BLOCK, NUM_OF_MEMORY_LAYOUT_ENUM
  };
  enum ResampleTypeEnum
  {
    POINT, TRILIN, BOX, TENT, GAUSS, MAJORITY, WAVELET, NUM_OF_RESAMPLE_TYPE_ENUM
  };
  enum PermuteCoordinatesEnum
  {
    TRANSVERSE_TO_CORONAL, TRANSVERSE_TO_SAGGITTAL, CORONAL_TO_SAGGITTAL, NUM_OF_PERMUTE_COORDINATES_ENUM
  };

/**
 * This class stores all generic volume
 * variables and provides several utility functions.  The virtual functions
 * specify the minimum interface subclasses must support to be instantiable.
 * The majority of the "hdr" namespace is found here, this should correspond
 * directly with the current version of the Viatronix File Specification
 * document.
 * $Id: VolAPI.h,v 1.3 2006/02/03 15:22:11 frank Exp $ <a href="../../vxBase/8_volume/Spec/VolumeDesign_Specification.htm">Design Specification</a>
  */
  template <class T>
    class VolAPI 
  {
  public:
    VolAPI();                                            // default constructor
    VolAPI(const Triple<uint4> & dim, 
           const uint4 & uiBitsPerVoxel=sizeof(T)*8,     // full initializer
           const Triple<float4> & units = Triple<float4>(1.0,1.0,1.0),
           hdr::UnitTypeEnum unitType = hdr::MILLIMETER,
           const T & background = 0, hdr::ExtensionEnum ext=VOL,
           const std::string & sFileName = "Blank.vol",
           const std::string & sTitle    = "Blank",
           const std::string & sAuthor   = "Blank",
           const vx::Date & date = vx::Date::GetCurrentDate());
    // copy constructor
    VolAPI(const VolAPI<T> & rhs);            
    // assignment operator
    VolAPI<T> const & operator=(const VolAPI<T> & rhs);   
    // destructor
    virtual ~VolAPI();                                                

    /// Sets the value at the location specified.
    inline void SetVoxel(const Point<uint2> & location, T value);
    /// Sets the value at the location specified, NO bounds checking.
    inline void SetVoxelRaw(const Point<uint2> & location, T value);
    /// Returns the value of the voxel at the location specified by point, NO bounds checking.
    inline const T & GetVoxelRaw(const Point<uint2> & location) const;

    /// Returns the current background value.
    inline const T & GetBackground() const;

    /// Forces the given compression type for file storage.
    inline void SetCompression(hdr::FileCompressionEnum compressType);
    /// Returns the current compression, normally m_bestCompression.
    inline hdr::FileCompressionEnum GetCompression() const;

    /// Returns the extension of the volume as read from the file header.
    inline ExtensionEnum GetExtension() const;

    /// Sets the author/source string in the volume header.
    inline void SetAuthor(const std::string & sAuthor);
    /// Returns the author/source string in the volume header.
    inline const std::string & GetAuthor() const;

    /// Sets the volume title.
    inline void SetTitle(const std::string & sTitle);
    /// Returns the title of the volume.
    inline const std::string & GetTitle() const;

    /// Sets the date on which the file is created.
    inline void SetCreationDate(vx::Date d);
    /// Returns the date on which the file was created.
    inline const vx::Date & GetCreationDate() const;

    /// Returns the unitType of the stored volume such as METER, MICRON, etc.
    inline UnitTypeEnum GetUnitType() const;

    /// Returns the x,y,z voxel dimensions of the volume.
    inline const Triple<uint4> & GetDim() const;
    /// Returns the x,y,z units of the volume.
    inline const Triple<float4> & GetUnits() const;
    /// Sets the x,y,z units of the volume.
    inline void SetUnits(const Triple<float4> & units);

    /// Returns the bits per voxel.
    inline const uint4 & GetBitsPerVoxel() const;
    /// Returns the bytes per voxel.
    inline const uint4 GetBytesPerVoxel() const;
    /// Returns the total memory usage in bytes.
    virtual const uint4 GetMemoryUsage() const = 0;
    /// Reports relevant volume layout statistics
    virtual void Stats(std::ostream & os = std::cout) const = 0;

    /// Returns the Filename string
    inline const std::string & GetFileName() const;
    /// Returns the Filename string without the extension not including the ".".
    inline std::string GetFileNameRoot() const;
    /// Sets the file name
    inline void SetFileName(const std::string & sFileName);

    /// Returns a matrix for converting from volume space into world space.
    const Matrix<float4> & GetVolumeToWorldMatrix() const;
    /// Returns a matrix for converting from world space into volume space.
    const Matrix<float4> & GetWorldToVolumeMatrix() const;

    /// Returns true if the point lies within the volume.
    inline bool InsideVolume(const Point<uint2> & p) const;

    /// Prints all file header variables to the ostream.
    std::ostream & AsOstream (std::ostream & os = std::cout) const;

    /// Reports the number of voxels
    const uint4 GetNumberOfVoxels() const;

    /// Returns the header map
    const VarMap & GetHdrMap() const
    {
      return(m_headerMap);
    }

    /// Sets the header map, should only be used in conjunction with GetHdrMap().
    void SetHdrMap(const VarMap & newMap)
    {
      // should verify new map first for correctness

      m_headerMap = newMap;
    }

    /// Set a new header variable name, value and description.
    template <class Type> 
    void SetHdrVar(const std::string & sVarName, const Type & value, 
                   const std::string & sDesc)
    {
      m_headerMap.SetHdrVar(sVarName, value, sDesc);
    }

    /// Looks up varName and assigns value.
    template <class Type>
    bool GetHdrVar(const std::string & sVarName, Type & value)
    {
      return(m_headerMap.GetHdrVar(sVarName, value));
    }

    /// Looks up sVarName and assigns value.
    template <>
    bool GetHdrVar(const std::string & sVarName, std::string & sValue)
    {
      return(m_headerMap.GetHdrVar(sVarName, sValue));
    }

		/// Return memory layout
		hdr::MemoryLayoutEnum GetMemoryLayout() {return m_eMemoryLayout; }
  protected: //functions
    // copies all VolAPI data
    void Copy(VolAPI<T> const & rhs);                                
     // destroys all VolAPI data
    void Destroy();                                                  
    /// Builds the ascii header for a volume according to Viatronix specs.
    const std::string BuildAsciiHeader(MemoryLayoutEnum sl, ExtensionEnum ext);
    /// Reads and assigns the common header fields from headerPtr.
    MemoryLayoutEnum ReadCommonHeader(const std::string & sHeaderPtr);
    /// Checks if the data read in are in the acceptable range.
    void VerifyHeader(const std::string & sDataType = "NULL");
    //// Check that m_bitsPerVoxel and m_dataType are compatible
    virtual void CheckBitsPerVoxel();
    /// Returns the enum matching a particular suffix.
    ExtensionEnum GetExtension(const std::string & suffix);
    /// Initialize the vectors that contain the type of data etc.
    void InitializeTypes();

  protected: //data
    uint4 m_id;
    Triple<uint4> m_dimension;  // x,y,z volume dimensions
    uint4 m_uiBitsPerVoxel;       // sig. bits per voxel
    Triple<float4> m_units;     // x,y,z units i.e. 1.2,1.4,1.7
    UnitTypeEnum m_unitType;    // type of units i.e. METER, MILLI, etc
    Matrix<float4> m_mVolumeToWorld;  // to transform from volume to world coordinates
    Matrix<float4> m_mWorldToVolume;  // to transform from world to volume coordinates
    const FileCompressionEnum m_bestCompression; //best method available
    FileCompressionEnum m_fileCompression;  // compression being used
    T m_background;            // value returned when access outside
    ExtensionEnum m_extension;
    std::string m_sTitle, m_sAuthor;
    std::string m_sFileName;
    VarMap m_headerMap;

    const uint4 m_uiMaxTitleLength;
    const uint4 m_uiMaxAuthorLength;

    vx::Date m_creationDate;

    std::vector<std::string> UnitTypeStr;
    std::vector<std::string> ExtensionStr;
    std::vector<std::string> MemoryLayoutStr;
		
		hdr::MemoryLayoutEnum m_eMemoryLayout;
  public:
    //PNGImage m_icon; // public so BuildIcon() in Volume.h can access it
  }; // class VolAPI


#include "VolAPI.inl"
}  //end namespace

#endif


// $Log: VolAPI.h,v $
// Revision 1.3  2006/02/03 15:22:11  frank
// removed png library
//
// Revision 1.2  2005/08/05 13:01:31  geconomos
// moved from vx repository
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.10  2003/09/05 20:22:16  mkontak
// change for vx::Date
//
// Revision 3.9  2003/09/04 17:29:43  michael
// merged the branch "XP1_1" back into the trunc
//
// Revision 3.8.2.1  2003/08/11 11:07:22  mkontak
// moved Date into vx namespace
//
// Revision 3.8  2003/05/16 11:54:10  michael
// code review
//
// Revision 3.7  2002/03/08 22:35:35  michael
// include cleanup
//
// Revision 3.6  2002/02/20 01:42:23  michael
// constructor, one parameter per line
//
// Revision 3.5  2002/01/12 04:19:19  frank
// Disabled time_varying datasets for now.
//
// Revision 3.4  2002/01/11 04:55:24  frank
// Added TimeVolume support.
//
// Revision 3.3  2002/01/08 22:16:25  soeren
// added default value for in constructor
//
// Revision 3.2  2001/12/24 16:46:19  ingmar
// added class Sec_30_volume to structure the doxygen documentation
//
// Revision 3.1  2001/12/12 20:19:05  jmeade
// Template "interface" for string members of header map.
//
// Revision 3.0  2001/10/14 23:02:21  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:47:50   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:35:08   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:12:21  soeren
// renamed volume  directory
//
// Revision 1.65  2001/08/30 23:30:10  pagron
// named Resize method on volumes more appropriately as Resample and added a Resize func that does a dummy resize
//
// Revision 1.64  2001/08/30 15:31:52  soeren
// added new enum NO_RESAMPLE
//
// Revision 1.63  2001/08/28 23:56:37  ingmar
// Sec_volume -> Sec_8_volume
//
// Revision 1.62  2001/06/25 21:23:56  geconomos
// moved from 51_applicationModel
//
// Revision 1.61  2001/06/14 23:53:41  liwei
// Bookkeeping the information of memory layout and datatype
//
// Revision 1.60  2001/04/19 20:59:05  dave
// updated variable names
//
// Revision 1.59  2001/04/16 12:53:04  frank
// Added permanent volume coordinate conversion matrix.
//
// Revision 1.58  2001/04/09 22:25:44  dave
// changing from Abort to use exceptions
//
// Revision 1.57  2001/04/09 16:25:35  dave
// removed some uses of uint, uchar, etc
//
// Revision 1.56  2001/04/08 22:04:17  ingmar
// prepended Sec_ to documentation section dummy classes
//
// Revision 1.55  2001/04/08 21:38:22  dave
// updated doc headers
//
// Revision 1.54  2001/04/05 18:00:47  dmitry
// updated datatypes
//
// Revision 1.53  2001/04/05 03:41:35  ingmar
// added documentation classes
//
// Revision 1.52  2001/04/03 05:46:59  soeren
// added documentation
//
// Revision 1.51  2001/03/31 19:06:44  ingmar
// test id instead of revision and date
//
// Revision 1.50  2001/03/31 19:01:34  ingmar
// added revision and date to comment that will show up at top of doxygen file
//
// Revision 1.49  2001/03/31 18:44:37  ingmar
// added revision and date to comment that will show up at top of doxygen file
//
// Revision 1.48  2001/03/31 18:38:29  ingmar
// added sample \todo and \bug comment
//
// Revision 1.47  2001/03/31 18:24:50  ingmar
// improved documentation and fixed addition of .SLC extension
//
// Revision 1.46  2001/03/31 05:34:37  ingmar
// uint8 -> uint4
//
// Revision 1.45  2001/03/30 19:07:22  soeren
// Added new Header extension "slc"
//
// Revision 1.44  2001/03/30 18:50:03  dmitry
// Updated datatypes.
//
// Revision 1.43  2001/03/23 00:47:33  dave
// towards a more Viatronix standards compliant version
//
// Revision 1.42  2000/12/12 15:55:50  dave
// removed SetBackground()
//
// Revision 1.41  2000/12/11 19:55:08  dave
// new libpng, exception, file versions etc
//
// Revision 1.40  2000/11/20 22:36:12  dave
// GetHdr changed back
//
// Revision 1.39  2000/11/20 17:04:23  jeff
// GetHdrVar() returns entire string if a string is requested (i.e. if typeid(T) == string)
//
// Revision 1.38  2000/11/19 02:44:50  dave
// corrected gcc error
//
// Revision 1.37  2000/11/18 23:33:25  dave
// corrected GetHdrVar() for type std::string
//
// Revision 1.36  2000/11/17 01:00:22  dave
// GetHdrVar returns bool
//
// Revision 1.35  2000/11/15 05:14:05  dave
// revision of Date class
//
// Revision 1.34  2000/11/13 17:19:08  dongqing
// no message
//
// Revision 1.33  2000/11/11 00:33:11  jeff
// GetHdrVar() - string variables should expect entire string returned
//
// Revision 1.32  2000/11/07 04:07:37  dave
// added SetHdrMap()
//
// Revision 1.31  2000/11/05 19:51:23  dave
// reads correctly again
//
// Revision 1.30  2000/11/03 17:02:26  dave
// converted back
//
// Revision 1.29  2000/11/03 15:50:12  ingmar
// GetNumberOfVoxels now returns uint
//
// Revision 1.28  2000/11/03 03:14:07  ingmar
// fixed some gcc syntax errors and warnings
//
// Revision 1.27  2000/11/02 19:48:09  dave
// Dimensions changed to Triple<short> from uint
//
// Revision 1.26  2000/11/02 16:48:50  sarang
// Removed errors (thanks to gcc!)
//
// Revision 1.25  2000/11/02 04:30:37  dave
// Removed desc from GetHdrVar()
//
// Revision 1.24  2000/11/02 04:17:49  dave
// misc updates
//
// Revision 1.23  2000/11/01 21:22:52  dave
// LZO default, GetDataType() fully removed, re-order Vol. declaration
//
// Revision 1.21  2000/10/31 17:16:02  dave
// template<bool> corrected, LZO kinda added
//
// Revision 1.20  2000/10/25 01:35:25  ingmar
// removed VxIterator and improved VxVolIterator and RegGrow6Iterator
//
// Revision 1.19  2000/10/24 02:56:46  ingmar
// removed VxIterator and added the functionality to the VolIterator
//
// Revision 1.18  2000/10/22 23:48:21  dave
// added RecBlockVolume
//
// Revision 1.17  2000/10/22 03:02:44  dave
// added LinVol GetDataPtr(), and background in all constructors
//
// Revision 1.16  2000/10/22 00:24:23  dave
// better iterators
//
// Revision 1.15  2000/10/21 06:06:03  ingmar
// merged mixkit branch back into main trunk
//
// Revision 1.14  2000/10/18 02:59:49  antonio
// fixed minor problems in skeleton stage
//
// Revision 1.13  2000/10/16 08:29:18  antonio
// resolved most warnings
//
// Revision 1.12.6.1  2000/10/18 01:50:29  binli
// added majority selection for resizeing -Ingmar
//
// Revision 1.12  2000/10/04 03:13:23  dave
// fixed gcc compilation
//
// Revision 1.11  2000/10/03 21:38:08  dave
// added HeaderVolume
//
// Revision 1.10  2000/09/26 01:17:49  kevin
// Result of StaticLibraries branch Merge
//
// Revision 1.9.2.1  2000/09/25 18:56:37  kevin
// Made Static Libraries
//
// Revision 1.9  2000/09/21 06:37:37  dave
// fixed specific header in volume conversion
//
// Revision 1.8  2000/09/21 06:01:22  dave
// replaced map with vector
//
// Revision 1.7  2000/09/21 03:35:51  dave
// searching for segfaults
//
// Revision 1.6  2000/09/19 15:27:36  michael
// now header variable values are propperly terminated
//
// Revision 1.5  2000/09/17 02:49:24  dave
// removed SpecificHeader*, as now replaced by reneric map
//
// Revision 1.4  2000/09/17 02:38:38  dave
// removed SpecificHeader*, as now replaced by reneric map
//
// Revision 1.3  2000/09/17 02:16:54  dave
// removed SpecificHeader*, as now replaced by reneric map
//
// Revision 1.2  2000/09/11 22:46:09  dave
// added date field to constructors
//
// Revision 1.1.1.1  2000/08/11 22:15:52  ingmar
// unit tests complete
//
//
// 38    9/09/00 9:32a Ingmar
// removed obsolete SpecificHeaderVol and VxTypedef
//
// 37    9/05/00 9:24p Dave
// many walkthrough changes in documentation, functionality
//
// 36    9/04/00 1:39a Dave
// checking in for cvs change
//
// 35    9/01/00 5:56p Dave
// initial code walkthrough changes
//
// 34    8/31/00 1:08p Dave
// added SetFileName
//
// 33    8/30/00 2:42p Dave
// added SetUnits()
//
// 32    8/21/00 12:46p Sarang
// added forward declarations for Linear, Hash and Rle volue.. wonder how
// VC++ compiled without them!
//
// 31    8/21/00 1:53a Sarang
// updated header
//
// *****************  Version 30  *****************
// User: Sarang       Date: 8/20/00    Time: 6:49p
// Updated in $/v2k/src/Volume
// The whole pipeline now works under linux+gcc. Removed tons of bugs and
// syntax errors found by gcc.
//
// *****************  Version 29  *****************
// User: Ingmar       Date: 8/19/00    Time: 3:16p
// Updated in $/v2k/src/Volume
// GetVoxel/GetVoxelRaw + inline Error::DebugMsg
//
// *****************  Version 28  *****************
// User: Jay          Date: 8/17/00    Time: 10:12p
// Updated in $/v2k/src/Volume
//
// *****************  Version 27  *****************
// User: Jay          Date: 8/17/00    Time: 5:16a
// Updated in $/v2k/src/Volume
// CopySpecificHeader added in VolAPI.
//
// *****************  Version 26  *****************
// User: Jay          Date: 8/17/00    Time: 2:27a
// Updated in $/v2k/src/Volume
// Added header for Creation Date
//
// *****************  Version 25  *****************
// User: Jay          Date: 8/16/00    Time: 1:35p
// Updated in $/v2k/src/Volume
// Overloaded constructor in Volume to enable creating HashVolumes with
// various hash bits
//
// *****************  Version 24  *****************
// User: Antonio      Date: 8/11/00    Time: 1:34p
// Updated in $/vx/src/Volume
// added header and history to end of file.
//
// *****************  Version 23  *****************
// User: Jay             Date:  8/11/00  Time:  1:35a
// Checked in $/v2k/src/Volume
// Comment:
//
//
// *****************  Version 22  *****************
// User: Jay             Date:  8/08/00  Time: 10:45a
// Checked in $/vx/src/Volume
// Comment:
//   got old volume library back into working state
//
// *****************  Version 21  *****************
// User: Jay             Date:  8/08/00  Time:  1:39a
// Checked in $/vx/src/Volume
// Comment:
//   Before changing the implementation of Volume to that using
// std::vector
//
// *****************  Version 20  *****************
// User: Jay             Date:  8/08/00  Time: 12:00a
// Checked in $/vx/src/Volume
// Comment:
//   Cleaned up
//
// *****************  Version 19  *****************
// User: Jay             Date:  8/07/00  Time:  9:34p
// Checked in $/vx/src/Volume
// Comment:
//   Made a template version of volume
//
// *****************  Version 18  *****************
// User: Jay             Date:  7/31/00  Time:  1:27p
// Checked in $/vx/src/Volume
// Comment:
//   Added the transformation from Coronal to Saggittal view.
// If time permits one could add the other transformations too. Also instead of
// performing Matrix multiplications (9 multiplications) a 3 additions are done as
// we know that the transformation matrix has most elements zero in it.
//
// *****************  Version 17  *****************
// User: Jay             Date:  7/31/00  Time: 11:57a
// Checked in $/vx/src/Volume
// Comment:
//   Was not updating the dimension when doing a Coronal transform ..
// removed the bug, Transform (Transverse to Coronal/Saggittal) works fine
//
// *****************  Version 16  *****************
// User: Jay             Date:  7/29/00  Time:  3:16p
// Checked in $/vx/src/Volume
// Comment:
//   Added a template function RayCast to build the icon
// Moved  it in the VolAPI class
//
// *****************  Version 15  *****************
// User: Jay             Date:  7/29/00  Time:  3:26a
// Checked in $/vx/src/Volume
// Comment:
//   Added TransformCoordinates() and a template fn ApplyTransform() to
// do the same. Tried to add it in VolAPI but the different implementations of
// their data representation suggest non template implementation. Cant create
// instatnces of types passed as template parameters other than the default
// constructors and the copy constructors. Both donot work here as the dimension of
// the volumes would change here due to the transformation.
//
// *****************  Version 14  *****************
// User: Jay             Date:  7/29/00  Time: 12:33a
// Checked in $/vx/src/Volume
// Comment:
//   Added enums for View :
// CORONAL SAGGITTAL TRANSVERSE
//
// *****************  Version 13  *****************
// User: Jay             Date:  7/27/00  Time:  2:15p
// Checked in $/vx/src/Volume
// Comment:
//   Included Math.h needed by Volume classes for floor funcions
//
// *****************  Version 12  *****************
// User: Dave            Date:  7/27/00  Time:  1:17p
// Checked in $/v2k/src/Volume
// Comment:
//   Constructor takes string argument
//
// *****************  Version 11  *****************
// User: Dave            Date:  7/26/00  Time:  4:00p
// Checked in $/v2k/src/Volume
// Comment:
//   Made it pure virtual, update VerifyHeader
//
// *****************  Version 10  *****************
// User: Frank           Date:  7/26/00  Time: 11:51a
// Checked in $/Viatron1000/src/Volume
// Comment:
//   Made access functions constant
//
// *****************  Version 9  *****************
// User: Jay             Date:  7/25/00  Time:  3:33p
// Checked in $/vx/src/Volume
// Comment:
//   Added the GetFileName functions.
// Tested it, used string functions substr, and find_last_of
//
// *****************  Version 8  *****************
// User: Jay             Date:  7/25/00  Time:  3:01p
// Checked in $/vx/src/Volume
// Comment:
//   Added  GetFileName and GetFileNameRoot
//
// *****************  Version 7  *****************
// User: Frank           Date:  7/25/00  Time:  3:05p
// Checked in $/Viatron1000/src/Volume
// Comment:
//   Added coordinate space conversion matrices
//
// *****************  Version 6  *****************
// User: Jay             Date:  7/25/00  Time: 11:50a
// Checked in $/Viatron1000/src/Volume
// Comment:
//   Added enum for Resampling: POINT, TRILIN, BOX, TENT, GAUSS
//
// *****************  Version 5  *****************
// User: Jay             Date:  7/21/00  Time:  7:33p
// Checked in $/Viatron1000/src/Volume
// Comment:
//   RleVolume assignment operator added
//
// *****************  Version 4  *****************
// User: Jay             Date:  7/20/00  Time:  1:22p
// Checked in $/vx/src/Volume
// Comment:
//   Added Verify() to verify acceptable enums ..
//
// *****************  Version 3  *****************
// User: Jay             Date:  7/19/00  Time:  2:15p
// Checked in $/vx/src/Volume
// Comment:
//   Synchronized with CVS
//
// *****************  Version 2  *****************
// User: Frank           Date:  7/18/00  Time:  3:57p
// Checked in $/Viatron1000/src/Volume
// Comment:
//   Removed "using namespace *" from header files to prevent ambiguities
//
// *****************  Version 1  *****************
// User: Ingmar          Date:  7/18/00  Time:  9:03a
// Created VolAPI.h
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/VolAPI.h,v 1.3 2006/02/03 15:22:11 frank Exp $
// $Id: VolAPI.h,v 1.3 2006/02/03 15:22:11 frank Exp $
