#include "Stdafx.h"
#include "LoggerElementCollection.h"


USING_LOGGING_NS

/**
 * Gets the element key
 *
 * @param mpElement    Element to get the key for
 *
 * @return Returns the key for the supplied element or null
 */
Object ^ LoggerElementCollection::GetElementKey(ConfigurationElement ^ mpElement)
{
  if (mpElement != nullptr)
    return ((LoggerElement ^)mpElement)->Name;
  else
    return nullptr;
} // GetElementKey(ConfigurationElement ^ mpElement)
