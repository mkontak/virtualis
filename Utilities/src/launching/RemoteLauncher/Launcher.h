#pragma once

// forward declarations
class LaunchRequest;

/**
 * Implements the launcher object used to launch the viewer remotely
 */
class Launcher
{

public:

  // Launchers the viewer specified in the request
  static void Launch( LaunchRequest request, CString & session);


};  // class Launcher