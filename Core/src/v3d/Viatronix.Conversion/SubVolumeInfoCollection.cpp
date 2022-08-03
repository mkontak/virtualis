#include "Stdafx.h"
#include "SubVolumeInfo.h"
#include "SubVolumeInfoCollection.h"

using namespace System::Text;
USING_CONVERSION_NS

/**
 * Constructor for the sub-volume collection
 *
 * @param sJobId        Job id associted with this process
 * @param sTaskName     Task's name
 */
SubVolumeInfoCollection::SubVolumeInfoCollection(String ^ sJobId, String ^ sTaskName) :
m_sJobId(sJobId),
m_sTaskName(sTaskName)
{

}


/**
 * Generate the xml repesentation fofthe Sub-Volume collection
 *
 * @return Xml string
 */
String ^ SubVolumeInfoCollection::ToXml()
{

  StringBuilder ^ sXml = gcnew StringBuilder(String::Format("<pipeline task=\"{0}\" jid=\"{1}\" >", m_sTaskName, m_sJobId));

  sXml->Append("<volumes>");
  for each ( KeyValuePair<unsigned int, SubVolumeInfo ^> ^ mpPair in this )
  {
    sXml->Append(mpPair->Value->ToXml());
  }

  sXml->Append("</volumes>");
  sXml->Append("</pipeline>");

  return sXml->ToString();
} // ToXml()