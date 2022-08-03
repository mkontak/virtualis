using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Viatronix.Imapi2.Interop;
using FileSystemInterop = Viatronix.Imapi2.FileSystem.Interop;

namespace Viatronix.Imapi
{

  #region MediaTypes enum

  /// <summary>
  /// The various media types.
  /// </summary>
  public enum MediaTypes : int
  {
    BDR             = IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_BDR,
    BDRE            = IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_BDRE,
    BDROM           = IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_BDROM,
    CDR             = IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_CDR,
    CDROM           = IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_CDROM,
    CDRW            = IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_CDRW,
    DISK            = IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_DISK,
    DVD_DASH_R      = IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_DVDDASHR,
    DVD_DASH_R_DL   = IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_DVDDASHR_DUALLAYER,
    DVD_DASH_RW     = IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_DVDDASHRW,
    DVD_PLUS_R      = IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_DVDPLUSR,
    DVD_PLUS_R_DL   = IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_DVDPLUSR_DUALLAYER,
    DVD_PLUS_RW     = IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_DVDPLUSRW,
    DVD_PLUS_RW_DL  = IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_DVDPLUSRW_DUALLAYER,
    DVD_RAM         = IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_DVDRAM,
    DVD_ROM         = IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_DVDROM,
    HD_DVD_R        = IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_HDDVDR,
    HD_DVD_RAM      = IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_HDDVDRAM,
    HD_DVD_ROM      = IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_HDDVDROM,
    MAX             = IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_MAX,
    UNKNOWN         = IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_UNKNOWN,
  } // enum MediaTypes

  #endregion

  #region FileSystemTypes enum

  /// <summary>
  /// Enumeration of various file system that relate to optical media.
  /// </summary>
  public enum FileSystemTypes : int
  {
    None = FileSystemInterop.FsiFileSystems.FsiFileSystemNone,
    ISO9660 = FileSystemInterop.FsiFileSystems.FsiFileSystemISO9660,
    Joliet = FileSystemInterop.FsiFileSystems.FsiFileSystemJoliet,
    UDF = FileSystemInterop.FsiFileSystems.FsiFileSystemUDF,
    Unknown = FileSystemInterop.FsiFileSystems.FsiFileSystemUnknown,
  } // enum FileSystemTypes

  #endregion

  #region VerificationLevel enum

  /// <summary>
  /// The verification level.
  /// </summary>
  public enum VerificationLevel : int
  {
    None = IMAPI_BURN_VERIFICATION_LEVEL.IMAPI_BURN_VERIFICATION_NONE,
    Quick = IMAPI_BURN_VERIFICATION_LEVEL.IMAPI_BURN_VERIFICATION_QUICK,
    Full = IMAPI_BURN_VERIFICATION_LEVEL.IMAPI_BURN_VERIFICATION_FULL
  } // enum VerificationLevel

  #endregion


  /// <summary>
  /// A class to help with various enum utility functions.
  /// </summary>
  public class EnumUtils
  {

    #region methods

    /// <summary>
    /// Converts an incoming type into the corresponding enum value.
    /// </summary>
    /// <param name="type"></param>
    /// <returns></returns>
    public static IMAPI_MEDIA_PHYSICAL_TYPE ConvertIntToMediaPhysicalType(int type)
    {
      switch (type)
      {
        case (int)IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_BDR:
          return IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_BDR;
        case (int)IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_BDRE:
          return IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_BDRE;
        case (int)IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_BDROM:
          return IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_BDROM;
        case (int)IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_CDR:
          return IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_CDR;
        case (int)IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_CDROM:
          return IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_CDROM;
        case (int)IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_CDRW:
          return IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_CDRW;
        case (int)IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_DVDDASHR:
          return IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_DVDDASHR;
        case (int)IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_DVDDASHR_DUALLAYER:
          return IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_DVDDASHR_DUALLAYER;
        case (int)IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_DVDDASHRW:
          return IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_DVDDASHRW;
        case (int)IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_DVDPLUSR:
          return IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_DVDPLUSR;
        case (int)IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_DVDPLUSR_DUALLAYER:
          return IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_DVDPLUSR_DUALLAYER;
        case (int)IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_DVDPLUSRW:
          return IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_DVDPLUSRW;
        case (int)IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_DVDPLUSRW_DUALLAYER:
          return IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_DVDPLUSRW_DUALLAYER;
        case (int)IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_DVDRAM:
          return IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_DVDRAM;
        case (int)IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_DVDROM:
          return IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_DVDROM;
        case (int)IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_HDDVDR:
          return IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_HDDVDR;
        case (int)IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_HDDVDRAM:
          return IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_HDDVDRAM;
        case (int)IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_HDDVDROM:
          return IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_HDDVDROM;
        default:
          return IMAPI_MEDIA_PHYSICAL_TYPE.IMAPI_MEDIA_TYPE_UNKNOWN;
      }
    } // ConvertIntToMediaPhysicalType( type )


    /// <summary>
    /// Converts the media type to a short string.
    /// </summary>
    /// <param name="type"></param>
    /// <returns></returns>
    public static string ConvertMediaPhysicalTypeToShortString(MediaTypes type)
    {
      switch (type)
      {
        case MediaTypes.BDR:
          return "BD-R";
        case MediaTypes.BDRE:
          return "BD-RE";
        case MediaTypes.BDROM:
          return "BD-ROM";
        case MediaTypes.CDR:
          return "CD-R";
        case MediaTypes.CDRW:
          return "CD-RW";
        case MediaTypes.CDROM:
          return "CD ROM";
        case MediaTypes.DVD_DASH_R:
          return "DVD-R";
        case MediaTypes.DVD_DASH_R_DL:
          return "DVD-R DL";
        case MediaTypes.DVD_DASH_RW:
          return "DVD-RW";
        case MediaTypes.DVD_PLUS_R:
          return "DVD+R";
        case MediaTypes.DVD_PLUS_R_DL:
          return "DVD+R DL";
        case MediaTypes.DVD_PLUS_RW:
          return "DVD+RW";
        case MediaTypes.DVD_PLUS_RW_DL:
          return "DVD+RW DL";
        case MediaTypes.DVD_RAM:
          return "DVD-RAM";
        case MediaTypes.DVD_ROM:
          return "DVD-ROM";
        case MediaTypes.HD_DVD_R:
          return "HDDVD-R";
        case MediaTypes.HD_DVD_RAM:
          return "HDDVD-RAM";
        case MediaTypes.HD_DVD_ROM:
          return "HDDVD-ROM";
        default:
          return null;
      }
    } // ConvertMediaPhysicalTypeToString( type )

    #endregion

  } // class EnumUtils
} // namespace Viatronix.Imapi
