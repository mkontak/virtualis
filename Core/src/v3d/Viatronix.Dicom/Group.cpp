#include "Stdafx.h"
#include "Group.h"

USING_DICOM_NS


/**
 * Constructors
 *
 * @param iId             Group id
 * @param sDescription    Group description
 */
Group::Group(unsigned short iId, String ^ msDescription) :
m_pGroup(new dcm::DcmGroup(iId, ss(msDescription)))
{
}

Group::~Group()
{
  this->!Group();
}

Group::!Group()
{
  delete m_pGroup;
}

String ^ Group::FormattedId::get()
{
  return gcnew String(m_pGroup->GetFormattedId().c_str());
}

unsigned short Group::Id::get()
{
  return m_pGroup->GetId();
}

String ^ Group::Description::get()
{
  return gcnew String(m_pGroup->GetDescription().c_str());
}