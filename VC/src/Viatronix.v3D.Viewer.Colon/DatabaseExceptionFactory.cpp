#include "StdAfx.h"
#include "DatabaseExceptionFectory.h"




/**
 * Creates a database exception based on the code
 */
void DatabaseExceptionFactory::Throw(ex::VException & ex, const std::string & sClass, const std::string & sMethod)
{

  std::string sTextMessage( ex.GetLogRecord().GetTextMessage() );

  if ( sTextMessage.find("VX") == 0 )
  {
    int code = atoi(sTextMessage.substr(2,4).c_str());
    std::string sMessage(sTextMessage.substr(8));

    switch ( code )
    {
    case 1000:
      throw ex::LicenseException(LogRec(sMessage,sClass,sMethod));

    case 9000:
      throw ex::SystemException(LogRec(sMessage,sClass,sMethod));

    case 9001:
      throw ex::SystemException(LogRec(sMessage,sClass,sMethod));

    case 9002:
      throw ex::SystemException(LogRec(sMessage,sClass,sMethod));

    case 9003:
      throw ex::SystemException(LogRec(sMessage,sClass,sMethod));

    case 9004:
      throw ex::SystemException(LogRec(sMessage,sClass,sMethod));

    case 9005:
      throw ex::SystemException(LogRec(sMessage,sClass,sMethod));

    case 9006:
      throw ex::SystemException(LogRec(sMessage,sClass,sMethod));

    case 900:
      throw ex::AuthenticationException(LogRec(sMessage,sClass,sMethod));

    case 901:
      throw ex::AccountUnknownException(LogRec(sMessage,sClass,sMethod));

    case 902:
      throw ex::SecurityException(LogRec(sMessage,sClass,sMethod));

    case 903:
      throw ex::SecurityException(LogRec(sMessage,sClass,sMethod));

    case 904:
      throw ex::SecurityException(LogRec(sMessage,sClass,sMethod));

    case 905:
      throw ex::PasswordChangeException(LogRec(sMessage,sClass,sMethod));

    case 906:
      throw ex::PasswordException(LogRec(sMessage,sClass,sMethod));

    case 907:
      throw ex::PasswordException(LogRec(sMessage,sClass,sMethod));

    case 908:
      throw ex::SecurityException(LogRec(sMessage,sClass,sMethod));

    case 909:
      throw ex::SecurityException(LogRec(sMessage,sClass,sMethod));

    case 910:
      throw ex::SecurityException(LogRec(sMessage,sClass,sMethod));

    case 911:
      throw ex::AccountDisabledException(LogRec(sMessage,sClass,sMethod));

    case 912:
      throw ex::LicenseException(LogRec(sMessage,sClass,sMethod));

    case 928:
      throw ex::LicenseExpiredException(LogRec(sMessage,sClass,sMethod));

    case 942:
      throw ex::AccountExpiredException(LogRec(sMessage,sClass,sMethod));

    default:
      throw ex::VException(LogRec(sMessage,sClass, sMethod));
    } // END switch
  }
  else
    throw ex;

}