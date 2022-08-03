#include "StdAfx.h"
#include "ContextState.h"
#include "Exception.h"

USING_VC_NS
using namespace Viatronix::Colon::Data;

bool ContextState::Acquire( const std::string & sName, const std::string & sData, std::string & sID, bool bState )
{
  return true;
  try
  {
    System::String ^ sContext = DatabaseGateway::AcquireContext( gcnew System::String( sName.c_str() ), gcnew System::String( sData.c_str() ), gcnew System::String( sID.c_str() ), bState );
    if( !System::String::IsNullOrEmpty( sContext ))
    {
      sID = ss( sContext );
      return true;
    }
    else
      return false;
  }
  catch( System::Data::DataException ^ mpE )
  {
    throw ex::IOException( LogRec( ss( mpE->Message ), "ContextState", "Acquire" ));
  }
  catch( System::Exception ^ mpE )
  {
    throw ex::RuntimeException( LogRec( ss( mpE->Message ), "ContextState", "Acquire" ));
  }
}


/**
 *  private constructor
 */
ContextState::ContextState()
{
}
