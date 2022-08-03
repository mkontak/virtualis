#include "Stdafx.h"
#include "LoggersSection.h"

USING_LOGGING_NS

/**
 * Gets the section specified by the name
 *
 * @param msName      Name of the section
 *
 * @return Returns the LoggersSection
 */
LoggersSection ^ LoggersSection::GetSection(String ^ msName)
{

  // ====================================================
  // Only need to extract the section if it was not done
  // =====================================================
  if( m_mpSection == nullptr )
  {
   
    m_mpSection = (LoggersSection ^)ConfigurationManager::GetSection( msName );

    if( m_mpSection == nullptr )
      throw gcnew ConfigurationErrorsException( "The <" + msName + "> section is not defined in your .config file!" );
  } // END ... If the section has not been retrieved

  return m_mpSection;

} // GetSection(String ^ msName)