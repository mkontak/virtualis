#include "Stdafx.h"
#include "SubVolumeInfo.h"


USING_CONVERSION_NS
using namespace System::Collections::Generic;




SubVolumeInfo::SubVolumeInfo() :
m_pSubVolumeInfo(scoped_ptr<std::shared_ptr<cvn::CvnSubVolumeInfo>>(__nullptr) ),
m_properties(gcnew  Dictionary<String ^,Dictionary<String ^, String^> ^>())
{
}


/**
 * Finalizer for SubVolumeInfo object
 */
SubVolumeInfo::!SubVolumeInfo()
{
  m_pSubVolumeInfo.reset(__nullptr);
}

/**
 * Creates the sub volume information object from the unmanaged version
 *
 * @param subVolumeInfomationPtr        Pointer to shared_ptr
 *
 * @return SubVOlumeInformation object
 */
SubVolumeInfo ^ SubVolumeInfo::Create(IntPtr subVolumeInformationPtr)
{
  SubVolumeInfo ^ mpInfo(gcnew SubVolumeInfo());

  if ( mpInfo == nullptr )
    throw gcnew OutOfMemoryException("Failed to allocate SubVolumeInfo object");

  mpInfo->m_pSubVolumeInfo.reset((std::shared_ptr<cvn::CvnSubVolumeInfo> *)(subVolumeInformationPtr.ToPointer()));

  return mpInfo;


} // Create(IntPtr subVolumeInformationPtr)


/**
 * Returns the xml repsentation of the object
 */
String ^ SubVolumeInfo::ToXml()
{
  return gcnew String((*m_pSubVolumeInfo)->ToXml().c_str());
} // ToXml()

/**
 * Gets the image count
 */
unsigned int SubVolumeInfo::ImageCount::get()
{
  return (*m_pSubVolumeInfo)->GetImageCount();
} // ImageCount

/**
 * Gets the sub volume number
 */
unsigned int SubVolumeInfo::Number::get()
{
  return (*m_pSubVolumeInfo)->GetSubVolumeNumber();
} // Number

/**
 * Gets the file path
 */
String ^ SubVolumeInfo::FilePath::get()
{
  return gcnew String((*m_pSubVolumeInfo)->GetFilePath().c_str());
} // FilePath

/**
 * Gets the file path
 */
String ^ SubVolumeInfo::Application::get()
{
  return gcnew String((*m_pSubVolumeInfo)->GetApplication().c_str());
} // Application

/**
 * Gets the file path
 */
bool SubVolumeInfo::IsFunctionalTemporalVolume::get()
{
  return (*m_pSubVolumeInfo)->IsFunctionalTemporalVolume();
} // IsFunctionalTemporalVolume

/**
 * Gets the application sub type
 */
String ^ SubVolumeInfo::SubType::get()
{
  return gcnew String((*m_pSubVolumeInfo)->GetSubType().c_str());
} // SubType

/**
 * Gets the application sub type
 */
Dictionary<String ^, Dictionary<String ^, String^> ^> ^  SubVolumeInfo::Properties::get()
{

  if ( m_properties->Count == 0 && (*m_pSubVolumeInfo)->GetAllProperties().size() > 0  )
  {
   
    cvn::PROPERTIESMAP map =  (*m_pSubVolumeInfo)->GetAllProperties();

    for ( cvn::PROPERTIESMAP::iterator pm = map.begin(); pm != map.end(); pm++ )
    {
      for ( cvn::PROPERTIES::iterator pi = pm->second.begin(); pi != pm->second.end(); pi++ )
      {
        String ^ sTag(gcnew String(pi->first.c_str()));
        if ( m_properties->ContainsKey(sTag) )
          m_properties[sTag]->Add(gcnew String(pi->first.c_str()), gcnew String(pi->second.c_str()));
        else
        {
          Dictionary<String ^, String ^> ^ mpProperties = gcnew Dictionary<String ^, String ^>();
          mpProperties->Add(gcnew String(pi->first.c_str()), gcnew String(pi->second.c_str()));
          m_properties->Add(sTag, mpProperties );
        }
      }
    }
    
  }

  return m_properties;
} // SubType