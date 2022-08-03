#include "stdafx.h"
#include "CvnSubVolumeInfo.h"
#include "CvnExceptions.h"


using namespace cvn;

std::string CvnSubVolumeInfo::m_sConversionId("");


/** 
 *  default constructor
 *
 *  @param iSubVolumeNumber: the subvolume index number
 */
CvnSubVolumeInfo::CvnSubVolumeInfo( uint4 iSubVolumeNumber) :
m_iSubVolumeNumber(iSubVolumeNumber),
m_volume(std::shared_ptr<vxBlockVolume<uint2>>(__nullptr)),
m_eApplicationType(ApplicationTypes::VC),
m_bIsFunctionalTemporalVolume(false),
m_iImageCount(0),
m_sSeriesUid(""),
m_sDescription(""),
m_sPath(""),
m_sFilename("")
{
} // CvnSubVolumeInfo


/**
 *  default destructor
 */
CvnSubVolumeInfo::~CvnSubVolumeInfo()
{
  m_volume = __nullptr;
} // ~CvnSubVolumeInfo()


/**
 * Creates the sub volume information object
 *
 * @param sConversionId           Conversion id
 * @param iSubVolumeNumber        Subvolume number
 *
 * @return shared pointer to a SubVOlumeInformation object
 */
std::shared_ptr<CvnSubVolumeInfo> CvnSubVolumeInfo::Create( uint4 iSubVolumeNumber)
{

  std::shared_ptr<CvnSubVolumeInfo> info = std::shared_ptr<CvnSubVolumeInfo>(std::make_shared<CvnSubVolumeInfo>( iSubVolumeNumber));

  if ( info == __nullptr )
    throw CvnOutOfMemoryException(LogRec("Failed to allocate CvnSubVolumeInfo","CvnSubVolumeInfo","Create"));

  return info;
} // CvnSubVolumeInfo::Create(uint4 iSubVolumeNumber)


/**
 * Creates the sub volume information object
 *
 * @param iSubVolumeNumber        Subvolume number
 * @param sFilePath               Volume file path
 *
 * @return shared pointer to a SubVolumeInformation object
 */
std::shared_ptr<CvnSubVolumeInfo> CvnSubVolumeInfo::Create( uint4 iSubVolumeNumber, const uint4 & iImageCount, const std::string & sFilePath)
{

  std::shared_ptr<CvnSubVolumeInfo> info = Create( iSubVolumeNumber);

  info->m_iImageCount = iImageCount;
  info->SetFilePath( sFilePath );

  return info;
} // CvnSubVolumeInfo::Create(uint4 iSubVolumeNumber, const std::string & sFilePath)


/**
 * Creates the sub volume information object
 *
 * @param iSubVolumeNumber        Subvolume number
 * @param volume                  block volume
 *
 * @return shared pointer to a SubVOlumeInformation object
 */
std::shared_ptr<CvnSubVolumeInfo> CvnSubVolumeInfo::Create( uint4 iSubVolumeNumber, const uint4 & iImageCount, const std::shared_ptr<vxBlockVolume<uint2>> & volume)
{

  std::shared_ptr<CvnSubVolumeInfo> info = Create( iSubVolumeNumber);

  info->m_iImageCount = iImageCount;
  info->m_volume = volume;

  return info;
} // CvnSubVolumeInfo::Create(uint4 iSubVolumeNumber, const std::shared_ptr<vxBlockVolume<uint2>> & volume)


/**
 * Returns the xml representation of the SubVolumeInfo object
 */
std::string CvnSubVolumeInfo::ToXml()
{
  std::string sXml("<volume ");

  sXml.append(util::Strings::Format("id=\"%s\" ", m_sConversionId.c_str()));
  sXml.append(util::Strings::Format("name=\"%s\" ", GetName().c_str()));
  sXml.append(util::Strings::Format("app=\"%s\" ",  GetApplication().c_str()));
  sXml.append(util::Strings::Format("sub=\"%s\" ",  GetSubType().c_str()));
  sXml.append(util::Strings::Format("icnt=\"%d\" ", GetImageCount()));
  sXml.append(util::Strings::Format("uid=\"%s\" ",  m_sSeriesUid.c_str()));
  sXml.append(util::Strings::Format("path=\"%s\" ", m_sPath.c_str()));
  sXml.append(util::Strings::Format("file=\"%s\" ", m_sFilename.c_str()));
  sXml.append(util::Strings::Format("desc=\"%s\" ", m_sDescription.c_str()));
	sXml.append(util::Strings::Format("temporal=\"%s\" ", (m_bIsFunctionalTemporalVolume ? "yes" : "no") ));

  if ( m_properties.size() > 0 )
  {
    sXml.append(">");

    sXml.append("<ext>");
    
    for ( PROPERTIESMAP::iterator pm = m_properties.begin(); pm != m_properties.end(); pm++ )
    {
      if ( pm->second.size() > 0 )
      {
        sXml.append(util::Strings::Format("<%s ", pm->first.c_str()));
      
        for ( PROPERTIES::iterator pi = pm->second.begin(); pi != pm->second.end(); pi++ )
        {
          sXml.append(util::Strings::Format(" %s=\"%s\"",pi->first.c_str(), pi->second.c_str()));
        }

        sXml.append(" />");
      } // if
    } // for

    sXml.append("</ext>");
    sXml.append("</volume>");

  } // if
  else
    sXml.append("/>");


  return sXml;

} // ToXml()


/**
 * Adds property if it does not exists or modifies the value if it does.  
 * Allows the having multiple property tags with the default being "properties".
 *
 * Example:
 *
 * sTag not supplied will default it to "properties"
 *
 * Only a single entry 
 *
 * <ext>
 *   <properties  att1="value_1" att2="value_2"/>             Default if no sTag is specified then all entries are added to "properties"
 * </ext>
 *
 *  sTag = "tag1" and sTag = "tag2"
 *
 * Two entries in <etx> will appear       
 *
 * <ext>
 *   <tag1 att1="value_1" att2="value_2"/>                    
 *   <tag2 att1="value_1" att2="value_2"/>
 * </ext>
 *  
 *
 * @param sName   property name
 * @param sValue  property value
 * @param sTag    property tag (default is properties)
 *
 */
void CvnSubVolumeInfo::AddProperty(const std::string & sName, const std::string & sValue, const std::string & sTag)
{ 
  if ( m_properties.find(sTag) != m_properties.end() )
  {
    if ( m_properties[sTag].find(sName) != m_properties[sTag].end() )
      m_properties[sTag].insert(std::pair<std::string,std::string>(sName, sValue));
    else
      m_properties[sTag][sName] = sValue;
  }
  else
  {
    PROPERTIES properties;
    properties.insert(std::pair<std::string, std::string>(sName,sValue));
    m_properties.insert(std::pair<std::string,PROPERTIES>(sTag, properties));
  }
} // AddProperty(const std::string & sName, const std::string & sValue)


/**
 * Removes the sepcified attribute
 *
 * @param sName   property name
 * @param sTag    property tag (defailt is properties)
 */
bool CvnSubVolumeInfo::RemoveProperty(const std::string & sName, const std::string & sTag )
{ 
  bool removed(false);

  if ( m_properties.find(sTag) != m_properties.end() )
  {
    if ( m_properties[sTag].find(sName) != m_properties[sTag].end() )
    {
      m_properties[sTag].erase(sName); 
      removed = true;
    }
  } // if

  return removed;
} // RemoveProperty(const std::string & sName)


/**
 * Sets the file path
 * 
 * @param sFilePath     File path
 */
const void CvnSubVolumeInfo::SetFilePath(const std::string & sFilePath)
{
  STRINGLIST pathParts = util::IO::SplitPath(sFilePath);


  /// Path
  m_sPath = util::Strings::Format("%s%s",pathParts[util::IO::DRIVE].c_str(),pathParts[util::IO::DIRECTORY].c_str());

  /// File name with extension
  m_sFilename = util::Strings::Format("%s%s",pathParts[util::IO::FILE].c_str(),pathParts[util::IO::EXTENSION].c_str());

} // SetFilePath(const std::string & sValue)


/**
 * gets the name of the volume
 */
const std::string & CvnSubVolumeInfo::GetName()
{
  if ( m_sName.empty() )
  {
    m_sName = util::Strings::Format("volume%03d", m_iSubVolumeNumber);
    //if ( ! m_sFilename.empty() )
    //{
    //  STRINGLIST parts = util::Strings::Split(m_sFilename,'.');

    //  // Set the name to the file name
    //  m_sName = parts[0];
    //}
  } // if

  return m_sName;

} // GetName()


/**
 * Gets the application type as a string
 */
const std::string CvnSubVolumeInfo::GetApplication() 
{

  std::string sApplication("");

  switch ( m_eApplicationType )
  {
  case VC:
    sApplication.append("VC");
    break;
  case CAR:
    sApplication.append("CAR");
    break;
  case XP:
    sApplication.append("XP");
    break;
  case VAS:
    sApplication.append("VAS");
    break;
  case XA:
    sApplication.append("XA");
    break;
  case CS:
    sApplication.append("CS");
    break;
  case PTX:
    sApplication.append("PTX");
    break;
  default:
    break;

  };

  return sApplication;
} // GetApplication()