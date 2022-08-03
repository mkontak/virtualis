#pragma once

#include "Typedef.h"
#include "CvnEnums.h"
#include <unordered_map>



// Namespaces
namespace cvn
{


typedef  std::unordered_map<std::string, std::string> PROPERTIES;

typedef  std::unordered_map<std::string, PROPERTIES> PROPERTIESMAP;


/**
 * Contains the sub-volume information 
 */
class CVN_DLL CvnSubVolumeInfo
{
  
public:

  // Constructor. iSubVolumeNumber is the volume index. It is not the number of sub-volume in the series.
  CvnSubVolumeInfo( uint4 iSubVolumeNumber);

  // Destructor
  ~CvnSubVolumeInfo();

  // Create the sub-volume info using the file path
  static std::shared_ptr<CvnSubVolumeInfo> Create( uint4 iSubVolumeNumber, const uint4 & iImageCount, const std::string & sFilePath);

  // Creates the sub-volume info using the volume
  static std::shared_ptr<CvnSubVolumeInfo> Create(uint4 iSubVolumeNumber, const uint4 & iImageCount, const std::shared_ptr<vxBlockVolume<uint2>> & volume);

  // Create the sub-volume info
  static std::shared_ptr<CvnSubVolumeInfo> Create( uint4 iSubVolumeNumber );

  // Generates the XML
  std::string ToXml();

  // Adds the property 
  void AddProperty(const std::string & sName, const std::string & sValue, const std::string & sTag = "properties");

  // Removes the specified property (return true if removed)
  bool RemoveProperty(const std::string & sName, const std::string & sTag = "properties");

public:

  // Get the sub-volume name
  const std::string & GetName();

  // Sets the name
  void SetName(const std::string & sName) 
  { m_sName = sName; }
   
  // Gets the image count
  const uint4 GetImageCount() const
  { return m_iImageCount; }

  // Sets the image count
  void SetImageCount(uint4 iValue) 
  { m_iImageCount = iValue; }

  // Gets the application type
  const ApplicationTypes GetApplicationType() const 
  { return m_eApplicationType; }

  // Sets the applciation type
  void SetApplicationType(ApplicationTypes eType)
  { m_eApplicationType = eType; }

  // Gets the sub-volumenumber
  const uint4 GetSubVolumeNumber() const
  { return m_iSubVolumeNumber; }
  
  // Gets the functional temporal flag
  const bool IsFunctionalTemporalVolume() const
  { return m_bIsFunctionalTemporalVolume; }

  // Sets the functional temporal flag
  void IsFunctionalTemporalVolume( bool bValue ) 
  { m_bIsFunctionalTemporalVolume = bValue; }

  // Gets the file path
  const std::string  GetFilePath() const
  { return ( m_sFilename.empty() ? m_sPath : util::IO::Combine(m_sPath, m_sFilename) ); }

  // Sets the file [ath
  const void SetFilePath(const std::string & sValue);

  // Gets the volume
  const std::shared_ptr<vxBlockVolume<uint2>> & GetVolume() const
  { return m_volume; }


  // Gets the application as a string
  const std::string GetApplication() ;

  // Gets the series UID
  const std::string & GetSeriesUid() const
  { return m_sSeriesUid; }

  // Sets the series uid
  const void SetSeriesUid(const std::string & sValue)
  { m_sSeriesUid = sValue; }

  // Sets the sub volume description
  const void SetDescription(const std::string & sDescription)
  { m_sDescription = sDescription; }

  // Gets the sub volume description
  const std::string & GetDescription() const
  { return m_sDescription; }

  // Gets the application subtype
  const std::string & GetSubType() const
  { return m_sSubType; }

  // Set the application subtype
  void SetSubType(const std::string & sSubType)
  { m_sSubType = sSubType; }


  // Gets the properties for this sub-volume
  const PROPERTIES & GetProperties(const std::string & sTag = "properties")
  {
    static  PROPERTIES blank;

    if ( m_properties.find(sTag) == m_properties.end() )
      return m_properties[sTag];
    else
      return blank;
  }

  // Gets the properties for this sub-volume
  const PROPERTIESMAP & GetAllProperties()
  {
    return m_properties;
  }

  /// Gets the conversion id
  static const std::string & GetConversionId() 
  { 
    if ( m_sConversionId.empty() )
    {

      SYSTEMTIME sys;
      ::GetSystemTime(&sys);
      m_sConversionId = util::Strings::Format("%04d%02d%02d%d",sys.wYear,sys.wMonth,sys.wDay, GetTickCount());

    }

    return m_sConversionId; 
  }

private:


  // Properties associated with the sub-type (Tag -> (Name/Value) pairs)
  PROPERTIESMAP m_properties;

  // Application sub-type
  std::string m_sSubType;

  // Dicom series uid
  std::string m_sSeriesUid;

  /// Name of the sub-volume
  std::string m_sName;

  /// Image count
  uint4 m_iImageCount;

  /// the sub-volume index number
	uint4 m_iSubVolumeNumber;

  /// application type, 
	ApplicationTypes m_eApplicationType;

  /// flag for functional temporal volume
	bool m_bIsFunctionalTemporalVolume;

  /// path the volume is written to
  std::string m_sPath;

  /// Filename
  std::string m_sFilename;

  /// Memory volume if not writtn
  std::shared_ptr<vxBlockVolume<uint2>> m_volume;

  /// Description for the sub-volume
  std::string m_sDescription;

  /// Conversion Id
  static std::string m_sConversionId;

};  // CvnSubVolumeInfo


} // namespace cvn