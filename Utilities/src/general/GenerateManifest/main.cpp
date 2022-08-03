// includes
#include "stdafx.h"
#include "Program.h"

// namespaces
using namespace System;
USING_MANIFEST_NS

/**
 * The main entry point.
 *
 * @param mpArgs The arguments to take into account.
 */
int main(cli::array<System::String ^> ^ mpArgs)
{
  Program ^ mpProgram = gcnew Program();
  mpProgram->Start(mpArgs);
} // main( mpArgs )
