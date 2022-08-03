//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] (mkontak@viatronix.com)

// includes
#include "stdafx.h"
#include "DemoRegistry.h"
#include "Wmi.h"
#include "crypto\AESCrypt.h"


// namespaces
using namespace std;
using namespace sys;


#pragma region static definitions

const std::string DemoRegistry64::Attributes::InstallDate = "idt";
const std::string DemoRegistry64::Attributes::SystemId = "sid";
const std::string DemoRegistry64::Attributes::Key = "ky";
const std::string DemoRegistry64::Attributes::Days = "dy";
const std::string DemoRegistry64::Attributes::ExpirationDate = "edt";

#pragma endregion

/**
 * Constructor using the application code string
 *
 * @param sApplicationCode        Application code
 */
DemoRegistry64::DemoRegistry64(const std::string & sApplicationCode) :
m_sSystemId(Wmi::GetSystemId()),
m_sApplication(sApplicationCode),
m_days(30),
m_sKey(""),
SoftwareRegistry64((sApplicationCode.empty() ? "DVX" : util::Strings::Format("DVX\\%s", ScrambleApplication(sApplicationCode).c_str())))
{
  Initialize();
  
}

/**
 * Constructor using the application enumerator
 */
DemoRegistry64::DemoRegistry64(Applications app) :
m_sSystemId(Wmi::GetSystemId()),
m_sApplication(ToString(app)),
m_days(30),
m_sKey(""),
SoftwareRegistry64(util::Strings::Format("DVX\\%s", ScrambleApplication(ToString(app)).c_str()))
{
  Initialize();


}


/**
 * initializes the object
 **/
void DemoRegistry64::Initialize()
{


  // =================================================================================================
  // If the registry exists then initialize the internal data from the registry. otherwise set to 
  // the initial values.
  // ================================================================================================
  if (Exists())
  {

    // =====================
    // Get the system id
    // ======================
    BYTE systemId[1024];
    DWORD size(1024);
    if (!TryGetBinary(Attributes::SystemId, systemId, &size))
      throw ex::DemoInvalidException(LogRec("Invalid demo system, no system id", "DemoRegistry", "Initialize"));

    m_sSystemId.assign((char *)systemId, size);

    // =========================
    // Get the date installed
    // =========================
    size = sizeof(struct tm);
    if (!TryGetBinary(Attributes::InstallDate, (LPBYTE)&m_installation, &size))
      throw ex::DemoInvalidException(LogRec("Invalid demo system, no install date", "DemoRegistry", "Initialize"));


    // =========================
    // Get the expiration date
    // =========================
    size = sizeof(struct tm);
    if (!TryGetBinary(Attributes::ExpirationDate, (LPBYTE)&m_expiration, &size))
      throw ex::DemoInvalidException(LogRec("Invalid demo system, no expiration date", "DemoRegistry", "Initialize"));

    // =======================
    // Get the activation key
    // ======================
    BYTE key[2048];
    size = 2048;
    if (!TryGetBinary(Attributes::Key, key, &size))
      throw ex::DemoInvalidException(LogRec("Invalid demo system, no key", "DemoRegistry", "Initialize"));

    m_sKey.assign((char *)key,size);

    // ==============
    // Get the days
    // ==============
    if (!TryGetDWORD(Attributes::Days, &m_days))
      throw ex::DemoInvalidException(LogRec("Invalid demo system, no days", "DemoRegistry", "Initialize"));

  }
  else
  {

    // Get the current systems id
    m_sSystemId = Wmi::GetSystemId();

    // Get the current date and time
    time_t rawtime;
    time(&rawtime);
    
    struct tm * date = gmtime(&rawtime);

    memcpy(&m_installation, date, sizeof(struct tm));

    date->tm_mday += m_days;

    time_t later = mktime(date);

    date = gmtime(&later);


    memcpy(&m_expiration, date, sizeof(struct tm));


  }

  

}


/**
 * Activates the demo using the internal settings
 */
void DemoRegistry64::Activate()
{

  // ===================================================
  // Make sure the system id and activation key is set
  // ===================================================
  if (m_sSystemId.empty() || m_sKey.empty() )
    throw ex::DemoInvalidDataException(LogRec("Cannot activate missing data", "DemoRegistry64", "Activate"));

  try
  {

    SetBinary(Attributes::SystemId, (LPBYTE)m_sSystemId.c_str(), m_sSystemId.size());
    SetBinary(Attributes::InstallDate, (LPBYTE)&m_installation, sizeof(struct tm));
    SetBinary(Attributes::Key, (LPBYTE)m_sKey.c_str(), m_sKey.size());
    SetBinary(Attributes::ExpirationDate, (LPBYTE)&m_expiration, sizeof(struct tm));
    SetDWORD(Attributes::Days, m_days);
  }
  catch (ex::VException & ex)
  {
    throw ex::DemoRegistryException(ex.GetLogRecord());
  }
  catch (...)
  {
    throw ex::DemoRegistryException(LogRec("Failed to write to registry", "DemoRegistry64", "Activate"));
  }
} // Activate()


/**
 * Converts the application enumerator to a string value
 *
 * @param app     Application
 *
 * @return Returns the application as a string
 */
std::string DemoRegistry64::ToString(Applications app)
{

  std::string sApp("");

  switch (app)
  {
  case VC:  
    sApp.assign("VC");
    break;
  case CAR:
    sApp.assign("CAR");
    break;
  case VAS:
    sApp.assign("VAS");
    break;
  case XP:
    sApp.assign("XP");
    break;
  case FN:
    sApp.assign("FN");
    break;
  case CS:
    sApp.assign("CS");
    break;
    
  };  // END ... switch()

  return sApp;

} // ToString(Applications app)


/**
 * Determines in the demo system is expired
 *
 * @return True if the system has expired, false if not
 */
bool DemoRegistry64::IsExpired()
{
  bool expired(false);


  // Get the current date and time
  time_t rawtime;
  time(&rawtime);

  struct tm * current = gmtime(&rawtime);

  if (current->tm_year > m_expiration.tm_year || current->tm_yday > m_expiration.tm_yday)
    expired = true;

  return expired;
}


/**
 * Sets the activation key validating the contents in the process
 *
 * @param sKey  Activation key
 */
void DemoRegistry64::SetActivationKey(const std::string & sKey)
{

  std::string sActivationKey = crypto::AESCrypt::Decrypt(sKey, false);


  char * ptr = (char *)sActivationKey.c_str();

  std::string sApplication;

  sApplication.assign(ptr);

  ptr += 5;

  memcpy(&m_days,ptr, sizeof(int));

  ptr += sizeof(int);

  int size;

  memcpy(&size, ptr, sizeof(int));

  ptr += sizeof(int);

  std::string sSystemId(ptr, size);

  ptr += size;

  struct tm  dateTime;

  memcpy(&dateTime, ptr, sizeof(struct tm));

  
  if (strncmp(m_sApplication.c_str(), sApplication.c_str(), 5) != 0 )
    throw ex::DemoInvalidDataException(LogRec("Invalid activation key, invalid application", "DemoRegistry64", "SetActivationKey"));

  if ( m_sSystemId.size() != sSystemId.size() || strncmp(m_sSystemId.c_str(), sSystemId.c_str(), m_sSystemId.size()) != 0)
    throw ex::DemoInvalidDataException(LogRec("Invalid activation key, invalid system id", "DemoRegistry64", "SetActivationKey"));

  time_t rawtime;
  time(&rawtime);

  struct tm * current = gmtime(&rawtime);

  // Make sure the date is the same. The key expires at the end of the day.
  if (dateTime.tm_year > current->tm_year || dateTime.tm_yday > current->tm_yday)
    throw ex::DemoActivationKeyExpiredException(LogRec("Invalid activation key, expired", "DemoRegistry64", "SetActivationKey"));


  dateTime.tm_mday += m_days;
  time_t later = mktime(&dateTime);
  memcpy(&m_expiration, gmtime(&later), sizeof(struct tm));

  
  m_sKey = sKey;

} // SetActivationKey(const std::string & sKey)

/**
 * Generates the activation key 
 * 
 * @param sSystemId       System Id
 * @param days            Number of days to activate
 *
 */
std::string DemoRegistry64::GenerateActivationKey(const std::string & sApplication, const std::string & sSystemId, int days)
{
  
  time_t rawtime;
  time(&rawtime);

  struct tm * dateTime = gmtime(&rawtime);

  const int intSize(sizeof(int));
  const int size(sSystemId.size());
  size_t bufferSize = 5 + (intSize * 2) + size + sizeof(struct tm);
  std::shared_ptr<char> buffer = std::shared_ptr< char>(new  char[bufferSize], [](char * value) { delete[] value; });

  RtlZeroMemory(buffer.get(), bufferSize);

  char * ptr(buffer.get());

  memcpy(ptr, sApplication.c_str(), min(5, sApplication.size()));

  ptr += 5;

  memcpy(ptr, (char *)&days, intSize);

  ptr += intSize;


  memcpy(ptr, (char *)&size, intSize);

  ptr += intSize;

  memcpy(ptr, sSystemId.c_str(), sSystemId.size());

  ptr += size;

  memcpy(ptr, (char *)dateTime, sizeof(struct tm));

  std::string sCombined(buffer.get(), bufferSize);

  return crypto::AESCrypt::Encrypt(sCombined, false);

} // GenerateActivationKey(const std::string & sCustomerId, const std::string & sSystemId, int days)

/**
 * Scrambles the application code
 */
std::string DemoRegistry64::ScrambleApplication(const std::string & sApplication)
{

  std::string sScrambledCode;

  std::for_each(sApplication.begin(), sApplication.end(), [&sScrambledCode](CHAR ch)
  {
    int uch = (122 - (toupper(ch) - 65));

    sScrambledCode.push_back((CHAR)uch);

  });


  return sScrambledCode;
}

/**
 * Scrambles the application code
 */
std::string DemoRegistry64::UnScrambleApplication(const std::string & sScrambledCode)
{

  std::string sApplication;

  std::for_each(sScrambledCode.begin(), sScrambledCode.end(), [&sApplication](CHAR ch)
  {
    int uch = (65 + (122 - (int)(ch)));

    sApplication.push_back((CHAR)uch);

  });


  return sApplication;
}

/**
 * Returns the list of applications associated with DEMOS, see Applications enumerator
  */
std::vector<std::string> DemoRegistry64::GetApplicationList()
{
  std::vector<std::string> applications;

  applications.push_back("Virtual Colonoscopy");
  applications.push_back("Cardiac");
  applications.push_back("Calcium Scoring");
  applications.push_back("Explorer");
  applications.push_back("Vascular");
  applications.push_back("Fusion");
  applications.push_back("PTX");

  return applications;
}