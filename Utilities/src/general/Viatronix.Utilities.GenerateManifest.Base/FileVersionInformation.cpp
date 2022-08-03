// includes
#include "stdafx.h"
#include "FileVersionInformation.h"

// namespaces
using namespace System;
using namespace System::Xml;
using namespace System::Collections::Generic;

USING_GENMANIFESTBASE_NS

/**
 * Constructor.
 */
 FileVersionInformation::FileVersionInformation(String ^ mpFilename) : m_mpFilename(mpFilename)
{
} // FileVersionInformation( mpFilename )
