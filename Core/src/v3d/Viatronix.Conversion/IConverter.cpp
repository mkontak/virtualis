#include "Stdafx.h"
#include "IConverter.h"





#pragma region usings

using namespace System;
using namespace System::Xml;
using namespace System::IO;
using namespace System::Diagnostics;
using namespace System::Threading;
using namespace System::Threading::Tasks;
using namespace System::Runtime::InteropServices;
using namespace Viatronix::Logging;

USING_CONVERSION_NS
 

#pragma endregion


/**
 * Constructor
 */
IConverter::IConverter() :
m_msLogSource("Default"),
m_msJobId(String::Empty),
m_msTaskId(String::Empty),
m_msDefaultApplication(String::Empty),
m_exitCode(0),
m_mpEventWaitHandle(gcnew EventWaitHandle(false, System::Threading::EventResetMode::ManualReset))
{



} // Find()



/**
 * Process arguements
 */
void IConverter::ProcessArguments(Dictionary<String ^, String ^> ^ mpArgs)
{

  if ( mpArgs->Count > 0 )
  {
    m_msDefaultApplication = ( mpArgs->ContainsKey("app") ? mpArgs["app"] : String::Empty );
    m_msJobId = ( mpArgs->ContainsKey("job") ? mpArgs["job"] : String::Empty );
    m_msTaskId = ( mpArgs->ContainsKey("task") ? mpArgs["task"] : String::Empty );

    
  }

} // ProcessArguments(Dictionary<String ^, String ^> ^ mpArgs)

